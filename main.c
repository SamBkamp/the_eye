#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

#include <openssl/ssl.h>

#include "formatting.h"
#include "connections.h"
#include "ping.h"
/*
EXAMPLE EXCHANGE (from wikipedia article {{Simple_Mail_Transfer_Protocol}}):
S: 220 smtp.example.com ESMTP Postfix
C: HELO relay.example.org
S: 250 Hello relay.example.org, I am glad to meet you
C: MAIL FROM:<bob@example.org>
S: 250 Ok
C: RCPT TO:<alice@example.com>
S: 250 Ok
C: RCPT TO:<theboss@example.com>
S: 250 Ok
C: DATA
S: 354 End data with <CR><LF>.<CR><LF>
C: From: "Bob Example" <bob@example.org>
C: To: "Alice Example" <alice@example.com>
C: Cc: theboss@example.com
C: Date: Tue, 15 Jan 2008 16:02:43 -0500
C: Subject: Test message
C:
C: Hello Alice.
C: This is a test message with 5 header fields and 4 lines in the message body.
C: Your friend,
C: Bob
C: .
S: 250 Ok: queued as 12345
C: QUIT
S: 221 Bye
 */

#define LINE_END "\r\n"

enum message_index{
  HELO,
  MAIL_FROM,
  RCPT_TO,
  DATA,
  QUIT
};


const char *message_prefixes[] = {
  "HELO ",
  "MAIL FROM:",
  "RCPT TO:",
  "DATA",
  "QUIT"
};

int send_body(config *cfg, char *data){
  int retval;

  if(!cfg->ssl)
    retval = write(cfg->fd, data, strlen(data));
  else
    retval = SSL_write(cfg->ssl, data, strlen(data));

  return retval;
}

void free_message_data(res_message *res){
  free(res->code);
  return;
}

int send_directive(config *cfg, enum message_index step, char *argv){
  //you have to send the whole message in one write(), can't split it into multiple writes like http
  char built_message[1024];
  int retval;
  strcpy(built_message, message_prefixes[step]);
  if(argv != NULL)
    strcat(built_message, argv);
  strcat(built_message, LINE_END);

  if(!cfg->ssl)
    retval = write(cfg->fd, built_message, strlen(built_message));
  else
    retval = SSL_write(cfg->ssl, built_message, strlen(built_message));
  return retval;
}


int main(int argc, char* argv[]){
  int ping_result = ping("sam-bonnekamp.com");
  char *data;
  res_message res = {0};
  enum message_index step = HELO;
  config cfg = {
    .my_domain = "bonnekamp.com",
    .from = "<test@bonnekamp.com>",
    .to = "<sam@fish>",
    .peer_domain = "fish",
    .port = "25"
  };
  char *serialized_args[] = {cfg.my_domain, cfg.from, cfg.to, NULL, NULL};

  if(ping_result < 0)
    data = format_data(&cfg, "your server is down!");
  else
    data = format_data(&cfg, "Hi Sam,\r\n your server is up :3\r\nGreetings: your server");



  cfg.fd = connect_to_service(cfg.peer_domain, cfg.port);
  if(cfg.fd < 0){
    perror("connecting to service");
    return EXIT_FAILURE;
  }

  read_and_parse(&cfg, &res);
  print_response(&res);
  free_message_data(&res);



  while(step <= QUIT){
    send_directive(&cfg, step, serialized_args[step]);

    read_and_parse(&cfg, &res);
    print_response(&res);

    if(res.code_int != 220 && res.code_int != 250 && res.code_int != 354){
      puts("something went wrong");
      printf("got code: %d\n", res.code_int);
      free_message_data(&res);
      break;
    }
    free_message_data(&res);

    //step specific work
    switch(step){
    case HELO:
      cfg.ssl = upgrade_connection(cfg.fd);
      if(!cfg.ssl)
        return EXIT_FAILURE;
      break;
    case DATA:
      send_body(&cfg, data);
      break;
    default:
      break;
    }
    step++;
  }
  return EXIT_SUCCESS;
}

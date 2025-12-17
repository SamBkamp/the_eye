#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

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

int send_body(int fd, char *data){
  if(write(fd, data, strlen(data))<0) return -1;
  return 0;

}

void free_message_data(res_message *res){
  free(res->code);
  return;
}

int send_directive(int fd, enum message_index step, char *argv){
  //you have to send the whole message in one write(), can't split it into multiple writes like http
  char built_message[1024];
  strcpy(built_message, message_prefixes[step]);
  if(argv != NULL)
    strcat(built_message, argv);
  strcat(built_message, LINE_END);

  write(fd, built_message, strlen(built_message));
  return 0;
}


int main(int argc, char* argv[]){
  int ping_result = ping();
  config cfg = {
    .my_domain = "bonnekamp.com",
    .from = "test@bonnekamp.com",
    .to = "sam@fish",
    .peer_domain = "fish",
    .port = "25"
  };
  char *data;
  res_message res = {0};
  int s_fd;
  enum message_index step = HELO;
  char *serialized_args[] = {cfg.my_domain, cfg.from, cfg.to, NULL, NULL};

  if(ping_result < 0)
    data = format_data(&cfg, "your server is down!");
  else
    data = format_data(&cfg, "server is up :3");

  s_fd = connect_to_service(cfg.peer_domain, cfg.port);
  if(s_fd < 0){
    perror("connecting to service");
    return EXIT_FAILURE;
  }

  read_and_parse(s_fd, &res);
  print_response(&res);
  free_message_data(&res);

  while(step <= QUIT){
    send_directive(s_fd, step, serialized_args[step]);
    send_directive(fileno(stdout), step, serialized_args[step]);

    read_and_parse(s_fd, &res);
    print_response(&res);
    if(res.code_int != 220 && res.code_int != 250 && res.code_int != 354){
      puts("something went wrong");
      printf("got code: %d\n", res.code_int);
      free_message_data(&res);
      break;
    }

    free_message_data(&res);
    if(step == DATA)
      send_body(s_fd, data);

    step++;
  }
  return EXIT_SUCCESS;
}

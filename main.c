#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
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
  "MAIL FROM: ",
  "RCPT TO: ",
  "DATA ",
  "QUIT"
};

typedef struct{
  char *code;
  char *content;
}res_message;

typedef struct{
  enum message_index prefix;
  char *message_content;
}req_message;

void send_directive(enum message_index step, char *argv){
  fputs(message_prefixes[step], stdout);
  if(argv != NULL)
    fputs(argv, stdout);
  fputs(LINE_END, stdout);
}

int print_response(res_message *res_msg){
  return printf("greeting: %s - %s\n", res_msg->code, res_msg->content);
}

int parse_response(res_message *res, char *raw){
  res->code = raw;
  *(raw+3) = 0;
  res->content = raw+4;
  return 0;
}

int get_sockaddr_fqdn(struct addrinfo **res, char *fqdn, char *port){
  int gai;
  struct addrinfo hints = {0};

  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  gai = getaddrinfo(fqdn, port, &hints, res);
  if(gai != 0)
    printf("gai: %s\n", gai_strerror(gai));

  return gai;
}


int connect_to_service(){
  struct addrinfo *res;
  int gai, sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd < 0){
    return -1;
  }

  gai = get_sockaddr_fqdn(&res, "localhost", "25");
  if(gai != 0){
    return -1;
  }

  if(connect(sockfd, res->ai_addr, res->ai_addrlen)<0){
    freeaddrinfo(res);
    return -1;
  }
  freeaddrinfo(res);
  return sockfd;
}

int main(int argc, char* argv[]){
  const char *port = "25";
  char *domain = "erm";
  char *from = "<sam@bonnekamp.com>";
  char *recipient = "<inbox@bonnekamp.com>";
  int s_fd;
  enum message_index step = HELO;
  char *serialized_args[] = {domain, from, recipient, NULL, NULL};


  s_fd = connect_to_service();
  if(s_fd < 0){
    perror("connecting to service");
    return EXIT_FAILURE;
  }
  while(step <= QUIT){
    send_directive(step, serialized_args[step]);
    step++;
  }


  return EXIT_SUCCESS;
}

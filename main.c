#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
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


int main(int argc, char* argv[]){
  const char *port = "25";
  const short port_short = 25;
  struct sockaddr_in peer_addr;
  struct addrinfo hints = {0}, *res;
  socklen_t peer_addr_len = sizeof(peer_addr);
  int gai, s_fd = socket(AF_INET, SOCK_STREAM, 0);

  if(s_fd < 0){
    perror("socket");
    return EXIT_FAILURE;
  }

  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  peer_addr.sin_family = AF_INET;
  peer_addr.sin_port = htons(port_short);
  peer_addr.sin_addr = (struct in_addr){htonl(2130706433)}; //127.0.0.1 as a long

  /*
  gai = getaddrinfo("localhost", port, &hints, &res);
  if(gai != 0){
    printf("gai: %s\n", gai_strerror(gai));
    return -1;
  }

  struct sockaddr_in *res_saddr = (struct sockaddr_in *)res->ai_addr;
  printf("my addr is: %ld - gai addr is: %ld\n", ntohl(peer_addr.sin_addr.s_addr), ntohl(res_saddr->sin_addr.s_addr));
  */

  if(connect(s_fd, (struct sockaddr *)&peer_addr, peer_addr_len)<0){
    perror("connect");
    freeaddrinfo(res);
    return EXIT_FAILURE;
  }
  char data[1024];
  int bytes_read = read(s_fd, data, 1023);
  if(bytes_read < 0){
    perror("read");
    return EXIT_FAILURE;
  }
  data[bytes_read] = 0;
  puts(data);
  char *messages[] = {
    "HELO example.com\r\n",
    "MAIL FROM:<program@example.com>\r\n",
    "RCPT TO:<sam@fish>\r\n",
    "DATA\r\n",
    "From: \"your program\" <program@example.com>\r\n",
    "To: \"Sam Fish\" <sam@fish>\r\n",
    "Date: Sat, 13 Dec 2025 14:00 +0800\r\n",
    "Subject: Your program is speaking!\r\n",
    "\r\n",
    "Hello!\r\n",
    "bye\r\n",
    ".\r\n",
    "QUIT\r\n",
    NULL
  };
  for(int i = 0; i < 4; i++){
    write(s_fd, messages[i], strlen(messages[i]));
    data[read(s_fd, data, 1023)] = 0;
    puts(data);
  }

  for(int i = 0; i < 8; i++){
    write(s_fd, messages[i+4], strlen(messages[i+4]));
  }

  write(s_fd, messages[12], strlen(messages[12]));
  data[read(s_fd, data, 1023)] = 0;
  puts(data);
}

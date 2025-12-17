#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>


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

int connect_to_service(char *domain, char *port){
  struct addrinfo *res;
  int gai, sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd < 0)
    return -1;

  gai = get_sockaddr_fqdn(&res, domain, port);
  if(gai != 0)
    return -1;

  if(connect(sockfd, res->ai_addr, res->ai_addrlen)<0){
    freeaddrinfo(res);
    return -1;
  }
  freeaddrinfo(res);
  return sockfd;
}

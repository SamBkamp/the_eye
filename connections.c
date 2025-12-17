#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

#include <openssl/ssl.h>
#include <openssl/err.h>

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

SSL *upgrade_connection(int fd){
  char upgrade[] = "STARTTLS\r\n";
  char return_data[1024];
  int r;
  SSL_CTX *ctx;
  SSL *ssl;

  //send upgrade request message
  write(fd, upgrade, sizeof(upgrade));
  r = read(fd, return_data, 1023);
  return_data[r] = 0;
  puts(return_data);

  SSL_load_error_strings();
  ctx = SSL_CTX_new(TLS_client_method());
  if(!ctx){
    puts("could not create SSL context");
    return NULL;
  }

  ssl = SSL_new(ctx);
  if(!ssl){
    puts("could not create SSL struct");
    return NULL;
  }

  SSL_set_fd(ssl, fd);
  if(SSL_connect(ssl) < 0){
    puts("could not connect with SSL");
    printf("%s\n", ERR_error_string(ERR_get_error(), NULL));
    return NULL;
  }
  return ssl;
}

#ifndef EYE_CONNECTIONS
#define EYE_CONNECTIONS
#include <netdb.h>
#include <openssl/ssl.h>

int get_sockaddr_fqdn(struct addrinfo **res, char *fqdn, char *port);
int connect_to_service(char *domain, char *port);
SSL *upgrade_connection(int fd);

#endif

#ifndef EYE_CONNECTIONS
#define EYE_CONNECTIONS
#include <netdb.h>

int get_sockaddr_fqdn(struct addrinfo **res, char *fqdn, char *port);
int connect_to_service(char *domain, char *port);

#endif

#ifndef EYE_FORMATTING
#define EYE_FORMATTING

#include <openssl/ssl.h>

typedef struct{
  uint16_t code_int;
  char *code;
  char *content;
}res_message;

typedef struct{
  char *my_domain;
  char *from;
  char *to;
  char *peer_domain;
  char *port;
  SSL *ssl;
  int fd;
}config;

int read_and_parse(config *cfg, res_message *res);
int print_response(res_message *res_msg);
int parse_response(res_message *res, char *raw);
char *format_data(config *cfg, char *body);
#endif

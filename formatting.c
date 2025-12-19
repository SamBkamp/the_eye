#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "formatting.h"

int format_time(char *buf, size_t buf_size){
  time_t curr_time = time(NULL);
  struct tm *time_struct = localtime(&curr_time);
  strftime(buf, buf_size, "%a, %d %b %Y %T %z", time_struct);
  return 0;
}

char *format_data(config *cfg, char *body){
  char format[] = "From: \"%s\" <%s>\r\nTo: \"%s\" <%s>\r\nDate: %s\r\nSubject:%s\r\n\r\n%s\r\n.\r\n";
  size_t format_len = sizeof(format);
  char *data = malloc(format_len+512);
  char time_buffer[100];

  format_time(time_buffer, sizeof(time_buffer));
  snprintf(data, format_len+512, format, cfg->from, cfg->from, cfg->to, cfg->to, time_buffer, "erm", body);
  return data;
}

int read_and_parse(config *cfg, res_message *res){
  char *response_data = malloc(1024);
  int bytes_read;

  if(!cfg->ssl)
    bytes_read = read(cfg->fd, response_data, 1023);
  else
    bytes_read = SSL_read(cfg->ssl, response_data, 1023);

  response_data[bytes_read] = 0;
  if(bytes_read >= 0)
    parse_response(res, response_data);

  return bytes_read;
}


int print_response(res_message *res_msg){
  return printf("%s  %s", res_msg->code, res_msg->content);
}

int parse_response(res_message *res, char *raw){
  res->code = raw;
  *(raw+3) = 0;
  res->content = raw+4;
  res->code_int = atoi(res->code);
  return 0;
}

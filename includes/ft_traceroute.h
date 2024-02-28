#ifndef FT_TRACEROUTE_H
#define FT_TRACEROUTE_H

#include "../libft/include/libft.h"
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

typedef struct s_trace {
  char *hostname;
  char host[1046];
  char *packet;
  char ip[100];

  int alloc;
  int packetSize;
  int seqRecv;
  int sockfd;
  int ttl;
  int verose;
  int signalStop;
  uint16_t seq;

  struct icmp *icmp_header;
  struct sockaddr_in dest_addr;
  struct timeval tv;

  void (*help)(char *);
  void (*free)();
  int (*getname)();
  void (*getinfo)(char *ip);
  void (*header)();
  int8_t (*connection)();
  void (*run)();
  int8_t (*send)();
  int8_t (*recv)();
  int8_t (*set_ttl)();
} t_trace;

int host_to_ip();
int8_t ft_receive();
int8_t ft_send();
int8_t openSocket();
int8_t set_ttl();
t_trace *ft_trace(t_trace *t);
t_trace *initTrace(int ac, char **av);
unsigned short calculate_checksum(void *addr, size_t count);
void fill_seq_icmp();
void free_struct();
void ft_traceroute();
void handle_signal();
void help(char *s);
void get_name_ip(char *ip);

#endif

#ifndef FT_TRACEROUTE_H
#define FT_TRACEROUTE_H

#include "../libft/include/libft.h"
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/udp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#define QUERY 3
#define MAX_HOPS 255
#define DEFAULT_HOPS 30
#define PACKET_SIZE 60
#define TIMEOUT_SEC 1
#define EXIT_RECV -1
#define EXIT_RECV_DONE 2
#define MAXQUERY 10
#define DEST_PORT 33434
#define MAXPORT 65535

typedef struct s_query {
  int ttl;
  char *host;
  int count;
  int queryMax;
} t_query;

typedef struct s_check {
  int8_t ok;
  int value;
} t_check;

typedef struct s_flag {
  t_check verbose;
} t_flag;

typedef struct s_trace {
  char *hostname;
  char host[1046];
  char *packet;
  char ip[100];

  int sock;
  int alloc;
  int packetSize;
  int seqRecv;
  int sockfd;
  int sockfd_udp;
  int ttl;
  int hopmax;
  int verose;
  int signalStop;
  int proto;
  int query;
  int port;
  uint16_t seq;

  struct icmp *icmp_header;
  struct sockaddr_in dest_addr;
  struct sockaddr_in dest_addr_udp;
  struct timeval tv;

  void (*help)(char *);
  void (*free)();
  int (*getname)();
  void (*getinfo)(char *ip);
  void (*header)();
  int8_t (*connection)();
  void (*run)();
  int8_t (*send)();
  int8_t (*recv)(t_query *q);
  int8_t (*set_ttl)();
  int (*parse)(int ac, char **av);
  t_flag flag;
} t_trace;

int host_to_ip();
int8_t ft_receive(t_query *q);
int8_t ft_send();
int8_t openSocket();
int8_t set_ttl();
t_trace *ft_trace(t_trace *t);
t_trace *initTrace();
unsigned short calculate_checksum(void *addr, size_t count);
void fill_seq_icmp();
void free_struct();
void ft_traceroute();
void handle_signal();
void help(char *s);
void get_name_ip(char *ip);
int parse(int ac, char **av);
void SetPortUdp(t_trace *trace, int port);
#endif

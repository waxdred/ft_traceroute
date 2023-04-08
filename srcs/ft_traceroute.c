#include "../includes/ft_traceroute.h"
#include <netdb.h>

int signalStop;

static void handle_signal() { signalStop = 0; }
static void help(char *s) {
  fprintf(stderr, "Usage: %s, ip_adress\n", s);
  return;
}

static int host_to_ip(t_trace *trace) {
  char host[100];
  struct addrinfo hint;
  struct addrinfo *servinfo, *tmp;
  bzero(&hint, sizeof(hint));
  hint.ai_family = AF_INET;

  int recv = getaddrinfo(trace->hostname, NULL, &hint, &servinfo);
  if (recv < 0) {
    perror("ft_trace: error getaddrinfo");
    return EXIT_FAILURE;
  } else if (servinfo == NULL) {
    fprintf(stderr, "ft_trace: cannot resolve: %s: Unknown host\n",
            trace->hostname);
    return EXIT_FAILURE;
  }
  for (tmp = servinfo; tmp != NULL; tmp = tmp->ai_next) {
    getnameinfo(tmp->ai_addr, tmp->ai_addrlen, host, sizeof(host), NULL, 0,
                NI_NUMERICHOST);
    strcpy(trace->ip, host);
  }
  freeaddrinfo(servinfo);
  return EXIT_SUCCESS;
}

static unsigned short calculate_checksum(void *addr, size_t count) {
  unsigned short *ptr;
  unsigned long sum;

  ptr = addr;
  for (sum = 0; count > 1; count -= 2)
    sum += *ptr++;
  if (count > 0)
    sum += *(unsigned char *)ptr;
  while (sum >> 16)
    sum = (sum & 0xffff) + (sum >> 16);
  return (~sum);
}

static void fill_seq_icmp(t_trace *trace) {
  trace->icmp_header->icmp_type = ICMP_ECHO;
  trace->icmp_header->icmp_code = 0;
  trace->icmp_header->icmp_id = getpid() & 0xFFFF;
  trace->icmp_header->icmp_seq = htons(trace->seq);
  memset(trace->icmp_header->icmp_data, '*', trace->packetSize);
  trace->icmp_header->icmp_cksum = 0;
  trace->icmp_header->icmp_cksum = calculate_checksum(
      (unsigned short *)trace->icmp_header, trace->packetSize);
}

static int8_t openSocket(t_trace *trace) {
  struct timeval timeout = {1, 0};
  socklen_t len = sizeof(timeout);
  trace->sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
  if (trace->sockfd == -1) {
    fprintf(stderr, "ft_trace: error creation socket: %s\n", strerror(errno));
    return EXIT_FAILURE;
  }
  trace->packetSize = sizeof(struct icmp) + sizeof(struct timeval);
  trace->packet = (char *)malloc(trace->packetSize);
  if (!trace->packet) {
    fprintf(stderr, "ft_trace: error allocation: %s\n", strerror(errno));
    return 2;
  }
  memset(trace->packet, 0, sizeof(struct icmp));
  trace->alloc = 1;
  if (trace->getname(trace)) {
    return EXIT_FAILURE;
  }
  // set time out for receive
  if (setsockopt(trace->sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, len) < 0) {
    fprintf(stderr, "ft_trace: error set setsockopt timeout: %s\n",
            strerror(errno));
    return EXIT_FAILURE;
  }
  trace->dest_addr.sin_family = AF_INET;
  trace->dest_addr.sin_addr.s_addr = inet_addr(trace->ip);
  trace->icmp_header = (struct icmp *)trace->packet;
  trace->icmp_header->icmp_id = getpid() & 0xFFFF;
  trace->header(trace);
  return EXIT_SUCCESS;
}

static int8_t ft_send(t_trace *trace) {
  trace->header(trace);
  if (sendto(trace->sockfd, trace->packet, trace->packetSize, 0,
             (struct sockaddr *)&trace->dest_addr,
             sizeof(trace->dest_addr)) < 0) {
    fprintf(stderr, "ft_trace: error sending packet: %s\n", strerror(errno));
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

static int8_t ft_receive(t_trace *trace) {
  int8_t ret = 0;
  char buf[108];
  struct timeval end;
  struct sockaddr_in from;
  socklen_t fromlen = sizeof(from);
  char srcIP[INET_ADDRSTRLEN], dstIP[INET_ADDRSTRLEN];

  bzero(buf, 108);
  ret = recvfrom(trace->sockfd, buf, sizeof(buf), 0, (struct sockaddr *)&from,
                 &fromlen);
  if (ret < 0) {
    printf("%d * * *\n", trace->ttl);
    trace->ttl++;
    return EXIT_SUCCESS;
  }
  gettimeofday(&end, NULL);
  struct ip *ip = (struct ip *)buf;
  double data = (double)(end.tv_usec - trace->tv.tv_usec) / 1000;

  inet_ntop(AF_INET, &(ip->ip_src), srcIP, INET_ADDRSTRLEN);
  inet_ntop(AF_INET, &(ip->ip_dst), dstIP, INET_ADDRSTRLEN);
  trace->getinfo(trace, srcIP);
  printf("%d %s (%s) %.3lf ms\n", trace->ttl, trace->host, srcIP, data);
  if (strcmp(trace->ip, srcIP) == 0) {
    trace->ttl = 100;
    EXIT_FAILURE;
  }
  trace->seq++;
  trace->seqRecv++;
  trace->ttl++;
  return EXIT_SUCCESS;
}

static void get_name_ip(t_trace *trace, char *ip) {
  struct sockaddr_in sa;

  memset(&sa, 0, sizeof(sa));
  memset(&trace->host, 0, sizeof(trace->host));
  sa.sin_family = AF_INET;
  sa.sin_addr.s_addr = inet_addr(ip);
  if (getnameinfo((struct sockaddr *)&sa, sizeof(sa), trace->host,
                  sizeof(trace->host), NULL, 0, 0)) {
    fprintf(stderr, "Could not resolve hostname\n");
    exit(EXIT_FAILURE);
  }
}

static void free_struct(t_trace *trace) {
  if (trace->alloc == 1)
    free(trace->packet);
  close(trace->sockfd);
  free(trace);
  return;
}

static int8_t set_ttl(t_trace *trace) {
  // set ttl
  if (setsockopt(trace->sockfd, IPPROTO_IP, IP_TTL, &trace->ttl,
                 sizeof(trace->ttl)) == -1) {
    fprintf(stderr, "ft_trace: error set setsockopt ttl: %s\n",
            strerror(errno));
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

static void ft_traceroute(t_trace *trace) {
  signalStop = 1;
  signal(SIGINT, handle_signal);
  for (; signalStop && trace->ttl < 64;) {
    gettimeofday(&trace->tv, NULL);
    if (trace->set_ttl(trace))
      break;
    if (trace->send(trace))
      break;
    if (trace->recv(trace)) {
      break;
    }
  }
}

t_trace *initTrace(int ac, char **av) {
  t_trace *trace = malloc(sizeof(t_trace));

  if (!trace)
    return NULL;
  memset(trace, 0, sizeof(t_trace));
  if (ac == 2)
    trace->hostname = av[1];
  trace->help = &help;
  trace->free = &free_struct;
  trace->getname = &host_to_ip;
  trace->header = &fill_seq_icmp;
  trace->connection = &openSocket;
  trace->send = &ft_send;
  trace->recv = &ft_receive;
  trace->run = &ft_traceroute;
  trace->set_ttl = &set_ttl;
  trace->getinfo = &get_name_ip;
  trace->ttl = 1;
  return trace;
}

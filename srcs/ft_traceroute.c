#include "../includes/ft_traceroute.h"

int8_t ft_send() {
  t_trace *trace = ft_trace(NULL);
  trace->header();
  if (sendto(trace->sockfd, trace->packet, trace->packetSize, 0,
             (struct sockaddr *)&trace->dest_addr,
             sizeof(trace->dest_addr)) < 0) {
    fprintf(stderr, "ft_trace: error sending packet: %s\n", strerror(errno));
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int8_t ft_receive() {
  t_trace *trace = ft_trace(NULL);
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
  trace->getinfo(srcIP);
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

void ft_traceroute() {
  t_trace *trace = ft_trace(NULL);
  trace->signalStop = 1;
  signal(SIGINT, handle_signal);
  for (; trace->signalStop && trace->ttl < 64;) {
    gettimeofday(&trace->tv, NULL);
    if (trace->set_ttl())
      break;
    if (trace->send())
      break;
    if (trace->recv()) {
      break;
    }
  }
}

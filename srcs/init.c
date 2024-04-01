
#include "../includes/ft_traceroute.h"
#include <netinet/in.h>

t_trace *initTrace() {
  t_trace *trace = malloc(sizeof(t_trace));

  if (!trace)
    return NULL;
  ft_memset(trace, 0, sizeof(t_trace));
  ft_trace(trace);
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
  trace->parse = &parse;
  trace->ttl = 1;
  trace->proto = IPPROTO_UDP;
  trace->hopmax = DEFAULT_HOPS;
  trace->query = QUERY;
  trace->port = DEST_PORT;
  return trace;
}

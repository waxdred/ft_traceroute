
#include "../includes/ft_traceroute.h"

void free_struct() {
  t_trace *trace = ft_trace(NULL);
  if (trace->alloc == 1)
    free(trace->packet);
  close(trace->sockfd);
  free(trace);
  return;
}

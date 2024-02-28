#include "../includes/ft_traceroute.h"

void handle_signal() {
  t_trace *trace = ft_trace(NULL);
  trace->signalStop = 0;
}

t_trace *ft_trace(t_trace *t) {
  static t_trace *trace;
  if (t != NULL) {
    trace = t;
  }
  return trace;
}

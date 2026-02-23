#include "types.h"
#include "stat.h"
#include "user.h"

#define MAX_CHILDREN 8

struct sample {
  int pid;
  int tickets;
  uint work;
};

static int
parse_positive(char *s)
{
  int v = atoi(s);
  if(v < 1)
    return -1;
  return v;
}

int
main(int argc, char **argv)
{
  int duration = 500;
  int children = 3;
  int tickets[MAX_CHILDREN] = {10, 20, 40, 0, 0, 0, 0, 0};
  int p[2];
  struct sample results[MAX_CHILDREN];
  int i;
  int start;
  int end;
  int total_tickets = 0;
  uint total_work = 0;

  if(argc >= 2){
    duration = parse_positive(argv[1]);
    if(duration < 1){
      printf(1, "usage: lotteryshare [duration_ticks] [ticket1 ticket2 ...]\n");
      exit();
    }
  }

  if(argc >= 3){
    children = argc - 2;
    if(children > MAX_CHILDREN){
      printf(1, "lotteryshare: max %d children\n", MAX_CHILDREN);
      exit();
    }
    for(i = 0; i < children; i++){
      tickets[i] = parse_positive(argv[i + 2]);
      if(tickets[i] < 1){
        printf(1, "lotteryshare: tickets must be >= 1\n");
        exit();
      }
    }
  }

  for(i = 0; i < children; i++)
    total_tickets += tickets[i];

  if(pipe(p) < 0){
    printf(1, "lotteryshare: pipe failed\n");
    exit();
  }

  start = uptime() + 20;
  end = start + duration;

  printf(1, "lotteryshare: duration=%d ticks, children=%d\n", duration, children);
  printf(1, "lotteryshare: tickets:");
  for(i = 0; i < children; i++)
    printf(1, " %d", tickets[i]);
  printf(1, "\n");

  for(i = 0; i < children; i++){
    int pid = fork();
    if(pid < 0){
      printf(1, "lotteryshare: fork failed\n");
      exit();
    }

    if(pid == 0){
      struct sample s;
      int j;

      close(p[0]);
      if(settickets(tickets[i]) < 0){
        printf(1, "lotteryshare: child settickets failed\n");
        exit();
      }

      while(uptime() < start)
        ;

      s.work = 0;
      while(uptime() < end){
        for(j = 0; j < 1000; j++)
          s.work++;
      }

      s.pid = getpid();
      s.tickets = tickets[i];
      write(p[1], &s, sizeof(s));
      close(p[1]);
      exit();
    }
  }

  close(p[1]);
  for(i = 0; i < children; i++){
    struct sample s;
    int got = read(p[0], &s, sizeof(s));
    if(got != sizeof(s)){
      printf(1, "lotteryshare: short read (%d)\n", got);
      continue;
    }

    total_work += s.work;
    results[i] = s;
    printf(1, "child pid=%d tickets=%d work=%d\n", s.pid, s.tickets, s.work);
  }
  close(p[0]);

  while(wait() >= 0)
    ;

  if(total_work == 0){
    printf(1, "lotteryshare: no work recorded\n");
    exit();
  }

  printf(1, "expected vs observed (0.1%% units)\n");
  printf(1, "total tickets=%d total work=%d\n", total_tickets, total_work);

  for(i = 0; i < children; i++){
    int expected10 = (results[i].tickets * 1000) / total_tickets;
    int observed10;
    if(total_work >= 1000)
      observed10 = results[i].work / (total_work / 1000);
    else
      observed10 = (results[i].work * 1000) / total_work;
    printf(1, "pid=%d tickets=%d expected=%d.%d%% observed=%d.%d%%\n",
           results[i].pid,
           results[i].tickets,
           expected10 / 10,
           expected10 % 10,
           observed10 / 10,
           observed10 % 10);
  }

  exit();
}

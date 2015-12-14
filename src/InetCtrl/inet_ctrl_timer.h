#ifndef INET_CTRL_TIMER_H
#define INET_CTRL_TIMER_H

int inetctrl_init_timer(void);
int inetctrl_remove_timer(void);

int start_writefile_thread(void);
int stop_writefile_thread(void);

void execute_slow_timeout(void);

#endif


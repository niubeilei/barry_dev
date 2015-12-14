#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/wait.h>

#define MAXTIME	86400 /* 24 hours */
#define NOCOLS	60

void catch_signal(int sig);
static uint8_t stop_the_dots = 0;

int
main(int ac, char *av[])
{
	int	i, pid;
	
	if (ac < 1)
		exit(1);
	
	signal(SIGUSR1, catch_signal);
	
	pid = fork();
	if (!pid) {
		execv(av[1], av+1);
		exit(-1);
	}
	
	for (i = 0; i < MAXTIME; i++) {
		if (waitpid(pid, NULL, WNOHANG) == pid) {
			printf("\n");
			break;
		}
		
		if (kill(pid, 0) == -1) {
			printf("\n");
			break;
		}

		sleep(1);
		if (!stop_the_dots) {
			printf(".");
			fflush(stdout);
			if (!((i+1)%NOCOLS))
				printf("\n");
		}
	}
	exit(0);
}

void 
catch_signal(int sig)
{
	if (sig == SIGUSR1) {
		if (stop_the_dots) {
			stop_the_dots = 0;
		} else {
			stop_the_dots = 1;
		}
	}
}

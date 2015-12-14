#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, const char **argv)
{
	int sleep_time;

	sleep_time = 10;

	if (2 <= argc) {
		sleep_time = atoi(argv[1]);
	}

	if (sleep_time <= 0)
		sleep_time = 10;
  
	daemon(0, 0);
	int fd=open("/dev/watchdog", O_WRONLY);
	if (fd < 0) {
		perror("watchdog");
		return -1;
	}       

	while(1) {
		write(fd, "\0", 1);
		fsync(fd);
		sleep(sleep_time);
	}
	return 1;
}



#include <sys/types.h>
#include <dirent.h>
#include <sys/time.h> //for HZ

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include <sys/param.h>
/*
#include <sys/sysinfo.h>
#include <sys/sysctl.h>
#include <sys/ultrasound.h>
#include <sys/prctl.h>
#include <sys/kd.h>
#include <sys/soundcard.h>
#include <sys/vt.h>
#include <sys/pci.h>
*/


struct param
{
	int debug;
	int interval;
	char cpu_file[256];
	char process_cpu_file[256];
}
param;

#define PROCESS_TOTAL 50

struct process_cpu_snapshot
{
	struct process
	{
		char name[256];
		double time;
	}
	process[PROCESS_TOTAL];
	int end;
};



static void usage(void)
{
	puts("Usage: cpumgr [OPTION]...");
	puts("  -h\tHelp");
	puts("  -d\tDebug mode");
	exit(0);
}

static void parse_arg(int argc, char **argv)
{
	int             ret;

	while(1)
	{
		ret = getopt(argc, argv, "hd");
		if(ret == -1)
			break;
		switch (ret)
		{
			case 'h':
				usage();
				break;
			case 'd':
				param.debug = 1;
				break;
		}
	}
}

struct cpu_snapshot
{
	unsigned long cpu_user;
	unsigned long cpu_nice;
	unsigned long cpu_system;
	unsigned long cpu_idle;
	unsigned long cpu_iowait;
	unsigned long cpu_irq;
	unsigned long cpu_sirq;
};

#define PROC_STAT_FILE "/proc/stat"

static void get_cpu_snapshot( struct cpu_snapshot * snap )
{

	FILE *fp;

	memset( snap, 0, sizeof(*snap) );
	fp = fopen(PROC_STAT_FILE, "r");
	if(fp)
	{
		fscanf(fp, "%*s %lu %lu %lu %lu %lu %lu %lu", 
				&snap->cpu_user, &snap->cpu_nice, &snap->cpu_system, &snap->cpu_idle, 
				&snap->cpu_iowait, &snap->cpu_irq, &snap->cpu_sirq);
		fclose(fp);
	}

}

static void save_cpu_snapshot( time_t current_time, struct cpu_snapshot * snap)
{

	FILE * fp;
	fp = fopen( param.cpu_file, "a" );
	if(!fp)
		return;
	fprintf(fp,"%lu %lu %lu %lu %lu %lu %lu %lu\n", current_time,
			snap->cpu_user, snap->cpu_nice, snap->cpu_system, snap->cpu_idle, 
			snap->cpu_iowait, snap->cpu_irq, snap->cpu_sirq);
	fclose(fp);
}

static int ptable_find_pos( struct process_cpu_snapshot *ps, double time )
{
	int i;

	for(i=0;i<ps->end;i++)
	{
		if(ps->process[i].time<time)
			break;
	}
	return i;
}
static void ptable_insert_at( struct process_cpu_snapshot *ps, int pos, char *pname, double time )
{
	int i;
	for(i=ps->end; i>pos; i--)
	{
		if(i<PROCESS_TOTAL)
		{
			ps->process[i].time = ps->process[i-1].time;
			strcpy( ps->process[i].name, ps->process[i-1].name );
		}
	}
	ps->process[pos].time = time;
	strcpy(ps->process[pos].name, pname);
	if(ps->end<PROCESS_TOTAL)
		ps->end++;

}
static int ptable_exist( struct process_cpu_snapshot *ps, char * name )
{
	int i;
	for(i=0;i<ps->end;i++)
	{
		if(strcmp(ps->process[i].name, name)==0)
			return i;
	}
	return -1;
}
static void ptable_insert( struct process_cpu_snapshot *ps, char *pname, double pcpu )
{
	int pos;

	pos = ptable_exist( ps, pname );
	if (pos>=0)
	{
		ps->process[pos].time += pcpu;
		while(pos>0 && ps->process[pos].time>ps->process[pos-1].time)
		{
			char tmp[256];
			double t;

			t = ps->process[pos].time;
			strcpy(tmp,ps->process[pos].name);
			ps->process[pos].time = ps->process[pos-1].time;
			strcpy(ps->process[pos].name,ps->process[pos-1].name);
			ps->process[pos-1].time = t;
			strcpy(ps->process[pos-1].name,tmp);
			pos--;
		}
	}
	else
	{
		pos = ptable_find_pos( ps, pcpu);
		if(pos>=0 && pos <PROCESS_TOTAL)
			ptable_insert_at( ps, pos, pname, pcpu);
	}
}
static double get_seconds_since_boot( void )
{
	double uptime=0;
	FILE * fp = fopen( "/proc/uptime", "r" );
	if(fp)
	{
		fscanf(fp, "%lf %*s", &uptime);
		fclose(fp);
	}
	return uptime;
}
static void get_process_cpu_snapshot( struct process_cpu_snapshot *ps )
{

	DIR *dir;
	struct dirent *drp;

	memset(ps,0,sizeof(*ps));
	if ((dir = opendir("/proc")) == NULL)
	{
		return ;
	}

	while ( (drp = readdir(dir)) ) 
	{
		if (drp->d_name[0] < '0' || drp->d_name[0] > '9')
		{
			continue;
		}

		char tmp[256];
		unsigned long long utime,stime;
		double seconds_since_boot=0;
		unsigned long long start_time=0;
		double rate;
		sprintf( tmp, "%s%s%s", "/proc/", drp->d_name, "/stat");
		FILE * fp = fopen( tmp, "r" );
		if(!fp)
			continue;
		fscanf(fp, "%*d %s %*s %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %Lu %Lu %*u %*u %*d %*d %*u %*u %Lu %*u %*u %*u %*u %*u %*u %*u %*u %*u %*u %*u %*u %*u %*u %*u %*u %*u\n", tmp, &utime, &stime, &start_time);
		fclose(fp);
		seconds_since_boot = get_seconds_since_boot();
		rate = (utime+stime) / (seconds_since_boot*HZ - start_time); 
		printf("%f\n",rate);
		ptable_insert( ps, tmp, rate );
	}
	closedir(dir);

}
static void save_process_cpu_snapshot( time_t current_time, struct process_cpu_snapshot * snap)
{

	FILE * fp;
	int i;

	fp = fopen( param.process_cpu_file, "a" );
	if(!fp)
		return;
	fprintf(fp,"%lu ", current_time);
	for(i=0;i<PROCESS_TOTAL;i++)
	{
		fprintf(fp,"%s %lf ", snap->process[i].name, snap->process[i].time);
	}
	fprintf(fp,"\n");
	fclose(fp);
}


static void do_cpu( void )
{
	struct cpu_snapshot cs;
	struct process_cpu_snapshot ps;
	time_t current_time;

	get_cpu_snapshot(&cs);
	get_process_cpu_snapshot(&ps);
	current_time=time(0);
	save_cpu_snapshot(current_time, &cs);
	save_process_cpu_snapshot(current_time, &ps);
}

static void loop( void )
{
	while(1)
	{
		do_cpu();
		sleep( param.interval );
	}
}

static void init_param( void )
{
	param.debug=0;
	param.interval=6;

	strcpy(param.cpu_file, "/tmp/cpuinfo");
	strcpy(param.process_cpu_file, "/tmp/processcpuinfo");
}
int main(int argc, char **argv)
{

	init_param();
	parse_arg(argc, argv);
	//	print_arg();

	if(!param.debug)
	{
		daemon(0, 0);
	}

	loop();

	return 0;
}

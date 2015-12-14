#include "JimoAgentUtil/SearchIp.h"
#include "JimoAgentUtil/IpAddress.h"
//#include <algorithm>

set<string> online_ips;
set<string>::iterator itr;
int start_ip = 1;
int end_ip = 254;
int time_connect = 5;
int port = 22;
int nthreads = 32;

void usage() 
{
	printf("use format:\n\t-s startip\n\t-e endip\n\t-t connect_timeout\n\t-p port\n");
	exit(-1);
}

void testIp()
{
	if (AosIpAddress::isValidSubNetMask("255.255.255.0"))
	{
		printf("111111111111 true\n");
	}
	else
	{
		printf("111111111111 false\n");
	}
	if (AosIpAddress::isValidIpV4Address("192.168.99.0"))
	{
		printf("222222222222 true\n");
	}
	else
	{
		printf("222222222222 false\n");
	}
}


int main(int argc, char** argv) 
{
	int opt;
	while((opt = getopt(argc, argv, "s:e:t:p:n:")) != -1) 
	{
		switch(opt) 
		{
			case 's':start_ip = atoi(optarg);break;
			case 'e':end_ip   = atoi(optarg);break;
			case 't':time_connect = atoi(optarg);break;
			case 'p':port = atoi(optarg);break;
			case 'n':nthreads = atoi(optarg);break;
			case '?':usage();
		}
	}
	if(nthreads <= 1)
	{
		nthreads = 1;
	}
	int toscan = end_ip - start_ip + 1;
	if(nthreads > toscan)nthreads = toscan;
	
	printf("-----------------------------------------------------------------------------------------------------\n");
	printf("\t scan will use[%d]threads 192.168.99.%d -> 192.168.99.%d with connect timeout %d at port %d \n", nthreads, start_ip, end_ip, time_connect, port);
	printf("-----------------------------------------------------------------------------------------------------\n");
	AosSearchIp search(nthreads, port, time_connect); 
	while(true)
	{
		search.start(start_ip, end_ip);
		set<string> online_ips = search.getOnlineList();
		//std::sort(online_ips.begin(), online_ips.end());
		char buff[100];
		sprintf(buff, "%d", (unsigned int)online_ips.size());
		string size = buff;
		string str;
		str += "+---------------+-------+\n";
		str += "|ip(" + size + ")\t\t|status\t|\n";
		str += "+---------------+-------+\n";
		itr = online_ips.begin();
		while(itr != online_ips.end())
		{
			str += "|" + *itr + "\t|online\t|\n";
			str += "+---------------+-------+\n";
			itr++;
		}
		str += "|total(" + size + ")\t\t|\n";
		str += "+---------------+-------+\n";
		printf("%s", str.data());
		sleep(10);
	}
	return 0;
}

#include "UtilComm/TcpClient.h"
#include "Porting/Sleep.h"
#include "JimoAgentClient/JimoAgentSCP.h"
#include <pwd.h> 

int gAosLogLevel = 1;
int gAosShowNetTraffic = 1;

bool is_file = true;
bool is_tolocal = true;
int port = -1;
int def_port = 2200;
OmnString host = "";
OmnString user = "";
OmnString remote = "";
OmnString local = "";

//format:
//	[jscp] -p [port] [user]@[host]:[remote file] [local file]
//	[jscp] -p [port] [local file] [user]@[host]:[remote dir]
//	[jscp] -r -p [port] [user]@[host]:[remote dir] [local dir]
//	[jscp] -r -p [port] [local dir] [user]@[host]:[remote dir]

void
jscp()
{
	OmnTcpClientPtr client = OmnNew OmnTcpClient(
			"agent_client", 
			host,
			port, 
			1,
			eAosTLT_FirstFourHigh);
	OmnString errmsg;
	int trys = 10;
	while(trys-- > 0)
	{
    	if (!client->connect(errmsg))
		{
			OmnSleep(1);
			continue;
		}
		break;
	}
	if (trys < 0)
	{
		exit(-1);
	}
	AosJimoAgentSCP* jscp = OmnNew AosJimoAgentSCP(
			client, user, is_file, is_tolocal, remote, local);
	jscp->start();
}

void test()
{
	OmnString speed = "10M/S";
	OmnString percent = "100%";
	OmnString length = "30M";
	OmnString fname = "2014-11-11.src.tar.gz";
	printf("%s%100s\t%s\t%s\n", fname.data(), percent.data(), length.data(), speed.data());
}

int main(int argc,char **argv)
{
	if (argc < 2)
	{
		OmnString str;
		str << "[jscp] -p [port] [user]@[host]:[remote file] [local file]\n"
			<< "[jscp] -p [port] [local file] [user]@[host]:[remote dir]\n"
			<< "[jscp] -r -p [port] [user]@[host]:[remote dir] [local dir]\n"
			<< "[jscp] -r -p [port] [local dir] [user]@[host]:[remote dir]\n";
		printf("%s", str.data());
		exit(-1);
	}

	int i = 1;
	if (strcmp(argv[i], "-r") == 0)
	{
		is_file = false;
		i++;
		if (strcmp(argv[i], "-p") == 0)
		{
			port = atoi(argv[i+1]);
			i += 2;
		}
		else
		{
			port = def_port;
			i++;
		}
	}
	else if (strcmp(argv[i], "-p") == 0)
	{
		port = atoi(argv[i+1]);
		i += 2;
	}
	else
	{
		port = def_port;
	}

	OmnString str = argv[i];
	//printf("%s\n", str.data());
	int idx_h = str.find('@', false);
	int idx_f = str.find(':', false);
	if (idx_h < 0 && idx_f < 0)
	{
		local = str;
		OmnString last_str = argv[i+1];
		idx_h = last_str.find('@', false);
		idx_f = last_str.find(':', false);
		if (idx_f < 0)
		{
			printf("missing remote directory\n");
			exit(-1);
		}
		remote = last_str.substr(idx_f+1);
		if (idx_h < 0)
		{
			//[local] [host]:[remote]
			host = last_str.substr(0, idx_f-1);
			passwd* pa = getpwuid(getuid());
			user = pa->pw_name;

		}
		else
		{
			//[local] [user]@[host]:[remote]
			host = last_str.substr(idx_h + 1, idx_f-1);
			user = last_str.substr(0, idx_h-1);
		}
		is_tolocal = false;
	}
	else if (idx_h > 0 && idx_f > 0)
	{
		//[user]@[host]:[remote] [local]
		user = str.substr(0, idx_h-1);
		host = str.substr(idx_h+1, idx_f-1);
		remote = str.substr(idx_f+1);
		local = argv[i+1];

	}
	else if (idx_h < 0 && idx_f > 0)
	{
		//[host]:[remote] [local]
		passwd* pa = getpwuid(getuid());
		user = pa->pw_name;
		host = str.substr(0, idx_f-1);
		remote = str.substr(idx_f+1);
		local = argv[i+1];
	}
	else
	{
		printf("args error\n");
		exit(-1);
	}
	//printf("port: %d\nuser: %s\nhost: %s\nremote: %s\nlocal: %s\n", port, user.data(), host.data(), remote.data(), local.data());
	jscp();
	exit(0);
}

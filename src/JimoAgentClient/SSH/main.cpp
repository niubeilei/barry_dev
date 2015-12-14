#include "UtilComm/TcpClient.h"
#include "Porting/Sleep.h"
#include "JimoAgentClient/JimoAgentSSH.h"
#include "JimoAgentUtil/SearchIp.h"

#include <pwd.h> 

int gAosLogLevel = 1;
int gAosShowNetTraffic = 1;

//format : [jssh] -p [port] [user]@[host] [command]
int port = -1;
int def_port = 2200;
OmnString host = "";
OmnString user = "";
OmnString command = "";
void
jssh()
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
	AosJimoAgentSSH* jssh = OmnNew AosJimoAgentSSH(client, user, command);
	jssh->start();
}

int main(int argc,char **argv)
{
	if (argc < 2)
	{
		printf("used [jssh] -p [port] [user]@[host] [command]\n");
		exit(-1);
	}
	int i = 1;
	if (strcmp(argv[i], "-p") == 0)
	{
		port = atoi(argv[i+1]);	
		if (port < 0)
		{
			printf("is invalid port: %d\n", port);
			exit(-1);
		}
		i += 2;
	}
	else
	{
		port = def_port;
	}
	//printf("%d\n", port);

	OmnString str = argv[i];
	int idx = str.find('@', false);
	if (idx < 0)
	{
		host = str;
		passwd* pa = getpwuid(getuid());
		user = pa->pw_name;
	}
	else
	{
		user = str.substr(0, idx-1);
		host = str.substr(idx + 1);
	}
	if (!AosSearchIp::isValidIp(host.data()))
	{
		printf("is invalid host: %s\n", host.data());
		exit(-1);
	}
	i++;
	while(i < argc)
	{
		command << argv[i] << " ";
		i++;
	}
	command.setLength(command.length() - 1);
	jssh();
	//OmnString command;
	//command << "ssh " << user << "@" << host << " " 
	//	    << str;
	//system(command.data());
}

#include "UtilComm/TcpClient.h"
#include "Porting/Sleep.h"

int gAosLogLevel = 1;
int gAosShowNetTraffic = 1;
int main(int argc,char **argv)
{
	int i=1;
	OmnString argv_str;
	if(argc==1)
	{
		printf("input erroe\n");
		return 0;
	}	
	while(i<argc)
	{
		argv_str << argv[i];
		i++;
	}
	printf("%s\n",argv_str.data());

	OmnTcpClientPtr client = OmnNew OmnTcpClient(
			"jimoldclient", 
			OmnString("192.168.99.82"), 
			2200, 
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

	bool rslt = client->smartSend(argv_str.data(), argv_str.length());
	OmnConnBuffPtr buff;
	rslt = client->smartRead(buff);
	if (!rslt || !buff)
	{
		exit(-1);
	}
	char* data = buff->getData();
	OmnString resp;
	resp << data;
	printf("============%s\n", resp.data());
}

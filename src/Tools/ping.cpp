#include "CPing.h"  
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

int main()  
{  
	vector<CPing*> *pingList = new vector<CPing*>;
	CPing *ping;

	//ping an IP for 4 times, timeout = 3 seconds 
	CPing *ping1 = new CPing("www.sohu.com", 3);  
	CPing *ping2 = new CPing("192.168.99.181", 3);  
	pingList->push_back(ping2);
	pingList->push_back(ping1);
	for (int i = 0; i < pingList->size(); i++)
	{
		ping = (*pingList)[i];
		if (ping->start())
			ping->send_packet();
	//}
	
	//for (int i = 0; i < pingList->size(); i++)
	//{
	//	ping = (*pingList)[i];
		if (ping->GetSendTimes() > 0) 
		{
			ping->recv_packet();
			ping->finish();
			cout << "ping " << ping->GetIp() << " average time: " << ping->GetAvgResponseTime() << endl;
		} else {
			ping->finish();
			cout << "failed to send ping packet to: " << ping->GetIp() << endl;
		}

	}
}

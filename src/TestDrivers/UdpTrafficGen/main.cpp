////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: main.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "AppMgr/App.h"
#include "Porting/Sleep.h"
#include "Util/OmnNew.h"
#include "UtilComm/Udp.h"

#include "UdpTrafficGen.h"
const int SLEEP_TIME=1000;// ms

void print_Usage()
{
	cout<<""<<std::endl<<std::endl;

	cout<<"Usage of UdpTrafficGen:"<<std::endl;
	cout<<"\tUdpTrafficGen -la [local addr] -lp [local port] -ra [remote addr] -rp [remote port]"<<std::endl;
	cout<<""<<std::endl;
	cout<<"optional:"<<std::endl;
	cout<<"\t-u [update duration]\t\toutput update duration in mini-second"<<std::endl;
	cout<<"\t-rate [rate]\t\tsend rate(packets per second)"<<std::endl;
	cout<<"\t-pt [packetsize]\tpacket size in bytes"<<std::endl;
	cout<<"\t-f [filename]\t\tfile where the sent data is read from"<<std::endl;
	cout<<"\t-m [maxPacketsNum]\t\tthe number of packets need to be sent"<<std::endl;
	cout<<"\t-break [breakTime]\t\tevery breakTime in second it will stop sending packets, and users can press any key to continue"<<std::endl;
	cout<<""<<std::endl;
	cout<<"Example:"<<std::endl;
	cout<<"\t./UdpTrafficGen.exe -la 127.0.0.1 -lp 3001 -ra 127.0.0.1 -rp 3000 -pt 256 -rate 1000 -u 1000 -m 10000"<<std::endl;
}

int 
main(int argc, char **argv)
{
	OmnIpAddr remoteAddr, localAddr;
	int times=0;
	int remotePort = -1, localPort;
	int rate=1000, packetSize=512;
	OmnString fileName;
	int updateTime=SLEEP_TIME;
	unsigned int maxPackets=0;
	int breakTime=0;

	OmnApp theApp(argc, argv);
	try
	{
	}

	catch (const OmnExcept &e)
	{
		OmnAlarm << "Failed to start the application: " 
			<< e.toString() << enderr;
		return 0;
	}

	int index = 1;
	while (index < argc)
	{
		if (strcmp(argv[index], "-ra") == 0)
		{
			remoteAddr = OmnIpAddr(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-rp") == 0)
		{
			remotePort = atoi(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-la") == 0)
		{
			localAddr = OmnIpAddr(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-lp") == 0)
		{
			localPort = atoi(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-times") == 0)
		{
			times = atoi(argv[index+1]);
			index += 2;
			continue;
		}
	
		if (strcmp(argv[index], "-rate") == 0)
		{
			rate = atoi(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-pt") == 0)
		{
			packetSize = atoi(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-f") == 0)
		{
			fileName = argv[index+1];
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-u") == 0)
		{
			updateTime = atoi(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-m") == 0)
		{
			maxPackets = atoi(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-break") == 0)
		{
			breakTime = atoi(argv[index+1]);
			index += 2;
			continue;
		}
		index++;
	}

	if (localPort < 0 || remotePort < 0)
	{
		cout << "Need to set the ports" << endl;
		print_Usage();
		theApp.exitApp();
		return 0;
	}

	if (!localAddr || !remoteAddr)
	{
		cout << "Need to set the addresses" << endl;
		print_Usage();
		theApp.exitApp();
		return 0;
	}

	if (updateTime <= 0)
	{
		cout << "the update duration must more than 0" << endl;
		print_Usage();
		theApp.exitApp();
		return 0;
	}


	cout<<"rate="<<rate<<endl;
	AosUdpTrafficGen udpTrafficGen(localAddr,localPort,remoteAddr,remotePort,fileName,packetSize,rate,maxPackets, breakTime);
	udpTrafficGen.start();

	int packetsSent,packetsRecvd,packetsSentRate,packetsRecvdRate,packetsLossRate,packetsLossTotal; 
	int bytesSent,bytesRecvd,bytesSentRate,bytesRecvdRate,bytesLossRate,bytesLossTotal; 
	packetsSent=packetsRecvd=packetsSentRate=packetsRecvdRate=packetsLossRate=packetsLossTotal=0; 
	bytesSent=bytesRecvd=bytesSentRate=bytesRecvdRate=bytesLossRate=bytesLossTotal=0; 

	while (1)
	{
		//system("clear");
		packetsSentRate = (udpTrafficGen.getPacketSent()-packetsSent)*1000/updateTime;
		packetsRecvdRate = (udpTrafficGen.getPacketReceived()-packetsRecvd)*1000/updateTime;
		bytesSentRate = (udpTrafficGen.getBytesSent()-bytesSent)*1000/updateTime;
		bytesRecvdRate = (udpTrafficGen.getBytesReceived()-bytesRecvd)*1000/updateTime;

		packetsLossTotal = udpTrafficGen.getPacketSent() - udpTrafficGen.getPacketReceived();
		bytesLossTotal = udpTrafficGen.getBytesSent() - udpTrafficGen.getBytesReceived();
		packetsLossRate = packetsSentRate - packetsRecvdRate;
		bytesLossRate = bytesSentRate - bytesRecvdRate;


		if (udpTrafficGen.isSendingStopped())
		{
			printf("%d packets are sent over\n",maxPackets);
		}

		printf("********************************************************Begin*"
			"********************************************************\n");
		printf("Input params: Update time=%d ms; Send Rate=%d packet/s; Packet size=%d bytes;MaxPackets=%d\n\n", updateTime, rate, packetSize,maxPackets);
		printf("Packets Sent:%10d\tPackets Recvd:%10d\tPackets SentRate:%10d\tPackets RecvdRate:%10d\n"
		       "Bytes Sent:%12d\tBytes Recvd:%12d\tBytes   SentRate:%10d\tBytes   RecvdRate:%10d\n"
		       "Packets Loss:%10d\tBytes    Loss:%10d\tPackets LossRate:%10d\tBytes    LossRate:%10d\n",
				udpTrafficGen.getPacketSent(),
				udpTrafficGen.getPacketReceived(),
				packetsSentRate,
				packetsRecvdRate,
				udpTrafficGen.getBytesSent(),
				udpTrafficGen.getBytesReceived(),
				bytesSentRate,
				bytesRecvdRate,
				packetsLossTotal,
				bytesLossTotal,
				packetsLossRate,
				bytesLossRate
				);

		printf("********************************************************End**"
			"*********************************************************\n\n");
	
		packetsSent = udpTrafficGen.getPacketSent(); 
		packetsRecvd = udpTrafficGen.getPacketReceived(); 
		bytesSent = udpTrafficGen.getBytesSent(); 
		bytesRecvd = udpTrafficGen.getBytesReceived();

		if (udpTrafficGen.getIsBreak())
		{
			printf("stop sending after %d seconds, press enter key to continue sending.........................................\n", breakTime);
			pthread_mutex_lock(&udpTrafficGen.mMutex);
			pthread_cond_wait(&udpTrafficGen.mCond,&udpTrafficGen.mMutex);
			pthread_mutex_unlock(&udpTrafficGen.mMutex);
		}

		if (udpTrafficGen.isSendingStopped())
		{
			OmnSleep(5);
			continue;
		}
		OmnMsSleep(updateTime);
	}







/*	OmnUdp udp("test", localAddr, localPort);
	OmnString errmsg;
	if (!udp.connect(errmsg))
	{
		cout << "************ Failed to connect: " << errmsg << endl;
		theApp.exitApp();
		print_Usage();
		return 0;
	}

	OmnString contents = "Hello World!";
	udp.writeTo(contents, contents.length(), remoteAddr, remotePort);
	cout << "Contents sent: " << contents.data() << endl;
*/
	theApp.exitApp();
	return 0;
} 
 

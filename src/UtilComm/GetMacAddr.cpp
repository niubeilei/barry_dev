////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
// This is from http://www.oschina.net/code/snippet_145001_14507
//   
//
// Modification History:
// 10/14/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "UtilComm/GetMacAddr.h"

#if 0
int GetLocalMacAddr(char *szMac,int *pnMacLen)
{
	int   sock;  
	struct   sockaddr_in   sin;  
	struct   sockaddr   sa;  
	struct   ifreq   ifr;  
	unsigned   char   mac[6];  
		     
	sock=socket(AF_INET,SOCK_DGRAM,0);  
	if (sock==-1)  
	{  
		perror("socket");  
		return 1;  
	}  
		     
	strncpy(ifr.ifr_name,"eth0",sizeof(ifr.ifr_name));  
	ifr.ifr_name[IFNAMSIZ-1]   =   0;  
		     
	memset(mac,0,sizeof(mac));  
	if (ioctl(sock,SIOCGIFHWADDR,&ifr)< 0)  
	{  
		perror("ioctl");  
		return 2;  
	}  
		     
	memcpy(&sa,&ifr.ifr_addr,sizeof(sin));  
	memcpy(mac,sa.sa_data,sizeof(mac));  
	char curmacstr[64];
	memset(curmacstr,0,sizeof(curmacstr));
	//sprintf(curmacstr,"%.2X-%.2X-%.2X-%.2X-%.2X-%.2X",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
	sprintf(curmacstr,"%.2X:%.2X:%.2X:%.2X:%.2X:%.2X",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
	strcpy(szMac,curmacstr);
	return 0;
}


// windows
int CUtil::GetOtherMacAddr(char *szIP,char *szBuf,int *pnBufLen)
{
	HRESULT hr;
	IPAddr  ipAddr;
	ULONG   pulMac[2];
	ULONG   ulLen;
	char strMacAddr[100]={0};
	ipAddr = inet_addr (szIP);
	memset (pulMac, 0xff, sizeof (pulMac));
	ulLen = 6;
	hr = SendARP (ipAddr, 0, pulMac, &ulLen);
	if(hr!=NO_ERROR)
	return 1;
	
	unsigned char * mac_addr=(unsigned char*)pulMac;
	sprintf(strMacAddr,"%02X-%02X-%02X-%02X-%02X-%02X",mac_addr[0],mac_addr[1],
	mac_addr[2],mac_addr[3],mac_addr[4],mac_addr[5]);
	sprintf(strMacAddr,"%02X:%02X:%02X:%02X:%02X:%02X",mac_addr[0],mac_addr[1],
	mac_addr[2],mac_addr[3],mac_addr[4],mac_addr[5]);
	if ( *pnBufLen <= (int)strlen(strMacAddr) ) return 2;
	strcpy(szBuf,strMacAddr);
	*pnBufLen = strlen(szBuf);
	
	return 0;
}

int CUtil::GetLocalMacAddr(char *szMac,int *pnMacLen,char *szIP /*=NULL */)
{
	//如果指定了IP，则直接按IP获取MAC
	//否则，需要先获取本机名称，再获取IP，再获取MAC
	if ( szIP != NULL )
	return GetOtherMacAddr(szIP,szMac,pnMacLen);
	
	char szHostName[256] = {0};
	int nRet = gethostname(szHostName,256);
	if ( nRet == SOCKET_ERROR )
	return 1;
	
	//获取本机名称
	struct hostent* hHost = gethostbyname(szHostName);
	if ( hHost == NULL ||  hHost->h_addr_list[0] == NULL )
	return 2;
	
	//获取IP地址
	memset(szHostName,0,256);
	strcpy(szHostName,inet_ntoa(*(struct in_addr *)hHost->h_addr_list[0]));
	
	//获取MAC
	return  GetOtherMacAddr(szHostName,szMac,pnMacLen);
}
#endif

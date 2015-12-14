////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SystemApi.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "rhcAPI/SystemApi.h"

#include <stdio.h>
#include <ctype.h>

#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <string.h>
#include <mntent.h>
#include <sys/vfs.h>    /* or <sys/statfs.h> */

#define TCP_PROTO	"tcp"
#define MAX_LEN	256

#define MIN(x,y) ((x)>(y)?(y):(x))
struct dhcpConfigEntry sgDhcp;	
struct aos_list_head sgDhcpIpBindList;
struct aos_list_head sgSecureCmdList;

/*
int get_Word(const char *buffer, const unsigned int index,char *word) 
{
	// 
	// Starting from the end, find the first space
	//
	int len = strlen(buffer);
	if(index > len)
	{
		return -1;
	}
	int curPos = index;
	while ( curPos < len && 
			( buffer[curPos] == ' '
			  || buffer[curPos] == '\t' 
			  || buffer[curPos] == '\r' 
			  || buffer[curPos] == '\n' 
			  )) 
	{
		curPos ++;
	}
    if (curPos >= len)
	{
		word = "";
		return curPos;
	}
	while ( curPos < len && 
			( buffer[curPos] == ' '
			  || buffer[curPos] != '\t' 
			  || buffer[curPos] != '\r' 
			  || buffer[curPos] != '\n' 
			  )) 
	{
		curPos ++;
	}
	memcpy(word,buffer+index,curPos - index); 
	return curPos;
}
*/	
/*	
	char contents[MAX_LEN];
	strcpy(contents, buffer);
	len = strlen(contents);
	if(index >= len)
	{
		substr = "";
		return -1;
	}
	unsigned int curPos = index;

	while ( curPos < len && 
			( contents[curPos] == ' '
			  || contents[curPos] == '\t' 
			  || contents[curPos] == '\r' 
			  || contents[curPos] == '\n' 
			  )) 
	{
		curPos++;
	}

	if (curPos >= Len)
	{
		substr = "";
		return curPos;
	}

	unsigned int startPos = curPos;
	// 
	// Find the next non-space character
	//
	//while (curPos < mLen && (!(mDataPtr[curPos] == ' ')) && (!(mDataPtr[curPos] == '\t'))) curPos++;

	while ( curPos < mLen 
			&& contents[curPos] != ' '
			&& contents[curPos] != '\t' 
			&& contents[curPos] != '\r' 
			&& contents[curPos] != '\n' 
			  ) 
	{
		curPos++;
	}

	//substr.assign(mDataPtr+startPos,curPos-startPos);
	substr = assign(mDataPtr+startPos,curPos-startPos);
	return curPos;
}*/

/*

int readBlock(const char *buffer, const char *start, const char *end, char *result)
{
	// 
	// This function reads a block. The block starts
	// with a line that matches 'start' and ends with
	// a line that matches 'end'. Anything in between
	// is retrieved into 'contents'.
	//
	
	int len, tmplen;
	int i=0;
	unsigned int index = 0;
	unsigned int startPos = 0;
	int isSame = 1;
	char c;
	char tmpStr[MAX_LEN];
	char tmp[MAX_LEN];
	unsigned int bytesRead = 0;
	int containPattern = -1;
	char contents[MAX_LEN];
	tmpStr[0] = 0;
	tmp[0] = 0;

	strcpy(contents, buffer);
	len = strlen(start);
	while (1)
	{
		c = contents[i];
		i++;
		//if (c == 0)
		//{
		//	return -1;
		//}
		tmplen = strlen(tmpStr);
		tmpStr[tmplen] = c;
		tmpStr[tmplen+1] = '\0';
		// if length enough , check whether it matchs.
		if(startPos + len <= strlen(tmpStr))
		{
			
			index = 0;
			isSame = 1;
			while(index < len)
			{
				if (! (*(tmpStr + startPos + index) == *(start + index)))
				{
					isSame = 0;
					break;
				}
				index ++;
			}
			if(isSame == 1)
			{
				break;
				// find it and return
			}
			startPos ++;
		}
	}

	index = 0;
	isSame = 1;
	startPos = 0;
	len = strlen(end);
	while (i < strlen(contents))
	{
		c = contents[i];
		i++;
		if (c == 0)
		{
			return -1;
		}
		tmplen = strlen(result);
		result[tmplen] = c;
		result[tmplen+1] = '\0';
		// Compare whether it match the pattern		
		if(startPos + len <= strlen(result))
		{
			index = 0;
			isSame = 1;
			while(index < len)
			{
				if (! (*(result + startPos + index) == *(end + index)))
				{
					isSame = 0;
					break;
				}
				index ++;
			}
			if(isSame == 1)
			{
				// find it and return
				if (containPattern)
				{
					tmplen = strlen(result);
					if (tmplen >= len)
					{
						result[tmplen - len] = '\0';
					}
				}
				return 0;
			}
			startPos ++;			
		}
		bytesRead ++;
	}

	if (strlen(result) == 0)
	{
		return -1;
	}
	return 0;
}
*/

int readBlock(const char *buffer, const char *start, const char *end, char *result, int result_len)
{
        int buffer_len,start_len,end_len;
        char *p_start, *p_end;

        if(!buffer || !start || !end || result_len<2)
                return -1;

        start_len = strlen(start);
        end_len = strlen(end);
        buffer_len = strlen(buffer);

        result[0]=0;

        if( buffer_len <= start_len+end_len )
                return -1;

        p_start = strstr( buffer, start );
        if ( !p_start )
                return -1;

        p_end = strstr( p_start+start_len, end );
        if ( !p_end )
                return -1;

	strncpy(result, p_start+start_len, MIN(result_len-1,p_end-p_start-start_len));
	result[MIN(result_len-1,p_end-p_start-start_len)]=0;
        return 0;
 }
 
int	sendSystemCli(char *sbuffer, char *rbuffer)
{
	FILE *fp;
	int isosockfd; 
	int connstat;
	int iolen;
	struct hostent *hostp; 
    struct sockaddr_in addr;
	char buf[4096];
	char ip[MAX_IP_STRING_LEN];
	char port[MAX_PORT_STRING_LEN];
	char tmp[MAX_MODID_STRING_LEN];

	if((fp = fopen(CMD_TXT, "r")) == NULL)
		return -1; 
	
	buf[0] = 0;
	int bFinded = 0;	/*add by CHK*/
	while ((fgets(buf, sizeof(buf), fp))!=NULL)
	{
		if (strcmp(buf, "<ModDef>\n") == 0)
		{
			fgets(buf, sizeof(buf), fp);
			while(strcmp(buf, "</ModDef>\n") != 0)
			{
				tmp[0] = 0;
				readBlock(buf, "<ModId>", "</ModId>", tmp,MAX_LEN); 
				if (strcmp(tmp, "system") == 0)
				{
					tmp[0] = 0;
					fgets(buf, sizeof(buf), fp);
					readBlock(buf, "<ModAddr>", "</ModAddr>", tmp,MAX_LEN); 
					strcpy(ip, tmp);
					tmp[0] = 0;
					fgets(buf, sizeof(buf), fp);
					readBlock(buf, "<ModPort>", "</ModPort>", tmp,MAX_LEN); 
					strcpy(port, tmp);
					bFinded = 1;	/*add by CHK*/
					break;	/*add by CHK*/
				}	
				fgets(buf, sizeof(buf), fp);
			}
			if(bFinded)/*add by CHK*/
				break;	/*add by CHK*/
		}
	}

	if(!bFinded)/*add by CHK*/
		return -1;

	fclose(fp);
	
    /* First create a socket */
	if ((isosockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{
		printf("failed to create socket to host");
        return -1;
    }
	bzero(&addr, sizeof(addr)); 
	addr.sin_family = AF_INET; 
	if ((hostp = gethostbyname(ip)) != NULL)
	{	
		bcopy(hostp->h_addr,&addr.sin_addr,hostp->h_length); 
	}
	else 
	{ 
		printf("unknown host\r\n"); 
		exit(1); 
	}
	addr.sin_port = htons(atoi(port)); 
	connstat = connect(isosockfd,(struct sockaddr *) &addr, sizeof(addr)); 
	strcpy(buf, sbuffer);
	
	//printf("To Server Msg:-------");
	printf("To Server Msg:%s\n", buf);
	//write(isosockfd, buf, iolen); 
	write(isosockfd, buf, strlen(buf)); 
	/* is the host sending data? */ 
	iolen = read(isosockfd, buf, sizeof(buf));
	/* zero length means the host disconnected */ 
	//printf("Server Msg:-------");
	//printf("Server Msg:%s\n", buf);
	//printk(KERN_ALERT "Before the sock connect!\n");

	strcpy(rbuffer, buf);

	close(isosockfd);

	return 0;
}
		
extern int dhcpServerDnsSet(char *dns, char *result)
{
	char sbuffer[1024];
	char rbuffer[2048];
	// send the data to system CLI daemon
	strcpy(sbuffer, "dhcp server dns set");
	strcat(sbuffer, " ");
	strcat(sbuffer, dns);
	//printf("the send buffer is:%s\n", sbuffer);
	sendSystemCli(sbuffer, rbuffer);	
	// return the return message to the result

	//strcpy(rbuffer, "test!!!\n");
	strcpy(result, rbuffer);
	return 0;
}		

int dhcpServerRouterSet(char *router, char *result) 
{
	char sbuffer[1024];
	char rbuffer[2048];
	// send the data to system CLI daemon
	strcpy(sbuffer, "dhcp server router set");
	strcat(sbuffer, " ");
	strcat(sbuffer, router);
	//printf("the send buffer is:%s\n", sbuffer);
	sendSystemCli(sbuffer, rbuffer);	
	// return the return message to the result

	//strcpy(rbuffer, "test!!!\n");
	strcpy(result, rbuffer);
	return 0;
}

int dhcpServerAddBind(char *label, char *mac, char *ip, char *result) 
{
	char sbuffer[1024];
	char rbuffer[2048];
	// send the data to system CLI daemon
	strcpy(sbuffer, "dhcp server add bind");
	strcat(sbuffer, " ");
	strcat(sbuffer, label);
	strcat(sbuffer, " ");
	strcat(sbuffer, mac);
	strcat(sbuffer, " ");
	strcat(sbuffer, ip);
	//printf("the send buffer is:%s\n", sbuffer);
	sendSystemCli(sbuffer, rbuffer);	
	// return the return message to the result

	//strcpy(rbuffer, "test!!!\n");
	strcpy(result, rbuffer);
	return 0;
}

int dhcpServerDelBind(char *label, char *result) 
{
	char sbuffer[1024];
	char rbuffer[2048];
	// send the data to system CLI daemon
	strcpy(sbuffer, "dhcp server del bind");
	strcat(sbuffer, " ");
	strcat(sbuffer, label);
	//printf("the send buffer is:%s\n", sbuffer);
	sendSystemCli(sbuffer, rbuffer);	
	// return the return message to the result

	//strcpy(rbuffer, "test!!!\n");
	strcpy(result, rbuffer);
	return 0;
}

int dhcpServerIpBlockSet(char *ip1, char *ip2, char *result) 
{
	char sbuffer[1024];
	char rbuffer[2048];
	// send the data to system CLI daemon
	strcpy(sbuffer, "dhcp server ip block");
	strcat(sbuffer, " ");
	strcat(sbuffer, ip1);
	strcat(sbuffer, " ");
	strcat(sbuffer, ip2);
	//printf("the send buffer is:%s\n", sbuffer);
	sendSystemCli(sbuffer, rbuffer);	
	// return the return message to the result

	//strcpy(rbuffer, "test!!!\n");
	strcpy(result, rbuffer);
	return 0;
}

int dhcpServerLeaseTimeSet(char *leaseTime, char *result) 
{
	char sbuffer[1024];
	char rbuffer[2048];
	// send the data to system CLI daemon
	strcpy(sbuffer, "dhcp server lease time set");
	strcat(sbuffer, " ");
	strcat(sbuffer, leaseTime);
	//printf("the send buffer is:%s\n", sbuffer);
	sendSystemCli(sbuffer, rbuffer);	
	// return the return message to the result

	//strcpy(rbuffer, "test!!!\n");
	strcpy(result, rbuffer);
	return 0;
}

int dhcpServerStart(char *result) 
{
	char sbuffer[1024];
	char rbuffer[2048];
	// send the data to system CLI daemon
	strcpy(sbuffer, "dhcp server start");
	//printf("the send buffer is:%s\n", sbuffer);
	sendSystemCli(sbuffer, rbuffer);	
	// return the return message to the result

	//strcpy(rbuffer, "test!!!\n");
	strcpy(result, rbuffer);
	return 0;
}

int dhcpServerStop(char *result) 
{
	char sbuffer[1024];
	char rbuffer[2048];
	// send the data to system CLI daemon
	strcpy(sbuffer, "dhcp server stop");
	//printf("the send buffer is:%s\n", sbuffer);
	sendSystemCli(sbuffer, rbuffer);	
	// return the return message to the result

	//strcpy(rbuffer, "test!!!\n");
	strcpy(result, rbuffer);
	return 0;
}

extern int dhcpServerShowConfig(struct dhcpConfigEntry *dhcp, char *result)
{
	char sbuffer[1024];
	char rbuffer[2048];
	char ip1[MAX_IP_STRING_LEN];
	char ip2[MAX_IP_STRING_LEN];
	char tmp[MAX_LEN];
	char *tmpsave;
	char ipbind[MAX_LEN];
	char alias[MAX_LEN];
	char mac[MAX_MAC_STRING_LEN];
	char ip[MAX_IP_STRING_LEN];
	int ret = 0;
	// send the data to system CLI daemon
	strcpy(sbuffer, "dhcp server show config");
	//printf("the send buffer is:%s\n", sbuffer);
	sendSystemCli(sbuffer, rbuffer);	
	// return the return message to the result

	//strcpy(rbuffer, "test!!!\n");
	//strcpy(result, rbuffer);

	//parse the rbuffer

    strcpy(tmp,"");
    readBlock(rbuffer, "status : ", "\n\tlease", tmp, MAX_LEN);
	strcpy(dhcp->status, tmp);
	printf("    the status is : %s\n", dhcp->status);

    strcpy(tmp,"");
	readBlock(rbuffer, "time : ", "\n\t    router", tmp, MAX_LEN);
	strcpy(dhcp->dft_leaseTime, tmp);
	printf("the lease time is : %s\n", dhcp->dft_leaseTime);

	strcpy(tmp,"");
	readBlock(rbuffer, "router : ", "\n\t       dns", tmp, MAX_LEN);
	strcpy(dhcp->router, tmp);
	printf("    the router is : %s\n", dhcp->router);

    strcpy(tmp,"");
   	readBlock(rbuffer, "dns : ", "\n\t  ip block", tmp, MAX_LEN);
	strcpy(dhcp->dns, tmp);
	printf("       the dns is : %s\n", dhcp->dns);
   
    strcpy(tmp,"");
   	//readBlock(rbuffer, "block : ", "\n\t bind", tmp);
   	readBlock(rbuffer, "block : ", "\n\t bind", tmp, MAX_LEN);
	
	if(strcmp(tmp,"")!=0)
	{
		strcpy(ip1, strtok_r(tmp, " ", &tmpsave));
		strcpy(ip2, strtok_r(NULL, " ", &tmpsave));
	    strcpy(&dhcp->ip1, ip1);
    	strcpy(&dhcp->ip2, ip2);
	}
	printf("  the ip block is : %s\t%s\n", dhcp->ip1,dhcp->ip2);

    struct dhcpIpBindEntry * ptr;
    struct dhcpIpBindEntry * tmpptr;
	aos_list_for_each_entry_safe(ptr, tmpptr, &sgDhcpIpBindList, datalist)
    {       
       	aos_list_del(&ptr->datalist);
        free(ptr);
    }
	AOS_INIT_LIST_HEAD(&sgDhcpIpBindList);
	
	strcpy(tmp,"");
   	readBlock(rbuffer, "IP\n\t\t\t--------------\n\t\t\t", "\n", tmp, MAX_LEN);
	if(strcmp(tmp,"")!=0)
	{
		char c;
		char *seperator=" ";
		strcpy(alias,"");
		strcpy(mac,"");
		strcpy(ip,"");
	    strcpy(alias,strtok_r(tmp,seperator,&tmpsave));
		strcpy(mac,strtok_r(NULL,seperator,&tmpsave));
		strcpy(ip,strtok_r(NULL,seperator,&tmpsave));

	    printf("  the bind : %s\t%s\t%s\n", alias, mac, ip);

		struct dhcpIpBindEntry * ptr;

    	aos_list_for_each_entry(ptr, &sgDhcpIpBindList, datalist)
    	{       
    		if(strcmp(ptr->alias, alias)==0) 
   			{       
				return -1;
			}
      	}            



		struct dhcpIpBindEntry * data;
		if((data = (struct dhcpIpBindEntry *)malloc(sizeof(struct dhcpIpBindEntry)))==NULL)
		{
			//printk("malloc error");
			return -1;
		}
		memset(data, 0, sizeof(struct dhcpIpBindEntry));
		strcpy(data->alias, alias);
		strcpy(data->mac, mac);
		strncpy(data->ip, ip,MAX_IP_STRING_LEN);
		aos_list_add_tail((struct aos_list_head *)&data->datalist, &sgDhcpIpBindList);

	}

	while(1)
	{
		memset(tmp, 0, sizeof(256));
		strcpy(ipbind, ip);
		strcat(ipbind, "\n\t\t\t");
   		readBlock(rbuffer, ipbind, "\n", tmp, MAX_LEN);
	 //	printf("  the tmp is %s\n", tmp);
		if(strcmp(tmp,"") == 0)
			break;

		strcpy(alias,"");
		strcpy(mac,"");
		strcpy(ip,"");
	    strcpy(alias, strtok_r(tmp," ",&tmpsave));
		strcpy(mac, strtok_r(NULL," ",&tmpsave));
		strcpy(ip, strtok_r(NULL," ",&tmpsave));
	    printf("  the bind : %s\t%s\t%s\n", alias, mac, ip);

		struct dhcpIpBindEntry * ptr;

    	aos_list_for_each_entry(ptr, &sgDhcpIpBindList, datalist)
    	{       
    		if(strcmp(ptr->alias, alias)==0) 
   			{       
				return -1;
			}
      	}            

		struct dhcpIpBindEntry * data;                                                    
		if((data = (struct dhcpIpBindEntry *)malloc(sizeof(struct dhcpIpBindEntry)))==NULL)                                               
		{                                                    
			//printk("malloc error");                                                    
			return -1;                                                    
		}                                                    
		memset(data, 0, sizeof(struct dhcpIpBindEntry));                                                    
		strcpy(data->alias, alias);                                                    
		strcpy(data->mac, mac);                                                    
		strcpy(data->ip, ip);                                                    
		aos_list_add_tail((struct aos_list_head *)&data->datalist, &sgDhcpIpBindList);                                                    
	}
	
	return 0;
}

//Dns Proxy
int dnsproxyStart(char *result) 
{
	char sbuffer[1024];
	char rbuffer[2048];
	// send the data to system CLI daemon
	strcpy(sbuffer, "dns proxy start");
	//printf("the send buffer is:%s\n", sbuffer);
	sendSystemCli(sbuffer, rbuffer);	
	// return the return message to the result

	//strcpy(rbuffer, "test!!!\n");
	strcpy(result, rbuffer);
	return 0;
}


int dnsproxyStop(char *result) 
{
	char sbuffer[1024];
	char rbuffer[2048];
	// send the data to system CLI daemon
	strcpy(sbuffer, "dns proxy stop");
	//printf("the send buffer is:%s\n", sbuffer);
	sendSystemCli(sbuffer, rbuffer);	
	// return the return message to the result

	//strcpy(rbuffer, "test!!!\n");
	strcpy(result, rbuffer);
	return 0;
}


int dnsproxySetName(char *label, char *result) 
{
	char sbuffer[1024];
	char rbuffer[2048];
	// send the data to system CLI daemon
	strcpy(sbuffer, "dns proxy set name");
	strcat(sbuffer, " ");
	strcat(sbuffer, label);
	//printf("the send buffer is:%s\n", sbuffer);
	sendSystemCli(sbuffer, rbuffer);	
	// return the return message to the result

	//strcpy(rbuffer, "test!!!\n");
	strcpy(result, rbuffer);
	return 0;
}


int dnsproxySetIp(char *ip, char *result) 
{
	char sbuffer[1024];
	char rbuffer[2048];
	// send the data to system CLI daemon
	strcpy(sbuffer, "dns proxy set ip");
	strcat(sbuffer, " ");
	strcat(sbuffer, ip);
	//printf("the send buffer is:%s\n", sbuffer);
	sendSystemCli(sbuffer, rbuffer);	
	// return the return message to the result

	//strcpy(rbuffer, "test!!!\n");
	strcpy(result, rbuffer);
	return 0;
}


int dnsproxySetLIp(char *lip, char *dev, char *result) 
{
	char sbuffer[1024];
	char rbuffer[2048];
	// send the data to system CLI daemon
	strcpy(sbuffer, "dns proxy set lip");
	strcat(sbuffer, " ");
	strcat(sbuffer, lip);
	strcat(sbuffer, " ");
	strcat(sbuffer, dev);
	//printf("the send buffer is:%s\n", sbuffer);
	sendSystemCli(sbuffer, rbuffer);	
	// return the return message to the result

	//strcpy(rbuffer, "test!!!\n");
	strcpy(result, rbuffer);
	return 0;
}


int dnsproxyAddGlobal(char *result) 
{
	char sbuffer[1024];
	char rbuffer[2048];
	// send the data to system CLI daemon
	strcpy(sbuffer, "dns proxy add global");
	//printf("the send buffer is:%s\n", sbuffer);
	sendSystemCli(sbuffer, rbuffer);	
	// return the return message to the result

	//strcpy(rbuffer, "test!!!\n");
	strcpy(result, rbuffer);
	return 0;
}


int dnsproxyDelGlobal(char *result) 
{
	char sbuffer[1024];
	char rbuffer[2048];
	// send the data to system CLI daemon
	strcpy(sbuffer, "dns proxy del global");
	//printf("the send buffer is:%s\n", sbuffer);
	sendSystemCli(sbuffer, rbuffer);	
	// return the return message to the result

	//strcpy(rbuffer, "test!!!\n");
	strcpy(result, rbuffer);
	return 0;
}


int dnsproxyShow(struct dnsProxyConfigEntry *dnsproxy, char *result) 
{
	char sbuffer[1024];
	char rbuffer[2048];
	char tmp[MAX_LEN];
	// send the data to system CLI daemon
	strcpy(sbuffer, "dns proxy show config");
	//printf("the send buffer is:%s\n", sbuffer);
	sendSystemCli(sbuffer, rbuffer);	
	// return the return message to the result

	//strcpy(rbuffer, "test!!!\n");
	//strcpy(result, rbuffer);
	
	strcpy(tmp,"");
    readBlock(rbuffer, "status : ", "\n\t   global", tmp, MAX_LEN);
	strcpy(dnsproxy->status, tmp);
	printf("    the status is : %s\n", dnsproxy->status, MAX_LEN);
    
	strcpy(tmp,"");
    readBlock(rbuffer, "global : ", "\n\t    label", tmp, MAX_LEN);
	strcpy(dnsproxy->global, tmp);
	printf("           global : %s\n", dnsproxy->global);

	strcpy(tmp,"");
    readBlock(rbuffer, "label : ", "\n\t      dns", tmp, MAX_LEN);
	strcpy(dnsproxy->name, tmp);
	printf("             name : %s\n", dnsproxy->name);

	strcpy(tmp,"");
    readBlock(rbuffer, "dns : ", "\n\t local", tmp, MAX_LEN);
	strcpy(dnsproxy->ip, tmp);
	printf("              dns : %s\n", dnsproxy->ip);

    strcpy(tmp,"");
    readBlock(rbuffer, "ip : ", "\n\t      dev", tmp, MAX_LEN);
	strcpy(dnsproxy->lIp, tmp);
	printf("               ip : %s\n", dnsproxy->lIp);

    strcpy(tmp,"");
    readBlock(rbuffer, "dev : ", "\n", tmp, MAX_LEN);
	strcpy(dnsproxy->dev, tmp);
	printf("              dev : %s\n", dnsproxy->dev);


	return 0;
}

int secureAuthcmdPolicy(char *policy, char *result)
{
	char sbuffer[1024];
	char rbuffer[2048];
	// send the data to system CLI daemon
	strcpy(sbuffer, "secure authcmd policy");
	strcat(sbuffer, " ");
	strcat(sbuffer, policy);
	//printf("the send buffer is:%s\n", sbuffer);
	sendSystemCli(sbuffer, rbuffer);	
	// return the return message to the result

	//strcpy(rbuffer, "test!!!\n");
	strcpy(result, rbuffer);
	return 0;
}		

int secureAuthcmdCommandAdd(char *command, char *result)
{
	char sbuffer[1024];
	char rbuffer[2048];
	// send the data to system CLI daemon
	strcpy(sbuffer, "secure authcmd command add");
	strcat(sbuffer, " ");
	strcat(sbuffer,command );
	//printf("the send buffer is:%s\n", sbuffer);
	sendSystemCli(sbuffer, rbuffer);	
	// return the return message to the result

	//strcpy(rbuffer, "test!!!\n");
	strcpy(result, rbuffer);
	return 0;
}

int secureAuthcmdCommandDel(char *command, char *result)
{
	char sbuffer[1024];
	char rbuffer[2048];
	// send the data to system CLI daemon
	strcpy(sbuffer, "secure authcmd command del");
	strcat(sbuffer, " ");
	strcat(sbuffer,command );
	//printf("the send buffer is:%s\n", sbuffer);
	sendSystemCli(sbuffer, rbuffer);	
	// return the return message to the result

	//strcpy(rbuffer, "test!!!\n");
	strcpy(result, rbuffer);
	return 0;
}

int secureAuthcmdCommandReset(char *result)
{
	char sbuffer[1024];
	char rbuffer[2048];
	// send the data to system CLI daemon
	strcpy(sbuffer, "secure authcmd command reset");
	//printf("the send buffer is:%s\n", sbuffer);
	sendSystemCli(sbuffer, rbuffer);	
	// return the return message to the result

	//strcpy(rbuffer, "test!!!\n");
	strcpy(result, rbuffer);
	return 0;

}

int secureAuthcmdCommandShow(struct secureCmdEntry *secureCmd, char *result)
{
	char sbuffer[1024];
	char rbuffer[2048];
	char tmp[MAX_LEN];
	char *tmpsave;
	char macbind[MAX_LEN];
	char path[MAX_NAME_STRING_LEN];
	char mac[MAX_NAME_STRING_LEN];
	char policy[MAX_NAME_STRING_LEN];
	// send the data to system CLI daemon
	strcpy(sbuffer, "secure authcmd command show");
	//printf("the send buffer is:%s\n", sbuffer);
	sendSystemCli(sbuffer, rbuffer);	
	// return the return message to the result

	//strcpy(rbuffer, "test!!!\n");
	//	strcpy(result, rbuffer);
	struct secureCmdEntry * ptr;
    struct secureCmdEntry * tmpptr;
	aos_list_for_each_entry_safe(ptr, tmpptr, &sgSecureCmdList, datalist)
    {       
       	aos_list_del(&ptr->datalist);
        free(ptr);
    }
	AOS_INIT_LIST_HEAD(&sgSecureCmdList);
	
	//printf("rbuffer = %s \n", rbuffer);
	strcpy(tmp,"");
   	readBlock(rbuffer, "The Command List\n------------------------\n", "\n", tmp, MAX_LEN);
	if(strcmp(tmp,"")!=0)
	{
		char c;
		char *seperator="\t";
		strcpy(path,"");
		strcpy(mac,"");
	    strcpy(path,strtok_r(tmp,seperator,&tmpsave));
		strcpy(mac,strtok_r(NULL,seperator,&tmpsave));

		if((strcmp(path,"filename") == 0) && (strcmp(mac,"mac") ==0))
		{
			
   			readBlock(rbuffer, "filename\tmac\n", "\n", tmp, MAX_LEN);
			strcpy(path,"");
			strcpy(mac,"");
	    	strcpy(path,strtok_r(tmp,seperator,&tmpsave));
			strcpy(mac,strtok_r(NULL,seperator,&tmpsave));
	    	printf("  path = %s mac = %s \n", path, mac);
			struct secureCmdEntry * ptr1;

    		aos_list_for_each_entry(ptr1, &sgSecureCmdList, datalist)
    		{       
    			if(strcmp(ptr->path, path)==0) 
   				{       
					return -1;
				}
      		}            

			struct secureCmdEntry * data;
			if((data = (struct secureCmdEntry *)malloc(sizeof(struct secureCmdEntry)))==NULL)
			{
				//printk("malloc error");
				return -1;
			}
			memset(data, 0, sizeof(struct secureCmdEntry));
			strncpy(data->path, path,MAX_NAME_STRING_LEN);
			strncpy(data->mac, mac,MAX_NAME_STRING_LEN);
			aos_list_add_tail((struct aos_list_head *)&data->datalist, &sgSecureCmdList);

		}
		while(1)
		{

			char *seperator="\t";
			memset(tmp, 0, sizeof(256));
			strcpy(macbind, "");
			strcpy(macbind, mac);
			strcat(macbind, "\n");
   	   	    readBlock(rbuffer, macbind, "\n", tmp, MAX_LEN);
			//printf("the rbuffer is %s\n", rbuffer);
	   		 //printf("  the ipbind is %s\n", macbind);
	   		 //printf("  the tmp is %s\n", tmp);
			if(strcmp(tmp,"") == 0 || strcmp(tmp, "------------------------") == 0)
			break;

			strcpy(path,"");
			strcpy(mac,"");
	   	    strcpy(path, strtok_r(tmp,seperator,&tmpsave));
			strcpy(mac, strtok_r(NULL,seperator,&tmpsave));

			printf("  path = %s mac = %s \n", path, mac);
			struct secureCmdEntry * ptr;

    		aos_list_for_each_entry(ptr, &sgSecureCmdList, datalist)
    		{       
    			if(strcmp(ptr->path, path)==0) 
   				{       
					return -1;
				}
      		}            

			struct secureCmdEntry * data;
			if((data = (struct secureCmdEntry *)malloc(sizeof(struct secureCmdEntry)))==NULL)
			{
				//printk("malloc error");
				return -1;
			}
			memset(data, 0, sizeof(struct secureCmdEntry));
			strcpy(data->path, path);
			strcpy(data->mac, mac);
			aos_list_add_tail((struct aos_list_head *)&data->datalist, &sgSecureCmdList);
		}
	
		readBlock(rbuffer,"The Policy\n------------------------\n","\t",tmp,MAX_LEN);
		strncpy(policy,tmp,MAX_NAME_STRING_LEN);
		printf ("the policy is %s\n", policy);
		return 0;

	}
	else
	{
		readBlock(rbuffer,"The Policy\n------------------------\n","\t",tmp,MAX_LEN);
		strncpy(policy,tmp,MAX_NAME_STRING_LEN);

		printf ("the policy is %s\n", policy);
		return 0;
	}
	
}

//PPPOE section (added by ricky chen  1/6/07)
int pppoeStart(char *result)
{
	char sbuffer[1024];
	char rbuffer[2048];
	strcpy(sbuffer, "pppoe start");
	sendSystemCli(sbuffer, rbuffer);	
	strcpy(result, rbuffer);
	return 0;
}

int pppoeStop(char *result)
{
	char sbuffer[1024];
	char rbuffer[2048];
	strcpy(sbuffer, "pppoe stop");
	sendSystemCli(sbuffer, rbuffer);	
	strcpy(result, rbuffer);
	return 0;
}

int pppoeUsernameSet(char *name,char *result)
{
	
	char sbuffer[1024];
	char rbuffer[2048];
	strcpy(sbuffer, "pppoe username set");
	strcat(sbuffer, " ");
	strcat(sbuffer, name);
	sendSystemCli(sbuffer, rbuffer);	
	strcpy(result, rbuffer);
	return 0;
}

int pppoePasswordSet(char *password,char *result)
{
	
	char sbuffer[1024];
	char rbuffer[2048];
	strcpy(sbuffer, "pppoe password set");
	strcat(sbuffer, " ");
	strcat(sbuffer, password);
	sendSystemCli(sbuffer, rbuffer);	
	strcpy(result, rbuffer);
	return 0;
}

int pppoeDnsSet(char *dns_ip1,char *dns_ip2,char *result)
{
	char sbuffer[1024];
	char rbuffer[2048];
	strcpy(sbuffer, "pppoe dns set");
	strcat(sbuffer, " ");
	strcat(sbuffer, dns_ip1);
	strcat(sbuffer, " ");
	strcat(sbuffer, dns_ip2);
	sendSystemCli(sbuffer, rbuffer);	
	strcpy(result, rbuffer);
	return 0;
}



int pppoeShow(struct pppoeConfigEntry *pppoe_info,char *result)
{
	char sbuffer[1024];
	char rbuffer[2048];
	char tmp[MAX_LEN];
	strcpy(sbuffer, "pppoe show config");
	sendSystemCli(sbuffer, rbuffer);	
	
	strcpy(tmp,"");
    readBlock(rbuffer, "status :", "\n\tusername", tmp, MAX_LEN);
	strcpy(pppoe_info->status, tmp);
	printf("            status : %s\n", pppoe_info->status, MAX_LEN);
    
	strcpy(tmp,"");
    readBlock(rbuffer, "username : ", "\n", tmp, MAX_LEN);
	strcpy(pppoe_info->username, tmp);
	printf("          username : %s\n", pppoe_info->username);

	strcpy(tmp,"");
    readBlock(rbuffer, "password : ", "\n", tmp, MAX_LEN);
	strcpy(pppoe_info->password, tmp);
	printf("          password : %s\n", pppoe_info->password);

	strcpy(tmp,"");
    readBlock(rbuffer, "dns1 : ", "\n", tmp, MAX_LEN);
	strcpy(pppoe_info->dns1, tmp);
	printf("              dns1 : %s\n", pppoe_info->dns1);

    strcpy(tmp,"");
    readBlock(rbuffer, "dns2 : ", "\n", tmp, MAX_LEN);
	strcpy(pppoe_info->dns2, tmp);
	printf("              dns2 : %s\n", pppoe_info->dns2);

	return 0;

}

//MAC CLONE  (added by ricky chen 01082007)
int macSet(char *dev,char *mac,char *result)
{
	char sbuffer[1024];
	char rbuffer[2048];
	strcpy(sbuffer, "mac set");
	strcat(sbuffer, " ");
	strcat(sbuffer, dev);
	strcat(sbuffer, " ");
	strcat(sbuffer, mac);
	sendSystemCli(sbuffer, rbuffer);	
	strcpy(result, rbuffer);
	return 0;
}
int macBackSet(char *dev,char *result)
{
	char sbuffer[1024];
	char rbuffer[2048];
	strcpy(sbuffer, "mac back set");
	strcat(sbuffer, " ");
	strcat(sbuffer, dev);
	sendSystemCli(sbuffer, rbuffer);	
	strcpy(result, rbuffer);
	return 0;
	
}
int macShowConfig(struct macListEntry macclone_info[],char *result)
{
	char sbuffer[1024];
	char rbuffer[2048];
	char tmp[MAX_LEN];
	char *loc=NULL;
	int i =0;
	int j=0;
	strcpy(sbuffer, "mac show config");
	sendSystemCli(sbuffer, rbuffer);	
	loc=strstr(rbuffer,"\t\t\t ");
	while(loc)
	{
		strcpy(tmp,"");
        readBlock(loc, "\t\t\t "," ", tmp, MAX_LEN);
		strcpy(macclone_info[i].dev,tmp);
		strcpy(tmp,"");
        readBlock(loc+4, " ","  ", tmp, MAX_LEN);
		strcpy(macclone_info[i].mac,tmp);
		strcpy(tmp,"");
        readBlock(loc, "  ","\n", tmp, MAX_LEN);
		strcpy(macclone_info[i].oldmac,tmp);
        i++;
		loc=strstr(loc+4,"\t\t\t ");
		
	}
	printf("the result is :\n");
	printf("Ethernet interface mac information\n");
	printf("------------------------------------\n");
	printf("\t\t\tdev            mac           oldMac\n");
	printf("\t\t\t----------------------------------------\n");
	for(j=0;j<i;j++)
	{
	  printf("\t\t\t%s ", &macclone_info[j].dev, MAX_LEN);
	  printf("%s ", &macclone_info[j].mac, MAX_LEN);
	  printf("%s\n ", &macclone_info[j].oldmac, MAX_LEN);
		
	}
	return 0;

}

// systemconfig (added by ricky chen 01082007)
/*
int systemDns(char *dns,char *result)
{
	char sbuffer[1024];
	char rbuffer[2048];
	strcpy(sbuffer, "system ip dns");
	strcat(sbuffer, " ");
	strcat(sbuffer, dns);
	sendSystemCli(sbuffer, rbuffer);	
	strcpy(result, rbuffer);
	return 0;
	
}
int systemIpAddress(char *interface,char *ip,char *mask,char *result)
{
	char sbuffer[1024];
	char rbuffer[2048];
	strcpy(sbuffer, "system ip address");
	strcat(sbuffer, " ");
	strcat(sbuffer, interface);
	strcat(sbuffer, " ");
	strcat(sbuffer, ip);
	strcat(sbuffer, " ");
	strcat(sbuffer, mask);
	sendSystemCli(sbuffer, rbuffer);	
	strcpy(result, rbuffer);
	return 0;
	
}
int systemUname(char *id,char *result)
{
	char sbuffer[1024];
	char rbuffer[2048];
	strcpy(sbuffer, "system uname");
	strcat(sbuffer, " ");
	strcat(sbuffer, id);
	sendSystemCli(sbuffer, rbuffer);	
	strcpy(result, rbuffer);
	return 0;
	
	
}

int showSystemConfig(struct systemConfigListEntry *system_info,char *result)
{

}
int clearSystemConfig(char *result)
{
}

*/


//BRIDGE SECTION (added by ricky chen 01092007)
int bridgeGroupAdd(char *group_name,char *result)
{
	char sbuffer[1024];
	char rbuffer[2048];
	strcpy(sbuffer, "bridge group add");
	strcat(sbuffer, " ");
	strcat(sbuffer, group_name);
	sendSystemCli(sbuffer, rbuffer);	
	strcpy(result, rbuffer);
	return 0;
	
}

int bridgeGroupDel(char *group_name,char *result) 
{
	char sbuffer[1024];
	char rbuffer[2048];
	strcpy(sbuffer, "bridge group del");
	strcat(sbuffer, " ");
	strcat(sbuffer, group_name);
	sendSystemCli(sbuffer, rbuffer);	
	strcpy(result, rbuffer);
	return 0;
}

int bridgeGroupMemberAdd(char *bridge_name,char *dev,char *result) 
{
	char sbuffer[1024];
	char rbuffer[2048];
	strcpy(sbuffer, "bridge group member add");
	strcat(sbuffer, " ");
	strcat(sbuffer, bridge_name);
	strcat(sbuffer, " ");
	strcat(sbuffer, dev);
	sendSystemCli(sbuffer, rbuffer);	
	strcpy(result, rbuffer);
	return 0;
}

int bridgeGroupIp(char *bridge_name,char *ip,char *result) 
{
	char sbuffer[1024];
	char rbuffer[2048];
	strcpy(sbuffer, "bridge group set ip");
	strcat(sbuffer, " ");
	strcat(sbuffer, bridge_name);
	strcat(sbuffer, " ");
	strcat(sbuffer, ip);
	sendSystemCli(sbuffer, rbuffer);	
	strcpy(result, rbuffer);
	return 0;
}

int bridgeGroupMemberDel(char *bridge_name,char *dev,char *result) 
{
	char sbuffer[1024];
	char rbuffer[2048];
	strcpy(sbuffer, "bridge group member del");
	strcat(sbuffer, " ");
	strcat(sbuffer, bridge_name);
	strcat(sbuffer, " ");
	strcat(sbuffer, dev);
	sendSystemCli(sbuffer, rbuffer);	
	strcpy(result, rbuffer);
	return 0;
}

int bridgeClearConfig(char *result) 
{
	char sbuffer[1024];
	char rbuffer[2048];
	strcpy(sbuffer, "bridge clear config");
	sendSystemCli(sbuffer, rbuffer);	
	strcpy(result, rbuffer);
	return 0;
}

int bridgeGroupMemberIp(char *dev,char *ip,char *result) 
{
	char sbuffer[1024];
	char rbuffer[2048];
	strcpy(sbuffer, "bridge group member set ip");
	strcat(sbuffer, " ");
	strcat(sbuffer, dev);
	strcat(sbuffer, " ");
	strcat(sbuffer, ip);
	sendSystemCli(sbuffer, rbuffer);	
	strcpy(result, rbuffer);
	return 0;
}

int bridgeGroupUp(char *bridge_name,char *result)
{
	char sbuffer[1024];
	char rbuffer[2048];
	strcpy(sbuffer, "bridge group up");
	strcat(sbuffer, " ");
	strcat(sbuffer, bridge_name);
	sendSystemCli(sbuffer, rbuffer);	
	strcpy(result, rbuffer);
	return 0;
}

int bridgeGroupDown(char *bridge_name,char *result) 
{
	char sbuffer[1024];
	char rbuffer[2048];
	strcpy(sbuffer, "bridge group down");
	strcat(sbuffer, " ");
	strcat(sbuffer, bridge_name);
	sendSystemCli(sbuffer, rbuffer);	
	strcpy(result, rbuffer);
	return 0;
}

int bridgeShowConfig(struct bridgeDevEntry bridgedev[],struct bridgeConfigEntry *bridgeconfig_info,char *result)
{
	char sbuffer[1024];
	char rbuffer[2048];
	char tmp[MAX_LEN];
	char *loc=NULL; 
	int i=0;
	int j=0;
	strcpy(sbuffer, "bridge show config");
	sendSystemCli(sbuffer, rbuffer);	
	printf("Mes sended to server: %s\n",sbuffer);
	printf("bridge       information \n");
	printf("----------------------------\n"); 
	
    strcpy(tmp,"");
    readBlock(rbuffer, "bridge name: ","\n", tmp, MAX_LEN);
	strcpy(&bridgeconfig_info->bridgeName, tmp);
	printf("            bridge name: %s\n", &bridgeconfig_info->bridgeName, MAX_LEN);
    
	strcpy(tmp,"");
    readBlock(rbuffer, "ip : ","\n", tmp, MAX_LEN);
	strcpy(&bridgeconfig_info->ip, tmp);
	printf("                     ip: %s\n", &bridgeconfig_info->ip, MAX_LEN);

	strcpy(tmp,"");
    readBlock(rbuffer, "status : ","\n", tmp, MAX_LEN);
	strcpy(&bridgeconfig_info->status, tmp);
	printf("                 status: %s\n", &bridgeconfig_info->status, MAX_LEN);
   
	loc=strstr(rbuffer,"                        ");
    while((loc)&&(i<16)&&(*loc!="\0"))
    {
	  strcpy(tmp,"");
	  readBlock(loc, "                        ","\t", tmp, MAX_LEN);
	  strcpy(bridgedev[i].dev,tmp);
	  strcpy(tmp,"");
	  readBlock(loc, "\t","\n", tmp, MAX_LEN);
	  strcpy(bridgedev[i].ip,tmp);
	  i++;
	  loc+=24;
	  loc=strstr(loc,"                        ");
    }
   printf( "\t\t\tDev\tIP\n");
   printf("\t\t\t----------------------------\n");
   
   for(j=0;j<i;j++)
   {
	printf("\t\t\t%s ", &bridgedev[j].dev, MAX_LEN);
	printf("%s\n ", &bridgedev[j].ip, MAX_LEN);
   }
}


/*Firewall API*/
//macfil API
int fwMacfilAll(char *chain,char *smac,char *dip,char *dmask,char *action,char *log,char *result)
{
	char sbuffer[1024];
	char rbuffer[2048];
	
	// send the data to system CLI daemon
	strcpy(sbuffer, "firewall macfil all");
	strcat(sbuffer, " ");
	strcat(sbuffer,chain);
	strcat(sbuffer, " ");
	strcat(sbuffer,smac);
	strcat(sbuffer, " ");
	strcat(sbuffer,dip);
	strcat(sbuffer, " ");
	strcat(sbuffer,dmask);
	strcat(sbuffer, " ");
	strcat(sbuffer,action);
	strcat(sbuffer, " ");
	strcat(sbuffer,log);
	
	sendSystemCli(sbuffer, rbuffer);	

	// return the return message to the result
	strcpy(result, rbuffer);
	
	return 0;
}

int fwMacfilTcp(char *chain,char *smac,char *sport,char *dip,char *dmask,char *dport,
                char *action,char *log,char *result)
{
	char sbuffer[1024];
	char rbuffer[2048];
	
	// send the data to system CLI daemon
	strcpy(sbuffer, "firewall macfil tcp");
	strcat(sbuffer, " ");
	strcat(sbuffer,chain);
	strcat(sbuffer, " ");
	strcat(sbuffer,smac);
	strcat(sbuffer, " ");
	strcat(sbuffer,sport);
	strcat(sbuffer, " ");
	strcat(sbuffer,dip);
	strcat(sbuffer, " ");
	strcat(sbuffer,dmask);
	strcat(sbuffer, " ");
	strcat(sbuffer,dport);
	strcat(sbuffer, " ");
	strcat(sbuffer,action);
	strcat(sbuffer, " ");
	strcat(sbuffer,log);
	
	sendSystemCli(sbuffer, rbuffer);	

	// return the return message to the result
	strcpy(result, rbuffer);
	
	return 0;
}

int fwMacfilUdp(char *chain,char *smac,char *sport,char *dip,char *dmask,char *dport,
                char *action,char *log,char *result)
{
	char sbuffer[1024];
	char rbuffer[2048];
	
	// send the data to system CLI daemon
	strcpy(sbuffer, "firewall macfil udp");
	strcat(sbuffer, " ");
	strcat(sbuffer,chain);
	strcat(sbuffer, " ");
	strcat(sbuffer,smac);
	strcat(sbuffer, " ");
	strcat(sbuffer,sport);
	strcat(sbuffer, " ");
	strcat(sbuffer,dip);
	strcat(sbuffer, " ");
	strcat(sbuffer,dmask);
	strcat(sbuffer, " ");
	strcat(sbuffer,dport);
	strcat(sbuffer, " ");
	strcat(sbuffer,action);
	strcat(sbuffer, " ");
	strcat(sbuffer,log);
	
	sendSystemCli(sbuffer, rbuffer);	

	// return the return message to the result
	strcpy(result, rbuffer);
	
	return 0;
}

int fwMacfilIcmp(char *chain,char *smac,char *dip,char *dmask,
                char *action,char *log,char *result)
{
	char sbuffer[1024];
	char rbuffer[2048];
	
	// send the data to system CLI daemon
	strcpy(sbuffer, "firewall macfil icmp");
	strcat(sbuffer, " ");
	strcat(sbuffer,chain);
	strcat(sbuffer, " ");
	strcat(sbuffer,smac);
	strcat(sbuffer, " ");
	strcat(sbuffer,dip);
	strcat(sbuffer, " ");
	strcat(sbuffer,dmask);
	strcat(sbuffer, " ");
	strcat(sbuffer,action);
	strcat(sbuffer, " ");
	strcat(sbuffer,log);
	
	sendSystemCli(sbuffer, rbuffer);	

	// return the return message to the result
	strcpy(result, rbuffer);
	
	return 0;
}

int fwMacfilDelAll(char *chain,char *smac,char *dip,char *dmask,char *action,char *log,char *result)
{
	char sbuffer[1024];
	char rbuffer[2048];
	
	// send the data to system CLI daemon
	strcpy(sbuffer, "firewall macfil del all");
	strcat(sbuffer, " ");
	strcat(sbuffer,chain);
	strcat(sbuffer, " ");
	strcat(sbuffer,smac);
	strcat(sbuffer, " ");
	strcat(sbuffer,dip);
	strcat(sbuffer, " ");
	strcat(sbuffer,dmask);
	strcat(sbuffer, " ");
	strcat(sbuffer,action);
	strcat(sbuffer, " ");
	strcat(sbuffer,log);
	
	sendSystemCli(sbuffer, rbuffer);	

	// return the return message to the result
	strcpy(result, rbuffer);
	
	return 0;
}

int fwMacfilDelTcp(char *chain,char *smac,char *sport,char *dip,char *dmask,char *dport,
                char *action,char *log,char *result)
{
	char sbuffer[1024];
	char rbuffer[2048];
	
	// send the data to system CLI daemon
	strcpy(sbuffer, "firewall macfil del tcp");
	strcat(sbuffer, " ");
	strcat(sbuffer,chain);
	strcat(sbuffer, " ");
	strcat(sbuffer,smac);
	strcat(sbuffer, " ");
	strcat(sbuffer,sport);
	strcat(sbuffer, " ");
	strcat(sbuffer,dip);
	strcat(sbuffer, " ");
	strcat(sbuffer,dmask);
	strcat(sbuffer, " ");
	strcat(sbuffer,dport);
	strcat(sbuffer, " ");
	strcat(sbuffer,action);
	strcat(sbuffer, " ");
	strcat(sbuffer,log);
	
	sendSystemCli(sbuffer, rbuffer);	

	// return the return message to the result
	strcpy(result, rbuffer);
	
	return 0;
}

int fwMacfilDelUdp(char *chain,char *smac,char *sport,char *dip,char *dmask,char *dport,
                char *action,char *log,char *result)
{
	char sbuffer[1024];
	char rbuffer[2048];
	
	// send the data to system CLI daemon
	strcpy(sbuffer, "firewall macfil del udp");
	strcat(sbuffer, " ");
	strcat(sbuffer,chain);
	strcat(sbuffer, " ");
	strcat(sbuffer,smac);
	strcat(sbuffer, " ");
	strcat(sbuffer,sport);
	strcat(sbuffer, " ");
	strcat(sbuffer,dip);
	strcat(sbuffer, " ");
	strcat(sbuffer,dmask);
	strcat(sbuffer, " ");
	strcat(sbuffer,dport);
	strcat(sbuffer, " ");
	strcat(sbuffer,action);
	strcat(sbuffer, " ");
	strcat(sbuffer,log);
	
	sendSystemCli(sbuffer, rbuffer);	

	// return the return message to the result
	strcpy(result, rbuffer);
	
	return 0;
}

int fwMacfilDelIcmp(char *chain,char *smac,char *dip,char *dmask,
                char *action,char *log,char *result)
{
	char sbuffer[1024];
	char rbuffer[2048];
	
	// send the data to system CLI daemon
	strcpy(sbuffer, "firewall macfil del icmp");
	strcat(sbuffer, " ");
	strcat(sbuffer,chain);
	strcat(sbuffer, " ");
	strcat(sbuffer,smac);
	strcat(sbuffer, " ");
	strcat(sbuffer,dip);
	strcat(sbuffer, " ");
	strcat(sbuffer,dmask);
	strcat(sbuffer, " ");
	strcat(sbuffer,action);
	strcat(sbuffer, " ");
	strcat(sbuffer,log);
	
	sendSystemCli(sbuffer, rbuffer);	

	// return the return message to the result
	strcpy(result, rbuffer);
	
	return 0;
}

//ipfil API
int fwIpfilAll(char *chain,char *sip,char *smask,char *dip,char *dmask,
                char *action,char *log,char *result)
{
	char sbuffer[1024];
	char rbuffer[2048];
	
	// send the data to system CLI daemon
	strcpy(sbuffer, "firewall ipfil all");
	strcat(sbuffer, " ");
	strcat(sbuffer,chain);
	strcat(sbuffer, " ");
	strcat(sbuffer,sip);
	strcat(sbuffer, " ");
	strcat(sbuffer,smask);
	strcat(sbuffer, " ");
	strcat(sbuffer,dip);
	strcat(sbuffer, " ");
	strcat(sbuffer,dmask);
	strcat(sbuffer, " ");
	strcat(sbuffer,action);
	strcat(sbuffer, " ");
	strcat(sbuffer,log);
	
	sendSystemCli(sbuffer, rbuffer);	

	// return the return message to the result
	strcpy(result, rbuffer);
	
	return 0;
}

int fwIpfilDelAll(char *chain,char *sip,char *smask,char *dip,char *dmask,
                char *action,char *log,char *result)
{
	char sbuffer[1024];
	char rbuffer[2048];
	
	// send the data to system CLI daemon
	strcpy(sbuffer, "firewall ipfil del all");
	strcat(sbuffer, " ");
	strcat(sbuffer,chain);
	strcat(sbuffer, " ");
	strcat(sbuffer,sip);
	strcat(sbuffer, " ");
	strcat(sbuffer,smask);
	strcat(sbuffer, " ");
	strcat(sbuffer,dip);
	strcat(sbuffer, " ");
	strcat(sbuffer,dmask);
	strcat(sbuffer, " ");
	strcat(sbuffer,action);
	strcat(sbuffer, " ");
	strcat(sbuffer,log);
	
	sendSystemCli(sbuffer, rbuffer);	

	// return the return message to the result
	strcpy(result, rbuffer);
	
	return 0;
}

int fwIpfilTcp(char *chain,char *sip,char *smask,char *sport,char *dip,char *dmask,
                char *dport,char *action,char *log,char *result)
{
	char sbuffer[1024];
	char rbuffer[2048];
	
	// send the data to system CLI daemon
	strcpy(sbuffer, "firewall ipfil tcp");
	strcat(sbuffer, " ");
	strcat(sbuffer,chain);
	strcat(sbuffer, " ");
	strcat(sbuffer,sip);
	strcat(sbuffer, " ");
	strcat(sbuffer,smask);
	strcat(sbuffer, " ");
	strcat(sbuffer,sport);
	strcat(sbuffer, " ");
	strcat(sbuffer,dip);
	strcat(sbuffer, " ");
	strcat(sbuffer,dmask);
	strcat(sbuffer, " ");
	strcat(sbuffer,dport);
	strcat(sbuffer, " ");
	strcat(sbuffer,action);
	strcat(sbuffer, " ");
	strcat(sbuffer,log);
	
	sendSystemCli(sbuffer, rbuffer);	

	// return the return message to the result
	strcpy(result, rbuffer);
	
	return 0;
}

int fwIpfilDelTcp(char *chain,char *sip,char *smask,char *sport,char *dip,char *dmask,
                char *dport,char *action,char *log,char *result)
{
	char sbuffer[1024];
	char rbuffer[2048];
	
	// send the data to system CLI daemon
	strcpy(sbuffer, "firewall ipfil del tcp");
	strcat(sbuffer, " ");
	strcat(sbuffer,chain);
	strcat(sbuffer, " ");
	strcat(sbuffer,sip);
	strcat(sbuffer, " ");
	strcat(sbuffer,smask);
	strcat(sbuffer, " ");
	strcat(sbuffer,sport);
	strcat(sbuffer, " ");
	strcat(sbuffer,dip);
	strcat(sbuffer, " ");
	strcat(sbuffer,dmask);
	strcat(sbuffer, " ");
	strcat(sbuffer,dport);
	strcat(sbuffer, " ");
	strcat(sbuffer,action);
	strcat(sbuffer, " ");
	strcat(sbuffer,log);
	
	sendSystemCli(sbuffer, rbuffer);	

	// return the return message to the result
	strcpy(result, rbuffer);
	
	return 0;
}

int fwIpfilUdp(char *chain,char *sip,char *smask,char *sport,char *dip,char *dmask,
                char *dport,char *action,char *log,char *result)
{
	char sbuffer[1024];
	char rbuffer[2048];
	
	// send the data to system CLI daemon
	strcpy(sbuffer, "firewall ipfil udp");
	strcat(sbuffer, " ");
	strcat(sbuffer,chain);
	strcat(sbuffer, " ");
	strcat(sbuffer,sip);
	strcat(sbuffer, " ");
	strcat(sbuffer,smask);
	strcat(sbuffer, " ");
	strcat(sbuffer,sport);
	strcat(sbuffer, " ");
	strcat(sbuffer,dip);
	strcat(sbuffer, " ");
	strcat(sbuffer,dmask);
	strcat(sbuffer, " ");
	strcat(sbuffer,dport);
	strcat(sbuffer, " ");
	strcat(sbuffer,action);
	strcat(sbuffer, " ");
	strcat(sbuffer,log);
	
	sendSystemCli(sbuffer, rbuffer);	

	// return the return message to the result
	strcpy(result, rbuffer);
	
	return 0;
}

int fwIpfilDelUdp(char *chain,char *sip,char *smask,char *sport,char *dip,char *dmask,
                char *dport,char *action,char *log,char *result)
{
	char sbuffer[1024];
	char rbuffer[2048];
	
	// send the data to system CLI daemon
	strcpy(sbuffer, "firewall ipfil del udp");
	strcat(sbuffer, " ");
	strcat(sbuffer,chain);
	strcat(sbuffer, " ");
	strcat(sbuffer,sip);
	strcat(sbuffer, " ");
	strcat(sbuffer,smask);
	strcat(sbuffer, " ");
	strcat(sbuffer,sport);
	strcat(sbuffer, " ");
	strcat(sbuffer,dip);
	strcat(sbuffer, " ");
	strcat(sbuffer,dmask);
	strcat(sbuffer, " ");
	strcat(sbuffer,dport);
	strcat(sbuffer, " ");
	strcat(sbuffer,action);
	strcat(sbuffer, " ");
	strcat(sbuffer,log);
	
	sendSystemCli(sbuffer, rbuffer);	

	// return the return message to the result
	strcpy(result, rbuffer);
	
	return 0;
}

int fwIpfilIcmp(char *chain,char *sip,char *smask,char *dip,char *dmask,
                char *action,char *log,char *result)
{
	char sbuffer[1024];
	char rbuffer[2048];
	
	// send the data to system CLI daemon
	strcpy(sbuffer, "firewall ipfil icmp");
	strcat(sbuffer, " ");
	strcat(sbuffer,chain);
	strcat(sbuffer, " ");
	strcat(sbuffer,sip);
	strcat(sbuffer, " ");
	strcat(sbuffer,smask);
	strcat(sbuffer, " ");
	strcat(sbuffer,dip);
	strcat(sbuffer, " ");
	strcat(sbuffer,dmask);
	strcat(sbuffer, " ");
	strcat(sbuffer,action);
	strcat(sbuffer, " ");
	strcat(sbuffer,log);
	
	sendSystemCli(sbuffer, rbuffer);	

	// return the return message to the result
	strcpy(result, rbuffer);
	
	return 0;
}

int fwIpfilDelIcmp(char *chain,char *sip,char *smask,char *dip,char *dmask,
                char *action,char *log,char *result)
{
	char sbuffer[1024];
	char rbuffer[2048];
	
	// send the data to system CLI daemon
	strcpy(sbuffer, "firewall ipfil del icmp");
	strcat(sbuffer, " ");
	strcat(sbuffer,chain);
	strcat(sbuffer, " ");
	strcat(sbuffer,sip);
	strcat(sbuffer, " ");
	strcat(sbuffer,smask);
	strcat(sbuffer, " ");
	strcat(sbuffer,dip);
	strcat(sbuffer, " ");
	strcat(sbuffer,dmask);
	strcat(sbuffer, " ");
	strcat(sbuffer,action);
	strcat(sbuffer, " ");
	strcat(sbuffer,log);
	
	sendSystemCli(sbuffer, rbuffer);	

	// return the return message to the result
	strcpy(result, rbuffer);
	
	return 0;
}

int fwIpfilStateNew(char *chain,char *sip,char *smask,char *dip,char *dmask,
                char *action,char *log,char *result)
{
	char sbuffer[1024];
	char rbuffer[2048];
	
	// send the data to system CLI daemon
	strcpy(sbuffer, "firewall ipfil state new");
	strcat(sbuffer, " ");
	strcat(sbuffer,chain);
	strcat(sbuffer, " ");
	strcat(sbuffer,sip);
	strcat(sbuffer, " ");
	strcat(sbuffer,smask);
	strcat(sbuffer, " ");
	strcat(sbuffer,dip);
	strcat(sbuffer, " ");
	strcat(sbuffer,dmask);
	strcat(sbuffer, " ");
	strcat(sbuffer,action);
	strcat(sbuffer, " ");
	strcat(sbuffer,log);
	
	sendSystemCli(sbuffer, rbuffer);	

	// return the return message to the result
	strcpy(result, rbuffer);
	
	return 0;
}

int fwIpfilDelStateNew(char *chain,char *sip,char *smask,char *dip,char *dmask,
                char *action,char *log,char *result)
{
	char sbuffer[1024];
	char rbuffer[2048];
	
	// send the data to system CLI daemon
	strcpy(sbuffer, "firewall ipfil del state new");
	strcat(sbuffer, " ");
	strcat(sbuffer,chain);
	strcat(sbuffer, " ");
	strcat(sbuffer,sip);
	strcat(sbuffer, " ");
	strcat(sbuffer,smask);
	strcat(sbuffer, " ");
	strcat(sbuffer,dip);
	strcat(sbuffer, " ");
	strcat(sbuffer,dmask);
	strcat(sbuffer, " ");
	strcat(sbuffer,action);
	strcat(sbuffer, " ");
	strcat(sbuffer,log);
	
	sendSystemCli(sbuffer, rbuffer);	

	// return the return message to the result
	strcpy(result, rbuffer);
	
	return 0;
}

int fwIpfilStateAck(char *chain,char *sip,char *smask,char *dip,char *dmask,
                char *action,char *log,char *result)
{
	char sbuffer[1024];
	char rbuffer[2048];
	
	// send the data to system CLI daemon
	strcpy(sbuffer, "firewall ipfil state ack");
	strcat(sbuffer, " ");
	strcat(sbuffer,chain);
	strcat(sbuffer, " ");
	strcat(sbuffer,sip);
	strcat(sbuffer, " ");
	strcat(sbuffer,smask);
	strcat(sbuffer, " ");
	strcat(sbuffer,dip);
	strcat(sbuffer, " ");
	strcat(sbuffer,dmask);
	strcat(sbuffer, " ");
	strcat(sbuffer,action);
	strcat(sbuffer, " ");
	strcat(sbuffer,log);
	
	sendSystemCli(sbuffer, rbuffer);	

	// return the return message to the result
	strcpy(result, rbuffer);
	
	return 0;
}

int fwIpfilDelStateAck(char *chain,char *sip,char *smask,char *dip,char *dmask,
                char *action,char *log,char *result)
{
	char sbuffer[1024];
	char rbuffer[2048];
	
	// send the data to system CLI daemon
	strcpy(sbuffer, "firewall ipfil del state ack");
	strcat(sbuffer, " ");
	strcat(sbuffer,chain);
	strcat(sbuffer, " ");
	strcat(sbuffer,sip);
	strcat(sbuffer, " ");
	strcat(sbuffer,smask);
	strcat(sbuffer, " ");
	strcat(sbuffer,dip);
	strcat(sbuffer, " ");
	strcat(sbuffer,dmask);
	strcat(sbuffer, " ");
	strcat(sbuffer,action);
	strcat(sbuffer, " ");
	strcat(sbuffer,log);
	
	sendSystemCli(sbuffer, rbuffer);	

	// return the return message to the result
	strcpy(result, rbuffer);
	
	return 0;
}

//timefil API
int fwTimefil(char *chain,char *proto,char *sip,char *smask,char *dip,char *dmask,
              char *action,char *beginTime,char *endTime,
              char *week,char *log,char *result)
{
	char sbuffer[1024];
	char rbuffer[2048];
	
	// send the data to system CLI daemon
	strcpy(sbuffer, "firewall timefil");
	strcat(sbuffer, " ");
	strcat(sbuffer,chain);
	strcat(sbuffer, " ");
	strcat(sbuffer,proto);
	strcat(sbuffer, " ");
	strcat(sbuffer,sip);
	strcat(sbuffer, " ");
	strcat(sbuffer,smask);
	strcat(sbuffer, " ");
	strcat(sbuffer,dip);
	strcat(sbuffer, " ");
	strcat(sbuffer,dmask);
	strcat(sbuffer, " ");
	strcat(sbuffer,action);
	strcat(sbuffer, " ");
	strcat(sbuffer,beginTime);
	strcat(sbuffer, " ");
	strcat(sbuffer,endTime);
	strcat(sbuffer, " ");
	strcat(sbuffer,week);
	strcat(sbuffer, " ");
	strcat(sbuffer,log);
	
	sendSystemCli(sbuffer, rbuffer);	

	// return the return message to the result
	strcpy(result, rbuffer);
	
	return 0;
}

int fwDelTimefil(char *chain,char *proto,char *sip,char *smask,char *dip,char *dmask,
              char *action,char *beginTime,char *endTime,
              char *week,char *log,char *result)
{
	char sbuffer[1024];
	char rbuffer[2048];
	
	// send the data to system CLI daemon
	strcpy(sbuffer, "firewall del timefil");
	strcat(sbuffer, " ");
	strcat(sbuffer,chain);
	strcat(sbuffer, " ");
	strcat(sbuffer,proto);
	strcat(sbuffer, " ");
	strcat(sbuffer,sip);
	strcat(sbuffer, " ");
	strcat(sbuffer,smask);
	strcat(sbuffer, " ");
	strcat(sbuffer,dip);
	strcat(sbuffer, " ");
	strcat(sbuffer,dmask);
	strcat(sbuffer, " ");
	strcat(sbuffer,action);
	strcat(sbuffer, " ");
	strcat(sbuffer,beginTime);
	strcat(sbuffer, " ");
	strcat(sbuffer,endTime);
	strcat(sbuffer, " ");
	strcat(sbuffer,week);
	strcat(sbuffer, " ");
	strcat(sbuffer,log);
	
	sendSystemCli(sbuffer, rbuffer);	

	// return the return message to the result
	strcpy(result, rbuffer);
	
	return 0;
}

//NAT API
int fwNatSnat(char *interface,char *sip,char *smask,char *to,char *result)
{
	char sbuffer[1024];
	char rbuffer[2048];
	
	// send the data to system CLI daemon
	strcpy(sbuffer, "firewall snat");
	strcat(sbuffer, " ");
	strcat(sbuffer,interface);
	strcat(sbuffer, " ");
	strcat(sbuffer,sip);
	strcat(sbuffer, " ");
	strcat(sbuffer,smask);
	strcat(sbuffer, " ");
	strcat(sbuffer,to);
	
	sendSystemCli(sbuffer, rbuffer);	

	// return the return message to the result
	strcpy(result, rbuffer);
	
	return 0;
}

int fwNatDelSnat(char *interface,char *sip,char *smask,char *to,char *result)
{
	char sbuffer[1024];
	char rbuffer[2048];
	
	// send the data to system CLI daemon
	strcpy(sbuffer, "firewall del snat");
	strcat(sbuffer, " ");
	strcat(sbuffer,interface);
	strcat(sbuffer, " ");
	strcat(sbuffer,sip);
	strcat(sbuffer, " ");
	strcat(sbuffer,smask);
	strcat(sbuffer, " ");
	strcat(sbuffer,to);
	
	sendSystemCli(sbuffer, rbuffer);	

	// return the return message to the result
	strcpy(result, rbuffer);
	
	return 0;
}

int fwNatMasq(char *interface,char *sip,char *smask,char *result)
{
	char sbuffer[1024];
	char rbuffer[2048];
	
	// send the data to system CLI daemon
	strcpy(sbuffer, "firewall masq");
	strcat(sbuffer, " ");
	strcat(sbuffer,interface);
	strcat(sbuffer, " ");
	strcat(sbuffer,sip);
	strcat(sbuffer, " ");
	strcat(sbuffer,smask);
	
	sendSystemCli(sbuffer, rbuffer);	

	// return the return message to the result
	strcpy(result, rbuffer);
	
	return 0;
}

int fwNatDelMasq(char *interface,char *sip,char *smask,char *result)
{
	char sbuffer[1024];
	char rbuffer[2048];
	
	// send the data to system CLI daemon
	strcpy(sbuffer, "firewall del masq");
	strcat(sbuffer, " ");
	strcat(sbuffer,interface);
	strcat(sbuffer, " ");
	strcat(sbuffer,sip);
	strcat(sbuffer, " ");
	strcat(sbuffer,smask);
	
	sendSystemCli(sbuffer, rbuffer);	

	// return the return message to the result
	strcpy(result, rbuffer);
	
	return 0;
}

int fwNatDnatPort(char *interface,char *proto,char *sip,char *sport,
                  char *dip,char *dport,char *result)
{
	char sbuffer[1024];
	char rbuffer[2048];
	
	// send the data to system CLI daemon
	strcpy(sbuffer, "firewall dnat port");
	strcat(sbuffer, " ");
	strcat(sbuffer,interface);
	strcat(sbuffer, " ");
	strcat(sbuffer,proto);
	strcat(sbuffer, " ");
	strcat(sbuffer,sip);
	strcat(sbuffer, " ");
	strcat(sbuffer,sport);
	strcat(sbuffer, " ");
	strcat(sbuffer,dip);
	strcat(sbuffer, " ");
	strcat(sbuffer,dport);
	
	sendSystemCli(sbuffer, rbuffer);	

	// return the return message to the result
	strcpy(result, rbuffer);
	
	return 0;
}

int fwNatDelDnatPort(char *interface,char *proto,char *sip,char *sport,
                  char *dip,char *dport,char *result)
{
	char sbuffer[1024];
	char rbuffer[2048];
	
	// send the data to system CLI daemon
	strcpy(sbuffer, "firewall del dnat port");
	strcat(sbuffer, " ");
	strcat(sbuffer,interface);
	strcat(sbuffer, " ");
	strcat(sbuffer,proto);
	strcat(sbuffer, " ");
	strcat(sbuffer,sip);
	strcat(sbuffer, " ");
	strcat(sbuffer,sport);
	strcat(sbuffer, " ");
	strcat(sbuffer,dip);
	strcat(sbuffer, " ");
	strcat(sbuffer,dport);
	
	sendSystemCli(sbuffer, rbuffer);	

	// return the return message to the result
	strcpy(result, rbuffer);
	
	return 0;
}

int fwNatDnatIP(char *interface,char *proto,char *sip,char *dip,char *result)
{
	char sbuffer[1024];
	char rbuffer[2048];
	
	// send the data to system CLI daemon
	strcpy(sbuffer, "firewall dnat ip");
	strcat(sbuffer, " ");
	strcat(sbuffer,interface);
	strcat(sbuffer, " ");
	strcat(sbuffer,proto);
	strcat(sbuffer, " ");
	strcat(sbuffer,sip);
	strcat(sbuffer, " ");
	strcat(sbuffer,dip);
	
	sendSystemCli(sbuffer, rbuffer);	

	// return the return message to the result
	strcpy(result, rbuffer);
	
	return 0;
}

int fwNatDelDnatIP(char *interface,char *proto,char *sip,char *dip,char *result)
{
	char sbuffer[1024];
	char rbuffer[2048];
	
	// send the data to system CLI daemon
	strcpy(sbuffer, "firewall del dnat ip");
	strcat(sbuffer, " ");
	strcat(sbuffer,interface);
	strcat(sbuffer, " ");
	strcat(sbuffer,proto);
	strcat(sbuffer, " ");
	strcat(sbuffer,sip);
	strcat(sbuffer, " ");
	strcat(sbuffer,dip);
	
	sendSystemCli(sbuffer, rbuffer);	

	// return the return message to the result
	strcpy(result, rbuffer);
	
	return 0;
}

int fwNatRedi(char *interface,char *proto,char *fromport,char *toport,char *result)
{
	char sbuffer[1024];
	char rbuffer[2048];
	
	// send the data to system CLI daemon
	strcpy(sbuffer, "firewall redi");
	strcat(sbuffer, " ");
	strcat(sbuffer,interface);
	strcat(sbuffer, " ");
	strcat(sbuffer,proto);
	strcat(sbuffer, " ");
	strcat(sbuffer,fromport);
	strcat(sbuffer, " ");
	strcat(sbuffer,toport);
	
	sendSystemCli(sbuffer, rbuffer);	

	// return the return message to the result
	strcpy(result, rbuffer);
	
	return 0;
}

int fwNatDelRedi(char *interface,char *proto,char *fromport,char *toport,char *result)
{
	char sbuffer[1024];
	char rbuffer[2048];
	
	// send the data to system CLI daemon
	strcpy(sbuffer, "firewall del redi");
	strcat(sbuffer, " ");
	strcat(sbuffer,interface);
	strcat(sbuffer, " ");
	strcat(sbuffer,proto);
	strcat(sbuffer, " ");
	strcat(sbuffer,fromport);
	strcat(sbuffer, " ");
	strcat(sbuffer,toport);
	
	sendSystemCli(sbuffer, rbuffer);	

	// return the return message to the result
	strcpy(result, rbuffer);
	
	return 0;
}

//Blacklist API
int fwBlacklistLoadFile(char *fileName,char *result)
{
	char sbuffer[1024];
	char rbuffer[2048];
	
	// send the data to system CLI daemon
	strcpy(sbuffer, "firewall blacklist load file");
	strcat(sbuffer, " ");
	strcat(sbuffer,fileName);
	
	sendSystemCli(sbuffer, rbuffer);	

	// return the return message to the result
	strcpy(result, rbuffer);
	
	return 0;
}

int fwBlacklistRemoveFile(char *fileName,char *result)
{
	char sbuffer[1024];
	char rbuffer[2048];
	
	// send the data to system CLI daemon
	strcpy(sbuffer, "firewall blacklist remove file");
	strcat(sbuffer, " ");
	strcat(sbuffer,fileName);
	
	sendSystemCli(sbuffer, rbuffer);	

	// return the return message to the result
	strcpy(result, rbuffer);
	
	return 0;
}

int fwBlacklistLoadUrl(char *fileName,char *result)
{
	char sbuffer[1024];
	char rbuffer[2048];
	
	// send the data to system CLI daemon
	strcpy(sbuffer, "firewall blacklist load url");
	strcat(sbuffer, " ");
	strcat(sbuffer,fileName);
	
	sendSystemCli(sbuffer, rbuffer);	

	// return the return message to the result
	strcpy(result, rbuffer);
	
	return 0;
}

int fwBlacklistRemoveUrl(char *fileName,char *result)
{
	char sbuffer[1024];
	char rbuffer[2048];
	
	// send the data to system CLI daemon
	strcpy(sbuffer, "firewall blacklist remove url");
	strcat(sbuffer, " ");
	strcat(sbuffer,fileName);
	
	sendSystemCli(sbuffer, rbuffer);	

	// return the return message to the result
	strcpy(result, rbuffer);
	
	return 0;
}

//Disk Quota Section (added by Ricky Chen 20070111)
int diskMgrQuotaStatus(char *status,char *partition,char *result)
{
	char sbuffer[1024];
	char rbuffer[2048];
	strcpy(sbuffer, "disk mgr quota status");
	strcat(sbuffer, " ");
	strcat(sbuffer,status);
	strcat(sbuffer, " ");
	strcat(sbuffer,partition);
	sendSystemCli(sbuffer, rbuffer);	
	strcpy(result, rbuffer);
	return 0;
}

int diskMgrQuotaList(char *result)
{
	char sbuffer[1024];
	char rbuffer[2048];
	strcpy(sbuffer, "disk mgr quota list");
	sendSystemCli(sbuffer, rbuffer);	
	strcpy(result, rbuffer);
	return 0;
}

int diskMgrQuotaSet(char *username,char *size,char *mountpoint,char *result)
{
	char sbuffer[1024];
	char rbuffer[2048];
	strcpy(sbuffer, "disk mgr quota set");
	strcat(sbuffer, " ");
	strcat(sbuffer,username);
	strcat(sbuffer, " ");
	strcat(sbuffer,size);
	strcat(sbuffer, " ");
	strcat(sbuffer,mountpoint);
	sendSystemCli(sbuffer, rbuffer);	
	strcpy(result, rbuffer);
	return 0;
}

int diskMgrQuotaDelete(char *username,char *partition,char *result)
{
	char sbuffer[1024];
	char rbuffer[2048];
	strcpy(sbuffer, "disk mgr quota delete");
	strcat(sbuffer, " ");
	strcat(sbuffer,username);
	strcat(sbuffer, " ");
	strcat(sbuffer,partition);
	sendSystemCli(sbuffer, rbuffer);	
	strcpy(result, rbuffer);
	return 0;
}

int diskMgrQuotaShow(char *starttime,char *endtime,char *times,char *result)
{
	char sbuffer[1024];
	char rbuffer[2048];
	strcpy(sbuffer, "disk mgr quota show");
	strcat(sbuffer, " ");
	strcat(sbuffer,starttime);
	strcat(sbuffer, " ");
	strcat(sbuffer,endtime);
	strcat(sbuffer, " ");
	strcat(sbuffer,times);
	sendSystemCli(sbuffer, rbuffer);	
//	strcpy(result, rbuffer);
	return 0;
}

int diskMgrQuotaStatusShow(char *result)
{
	char sbuffer[1024];
	char rbuffer[2048];
	strcpy(sbuffer, "disk mgr quota status show");
	sendSystemCli(sbuffer, rbuffer);	
	return 0;
}















//config API
int firewallShowConfig(struct fwFilterListEntry *filter,struct fwNatListEntry *nat,char *result)
{
	char tmp[128];
	
	char type[32];
	char beginTime[32];   
	char endTime[32];   
	char week[32]; 
	char chain[32];
	char proto[32];
	char sip[32];
	char smask[32];
	char smac[32];
	char sport[32];
	char dip[32];
	char dmask[32];
	char dport[32];
	char action[32];
	char rate[32];
	char log[32];
	char interface[32];
	char to[32];	
	char fromport[32];	
	char toport[32];	

//	char cmd[32];
	char* tmpsave;
	char fwPrefix[64];
	char subPrefix[32];

	char sbuffer[1024];
	char rbuffer[2048];

	strcpy(sbuffer, "firewall show config");
	sendSystemCli(sbuffer, rbuffer);

	char* curPos = rbuffer+13;
    int ret = 0;
	

	struct aos_list_head sgFwFilterList;
	struct aos_list_head sgFwNatList;
	
	AOS_INIT_LIST_HEAD(&sgFwNatList);	
	AOS_INIT_LIST_HEAD(&sgFwFilterList);	
	
    struct fwFilterListEntry * fil_ptr;
    struct fwFilterListEntry * fil_tmpptr;
    
	aos_list_for_each_entry_safe(fil_ptr, fil_tmpptr, &sgFwFilterList, datalist)
    {       
       	aos_list_del(&fil_ptr->datalist);
        free(fil_ptr);
    }
	
    struct fwNatListEntry * nat_ptr;
    struct fwNatListEntry * nat_tmpptr;
	aos_list_for_each_entry_safe(nat_ptr, nat_tmpptr, &sgFwNatList, datalist)
    {       
       	aos_list_del(&nat_ptr->datalist);
        free(nat_ptr);
    }	
   
   	while(!ret)
   { 	
		ret = readBlock(curPos, "<Cmd>", "</Cmd>", tmp,128);
		curPos += (strlen(tmp)+6);				
		
		char* tmpStr = strtok_r(tmp, " ", &tmpsave);		
		if(tmpStr)
		{
			
//		strcpy(fwPrefix, strtok_r(tmp, " ", &tmpsave));
		strcpy(fwPrefix, tmpStr);
		if(!strcmp(fwPrefix,"firewall"))
		{
			strcpy(subPrefix, strtok_r(NULL, " ", &tmpsave));					
			if(!strcmp(subPrefix,"ipfil"))
			{
				strcpy(subPrefix, strtok_r(NULL, " ", &tmpsave));
				if(!strcmp(subPrefix, "all"))
				{
					strcpy(chain,strtok_r(NULL, " ", &tmpsave));
					strcpy(sip,strtok_r(NULL, " ", &tmpsave));
					strcpy(smask,strtok_r(NULL, " ", &tmpsave));
					strcpy(dip,strtok_r(NULL, " ", &tmpsave));
					strcpy(dmask,strtok_r(NULL, " ", &tmpsave));
					strcpy(action,strtok_r(NULL, " ", &tmpsave));
					strcpy(log,strtok_r(NULL, " ", &tmpsave));
					strcpy(type,"filter");
				}
				else if(!strcmp(subPrefix, "tcp"))
				{
					strcpy(chain,strtok_r(NULL, " ", &tmpsave));
					strcpy(sip,strtok_r(NULL, " ", &tmpsave));
					strcpy(smask,strtok_r(NULL, " ", &tmpsave));
					strcpy(dip,strtok_r(NULL, " ", &tmpsave));
					strcpy(dmask,strtok_r(NULL, " ", &tmpsave));
					strcpy(action,strtok_r(NULL, " ", &tmpsave));
					strcpy(log,strtok_r(NULL, " ", &tmpsave));
					strcpy(type,"filter");
				}
				else if(!strcmp(subPrefix, "udp"))
				{
					strcpy(chain,strtok_r(NULL, " ", &tmpsave));
					strcpy(sip,strtok_r(NULL, " ", &tmpsave));
					strcpy(smask,strtok_r(NULL, " ", &tmpsave));
					strcpy(dip,strtok_r(NULL, " ", &tmpsave));
					strcpy(dmask,strtok_r(NULL, " ", &tmpsave));
					strcpy(action,strtok_r(NULL, " ", &tmpsave));
					strcpy(log,strtok_r(NULL, " ", &tmpsave));
					strcpy(type,"filter");
				}
				else if(!strcmp(subPrefix, "icmp"))
				{
					strcpy(chain,strtok_r(NULL, " ", &tmpsave));
					strcpy(sip,strtok_r(NULL, " ", &tmpsave));
					strcpy(smask,strtok_r(NULL, " ", &tmpsave));
					strcpy(dip,strtok_r(NULL, " ", &tmpsave));
					strcpy(dmask,strtok_r(NULL, " ", &tmpsave));
					strcpy(action,strtok_r(NULL, " ", &tmpsave));
					strcpy(log,strtok_r(NULL, " ", &tmpsave));
					strcpy(type,"filter");			
				}
				else if(!strcmp(subPrefix, "state"))
				{
					strcpy(subPrefix,strtok_r(NULL, " ", &tmpsave));
					if(!strcmp(subPrefix, "new"))
					{				
						strcpy(chain,strtok_r(NULL, " ", &tmpsave));
						strcpy(sip,strtok_r(NULL, " ", &tmpsave));
						strcpy(smask,strtok_r(NULL, " ", &tmpsave));
						strcpy(dip,strtok_r(NULL, " ", &tmpsave));
						strcpy(dmask,strtok_r(NULL, " ", &tmpsave));
						strcpy(action,strtok_r(NULL, " ", &tmpsave));
						strcpy(log,strtok_r(NULL, " ", &tmpsave));
						strcpy(type,"filter");
					}
					else if(!strcmp(subPrefix, "ack"))
					{
						strcpy(chain,strtok_r(NULL, " ", &tmpsave));
						strcpy(sip,strtok_r(NULL, " ", &tmpsave));
						strcpy(smask,strtok_r(NULL, " ", &tmpsave));
						strcpy(dip,strtok_r(NULL, " ", &tmpsave));
						strcpy(dmask,strtok_r(NULL, " ", &tmpsave));
						strcpy(action,strtok_r(NULL, " ", &tmpsave));
						strcpy(log,strtok_r(NULL, " ", &tmpsave));
						strcpy(type,"filter");
					}
				}
			}
		    else if(!strcmp(subPrefix, "macfil"))
		    {	
		    	strcpy(subPrefix,strtok_r(NULL, " ", &tmpsave));
		    	if(!strcmp(subPrefix, "all"))
		    	{
		    		strcpy(chain,strtok_r(NULL, " ", &tmpsave));
		    		strcpy(smac,strtok_r(NULL, " ", &tmpsave));
		    		strcpy(dip,strtok_r(NULL, " ", &tmpsave));
		    		strcpy(dmask,strtok_r(NULL, " ", &tmpsave));
		    		strcpy(action,strtok_r(NULL, " ", &tmpsave));
		    		strcpy(log,strtok_r(NULL, " ", &tmpsave));
		    		strcpy(type,"filter");
		    	}
		    	else if(!strcmp(subPrefix, "tcp"))
		    	{
		    			
		    		strcpy(chain,strtok_r(NULL, " ", &tmpsave));
		    		strcpy(smac,strtok_r(NULL, " ", &tmpsave));
		    		strcpy(dip,strtok_r(NULL, " ", &tmpsave));
		    		strcpy(dmask,strtok_r(NULL, " ", &tmpsave));
		    		strcpy(action,strtok_r(NULL, " ", &tmpsave));
		    		strcpy(log,strtok_r(NULL, " ", &tmpsave));
					strcpy(type,"filter");	    	
		    	}
		    	else if(!strcmp(subPrefix, "udp"))
		    	{
		    			
		    		strcpy(chain,strtok_r(NULL, " ", &tmpsave));
		    		strcpy(smac,strtok_r(NULL, " ", &tmpsave));
		    		strcpy(dip,strtok_r(NULL, " ", &tmpsave));
		    		strcpy(dmask,strtok_r(NULL, " ", &tmpsave));
		    		strcpy(action,strtok_r(NULL, " ", &tmpsave));
		    		strcpy(log,strtok_r(NULL, " ", &tmpsave));
		    		strcpy(type,"filter");
		    	}
		    	else if(!strcmp(subPrefix, "icmp"))
		    	{
		    			
		    		strcpy(chain,strtok_r(NULL, " ", &tmpsave));
		    		strcpy(smac,strtok_r(NULL, " ", &tmpsave));
		    		strcpy(dip,strtok_r(NULL, " ", &tmpsave));
		    		strcpy(dmask,strtok_r(NULL, " ", &tmpsave));
		    		strcpy(action,strtok_r(NULL, " ", &tmpsave));
		    		strcpy(log,strtok_r(NULL, " ", &tmpsave));
		    		strcpy(type,"filter");
		    	}
		    }
		    else if(!strcmp(subPrefix, "snat"))
		    {
		    		
		    	strcpy(interface,strtok_r(NULL, " ", &tmpsave));
		    	strcpy(sip,strtok_r(NULL, " ", &tmpsave));
		    	strcpy(smask,strtok_r(NULL, " ", &tmpsave));
		    	strcpy(to,strtok_r(NULL, " ", &tmpsave));
		    	strcpy(type,"nat");
		    }
		    else if(!strcmp(subPrefix, "masq"))
		    {
		    		
		    	strcpy(interface,strtok_r(NULL, " ", &tmpsave));
		    	strcpy(sip,strtok_r(NULL, " ", &tmpsave));
		    	strcpy(smask,strtok_r(NULL, " ", &tmpsave));
		    	strcpy(type,"nat");
		    }
		    else if(!strcmp(subPrefix, "dnat"))
		    {
		    	
		    	strcpy(subPrefix,strtok_r(NULL, " ", &tmpsave));
		    	if(!strcmp(subPrefix, "ip"))
		    	{
		    			
		    		strcpy(interface,strtok_r(NULL, " ", &tmpsave));
		    		strcpy(proto,strtok_r(NULL, " ", &tmpsave));
		    		strcpy(sip,strtok_r(NULL, " ", &tmpsave));
		    		strcpy(smask,strtok_r(NULL, " ", &tmpsave));
		    		strcpy(dip,strtok_r(NULL, " ", &tmpsave));
		    		strcpy(type,"nat");
		    	}
		    	else if(!strcmp(subPrefix, "port"))
		    	{
		    			
		    		strcpy(interface,strtok_r(NULL, " ", &tmpsave));
		    		strcpy(proto,strtok_r(NULL, " ", &tmpsave));
		    		strcpy(sip,strtok_r(NULL, " ", &tmpsave));
		    		strcpy(smask,strtok_r(NULL, " ", &tmpsave));
		    		strcpy(sport,strtok_r(NULL, " ", &tmpsave));
		    		strcpy(dip,strtok_r(NULL, " ", &tmpsave));
		    		strcpy(dport,strtok_r(NULL, " ", &tmpsave));
		    		strcpy(type,"nat");
		    	}
		    }
		    else if(!strcmp(subPrefix, "timefil"))   
            {   
                	
                strcpy(chain,strtok_r(NULL, " ", &tmpsave));   
                strcpy(proto,strtok_r(NULL, " ", &tmpsave));   
                strcpy(sip,strtok_r(NULL, " ", &tmpsave));   
                strcpy(smask,strtok_r(NULL, " ", &tmpsave));   
                strcpy(dip,strtok_r(NULL, " ", &tmpsave));   
                strcpy(dmask,strtok_r(NULL, " ", &tmpsave));   
                strcpy(action,strtok_r(NULL, " ", &tmpsave));   
                strcpy(beginTime,strtok_r(NULL, " ", &tmpsave));   
                strcpy(endTime,strtok_r(NULL, " ", &tmpsave));   
                strcpy(week,strtok_r(NULL, " ", &tmpsave));   
                strcpy(log,strtok_r(NULL, " ", &tmpsave));  
                strcpy(type,"filter"); 
            }
		    else if(!strcmp(subPrefix, "redi"))
		    {		    		
		    	strcpy(interface,strtok_r(NULL, " ", &tmpsave));
		    	strcpy(proto,strtok_r(NULL, " ", &tmpsave));
		    	strcpy(fromport,strtok_r(NULL, " ", &tmpsave));
		    	strcpy(toport,strtok_r(NULL, " ", &tmpsave));
		    	strcpy(type,"nat");
		    }
		    else if(!strcmp(subPrefix, "antiattack"))
			{
				strcpy(subPrefix,strtok_r(NULL, " ", &tmpsave));
				if(!strcmp(subPrefix, "syn"))
				{
					strcpy(dip,strtok_r(NULL, " ", &tmpsave));
					strcpy(dport,strtok_r(NULL, " ", &tmpsave));
					strcpy(rate,strtok_r(NULL, " ", &tmpsave));
				}
				else if(!strcmp(subPrefix, "icmp"))
				{
					strcpy(dip,strtok_r(NULL, " ", &tmpsave));
					strcpy(rate,strtok_r(NULL, " ", &tmpsave));
				}
			}
		}
		else if (!strcmp(fwPrefix, "webwall"))
		{
			strcpy(subPrefix,strtok_r(NULL, " ", &tmpsave));
			strcpy(interface,strtok_r(NULL, " ", &tmpsave));
			strcpy(fromport,strtok_r(NULL, " ", &tmpsave));
			strcpy(toport,strtok_r(NULL, " ", &tmpsave));
			strcpy(dip,strtok_r(NULL, " ", &tmpsave));
		}
printf("%s\n",type);		
		if(!strcmp(type,"filter"))
		{		
			struct fwFilterListEntry * fil_data;
			if((fil_data = (struct fwFilterListEntry *)malloc(sizeof(struct fwFilterListEntry)))==NULL)
			{
				return -1;
			}
			memset(fil_data, 0, sizeof(struct fwFilterListEntry));
			strncpy(fil_data->type, type, 32);
			strncpy(fil_data->chain, chain, 32);
			strncpy(fil_data->proto, proto, 32);
			strncpy(fil_data->sip, sip, 32);
			strncpy(fil_data->smask, smask, 32);
			strncpy(fil_data->smac, smac, 32);
			strncpy(fil_data->sport, sport, 32);
			strncpy(fil_data->dip, dip, 32);
			strncpy(fil_data->dmask, dmask, 32);
			strncpy(fil_data->dport, dport, 32);
			strncpy(fil_data->action, action, 32);
			strncpy(fil_data->beginTime, beginTime, 32);
			strncpy(fil_data->endTime, endTime, 32);
			strncpy(fil_data->week, week, 32);
			strncpy(fil_data->rate, rate, 32);
			strncpy(fil_data->log, log, 32);
			
			printf("%s\n",fil_data->chain);
			printf("%s\n",fil_data->proto);
			printf("%s\n",fil_data->sip);
			printf("%s\n",fil_data->dmask);
			printf("%s\n",fil_data->dport);

			
			aos_list_add_tail((struct aos_list_head *)&fil_data->datalist, &sgFwFilterList);	 		
		}	
		
		else if(!strcmp(type,"nat"))
		{		
			struct fwNatListEntry * nat_data;
			if((nat_data = (struct fwNatListEntry *)malloc(sizeof(struct fwNatListEntry)))==NULL)
			{
				return -1;
			}
			memset(nat_data, 0, sizeof(struct fwNatListEntry));
			strncpy(nat_data->type, type, 32);
			strncpy(nat_data->interface, interface, 32);
			strncpy(nat_data->proto, proto, 32);
			strncpy(nat_data->sip, sip, 32);
			strncpy(nat_data->smask, smask, 32);
			strncpy(nat_data->sport, sport, 32);
			strncpy(nat_data->dip, dip, 32);
			strncpy(nat_data->dmask, dmask, 32);
			strncpy(nat_data->dport, dport, 32);
			strncpy(nat_data->to, to, 32);
			strncpy(nat_data->fromport, fromport, 32);
			strncpy(nat_data->toport, toport, 32);
			
			aos_list_add_tail((struct aos_list_head *)&nat_data->datalist, &sgFwNatList);	 	
		}       
		 printf("ok\n");	
		 		
		}
	
	}   
	
	strcpy(result, rbuffer);
	return 0;
}	    
	    
/*	    
// get word
char* get_Word(char * zDes, const char * strSrc)
{
	char * zTmpBeg = NULL, * zTmpEnd = NULL;
	int nSizeBeg = 0, nSizeEnd = 0;
	zTmpBeg = strSrc;
	strsep(&zTmpBeg, " ");
	zTmpEnd = zTmpBeg;
	strsep(&zTmpEnd, " ");
	nSizeBeg = strlen(zTmpBeg);
	nSizeEnd = strlen(zTmpEnd);
	
	strncpy(zDes, zTmpBeg , nSizeBeg - nSizeEnd);
	zDes[nSizeBeg - nSizeEnd] = 0;
	return zTmpEnd;
}
*/


///////////////////////////////////////////////////////
// by CHK
///////////////////////////////////////////////////////
#ifdef _DEBUG_
int 
doShell(const char *zCmd, char *zMsg)
{
    FILE *fp = NULL;
    char line[2048];

    fp=popen(zCmd,"r");
    while(fgets(line,2048,fp))
    {
        strcpy(zMsg, line);
    }
    pclose(fp);
    return 0;
}
#endif

// Resource Manager's APIs
// Cpu Mgr APIs
int ros_cpu_mgr_set_record_time(char **zRsltPtr, const char* zDuration)
{
	char sbuffer[1024];
	// send the data to system CLI daemon
	sprintf(sbuffer, "cpu mgr set record time %s", zDuration);
	sendSystemCli(sbuffer, (*zRsltPtr));	
	// return the return message to the result
	return 0;
}

int ros_cpu_mgr_set_record_stop(char **zRsltPtr)
{
	char sbuffer[1024];
	// send the data to system CLI daemon
	strcpy(sbuffer, "cpu mgr set record stop");
	sendSystemCli(sbuffer, (*zRsltPtr));
	// return the return message to the result
	return 0;
}

int ros_cpu_mgr_show_cpu(char **zRsltPtr, const int nRsltLen, const int nIntervalSec, const char * zBegTime, const char * zEndTime, const char * zIntervalTime)
{
	char sbuffer[1024];
	// send the data to system CLI daemon
	sprintf(sbuffer, "cpu mgr show cpu %s %s %s", zBegTime, zEndTime, zIntervalTime);
	sendSystemCli(sbuffer, (*zRsltPtr));
	(*zRsltPtr)[nRsltLen-1] = 0;
	// return the return message to the result
	return 0;
}

int ros_cpu_mgr_show_process(char **zRsltPtr, const int nRsltLen, const int nIntervalSec, const char * zBegTime, const char * zEndTime, const int nTopProcNum)
{
	char sbuffer[1024];
	// send the data to system CLI daemon
	sprintf(sbuffer, "cpu mgr show proc %s %s", zBegTime, zEndTime);
	sendSystemCli(sbuffer, *zRsltPtr);
	(*zRsltPtr)[nRsltLen-1] = 0;
	// return the return message to the result
	return 0;
}

// Memory Mgr APIs
int ros_mem_mgr_set_record_time(char **zRsltPtr, const char* zDuration)
{
	char sbuffer[1024];
	// send the data to system CLI daemon
	sprintf(sbuffer, "mem mgr set record time %s", zDuration);
	sendSystemCli(sbuffer, (*zRsltPtr));
	// return the return message to the result
	return 0;
}

int ros_mem_mgr_set_record_stop(char **zRsltPtr)
{
	char sbuffer[256];
	// send the data to system CLI daemon
	strcpy(sbuffer, "mem mgr set record stop");
	sendSystemCli(sbuffer, (*zRsltPtr));
	// return the return message to the result
	return 0;
}

int ros_mem_mgr_show_mem(char **zRsltPtr, const int nRsltLen, const char * zBegTime, const char * zEndTime, const char * zIntervalTime)
{
	char sbuffer[256];
	// send the data to system CLI daemon
	sprintf(sbuffer, "mem mgr show mem %s %s %s", zBegTime, zEndTime, zIntervalTime);
	sendSystemCli(sbuffer, (*zRsltPtr));
	(*zRsltPtr)[nRsltLen-1] = 0;
	// return the return message to the result

	return 0;
}

// Disk Mgr APIs
int ros_disk_mgr_set_record_time(char **zRsltPtr, const int nRsltLen, const char* zDurationLen)
{
	char sbuffer[256];
	// send the data to system CLI daemon
	sprintf(sbuffer, "disk mgr set record time %s", zDurationLen);
	sendSystemCli(sbuffer, (*zRsltPtr));	
	// return the return message to the result

	return 0;
}

int ros_disk_mgr_set_record_stop(char **zRsltPtr)
{
	char sbuffer[256];
	// send the data to system CLI daemon
	strcpy(sbuffer, "disk mgr set record stop");
	sendSystemCli(sbuffer, (*zRsltPtr));	
	// return the return message to the result
	return 0;
}

int ros_disk_mgr_file_type_add(char **zRsltPtr, const char* zType)
{
	char sbuffer[256];
	// send the data to system CLI daemon
	sprintf(sbuffer, "disk mgr file type add %s", zType);
	sendSystemCli(sbuffer, (*zRsltPtr));	
	// return the return message to the result
	return 0;
}

int ros_disk_mgr_file_type_remove(char **zRsltPtr, const char* zType)
{
	char sbuffer[256];
	// send the data to system CLI daemon
	sprintf(sbuffer, "disk mgr file type remove %s", zType);
	sendSystemCli(sbuffer, (*zRsltPtr));	
	// return the return message to the result
	return 0;
}

int ros_disk_mgr_file_type_clear(char **zRsltPtr)
{
	char sbuffer[1024];
	// send the data to system CLI daemon
	strcpy(sbuffer, "disk mgr file type clear");
	sendSystemCli(sbuffer, (*zRsltPtr));	
	// return the return message to the result
	return 0;
}

int ros_disk_mgr_file_type_show(char **zRsltPtr)
{
	char sbuffer[1024];
	// send the data to system CLI daemon
	strcpy(sbuffer, "disk mgr file type show");
	sendSystemCli(sbuffer, (*zRsltPtr));	
	// return the return message to the result
	return 0;
}


// /proc/partitions DISKMGR_PARTITION_FILE
// /proc/mounts 	DISKMGR_MOUNTS_FILE
int ros_load_partition(ros_harddisk_info_t * const hd_info_ptr, int *hd_num , ros_partition_info_t * const part_info_ptr, int *part_num)
{
	int bRslt = 1;
	FILE *fp = NULL;
	char * line = NULL;
	size_t len = 0;
	int index = 0;
	int i = 0, nSize = 0;
	char mount_fname[256] = "";
	struct mntent *mnt_ptr;

	struct statfs fs;

	fp = fopen(DISKMGR_PARTITION_FILE, "r");
	if(fp)
	{
		while(getline(&line, &len, fp) > 0)
		{
			if(index > 1)
			{
				sscanf(line, "%d %d %ld %s" , 
						&part_info_ptr[index-2].hd_id, 
						&part_info_ptr[index-2].partition_id, 
						&part_info_ptr[index-2].space_total, 
						&mount_fname);
				part_info_ptr[index-2].is_mounted = 0;
				strcpy(part_info_ptr[index-2].mount_path, "/dev/");
				strcat(part_info_ptr[index-2].mount_path, mount_fname);
				part_info_ptr[index-2].space_used = 0;
				part_info_ptr[index-2].space_avail = 0;
			}
			index++;
		}
	}
	nSize = index-2;
	for(index = nSize; index < DISKMGR_PARTITION_MAX_NUM; index++)
	{
		part_info_ptr[index].hd_id = 0; 
		part_info_ptr[index].partition_id = 0; 
	}
	if(line)
	{
		free(line);
		line = NULL;
	}
	if(fp)
	{
		fclose(fp);
		fp = NULL;
	}
	
	// Get the Partition Entry
	fp = setmntent(DISKMGR_MOUNTS_FILE, "r");
	if(fp)
	{
		while ((mnt_ptr = getmntent(fp)) != NULL) 
		{
			if (NULL == strcasestr(mnt_ptr->mnt_fsname, "/dev/"))
			{
				continue;
			}
			for(i = 0; i < nSize; i++)
			{
				if(0 == strcasecmp(mnt_ptr->mnt_fsname, part_info_ptr[i].mount_path))
				{
					break;
				}
			}
			statfs(mnt_ptr->mnt_dir, &fs);

			strcpy(part_info_ptr[i].mount_dir, mnt_ptr->mnt_dir);
			part_info_ptr[i].space_total = fs.f_blocks;  				/* total data blocks in file system */
			part_info_ptr[i].space_used  = fs.f_blocks - fs.f_bfree;  /* other from free blocks in fs */
			part_info_ptr[i].space_avail = fs.f_bavail;   				/* free blocks avail to non-superuser */
			part_info_ptr[i].is_mounted  = 1;
		}
	}
	if(fp)
	{
		fclose(fp);
		fp = NULL;
	}
	// Generate the Hard disk info Entry
	// should sort part_info_ptr first!!!!!!!!!!! by hd_id , part_id
	{
		int j = 0;
		ros_partition_info_t part_info_tmp;
		for(i = 0; i < nSize; i++)
		{
			for(j = i + 1; j < nSize; j++)
			{
				if(part_info_ptr[i].hd_id > part_info_ptr[j].hd_id)
				{
					// doing swap
					part_info_tmp = part_info_ptr[i];
					part_info_ptr[i] = part_info_ptr[j];
					part_info_ptr[j] = part_info_tmp;
				}
			}
		}
	}
	index = 0;
	for(i = 0; i < nSize; i++) /* i for partition   --- index for hd */
	{
		if(hd_info_ptr[index].hd_id == part_info_ptr[i].hd_id)
		{
			hd_info_ptr[index].num_partitions++;
			hd_info_ptr[index].total_space += part_info_ptr[i].space_total;	/* total data blocks in file system */
			hd_info_ptr[index].space_used  += part_info_ptr[i].space_used;	/* other from free blocks in fs */
		}
		else
		{
			if(i > 0)
			{
				index ++;
			}
			hd_info_ptr[index].hd_id = part_info_ptr[i].hd_id;
			hd_info_ptr[index].num_partitions = 1;
			hd_info_ptr[index].total_space = part_info_ptr[i].space_total;	/* total data blocks in file system */
			hd_info_ptr[index].space_used  = part_info_ptr[i].space_used;	/* other from free blocks in fs */
		}
	}
	index++;
	*hd_num = nSize;
	*part_num = index;
	for(index; index < DISKMGR_HDISK_MAX_NUM; index++)
	{
		hd_info_ptr[index].hd_id = 0; 
		hd_info_ptr[index].num_partitions = 0; 
	}
	
	return bRslt;
}

//  
int ros_get_num_hd()
{
	int hd_num, part_num;
	ros_harddisk_info_t hd_info[DISKMGR_HDISK_MAX_NUM];
	ros_partition_info_t part_info[DISKMGR_PARTITION_MAX_NUM];
	ros_load_partition(hd_info, &hd_num , part_info, &part_num);

	return hd_num;
}

int ros_get_num_partition(int hd)
{
	int hd_num, part_num;
	ros_harddisk_info_t hd_info[DISKMGR_HDISK_MAX_NUM];
	ros_partition_info_t part_info[DISKMGR_PARTITION_MAX_NUM];
	ros_load_partition(hd_info, &hd_num , part_info, &part_num);

	return part_num;
}

int ros_get_harddisk_info(int hd, struct ros_harddisk_info *diskinfo)
{
	int i = 0;
	int hd_num, part_num;
	ros_harddisk_info_t hd_info[DISKMGR_HDISK_MAX_NUM];
	ros_partition_info_t part_info[DISKMGR_PARTITION_MAX_NUM];
	ros_load_partition(hd_info, &hd_num , part_info, &part_num);

	for(i = 0; i < hd_num; i++)
	{
		if(hd_info[i].hd_id == hd)
		{
			*diskinfo = hd_info[i];
			break;
		}
	}

	return 0;
}

int ros_get_partition_info(int hd, int partition, struct ros_partition_info * p )
{
	int i = 0;
	int hd_num, part_num;
	ros_harddisk_info_t hd_info[DISKMGR_HDISK_MAX_NUM];
	ros_partition_info_t part_info[DISKMGR_PARTITION_MAX_NUM];
	ros_load_partition(hd_info, &hd_num , part_info, &part_num);

	for(i = 0; i < part_num; i++)
	{
		if(part_info[i].hd_id == hd && part_info[i].partition_id == partition)
		{
			*p = part_info[i];
			break;
		}
	}

	return 0;
}




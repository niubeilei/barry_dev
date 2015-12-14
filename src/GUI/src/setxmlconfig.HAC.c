////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: setxmlconfig.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
/*************************************************************************
 * set SSL PROXY CONFIG FILE (XML format) from the config files that GUI USER configed
 *************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define ZMAXLINE 1024	
#define LENGTH	256
#define itemnum	24

//定义配置文件名
#define ETH0			"/etc/sysconfig/network-scripts/ifcfg-eth0"
#define APP_PROXY	"/usr/local/keyou/Config/proxy.conf"
#define CURRENTFILE	"/usr/local/keyou/Config/current"

typedef struct _apptable	{
	char appname[LENGTH];
	char apptype[LENGTH];
	char appport[LENGTH];
	char flag[LENGTH];
	char rs_addr[LENGTH];
	char rs_port[LENGTH];
	char status[10];
	struct _apptable * ru;
}	apptable;

static char *sgModuleNames[] = 
{
	"app proxy",
	""
};

	
int get_fedora_ip(char * eth0_IP)
{
	FILE * eth0fp;
	char line[ZMAXLINE];
	char * pos;
	int i;

	eth0fp = fopen(ETH0,"r");
	while (eth0fp)
	{
		memset( line, 0, sizeof(line) );
		if (!fgets(line, sizeof(line), eth0fp))
		{
	        	printf("can not find eth0 ipaddr\n");
	        	break;
		}
		if ( !(pos = strstr(line, "IPADDR")) && 
		     !(pos = strstr(line, "ipaddr")) )
		{
			continue;
		}
		pos +=6;
		while( *pos )
		{
			if( *pos != ' ' && *pos != '=' ) break; 
			pos++;
		}
		if( *pos == 0 ) break;

		i=0;
		while( (*pos == '.') || (*pos >= '0' && *pos <= '9') ){
			eth0_IP[i] = *pos;
			i++;
			pos++;
		}   
		eth0_IP[i] = 0;
		//printf("find eth0 addr %s\n", eth0_IP);
		break;
	}
	if(eth0fp) fclose(eth0fp);
	
	return 0;
}


int get_debian_ip(char * ip_eth0)
{
	FILE 	*fp;
	char	buf[1024];
	int	interface;

	fp = fopen("/etc/network/interfaces","r");
	if(!fp)
	{
		return -1;
	}
	interface=0;
	while(fgets(buf,1024,fp))
	{
		char * tmp;
		char * dest;
		int	i;

		buf[1023]=0;
		if(strstr(buf,"iface eth0"))
		{
			interface=0;
		}

		if((tmp=strstr(buf,"address")))
		{
			tmp+=8;
			while(tmp[0]==' ')
			{
				for(i=0;i<strlen(tmp)-1;i++)
					tmp[i]=tmp[i+1];
			}
			for(i=0;i<strlen(tmp);i++)
			{
				if(tmp[i]!='.' && (tmp[i]<'0' || tmp[i]>'9'))
					tmp[i]=0;
			}
			if(interface==0)
				dest=ip_eth0;
			strcpy(dest,tmp);
			break;
			//printf("%d %s\n",interface,tmp);
		}
	}
	fclose(fp);

	return 0;
}

int get_gentoo_ip(char * ip_eth0)
{
	FILE *fp;
	char buf[1024];
	char * tmp, *dest;
	int	interface;
	char eth0[20];
	int index, index_eth0;

	fp = fopen("/etc/conf.d/net","r");
	if(!fp)
	{
		return -1;
	}
	interface=0;
	index = 0;
	index_eth0 = 0;
	while(fgets(buf,1024,fp))
	{
		buf[1023]=0;
		if (buf[0] == '#')
			continue;
		if(strstr(buf,"iface_eth0"))
		{
			while (buf[index] != '\"')
			{
				index++;
			}
			index++;
			while ((buf[index] == '.') || ( buf[index] >= '0' && buf[index] <= '9'))
			{
				eth0[index_eth0] = buf[index];
				index_eth0++;
				index++;
			}
			eth0[index_eth0] = 0;
			break;
		}
	}
	tmp = eth0;
	dest = ip_eth0;
	strcpy(dest,tmp);

	fclose(fp);

	return 0;
}


int is_debian()
{
	int ret;
	ret = access("/etc/debian_version",R_OK);

	//ret = system("grep -i debian /etc/issue > /dev/null");
	if(ret==0)
		return 1;
	else
		return 0;
}

int is_fedora()
{
	int ret;
	ret = access("/etc/redhat-release",R_OK);

	if(ret==0)
		return 1;
	else
		return 0;
}


int is_gentoo()
{
	int ret;
	ret = access("/etc/gentoo-release",R_OK);

	if(ret==0)
		return 1;
	else
		return 0;
}


int main(int argc, char **argv)
{
	FILE *fp, *fpipe;
	char tmp[ZMAXLINE];
	char eth0_IP[20];
	char line[ZMAXLINE];
	
	int num;

	apptable *app,appt;

	int module_counts = 0;

	fpipe = fopen(CURRENTFILE, "w");		
	if (!fpipe)	
		return -1;

	//read netcard config
	strcpy(eth0_IP,"not found");

	if(is_debian())
	{
		get_debian_ip(eth0_IP);
	}
	else if(is_fedora())
	{
		get_fedora_ip(eth0_IP);
	}
	else if(is_gentoo())
	{
		get_gentoo_ip(eth0_IP);
	}
	else
	{	
		printf("The OS we do not know!");
	}
		
	app = &appt;
	memset(app, 0, sizeof(*app));

	//read app proxy config file
	fp = fopen(APP_PROXY,"r");		
			
	/***
	*  set app proxy with pmi rules
	***/
	
	sprintf(tmp, "------AosCliBlock: %s------\n", sgModuleNames[module_counts++]);
	fprintf(fpipe,"%s",tmp);
	fputs("<Commands>\n",fpipe);
	
	while ((fgets(line,sizeof(line),fp))!=NULL)
	{
		if (line[0]=='#') continue;
					
		//parse  the struct of every line  in the appproxy conf  file  
		//                              01 02 03 04 05 06 07 
		memset(app, 0, sizeof(*app));
		num = sscanf(line, "%s %s %s %s %s %s %s ",
			app->appname,
			app->apptype,
			app->appport, 
			app->flag,	
			app->rs_addr,
			app->rs_port,
			app->status); 

			if(strcasecmp(app->apptype,"smtp")==0)
				strcpy(app->apptype,"http");
			if(strcasecmp(app->apptype,"pop3")==0)
				strcpy(app->apptype,"http");
			if(strcasecmp(app->apptype,"tcp")==0)
				strcpy(app->apptype,"http");
			if(strcasecmp(app->apptype,"yzw")==0)
				strcpy(app->apptype,"jns");
			if(strcasecmp(app->apptype,"http")==0)
				strcpy(app->apptype,"http");
			if(strcasecmp(app->apptype,"ftp")==0)
				strcpy(app->apptype,"ftp");	
			if(strcasecmp(app->apptype,"telnet")==0)
				strcpy(app->apptype,"telnet");	
					
			sprintf(tmp, "<Cmd>app proxy add %s %s %s %s %s</Cmd>\n", app->appname, app->apptype, eth0_IP, app->appport, app->flag);
			fprintf(fpipe,"%s",tmp);
			sprintf(tmp, "<Cmd>app proxy rs add %s %s %s</Cmd>\n", app->appname, app->rs_addr, app->rs_port);
			fprintf(fpipe,"%s",tmp);
				
			if (strcasecmp(app->status,"on")==0) 
			{
				sprintf(tmp, "<Cmd>app proxy set status %s on</Cmd>\n", app->appname);
				fprintf(fpipe,"%s",tmp);
			}				
	}
		      		
	fputs("</Commands>\n",fpipe);	
	fputs("------EndAosCliBlock------\n\n",fpipe);
		
	fclose(fp);	
	fclose(fpipe);

	return 0;
} 

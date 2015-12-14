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
/*deleted by yt
#define SYNFLOOD_PN 	1
#define PINGFLOOD_PN 	5
#define SYNFLOOD_BW 	30
#define PINGFLOOD_BW 	30
*/ 
/**********
 * FUNCTION  :  Open an EXISTED config file.
 * PARAMETERS:  ident  config file's pathname
 *              oflag  OPEN_RD    Open for reading only
 *                     OPEN_RDWR  Open for writing only
 * RETURN    :  CONFIG Handler  on success ,NULL on error.
 **********/       

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
#define ETH1			"/etc/sysconfig/network-scripts/ifcfg-eth1"
#define APP_PROXY	"/usr/local/AOS/Config/app_proxy.conf"
#define RULES		"/usr/local/AOS/Config/rules.conf"
#define ASSIGN		"/usr/local/AOS/Config/assign.conf"
#define OCSP			 "/usr/local/AOS/Config/ocsp.conf"
#define PMI_GLOBAL	"/usr/local/AOS/Config/pmi_global.conf"
#define DENYPAGEDB	"/usr/local/AOS/Config/denypages.conf"
#define CRLSERVER	"/usr/local/AOS/Config/crlserver.conf"
#define USBKEY_USERS "/usr/local/AOS/Config/usbkeyuser.conf"

#define CURRENTFILE	"/usr/local/AOS/Config/current"

typedef struct _pmiglobal	{
	char status[10];
	char ip[20];
	char port[6];
	char orgname[LENGTH];
}	pmiglobal;

typedef struct _crlserver	{
	char status[10];
	char ip[20];
	char port[6];
	char URL[LENGTH];
	char timeout[20];
}	crlserver;

typedef struct _denypageDB	{
	char name[20];
	char locate[ZMAXLINE];
}	denypageDB;

typedef struct _pmirule	{
	char appname[LENGTH];
	char sysname[LENGTH];
	char orgname[LENGTH];
	char status[10];
}	pmirule;

typedef struct _apptable	{
	char changeflag[6];
	char appname[LENGTH];
	char proxytype[LENGTH];
	char apptype[LENGTH];
	char appport[LENGTH];
	char flag[LENGTH];
	char rs_addr[LENGTH];
	char rs_port[LENGTH];
	char status[10];
	char addtime[LENGTH];
	char denypage[LENGTH];
	char pmi_yes[10];
	char pmi_status[10];
	char pmi_sysname[LENGTH];
	struct _apptable * ru;
}	apptable;

typedef struct __usbkeyuser
{
	char number[19];
	char username[LENGTH];
	char department[LENGTH];	
} usbkey_user;
static char *sgModuleNames[] = 
{
	"app proxy",
	"forward table",
	"deny page",
	"cert manager",
	"cert chain",
	"usbkey",
	"ocsp",
	"amm",
	"ssl",
	"util",
	""
};

	
int get_fedora_ip(char * eth0_IP, char * eth1_IP)
{
	FILE * eth0fp, *eth1fp;
	char line[ZMAXLINE];
	char * pos;

	eth0fp = fopen(ETH0,"r");
	while (eth0fp)
	{
		memset(line, 0, sizeof(line));
		if (!fgets(line, sizeof(line), eth0fp))
		{
	        printf("can not find eth0 ipaddr\n");
	        break;
		}
		if (strstr(line, "IPADDR") || strstr(line, "ipaddr"))
		{
			pos = strstr(line, "=");
			if (!pos && *(pos+1)) continue;
			strncpy(eth0_IP, pos+1, sizeof(eth0_IP));
			printf("find eth0 addr %s\n", eth0_IP);
			break;
		}
	}
	if(eth0fp) fclose(eth0fp);
	

	eth1fp = fopen(ETH1,"r");
    while (eth1fp)
    {
        memset(line, 0, sizeof(line));
		if (!fgets(line, sizeof(line), eth1fp))
		{
			printf("can not find eth1 ipaddr\n");
			break;
		}
        if (strstr(line, "IPADDR") || strstr(line, "ipaddr"))
        {
            pos = strstr(line, "=");
			if (!pos && *(pos+1)) continue;
            strncpy(eth1_IP, pos+1, sizeof(eth1_IP));
            printf("find eth1 addr %s\n", eth1_IP);
			break;
        }
    }
	if(eth1fp) 
		fclose(eth1fp);
	return 0;
}


int get_debian_ip(char * ip_eth0, char * ip_eth1)
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
		else if(strstr(buf,"iface eth1"))
		{
			interface=1;
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
			else
				dest=ip_eth1;
			strcpy(dest,tmp);
			//printf("%d %s\n",interface,tmp);
		}
	}
	fclose(fp);

	return 0;
}

int is_debian()
{
	int ret;
	struct stat buf;
	ret = stat("/etc/debian_version",&buf);

	//ret = system("grep -i debian /etc/issue > /dev/null");
	if(ret==0)
		return 1;
	else
		return 0;
}

int main(int argc, char **argv)
{
	FILE * fp, *fpipe,*ocspfp,*crlserverfp,*pmirulefp,*pmiglobalfp,*denypageDBfp;
	FILE *usbkeyfp;
	char tmp[ZMAXLINE];
	char eth0_IP[20];
	char eth1_IP[20];
	char ocsp_STATUS[6];
	char ocsp_IP[20];
	char ocsp_PORT[6];
	char line[ZMAXLINE];
	
	int num;

	apptable *app,appt;
	pmiglobal *pmiglobalsetting,pmiglobalsettingt;
	crlserver *crlserversetting,crlserversettingt;
	pmirule *pmirules,pmirulest;	
	denypageDB *denypages,denypagest;
	usbkey_user usbkeydata;

	int find = 0;	
	int module_counts = 0;

	fpipe = fopen(CURRENTFILE, "w");		
	if (!fpipe)	
		return -1;

	//read netcard config
	strcpy(eth0_IP,"192.168.1.100");
	strcpy(eth1_IP,"192.168.0.100");

	if(is_debian())
	{
		get_debian_ip(eth0_IP,eth1_IP);
	}
	else
	{
		get_fedora_ip(eth0_IP,eth1_IP);
	}
		
	app = &appt;
	pmiglobalsetting = &pmiglobalsettingt;
	pmirules = &pmirulest;
	memset(app, 0, sizeof(*app));
	memset(pmiglobalsetting, 0, sizeof(*pmiglobalsetting));
	//memset(pmirules, 0, sizeof(*pmirules));

	//read pmi global setting
	pmiglobalfp=fopen(PMI_GLOBAL,"r");
	fgets(line,sizeof(line),pmiglobalfp);
	while (line[0]=='#') 
	{
		fgets(line,sizeof(line),pmiglobalfp);
	}		
	num = sscanf(line, "%s %s %s %s",
	pmiglobalsetting->status,
	pmiglobalsetting->ip, 
	pmiglobalsetting->port, 
	pmiglobalsetting->orgname);
	//if (num < 3) return NULL;
	fclose(pmiglobalfp);
	
	//read app proxy config file
	fp = fopen(APP_PROXY,"r");		
	pmirulefp=fopen(RULES,"r");	
		
			
	/***
	*  set app proxy with pmi rules
	***/
	
	sprintf(tmp, "------AosCliBlock: %s------\n", sgModuleNames[module_counts++]);
	fprintf(fpipe,"%s",tmp);
	fputs("<Commands>\n",fpipe);
	
	fputs("<Cmd>ktcpvs init</Cmd>\n",fpipe); 
		
	pmirulefp=fopen(RULES,"r");
	while ((fgets(line,sizeof(line),fp))!=NULL)
	{
		if (line[0]=='#') continue;
					
		//parse  the struct of every line  in the appproxy conf  file  
		//                              01 02 03 04 05 06 07 08 09 10 11 
		memset(app, 0, sizeof(*app));
		num = sscanf(line, "%s %s %s %s %s %s %s %s %s %s %s ",
			app->changeflag,				 
			app->appname,
			app->proxytype, 
			app->apptype,
			app->appport, 
			app->flag,	
			app->rs_addr,
			app->rs_port,
			app->status, 
			app->addtime,
			app->denypage);
		//if (num < 11) return NULL;

	 	strcpy(app->pmi_yes,"no");			
		strcpy(app->pmi_status,"off");
		while ((fgets(line,sizeof(line),pmirulefp))!=NULL)
		{
			if (line[0]=='#') continue;
			memset(pmirules, 0, sizeof(*pmirules));
			num = sscanf(line, "%s %s %s %s",
				pmirules->appname, 
				pmirules->orgname,
				pmirules->sysname, 
				pmirules->status);
			//if (num < 3) return NULL;
				
			if (strcasecmp(pmirules->appname,app->appname)==0)
			{
				strcpy(app->pmi_yes,"yes");
				strcpy(app->pmi_status,pmirules->status);
				strcpy(app->pmi_sysname,pmirules->sysname);
				break;
			}
		}
		rewind(pmirulefp);			
					
						
		if (strcasecmp(app->proxytype,"forward")==0)
		{
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
					
			sprintf(tmp, "<Cmd>app proxy add %s %s %s %s ssl</Cmd>\n", app->appname,app->apptype,eth0_IP,app->appport);
			fprintf(fpipe,"%s",tmp);
			sprintf(tmp, "<Cmd>app proxy rs add %s %s %s</Cmd>\n", app->appname,app->rs_addr,app->rs_port);
			fprintf(fpipe,"%s",tmp);
			//sprintf(tmp, "<Cmd>app proxy ssl front %s on</Cmd>\n", app->appname);
			//fprintf(fpipe,"%s",tmp);
			sprintf(tmp, "<Cmd>app proxy ssl clientauth %s on</Cmd>\n", app->appname);
			fprintf(fpipe,"%s",tmp);
				
			/*
			if (strcasecmp(app->denypage,"-")!=0) 
			{
				sprintf(tmp, "<Cmd>deny page assign %s %s</Cmd>\n", app->appname,app->denypage);
				fprintf(fpipe,"%s",tmp);
			}
			*/
			if ((strcasecmp(app->apptype,"http")==0)&&(strcasecmp(app->pmi_yes,"yes")==0))
			{
				sprintf(tmp, "<Cmd>app proxy pmi sysname %s %s</Cmd>\n", app->appname,app->pmi_sysname);
				fprintf(fpipe,"%s",tmp);	
			}
			sprintf(tmp, "<Cmd>app proxy set pmi %s %s</Cmd>\n", app->appname,app->pmi_status);
			fprintf(fpipe,"%s",tmp);		
			
			if (strcasecmp(app->status,"on")==0) 
			{
				sprintf(tmp, "<Cmd>app proxy set status %s on</Cmd>\n", app->appname);
				fprintf(fpipe,"%s",tmp);
			}				
		}
		if (strcasecmp(app->proxytype,"backward")==0)
		{
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
			if(strcasecmp(app->apptype,"dhttp")==0) 
			{
					sprintf(tmp, "<Cmd>app proxy add %s http %s %s plain</Cmd>\n", app->appname,eth1_IP,app->appport);
					fprintf(fpipe,"%s",tmp);
			}
			else
			{
					sprintf(tmp, "<Cmd>app proxy add %s %s %s %s plain</Cmd>\n", app->appname,app->apptype,eth1_IP,app->appport);
					fprintf(fpipe,"%s",tmp);
			}				
			if (strcasecmp(app->rs_addr,"-")!=0) 
			{
				sprintf(tmp, "<Cmd>app proxy rs add %s %s %s</Cmd>\n", app->appname,app->rs_addr,app->rs_port);
				fprintf(fpipe,"%s",tmp);
			}
			sprintf(tmp, "<Cmd>app proxy ssl backend %s on</Cmd>\n", app->appname);
			fprintf(fpipe,"%s",tmp);
			if ((strcasecmp(app->apptype,"dhttp") == 0) || 
				(strcasecmp(app->apptype, "jns") == 0))
			{
				sprintf(tmp, "<Cmd>app proxy forward table %s on</Cmd>\n", app->appname);
				fprintf(fpipe,"%s",tmp);	
			}				
			if (strcasecmp(app->status,"on")==0) 
			{
				sprintf(tmp, "<Cmd>app proxy set status %s on</Cmd>\n", app->appname);
				fprintf(fpipe,"%s",tmp);
			}				
		}					
	}
		      		
	fputs("</Commands>\n",fpipe);	
	fputs("------EndAosCliBlock------\n\n",fpipe);
		
	fclose(fp);	
	fclose(pmirulefp);

	//set forward table
	sprintf(tmp, "------AosCliBlock: %s------\n", sgModuleNames[module_counts++]);
	fprintf(fpipe,"%s",tmp);
	fputs("<Commands>\n",fpipe);
	fputs("</Commands>\n",fpipe);	
	fputs( "------EndAosCliBlock------\n\n",fpipe);
	
  	/***
	*  set deny pages
	***/
	
	denypageDBfp=fopen(DENYPAGEDB,"r");
	denypages = &denypagest;
	sprintf(tmp, "------AosCliBlock: %s------\n", sgModuleNames[module_counts++]);
	fprintf(fpipe,"%s",tmp);		
	fputs("<Commands>\n",fpipe);		
		
	while ((fgets(line,sizeof(line),denypageDBfp))!=NULL)
	{
		if (line[0]=='#') continue;		
		memset(denypages, 0, sizeof(*denypages));
		num = sscanf(line, "%s %s",
				denypages->name, 
				denypages->locate);
		//if (num < 3) return NULL;
		sprintf(tmp, "<Cmd>deny page import %s /usr/local/AOS/Config/Denypages/%s</Cmd>\n", denypages->name,denypages->locate);
		fprintf(fpipe,"%s",tmp);			
	}
	
	fp = fopen(APP_PROXY,"r");
	while ((fgets(line,sizeof(line),fp))!=NULL)
	{
		if (line[0]=='#') continue;
				
		//parse  the struct of every line  in the appproxy conf  file  
		//                              01 02 03 04 05 06 07 08 09 10 11 
		num = sscanf(line, "%s %s %s %s %s %s %s %s %s %s %s ",
			app->changeflag,				 
			app->appname,
			app->proxytype, 
			app->apptype,
			app->appport, 
			app->flag,	
			app->rs_addr,
			app->rs_port,
			app->status, 
			app->addtime,
			app->denypage);
			//if (num < 11) return NULL;
			if (strcasecmp(app->denypage,"-")!=0) 
			{
					sprintf(tmp, "<Cmd>deny page assign %s %s</Cmd>\n", app->appname,app->denypage);
					fprintf(fpipe,"%s",tmp);
			}
	}			
			
	fclose(denypageDBfp);
	fclose(fp);
	
	fputs("</Commands>\n",fpipe);	
	fputs( "------EndAosCliBlock------\n\n",fpipe);


	//set cert manager
	sprintf(tmp, "------AosCliBlock: %s------\n", sgModuleNames[module_counts++]);
	fprintf(fpipe,"%s",tmp);
	fputs("<Commands>\n",fpipe);
	fputs("</Commands>\n",fpipe);	
	fputs( "------EndAosCliBlock------\n\n",fpipe);

	//set cert chain
	sprintf(tmp, "------AosCliBlock: %s------\n", sgModuleNames[module_counts++]);
	fprintf(fpipe,"%s",tmp);
	fputs("<Commands>\n",fpipe);
	fputs("</Commands>\n",fpipe);	
	fputs( "------EndAosCliBlock------\n\n",fpipe);
	
	// set usbkey users
	
	usbkeyfp = fopen(USBKEY_USERS, "r");
	if(usbkeyfp)
	{
	sprintf(tmp, "------AosCliBlock: %s------\n", sgModuleNames[module_counts++]);
	fprintf(fpipe,"%s",tmp);		
	fputs("<Commands>\n",fpipe);		

	while ( (fgets(line, sizeof(line), usbkeyfp) != NULL ))
	{
		if (line[0] == '#') continue;		
		memset(&usbkeydata, 0, sizeof(usbkeydata));
		num = sscanf(line, "%s %s %s", usbkeydata.number, usbkeydata.username, usbkeydata.department); 
		sprintf(tmp, "<Cmd>usbkey add %s</Cmd>\n", usbkeydata.number);
		fprintf(fpipe,"%s",tmp);			
	}
	fputs("</Commands>\n",fpipe);	
	fputs( "------EndAosCliBlock------\n\n",fpipe);
	fclose(usbkeyfp);
	}
	/***
	*  set OCSP
	***/	

	ocspfp = fopen(OCSP,"r");
	fgets(line,sizeof(line),ocspfp);
	num = sscanf(line, "%s %s %s",ocsp_STATUS,ocsp_IP,ocsp_PORT);
	sprintf(tmp, "------AosCliBlock: %s------\n", sgModuleNames[module_counts++]);
	fprintf(fpipe,"%s",tmp);
	fputs("<Commands>\n",fpipe);
	sprintf(tmp, "<Cmd>ocsp set addr %s %s</Cmd>\n", ocsp_IP,ocsp_PORT);
	fprintf(fpipe,"%s",tmp);
	sprintf(tmp, "<Cmd>ocsp status %s</Cmd>\n", ocsp_STATUS);
	fprintf(fpipe,"%s",tmp);
	fclose(ocspfp);
	
	/***
	*  set CRLSERVER
	***/	

	crlserversetting = &crlserversettingt;
	memset(crlserversetting, 0, sizeof(*crlserversetting));
	//memset(pmirules, 0, sizeof(*pmirules));

	//read crl server setting
	crlserverfp=fopen(CRLSERVER,"a+");
	find = 0;
	while (!find)
	{
		memset(line, 0, sizeof(line));
		if(!fgets(line,sizeof(line),crlserverfp))
			break;
		if (line[0] == '#')
			continue;
		find = 1; 
	}	
	fclose(crlserverfp);
	if (find)
	{	
		num = sscanf(line, "%s %s %s %s %s",
			crlserversetting->status,
			crlserversetting->ip, 
			crlserversetting->port, 
			crlserversetting->URL,
			crlserversetting->timeout);
		//if (num < 5) return NULL;
		sprintf(tmp, "<Cmd>crl set addr %s %s</Cmd>\n", crlserversetting->ip,crlserversetting->port);
			fprintf(fpipe,"%s",tmp);	
		sprintf(tmp, "<Cmd>crl set url %s</Cmd>\n", crlserversetting->URL);
			fprintf(fpipe,"%s",tmp);		
		sprintf(tmp, "<Cmd>crl status %s</Cmd>\n", crlserversetting->status);
			fprintf(fpipe,"%s",tmp);
		sprintf(tmp, "<Cmd>crl set timeout %s</Cmd>\n", crlserversetting->timeout);
			fprintf(fpipe,"%s",tmp);
	}	
	fputs("</Commands>\n",fpipe);	
	fputs( "------EndAosCliBlock------\n\n",fpipe);
	
			
	
	/***
	*  set amm
	***/
	
	pmiglobalfp= fopen(PMI_GLOBAL,"r");
	fgets(line,sizeof(line),pmiglobalfp);
	num = sscanf(line, "%s %s %s %s",pmiglobalsetting->status,pmiglobalsetting->ip,pmiglobalsetting->port,pmiglobalsetting->orgname);
	sprintf(tmp, "------AosCliBlock: %s------\n", sgModuleNames[module_counts++]);
	fprintf(fpipe,"%s",tmp);
	fputs("<Commands>\n",fpipe);
	sprintf(tmp, "<Cmd>amm status %s</Cmd>\n", pmiglobalsetting->status);
		fprintf(fpipe,"%s",tmp);	
	sprintf(tmp, "<Cmd>amm set addr %s %s</Cmd>\n", pmiglobalsetting->ip,pmiglobalsetting->port);
		fprintf(fpipe,"%s",tmp);		
	sprintf(tmp, "<Cmd>amm set org %s</Cmd>\n", pmiglobalsetting->orgname);
		fprintf(fpipe,"%s",tmp);	
	fputs("</Commands>\n",fpipe);	
	fputs( "------EndAosCliBlock------\n\n",fpipe);
	

		
	
	/***
	*  set ssl
	***/
	sprintf(tmp, "------AosCliBlock: %s------\n", sgModuleNames[module_counts++]);
	fprintf(fpipe,"%s",tmp);
	fputs("<Commands>\n",fpipe);
	fputs("<Cmd>ssl session reuse client off</Cmd>\n",fpipe);
	fputs("<Cmd>ssl session reuse server on</Cmd>\n",fpipe);
	fputs("<Cmd>ssl hardware on</Cmd>\n",fpipe);
	fputs("</Commands>\n",fpipe);	
	fputs( "------EndAosCliBlock------\n\n",fpipe);

	fclose(fpipe);
	return 0;
} 

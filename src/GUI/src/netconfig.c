////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: netconfig.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
	
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


#define INTERFACES "/etc/network/interfaces"
   
void net_str_trip(char * tmp)
{
	int i;

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

}
int get_item(char * eth, char * item, char * value)
{
	FILE 	*fp;
	char	buf[1024];
	char	line[1024];

	line[0]=0;
	fp = fopen(INTERFACES,"r");
	if(!fp)
	{
		perror("open file");
		return -1;
	}

	while(fgets(buf,1024,fp))
	{
		char * tmp;

		buf[1023]=0;
		if(strstr(buf,"iface"))
		{
			strcpy(line,buf);
		}

		if(strstr(line,eth) && (tmp=strstr(buf,item)))
		{
			tmp+=strlen(item)+1;
			strcpy(value,tmp);
			net_str_trip(value);
		}
	}
	fclose(fp);
	return 0;
}


int set_item(char * eth, char * item, char * value)
{
	FILE 	*fp;
	FILE 	*fp2;
	char	buf[1024];
	char	template[256];
	char	line[1024];

	line[0]=0;
	strcpy(template,"/tmp/aos-set-net-XXXXXX");
	close(mkstemp(template));
	fp = fopen(INTERFACES,"r");
	fp2 = fopen(template,"w");
	if(!fp || !fp2)
	{
		perror("open file");
		return -1;
	}

	while(fgets(buf,1024,fp))
	{
		char * tmp;

		buf[1023]=0;
		if(strstr(buf,"iface"))
		{
			strcpy(line,buf);
		}

		if(strstr(line,eth) && (tmp=strstr(buf,item)))
		{
			tmp+=strlen(item);
			tmp[0]=0;
			fprintf(fp2,buf);
			strcpy(tmp," ");
			strcat(tmp,value);
			fprintf(fp2,"%s\n",tmp);
		}
		else
		{
			fputs(buf,fp2);
		}
	}
	fclose(fp);
	fclose(fp2);
	sprintf(buf,"/bin/rm -f %s",INTERFACES);
	system(buf);
	sprintf(buf,"/bin/cp -f %s %s",template,INTERFACES);
	system(buf);
	sprintf(buf,"/bin/rm -f %s",template);
	system(buf);
	sprintf(buf,"/bin/chmod +r %s",INTERFACES);
	system(buf);
	return 0;
}


void usage()
{
	printf("Usage: \n");
	printf("\tnetconfig get gateway\n");
	printf("\tnetconfig set gateway x.x.x.x\n");

	printf("\tnetconfig get eth0 address/netmask/network\n");
	printf("\tnetconfig set eth0 address/netmask/network x.x.x.x\n");
	exit(1);
}

int main(int argc, char ** argv)
{
	char value[1024];
	enum
	{
		get,
		set
	}
	action;
	
	value[0]=0;
	if(argc<3)
	{
		usage();
	}

	if(0==strcmp(argv[1],"set"))
	{
		action=set;	
	}
	else
		action=get;

	if(0==strcmp(argv[2],"gateway"))
	{
		if(get==action)
		{
			get_item("iface","gateway",value);
			printf("%s\n",value);
		}
		else
		{
			if(argc<4)
				usage();
			set_item("iface","gateway", argv[3]);
		}
	}
	else
	{
		// netconfig get/set eth0 netmask xxxx
		if(set==action && argc<5)
			usage();
		if(set==action)
		{
			set_item(argv[2],argv[3],argv[4]);
		}
		else
		{
			get_item(argv[2],argv[3],value);
			printf("%s\n",value);
		}
	}

	return 0;
}


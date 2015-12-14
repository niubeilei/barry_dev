////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: RouterStat.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Global.h"
//#include <sys/file.h>
#include "CliUtil/CliUtil.h"
#include "CliUtil/CliUtilProc.h"

#include <iomanip>


int routerStatisShow(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	
	FILE * fp;
	char * line = NULL;
	size_t len = 0;
	ssize_t read;
	struct RouterStatis RtStat;
	OmnString contents;
	char local[2048] = "";
	
	fp = fopen("/proc/net/dev","r");

	rewind(fp);
	
	for( int i = 0; i<2 ;i++)
	{
		read = getline(&line,&len,fp);
		if(read == -1)
		{
			break;
		}
	}
	
	contents << "Router Statistics Information\n" 
		     << "-----------------------------\n"
			 << "if-dev\t" 
			 << "Recv-Packets " 
			 << "Recv-Error "
			 << "Recv-Drop "
			 << "Recv-Bytes "
			 << "Send-Packets " 
			 << "Send-Error " 
			 << "Send-Drop " 
			 << "Send-Bytes\n";

	while( (read =getline(&line,&len,fp)) != -1)
	{
		
		int nSize = strlen(line);
		for(int i = 0; i < nSize; i++)         //replace ":" with " "
		{
			if(line[i] == ':')
			{
				line[i] = ' ';
			}
		}
		
		sscanf(line, "%s %lu %lu %lu %lu %*d %*d %*d %*d %lu %lu %lu %lu",
			   RtStat.Nic,&RtStat.RecByte,&RtStat.RecPack,&RtStat.RecError,&RtStat.RecDrop,&RtStat.SendByte,&RtStat.SendPack,&RtStat.SendError,&RtStat.SendDrop);

		contents << RtStat.Nic; 
		contents << "\t";
		contents << long(RtStat.RecPack);
		contents << " ";
		contents << long(RtStat.RecError);
		contents << " ";
		contents << long(RtStat.RecDrop);
		contents << " ";
		contents << long(RtStat.RecByte);
		contents << " " ;
		contents << long(RtStat.SendPack); 
		contents << " ";
		contents << long(RtStat.SendError); 
		contents << " ";
		contents << long(RtStat.SendDrop); 
		contents << " ";
		contents << long(RtStat.SendByte);
		contents << "\n";
	}
	
	if(line)
 	{
  		free(line);
  		line = NULL;
 	}

	strcpy(local, contents.data());

	CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));
	*optlen = index;
	return 0;

}

int routerStatisSetRecordTime(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	printf("router stat set record time");
	return 0; 
}

int routerStatisStatus(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	printf("router stat set status");
	return 0;
}					

int RouterStat_regCliCmd(void)
{	
	int ret;
	
	ret = CliUtil_regCliCmd("router stat show",routerStatisShow);
	ret |= CliUtil_regCliCmd("router stat status",routerStatisStatus);
	ret |= CliUtil_regCliCmd("router stat set record time",routerStatisSetRecordTime);
	
	return ret;
}

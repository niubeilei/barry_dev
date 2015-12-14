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
// Modification History:
////////////////////////////////////////////////////////////////////////////
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Jimold/JimoldReqProc.h"
#include "JimoAgentUtil/Configuration.h"
#include "Jimold/Ptrs.h"
#include "Proggie/ReqDistr/ReqDistr.h"
#include "XmlUtil/SeXmlParser.h"
#include "Util/File.h"
int gAosShowNetTraffic = 1;

map<string, string> sgConfig;

int
main(int argc, char **argv)
{
	if (argc < 2)
	{
		printf("execute jimod, please input args\n");
		exit(-1);
	}
	int index = 1;
	while(index < argc)
	{
		if (strcmp(argv[index], "-c") == 0)
		{
			bool rslt = AosConfiguration::ReadConfig(argv[index + 1], sgConfig);
			if (!rslt)
			{
				printf("read config error\n");
				exit(-1);
			}
			break;
		}
		index++;
	}
	OmnString port = sgConfig["Port"];
	OmnString str;
	str << "<config "
		<< "req_delinerator=\"first_four_high\" "
		<< "max_proc_time=\"10000\" "
		<< "max_conns=\"1024\" "
		<< "local_addr=\"0.0.0.0\" "
		<< "local_port=\"" << port << "-" << port << "\"/>";
	AosXmlTagPtr config = AosXmlParser::parse(str AosMemoryCheckerArgs);
	aos_assert_r(config, -1);
	AosJimoldReqProcPtr proc = OmnNew AosJimoldReqProc();
	AosReqDistr reqDistr(proc);
	if (!reqDistr.config(config))
	{
		printf("*************config error******************\n%s\n", str.data());
		exit(-1);
	}
	reqDistr.start();

	while(1)
	{
		sleep(1);
	}
	return 0;
} 


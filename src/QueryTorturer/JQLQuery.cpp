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
//
// Modification History:
// 2014/05/14	Created by Fei Pei
////////////////////////////////////////////////////////////////////////////
#include "QueryTorturer/JQLQuery.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Util/String.h"

static OmnString clientExe = "../../../../Jimo_Servers0/JQLClient.exe";
static OmnString host = "192.168.99.182";
static int port = 30570;	

AosJQLQuery::AosJQLQuery()
{
}


AosJQLQuery::~AosJQLQuery()
{
}

//
//This method accept a JQL statement
//and send to AccessServer to run
//the statement and return the result
//
bool 
AosJQLQuery::runQuery(AosRundataPtr rdata, 
					  OmnString stmt) 
{
	FILE *fp = NULL;
	char buf[1024];
	OmnString cmd = "";
	OmnString result = "";

	//generate the command to get the JQL statement result
	if (stmt == "")
	{
		OmnCout << "No statement to run!" << endl;
		return false;
	}
	
	cmd << clientExe << " -p " << port 
		<< " -f \"xml\"" 
		<< " -h " << host
		<< " -t \"" << stmt << "\""; 

	cout << cmd << endl;
	fp = popen(cmd.getBuffer(), "r");
	while (fgets(buf, 1024, fp)) {
		result << buf;
	}
	cout << result << endl;
	rdata->setJqlData(result);
	fclose(fp);
	return true;
}


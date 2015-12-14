//////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ParentControlListCli.cpp
// Description:
//   
// Author: Tim
//
// Modification History: 
//	created 12/27/2006
// 
////////////////////////////////////////////////////////////////////////////
//
#include "CliUtil/CliUtil.h"
#include "Tracer/Tracer.h"
#include "aos/aosReturnCode.h"
#include "Debug/Debug.h"
#include "ListConf.h"
#include "ParentCtrl/webwall_cli.h"

int WebwallListCreateCli(
	char *data, 
	unsigned int *optlen, 
	struct aosUserLandApiParms *parms, 
	char *errmsg, 
	const int errlen)
{
	OmnString strErr;
	char *list_type = parms->mStrings[0];
	char *list_name = parms->mStrings[1];

	*optlen = 0;
	
	OmnTrace << "list_type: " << list_type << ", list_name: " << list_name << endl;	
	ListConf::NewListConf(list_type, list_name);
	if (ListConf::CheckListType(list_type))
	{
		strErr = "the inputted list type is not supported, we only support the following types:\n";
		strErr +=  ListConf::ShowSupportedListType(); 
		strncpy(errmsg, strErr.data(), errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	
	return eAosRc_Success;
}

int ParentCtrl_regCliCmd(void)
{
	//CliUtil_regCliCmd("webwall list create", WebwallListCreateCli);
	//CliUtil_regCliCmd("log set user filter", AosTracer_setFilterCli);
	ww_cli_register();
	
	return eAosRc_Success;
}

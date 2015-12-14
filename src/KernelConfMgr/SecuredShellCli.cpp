////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SecuredShellCli.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////


#include "KernelConfMgr/SecuredShellCli.h"

#include "CliUtil/CliUtil.h"
#include "CliUtil/CliUtilProc.h"
#include "aosUtil/Alarm.h"
#include "aosUtil/List.h"  
#include "aosUtil/Memory.h"
#include "Debug/Debug.h"
#include "Tracer/Tracer.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "XmlParser/XmlItem.h"

#include "SecuredShell/SecuredShellMgr.h"

int 
securedShellCreateShell(char *data,
						unsigned int *optlen,
						struct aosUserLandApiParms *parms,
						char *errmsg,
						const int errlen)
{
	int ret = 0;
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	OmnString local;
	
	//
	//secured shell clear shell <username>
	//

	OmnString username;
	
	username = parms->mStrings[0];
	
	AosSecuredShellMgr shellMgr;
	
	if (shellMgr.createSecuredShell(local))
	{
		CliUtil_checkAndCopy(buf, &index, *optlen, local.data(), local.length()); 
		*optlen = index;
	}
	else
	{
		strncpy(errmsg, local, errlen-1);
		errmsg[errlen-1] = 0;
		ret = -1;
	}
	return ret;
}


int 
securedShellStartShell(char *data,
						unsigned int *optlen,
						struct aosUserLandApiParms *parms,
						char *errmsg,
						const int errlen)
{
	int ret = 0;
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	OmnString local;
	
	AosSecuredShellMgr shellMgr;

	//
	// secured shell start <shell_id> <password> <duration> <username>
	//

	OmnString username,shellId, password;
	int duration;
	
	shellId = parms->mStrings[0];
	password = parms->mStrings[1];
	username = parms->mStrings[2];
	duration = parms->mIntegers[0];

	if (shellMgr.startSecuredShell(username,shellId,password,duration,local))
	{
		CliUtil_checkAndCopy(buf, &index, *optlen, local.data(), local.length()); 
		*optlen = index;
	}
	else
	{
		strncpy(errmsg, local, errlen-1);
		errmsg[errlen-1] = 0;
		ret = -1;
	}
	return ret;
}

int 
securedShellStopShell(char *data,
						unsigned int *optlen,
						struct aosUserLandApiParms *parms,
						char *errmsg,
						const int errlen)
{
	int ret = 0;
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	OmnString local;
	
	AosSecuredShellMgr shellMgr;

	//
	// secured shell stop shell <shell_id> <password> <username>
	//

	OmnString username,shellId, password;
	
	shellId = parms->mStrings[0];
	password = parms->mStrings[1];
	username = parms->mStrings[2];

	if (shellMgr.stopSecuredShell(username,shellId,password,local))
	{
		CliUtil_checkAndCopy(buf, &index, *optlen, local.data(), local.length()); 
		*optlen = index;
	}
	else
	{
		strncpy(errmsg, local, errlen-1);
		errmsg[errlen-1] = 0;
		ret = -1;
	}
	return ret;
}


int 
securedShellRemoveCommands( char *data,
							unsigned int *optlen,
							struct aosUserLandApiParms *parms,
							char *errmsg,
							const int errlen)
{
	int ret = 0;
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	OmnString local;
	
	AosSecuredShellMgr shellMgr;

	//
	// secured shell remove commands <shell_id> <username>
	//

	OmnString username,shellId;
	
	shellId = parms->mStrings[0];
	username = parms->mStrings[1];

	if (shellMgr.removeSecuredShell(shellId,local))
	{
		CliUtil_checkAndCopy(buf, &index, *optlen, local.data(), local.length()); 
		*optlen = index;
	}
	else
	{
		strncpy(errmsg, local, errlen-1);
		errmsg[errlen-1] = 0;
		ret = -1;
	}
	return ret;
}

int 
securedShellRestoreCommands(char *data,
							unsigned int *optlen,
							struct aosUserLandApiParms *parms,
							char *errmsg,
							const int errlen)
{
	int ret = 0;
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	OmnString local;

	AosSecuredShellMgr shellMgr;

	//
	// secured shell restore commands <shellId> <username>
	//

	OmnString username,shellId;
	
	shellId = parms->mStrings[0];
	username = parms->mStrings[1];

	if (shellMgr.restoreSecuredShell(shellId,local))
	{
		CliUtil_checkAndCopy(buf, &index, *optlen, local.data(), local.length()); 
		*optlen = index;
	}
	else
	{
		strncpy(errmsg, local, errlen-1);
		errmsg[errlen-1] = 0;
		ret = -1;
	}
	return ret;
}

int 
securedShellClearShell(char *data,
						unsigned int *optlen,
						struct aosUserLandApiParms *parms,
						char *errmsg,
						const int errlen)
{
	int ret = 0;
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	OmnString local;
	
	AosSecuredShellMgr shellMgr;

	//
	//secured shell clear shell <username>
	//

	OmnString username;
	
	username = parms->mStrings[0];

	if (shellMgr.stopAllShell(local))
	{
		CliUtil_checkAndCopy(buf, &index, *optlen, local.data(), local.length()); 
		*optlen = index;
	}
	else
	{
		strncpy(errmsg, local, errlen-1);
		errmsg[errlen-1] = 0;
		ret = -1;
	}
	return ret;
}

int SecuredShell_regCliCmd(void)
{	
	int ret;
	
	ret = CliUtil_regCliCmd("secured shell create shell", securedShellCreateShell);
	ret |= CliUtil_regCliCmd("secured shell start shell", securedShellStartShell);
	ret |= CliUtil_regCliCmd("secured shell stop shell", securedShellStopShell);
	ret |= CliUtil_regCliCmd("secured shell clear shell", securedShellClearShell);
	ret |= CliUtil_regCliCmd("secured shell remove commands", securedShellRemoveCommands);
	ret |= CliUtil_regCliCmd("secured shell restore commands", securedShellRestoreCommands);
	
	return ret;
}

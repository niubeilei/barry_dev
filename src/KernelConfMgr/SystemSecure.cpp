////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SystemSecure.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include "KernelInterface/CliSysCmd.h"
#include "CliUtil/CliUtil.h"
#include "CliUtil/CliUtilProc.h"

#include "aosUtil/Alarm.h"
#include "aosUtil/Memory.h"
#include "Debug/Debug.h"
#include "Tracer/Tracer.h"
#include "Util/File.h"
#include "../rhcInclude/sqlite3.h"
#include "CommandMan/CommandMan.h"
#include "../rhcInclude/openssl/md5.h"

#include <string.h>

#include "Global.h"

int secureAuthcmdPolicy(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
    OmnString rslt;	
	
	sqlite3 *db=NULL;
	char *zErrMsg = 0;

	int rc;

	rc = sqlite3_open("../aos_secure/securecmd.db", &db);
	if( rc )
	{
		rslt = "Can't open database";
		strncpy(errmsg, rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;
		
		sqlite3_close(db);
		return -1;
	}

	char *sql;
	if(!strcmp(parms->mStrings[0], "accept"))
	{
	    printf("%s",parms->mStrings[0]);
		sql ="delete from policy;";
		sqlite3_exec(db,sql,0,0,&zErrMsg);
		sql ="insert into \"policy\" values (\"accept\");";
		sqlite3_exec(db,sql,0,0,&zErrMsg);
	}
	if(!strcmp(parms->mStrings[0], "deny"))
	{
	    printf("%s",parms->mStrings[0]);
	    sql ="delete from policy;";
		sqlite3_exec(db,sql,0,0,&zErrMsg);
		sql ="insert into \"policy\" values (\"deny\");";
		sqlite3_exec(db,sql,0,0,&zErrMsg);
	}
	sqlite3_close(db);
	
	*optlen = index;
	return 0;
}

int secureAuthcmdCommandAdd(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	
	unsigned int index = 0;
    OmnString rslt;	
	
	sqlite3 *db=NULL;
	char *zErrMsg = 0;
	char sql[1024];
	char md5[33];

	int rc;
    int ret = aos_Generate_MD5(parms->mStrings[0], md5);

	rc = sqlite3_open("../aos_secure/securecmd.db", &db);
	if( rc )
	{
		rslt = "Can't open database";
		strncpy(errmsg, rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;
		
		sqlite3_close(db);
		return -1;
	}
	if(ret == -1)
	{
		rslt = "No such command!";
		strncpy(errmsg, rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;
		
		return -1;
	}
	sprintf(sql, "insert into cmdtable values (\"%s\",\"%s\");",parms->mStrings[0], md5);
	sqlite3_exec(db, sql,0,0,&zErrMsg);
	sqlite3_close(db);

	*optlen = index;
	return 0;
}

int secureAuthcmdCommandDel(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{	
	unsigned int index = 0;
	OmnString rslt;	
	sqlite3 *db=NULL;
	char *zErrMsg = 0;
	char sql[1024];

	int rc;

	rc = sqlite3_open("../aos_secure/securecmd.db", &db);
	if( rc )
	{
		rslt = "Can't open database";
		strncpy(errmsg, rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;
		sqlite3_close(db);
		return -1;
	}
	sprintf(sql, "delete from cmdtable where filename =\"%s\";", parms->mStrings[0]);
	sqlite3_exec(db, sql,0,0,&zErrMsg);
	sqlite3_close(db);

	*optlen = index;
	return 0;
}

int secureAuthcmdCommandShow(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	OmnString rslt;	
	OmnString contents;
	char local[2048];
	
	sqlite3 *db=NULL;
	char *zErrMsg = 0;
	char *sql;
	int nrow = 0, ncolumn = 0;
	char **azResult; 
	int rc;

	rc = sqlite3_open("../aos_secure/securecmd.db", &db);
	if( rc )
	{
		rslt = "Can't open database";
		strncpy(errmsg, rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;
		sqlite3_close(db);
		return -1;
	}
	
	sql = "select * from cmdtable;";
	sqlite3_get_table(db,sql,&azResult,&nrow,&ncolumn,&zErrMsg );
	
	int i = 0 ;
	contents << "The Command List\n------------------------\n" ;
	for( i=0 ; i<( nrow + 1 ) * ncolumn ; i=i+2 )
	{	
		contents << azResult[i] << "\t" ;
		contents << azResult[i+1] << "\n" ;
	}	
	
	sql = "select \"policy\" from policy; ";
	sqlite3_get_table(db,sql,&azResult,&nrow,&ncolumn,&zErrMsg );
	contents << "\n" ;
	contents << "The Policy\n------------------------\n" << azResult[1] << "\t";

	sqlite3_free_table( azResult );
	sqlite3_close(db);

	strcpy(local, contents.data());
	CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));
	*optlen = index;
	return 0;
}

int secureAuthcmdCommandReset(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	OmnString rslt;
	
	sqlite3 *db=NULL;
	char *zErrMsg = 0;

	int rc;

	rc = sqlite3_open("../aos_secure/securecmd.db", &db);
	if( rc )
	{
		rslt = "Can't open database";
		strncpy(errmsg, rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;
		sqlite3_close(db);
		return -1;
	}

	char *sql;
	
	sql ="delete from cmdtable;";
	sqlite3_exec(db,sql,0,0,&zErrMsg);
	sqlite3_close(db);
	
	*optlen = index;
	return 0;

}

int SecureAuth_regCliCmd(void)
{	
	int ret;
	
	ret = CliUtil_regCliCmd("secure authcmd policy", secureAuthcmdPolicy);
	ret |= CliUtil_regCliCmd("secure authcmd command add", secureAuthcmdCommandAdd);
	ret |= CliUtil_regCliCmd("secure authcmd command del", secureAuthcmdCommandDel);
	ret |= CliUtil_regCliCmd("secure authcmd command show", secureAuthcmdCommandShow);
	ret |= CliUtil_regCliCmd("secure authcmd command reset", secureAuthcmdCommandReset);
	
	return ret;
}

////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//
// Modification History:
// 2007-05-30: Created by Frank
////////////////////////////////////////////////////////////////////////////

#include "AmUtil/AmExternalSvrMgr.h"
#include "Util/OmnNew.h"
#include <sqlite3.h>

#include "Alarm/Alarm.h"


AosAmExternalSvrMgr::AosAmExternalSvrMgr(const char * db_conn_str)
{
	if(db_conn_str)
	{
		strncpy(mDBConn, db_conn_str, 256);
		mDBConn[255] = 0;
	}
	else
	{
		mDBConn[0] = 0;
	}
	mIsGood = false; // loadSvrConfig(db_conn_str);
}


AosAmExternalSvrMgr::~AosAmExternalSvrMgr()
{
}


bool AosAmExternalSvrMgr::appendSvr(const AosAmExternalSvrPtr &server)
{
	if(!server)
	{
		return false;
	}

	int serverTypeId = server->getServerTypeId();
	mSvrQueue[serverTypeId].append(server);
	
	int i;
	AosAmExternalSvrPtr tmp;
	for(i = 0; i < AosAmMsgId::eMaxMsgId; i++)
	{
		if(server->isMsgProcess(i))
		{
			mMsgQueue[i].reset();
			while(mMsgQueue[i].hasMore())
			{
				tmp = mMsgQueue[i].next();
				if(server->getMsgProcessPriority(i) < tmp->getMsgProcessPriority(i))
				{
					break;
				}
			}
			mMsgQueue[i].insertAt(server);
		}
	}
	return true;
}


bool AosAmExternalSvrMgr::syncMsgSvr()
{
	int i, svrTypeId;
	AosAmExternalSvrPtr tmp, server;

	for(i = 0; i < AosAmMsgId::eMaxMsgId; i++)
	{
		mMsgQueue[i].clear();
	}

	for(svrTypeId = 0; svrTypeId < AosAmRequestSvrTypeId::eMaxSvrTypeId; svrTypeId++)
	{
		mSvrQueue[svrTypeId].reset();
		while(mSvrQueue[svrTypeId].hasMore())
		{
			server = mSvrQueue[svrTypeId].next();
			for(i = 0; i < AosAmMsgId::eMaxMsgId; i++)
			{
				if(server->isMsgProcess(i))
				{
					mMsgQueue[i].reset();
					while(mMsgQueue[i].hasMore())
					{
						tmp = mMsgQueue[i].next();
						if(server->getMsgProcessPriority(i) < tmp->getMsgProcessPriority(i))
						{
							break;
						}
					}
					mMsgQueue[i].insertAt(server);
				}
			}
		}
	}
//printf("Server response trace response OK! mMsgQueue[1].entries=%d FILE(%s)LINE(%d)\n",mMsgQueue[1].entries(), __FILE__, __LINE__ );
	return true;
}


bool AosAmExternalSvrMgr::removeSvr(const char *svr_name)
{
	int i;
	AosAmExternalSvrPtr server; 
	for(i = 0; i < AosAmRequestSvrTypeId::eMaxSvrTypeId; i++)
	{
		mMsgQueue[i].reset();
		while(mSvrQueue[i].hasMore())
		{
			server = mSvrQueue[i].crtValue();
			if(server && strcmp(server->getServerName(), svr_name) == 0)
			{
				mSvrQueue[i].eraseCrt();
				return true;
			}
			mSvrQueue[i].next();
		}
	}
	return false;
}


AosAmExternalSvrPtr AosAmExternalSvrMgr::getSvr(const char *svr_name)
{
	int i;
	AosAmExternalSvrPtr server; 
	for(i = 0; i < AosAmRequestSvrTypeId::eMaxSvrTypeId; i++)
	{
		mSvrQueue[i].reset();
		while(mSvrQueue[i].hasMore())
		{
			server = mSvrQueue[i].next();
			if(server && strcmp(server->getServerName(), svr_name) == 0)
			{
				return server;
			}
		}
	}
	return 0;
}


bool 
AosAmExternalSvrMgr::getMsgSvrArray(const int msgId, OmnVList<AosAmExternalSvrPtr> &serverArray)
{
	AosAmExternalSvrPtr tmpSvr;
	if(msgId < 1 || msgId >= AosAmMsgId::eMaxMsgId)
	{
		return false;
	}

	serverArray.clear();
	mMsgQueue[msgId].reset();
	while(mMsgQueue[msgId].hasMore())
	{
		tmpSvr = mMsgQueue[msgId].next();
		serverArray.append(tmpSvr);
	}
	return true;
}


bool AosAmExternalSvrMgr::initialize()
{
	sqlite3 *sqliteHandle;
	int retVal;
	char sql[1024];
	char **dbrslt = 0;
	char *errMsg;
	int i = 1, ret;
	int numRows, numFields;
	
	// tmp server value
	char 	name[30];
	int 	server_type_id;
	char 	ip[128];
	int 	port;
	char 	connStr[256], desc[256];
	AosAmExternalSvrPtr serverPtr;
	int 	msg_id, priority;

	if(strlen(mDBConn) == 0)
	{
		OmnAlarm << "Input connection string is NULL pointer!" << enderr;
		printf("Input connection string is NULL pointer!\n");
		return false;
	}

	retVal = sqlite3_open(mDBConn, &sqliteHandle);
	if(SQLITE_OK != retVal)
	{
		printf("db_conn[%s] Failed[%d]\n[%s]\n", mDBConn, retVal, sqlite3_errmsg(sqliteHandle));
		return false;
	}

	snprintf(sql, 1024, "select name, type_id, ip, port, conn_str, desc from conf_server");
	ret = sqlite3_get_table(sqliteHandle, sql, &dbrslt, &numRows, &numFields, &errMsg);
	if(SQLITE_OK != ret)
	{
		sqlite3_free_table(dbrslt);
		printf("db_conn[%s] Failed[%d]\n[%s]\n", mDBConn, ret, sqlite3_errmsg(sqliteHandle));
		sqlite3_close(sqliteHandle);
//		return -(ret+eAosRc_AmDBStatusStart);
		return false;
	}

	for(i = 1; i <= numRows; i++)
	{
		if(!dbrslt[i*numFields + 0])
		{
			name[0] = 0; // name[] = "";
			printf("Data load fail.");
			break;
		}
		else
		{
			strncpy(name, dbrslt[i*numFields + 0], 30);
		}
		if(!dbrslt[i*numFields + 1])
		{
			server_type_id = 0; // name[] = "";
		}
		else
		{
			server_type_id = atoi(dbrslt[i*numFields + 1]);
		}
		if(!dbrslt[i*numFields + 2])
		{
			ip[0] = 0; // ip[] = "";
			printf("Data load fail.");
			break;
		}
		else
		{
			strncpy(ip, dbrslt[i*numFields + 2], 128);
		}
		if(!dbrslt[i*numFields + 3])
		{
			port = 0; // port[] = "";
			printf("Data load fail.");
			break;
		}
		else
		{
			port = atoi(dbrslt[i*numFields + 3]);
		}
		if(!dbrslt[i*numFields + 4])
		{
			connStr[0] = 0; // connStr[] = "";
		}
		else
		{
			strncpy(connStr, dbrslt[i*numFields + 4], 128);
		}
		if(!dbrslt[i*numFields + 5])
		{
			desc[0] = 0; // desc[] = "";
		}
		else
		{
			strncpy(desc, dbrslt[i*numFields + 5], 256);
		}
		serverPtr = OmnNew AosAmExternalSvr(name, server_type_id, ip, port, connStr, desc);
		appendSvr(serverPtr);
		serverPtr = 0;
	}
	sqlite3_free_table(dbrslt);

	snprintf(sql, 1024, "select conf_server.name, conf_msg_type.id, conf_r_server_msg.priority "
						"from conf_server "
						"join conf_r_server_msg on conf_server.id = conf_r_server_msg.server_id "
						"join conf_msg_type on conf_msg_type.id = conf_r_server_msg.msg_id ");
	ret = sqlite3_get_table(sqliteHandle, sql, &dbrslt, &numRows, &numFields, &errMsg);
	if(SQLITE_OK != ret)
	{
		sqlite3_free_table(dbrslt);
		printf("db_conn[%s] Failed[%d]\n[%s]\n", mDBConn, ret, sqlite3_errmsg(sqliteHandle));
		sqlite3_close(sqliteHandle);
//		return -(ret+eAosRc_AmDBStatusStart);
		return false;
	}
	for(i = 1; i <= numRows; i++)
	{
		if(!dbrslt[i*numFields + 0])
		{
			printf("Data load fail.");
			continue;
		}
		if(!dbrslt[i*numFields + 1] || !dbrslt[i*numFields + 2])
		{
			printf("Data load fail.");
			continue;
		}
		serverPtr = getSvr(dbrslt[i*numFields + 0]);
		msg_id 	= atoi(dbrslt[i*numFields + 1]);
		priority= atoi(dbrslt[i*numFields + 2]);
		if(serverPtr)
		{
			serverPtr->setMsg(msg_id, priority);
		}
	}
	sqlite3_free_table(dbrslt);

	sqlite3_close(sqliteHandle);

	syncMsgSvr();
	mIsGood = true;
	return true;
}

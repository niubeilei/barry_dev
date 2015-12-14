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
//
// Modification History:
////////////////////////////////////////////////////////////////////////////
#include "MySqlAdapter/MySqlServer.h"

#include "alarm_c/alarm.h"
#include "Util/OmnNew.h"
#include "util_c/strutil.h"
#include "Util1/Ptrs.h"
#include "Util1/Timer.h"
#include "Util1/Wait.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/ConnBuff.h"
#include "UtilComm/TcpServerEpoll.h"
#include "UtilComm/TcpListener.h"
#include "XmlUtil/XmlTag.h"
#include "Rundata/Rundata.h"
#include "MySqlAdapter/MySqlAdapter.h"
#include "MySqlAdapter/Ptrs.h"



AosMySqlServer::AosMySqlServer()
{
}


AosMySqlServer::~AosMySqlServer()
{
}


bool
AosMySqlServer::start()
{
	OmnString ipAddr = "192.168.99.85";
	mServer = OmnNew OmnTcpServerEpoll(ipAddr, 13305, 1, 0, eAosTLT_FirstFourHigh);
	OmnTcpListenerPtr thisPtr(this, false);
	mServer->setListener(thisPtr);
	mServer->startReading();
	return true;
}


bool
AosMySqlServer::stop()
{
	aos_assert_r(mServer, false);
	mServer->stopReading();
	return true;
}


OmnString   
AosMySqlServer::getTcpListenerName() const
{
	return "WebProcMgr";
}


void        
AosMySqlServer::msgRecved(
		const OmnConnBuffPtr &conn_buff,
		const OmnTcpClientPtr &conn)
{
	AosRundataPtr rdata = OmnApp::getRundata();
	AosMySqlAdapterPtr mysql_adapter = OmnNew AosMySqlAdapter();
	int buff_len = conn_buff->getDataLength();
	char * data = conn_buff->getData();

	AosBuffPtr buff = OmnNew AosBuff(buff_len+10 AosMemoryCheckerArgs);
	memcpy(buff->data(), data, buff_len);
	buff->setDataLen(buff_len);
	buff->incIndex(buff_len);
	buff->setChar(';');
	mysql_adapter->procCommand(rdata, buff);
//	u64 buffsize = buff->dataLen();
	//char bufflen[4];
	//memcpy(bufflen, &buffsize, sizeof(buffsize));
	//conn->smartSend(bufflen, sizeof(bufflen));
	conn->smartSend(buff->data(), buff->dataLen());
}


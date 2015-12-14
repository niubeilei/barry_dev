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
// All requests are stored in mRequests. When there are too many, it will 
// stop adding requests to mRequests. When mRequests are empty, it checks
// whether there are additional requests from the log file. If yes, it
// reads in all the requests to mRequests. 
//
// Modification History:
// 2011/07/15: Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "WordTransServer/WordTransServer.h"

#include "IILUtil/IILFuncType.h"
#include "Rundata/Rundata.h"
#include "WordMgr/WordMgr1.h"
#include "IILClient/IILClient.h"


AosWordTransServer::AosWordTransServer()
{
	OmnScreen << "Starting the word server ...." << endl;
}


AosWordTransServer::~AosWordTransServer()
{
}


bool
AosWordTransServer::config(const AosXmlTagPtr &config)
{
	return true;
}


bool
AosWordTransServer::proc(const AosXmlTagPtr &req, const AosRundataPtr &rdata)
{
	if (!req)
	{
		rdata->setError() << "Failed to get the request!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	int operation = req->getAttrInt(AOSTAG_OPERATION, 0);
	AosIILFuncType opr;
	bool rslt = AosIILFuncType_toCode(opr, operation);
	aos_assert_r(rslt, false);

	switch(opr)
	{
	case eAosIILFunc_GetIILID:
		 rslt = getIILID(req, rdata);
		 break;

	default:
		 OmnAlarm << "error, failed to get the opr:" << opr << enderr; 
		 break;
	}

	return rslt;
}


bool
AosWordTransServer::getIILID(const AosXmlTagPtr &req, const AosRundataPtr &rdata)
{
	OmnString word = req->getAttrStr("word");
	bool addFlag = req->getAttrBool("addFlag");
	u64 wordid = AosWordMgr1::getSelf()->getWordId((u8 *)word.data(), word.length(), false);

	u64 iilid = 0;
	if(wordid == 0 && addFlag)
	{
		iilid = createIIL(req, rdata);
		aos_assert_r(iilid, false);
		wordid = AosWordMgr1::getSelf()->addWord(word, word.length(), iilid << 32);

OmnScreen << "get iilid," << "word:" << word << ", iilid:" << iilid << ", type:" << req->getAttrStr("iiltype") << endl;
	}
	else
	{
		if (wordid == 0)
		{
			rdata->setError() << "Failed to retrieve WordID!";
		}
		else
		{
			u64 ptr = 0;
			bool rslt = AosWordMgr1::getSelf()->getPtr(wordid, ptr);
			if(!rslt)
			{
				rdata->setError() << "Failed to get ptr: " << wordid;
			}
			iilid = ptr >> 32;
		}
	}
	
	OmnString str;
	str << "<rsp iilid=\"" << iilid << "\" "
		<< "wordid=\"" << wordid << "\" />";
	rdata->setContents(str);
	return true;
}


u64
AosWordTransServer::createIIL(const  AosXmlTagPtr &req, const AosRundataPtr &rdata)
{
	AosIILType iiltype = AosIILType_toCode(req->getAttrStr("iiltype"));
	aos_assert_r(iiltype != eAosIILType_Invalid, 0);
	
	u64 iilid = 0;
	bool isPersis = req->getAttrBool("isPersis");
	bool rslt = AosIILClient::getSelf()->createIILPublic(iilid, iiltype, isPersis, rdata);
	aos_assert_r(rslt, 0);
	
	return iilid;
}


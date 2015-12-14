////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 04/13/2011	Created by Ketty
////////////////////////////////////////////////////////////////////////////
#include "SmartDoc/SdocCheckUnique.h"

#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "SearchEngine/DocServer.h"
#include "UtilComm/TcpClient.h"
#include "XmlInterface/WebRequest.h"
#include "Actions/ActSeqno.h"
#include "Rundata/Rundata.h"
#include "SEUtilServer/CloudidSvr.h"
#include "SearchEngine/DocServer.h"
#include "SEModules/LoginMgr.h"
#include "SEInterfaces/IILClientObj.h"

AosSdocCheckUnique::AosSdocCheckUnique(const bool flag)
:
AosSmartDoc(AOSSDOCTYPE_CHECKUNIQUE, AosSdocId::eCheckUnique, flag)
{
}


AosSdocCheckUnique::~AosSdocCheckUnique()
{
}


bool
AosSdocCheckUnique::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata) 
{
	if (!sdoc)
	{
		rdata->setError() << "Missing smart doc!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	OmnString iilname, iilkey, contents;
	AosXmlTagPtr doc = rdata->getReceivedDoc();
	if (doc)
	{
		iilname = doc->getAttrStr(AOSTAG_IILNAME);
		if (iilname == "")
		{
			AosSetError(rdata, AosErrmsgId::eMissingFieldname);
			OmnAlarm << rdata->getErrmsg() << enderr;
			contents = "没有指定集合";//"No iilname";
			rdata->setResults(contents);
			return false;
		}
		iilkey = doc->getAttrStr(AOSTAG_IILKEY);
		if (iilkey == "")
		{
			AosSetError(rdata, AosErrmsgId::eMissingFieldname);
			OmnAlarm << rdata->getErrmsg() << enderr;
			contents = "没有指定关键字";//"No iilkey";
			rdata->setResults(contents);
			return false;
		}
	}
	else
	{
		iilname = sdoc->getAttrStr(AOSTAG_IILNAME);
		if (iilname == "")
		{
			AosSetError(rdata, AosErrmsgId::eMissingFieldname);
			OmnAlarm << rdata->getErrmsg() << enderr;
			contents = "没有指定集合";//"No iilname";
			rdata->setResults(contents);
			return false;
		}
		
		iilkey = sdoc->getAttrStr(AOSTAG_IILKEY);
		if (iilkey == "")
		{
			AosSetError(rdata, AosErrmsgId::eMissingFieldname);
			OmnAlarm << rdata->getErrmsg() << enderr;
			contents = "没有指定关键字";//"No iilkey";
			rdata->setResults(contents);
			return false;
		}
	}

	/*
	u64 docid, iilid;
	bool isunique, rslt;
	iilid = AosIILClientObj::getIILClient()->getIILId(iilname, rdata);
	if (iilid == 0)
	{
		AosSetError(rdata, AosErrmsgId::eDocidIsNull);
		OmnAlarm << rdata->getErrmsg() << enderr;
		contents = "系统中不存在这个集合";//"No iil!";
		rdata->setResults(contents);
		return false;
	}
	*/
	u64 docid;
	AosIILClientObj::getIILClient()->getDocid(iilname, iilkey, docid, rdata);
	if (docid == 0) 
	{
		//contents = "Value is not exist, you can use it!";
		contents = "检测值不存在，你可以使用！";//Value is not exist, you can use it!";
		rdata->setResults(contents);
		return true;
	}

	contents = "检测值已经存在，你不能使用！";
	rdata->setResults(contents);
	OmnAlarm << rdata->getErrmsg() << enderr;
	return false;
}

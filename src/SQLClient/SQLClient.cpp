////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 2011/05/26	Created by Ketty 
////////////////////////////////////////////////////////////////////////////

#include "SQLClient/SQLClient.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Debug/Debug.h"
#include "SEInterfaces/DocClientObj.h"
#include "Porting/Sleep.h"
#include "SQLClient/Ptrs.h"
#include "sys/stat.h"
#include "SqlUtil/SqlReqid.h"
#include "TransBasic/Trans.h"
#include "Util/OmnNew.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"


AosSQLClient::AosSQLClient()
{
}


AosSQLClient::~AosSQLClient()
{
}


bool
AosSQLClient::start(const AosXmlTagPtr &config)
{
	aos_assert_r(config, false);
	
	return true;
}


bool
AosSQLClient::convertData(
		const OmnString &sdoc_objid, 
		const AosRundataPtr &rdata)
{
	AosXmlTagPtr sdoc = AosDocClientObj::getDocClient()->getDocByObjid(sdoc_objid, rdata);
	
	if (!sdoc)
	{
		rdata->setError() << "Failed retrieving the smart doc: " << sdoc_objid;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	return convertData(sdoc, rdata);
}


bool
AosSQLClient::convertData(
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata)
{
	if (!sdoc)
	{
		rdata->setError() << "Missing smart doc!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// Create a transaction:
	// 	<request type="convertSql" ...>
	// 		<sdoc ..../>
	// 	</request>
	OmnString req = "<request>";
	req << sdoc->toString() << "</request>";

	// Ketty 2013/02/22
	AosTransPtr trans;// = OmnNew AosConvertDataTrans(sdoc, true, false);
	AosSendTrans(rdata, trans);
}



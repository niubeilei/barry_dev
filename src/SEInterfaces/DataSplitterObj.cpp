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
// 2013/11/21 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/DataSplitterObj.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"



AosDataSplitterObj::AosDataSplitterObj(const int version)
:
AosJimo(AosJimoType::eDataSplitter, version)
{
}


AosDataSplitterObj::~AosDataSplitterObj()
{
}


AosDataSplitterObjPtr
AosDataSplitterObj::createDataSplitterStatic(
		AosRundata *rdata, 
		const AosXmlTagPtr &def)
{
	AosJimoPtr jimo = AosCreateJimo(rdata, def);
	if (!jimo)
	{
		AosSetErrorU(rdata, "failed_creating_jimo") 
			<< "ErrorMsg: " << rdata->getErrmsg()
			<< "Config: " << def->toString() << enderr;
		return 0;
	}

	if (jimo->getJimoType() != AosJimoType::eDataSplitter)
	{
		AosSetErrorU(rdata, "not_data_splitter") << def->toString() << enderr;
		return 0;
	}

	AosDataSplitterObjPtr splitter = dynamic_cast<AosDataSplitterObj*>(jimo.getPtr());
	aos_assert_rr(splitter, rdata, 0);
	return splitter;
}



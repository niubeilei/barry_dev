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
// This is a utility to select docs.
//
// Modification History:
// 11/01/2011	Created by Linda Lin 
////////////////////////////////////////////////////////////////////////////
#include "DataGenSeqno/DataGenChineseSeqno.h"
#include "Util/String.h"
#include "Rundata/Rundata.h"

// static const char * sgNum[] = {"零", "一", "二", "三", "四", "五", "六", "七", "八", "九"};

// static const char *sgUnits[] = {"十", "百", "千", "万"};
AosDataGenChineseSeqno::AosDataGenChineseSeqno(const bool reg)
:
AosDataGenSeqno(AOSDATGGENTYPE_CHINESE_SEQNO, AosDataGenSeqnoType::eChineseSeqno, reg)
{
}


AosDataGenChineseSeqno::~AosDataGenChineseSeqno()
{
}


bool 
AosDataGenChineseSeqno::createSeqno(
		AosValueRslt &value,
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata)
{
	// <xxx zky_prefix = "xxx" zky_postfix="xxx" zky_start="xxx"
	// 		zky_end ="xxx"  zky_reusable="xxx" zky_exceptions="xxx"/>
	
	AosDataType::E datatype = AosDataType::toEnum(sdoc->getAttrStr(AOSTAG_DATA_TYPE));
	if (!AosDataType::isValid(datatype))
	{
	    datatype = AosDataType::eString;
	}

	if (datatype == AosDataType::eXmlDoc)
	{
	    rdata->setError() << "Datatype incorrect: " << datatype;
	    return false;
	}

   	u64 start = getAttrU64Value(sdoc, AOSTAG_START, 1, rdata);
    u64 end = getAttrU64Value(sdoc, AOSTAG_END, eMaxNum, rdata);
	if (end > eMaxNum)
	{
		rdata->setError() << "Missing end!";
		return false;
	}

	bool reusabel = sdoc->getAttrBool(AOSTAG_SEQNO_REUSABLE, true);
	u64 vvv = sdoc->getAttrU64(AOSTAG_SEQNO_VALUE, 0);

	if (vvv == 0) vvv = start;
	else vvv += 1;
	if (vvv > end && !reusabel)
	{
		rdata->setError() << "value Big!";
		OmnAlarm <<rdata->getErrmsg() << enderr;
		return true;
	}
	if (vvv > end) vvv = start;
	OmnString vv = ConvertToZhCN(vvv);;
	aos_assert_r(vv != "", false);

	OmnString prefix = getAttrStrValue(sdoc, AOSTAG_SEQNO_PREFIX, "", rdata);
	OmnString postfix = getAttrStrValue(sdoc, AOSTAG_SEQNO_POSTFIX, "", rdata);
	prefix<< vv << postfix;
	prefix.removeLeadingWhiteSpace();
	
	value.setStr(prefix);
	sdoc->setAttr(AOSTAG_SEQNO_VALUE, vvv);
	bool rslt = modifyDoc(sdoc, rdata);
	return rslt;
}

OmnString
AosDataGenChineseSeqno::ConvertToZhCN(u64 &num)
{
	if (num < 0 || num > eMaxNum) return "";
	OmnString value;
	value << num;
	int len = value.length();
	for (int i = len-1; i >= 0; i--)
	{
			
	}
	return "";
}

bool 
AosDataGenChineseSeqno::createSeqno(
			AosValueRslt &value,
			const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;        
}

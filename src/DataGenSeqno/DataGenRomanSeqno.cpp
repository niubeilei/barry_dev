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
#include "DataGenSeqno/DataGenRomanSeqno.h"
#include "Util/String.h"
#include "Rundata/Rundata.h"

static const char* sgUnits[10] = {"","I","II","III","IV","V","VI","VII","VIII","IX"};
static const char* sgTens[10] = {"","X","XX","XXX","XL","L","LX","LXX","LXXX","XC"};
static const char* sgHundreds[10] = {"","C","CC","CCC","CD","D","DC","DCC","DCCC","CM"};
static const char* sgThousands[10] = {"","M","MM","MMM","MV","V","VM","VMM","VMMM","MX"};


AosDataGenRomanSeqno::AosDataGenRomanSeqno(const bool reg)
:
AosDataGenSeqno(AOSDATGGENTYPE_ROMAN_SEQNO, AosDataGenSeqnoType::eRomanSeqno, reg)
{
}


AosDataGenRomanSeqno::~AosDataGenRomanSeqno()
{
}

bool 
AosDataGenRomanSeqno::createSeqno(
		AosValueRslt &value,
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata)
{
	// <xxx zky_prefix = "xxx" zky_postfix="xxx" zky_start="xxx"
	// 		zky_end ="xxx"  zky_step="xxx"	zky_exceptions="xxx" />
	
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

   	u64 start = getAttrU64Value(sdoc, AOSTAG_START, eDftStart, rdata);

    u64 end = getAttrU64Value(sdoc, AOSTAG_END, eMaxRomanNum, rdata);
	if (end > eMaxRomanNum)
	{
		rdata->setError() << "Missing end !" ;
		return false;
	}

	u64 step = getAttrU64Value(sdoc, AOSTAG_STEP, eDftStep, rdata);
	bool reusabel = sdoc->getAttrBool(AOSTAG_SEQNO_REUSABLE, true);
	u64 vvv = sdoc->getAttrU64(AOSTAG_SEQNO_VALUE, 0);

	if (vvv == 0) vvv = start;
	else vvv += step;
	if (vvv > end && !reusabel)
	{
		rdata->setError() << "value Big!";
		OmnAlarm <<rdata->getErrmsg() << enderr;
		return false;
	}
	if (vvv > end) vvv = start;
	OmnString vv = getNumberRoman(vvv);
	aos_assert_r(vv != "", false);

	OmnString prefix = getAttrStrValue(sdoc, AOSTAG_SEQNO_PREFIX, "", rdata);

	OmnString postfix = getAttrStrValue(sdoc,AOSTAG_SEQNO_POSTFIX, "", rdata);

	prefix << vv << postfix;
	prefix.removeLeadingWhiteSpace();
	value.setStr(prefix);
	sdoc->setAttr(AOSTAG_SEQNO_VALUE, vvv);
	bool rslt = modifyDoc(sdoc, rdata);
	aos_assert_r(rslt, false);
	return true;
}

OmnString
AosDataGenRomanSeqno::getNumberRoman(u64 &num)
{
    //Arabic numerals into Roman numerals (1-9999)
	if (num <= 0 || num > eMaxRomanNum) return "";
	OmnString value = sgThousands[num/1000];
	value << sgHundreds[(num/100)%10] << sgTens[(num/10)%10] << sgUnits[num%10]; 
	value.removeLeadingWhiteSpace();
	return value;
}


bool
AosDataGenRomanSeqno::createSeqno(
			AosValueRslt &value,
			const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}

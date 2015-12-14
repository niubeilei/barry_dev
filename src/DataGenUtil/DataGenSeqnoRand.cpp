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
// 12/01/2011	Created by Linda Lin 
////////////////////////////////////////////////////////////////////////////
#include "DataGenUtil/DataGenSeqnoRand.h"

#include "DataGenSeqno/DataGenArabicSeqno.h"
#include "Util/String.h"
#include "Rundata/Rundata.h"
#include "ValueSel/ValueSel.h"

AosDataGenSeqnoRand::AosDataGenSeqnoRand(const bool reg)
:
AosDataGenUtil(AOSDATGGENTYPE_SEQNO_RAND, AosDataGenUtilType::eSeqnoRand, reg)
{
}

AosDataGenSeqnoRand::AosDataGenSeqnoRand(const AosXmlTagPtr &config, const AosRundataPtr &rdata)
:
AosDataGenUtil(AOSDATGGENTYPE_SEQNO_RAND, AosDataGenUtilType::eSeqnoRand, false)
{
	aos_assert(parse(config, rdata));
}

AosDataGenSeqnoRand::AosDataGenSeqnoRand()
:
AosDataGenUtil(AOSDATGGENTYPE_SEQNO_RAND, AosDataGenUtilType::eSeqnoRand, false)
{
}

AosDataGenSeqnoRand::~AosDataGenSeqnoRand()
{
}

bool
AosDataGenSeqnoRand::nextValue(
		AosValueRslt &value,
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata)
{
	//<xxx zky_datagen_type="xxx"  AOSTAG_DATA_TYPE="">	
	//</xxx>
	//value.reset();
	aos_assert_r(sdoc, false);	
	OmnNotImplementedYet;
	return true;
}


bool 
AosDataGenSeqnoRand::nextValue(AosValueRslt &value, const AosRundataPtr &rdata)
{
	if (!mDataGenSeqno) 
	{
		value.setStr("");
		return true; 
	}
	value.reset();
	aos_assert_r(mDataGenSeqno, false);
	mDataGenSeqno->createSeqno(value, rdata);
	return true;
}

AosDataGenUtilPtr 
AosDataGenSeqnoRand::clone(const AosXmlTagPtr &config, const AosRundataPtr &rdata)
{
	return OmnNew AosDataGenSeqnoRand(config, rdata);
}

bool
AosDataGenSeqnoRand::parse(const AosXmlTagPtr &config, const AosRundataPtr &rdata)
{
	//<zky_datagen zky_datagen_type="seqno">
	//	<zky_seqno/>
	//</zky_datagen>
	aos_assert_r(config, false);
	AosXmlTagPtr datagen = config->getFirstChild();
	if (!datagen) return false; 
	mDataGenSeqno = OmnNew AosDataGenArabicSeqno(datagen, rdata);
	aos_assert_r(mDataGenSeqno, false);
	return true;
}

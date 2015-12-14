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
// 11/06/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "ValueSel/AllValueSel.h"

#include "ValueSel/ValueSelRandom.h"
#include "ValueSel/ValueSelConst.h"
#include "ValueSel/ValueSelYear.h"
#include "ValueSel/ValueSelMonth.h"
#include "ValueSel/ValueSelDay.h"
#include "ValueSel/ValueSel.h"
#include "ValueSel/ValueSelHour.h"
#include "ValueSel/ValueSelMinute.h"
#include "ValueSel/ValueSelSecond.h"
#include "ValueSel/ValueSelSeqno.h"
#include "ValueSel/ValueSelAttr.h"
#include "ValueSel/ValueSelText.h"
#include "ValueSel/ValueSelNumEntries.h"
#include "ValueSel/ValueSelNum2StrMap.h"
#include "ValueSel/ValueSelQuery.h"
#include "ValueSel/ValueSelByUserArgs.h"
#include "ValueSel/ValueSelEnum.h"
#include "ValueSel/ValueSelAccessed.h"
#include "ValueSel/ValueSelCompose.h"
#include "ValueSel/ValueSelEpoch.h"
#include "ValueSel/ValueSelFull.h"
#include "ValueSel/ValueSelPattern.h"
#include "ValueSel/ValueSelRange.h"
#include "ValueSel/ValueSelRequester.h"
#include "ValueSel/ValueSelRundata.h"
#include "ValueSel/ValueSelMath.h"
#include "ValueSel/ValueSelCondMap.h"
#include "ValueSel/ValueSelGetCounter.h"
#include "ValueSel/ValueSelCounterValue.h"
#include "ValueSel/ValueSelIILValue.h"
#include "ValueSel/ValueSelDocid.h"
#include "ValueSel/ValueSelObjid.h"
#include "ValueSel/ValueSelCid.h"
#include "ValueSel/ValueSelStrMap.h"			// Ken Lee, 2013/09/16
#include "ValueSel/ValueSelIILEntryMap.h"		// Ken Lee, 2013/11/29		
#include "ValueSel/ValueSelCounterKey.h"		// Ken Lee, 2013/11/29
#include "ValueSel/ValueSelIILPrefixValue.h"	// Andy Zhang, 2013/12/23

// #include "CustomizedValueSel/Unicom/UnicomSubstr.h"					// Chen Ding, 06/03/2012
#include "XmlUtil/XmlTag.h"

// AosValueSelObjPtr   sgValueSel[AosValueSelType::eMax+1];
// Chen Ding, 2013/02/09
// static AosStr2U32_t     	sgValueSelMap;
// static OmnString			sgNames[AosValueSelType::eMax];
//AosAllValueSels  	sgValueSels;


bool
AosAllValueSels::createSelectors()
{
	/*
	static AosValueSelRundata		sgValueSelRunata(true);
	static AosValueSelRequester		sgValueSelRequester(true);
	static AosValueSelRange			sgValueSelRange(true);
	static AosValueSelPattern		sgValueSelPattern(true);
	static AosValueSelFull			sgValueSelFull(true);
	static AosValueSelEpoch			sgValueSelEpoch(true);
	static AosValueSelCompose		sgValueSelCompose(true);
	static AosValueSelAccessed		sgValueSelAccessed(true);
	static AosValueSelRandom 		sgValueSelRandom(true);
	static AosValueSelConst	 		sgValueSelConst(true);
	static AosValueSelYear 			sgValueSelYear(true);
	static AosValueSelMonth			sgValueSelMonth(true);
	static AosValueSelDay			sgValueSelDay(true);
	static AosValueSelHour			sgValueSelHour(true);
	static AosValueSelMinute		sgValueSelMinute(true);
	static AosValueSelSecond		sgValueSelSecond(true);
	static AosValueSelSeqno			sgValueSelSeqno(true);
	static AosValueSelAttr			sgValueSelAttr(true);
	static AosValueSelText			sgValueSelText(true);
	static AosValueSelNumEntries	sgValueSelNumEntries(true);
	static AosValueSelNum2StrMap	sgValueSelNum2StrMap(true);
	static AosValueSelQuery			sgValueSelQuery(true);
	static AosValueSelByUserArgs	sgValueSelByUserArgs(true);
	static AosValueSelEnum			sgValueSelEnum(true);
	static AosValueSelMath			sgValueSelMath(true);
	static AosValueSelCondMap	    sgValueSelCondMap(true);
	static AosValueSelGetCounter	sgValueSelGetCounter(true);
	static AosValueSelCounterValue	sgValueSelCounterValue(true);
	static AosValueSelIILValue		sgValueSelIILValue(true);
	static AosValueSelDocid 		sgAosValueSelDocid(true);
	static AosValueSelObjid			sgAosValueSelObjid(true);
	static AosValueSelCid			sgAosValueSelCid(true);
	static AosValueSelStrMap		sgValueSelStrMap(true);			// Ken Lee, 2013/09/16
	static AosValueSelIILEntryMap	sgValueSelIILEntryMap(true);	// Ken Lee, 2013/11/29
	static AosValueSelCounterKey	sgValueSelCounterKey(true);		// Ken Lee, 2013/11/29
	static AosValueSelIILPrefixValue sgValueSelIILPrefixValue(true);		// Andy Zhang, 2013/12/23
	// static AosUnicomSubstr			sgUnicomSubstr(true);
	*/
	return true;
}


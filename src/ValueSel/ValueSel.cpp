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
// 09/15/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "ValueSel/ValueSel.h"

#include "alarm_c/alarm.h"
#include "Porting/GetTime.h"
#include "Porting/TimeOfDay.h"
#include "Random/RandomRule.h"
#include "Rundata/Rundata.h"
#include "SEUtil/DocTags.h"
#include "SEUtil/SeTypes.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "Util/UtUtil.h"
#include "Util/StrSplit.h"
#include "ValueSel/AllValueSel.h"
#include "ValueSel/ValueSelInit.h"
#include "XmlUtil/XmlTag.h"

// extern AosValueSelPtr 	sgValueSel[AosValueSelType::eMax+1];
// static OmnMutex    	 	sgLock;

#include "ValueSel/AllValueSel.h"

AosValueSel::AosValueSel(
		const OmnString &name, 
		const AosValueSelType::E type, 
		const bool reg)
:
AosValueSelObj(name, type, reg)
{
	AosValueSelObjPtr thisptr(this, false);                      
	if (reg) 
	{
		mIsTemplate = true;
		registerValueSel(thisptr);
	}
}


AosValueSel::~AosValueSel()
{
}


OmnString 
AosValueSel::getRandXmlStrStatic(
		const OmnString &tagname, 
		const int level,
		AosRandomRulePtr &rule, 
		const AosRundataPtr &rdata)
{
	/* Chen Ding, 05/31/2012
//	int i = rand()%(AosValueSelType::eMax-1)+1;
	int i = 6;
	rule = OmnNew AosRandomRule(AosValueSelType::toString(sgValueSel[i]->mType)); 
	return sgValueSel[i]->getXmlStr(tagname, level, rule, rdata);
	*/
	OmnNotImplementedYet;
	return "";
}


bool
AosValueSel::getValueForCounterVarStatic(
		const OmnString &xpath, 
		OmnString &value, 
		const AosRundataPtr &rdata)
{
	// This function is used to retrieve value for counters. 
	// 'xpath' is in the form:
	// 		did:xpath
	// 	
	// where 'did' indicates how to get the doc, and 'xpath' 
	// specifies the xpath in that doc.
	// In the current implementations:
	// 	01: Received Doc
	// 	02: Retrieved Doc
	// 	03: Created Doc
	aos_assert_r(xpath != "" && rdata, false);
	AosStrSplit split;
	bool finished;
	OmnString pair[3];
	split.splitStr(xpath.data(), ":", pair, 3, finished);
	aos_assert_r(pair[0] != "" && pair[1] != "" && pair[2] != "", false);
	AosXmlTagPtr docXml;
	int docType = atoi(pair[1].data());
	OmnString path = pair[2];
	switch(docType)
	{
	case 1 : docXml = rdata->getReceivedDoc(); break;
	case 2 : docXml = rdata->getRetrievedDoc(); break;
	case 3 : docXml = rdata->getCreatedDoc(); break;
	default :
			 rdata->setError() << "Unrecognized doc type" << pair[0];
	}
	aos_assert_r(docXml, false);
	bool exist;
	value = docXml->xpathQuery(path, exist, "");
	return exist;
}


OmnString
AosValueSel::getTagnameStatic(
		const OmnString &tagname,
		const AosRandomRulePtr &rule)
{
	if (tagname != "") return tagname;
	if (rule) return rule->getNodename("valuesel");
	return "valuesel";
}


void
AosValueSel::addHeader(
		OmnString &str,
		const OmnString &tagname,
		const OmnString &type,
		const AosRandomRulePtr &rule)
{
	str << tagname << " " << AOSTAG_VALUE_TYPE << "=\"" << type << "\" ";
	if (rule)
	{
		XmlGenRuleValueIter itr;
		for (itr=rule->mAttrs_r.begin(); itr != rule->mAttrs_r.end(); ++itr)
		{
			str << itr->first << "=\"" << (itr->second).getRandomValue()<<"\" ";
		}

		for(itr = rule->mAttrs_o.begin(); itr != rule->mAttrs_o.end(); ++itr)
		{
			if(rand()%10<8)
			{
				str << itr->first <<"=\"" << (itr->second).getRandomValue() <<"\" ";
			}
		}
	}
}


// Chen Ding, 05/30/2012
bool
AosValueSel::run(
		AosValueRslt &value,
		const char *record, 
		const int record_len,
		const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;
}


bool
AosValueSel::run(AosValueRslt &value, const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;
}


// Chen Ding, 05/31/2012
OmnString 
AosValueSel::getXmlStr(
		const OmnString &tagname, 
		const int level,
		const AosRandomRulePtr &rule, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return "";
}


bool
AosValueSel::init()
{
	// AosValueSelObjPtr obj = OmnNew AosValueSelInit();
	// AosValueSelObj::setValueSel(obj);
	AosAllValueSels::createSelectors();
	return true;
}


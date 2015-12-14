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
#include "ValueSel/ValueSelSeqno.h"

#include "Actions/ActUtil.h"
#include "Actions/ActSeqno.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "DataGenSeqno/DataGenSeqno.h"
#include "Util/OmnNew.h"
#include "Alarm/Alarm.h"
#include "Util/DynArray.h"
#include "Util/UtUtil.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"

AosValueSelSeqno::AosValueSelSeqno(const bool reg)
:
AosValueSel(AOSACTOPRID_SEQNO, AosValueSelType::eSeqno, reg)
{
}


AosValueSelSeqno::AosValueSelSeqno(
        const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
:
AosValueSel(AOSACTOPRID_SEQNO, AosValueSelType::eSeqno, false)
{
}


AosValueSelSeqno::~AosValueSelSeqno()
{
}


bool
AosValueSelSeqno::run(
		AosValueRslt    &valueRslt,
		const AosXmlTagPtr &item,
		const AosRundataPtr &rdata)
{
	//<name id="seqno" seqnoid="xxx"/>
	//<name AOSTAG_VALUE_TYPE="seqno" seq_num="xxx" AOSTAG_XPATH="" AOSTAG_VALUE_TYPE="xxx"/>
	//<docselector AOSTAG_DOCSELECTOR_TYPE="xxx">
	//</name>
	
	//Zky3293, Linda, 2011/04/25
	aos_assert_r(item, false);
	aos_assert_r(rdata, false);
	AosDataType::E datatype = AosDataType::toEnum(item->getAttrStr(AOSTAG_DATA_TYPE));
	if (!AosDataType::isValid(datatype))
	{
		datatype = AosDataType::eString;
	}

	if (datatype == AosDataType::eXmlDoc)
	{
		rdata->setError() << "Datatype incorrect: " << datatype;
		return false; 
	}

	return  AosDataGenSeqno::createSeqnoStatic(valueRslt, item, rdata);

#if 0
	AosXmlTagPtr doc;
	if (item->getAttrStr("zky_smartdoc")== "true")
	{
		doc = item;
		aos_assert_r(doc, false);
		doc = doc->getRoot();
		if (!doc)                                                       
		{                                                               
			rdata->setError() << "Failed to retrieve doc!";             
			return false; 
		}
	}
	else
	{
		// Retrieving the doc selector
		AosXmlTagPtr selector = item->getFirstChild(AOSTAG_DOCSELECTOR);
		if (!selector)                                  
		{                                               
			rdata->setError() << "Missing Doc selector";
			return false;                               
		}                                               

		// Retrieve the doc to be modified                              
		doc = AosDocSelector::doSelectDoc(selector, rdata);
		if (!doc)                                                       
		{                                                               
			rdata->setError() << "Failed to retrieve doc!";             
			return false; 
		}
	}

	OmnString path = item->getAttrStr(AOSTAG_XPATH);	
	if (doc->isRootTag()) doc = doc->getFirstChild();
	aos_assert_r(doc, false);
	/*
	seqnodoc = seqnodoc->getFirstChild();
	if (path!="")
	{
		if (seqnodoc)
		{
			OmnStrParser parser(path, "/", false, false);
			OmnString word;
			while((word = parser.nextWord()) != "")
			{	
				seqnodoc = seqnodoc->getFirstChild(word);
				if (!seqnodoc)
				{
					break;
				}
			}
		}
	}
	*/

	AosXmlTagPtr seqnodoc = doc;
	if (path != "" && path !="_ROOT")
	{
		seqnodoc = doc->xpathGetChild(path);
	}

	if (!seqnodoc)
	{
		rdata->setError() <<"Missing Seqno Path!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	int seq_num = item->getAttrInt("seq_num", 0);

	AosActSeqno seqno_action;
	u64 seqno = 0;
	bool rslt = seqno_action.getSeqno(seqno, seqnodoc, rdata, seq_num);
	aos_assert_r(rslt, false);
	OmnString seqnostr;	
	if (seqno) seqnostr << seqno;
	return 	valueRslt.setValue(datatype, seqnostr, rdata);
#endif
}


OmnString 
AosValueSelSeqno::getXmlStr(
		const OmnString &tagname, 
		const int level,
		const AosRandomRulePtr &rule, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return "";
}


AosValueSelObjPtr
AosValueSelSeqno::clone(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosValueSelSeqno(sdoc, rdata);
	}

	catch (...)
	{
		AosSetErrorU(rdata, "failed_creating_obj") << enderr;
		return 0;
	}
}








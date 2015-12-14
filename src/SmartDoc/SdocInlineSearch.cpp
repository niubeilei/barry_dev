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
// 11/17/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SmartDoc/SdocInlineSearch.h"

#include "Actions/ActSeqno.h"
#include "EventMgr/EventTriggers.h"
#include "SEUtil/IILName.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/QueryRsltObj.h"
#include "SEInterfaces/BitmapObj.h"
#include "UtilComm/TcpClient.h"
#include "ValueSel/ValueSel.h"
#include "ValueSel/ValueRslt.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlInterface/WebRequest.h"

#include "DbQuery/Query.h"
#include "SEInterfaces/IILClientObj.h"

#if 0

AosSdocInlineSearch::AosSdocInlineSearch(const bool flag)
:
AosSmartDoc(AOSSDOCTYPE_INLINESEARCH, AosSdocId::eInlineSearch, flag)
{
}


AosSdocInlineSearch::~AosSdocInlineSearch()
{
}

bool
AosSdocInlineSearch::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata) 
{
	if (!sdoc)
	{
		rdata->setError() << "Missing Smartdoc!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	OmnString ctnr;
	AosXmlTagPtr ctnrXml = sdoc->getFirstChild("ctnr");
	if(ctnrXml)
	{
		AosValueRslt valueRslt;
		bool rslt = AosValueSel::getValueStatic(valueRslt, ctnrXml, rdata);
		if(rslt)
		{
			if (valueRslt.isValid() && !valueRslt.isXmlDoc())
			{                                                  
				ctnr = valueRslt.getValueStr1();
			}                                                  
		}
	}
	if(ctnr == "")
	{
		rdata->setError() << "Missing Container!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	OmnString keyStr;
	AosXmlTagPtr keyXml = sdoc->getFirstChild("key");
	if(keyXml)
	{
		AosValueRslt valueRslt;
		bool rslt = AosValueSel::getValueStatic(valueRslt, keyXml, rdata);
		if(rslt)
		{
			if (valueRslt.isValid() && !valueRslt.isXmlDoc())
			{                                                  
				keyStr = valueRslt.getValueStr1();
			}                                                  
		}
	}
	OmnStrParser1 parser(keyStr, ", ");
	OmnString key;
	vector<OmnString> keys;
	while ((key = parser.nextWord()) != "")
	{
		keys.push_back(key);
	}
	if(keys.size() == 0)
	{
		rdata->setError() << "Missing keys!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	OmnString seqId;
	AosXmlTagPtr seqIdXml = sdoc->getFirstChild("seqid");
	if(seqIdXml)
	{
		AosValueRslt valueRslt;
		bool rslt = AosValueSel::getValueStatic(valueRslt, seqIdXml, rdata);
		if(rslt)
		{
			if (valueRslt.isValid() && !valueRslt.isXmlDoc())
			{                                                  
				seqId = valueRslt.getValueStr1();
			}                                                  
		}
	}
	if(seqId == "")
	{
		rdata->setError() << "Missing seqId!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	OmnString splitType;
	AosXmlTagPtr typeXml = sdoc->getFirstChild("splitType");
	if(typeXml)
	{
		AosValueRslt valueRslt;
		bool rslt = AosValueSel::getValueStatic(valueRslt, typeXml, rdata);
		if(rslt)
		{
			if (valueRslt.isValid() && !valueRslt.isXmlDoc())
			{                                                  
				splitType = valueRslt.getValueStr1();
			}                                                  
		}
	}
	
	OmnString event = rdata->getEvent();
	AosEventTrigger::E e = AosEventTrigger::toEnum(event);
	switch(e)
	{
	case AosEventTrigger::eDocCreated :
		 return docCreated(sdoc, ctnr, keys, seqId, splitType, rdata);

	case AosEventTrigger::eDocModify :
		 return docModify(sdoc, ctnr, keys, seqId, splitType, rdata);
		 
	case AosEventTrigger::eDocDeleted :
		 return docDeleted(sdoc, ctnr, keys, seqId, splitType, rdata);
		 
	default :
		 return false;
	}
	return false;
}


bool
AosSdocInlineSearch::docCreated(
		const AosXmlTagPtr &sdoc,
		const OmnString &ctnr,    
		vector<OmnString> &keys,
		const OmnString &seqId,
		const OmnString &splitType,
		const AosRundataPtr &rdata)
{
	AosXmlTagPtr new_doc = rdata->getDocByVar(AOSARG_NEW_DOC);
	aos_assert_r(new_doc, false);

	u64 seqid = new_doc->getAttrU64(seqId, 0);
	if(!seqid || (seqid >> 32) > 0)
	{
		rdata->setError() << "seqid error!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	OmnString iilname, key, value;
	for(int i=0; i<(int)keys.size(); i++)
	{
		key = keys[i];
		if(key == "") continue;
		value = new_doc->getAttrStr(key, "");
		if(value == "") continue;

		iilname = AosIILName::composeInlineSearchContainerAttrIILName(ctnr, key);
		//AosIILClient::getSelf()->addInlineSearchValue(iilname, value, seqid, splitType, rdata);
		AosIILClientObj::getIILClient()->addInlineSearchValue(iilname, value, seqid, splitType, rdata);
	}
	return true;
}


bool
AosSdocInlineSearch::docModify(
		const AosXmlTagPtr &sdoc,
		const OmnString &ctnr,    
		vector<OmnString> &keys,
		const OmnString &seqId,
		const OmnString &splitType,
		const AosRundataPtr &rdata)
{
	AosXmlTagPtr old_doc = rdata->getDocByVar(AOSARG_OLD_DOC);
	aos_assert_r(old_doc, false);
	
	AosXmlTagPtr new_doc = rdata->getDocByVar(AOSARG_NEW_DOC);
	aos_assert_r(new_doc, false);
	
	u64 old_seqid = old_doc->getAttrU64(seqId, 0);
	if(!old_seqid || (old_seqid >> 32) > 0)
	{
		rdata->setError() << "Old seqid error!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	u64 new_seqid = new_doc->getAttrU64(seqId, 0);
	if(!new_seqid || (new_seqid >> 32) > 0)
	{
		rdata->setError() << "New seqid error!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	OmnString iilname, key, old_value, new_value;
	for(int i=0; i<(int)keys.size(); i++)
	{
		key = keys[i];
		if(key == "") continue;
		old_value = old_doc->getAttrStr(key, "");
		new_value = new_doc->getAttrStr(key, "");
		if(old_value == new_value && old_seqid == new_seqid) continue;
		
		iilname = AosIILName::composeInlineSearchContainerAttrIILName(ctnr, key);
		AosIILClientObj::getIILClient()->modifyInlineSearchValue(iilname, old_value, new_value, old_seqid, new_seqid, splitType, rdata);
	}
	return true;
}


bool
AosSdocInlineSearch::docDeleted(
		const AosXmlTagPtr &sdoc,
		const OmnString &ctnr,    
		vector<OmnString> &keys,
		const OmnString &seqId,
		const OmnString &splitType,
		const AosRundataPtr &rdata)
{
	AosXmlTagPtr doc = rdata->getDocByVar(AOSARG_NEW_DOC);
	aos_assert_r(doc, false);
	
	u64 seqid = doc->getAttrU64(seqId, 0);
	if(!seqid || (seqid >> 32) > 0)
	{
		rdata->setError() << "Seqid error!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	OmnString iilname, key, value;
	for(int i=0; i<(int)keys.size(); i++)
	{
		key = keys[i];
		if(key == "") continue;
		value = doc->getAttrStr(key, "");
		if(key == "") continue;
		
		iilname = AosIILName::composeInlineSearchContainerAttrIILName(ctnr, key);
		AosIILClientObj::getIILClient()->removeInlineSearchValue(iilname, value, seqid, splitType, rdata);
	}
	return true;
}


bool
AosSdocInlineSearch::reBuild(
		const OmnString &sdocobjid, 
		const OmnString &ctnrobjid,
		const AosRundataPtr &rdata)
{
	AosXmlTagPtr sdoc = AosDocClientObj::getDocClient()->getDocByObjid(sdocobjid, rdata);	
	aos_assert_r(sdoc, false);
	rdata->setEvent("create");
	
	AosSdocInlineSearch inlineSearch(false);

	// Chen Ding, 11/28/2012
	AosQueryRsltObjPtr queryRslt = AosQueryRsltObj::getQueryRsltStatic();
	aos_assert_r(queryRslt, false);

	bool rslt = AosQuery::getSelf()->runQuery(
		"", AOSTAG_HPCONTAINER, ctnrobjid,
		eAosOpr_eq, false, queryRslt, NULL, rdata);
	aos_assert_r(rslt, false);

	u64 docid;
	bool finished = false;
	while(!finished)
	{
		queryRslt->nextDocid(docid, finished, rdata); 
		if (finished) return false;
		AosXmlTagPtr doc = AosDocClientObj::getDocClient()->getDocByDocid(docid, rdata);
		aos_assert_r(doc, false);
		rdata->setDocByVar(AOSARG_NEW_DOC, doc, true);
		inlineSearch.run(sdoc, rdata);
	}
	return true;
}
#endif

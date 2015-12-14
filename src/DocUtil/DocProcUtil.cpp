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
// 02/15/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DocUtil/DocProcUtil.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "DocUtil/SengUtil.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlUtil.h"


//static const int smMaxMetaAttrs = 50;
//static AosDocMetaAttr AosXmlUtil::smMetaAttrs[smMaxMetaAttrs];
//static int smNumMetaAttrs = 0;

bool
AosDocProcUtil::addMetaAttrs(
		//vector<AosBuffPtr> &buffs,
		vector<AosTransPtr>  *allTrans,
		const u32 arr_len,
		const AosXmlTagPtr &doc,
		const u64 &docId,
		const AosRundataPtr &rdata) 
{
	OmnString valueStr;

	for (int i=0; i<AosXmlUtil::smNumMetaAttrs; i++)
	{
		valueStr = doc->getAttrStr(AosXmlUtil::smMetaAttrs[i].attrname);
		if(valueStr != "")
		{
			//OmnScreen << "Add meta: " << 
			//		smMetaAttrs[i].attrname << ":" << valueStr << ":" << docId << endl;
			addValueToIIL(allTrans, arr_len, AosXmlUtil::smMetaAttrs[i].iilname, valueStr, docId, 
				AosXmlUtil::smMetaAttrs[i].value_unique, 
				AosXmlUtil::smMetaAttrs[i].docid_unique, 
				rdata);
		}
	}
	return true;
}


bool
AosDocProcUtil::removeMetaAttrs(
		//vector<AosBuffPtr> &buffs,
		vector<AosTransPtr>  *allTrans,
		const u32 arr_len,
		const AosXmlTagPtr &doc,
		const u64 &docid,
		const AosRundataPtr &rdata) 
{
	OmnString valueStr;
	OmnString word;

	for (int i=0; i<AosXmlUtil::smNumMetaAttrs; i++)
	{
		valueStr = doc->getAttrStr(AosXmlUtil::smMetaAttrs[i].attrname);
		if(valueStr != "")
		{
			// if (smShowLog) OmnScreen << "Remove meta: " << AosXmlUtil::smMetaAttrs[i].attrname << ":"
			// 	<< valueStr << ":" << docid << endl;
			// Chen Ding, 08/09/2011
			//removeValueFromIIL(buffs, AosXmlUtil::smMetaAttrs[i].iilname, valueStr, docid, rdata);
			removeValueFromIIL(allTrans, arr_len, AosXmlUtil::smMetaAttrs[i].iilname, valueStr, docid, rdata);
		}
	}
	return true;
}


bool
AosDocProcUtil::modifyMetaAttrs(
		//vector<AosBuffPtr> &buffs,
		vector<AosTransPtr>  *allTrans,
		const u32 arr_len,
		const AosXmlTagPtr &olddoc, 
		const AosXmlTagPtr &newdoc, 
		const u64 &docid,
		const AosRundataPtr &rdata) 
{
	OmnString word;
	OmnString word2;
	OmnString oldValue;
	OmnString newValue;

	for (int i=0; i<AosXmlUtil::smNumMetaAttrs; i++)
	{
		oldValue = olddoc->getAttrStr(AosXmlUtil::smMetaAttrs[i].attrname);
		newValue = newdoc->getAttrStr(AosXmlUtil::smMetaAttrs[i].attrname);
		if(oldValue != newValue)
		{

			if (oldValue != "")
			{
				// if (smShowLog) OmnScreen << "Remove meta: " << oldValue << ":" << docid << endl;
				//removeValueFromIIL(buffs, smMetaAttrs[i].iilname, oldValue, docid, rdata);
				removeValueFromIIL(allTrans, arr_len, AosXmlUtil::smMetaAttrs[i].iilname, oldValue, docid, rdata);
			}

			// add new
			if(newValue != "")
			{
				// if (smShowLog) OmnScreen << "Add  meta: " << newValue << ":" << docid << endl;
				addValueToIIL(allTrans, arr_len, AosXmlUtil::smMetaAttrs[i].iilname, newValue, docid, 
						AosXmlUtil::smMetaAttrs[i].value_unique,
						AosXmlUtil::smMetaAttrs[i].docid_unique, 
						rdata);
			}
		}
	}

	return true;
}


bool
AosDocProcUtil::collectWords(
		AosSeWordHash &wordHash,
		const AosWordParserPtr &wordParser,
		const AosWordNormPtr &wordNorm,
		const AosXmlTagPtr &parent_doc,
		const AosXmlTagPtr &xml,
		const bool marked)
{
	char *buff1;
	xml->resetGetWords();
	if (parent_doc && !marked)
	{
		xml->markAttrs(parent_doc);
	}
	char wordbuff1[AosXmlTag::eMaxWordLen+1];
	char wordbuff2[AosXmlTag::eMaxWordLen+1];
	int wordlen = 0;
	AosEntryMark::E mark;
	while (xml->nextWord(wordParser, wordbuff1, AosXmlTag::eMaxWordLen, wordlen, mark))
	{
		buff1 = (char *)wordNorm->normalizeWord(wordbuff1, wordlen, 
				wordbuff2, AosXmlTag::eMaxWordLen);
		if (buff1) wordHash.add(buff1, wordlen, mark);
	}
	return true;
}


bool
AosDocProcUtil::collectWords(
		AosSeWordHash &attrWordHash,
		const AosWordParserPtr &wordParser,
		const AosWordNormPtr &wordNorm,
		const OmnString &value)
{
	char *buff1 = NULL;
	char wordbuff1[AosXmlTag::eMaxWordLen+1];
	char wordbuff2[AosXmlTag::eMaxWordLen+1];
	int wordlen = 0;
	wordParser->setSrc(value.data(), 0, value.length());
	while (wordParser->nextWord(wordbuff1, wordlen))
	{
		buff1 = (char *)wordNorm->normalizeWord(wordbuff1, wordlen, 
				wordbuff2, AosXmlTag::eMaxWordLen);
		if (buff1) attrWordHash.add(buff1, wordlen, (AosEntryMark::E)0);
	}
	return true;
}


bool
AosDocProcUtil::collectAttrs(
		AosSeAttrHash &attrs,
		const AosXmlTagPtr &parent_doc,
		const AosXmlTagPtr &xml,
		const bool marked)
{
	xml->resetAttrNameValueLoop();
	if (parent_doc && !marked)
	{
		xml->markAttrs(parent_doc);
	}
	OmnString aname, avalue;
	bool finished = false;
	OmnString iilname;
	AosEntryMark::E mark;
	while (!finished)
	{
		if (!xml->nextAttr("", aname, avalue, finished, false, mark))
		{
			OmnAlarm << "Failed to retrieve attr" << enderr;
		}
		if (finished) break;
		attrs.add(aname, avalue, mark);
	}
	return true;
}


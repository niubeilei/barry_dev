////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// A document is stored in two forms: header and whole body. This class
// handles the header. There are two groups of fields: integral and 
// string fields. Header storage is fixed size (determined by 
// 'eDocHeaderSize'). This size is guaranteed to fit all integral 
// fields. If the total header size is not bigger than this value, 
// the entire header is stored in the header block. Otherwise, only
// the integral fields are stored in the header block. 
//
// When retrieving a header, it checks the 'mStorageFlag' field. If it
// is 'c', it means the string fields are stored in the Doc itself. 
// It needs to retrieve the doc and then set the header contents. 
//
// Modification History:
// 07/20/2011	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#if 0
#include "SearchEngine/PartialDocMgr.h"

#include "alarm_c/alarm.h"
#include "SEInterfaces/DocClientObj.h"
#include "DocProc/LocalDocMgr.h"
#include "IdGen/U64IdGen.h"
#include "IdGen/IdGen.h"
#include "Porting/GetTime.h"
#include "Porting/TimeOfDay.h"
#include "Rundata/Rundata.h"
#include "SEUtil/SeTypes.h"
#include "SEUtil/Docid.h"
#include "SEUtil/SeConfig.h"
#include "SEUtil/DocTags.h"
#include "SEUtil/SeUtil.h"
#include "SEUtil/SeTypes.h"
#include "SmartDoc/SmartDoc.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlUtil.h"

bool
AosPartialDocMgr::processPartialDoc(
		AosXmlTagPtr &partial_doc, 
		const AosRundataPtr &rdata, 
		bool &doc_modified)
{
	//It mean the doc is partical
	//1. you can get the query fileds
	//2. copy origin doc to modify by fileds
	doc_modified = false;
	OmnString fields;
	bool rslt = AosXmlUtil::verifySignedAttr(partial_doc, fields, 
			AOSTAG_PARTIAL_DOC_FNAMES, AOSTAG_PARTIAL_DOC_SIGNATURE, rdata);
	if (!rslt)
	{
		rdata->setError() << "Document failed security checking!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	if (fields == "")
	{
		// Need to check whether it is a full doc. 
		rslt = AosXmlUtil::verifySignedAttr(partial_doc, 
				AOSTAG_DOCID, AOSTAG_FULLDOC_SIGNATURE, rdata);
		if (!rslt)
		{
			rdata->setError() << "Document failed security checking!";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}

		partial_doc->removeAttr(AOSTAG_FULLDOC_SIGNATURE);
		doc_modified = true;
		return true;
	}

	partial_doc->removeAttr(AOSTAG_PARTIAL_DOC_FNAMES);
	partial_doc->removeAttr(AOSTAG_PARTIAL_DOC_SIGNATURE);
	u64 docid = partial_doc->getAttrU64(AOSTAG_DOCID, AOS_INVDID);
	aos_assert_r(docid != AOS_INVDID, false);

	AosXmlTagPtr exist_doc = AosLocalDocMgr::getSelf()->getDoc(docid, "", rdata);
	aos_assert_r(exist_doc, false);
		
	exist_doc = exist_doc->clone();
	aos_assert_r(exist_doc, false);
	rslt = modifyDocByFields(exist_doc, partial_doc, fields, rdata);
	if (rslt)
	{
		OmnString data = exist_doc->toString();
		AosXmlParser parser;
		AosXmlTagPtr root = parser.parse(data, "");
		if (root)
		{
			partial_doc = root;
		}
	}
	return rslt;
}


bool	
AosPartialDocMgr::modifyDocByFields(
		AosXmlTagPtr &exist_doc,
		const AosXmlTagPtr &partial_doc,
		const OmnString &fields, 
		const AosRundataPtr &rdata)
{
	// Chen Ding, 08/30/2011
	OmnNotImplementedYet;
	return false;
	//Linda 11/26/2010
	// This function modifies the doc 'exist_doc' by the fields 'fields', which is 
	// in the form:
	// oname|$|cname|$|type|exist|$|join_idx:oname|$|cname|$|type|exist|$|join_idx...
	// where:
	// 	'oname' 	is the original name of the field
	// 	'cname' 	is the converted name of the field
	// 	'type' 		can be one of 'eAttr', 'eText', 'eXml', and 'eCdata'
	// 	'exist' 	indicates whether the field must exist (not used here)
	// 	'join_idx' 	is the docid of the joined doc. If it is 0, it means there is no join for
	// 	           	this field. Otherwise, the field is retrieved from the joined doc, 
	// 	           	which means that the modifications should be modifying the joined
	// 	           	doc.
	//
	// Below outlines how to process it:
	// 	1. If 'join_idx' is not 0, retrieve the joined doc and modify the joined doc.
	// 	2. Use 'cname' to retrieve the value from 'partial_doc'. If it is empty, 
	// 	   it means the attribute is removed. Otherwise, it is to modify the doc.
	// 	3. For any attributes that are not in 'fields', add the attributes to the doc. 
	aos_assert_r(fields != "", false);
	OmnStrParser1 parser(fields, ":");
	OmnString word;
	u64 did = exist_doc->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_r(did, false);

	bool needModify = false;
	while((word = parser.nextWord()) != "")
	{
		AosStrSplit split;
		bool finished;
		OmnString parts[5];
		split.splitStr(word.data(), "|$|", parts, 5, finished);

		if (parts[4] != "0")
		{
			u64 joined_did = AosDocid::convertToU64(parts[4]);
			if (!joined_did)
			{
				rdata->setError() << "Failed converting docid: " << parts[4];
				OmnAlarm << rdata->getErrmsg() << enderr;
				continue;
			}

			if (joined_did != did)
			{
				AosXmlTagPtr joined_doc = AosDocMgr1::getSelf()->getDoc(joined_did, "", rdata);
				if (joined_doc)
				{
					// It is a joined field. Need to modify the joined doc.
					//modifyJoinedDoc(...);
					bool exist;
					OmnString cname = parts[1];
					aos_assert_r(cname != "", false);
					OmnString newvalue = partial_doc->xpathQuery(cname, exist, "");

					OmnString oname = parts[0];
					aos_assert_r(oname != "", false);
					OmnString joined_value = joined_doc->xpathQuery(oname, exist, "");
					
					if (newvalue != joined_value)
					{
						AosDocClientObj::getDocClient()->modifyAttrStr1(rdata, joined_did, 
								joined_doc->getAttrStr(AOSTAG_OBJID), oname, newvalue,
								"", false, false, __FILE__, __LINE__);
					}
					partial_doc->xpathRemoveAttr(cname);
					continue;
				}
				rdata->setError() << "Failed retrieving the joined doc: " << joined_did;
				OmnAlarm << rdata->getErrmsg() << enderr;
				continue;
			}
		}
		modifyDocByFields(exist_doc, partial_doc, parts[0], parts[1], parts[2],  needModify, rdata);
	}
	bool rslt = traversalXml(exist_doc, partial_doc);
	aos_assert_r(rslt, false);
	return true;
}



bool
AosPartialDocMgr::modifyDocByFields(
		AosXmlTagPtr &exist_doc,
		const AosXmlTagPtr &partial_doc,
		const OmnString &oname, 
		const OmnString &cname,
		const OmnString &type, 
		bool &modified, 
		const AosRundataPtr &rdata)
{
	if (type == "eAttr")
	{
		aos_assert_r(cname != "", false);
		bool exist;
		OmnString newvalue = partial_doc->xpathQuery(cname, exist, "");

		aos_assert_r(oname != "", false);
		OmnString exist_value = exist_doc->xpathQuery(oname, exist, "");
		if (exist_value != newvalue)
		{
			// This means the field value was changed. 
			if (newvalue == "")
			{
				// This means the field was removed. We need to remove the 
				// field from 'exist_doc'.
				exist_doc->xpathRemoveAttr(oname);
			}
			else
			{
				// The field was modified. need to modify 'exist_doc'.
				exist_doc->xpathSetAttr(oname, newvalue);
			}
			modified = true;
		}
		partial_doc->xpathRemoveAttr(cname);
		return true;
	}

	if (type == "eText")
	{
		aos_assert_r(cname != "", false);
		OmnString querystr = cname;
		querystr << "/_#text";
		bool exist;
		OmnString newvalue = partial_doc->xpathQuery(querystr, exist, "");

		aos_assert_r(oname != "", false);
		OmnString oname_full = oname;
		oname_full << "/_#text";
		OmnString exist_value = exist_doc->xpathQuery(oname_full, exist, "");
		if (exist_value != newvalue)
		{
			if (newvalue !="") 
			{
				exist_doc->xpathSetAttr(oname_full, newvalue);
			}
			else
			{
				//exist_doc->xpathRemoveText(oname_full);
				//exist_doc->removeNode(word, false, false);
				exist_doc->xpathRemoveNode(oname);
			}
			modified = true;
		}
		partial_doc->xpathRemoveNode(cname);
		return true;
	}

	rdata->setError() << "Unrecognized field type: " << type;
	OmnAlarm << rdata->getErrmsg() << enderr;
	return false;
}


bool
AosPartialDocMgr::traversalXml(
		AosXmlTagPtr &exist_doc,
		const AosXmlTagPtr &partial_doc)
{
	AosXmlTagPtr doc;
	OmnString aname, avalue;
	bool finished = false;
	OmnString path;

	partial_doc->resetAttrNameValueLoop();
	while(!finished)
	{
		if (!partial_doc->nextAttr(path, aname, avalue, finished, true)||finished) break;
		if (aname!="" && avalue!="") 
		{
			//root attr
			OmnStrParser1 parser1(aname, "/", false, false);
			OmnString word1 = parser1.nextWord();
			if (parser1.nextWord() != "") break;
			exist_doc->setAttr(aname, avalue);
		}
	}
	//add subtag
	doc = partial_doc->getFirstChild();
	if (!doc) return true;
	do
	{
		OmnString tagname = doc->getTagname();
		if (tagname!="")
		{
			OmnString subtag = doc->toString();
			exist_doc->addNode1(tagname, subtag);
		}
		doc = partial_doc->getNextChild();
	}while(doc);	
	return true;
}

#endif

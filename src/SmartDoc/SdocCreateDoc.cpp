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
#include "SmartDoc/SdocCreateDoc.h"

#include "SEInterfaces/DocClientObj.h"
#include "Rundata/Rundata.h"
#include "UtilComm/TcpClient.h"
#include "Actions/ActSeqno.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlInterface/WebRequest.h"

AosSdocCreateDoc::AosSdocCreateDoc(const bool flag)
:
AosSmartDoc(AOSSDOCTYPE_CREATEDOC, AosSdocId::eCreateDoc, flag)
{
}


AosSdocCreateDoc::~AosSdocCreateDoc()
{
}

bool
AosSdocCreateDoc::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata) 
{
	// req format is 
	// <request>
	// 	<objdef>
	// 		...
	// 	</objdef>
	// 	<itme>
	// 		...
	// 	</itme>
	// </request>
	//
	// req should have arguments: 
	// 1. orginal doc in <objdef>
	// 2. sdoc_objid in <item name='args'><![CDATA[xxxx]]</item>
	// 3. the reqid operation <item name='reqid'><![CDATA[create_order]]>
	
	if (!sdoc)
	{
		rdata->setError() << "Missing Smartdoc!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosXmlTagPtr root = rdata->getReceivedDoc();
	if (!root)
	{
		rdata->setError() << "Can't Retrieve Accessed Doc!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosXmlTagPtr child = root->getFirstChild();
	if (!child)
	{
		rdata->setError() << "Request incorrect!";
		return false;
	}

	AosXmlTagPtr objdef = child->getFirstChild("objdef");
	if (!objdef)
	{
		rdata->setError() << "Request incorrect!";
		return false;
	}

	AosXmlTagPtr doc_to_create = objdef->getFirstChild();
	if (!doc_to_create)
	{
		rdata->setError() << "Failed to Get Original doc!";
		return false;
	}

	OmnString cid = rdata->getCid();
	u64 seqno;
	AosXmlTagPtr tick_doc;

	bool rslt = getSeqno(sdoc, seqno, rdata);
	aos_assert_r(rslt, false);
	OmnString real_seqno = sdoc->getAttrStr(AOSTAG_SEQNO_PREFIX, "SO");
	real_seqno << seqno;
	OmnString ticket_aname = sdoc->getAttrStr(AOSTAG_TICKET_ANAME, AOSTAG_TICKS);
	doc_to_create->setAttr(ticket_aname, real_seqno);

	OmnString userdata = root->getChildTextByAttr("name", "userdata");
	doc_to_create->setAttr(AOSTAG_PARENTC, userdata);

	OmnString docstr = doc_to_create->toString();
	tick_doc = AosDocClientObj::getDocClient()->createDocSafe1(rdata, docstr, 
		    cid, "", true, true, false, false, false, true);

	aos_assert_r(tick_doc, false);
	OmnString zky_objid = tick_doc->getAttrStr(AOSTAG_OBJID);
	OmnString zky_docid = tick_doc->getAttrStr(AOSTAG_DOCID);

	OmnString results;
	results << "<Contents " << AOSTAG_OBJID
		<< "=\"" << zky_objid 
		<< "\" " << AOSTAG_DOCID << "=\"" << zky_docid << "\"/>";
	rdata->setResults(results);
	return true;
}


bool
AosSdocCreateDoc::getSeqno(
		const AosXmlTagPtr &sdoc,
		u64 &seqno,
		const AosRundataPtr &rdata)
{
	// There shall be a subtag:
	// 	<sdoc ...>
	// 		<AOSTAG_OBJID_SEQNO_ACT .../>
	// 		...
	// 	</sdoc>
	// ICE_WARN
	OmnAlarm << "Not complete yet!" << enderr;
	// AosActSeqno seqno_action(false);
	// seqno = 0;
	//bool rslt = seqno_action.getSeqno(seqno, sdoc, rdata);
	//aos_assert_r(rslt, false);
	return false;
}


AosXmlTagPtr
AosSdocCreateDoc::createDoc(
		const AosXmlTagPtr &sdoc, 
		const OmnString &action_tagname,
		const AosRundataPtr &rdata) 
{
	// This function creates new doc based on the smart doc 'sdoc'. It creates
	// the doc based on the working doc.
	if (!sdoc)
	{
		AosSetError(rdata, AOSLT_MISSING_SDOC);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	if (action_tagname != "")
	{
		AosSetError(rdata, AOSLT_MISSING_CREATEDOC_SDOC);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	// Retrieve the working doc
	AosXmlTagPtr working_doc = rdata->getWorkingDoc();
	if (!working_doc)
	{
		AosSetError(rdata, AOSLT_MISSING_WORKING_DOC);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	// Do the actions before creating the doc
	bool rslt = AosSdocAction::runActions(sdoc, action_tagname, rdata);
	aos_assert_rr(rslt, rdata, 0);

	OmnString docstr = working_doc->toString();
	AosXmlTagPtr doc_created = AosDocClientObj::getDocClient()->createDocSafe1(rdata, docstr, 
		    rdata->getCid(), "", false, true, false, false, true, true);

	aos_assert_rr(doc_created, rdata, 0);
	return doc_created;
}


/*
AosXmlTagPtr
AosSdocCreateDoc::createDoc(
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	// This is a utility function that is used to create a new
	// doc based on the smart doc 'sdoc'. 
}
*/

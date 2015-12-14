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
// 2011/04/25	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SmartDoc/SdocCreateDocs.h"

#include "SEInterfaces/DocClientObj.h"
#include "UtilComm/TcpClient.h"
#include "Actions/ActSeqno.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlInterface/WebRequest.h"



AosSdocCreateDocs::AosSdocCreateDocs(const bool flag)
:
AosSmartDoc(AOSSDOCTYPE_CREATEDOCS, AosSdocId::eCreateDocs, flag)
{
}


AosSdocCreateDocs::~AosSdocCreateDocs()
{
}


bool
AosSdocCreateDocs::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata) 
{
	// This smart doc assumes the received doc is in the form:
	// 	<doc ...>
	// 		...
	// 	</doc>
	//	<entries>
	// 		<entry .../>
	// 		<entry .../>
	// 		...
	//	</entries>
	//
	// This function removes <entries> subtag, create the doc. It then 
	// loop over all the <entry>s. Each will create a new doc.
	
	// Retrieve the smart doc
	if (!sdoc)
	{
		rdata->setError() << "Missing Smartdoc!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// Retrieve the received doc
	// 	<received_doc ...>
	// 		<doc>
	// 			<objdef>
	// 				<doc .../>	// The first doc
	// 			</objdef>
	// 			<objdef2>
	// 				<doc .../>	// The second doc (records)
	// 			</objdef2>
	// 		</doc>
	// 	</received_doc>
	//
	AosXmlTagPtr root = rdata->getReceivedDoc(); 
	if (!root)
	{
		rdata->setError() << "Can't Retrieve Accessed Doc!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	AosXmlTagPtr rootchild = root->getFirstChild();
	if (!rootchild)
	{
		rdata->setError() << "Request incorrect!!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosXmlTagPtr objdef = rootchild->getFirstChild("objdef");
	if (!objdef)
	{
		rdata->setError() << "Request incorrect!!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosXmlTagPtr received_doc = objdef->getFirstChild();
	if (!received_doc)
	{
		rdata->setError() << "Request incorrect!";
		return false;
	}

	AosXmlTagPtr objdef2 = rootchild->getFirstChild("objdef2");
	if (!objdef2)
	{
		rdata->setError() << "Request incorrect!!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosXmlTagPtr entries = objdef2->getFirstChild();
	if (!entries)
	{
		rdata->setError() << "Request incorrect!";
		return false;
	}
	
	AosXmlTagPtr old_received_doc = rdata->setReceivedDoc(received_doc, false);
	//bool is_remove_entrys = sdoc->getAttrBool("zky_isremove_entrys", true);
	// Retrieve the path that identifies <entries>
	//OmnString path = sdoc->getAttrStr(AOSTAG_ENTRIES);
	//AosXmlTagPtr entries;
	//if (path != "")
	//{
	//	entries = received_doc->xpathGetChild(path);
	//	if (entries)
	//	{
	//		OmnString node = entries->toString();
	//		aos_assert_r(node!="", false);
	//		AosXmlParser parser;
	//		entries = parser.parse(node, "" AosMemoryCheckerArgs);
	//		aos_assert_r(entries, false);
	//		if (entries->isRootTag()) entries = entries->getFirstChild();
	//		if (is_remove_entrys)
	//		{
	//			if (!received_doc->xpathRemoveNode(path))
	//			{
	//				rdata->setError() << "Failed to remove entries";
	//				return false;
	//			}
	//		}
	//	}
	//}

	AosXmlTagPtr old_sourcedoc = rdata->setSourceDoc(entries, false);
	bool is_create_maindoc = sdoc->getAttrBool(AOSTAG_CREATE_MAIN, true);
	AosXmlTagPtr created_doc = received_doc; 
	OmnString rslts = "<Contents>";
	if (is_create_maindoc)
	{
		// Process the actions on the doc
		AosXmlTagPtr actions = sdoc->getFirstChild(AOSTAG_CREATEDOC_ACTIONS);
		if (actions)
		{
			AosSdocAction::runActions(actions, rdata);
		}

		// Create the doc
		//OmnString docstr = received_doc->toString();
		AosXmlTagPtr doc = rdata->getCreatedDoc();
		aos_assert_r(doc, false);
		OmnString docstr = doc->toString();
		OmnString cid = rdata->getCid();
		bool is_public = sdoc->getAttrBool(AOSTAG_MAIN_PUBLIC, false);
		bool cid_required = sdoc->getAttrBool(AOSTAG_CID_REQUIRED, true);
		created_doc = AosDocClientObj::getDocClient()->createDocSafe1(
				rdata, docstr, cid, "", is_public, true, 
				false, false, cid_required, true);
		aos_assert_r(created_doc, false);

		rslts << "<entries status=\"true\" zky_objid=\"" 
			<< created_doc->getAttrStr(AOSTAG_OBJID) << "\" />";
	}

	AosXmlTagPtr old_targetdoc = rdata->setTargetDoc(created_doc, false);

	// Create the sub-docs
	if (entries)
	{
		OmnString success, failed;
		failed = "<";
		failed << AOSTAG_FAILED << ">";
		success = "<";
		success << AOSTAG_SUCCESS << ">";
		createEntries(entries, sdoc, success, failed, rdata);
		if (success != "")
		{
			rslts << success << "</" << AOSTAG_SUCCESS << ">";
		}
	
		if (failed != "")
		{
			rslts << failed << "</" << AOSTAG_FAILED << ">";
		}
	}
	rslts << "</Contents>";

	// Process the actions on the doc
	AosXmlTagPtr procactions = sdoc->getFirstChild(AOSTAG_PROCDOC_ACTIONS);
	if (procactions)
	{
		AosSdocAction::runActions(procactions, rdata);
	}
	rdata->setResults(rslts);
	if (old_sourcedoc) rdata->setSourceDoc(old_sourcedoc, false);
	if (old_targetdoc) rdata->setTargetDoc(old_targetdoc, false);
	if (old_received_doc) rdata->setReceivedDoc(old_received_doc, false);
	return true;
}


bool
AosSdocCreateDocs::createEntries(
		const AosXmlTagPtr &entries, 
		const AosXmlTagPtr &sdoc, 
		OmnString &success,
		OmnString &failed,
		const AosRundataPtr &rdata)
{
	// This function creates all the entries in 'entries':
	// 	<entries>
	// 		<entry .../>
	// 		<entry .../>
	// 		...
	// 	</entries>
	//
	// Upon return, it returns the following:
	// 	<Contents>
	// 		<AOSTAG_SUCCESS>
	// 			<entry idname="xxx"/>
	// 			<entry idname="xxx"/>
	// 			...
	// 		</AOSTAG_SUCCESS>
	// 		<AOSTAG_FAILED>
	// 			<entry idname="xxx"/>
	// 			<entry idname="xxx"/>
	// 			...
	// 		</AOSTAG_FAILED>
	// 	</Contents>
	//
	// The 'sdoc' has actions to process the doc before creating:
	// 	<sdoc ...>
	// 		<AOSTAG_ENTRY_ACTIONS
	// 			AOSTAG_ENTRY_PUBLIC="true|false"
	// 			AOSTAG_ENTRY_CID_REQUIRED="true|false"
	// 			AOSTAG_TO_IDNAME="xxx"
	// 			AOSTAG_FROM_IDNAME="xxx"
	// 			<action .../>
	// 			<action .../>
	// 			...
	// 		</AOSTAG_ENTRY_ACTIONS>
	// 	</sdoc>
	//
	
	aos_assert_r(entries, false);
	aos_assert_r(sdoc, false);

	AosXmlTagPtr actions= sdoc->getFirstChild(AOSTAG_ENTRY_ACTIONS);
	AosXmlTagPtr entry = entries->getFirstChild();
	bool is_public = sdoc->getAttrBool(AOSTAG_ENTRY_PUBLIC, false);
	bool cid_required = sdoc->getAttrBool(AOSTAG_ENTRY_CID_REQUIRED, false);
	OmnString to_idname = sdoc->getAttrStr(AOSTAG_TO_IDNAME, AOSTAG_TO_IDNAME);
	OmnString from_idname = sdoc->getAttrStr(AOSTAG_FROM_IDNAME, AOSTAG_FROM_IDNAME);
	OmnString entry_select_aname = sdoc->getAttrStr(AOSTAG_SELECT_FLAG_NAME);
	bool remove_entryattr = sdoc->getAttrBool(AOSTAG_REMOVE_FLAG, false);

	int num_success = 0;
	int num_failed = 0;
	while (entry)
	{
		if (entry_select_aname != "")
		{
			bool is_create = entry->getAttrBool(entry_select_aname, true);
			if (!is_create)
			{
				entry = entries->getNextChild();
				continue;
			}
			if (remove_entryattr) entry->removeAttr(entry_select_aname);
		}

		rdata->setSourceDoc(entry, false);
		if (actions)
		{
			AosSdocAction::runActions(actions, rdata);
		}
		//OmnString docstr = entry->toString();
		AosXmlTagPtr docxml = rdata->getCreatedDoc();
		aos_assert_r(docxml, false);

		OmnString docid = docxml->getAttrStr(AOSTAG_DOCID, "");
		if (docid == "")
		{
			// create doc
			OmnString docstr = docxml->toString();
			AosXmlTagPtr  receiveddoc= rdata->getReceivedDoc();
			AosXmlTagPtr doc = AosDocClientObj::getDocClient()->createDocSafe1(
					rdata, docstr, rdata->getCid(), "", is_public, true, false, 
					false, cid_required, true);
			if (receiveddoc) rdata->setReceivedDoc(receiveddoc, false);
			if (!doc)
			{
				// Failed creating the doc
				failed << "<record status=\"false\" " << to_idname << "=\"" 
					<< entry->getAttrStr(from_idname) << "\" />";
				num_failed++;
			}
			else
			{
				// Successful
				success << "<record status=\"true\" " << to_idname << "=\""
					<< doc->getAttrStr(from_idname) << "\" />";
				num_success++;
			}
		}
		else
		{
			bool mrslt = AosDocClientObj::getDocClient()->modifyObj(rdata, docxml, "", false);
			if (!mrslt)
			{
				// Failed creating the doc
				failed << "<record status=\"false\" " << to_idname << "=\"" 
					<< entry->getAttrStr(from_idname) << "\" />";
				num_failed++;
			}
			else
			{
				// Successful
				success << "<record status=\"true\"" << to_idname << "=\""
					<< docxml->getAttrStr(from_idname) << "\" />";
				num_success++;
			}
		}
		entry = entries->getNextChild(); 
	}
	rdata->setSourceDoc(0, false);

	if (num_success <= 0) success = "";
	if (num_failed <= 0) failed = "";
	rdata->setOk();
	return true;
}


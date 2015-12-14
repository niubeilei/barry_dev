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
// 12/08/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SmartDoc/SdocModifyCompDoc.h"

#include "Actions/ActSeqno.h"
#include "SEInterfaces/DocClientObj.h"
#include "Rundata/Rundata.h"
#include "SmartDoc/SdocCreateDocs.h"
#include "UtilComm/TcpClient.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlInterface/WebRequest.h"

AosSdocModifyCompDoc::AosSdocModifyCompDoc(const bool flag)
:
AosSmartDoc(AOSSDOCTYPE_MODIFY_COMPDOC, AosSdocId::eModifyCompDoc, flag)
{
}


AosSdocModifyCompDoc::~AosSdocModifyCompDoc()
{
}


bool
AosSdocModifyCompDoc::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata) 
{
	// This function assumes:
	// 1. There is a container. All docs in the container are members of the container.
	// 2. The input (received doc) is in the following format:
	// 		<doc ...>
	// 			<records>
	// 				<record opr="add|remove" objid="xxx"/>
	// 				<record opr="add|remove" objid="xxx"/>
	// 				...
	// 			</records>
	// 		</doc>
	// 	   Each <record> either adds or removes a member. 
	// 3. It removes <records> from <doc>, creates a log based on <doc>
	// 4. For each <record>, if it is to add, it checks whether the member is already 
	//    in the container. If yes, it checks whether it is the same. If yes, do nothing.
	//    Otherwise, it modifies the record. If the record is not in the database yet, 
	//    it creates the doc for it.
	//    If it is to remove, it checks whether the member is the container's member.
	//    If not, it is an error. Otherwise, it removes the doc, and then creates a log.
	//
	
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

	AosXmlTagPtr main_doc = objdef->getFirstChild();
	if (!main_doc)
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
	
	bool is_mod_maindoc = sdoc->getAttrBool(AOSTAG_MODIFY_MAIN, true);
	if (is_mod_maindoc) 
	{
		// Modify the main doc
		bool rslt = AosDocClientObj::getDocClient()->modifyObj(main_doc, rdata);
		if (!rslt)
		{
			AosSetError(rdata, AOSLT_FAILED_MODIFY_DOC);
			OmnAlarm << rdata->getErrmsg() << ". " << main_doc->toString() << enderr;
			return false;
		}
	}

	OmnString str = sdoc->getAttrStr("modified_attrs");
	vector<OmnString> attrnames;
	if (str != "")
	{
		bool finished = false;
		// Chen Ding, 2013/12/31
		// AosStrSplit::splitStrByChar(str.data(), ", ", attrnames, eMaxModifiedAttrs, finished);
		AosStrSplit::splitStrBySubstr(str.data(), ", ", attrnames, eMaxModifiedAttrs, finished);
	}

	AosXmlTagPtr old_sourcedoc = rdata->setSourceDoc(entries, false);
	AosXmlTagPtr old_targetdoc = rdata->setTargetDoc(main_doc, false);

	OmnString entry_select_aname = sdoc->getAttrStr(AOSTAG_SELECT_FLAG_NAME);
	bool remove_flag = sdoc->getAttrBool(AOSTAG_REMOVE_FLAG, false);

	OmnString failed = "<";
	failed << AOSTAG_FAILED << ">";
	OmnString success = "<";
	success << AOSTAG_SUCCESS << ">";

	// Create the sub-docs
	OmnString rslts = "<Contents>";
	rslts << "<record status=\"true\" zky_objid=\""
		<< main_doc->getAttrStr(AOSTAG_OBJID) << "\" />";

	if (entries)
	{
		OmnString entries_to_create = "<Contents>";
		bool need_create = false;

		AosXmlTagPtr entry = entries->getFirstChild();
		while (entry)
		{
			if (entry_select_aname != "")
			{
				if (!entry->getAttrBool(entry_select_aname, false))
				{
					// The entry is not selected. Ignore it.
					entry = entries->getNextChild();
					continue;
				}

				if (remove_flag)
				{
					entry->removeAttr(entry_select_aname);
				}
			}

			OmnString objid = entry->getAttrStr(AOSTAG_OBJID);
			bool processed = false;
			if (objid != "")
			{
				AosXmlTagPtr entry_doc = AosDocClientObj::getDocClient()->getDocByObjid(objid, rdata);
				if (entry_doc)
				{
					entry_doc = entry_doc->clone(AosMemoryCheckerArgsBegin);
					if (!entry_doc)
					{
						OmnAlarm << "Entry doc null" << enderr;
						rdata->setSourceDoc(old_sourcedoc, false);
						rdata->setTargetDoc(old_targetdoc, false);
						rdata->setDocByVar(AOSARG_MODIFIED_DOC, 0, false);
						return false;
					}

					processed = true;
					bool modified = false;
					// Need to modify the doc
					for (u32 i=0; i<attrnames.size(); i++)	
					{
						OmnString value1 = entry->getAttrStr(attrnames[i]);
						OmnString value2 = entry_doc->getAttrStr(attrnames[i]);
						if (value1 != value2)
						{
							modified = true;
							if (value1 == "")
							{
								entry_doc->removeAttr(attrnames[i]);
							}
							else
							{
								entry_doc->setAttr(attrnames[i], value1);
							}
						}
					}

					if (modified)
					{
						rdata->resetError();
						bool rslt = AosDocClientObj::getDocClient()->modifyObj(entry_doc, rdata);
						if (!rslt)
						{
							failed << "<record status=\"false\" zky_objid=\""
								<< entry_doc->getAttrStr(AOSTAG_OBJID)
								<< "\"><![CDATA[" << rdata->getErrmsg()
								<< "]]></record>";
						}
						else
						{
							success << "<record status=\"true\" zky_objid=\""
								<< entry_doc->getAttrStr(AOSTAG_OBJID) << "\"/>";
						}
					}
				}

				rdata->setDocByVar(AOSARG_MODIFIED_DOC, entry_doc, false);
				AosXmlTagPtr actions = sdoc->getFirstChild(AOSTAG_MODDOC_ACTIONS);
				if (actions)
				{
					AosSdocAction::runActions(actions, rdata);
				}
			}

			if (!processed)
			{
				// The entry is not in the database. It is to create. 
				entries_to_create << entry->toString();
				need_create = true;
			}

			entry = entries->getNextChild();
		}

		if (need_create)
		{
			// It needs to create the entries.
			entries_to_create <<"</Contents>";
			AosXmlTagPtr entries_xml = AosXmlParser::parse(entries_to_create AosMemoryCheckerArgs);
			if (!entries_xml)
			{
				OmnAlarm << "Failed creating the XML: " << entries_to_create << enderr;
			}
			else
			{
				if (entries_xml->isRootTag())entries_xml = entries_xml->getFirstChild();
				bool rslt = AosSdocCreateDocs::createEntries(
						entries_xml, sdoc, success, failed, rdata);
				if (!rslt)
				{
					OmnAlarm << "Failed creating entries: " << rdata->getErrmsg() << enderr;
				}
			}
		}
	}
	if (success != "")
	{
	    rslts << success << "</" << AOSTAG_SUCCESS << ">";
	}

	if (failed != "")
	{
	    rslts << failed << "</" << AOSTAG_FAILED << ">";
	}
	rslts << "</Contents>";

	// Process the actions on the doc
	AosXmlTagPtr procactions = sdoc->getFirstChild(AOSTAG_PROCDOC_ACTIONS);
	if (procactions)
	{
		AosSdocAction::runActions(procactions, rdata);
	}

	rdata->setResults(rslts);
	rdata->setSourceDoc(old_sourcedoc, false);
	rdata->setTargetDoc(old_targetdoc, false);
	rdata->setDocByVar(AOSARG_MODIFIED_DOC, 0, false);
	return true;
}


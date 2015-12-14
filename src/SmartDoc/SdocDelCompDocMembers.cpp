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
// 12/17/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SmartDoc/SdocDelCompDocMembers.h"

#include "Actions/ActSeqno.h"
#include "SEInterfaces/DocClientObj.h"
#include "Rundata/Rundata.h"
#include "SmartDoc/SdocCreateDocs.h"
#include "UtilComm/TcpClient.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlInterface/WebRequest.h"

AosSdocDelCompDocMembers::AosSdocDelCompDocMembers(const bool flag)
:
AosSmartDoc(AOSSDOCTYPE_DEL_COMPDOC_MEMBERS, AosSdocId::eDeleteCompDocMembers, flag)
{
}


AosSdocDelCompDocMembers::~AosSdocDelCompDocMembers()
{
}


bool
AosSdocDelCompDocMembers::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata) 
{
	// This function deletes the memebrs of a given comp doc based on the input.	
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
	
	AosXmlTagPtr old_targetdoc = rdata->setTargetDoc(main_doc, false);
	AosXmlTagPtr old_sourcedoc = rdata->setSourceDoc(entries, false);

	OmnString entry_select_aname = sdoc->getAttrStr(AOSTAG_SELECT_FLAG_NAME);
	bool remove_flag = sdoc->getAttrBool(AOSTAG_REMOVE_FLAG, false);

	OmnString failed = "<";
	failed << AOSTAG_FAILED << ">";
	OmnString success = "<";
	success << AOSTAG_SUCCESS << ">";

	// Create the sub-docs
	if (!entries)
	{
		OmnString contents = "<Contents></Contents>";
		rdata->setResults(contents);
		rdata->setOk();
		rdata->setTargetDoc(old_targetdoc, false);
		rdata->setSourceDoc(old_sourcedoc, false);
		return true;
	}

	OmnString rslts = "<Contents>";
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
		if (objid != "")
		{
			AosXmlTagPtr entry_doc = AosDocClientObj::getDocClient()->getDocByObjid(objid, rdata);
			if (entry_doc)
			{
				bool rslt = AosDocClientObj::getDocClient()->deleteDoc(entry_doc, rdata);
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
	
		entry = entries->getNextChild();
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
	rdata->setResults(rslts);
	rdata->setTargetDoc(old_targetdoc, false);
	rdata->setSourceDoc(old_sourcedoc, false);
	return true;
}


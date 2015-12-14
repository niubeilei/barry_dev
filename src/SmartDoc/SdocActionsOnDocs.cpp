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
// This smart doc performs actions on docs. The smart doc is:
// 	<sdoc AOSTAG_OBJID_ANAME="xxx"
// 		AOSTAG_BYOBJID="true|false"
// 		AOSTAG_XPATH="xxx">
// 		<AOSTAG_DOCSELECTOR .../>
// 		<actions>
// 			<action .../>
// 			<action .../>
// 			...
// 		</actions>
// 	</sdoc>
//
// It uses <objselector> to select the working object, which is in the 
// form:
// 	<entries>
// 		<entry .../>
// 		<entry .../>
// 		...
// 	</entries>
//
// The attribute AOSTAG_BYOBJID is used to determine whether the objects
// are <entry>s (false) or through objid (true). If not specified, it
// defaults to true. 
//
// If AOSTAG_BYOBJID is true, there shoudl be an attribute in <entry> 
// that is the objid of the object each <entry> maps to. The attribute 
// name is identified by AOSTAG_OBJID_ANAME.
//
//
// Modification History:
// 2011/04/25	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SmartDoc/SdocActionsOnDocs.h"

#include "Actions/ActSeqno.h"
#include "API/AosApi.h"
#include "DocSelector/DocSelector.h"
#include "SEInterfaces/DocClientObj.h"
#include "Rundata/Rundata.h"
#include "UtilComm/TcpClient.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlInterface/WebRequest.h"



AosSdocActionsOnDocs::AosSdocActionsOnDocs(const bool flag)
:
AosSmartDoc(AOSSDOCTYPE_ACTIONSONDOCS, AosSdocId::eActionsOnDocs, flag)
{
}


AosSdocActionsOnDocs::~AosSdocActionsOnDocs()
{
}


bool
AosSdocActionsOnDocs::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata) 
{
	// Refer to Document TSK0058, or the comments at the beginning of
	// this file.
	
	// Retrieve the smart doc
	if (!sdoc)
	{
		AosSetError(rdata, "missing_sdoc");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// Retrieve the working doc
	// AosXmlTagPtr workingDoc = AosDocSelector::selectDoc(sdoc, AOSTAG_DOCSELECTOR, rdata);
	AosXmlTagPtr workingDoc = AosRunDocSelector(rdata, sdoc, AOSTAG_DOCSELECTOR);
	if (!workingDoc)
	{
		AosSetError(rdata, "missing_doc");
		return false;
	}

	// Retrieve the path that identifies <entries>
	OmnString path = sdoc->getAttrStr(AOSTAG_XPATH);
	AosXmlTagPtr entries;
	if (path == "")
	{
		entries = workingDoc;
	}
	else
	{
		entries = workingDoc->xpathGetChild(path);
	}

	if (!entries)
	{
		// There are no docs. 
		AosSetError(rdata, "missing_docs");
		return false;
	}

	// Retrieve the actions
	AosXmlTagPtr actions = sdoc->getFirstChild(AOSTAG_ACTIONS);
	if (!actions)
	{
		AosSetError(rdata, "missing_actions");
		return false;
	}

	return doActions(sdoc, actions, entries, rdata);
}


bool
AosSdocActionsOnDocs::doActions(
		const AosXmlTagPtr &sdoc,
		const AosXmlTagPtr &actions, 
		const AosXmlTagPtr &entries,
		const AosRundataPtr &rdata)
{
	// Ready to run actions on all the docs.
	OmnString failed = "<";
	failed << AOSTAG_FAILED << ">";
	OmnString success = "<";
	success << AOSTAG_SUCCESS << ">";

	bool byobjid = sdoc->getAttrBool(AOSTAG_BYOBJID);
	AosXmlTagPtr entry = entries->getFirstChild();
	OmnString to_idname = sdoc->getAttrStr(AOSTAG_TO_IDNAME, AOSTAG_TO_IDNAME);
	OmnString from_idname = sdoc->getAttrStr(AOSTAG_FROM_IDNAME, AOSTAG_FROM_IDNAME);
	OmnString objid_aname = sdoc->getAttrStr(AOSTAG_OBJID_ANAME);
	u32 siteid = rdata->getSiteid();
	if (siteid == 0) 
	{
		AosSetError(rdata, "missing_siteid");
		return false;
	}

	if (byobjid)
	{
		if (objid_aname == "")
		{
			AosSetError(rdata, "missing_objid_aname");
			return false;
		}
	}

	int num_success = 0;
	int num_failed = 0;
	AosXmlTagPtr doc;
	while (entry)
	{
		doc = 0;
		if (byobjid)
		{
			OmnString objid = entry->getAttrStr(objid_aname);
			if (objid == "")
			{
				// It is by objid but failed retrieving the objid
				// from 'entry'. 
				failed << "<entry " << to_idname 
					<< "=\"" << entry->getAttrStr(from_idname) << "/>";
				num_failed++;
			}
			else
			{
				// Found the objid. Retrieve the doc
				doc = AosDocClientObj::getDocClient()->getDocByObjid(objid, rdata);
				if (!doc)
				{
					failed << "<entry " << to_idname 
						<< "=\"" << entry->getAttrStr(from_idname) << "/>";
					num_failed++;
				}
			}
		}
		else
		{
			// 'entry' is the doc. 
			doc = entry;
		}

		if (doc)
		{
			rdata->setTargetDoc(doc, true);
			if (!AosSdocAction::runActions(actions, rdata))
			{
				// Failed the actions
				failed << "<entry " << to_idname 
					<< "=\"" << entry->getAttrStr(from_idname) << "/>";
				num_failed++;
			}
			else
			{
				success << "<entry " << to_idname 
					<< "=\"" << entry->getAttrStr(from_idname) << "/>";
				num_success++;
			}
		}

		entry = entries->getNextChild();
	}

	OmnString rslts = "<Contents>";
	if (num_success > 0)
	{
		rslts << success << "</" << AOSTAG_SUCCESS << ">";
	}

	if (num_failed > 0)
	{
		rslts << success << "</" << AOSTAG_FAILED << ">";
	}

	rslts << "</Contents>";
	rdata->setResults(rslts);
	return true;
}


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
#include "Actions/ActRemoveDoc.h"

#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "SEInterfaces/DocClientObj.h"
#include "Rundata/Rundata.h"
#include "SmartDoc/SmartDoc.h"
#include "Util/OmnNew.h"
#include "Alarm/Alarm.h"
#include "Util/DynArray.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include <string>
#include <vector>
using namespace std;

AosActRemoveDoc::AosActRemoveDoc(const bool flag)
:
AosSdocAction(AOSACTTYPE_REMOVEDOC, AosActionType::eRemoveDoc, flag)
{
}

/*
AosActRemoveDoc::AosActRemoveDoc(const AosXmlTagPtr &def)
:
AosSdocAction(AosActionType::eRemoveDoc, false)
{
	if (!config(def))
	{
		OmnExcept e(OmnFileLine, OmnErrId::eSyntaxError,
			OmnString("Missing the doc tag"));
		throw e;
	}
}


bool
AosActRemoveDoc::config(const AosXmlTagPtr &def)
{
	// The 'def' should be in the form:
	// 	<action type="addattr">
	// 		<attr namebd="xxx" attrname="xxx"/>
	// 		<attr namebd="xxx" attrname="xxx"/>
	// 		...
	// 	</action>
	if (!def)
	{
		OmnExcept e(OmnFileLine, OmnErrId::eSyntaxError,
			OmnString("Definition is null"));
		throw e;
	}
	return true;
}
*/


AosActRemoveDoc::~AosActRemoveDoc()
{
}


bool	
AosActRemoveDoc::run(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	// It retrieves the value of the bound attribute and
	// set the retrieved value to the target doc.
	// AosSmartDocPtr smartdoc = rdata->getSmartdoc();
	// if (!smartdoc)
	// {
	// 	rdata->setError() << "Missing smartdoc (Internal error): "
	// 		<< rdata->toString(1);
	// 	OmnAlarm << rdata->getErrmsg() << enderr;
	// 	return false;
	// }
	AosXmlDocPtr header;
	AosXmlTagPtr sourceDoc = rdata->getSourceDoc();
	if (!sourceDoc)
	{
		rdata->setErrmsg("Failed To Retrieve SourceDoc");
		return false;
	}
	AosXmlTagPtr targetDoc = rdata->getTargetDoc();
	if (!targetDoc)
	{
		rdata->setErrmsg("Failed To Retrieve TargetDoc");
		return false;
	}

	u64 userid = rdata->getUserid();
	aos_assert_r(userid, false);
	OmnString  docid = targetDoc->getAttrStr(AOSTAG_DOCID);
	if (docid == "")
	{
		rdata->setError() << "can't find docid";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	OmnString objid = targetDoc->getAttrStr(AOSTAG_OBJID);
	if (objid == "")
	{
		rdata->setError() << "Can't Retrieve Objid!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	//--Ketty
	/*bool rslt = AosDocServer::getSelf()->deleteObj(
											rdata->getSiteid(), 
											AOSTAG_APPNAME, 
											userid, 
											docid, 
											objid, 
											rdata->getErrcode(), 
											rdata->getErrmsg());*/
	bool rslt = AosDocClientObj::getDocClient()->deleteObj(rdata, docid, objid, "", true);
	if (!rslt)
	{
		rdata->setError() << "Delete Doc Failed!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	OmnScreen << "final targetDoc will be(modifyOnly or not):" << targetDoc->toString() <<endl;
	return true;
}


AosActionObjPtr
AosActRemoveDoc::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActRemoveDoc(false);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}



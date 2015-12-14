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
#include "Actions/ActDeleteDoc.h"

#include "API/AosApi.h"
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
#include "DocSelector/DocSelector.h"
#include <string>
#include <vector>
using namespace std;

AosActDeleteDoc::AosActDeleteDoc(const bool flag)
:
AosSdocAction(AOSACTTYPE_DELETEDOC, AosActionType::eDeleteDoc, flag)
{
}

AosActDeleteDoc::~AosActDeleteDoc()
{
}


bool	
AosActDeleteDoc::run(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{

	AosXmlTagPtr target_doc= AosRunDocSelector(rdata, def, AOSTAG_DOCSELECTOR);
	if (!target_doc)
	{
		AosSetError(rdata, "eMissingDoc");
		OmnAlarm << rdata->getErrmsg() << ". Sdoc: " << def->toString() << enderr;
		return false;
	}
	OmnString objid = target_doc->getAttrStr(AOSTAG_OBJID, "");
	if(objid=="")
	{
		OmnAlarm << "missing objid!"<< enderr;
		return false;
	}
	OmnString docid = target_doc->getAttrStr(AOSTAG_DOCID, "");
	if(docid=="")
	{
		OmnAlarm << "missing docid!"<< enderr;
		return false;
	}

	bool rslt = AosDocClientObj::getDocClient()->deleteObj(rdata, docid, objid, "", false);
	if (!rslt)
	{
		rdata->setError() << "Delete Doc Failed!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
//	OmnScreen << "the obj deleted is :" << target_doc->toString() <<endl;
	return true;
}


AosActionObjPtr
AosActDeleteDoc::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActDeleteDoc(false);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}



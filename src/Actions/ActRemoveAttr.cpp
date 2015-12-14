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
#include "Actions/ActRemoveAttr.h"

#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "Util/OmnNew.h"
#include "Alarm/Alarm.h"
#include "SEUtilServer/CloudidSvr.h"
#include "SmartDoc/SmartDoc.h"
#include "Util/DynArray.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include <string>
#include <vector>
using namespace std;

AosActRemoveAttr::AosActRemoveAttr(const bool flag)
:
AosSdocAction(AOSACTTYPE_REMOVEATTR, AosActionType::eRemoveAttr, flag)
{
}


/*
AosActRemoveAttr::AosActRemoveAttr(const AosXmlTagPtr &def)
:
AosSdocAction(AosActionType::eRemoveAttr, false)
{
	if (!config(def))
	{
		OmnExcept e(OmnFileLine, OmnErrId::eSyntaxError,
			OmnString("Missing the doc tag"));
		throw e;
	}
}


bool
AosActRemoveAttr::config(const AosXmlTagPtr &def)
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

	AosXmlTagPtr attr = def->getFirstChild();
	if (!attr)
	{
		OmnExcept e(OmnFileLine, OmnErrId::eSyntaxError,
			OmnString("Missing attr tag"));
		throw e;
	}

	while (attr)
	{
		OmnString name = attr->getAttrStr(AOSTAG_NAME_BIND);	// join .. namebd

		if (name == "")
		{
			OmnAlarm << "Attribute name binding is empty!" << enderr;
		}
		else
		{
			mNames.push_back(name);
			OmnString attrname = attr->getAttrStr(AOSTAG_ATTRNAME);
			if (attrname == "") 
			{
				mAttrNames.push_back(name);
			}
			else
			{
				mAttrNames.push_back(attrname);
			}
		}

		attr = def->getNextChild();
	}
	return true;
}
*/


AosActRemoveAttr::~AosActRemoveAttr()
{
}


bool	
AosActRemoveAttr::run(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
	/*
	aos_assert_rr(def, rdata, false);
	rdata->setError();
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
	if (def->getAttrBool(AOSTAG_SDOCACT_MODONLY))
	{
		for (int i=0; i<(int )mNames.size(); i++)
		{
			OmnScreen << "mNames.size():" << (int)mNames.size() << endl;
			OmnString value = sourceDoc->getAttrStr(mNames[i]);
			targetDoc->setAttr(mAttrNames[i], value);
		}
		OmnScreen << "modifyDocOnly, the targetDoc will be++++: " << targetDoc->toString() << endl;
		return true;
	}

	int ss = mNames.size();
	for (int i=0; i<ss; i++)
	{
		OmnScreen << "mNames.size():" << (int)mNames.size() << endl;
		OmnString vv = targetDoc->getAttrStr(mNames[i]);
		if (vv != "")
		{
			targetDoc->removeAttr(mNames[i], false, true);
			//int ttl = 0;
			//u64 userid = rdata->getUserid();
			OmnString siteid = rdata->getSiteid();
			if (siteid == "")
			{
				rdata->setError() << "Missing siteid!" ;
				OmnAlarm << rdata->getErrmsg() << enderr;
			}
			AosXmlDocPtr header;
			//--Ketty
			//AosDocServerSelf->modifyObj(
			//				siteid, AOSTAG_APPNAME, userid, targetDoc, targetDoc, 0, false,
			//				rdata->getErrcode(), rdata->getErrmsg(), ttl);
			AosDocClientObj::getDocClient()->modifyObj(rdata, targetDoc, 0, false);
		}
	}
	OmnScreen << "final targetDoc will be(modifyOnly or not):" << targetDoc->toString() <<endl;
	*/
	return true;
}


AosActionObjPtr
AosActRemoveAttr::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActRemoveAttr(false);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}



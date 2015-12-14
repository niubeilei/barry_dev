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
// This action sets an attribute:
// 	<action zky_type="AOSACTION_SETATTR" xpath="xxx">
// 		<doc .../>
// 		<value .../>
// 	</action>
// 	where <doc> is a Doc Selection tag. <value> is a Value Tag. 
// 	This is the value to be set.
//
// Modification History:
// 09/15/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Actions/ActTrashDoc.h"

#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "SEInterfaces/DocClientObj.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DocSelObj.h"
#include "SmartDoc/SmartDoc.h"
#include "Util/OmnNew.h"
#include "Util/DynArray.h"
#include "ValueSel/ValueSel.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include <string>
#include <vector>
using namespace std;
#if 0

AosActTrashDoc::AosActTrashDoc(const bool flag)
:
AosSdocAction(AOSACTTYPE_TRASHDOC, AosActionType::eTrashDoc, flag)
{
}


/*
AosActTrashDoc::AosActTrashDoc(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
:
AosSdocAction(AosActionType::eTrashDoc, false)
{
	if (!config(def, rdata))
	{
		OmnExcept e(OmnFileLine, OmnErrId::eSyntaxError, rdata->getErrmsg());
		throw e;
	}
}


bool
AosActTrashDoc::config(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	// The 'def' should be in the form:
	// 	<action type="addattr">
	// 		<attr namebd="xxx" attrname="xxx"/>
	// 		<attr namebd="xxx" attrname="xxx"/>
	// 		...
	// 	</action>
	if (!def)
	{
		rdata->setError() << "Missing the smart doc";
		return false;
	}

	AosXmlTagPtr attr = def->getFirstChild();
	if (!attr)
	{
		rdata->setError() << "Missing attr tag";
		return false;
	}

	while (attr)
	{
		OmnString name = attr->getAttrStr(AOSTAG_NAME_BIND);

		if (name == "")
		{
			rdata->setError() << "Attribute name is empty";
			OmnAlarm << rdata->getErrmsg() << enderr;
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


AosActTrashDoc::~AosActTrashDoc()
{
}


bool	
AosActTrashDoc::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	// This action trash a doc. 
	//
	//<action type="trashdoc">	
	//		<zky_docselector zky_docselector_type="targetdoc"/>
	//		<zky_valuedef zky_value_type="attr" zky_xpath = "containerid" >	
	//				<zky_docselector zky_docselector_type="sourcedoc" />
	//		</zky_valuedef>
	//</action>
	//trash the target doc to the container. 
	if (!sdoc)
	{
		rdata->setError() << "Missing smartdoc (Internal error): "
			<< rdata->toString(1);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// Retrieve the source doc
	AosXmlTagPtr selector = sdoc->xpathGetChild(AOSTAG_DOCSELECTOR);
	if (!selector)
	{
		rdata->setError() << "Missing doc selector";
		return false;
	}

	AosXmlTagPtr targetDoc = AosRunDocSelector(rdata, selector);
	if (!targetDoc)
	{
		rdata->setError() << "Missing source doc!";
		return false;
	}
	OmnString objid = targetDoc->getAttrStr(AOSTAG_OBJID);

	// Retrieve the value
	AosXmlTagPtr value_def = sdoc->xpathGetChild(AOSTAG_VALUEDEF);
	if (!value_def)
	{
		rdata->setError() << "Missing value definition";
		return false;
	}

	AosValueRslt value;
	if (!AosValueSel::getValueStatic(value, value_def, rdata))
	{
		return false;
	}

	// The data type cannot be XML DOC
	if (!value.isNull())
	{
		rdata->setError() << "Value is invalid";
		return false;
	}

	if (value.isXmlDoc())
	{
		rdata->setError() << "Value is an XML doc";
		return false;
	}

	// Ready to trash the doc.
	OmnString vv = value.getStr();
	if (!AosDocClientObj::getDocClient()->deleteObj(rdata, "", objid, vv, false));
	{
		return false;
	}

	rdata->setOk();
	return true;
}


AosActionObjPtr
AosActTrashDoc::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActTrashDoc(false);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}

#endif

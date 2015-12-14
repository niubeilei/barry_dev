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
#include "Actions/ActModifyNode.h"

#include "ValueSel/ValueSel.h"
#include "Actions/Ptrs.h"
#include "Actions/ActUtil.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"

#include "Rundata/Rundata.h"
#include "SEUtil/DocTags.h"
#include "Security/SessionMgr.h"
#include "SmartDoc/Ptrs.h"
#include "SmartDoc/SmartDoc.h"
#include "Util/OmnNew.h"
#include "Util/DynArray.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlDoc.h"

#if 0

AosActModifyNode::AosActModifyNode(const bool flag)
:
AosSdocAction(AOSACTTYPE_MODIFYNODE, AosActionType::eModifyNode, flag)
{
}


/*
AosActModifyNode::AosActModifyNode(const AosXmlTagPtr &def)
:
AosSdocAction(AosActionType::eModifyNode, false)
{
	if(!config(def))
	{
		OmnExcept e(OmnFileLine, OmnErrId::eSyntaxError,
				OmnString("Missing the doc tag"));
		throw e;
	}
}
*/


AosActModifyNode::~AosActModifyNode()
{
}


bool	
AosActModifyNode::config(const AosXmlTagPtr &config)
{
	return true;
}


/*
AosActionObjPtr
AosActModifyNode::clone(
			const AosXmlTagPtr &def,
			const AosRundataPtr &rdata)
{
	try
	{
		AosSdocActionPtr sdoc = OmnNew AosActModifyNode(def);
		return sdoc;
	}
	catch (const OmnExcept &e)
	{
		rdata->setError() << "Failed to create action: " << e.getErrmsg();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
*/


bool
AosActModifyNode::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	// this actions modify an node to a doc
	// <action type = "modifynode" zky_delpath="" zky_addpath="" >
	//		<sourcedoc zky_value_type="const" zky_xpath="" zky_value_type="XmlDoc" zky_copy="true">
	//			<zky_docselector zky_docselector_type="receiveddoc"></zky_docselector>
	//		</sourcedoc>
	//		<targetdoc zky_value_type="const" zky_value_type="XmlDoc">
	//			<zky_docselector zky_docselector_type="receiveddoc"></zky_docselector>
	//		</targetdoc>
	// </action>
	
	if (!sdoc)
	{
		rdata->setError() << "Missing smartdoc (Internal error): "
			<< rdata->toString(1);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// Retrieve the Soure Doc 
	AosXmlTagPtr value_def = sdoc->xpathGetChild("sourcedoc");
	if (!value_def)
	{
		rdata->setError() << "Missing value definition";
		return false;
	}

	AosValueRslt valueRslt;
	if (!AosValueSel::getValueStatic(valueRslt, value_def, rdata))
	{
		return false;
	}

	// The data type cannot be XML DOC
	if (!valueRslt.isValid())
	{
		rdata->setError() << "Value is invalid";
		return false;
	}

	if (!valueRslt.isXmlDoc())
	{
		rdata->setError() << "Value is an XML doc";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosXmlTagPtr souredoc = valueRslt.getXmlValue(rdata.getPtr());
	if (!souredoc)
	{
		rdata->setError() << "Missing Source Doc";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// Retrieve the Targetdoc Doc
	
	AosXmlTagPtr value_target = sdoc->xpathGetChild("targetdoc");
	if (!value_target)
	{
		rdata->setError() << "Missing value definition";
		return false;
	}

	AosValueRslt valueRsltTar;
	if (!AosValueSel::getValueStatic(valueRsltTar, value_target, rdata))
	{
		return false;
	}

	// The data type cannot be XML DOC
	if (!valueRsltTar.isValid())
	{
		rdata->setError() << "Value is invalid";
		return false;
	}

	if (!valueRsltTar.isXmlDoc())
	{
		rdata->setError() << "Value is an XML doc";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosXmlTagPtr targetdoc = valueRsltTar.getXmlValue(rdata.getPtr());
	if (!targetdoc)
	{
		rdata->setError() << "Missing target Doc";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	OmnString delpath = sdoc->getAttrStr("zky_delpath");
	if (delpath != "")
	{
		if (!souredoc->xpathRemoveNode(delpath))
		{
		   rdata->setError() << "Failed to remove entries";
		   return false;
		}
	}

	OmnString addpath = sdoc->getAttrStr("zky_addpath");
	if (addpath != "")
	{
		if (!souredoc->xpathAddNode(addpath, targetdoc))
		{
		   rdata->setError() << "Failed to remove entries";
		   OmnAlarm << rdata->getErrmsg() << enderr;
		   return false;
		}
	}

	OmnString docstr = souredoc->toString();
	rdata->setResults(docstr);
	rdata->setOk();
	return true;
}


AosActionObjPtr
AosActModifyNode::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActModifyNode(false);
	}

	catch (const OmnExcept &e)
	{
		rdata->setError() << "Failed to create action: " << e.getErrmsg();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


#endif

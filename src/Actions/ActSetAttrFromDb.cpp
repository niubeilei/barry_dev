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
// 	<action zky_type="AOSACTION_" xpath="xxx">
// 		<doc .../>
// 		<value .../>
// 	</action>
// 	where <doc> is a Doc Selection tag. <value> is a Value Tag. 
// 	This is the value to be set.
//
// Modification History:
// 06/08/2011	Created by Tracy Huang
////////////////////////////////////////////////////////////////////////////
#include "Actions/ActSetAttrFromDb.h"

#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "DocSelector/DocSelector.h"
#include "Rundata/Rundata.h"
#include "SmartDoc/SmartDoc.h"
#include "Util/OmnNew.h"
#include "Util/DynArray.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include <string>
#include <vector>

// 06/08/2011, by Tracy
#include "Database/Ptrs.h" 
#include "Database/DbRecord.h"
#include "XmlParser/XmlParser.h"
#include "XmlUtil/SeXmlParser.h"
#include "Database/DbTable.h"
#include "Database/DbRecord.h"

using namespace std;

AosActSetAttrFromDb::AosActSetAttrFromDb(const bool flag)
:
AosSdocAction(AOSACTTYPE_SETATTRFROMDB, AosActionType::eSetAttrFromDb, flag)
{
}


/*
AosActSetAttrFromDb::AosActSetAttrFromDb(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
:
AosSdocAction(AosActionType::eSetAttrFromDb, false)
{
	if (!config(def, rdata))
	{
		OmnExcept e(OmnFileLine, OmnErrId::eSyntaxError, rdata->getErrmsg());
		throw e;
	}
}


bool
AosActSetAttrFromDb::config(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
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


AosActSetAttrFromDb::~AosActSetAttrFromDb()
{
}


bool	
AosActSetAttrFromDb::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	// Chen Ding, 01/01/2012
	// Need to move the database accessing from rdata to database module.
	/*
	// This function sets an attribute based on a db record. The
	// smart doc format should be:
	// 	<sdoc type="setattrfromdb" db_fidx="xxx" aname="xxx" datatype="xxx">
	// 		<docselector .../>
	// 	</sodc>
	if (!sdoc)
	{
		rdata->setError() << "Missing smartdoc (Internal error): "
			<< rdata->toString(1);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	OmnDbRecordPtr rcd = rdata->getDbRecord();
	if(!rcd)
	{
		rdata->setError() << "Missing Record: " 
		<< rdata->toString(1);
		return false;
	}
	
//	AosXmlTagPtr doc = AosDocSelector::selectDoc(sdoc, "docselector", rdata);
	AosXmlTagPtr doc = rdata->getCreatedDoc(); //tracy
	if (!doc)
	{
		rdata->setError() << "Failed retrieving the doc!";
		return false;
	}
	
	int idx = sdoc->getAttrInt(AOSTAG_FIELDIDX, -1);
	if (idx < 0)
	{
		rdata->setError() ;
	}

	OmnString xpath = sdoc->getAttrStr(AOSTAG_XPATH);
	if (xpath == "")
	{
		rdata->setError() << "Missing xpath";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	OmnString datatype = sdoc->getAttrStr(AOSTAG_DATATYPE);
	OmnString value;
	bool rslt = rcd->getValue(idx, datatype, value);
	if (!rslt)
	{
		rdata->setError() << "Failed retrieving the value: " << idx << ":" << datatype;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	bool rslt2 = doc->xpathSetAttr(xpath, value);
	if (!rslt2)
	{
		rdata->setError() << "Failed setting the value: " << xpath << ":" << value;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	rdata->setCreatedDoc(doc); //Tracy

	rdata->setOk();
	return true;
	*/
	OmnNotImplementedYet;
	return false;
}


AosActionObjPtr
AosActSetAttrFromDb::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActSetAttrFromDb(false);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}



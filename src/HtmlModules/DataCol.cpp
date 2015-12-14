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
//   
//
// Modification History:
// 07/16/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "HtmlModules/DataCol.h"

#include "HtmlUtil/HtmlRc.h"
#include "HtmlUtil/HtmlUtil.h"
#include "SearchEngineAdmin/SengAdmin.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "Util/String.h"
#include "HtmlModules/DclStr.h"
#include "HtmlModules/DclDb.h"


AosDataColPtr
AosDataCol::getInstance(const AosXmlTagPtr &vpd)
{
	AosXmlTagPtr dclvpd = vpd->getFirstChild("datacol");
 	if (!dclvpd) return 0;

	OmnString dclType = dclvpd->getAttrStr("gic_type","");
	DataColType dtype = getDataColType(dclType);
	switch(dtype)
	{
	case eDclType_Str:
		 return OmnNew AosDclStr();

	case eDclType_Db:
		 return OmnNew AosDclDb();

	default:
		 return 0;
	}
}


AosXmlTagPtr 
AosDataCol::retrieveDataStatic(
		const AosHtmlReqProcPtr &htmlPtr,
		const AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		const OmnString &tagname)
{
	aos_assert_r(vpd, 0);
	OmnString tname = tagname;
	if (tname == "") tname = "datacol";
	AosXmlTagPtr datacol = vpd->getFirstChild(tname);
	if (!datacol) return 0;

    OmnString gic_type = datacol->getAttrStr("gic_type");
	aos_assert_r(gic_type != "", 0);

    if (gic_type == "db_datacol")
	{
		// data should be this form
		// <Contents>
		//    <record .../> 
		//    <record .../> 
		//    ......
		//    <record .../> 
		// </Contents>
	    AosDclDb dcl;
		AosXmlTagPtr doc = dcl.retrieveData(htmlPtr, vpd, obj, "");
		if(!doc)
		{
		    OmnAlarm << "Failed to retrieve the doc definition: " 
			      << vpd->toString() << enderr;
			return 0;
		}
		return doc;
	}

	if (gic_type == "str_datacol")
	{
		AosDclStr dcl;
		AosXmlTagPtr doc = dcl.retrieveData(htmlPtr->getSiteid(), vpd, obj, 
				htmlPtr->getSsid(), tname);
		if(!doc)
		{
		    OmnAlarm << "Failed to retrieve the doc definition: " 
			      << vpd->toString() << enderr;
			return 0;
		}
		return doc;
	}

	OmnAlarm << "Unrecognized datacol type: " << gic_type << enderr;
	return 0;
}


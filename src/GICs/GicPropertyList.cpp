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
// 07/08/2010: Created by Wynn
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicPropertyList.h"
#include "HtmlModules/DclDb.h"
#include "HtmlUtil/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

// static AosGicPtr sgGic = new AosGicPropertyList();

AosGicPropertyList::AosGicPropertyList(const bool flag)
:
AosGic(AOSGIC_PROPERTYLIST, AosGicType::ePropertyList, flag)
{
}


AosGicPropertyList::~AosGicPropertyList()
{
}


bool	
AosGicPropertyList::generateCode(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		const OmnString &parentid,
		AosHtmlCode &code)
{
	aos_assert_r(convertToJson(vpd,obj,code), false);
	return true;
}

bool	
AosGicPropertyList::convertToJson(
		const AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		AosHtmlCode &code)
{
	OmnString gic_frame = vpd->getAttrStr("gic_frame", "false");
	OmnString gic_hideHeaders = vpd->getAttrStr("gic_hideHeaders", "true");
	OmnString gic_rowNumberer = vpd->getAttrStr("gic_rowNumberer", "false");
	OmnString gic_ddSource = vpd->getAttrStr("gic_ddSource");
	OmnString gic_ddTarget = vpd->getAttrStr("gic_ddTarget");
	OmnString gic_sortable = vpd->getAttrStr("gic_sortable", "false");
	OmnString gic_columnlines = vpd->getAttrStr("gic_columnlines", "false");
	OmnString gic_buildTbar = vpd->getAttrStr("gic_buildTbar", "true");
	OmnString gic_enableHdMenu = vpd->getAttrStr("gic_enableHdMenu", "true");
	OmnString gic_enableColumnResize = vpd->getAttrStr("gic_enableColumnResize", "false");
	OmnString gic_list_bd = vpd->getAttrStr("gic_list_bd","");
	OmnString gic_groupname = vpd->getAttrStr("gic_groupname","");
	OmnString gic_pb_def = vpd->getAttrStr("gic_pb_def","false");
	OmnString str;
	str << ",";
	str	<< "frame:" << gic_frame << "," 
		<< "sortable:" << gic_sortable << ","
		<< "enableColumnResize:" << gic_enableColumnResize << ","
		<< "columnLines:" << gic_columnlines << ","
		<< "buildTbar:" << gic_buildTbar << ","
		<< "hideHeaders:" << gic_hideHeaders << ","
		<< "rowNumberer:" << gic_rowNumberer << ","
		<< "pagingToolBar:" << gic_pb_def << ","
		<< "ddTarget:\"" << gic_ddTarget << "\","		 
		<< "ddGroup:\"" << gic_ddSource << "\","		 
		<< "groupname:\"" << gic_groupname << "\","		 
		<< "gic_list_bd:\"" << gic_list_bd << "\"";		 

	code.mJson << str;
	return true;
}


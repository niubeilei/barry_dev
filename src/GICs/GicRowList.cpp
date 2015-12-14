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
// 08/07/2010: Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicRowList.h"

#include "SearchEngineAdmin/SengAdmin.h"
#include "HtmlModules/Ptrs.h"
#include "HtmlModules/DclDb.h"
#include "HtmlUtil/HtmlUtil.h"
#include "HtmlServer/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

// static AosGicPtr sgGic = new AosGicRowList();

AosGicRowList::AosGicRowList(const bool flag)
:
AosGic(AOSGIC_ROWLIST, AosGicType::eRowList, flag)
{
}

AosGicRowList::~AosGicRowList()
{
}

bool	
AosGicRowList::generateCode(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		const OmnString &parentid,
		AosHtmlCode &code)
{
	// This function will generate:
	// 	1. HTML code
	// 	2. CSS code
	// 	3. JavaScript Code
	// 	4. Flash code
	aos_assert_r(convertToJson(vpd, obj, code, htmlPtr), false);
	return true;
}

bool
AosGicRowList::convertToJson(
	AosXmlTagPtr &vpd,
	const AosXmlTagPtr &obj,
	AosHtmlCode &code,
	const AosHtmlReqProcPtr &htmlPtr)
{
	// This function converts the VPD to Json form.
	OmnString gic_title = vpd->getAttrStr("gic_title");
	OmnString gic_isshopcard = vpd->getAttrStr("gic_isshopcard","false");
	OmnString gic_isshoplist = vpd->getAttrStr("gic_isshoplist","false");
	OmnString gic_frame = vpd->getAttrStr("gic_frame", "false");
	OmnString gic_hideHeaders = vpd->getAttrStr("gic_hideHeaders", "true");
	OmnString gic_rowNumberer = vpd->getAttrStr("gic_rowNumberer", "false");
	OmnString gic_checkColumn = vpd->getAttrStr("gic_checkColumn", "false");
	OmnString gic_pb_def = vpd->getAttrStr("gic_pb_def","true");
	OmnString gic_lstn = vpd->getAttrStr("gic_lstn");

	OmnString gic_ddSource = vpd->getAttrStr("gic_ddSource");
	OmnString gic_ddTarget = vpd->getAttrStr("gic_ddTarget");
	OmnString gic_scrollBar_v = vpd->getAttrStr("gic_scrollBar_v", "false");
	OmnString gic_scrollBar_h = vpd->getAttrStr("gic_scrollBar_h", "false");
	OmnString gic_forceFit = vpd->getAttrStr("gic_forceFit", "false");
	OmnString gic_eidtorable = vpd->getAttrStr("gic_editorable", "false");
	OmnString gic_sortable = vpd->getAttrStr("gic_sortable", "false");
	OmnString gic_columnlines = vpd->getAttrStr("gic_columnlines", "false");
	OmnString gic_columnMove = vpd->getAttrStr("gic_columnMove", "false");
	OmnString gic_enableHdMenu = vpd->getAttrStr("gic_enableHdMenu", "true");
	OmnString gic_enableColumnResize = vpd->getAttrStr("gic_enableColumnResize", "false");
	OmnString gic_selectWay = vpd->getAttrStr("gic_selectWay","singleRow");
	OmnString gic_enableRowHead = vpd->getAttrStr("gic_enableRowHead","false");
	OmnString gic_enableRowBody = vpd->getAttrStr("gic_enableRowBody","false");
	OmnString gic_rowhead = vpd->getAttrStr("gic_rowhead");
	OmnString gic_rowbody = vpd->getAttrStr("gic_rowbody");
	OmnString gic_grouping = vpd->getAttrStr("gic_grouping");
	OmnString gic_groupname = vpd->getAttrStr("gic_groupname");
	OmnString gic_expander = vpd->getAttrStr("gic_expander");
	//add by cody
	AosXmlTagPtr contents = vpd->getFirstChild("creators");
	OmnString content = "";
	if(contents)
	{
		contents = contents->getFirstChild();
		contents = contents->getFirstChild("contents");
	}
	if(contents)
	{
		content = contents->getNodeText();
	}
	AosXmlTagPtr datacol = vpd->getFirstChild("datacol");	
	if(!datacol) return false;
	OmnString gic_dcl_type = datacol->getAttrStr("gic_type");
	//db_datacol
	OmnString cmp_tname = datacol->getAttrStr("cmp_tname");
	OmnString cmp_query = datacol->getAttrStr("cmp_query");
	OmnString cmp_order = datacol->getAttrStr("cmp_order");
	OmnString cmp_fnames = datacol->getAttrStr("cmp_fnames");
	OmnString cmp_tnamebd = datacol->getAttrStr("cmp_tnamebd");
	OmnString cmp_psize = datacol->getAttrStr("cmp_psize","20");
	OmnString cmp_qrm = datacol->getAttrStr("cmp_qrm");
	OmnString cmp_dftqry = datacol->getAttrStr("cmp_dftqry");
	OmnString cmp_reverse = datacol->getAttrStr("cmp_reverse","false");
	//str_datacol
	OmnString cmp_anames = datacol->getAttrStr("cmp_anames");
	OmnString cmp_entry_sep = datacol->getAttrStr("cmp_entry_sep","|$$|");
	OmnString cmp_field_sep = datacol->getAttrStr("cmp_field_sep","|$|");
	OmnString cmp_attr_sep = datacol->getAttrStr("cmp_attr_sep","|^|");
	OmnString cmp_contents = datacol->getAttrStr("cmp_contents","");
	//embobj_datacol
	OmnString cmp_autoid = datacol->getAttrStr("cmp_autoid","false");
	OmnString cmp_path = datacol->getAttrStr("cmp_path");
OmnString str=",";
	str	<< "frame:" << gic_frame << "," 
		<< "editorable:" << gic_eidtorable << ","
		<< "isshopcard:" << gic_isshopcard << ","
		<< "isshoplist:" << gic_isshoplist << ","
		<< "sortable:" << gic_sortable << ","
		<< "enableHdMenu:" << gic_enableHdMenu << ","
		<< "enableColumnResize:" << gic_enableColumnResize << ","
		<< "columnLines:" << gic_columnlines << ","
		<< "enableColumnMove:" << gic_columnMove << ","
		<< "hideHeaders:" << gic_hideHeaders << ","
		<< "rowNumberer:" << gic_rowNumberer << ","
		<< "checkColumn:" << gic_checkColumn << ","
		<< "pagingToolBar:" << gic_pb_def << ","
		<< "gic_lstn:\"" << gic_lstn << "\","
		<< "selectWay:\"" << gic_selectWay << "\","
		<< "ddTarget:\"" << gic_ddTarget << "\","		 
		<< "ddGroup:\"" << gic_ddSource << "\","		 
		<< "grouping:\"" << gic_grouping << "\","		 
		<< "groupname:\"" << gic_groupname << "\","		 
		<< "content:\"" << content << "\","		 
		<< "enableexpander:\"" << gic_expander << "\"";		 
/*		<< "dcl_type:\"" << gic_dcl_type << "\","
		<< "cmp:{";
		
		if(gic_dcl_type == "db_datacol")
		{
			str << "cmp_tname:\"" << cmp_tname << "\","
				<< "cmp_query:\"" << cmp_query << "\","
				<< "cmp_order:\"" << cmp_order << "\","
				<< "cmp_fnames:\"" << cmp_fnames << "\","
				<< "cmp_tnamebd:\"" << cmp_tnamebd << "\","
				<< "cmp_psize:\"" << cmp_psize << "\","
				<< "cmp_qrm:\"" << cmp_qrm << "\","
				<< "cmp_dftqry:\"" << cmp_dftqry << "\","
				<< "cmp_reverse:" << cmp_reverse;				
	 	}
		else if(gic_dcl_type == "str_datacol")
		{
			str << "anames:\"" << cmp_anames << "\","
				<< "rsep:\"" << cmp_entry_sep << "\","
				<< "fsep:\"" << cmp_field_sep << "\","
				<< "asep:\"" << cmp_attr_sep << "\","
				<< "mcontent : \"" << cmp_contents << "\"";
		}
		else if(gic_dcl_type == "embobj_datacol")
		{
			str << "cmp_path:\"" << cmp_path << "\","
				<< "cmp_autoid:\"" << cmp_autoid << "\"";
		}
		 
		str << "}";
*/				
	//OmnScreen << "Generated code: " << str << endl;
	code.mJson << str;
	return true;
}


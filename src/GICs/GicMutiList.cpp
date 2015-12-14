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
// 08/07/2010: Created by	Wynn 
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicMutiList.h"

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

//static AosGicPtr sgGic = new AosGicMutiList();

AosGicMutiList::AosGicMutiList(const bool flag)
:
AosGic(AOSGIC_MUTILIST, AosGicType::eMutiList, flag)
{
}

AosGicMutiList::~AosGicMutiList()
{
}

bool	
AosGicMutiList::generateCode(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		const OmnString &parentid,
		AosHtmlCode &code)
{
	return convertToJson(vpd, obj, code, htmlPtr);
}

bool
AosGicMutiList::convertToJson(
	AosXmlTagPtr &vpd,
	const AosXmlTagPtr &obj,
	AosHtmlCode &code,
	const AosHtmlReqProcPtr &htmlPtr)
{
	OmnString gic_title = vpd->getAttrStr("gic_title");
	OmnString gic_mess = vpd->getAttrStr("gic_mess");
	OmnString gic_check_bd = vpd->getAttrStr("gic_check_bd","");
	OmnString gic_delcolumn = vpd->getAttrStr("gic_delcolumn");
	OmnString gic_isshopcard = vpd->getAttrStr("gic_isshopcard","false");
	OmnString gic_frame = vpd->getAttrStr("gic_frame", "false");
	OmnString withoutbgc = vpd->getAttrStr("withoutbgc", "false");
	OmnString gic_hideHeaders = vpd->getAttrStr("gic_hideHeaders", "true");
	OmnString gic_rowNumberer = vpd->getAttrStr("gic_rowNumberer", "false");
	OmnString gic_checkColumn = vpd->getAttrStr("gic_checkColumn", "false");
	OmnString gic_pb_def = vpd->getAttrStr("gic_pb_def","true");
	OmnString gic_lstn = vpd->getAttrStr("gic_lstn");

	OmnString gic_ddSource = vpd->getAttrStr("gic_ddSource");
	OmnString gic_ddTarget = vpd->getAttrStr("gic_ddTarget");
	OmnString gic_scrollBar_v = vpd->getAttrStr("gic_scrollBar_v", "false");
	OmnString gic_export = vpd->getAttrStr("gic_export", "false");
	OmnString gic_scrollBar_h = vpd->getAttrStr("gic_scrollBar_h", "false");
	OmnString gic_forceFit = vpd->getAttrStr("gic_forceFit", "true");
	OmnString gic_eidtorable = vpd->getAttrStr("gic_editorable", "false");
	OmnString gic_sortable = vpd->getAttrStr("gic_sortable", "false");
	OmnString gic_columnlines = vpd->getAttrStr("gic_columnlines", "false");
	OmnString gic_columnMove = vpd->getAttrStr("gic_columnMove", "false");
	OmnString gic_buildTbar = vpd->getAttrStr("gic_buildTbar", "true");
	OmnString gic_enableHdMenu = vpd->getAttrStr("gic_enableHdMenu", "true");
	OmnString gic_enableColumnResize = vpd->getAttrStr("gic_enableColumnResize", "false");
	OmnString gic_selectWay = vpd->getAttrStr("gic_selectWay","singleRow");
	OmnString gic_selectMod = vpd->getAttrStr("gic_selectMod","rowSelect");
	OmnString gic_enableRowHead = vpd->getAttrStr("gic_enableRowHead","false");
	OmnString gic_enableRowBody = vpd->getAttrStr("gic_enableRowBody","false");
	OmnString gic_rowhead = vpd->getAttrStr("gic_rowhead");
	OmnString gic_rowbody = vpd->getAttrStr("gic_rowbody");
	OmnString gic_grouping = vpd->getAttrStr("gic_grouping");
	OmnString gic_groupname = vpd->getAttrStr("gic_groupname");
	OmnString autosize = vpd->getAttrStr("autosize","false");
	OmnString gic_expander = vpd->getAttrStr("gic_expander");
	OmnString gic_expanding = vpd->getAttrStr("gic_expanding", "false");
	OmnString gic_autoexpandercolumn = vpd->getAttrStr("gic_autoexpandercolumn");
	OmnString gic_list_bd = vpd->getAttrStr("gic_list_bd","");
	OmnString gic_toolbar = vpd->getAttrStr("gic_toolbar","");
	OmnString gic_property = vpd->getAttrStr("gic_property","false");
	AosXmlTagPtr listdata = vpd->getFirstChild("listdata");
	OmnString listdata_content = "";
	if(listdata)
	{
		listdata_content = listdata->toString();
	}
	AosXmlTagPtr creator = vpd->getFirstChild("creator");
	AosXmlTagPtr expander_creator;
	if(creator)
	{
		expander_creator = creator->getFirstChild("contents");
	}
	OmnString expander_content="";
	if(expander_creator)
	{
		expander_content = expander_creator->getNodeText();
	}
	AosXmlTagPtr creators = vpd->getFirstChild("creators");
	if(!creators)
	{
		OmnAlarm << "This gic's vpd miss creators nodes. It's necessary." << enderr;
		return false;
	}
	AosXmlTagPtr column = creators->getFirstChild();
	if(!column)
	{
		OmnAlarm << "This gic's vpd miss column nodes. It's necessary." << enderr;
		return false;
	}
	OmnString titles = "[";
	OmnString cwidths = "[";
	OmnString valbds = "[";
	OmnString edits = "[";
	OmnString statuses = "[";
	OmnString mCreators = "[";
	OmnString celldefs = "[";
	OmnString contentsNodeValue ="";
	while(column)
	{
		OmnString title = column->getAttrStr("name", "");
		titles << "\'" << title << "\'";
		OmnString cwidth = column->getAttrStr("cwidth", "");
		cwidths << "\'" << cwidth << "\'";
		OmnString valbd = column->getAttrStr("valbd", "");
		valbds << "\'" << valbd << "\'";
		OmnString edit = column->getAttrStr("editable", "true");
		edits << "\'" << edit << "\'";
		OmnString  status = column->getAttrStr("status", "");
		statuses << "\'" << status << "\'";
		AosXmlTagPtr contents = column->getFirstChild("contents");
		if(contents)
		{
			contentsNodeValue = contents->getNodeText();
	
		}
		mCreators << "\'" << contentsNodeValue << "\'";
		AosXmlTagPtr celldefs_xml = column->getFirstChild("celldefs");
		AosXmlTagPtr formula;
		if(celldefs_xml)
		{
			formula = celldefs_xml->getFirstChild();
		}
		if(formula)
		{
			OmnString celldefs_content = "";
			while(formula)
			{
				OmnString row = formula->getAttrStr("rowindex", "");
				celldefs_content << row << "[sep]";
				OmnString col = formula->getAttrStr("colindex", "");
				celldefs_content << col << "[sep]";
				OmnString content = formula->getNodeText();
				if(content != "")
				{
					celldefs_content << content;
				}
				formula = celldefs_xml->getNextChild();
				if(formula)
				{
					celldefs_content << "[sep]";
				}
			}
			celldefs <<"\'" << celldefs_content << "\'";
		}
		column = creators->getNextChild();
		if(column)
		{
			titles << ",";
			cwidths << ",";
			valbds << ",";
			edits << ",";
			statuses << ",";
			mCreators << ",";
			celldefs << ",";
		}
	}
	titles << "]";
	cwidths << "]";
	valbds << "]";
	edits << "]";
	statuses << "]";
	mCreators << "]";
	celldefs << "]";
	
	OmnString str=",";
	str	<< "frame:" << gic_frame << "," 
		<< "withoutbgc:" << withoutbgc << ","
		<< "editorable:" << gic_eidtorable << ","
		<< "isshopcard:" << gic_isshopcard << ","
		<< "sortable:" << gic_sortable << ","
		<< "listdata_content : \'"<< listdata_content <<"\'," 
		<< "enableHdMenu:" << gic_enableHdMenu << ","
		<< "gic_property:" << gic_property << ","
		<< "enableColumnResize:" << gic_enableColumnResize << ","
		<< "columnLines:" << gic_columnlines << ","
		<< "enableColumnMove:" << gic_columnMove << ","
		<< "buildTbar:" << gic_buildTbar << ","
		<< "hideHeaders:" << gic_hideHeaders << ","
		<< "rowNumberer:" << gic_rowNumberer << ","
		<< "checkColumn:" << gic_checkColumn << ","
		<< "pagingToolBar:" << gic_pb_def << ","
		<< "gic_lstn:\"" << gic_lstn << "\","
		<< "gic_mess:\"" << gic_mess << "\","
		<< "gic_check_bd:\"" << gic_check_bd << "\","
		<< "gic_delcolumn:\"" << gic_delcolumn << "\","
		<< "selectWay:\"" << gic_selectWay << "\","
		<< "gic_forceFit:\"" << gic_forceFit << "\","
//		<< "selectMol:\"" << gic_selectMod << "\","
		<< "ddTarget:\"" << gic_ddTarget << "\","		 
		<< "ddGroup:\"" << gic_ddSource << "\","		 
		<< "grouping:\"" << gic_grouping << "\","		 
		<< "groupname:\"" << gic_groupname << "\","		 
		<< "autosize:\"" << autosize << "\","		 
		<< "enableexpander:\"" << gic_expander << "\","		 
		<< "gic_expanding:\"" << gic_expanding << "\","
		<< "gic_autoexpandercolumn:\"" << gic_autoexpandercolumn << "\","		 
		<< "expander_content:\"" << expander_content << "\","		 
		<< "gic_list_bd:\"" << gic_list_bd << "\","		 
		<< "gic_toolbar:\"" << gic_toolbar << "\","		 
		<< "gic_export:\"" << gic_export << "\","		 
		<< "titles:\"" << titles << "\","
		<< "cwidths:\"" << cwidths << "\","
		<< "valbds:\"" << valbds << "\","
		<< "edits:\"" << edits << "\","
		<< "statuses:\"" << statuses << "\","
		<< "celldefs :\""<< celldefs <<"\"," 
		<< "mCreators:\"" << mCreators << "\"";
	
	code.mJson << str;
	return true;
}


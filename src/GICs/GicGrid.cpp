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
#include "GICs/GicGrid.h"

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

// static AosGicPtr sgGic = new AosGicGrid();

AosGicGrid::AosGicGrid(const bool flag)
:
AosGic(AOSGIC_GRID, AosGicType::eGrid, flag)
{
	OmnString errmsg;
	//AosGicType::addName(AOSGIC_LIST, AosGicType::eList, errmsg);
	//AosGicType::addName(AOSGIC_SIMPLELIST, AosGicType::eSimpleList, errmsg);
	AosGicType::addName(AOSGIC_LIST, AosGicType::eGrid, errmsg);
	AosGicType::addName(AOSGIC_SIMPLELIST, AosGicType::eGrid, errmsg);
}

AosGicGrid::~AosGicGrid()
{
}

bool	
AosGicGrid::generateCode(
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
	
	// 1. Generate HTML code. We will generate a <table> HTML tag. This
	// GIC assumes that the data is provided by 'obj'. 

	// The location of the data is specified by "gic_databd". "gic_databd"
	// should be a path pointing to the starting tag, which should be 
	// in the following format:
	// 	<object ...>
	// 		...
	// 		<records ...>
	// 			<record .../>
	// 			<record .../>
	// 			...
	// 			<record .../>
	// 		</records ...>
	// 		...
	// 	</object>
	//
	// 	Or the format:
	// 	<object ...>
	// 		<record .../>
	// 		<record .../>
	// 		...
	// 		<record .../>
	// 	</object>
	// 
	// We need the following information about the table structure:
	// 	<tabledef>
	// 		<column name="xxx" width="xxx" valuebd="xxx"/>
	// 		<column name="xxx" width="xxx" valuebd="xxx"/>
	// 		...
	// 		<column name="xxx" width="xxx" valuebd="xxx"/>
	// 	</tabledef>
	
	//AosDclDb dcl;
	//AosXmlTagPtr datacol = vpd->getFirstChild("datacol");	
	//OmnString gic_dcl_type = datacol->getAttrStr("gic_type","");
	/*if(gic_dcl_type == "db_datacol")
	{
		AosXmlTagPtr doc = dcl.retrieveData("100",vpd,obj,"","");
		if(!doc)
		{
			OmnAlarm << "Failed to retrieve the doc definition: " << vpd->toString() << enderr;
			return false;
		}
		AosXmlTagPtr doc_contents = doc->getFirstChild("Contents");
	
		AosXmlTagPtr table = vpd->getFirstChild("tabledef");
		if(!table)
		{
			OmnAlarm << "Failed to retrieve the table definition: " << vpd->toString() << enderr;
			return false;
		}

		AosXmlTagPtr tabledef = table->getFirstChild();
		if(!tabledef)
		{
			OmnAlarm << "Failed to retrieve the table definition: " << vpd->toString() << enderr;
			return false;
		}

		AosXmlTagPtr column = tabledef->getFirstChild();
		if (!column)
		{
			OmnAlarm << "No columns specified!" << enderr;
			return false;
		}

		OmnString path = vpd->getAttrStr("gic_datapath");
		//AosXmlTagPtr dataroot = obj;
		AosXmlTagPtr dataroot = doc_contents;

		if(path != "")
		{
			dataroot = obj->getFirstChild(path);
			if (!dataroot)
			{
				OmnAlarm << "Missing the required path: " << path << enderr;
				return false;
			}
		}

		// 	<a-name>
		//		<record .../>
		//		<record .../>
		//		...
		//	</a-name>
		OmnString totalRecords = dataroot->getAttrStr("total");
		AosXmlTagPtr data = dataroot->getFirstChild();
		if (!data)
		{
			OmnAlarm << "No records found!" << enderr;
			return false;
		}
*/
	AosXmlTagPtr creators = vpd->getFirstChild("creators");	
	if(!creators) return false;
	AosXmlTagPtr creator = creators->getFirstChild("creator");
	if(!creator) return false;
	OmnString gic_itemh = creator->getAttrStr("gic_height","19");
	OmnString gic_h = vpd->getAttrStr("gic_itemh","19");
	if(atoi(gic_itemh.data())>atoi(gic_h.data()))
	{
		gic_h = gic_itemh;
	}
	vpd->setAttr("gic_h",gic_h);
	aos_assert_r(convertToJson(vpd, obj, code, htmlPtr), false);
	return true;
}

bool
AosGicGrid::convertToJson(
	AosXmlTagPtr &vpd,
	const AosXmlTagPtr &obj,
	AosHtmlCode &code,
	const AosHtmlReqProcPtr &htmlPtr)
{
	// This function converts the VPD to Json form.
	OmnString gic_title = vpd->getAttrStr("gic_title");
	OmnString gic_frame = vpd->getAttrStr("gic_frame", "false");
	OmnString gic_rcdselbd = vpd->getAttrStr("gic_rcdselbd");
	OmnString gic_hideHeaders = vpd->getAttrStr("gic_hideHeaders", "true");
	OmnString gic_rowNumberer = vpd->getAttrStr("gic_rowNumberer", "false");
	OmnString gic_checkColumn = vpd->getAttrStr("gic_checkColumn", "false");
	OmnString gic_pb_def = vpd->getAttrStr("gic_pb_def","true");
	OmnString gic_lstn = vpd->getAttrStr("gic_lstn");
	
	OmnString gic_itemBorder = vpd->getAttrStr("gic_itemBorder","0");
	OmnString gic_itemBorderColor = vpd->getAttrStr("gic_itemBorderColor","#ffffff");
	if(gic_itemBorderColor != "") gic_itemBorderColor = AosHtmlUtil::getWebColor(gic_itemBorderColor);
	OmnString gic_vs = vpd->getAttrStr("gic_vs", "0");
	OmnString gic_linset = vpd->getAttrStr("gic_linset", "0");
	OmnString gic_rinset = vpd->getAttrStr("gic_rinset", "0");
	OmnString gic_binset = vpd->getAttrStr("gic_binset", "0");
	OmnString gic_tinset = vpd->getAttrStr("gic_tinset", "0");
	OmnString gic_h = vpd->getAttrStr("gic_h", "19");

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
	
	OmnString gic_bgcolor = vpd->getAttrStr("gic_bgcolor", "transparent");
	if(gic_bgcolor != "transparent") gic_bgcolor = AosHtmlUtil::getWebColor(gic_bgcolor);
	
	OmnString gic_border = vpd->getAttrStr("gic_border");
	OmnString gic_border_color = vpd->getAttrStr("gic_border_color");
	if(gic_border_color != "") gic_border_color = AosHtmlUtil::getWebColor(gic_border_color);

	AosXmlTagPtr creators = vpd->getFirstChild("creators");	
	if(!creators) return false;
	AosXmlTagPtr creator = creators->getFirstChild();
	if(!creator) return false;
	AosHtmlCode creatorCode;
	AosXmlTagPtr mobj;

	OmnString json, css;
	json << ",mCreators:[";
	while(creator)
	{
		//AosGic::createGic(htmlPtr,creator,mobj,"",creatorCode);
		AosGic::createGic(htmlPtr,creator,mobj,"",creatorCode, -1, -1); //Ketty 2011/09/22
		json << creatorCode.mJson;
		css << creatorCode.mCss;
		creatorCode.reset();
		creator = creators->getNextChild();
		if(creator) json << ",";
	}

	json << "],"
		 << "gic_itemBorder:" << gic_itemBorder << ","
		 << "gic_itemBorderColor:\"" << gic_itemBorderColor << "\","
		 << "gic_vs:" << gic_vs << ","
		 << "gic_linset:" << gic_linset << ","
		 << "gic_rinset:" << gic_rinset << ","
		 << "gic_binset:" << gic_binset << ","
		 << "gic_tinset:" << gic_tinset << ","
		 << "gic_h:" << gic_h << ","	
		 << "gic_scrollBar_v:" << gic_scrollBar_v << ","
		 << "gic_scrollBar_h:" << gic_scrollBar_h << ","
		 << "gic_forceFit:" << gic_forceFit << ","
		 << "gic_rcdselbd:\"" << gic_rcdselbd << "\","
		
		 << "title:\"" << gic_title << "\","
		 << "frame:" << gic_frame << "," 
		 << "enableHdMenu:" << gic_enableHdMenu << ","
		 << "enableColumnResize:" << gic_enableColumnResize << ","
		 << "columnLines:" << gic_columnlines << ","
		 << "enableColumnMove:" << gic_columnMove << ","
		 << "hideHeaders:" << gic_hideHeaders << ","
		
		 << "rowNumberer:" << gic_rowNumberer << ","
		 << "checkColumn:" << gic_checkColumn << ","
		 << "editorable:" << gic_eidtorable << ","
		 << "sortable:" << gic_sortable << ","
		 << "pagingToolBar:" << gic_pb_def << ","
		 << "gic_lstn:\"" << gic_lstn << "\","
		 << "bgcolor:\"" << gic_bgcolor << "\","
		 << "border:\"" << gic_border << "\","
		 << "border_color:\"" << gic_border_color << "\","
		 << "selectWay:\"" << gic_selectWay << "\","
		 << "ddTarget:\"" << gic_ddTarget << "\","		 
		 << "ddGroup:\"" << gic_ddSource << "\"";		 
	code.mJson << json;
	code.mCss << css;
	return true;
}

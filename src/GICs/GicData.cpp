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
// 12/29/2010: Created by Max 
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicData.h"
#include "HtmlModules/DclDb.h"
#include "HtmlUtil/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

static const OmnString strNull = "";

AosGicData::AosGicData(const bool flag)
:
AosGic(AOSGIC_DATA, AosGicType::eData, flag)
{
}


AosGicData::~AosGicData()
{
}


bool	
AosGicData::generateCode(
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
	//OmnString temp="";
	//OmnString gic_rootIconCollapsed=vpd->getAttrStr("gic_rootIconCollapsed","");//根结点图标的路径绑定
	//if(gic_rootIconCollapsed!="")
	//{
	//	temp<<"url(http://218.64.170.28/lps-4.7.2/images/"<<gic_rootIconCollapsed<<")";
	//	gic_rootIconCollapsed=temp;
	//	temp="";
	//}
	//code.mCss<<".x-tree-node-leaf .x-tree-node-icon{background-image:"<<gic_nodeLeaf<<";}";
	//code.mCss<<".x-tree-node-collapsed .x-tree-node-icon{background-image:"<<gic_nodeCollapsed<<";}";
	//code.mCss<<".x-tree-node-expanded .x-tree-node-icon{background-image:"<<gic_nodeExpanded<<";}";
	//code.mCss<<".x-tree-node-collapsed .root{background-image:"<<gic_rootIconCollapsed<<";}";
	//code.mCss<<".x-tree-node-expanded .root{background-image:"<<gic_rootIconExpanded<<";}";
	//
	aos_assert_r(convertToJson(vpd, obj,parentid,code),false);
	return true;
}
bool
AosGicData::convertToJson(
		AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		const OmnString &parentid,
		AosHtmlCode &code)
{
	//This function converts the VPD to Json form.
	OmnString gic_showWhat=vpd->getAttrStr("gic_showWhat","time");
	OmnString gic_showUrl=vpd->getAttrStr("gic_showUrl","country");
	bool gic_dynamic=vpd->getAttrStr("gic_dynamic","false")=="true"?true:false;
//	bool gic_autoWidth=false;
//	bool gic_autoHeight=false;
	code.mJson<<",xtype:'gic_data',";
	code.mJson<<"gic_dynamic:"<<gic_dynamic<<",";
	code.mJson<<"gic_showUrl:\""<<gic_showUrl<<"\",";
	code.mJson<<"gic_showWhat:\""<<gic_showWhat<<"\"";
	//code.mJson<<"animate:"<<gic_animate<<",";
	//code.mJson<<"autoHeight:"<<gic_autoHeight<<",";
	//code.mJson<<"root:\""<<gic_rootText<<"\"";//root  config end
	return true;
}

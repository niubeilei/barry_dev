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
// 07/08/2010: Created by Max 
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicColumnTree.h"
#include "HtmlModules/DclDb.h"
#include "HtmlUtil/HtmlUtil.h"
#include "SEUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

static const OmnString strNull = "";

AosGicColumnTree::AosGicColumnTree(const bool flag)
:
AosGic(AOSGIC_COLUMNTREE, AosGicType::eColumnTree, flag)
{
}


AosGicColumnTree::~AosGicColumnTree()
{
}


bool	
AosGicColumnTree::generateCode(
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
	//OmnString gic_rootIconExpanded=vpd->getAttrStr("gic_rootIconExpanded","");//根结点图标的路径绑定
	//OmnString gic_nodeLeaf=vpd->getAttrStr("gic_nodeLeaf","");//叶子结点图标
	//OmnString gic_nodeCollapsed=vpd->getAttrStr("gic_nodeCollapsed","");//结点闭合图标
	//OmnString gic_nodeExpanded=vpd->getAttrStr("gic_nodeExpanded","");//结点的展开图标
	//if(gic_rootIconCollapsed!="")
	//{
	//	temp<<"url(http://218.64.170.28/lps-4.7.2/images/"<<gic_rootIconCollapsed<<")";
	//	gic_rootIconCollapsed=temp;
	//	temp="";
	//}
	//if(gic_rootIconExpanded!="")
	//{
	//	temp<<"url(http://218.64.170.28/lps-4.7.2/images/"<<gic_rootIconExpanded<<")";
	//	gic_rootIconExpanded=temp;
	//	temp="";
	//}
	//if(gic_nodeLeaf!="")
	//{
	//	temp<<"url(http://218.64.170.28/lps-4.7.2/images/"<<gic_nodeLeaf<<")";
	//	gic_nodeLeaf=temp;
	//	temp="";
	//}
	//if(gic_nodeCollapsed!="")
	//{
	//	temp<<"url(http://218.64.170.28/lps-4.7.2/images/"<<gic_nodeCollapsed<<")";
	//	gic_nodeCollapsed=temp;
	//	temp="";
	//}
	//if(gic_nodeExpanded!="")
	//{
	//	temp<<"url(http://218.64.170.28/lps-4.7.2/images/"<<gic_nodeExpanded<<")";
	//	gic_nodeExpanded=temp;
	//	temp="";
	//}
	//code.mCss<<".x-tree-node-leaf .x-tree-node-icon{background-image:"<<gic_nodeLeaf<<";}";
	//code.mCss<<".x-tree-node-collapsed .x-tree-node-icon{background-image:"<<gic_nodeCollapsed<<";}";
	//code.mCss<<".x-tree-node-expanded .x-tree-node-icon{background-image:"<<gic_nodeExpanded<<";}";
	//code.mCss<<".x-tree-node-collapsed .root{background-image:"<<gic_rootIconCollapsed<<";}";
	//code.mCss<<".x-tree-node-expanded .root{background-image:"<<gic_rootIconExpanded<<";}";
	//
	return convertToJson(htmlPtr, vpd, obj,parentid,code);
}


bool
AosGicColumnTree::convertToJson(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		const OmnString &parentid,
		AosHtmlCode &code)
{
	AosDclDb dcl;
	AosXmlTagPtr docroot = dcl.retrieveData(htmlPtr, vpd, obj, "");
	AosXmlTagPtr doc = docroot->getFirstChild();//获得根结点
	

	//This function converts the VPD to Json form.
	OmnString gic_rootIconCollapsed=vpd->getAttrStr("gic_rootIconCollapsed","");//根结点图标的路径绑定
	OmnString gic_rootIconExpanded=vpd->getAttrStr("gic_rootIconExpanded","");//根结点图标的路径绑定
	OmnString gic_nodeLeaf=vpd->getAttrStr("gic_nodeLeaf","");//叶子结点图标
	OmnString gic_nodeCollapsed=vpd->getAttrStr("gic_nodeCollapsed","");//结点闭合图标
	OmnString gic_nodeExpanded=vpd->getAttrStr("gic_nodeExpanded","");//结点的展开图标
	OmnString gic_leaficon=vpd->getAttrStr("gic_leaficon","zky_tnail");//叶子结点图标
	OmnString gic_collapseicon=vpd->getAttrStr("gic_collapseicon","zky_tnail");//结点闭合图标
	OmnString gic_expandicon=vpd->getAttrStr("gic_expandicon","zky_tnail");//结点的展开图标
	OmnString gic_lbbd=vpd->getAttrStr("gic_lbbd","");//结点文本的路径绑定
	bool gic_useArrows=vpd->getAttrStr("gic_useArrows","false")=="true"?true:false;
	bool gic_enableD=vpd->getAttrStr("gic_enableD","false")=="true"?true:false;//树是否可以拖动
	OmnString gic_title=vpd->getAttrStr("gic_title","");
	bool gic_animate=vpd->getAttrStr("gic_animate","false")=="true"?true:false;
	//bool gic_draggable=vpd->getAttrStr("gic_draggable","false")=="true"?true:false;//根结点是否可以拖动
	OmnString gic_rootTipText=vpd->getAttrStr("gic_rootTipText","没取到根结点的提示文本");
	OmnString gic_rootText=vpd->getAttrStr("gic_rootText","请配置根结点文本");

	OmnString gic_dragGroup=vpd->getAttrStr("gic_dragGroup","");
	OmnString gic_dropGroup=vpd->getAttrStr("gic_dropGroup","");
	//bool gic_qtip=vpd->getAttrStr("gic_qtip","false")=="true"?true:false;//树的提示功能开启
/*	OmnString cmp_tname;//查询的表名
	OmnString cmp_fnames;//查询的字段
	int cmp_psize;//查询的记录个数
	
	//临时解决方案
	AosXmlTagPtr dclvpd = vpd->getFirstChild("datacol");
	if (dclvpd)
	{
		cmp_tname=dclvpd->getAttrStr("cmp_tname","");//get tablename
		cmp_fnames=dclvpd->getAttrStr("cmp_fnames","");//get fieldname
		cmp_psize=dclvpd->getAttrInt("cmp_psize",20);
	}
	


	*/
	AosXmlTagPtr creators = vpd->getFirstChild("creators");
	if(!creators)
	{
		OmnAlarm << "The vpd missing createors node" << enderr;
		return false;
	}
	AosXmlTagPtr column = creators->getFirstChild();
	if (!column)
	{
		OmnAlarm << "The vpd missing column node" << enderr;
		return false;
	}
	OmnString columnConfig = "";
	OmnString head = column->getAttrStr("gic_head", "");
	OmnString width = column->getAttrStr("gic_fw", "");
	OmnString field = column->getAttrStr("gic_field", "");
	columnConfig<<"gic_head|^|"<<head<<"|$|gic_fw|^|"<<width<<"|$|gic_field|^|"<<field;
	column = creators->getNextChild();
	while(column)
	{
		columnConfig<<"|$$|";
		head = column->getAttrStr("gic_head", "");
		width = column->getAttrStr("gic_fw", "");
		field = column->getAttrStr("gic_field", "");
		columnConfig<<"gic_head|^|"<<head<<"|$|gic_fw|^|"<<width<<"|$|gic_field|^|"<<field;
		column = creators->getNextChild();
	}	
	bool gic_autoWidth=false;
	bool gic_autoHeight=false;
	code.mJson<<",xtype:'gic_columntree',";
	code.mJson<<"gic_rootIconCollapsed:\""<<gic_rootIconCollapsed<<"\",";//根结点闭合图标
	code.mJson<<"gic_rootIconExpanded:\""<<gic_rootIconExpanded<<"\",";//根结点展开图标
	code.mJson<<"gic_nodeLeaf:\""<<gic_nodeLeaf<<"\",";//叶子图标
	code.mJson<<"gic_nodeCollapsed:\""<<gic_nodeCollapsed<<"\",";//结点闭合图标
	code.mJson<<"gic_nodeExpanded:\""<<gic_nodeExpanded<<"\",";//结点展开图标
	//code.mJson<<"cmp:{cmp_tname:\""<<cmp_tname<<"\",cmp_fnames:\""<<cmp_fnames<<"\",cmp_psize:"<<cmp_psize<<"},";
	code.mJson<<"gic_leaficon:\""<<gic_leaficon<<"\",";//图标名称绑定
	code.mJson<<"gic_expandicon:\""<<gic_expandicon<<"\",";//图标名称绑定
	code.mJson<<"gic_collapseicon:\""<<gic_collapseicon<<"\",";//图标名称绑定
	code.mJson<<"dragGroup:\""<<gic_dragGroup<<"\",";
	code.mJson<<"dropGroup:\""<<gic_dropGroup<<"\",";
	code.mJson<<"enableDD:"<<true<<",";
	code.mJson<<"animate:"<<gic_animate<<",";
	code.mJson<<"columnConfig:\""<<columnConfig<<"\",";
	code.mJson<<"rootVisible:false,";
	code.mJson<<"useArrows:"<<gic_useArrows<<",";
	code.mJson<<"enableD:"<<gic_enableD<<",";//树面板拖动
	code.mJson<<"autoScroll :true,";//记录较多时使用滚动条
	code.mJson<<"title:\""<<gic_title<<"\",";
	code.mJson<<"lbbd:\""<<gic_lbbd<<"\",";
	code.mJson<<"autoWidth:"<<gic_autoWidth<<",";
	code.mJson<<"autoHeight:"<<gic_autoHeight<<",";
	code.mJson<<"root:\""<<gic_rootText<<"\"";//root config end
	return true;
}

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
#include "GICs/GicTree.h"
#include "HtmlModules/DclDb.h"
#include "HtmlUtil/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

static const OmnString strNull = "";

AosGicTree::AosGicTree(const bool flag)
:
AosGic(AOSGIC_TREE, AosGicType::eTree, flag)
{
}


AosGicTree::~AosGicTree()
{
}


bool	
AosGicTree::generateCode(
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
	//OmnScreen<<"css的数据："<<code.mCss<<endl;
	aos_assert_r(convertToJson(vpd, obj,parentid,code),false);
	return true;
}




bool
AosGicTree::convertToJson(
		AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		const OmnString &parentid,
		AosHtmlCode &code)
{
	//This function converts the VPD to Json form.
	OmnString gic_rootIconCollapsed=vpd->getAttrStr("gic_rootIconCollapsed","");//根结点图标的路径绑定
	OmnString gic_rootIconExpanded=vpd->getAttrStr("gic_rootIconExpanded","");//根结点图标的路径绑定
	OmnString gic_nodeLeaf=vpd->getAttrStr("gic_nodeLeaf","");//叶子结点图标
	OmnString gic_nodeCollapsed=vpd->getAttrStr("gic_nodeCollapsed","");//结点闭合图标
	OmnString gic_nodeExpanded=vpd->getAttrStr("gic_nodeExpanded","");//结点的展开图标
	OmnString gic_leaficon=vpd->getAttrStr("gic_leaficon","");//叶子结点图标
	OmnString gic_collapseicon=vpd->getAttrStr("gic_collapseicon","");//结点闭合图标
	OmnString gic_expandicon=vpd->getAttrStr("gic_expandicon","zky_tnail");//结点的展开图标
	OmnString gic_lbbd=vpd->getAttrStr("gic_lbbd","");//结点文本的路径绑定
	OmnString gic_xmlText=vpd->getAttrStr("gic_xmlText","");//结点文本的路径绑定
	OmnString gic_rtlblbbd=vpd->getAttrStr("gic_rtlblbbd","");//根结点文本的路径绑定
	OmnString gic_nodeclickchoice=vpd->getAttrStr("gic_nodeclickchoice","");//结点选中的选择
	bool gic_useArrows=vpd->getAttrStr("gic_useArrows","false")=="true"?true:false;
	OmnString gic_enableDD=vpd->getAttrStr("gic_enableDD","false");//树结点是否可以拖放
	bool gic_enableD=vpd->getAttrStr("gic_enableD","false")=="true"?true:false;//树是否可以拖动
	bool gic_enableCheckbox=vpd->getAttrStr("gic_enableCheckbox","false")=="true"?true:false;
	bool gic_enableChildrenDrop=vpd->getAttrStr("gic_enableChildrenDrop","false")=="true"?true:false;
	bool gic_enableContextmenu=vpd->getAttrStr("gic_enableContextmenu","false")=="true"?true:false;
	bool gic_nodeOnlyCopy=vpd->getAttrStr("gic_nodeOnlyCopy","false")=="true"?true:false;
	bool gic_showAttributes=vpd->getAttrStr("gic_showAttributes","false")=="true"?true:false;
	bool gic_enableCreateData=vpd->getAttrStr("gic_enableCreateData","false")=="true"?true:false;
	bool gic_rootUpdate=vpd->getAttrStr("gic_rootUpdate","false")=="true"?true:false;
	bool gic_hidden=false;
	OmnString gic_lstn = vpd->getAttrStr("gic_lstn");//动态更新列表控制变量
	OmnString gic_objsync=vpd->getAttrStr("gic_objsync","");
	OmnString gic_title=vpd->getAttrStr("gic_title","");
	OmnString tre_objtpl=vpd->getAttrStr("gic_objtpl","");
	OmnString tre_objidbd=vpd->getAttrStr("gic_objidbd","zky_objid");
	OmnString gic_obbd=vpd->getAttrStr("gic_obbd","@gic_data");
	bool gic_animate=vpd->getAttrStr("gic_animate","false")=="true"?true:false;
	bool gic_parseRule=vpd->getAttrStr("gic_parseRule","false")=="true"?true:false;
	bool gic_rootVisible=vpd->getAttrStr("gic_rootVisible","false")=="true"?true:false;
	OmnString gic_rootTipText=vpd->getAttrStr("gic_rootTipText","没取到根结点的提示文本");
	OmnString gic_rootText=vpd->getAttrStr("gic_rootText","请配置根结点文本");
	OmnString gic_dragGroup=vpd->getAttrStr("gic_dragGroup","TreeDD");
	OmnString gic_dropGroup=vpd->getAttrStr("gic_dropGroup","TreeDD");
	OmnString gic_dynamicVerbBd=vpd->getAttrStr("gic_dynamicVerbBd","zky_docid");
	OmnString gic_dynamicVerb=vpd->getAttrStr("gic_dynamicVerb","");
	OmnString gic_labelmd=vpd->getAttrStr("gic_labelmd","method1");
	OmnString gic_rightmn=vpd->getAttrStr("gic_rightmn","");
	OmnString gic_labelnm=vpd->getAttrStr("gic_labelnm","");
	OmnString gic_labelam=vpd->getAttrStr("gic_labelam","gic_name");
	OmnString gic_lbschm4=vpd->getAttrStr("gic_lbschm4","");
	OmnString gic_lbschm5=vpd->getAttrStr("gic_lbschm5","");
	OmnString gic_lbschm6=vpd->getAttrStr("gic_lbschm6","");
	OmnString gic_nimgsrc=vpd->getAttrStr("gic_nimgsrc","");
	OmnString gic_limgsrc=vpd->getAttrStr("gic_limgsrc","");
	OmnString gic_displvl=vpd->getAttrStr("gic_displvl","");
	OmnString gic_nlabelbd=vpd->getAttrStr("gic_nlabelbd","");
	OmnString gic_llabelbd=vpd->getAttrStr("gic_llabelbd","");
	OmnString gic_nodelb=vpd->getAttrStr("gic_nodelb","");
	OmnString gic_leaflb=vpd->getAttrStr("gic_leaflb","");
	bool gic_qtip=vpd->getAttrStr("gic_qtip","false")=="true"?true:false;//树的提示功能开启
	bool gic_asEditor=vpd->getAttrStr("gic_asEditor","false")=="true"?true:false;//作为编辑器使用
	bool gic_readonly=vpd->getAttrStr("gic_readonly","false")=="true"?true:false;//作为编辑器使用
	bool gic_dynupdate=vpd->getAttrStr("gic_dynupdate","false")=="true"?true:false;//作为编辑器使用
	bool gic_asXmlTree=vpd->getAttrStr("gic_asXmlTree","false")=="true"?true:false;//作为编辑器使用
	bool gic_addLeafOnly=vpd->getAttrStr("gic_addLeafOnly","false")=="true"?true:false;//树的提示功能开启
	bool gic_onlyShowContainer=vpd->getAttrStr("gic_onlyShowContainer","false")=="true"?true:false;//树的提示功能开启
	bool gic_autoWidth=false;
	bool gic_autoHeight=false;
	bool gic_app = vpd->getAttrStr("gic_app","false")=="true"?true:false;
	code.mJson<<",xtype:'gic_tree',";
	code.mJson<<"gic_rootIconCollapsed:\""<<gic_rootIconCollapsed<<"\",";//根结点闭合图标
	code.mJson<<"gic_rootIconExpanded:\""<<gic_rootIconExpanded<<"\",";//根结点展开图标
	code.mJson<<"gic_nodeLeaf:\""<<gic_nodeLeaf<<"\",";//叶子图标
	code.mJson<<"gic_nodeCollapsed:\""<<gic_nodeCollapsed<<"\",";//结点闭合图标
	code.mJson<<"gic_nodeExpanded:\""<<gic_nodeExpanded<<"\",";//结点展开图标
	code.mJson<<"edt_query:\""<<gic_obbd<<"\",";
	code.mJson<<"gic_labelmd:\""<<gic_labelmd<<"\",";
	code.mJson<<"gic_rightmn:\""<<gic_rightmn<<"\",";
	code.mJson<<"gic_labelnm:\""<<gic_labelnm<<"\",";
	code.mJson<<"gic_labelam:\""<<gic_labelam<<"\",";
	code.mJson<<"gic_lbschm4:\""<<gic_lbschm4<<"\",";
	code.mJson<<"gic_lbschm5:\""<<gic_lbschm5<<"\",";
	code.mJson<<"gic_lbschm6:\""<<gic_lbschm6<<"\",";
	code.mJson<<"gic_nimgsrc:\""<<gic_nimgsrc<<"\",";
	code.mJson<<"gic_limgsrc:\""<<gic_limgsrc<<"\",";
	code.mJson<<"gic_displvl:\""<<gic_displvl<<"\",";
	code.mJson<<"gic_nlabelbd:\""<<gic_nlabelbd<<"\",";
	code.mJson<<"gic_llabelbd:\""<<gic_llabelbd<<"\",";
	code.mJson<<"gic_nodelb:\""<<gic_nodelb<<"\",";
	code.mJson<<"gic_leaflb:\""<<gic_leaflb<<"\",";
	code.mJson<<"gic_lstn:\""<<gic_lstn<<"\",";
	code.mJson<<"animate:"<<gic_animate<<",";
	code.mJson<<"enableTip:"<<gic_qtip<<",";//树的提示功能开启
	code.mJson<<"gic_addLeafOnly:"<<gic_addLeafOnly<<",";//只能添加叶子
	code.mJson<<"gic_onlyShowContainer:"<<gic_onlyShowContainer<<",";//只能添加叶子
	code.mJson<<"gic_asEditor:"<<gic_asEditor<<",";//只能添加叶子
	code.mJson<<"gic_readonly:"<<gic_readonly<<",";//只能添加叶子
	code.mJson<<"gic_dynupdate:"<<gic_dynupdate<<",";//只能添加叶子
	code.mJson<<"gic_asXmlTree:"<<gic_asXmlTree<<",";//作为xmltree
	code.mJson<<"rootVisible:"<<gic_rootVisible<<",";
	code.mJson<<"expand:"<<true<<",";//展开树的所有结点
	code.mJson<<"useArrows:"<<gic_useArrows<<",";
	code.mJson<<"gic_showAttributes:"<<gic_showAttributes<<",";
	code.mJson<<"gic_enableCreateData:"<<gic_enableCreateData<<",";
	code.mJson<<"tre_objsync:\""<<gic_objsync<<"\",";
	code.mJson<<"gic_nodeclickchoice:\""<<gic_nodeclickchoice<<"\",";
	code.mJson<<"gic_leaficon:\""<<gic_leaficon<<"\",";//图标名称绑定
	code.mJson<<"gic_expandicon:\""<<gic_expandicon<<"\",";//图标名称绑定
	code.mJson<<"gic_collapseicon:\""<<gic_collapseicon<<"\",";//图标名称绑定
	code.mJson<<"dragGroup:\""<<gic_dragGroup<<"\",";
	code.mJson<<"dropGroup:\""<<gic_dropGroup<<"\",";
	code.mJson<<"gic_dynamicVerb:\""<<gic_dynamicVerb<<"\",";
	code.mJson<<"gic_dynamicVerbBd:\""<<gic_dynamicVerbBd<<"\",";
	code.mJson<<"enableDD:"<<true<<",";
	code.mJson<<"gic_enableContextmenu:\""<<gic_enableContextmenu<<"\",";
	code.mJson<<"tre_objtpl:\""<<tre_objtpl<<"\",";
	code.mJson<<"tre_objidbd:\""<<tre_objidbd<<"\",";
	code.mJson<<"enableD:"<<gic_enableD<<",";
	code.mJson<<"enableCheckbox:"<<gic_enableCheckbox<<",";
	code.mJson<<"gic_enableChildrenDrop:"<<gic_enableChildrenDrop<<",";
	code.mJson<<"gic_nodeOnlyCopy:"<<gic_nodeOnlyCopy<<",";
	code.mJson<<"autoScroll :true,";//记录较多时使用滚动条
	code.mJson<<"hidden:"<<gic_hidden<<",";
	code.mJson<<"title:\""<<gic_title<<"\",";
	//code.mJson<<"title:\"title\",";
	code.mJson<<"lbbd:\""<<gic_lbbd<<"\",";
	code.mJson<<"gic_xmlText:\""<<gic_xmlText<<"\",";
	code.mJson<<"gic_rtlblbbd:\""<<gic_rtlblbbd<<"\",";
	code.mJson<<"gic_parseRule:\""<<gic_parseRule<<"\",";
	code.mJson<<"autoWidth:"<<gic_autoWidth<<",";
	code.mJson<<"autoHeight:"<<gic_autoHeight<<",";
	code.mJson<<"root:\""<<gic_rootText<<"\",";
	code.mJson<<"gic_app:"<<gic_app<<",";
	code.mJson<<"gic_rootUpdate:"<<gic_rootUpdate<<"";//root  config end
	return true;
}

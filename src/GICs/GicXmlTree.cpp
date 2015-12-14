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
// 10/11/2010: Created by Max 
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicXmlTree.h"
#include "HtmlModules/DclDb.h"
#include "HtmlUtil/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

static const OmnString strNull = "";

AosGicXmlTree::AosGicXmlTree(const bool flag)
:
AosGic(AOSGIC_XMLTREE, AosGicType::eXmlTree, flag)
{
}


AosGicXmlTree::~AosGicXmlTree()
{
}


bool	
AosGicXmlTree::generateCode(
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
AosGicXmlTree::convertToJson(
		AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		const OmnString &parentid,
		AosHtmlCode &code)
{
	//AosDclDb dcl;
	//AosXmlTagPtr docroot = dcl.retrieveData("100", vpd, obj, "", "");
	//AosXmlTagPtr doc = docroot->getFirstChild();//获得根结点
	
//	OmnScreen<<"doc的数据："<<doc->getData()<<endl;
//	OmnScreen<<"doc的数据结束"<<endl;

	//This function converts the VPD to Json form.
	OmnString gic_rootIconCollapsed=vpd->getAttrStr("gic_rootIconCollapsed","");//根结点图标的路径绑定
	OmnString gic_rootIconExpanded=vpd->getAttrStr("gic_rootIconExpanded","");//根结点图标的路径绑定
	OmnString gic_nodeLeaf=vpd->getAttrStr("gic_nodeLeaf","");//叶子结点图标
	OmnString gic_nodeCollapsed=vpd->getAttrStr("gic_nodeCollapsed","");//结点闭合图标
	OmnString gic_nodeExpanded=vpd->getAttrStr("gic_nodeExpanded","");//结点的展开图标
	OmnString gic_lbbd=vpd->getAttrStr("gic_lbbd","");//结点文本的路径绑定
	bool gic_useArrows=vpd->getAttrStr("gic_useArrows","false")=="true"?true:false;
//	bool gic_enableDD=vpd->getAttrStr("gic_enableDD","false")=="true"?true:false;//树结点是否可以拖放
	bool gic_enableD=vpd->getAttrStr("gic_enableD","false")=="true"?true:false;//树是否可以拖动
	bool gic_enableCheckbox=vpd->getAttrStr("gic_enableCheckbox","false")=="true"?true:false;
	bool gic_enableChildrenDrop=vpd->getAttrStr("gic_enableChildrenDrop","false")=="true"?true:false;
	bool gic_nodeOnlyCopy=vpd->getAttrStr("gic_nodeOnlyCopy","false")=="true"?true:false;
	bool gic_hidden=false;
	OmnString gic_lstn = vpd->getAttrStr("gic_lstn");//动态更新列表控制变量
	OmnString gic_objsync=vpd->getAttrStr("gic_objsync","");
	OmnString gic_property=vpd->getAttrStr("gic_property","");//根据此绑定的值寻找相应的组件
	OmnString gic_title=vpd->getAttrStr("gic_title","");
	OmnString tre_objtpl=vpd->getAttrStr("gic_objtpl","");
	OmnString tre_objidbd=vpd->getAttrStr("gic_objidbd","zky_objid");
	bool gic_animate=vpd->getAttrStr("gic_animate","false")=="true"?true:false;
	bool gic_showAttributes=vpd->getAttrStr("gic_showAttributes","false")=="true"?true:false;
	bool gic_saveData=vpd->getAttrStr("gic_saveData","false")=="true"?true:false;
	bool gic_rootVisible=vpd->getAttrStr("gic_rootVisible","false")=="true"?true:false;
	bool gic_enableCreateData=vpd->getAttrStr("gic_enableCreateData","false")=="true"?true:false;
	//bool gic_draggable=vpd->getAttrStr("gic_draggable","false")=="true"?true:false;//根结点是否可以拖动
	OmnString gic_rootTipText=vpd->getAttrStr("gic_rootTipText","没取到根结点的提示文本");
	OmnString gic_rootText=vpd->getAttrStr("gic_rootText","请配置根结点文本");
	OmnString gic_dragGroup=vpd->getAttrStr("gic_dragGroup","");
	OmnString gic_dropGroup=vpd->getAttrStr("gic_dropGroup","");
	OmnString gic_ddGroup=vpd->getAttrStr("gic_ddGroup","TreeDD");
	bool gic_qtip=vpd->getAttrStr("gic_qtip","false")=="true"?true:false;//树的提示功能开启
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
	


	OmnScreen<<"cmp************************的数据："<<cmp_tname<<endl;
	OmnScreen<<"cmp************************的数据："<<cmp_fnames<<endl;
*/	bool gic_autoWidth=false;
	bool gic_autoHeight=false;
	code.mJson<<",xtype:'gic_xmltree',";
	code.mJson<<"gic_rootIconCollapsed:\""<<gic_rootIconCollapsed<<"\",";//根结点闭合图标
	code.mJson<<"gic_rootIconExpanded:\""<<gic_rootIconExpanded<<"\",";//根结点展开图标
	code.mJson<<"gic_nodeLeaf:\""<<gic_nodeLeaf<<"\",";//叶子图标
	code.mJson<<"gic_nodeCollapsed:\""<<gic_nodeCollapsed<<"\",";//结点闭合图标
	code.mJson<<"gic_nodeExpanded:\""<<gic_nodeExpanded<<"\",";//结点展开图标
	code.mJson<<"gic_lstn:\""<<gic_lstn<<"\",";
//	code.mJson<<"cmp:{cmp_tname:\""<<cmp_tname<<"\",cmp_fnames:\""<<cmp_fnames<<"\",cmp_psize:"<<cmp_psize<<"},";
	code.mJson<<"gic_enableCreateData:"<<gic_enableCreateData<<",";
	code.mJson<<"animate:"<<gic_animate<<",";
	code.mJson<<"gic_showAttributes:"<<gic_showAttributes<<",";
	code.mJson<<"enableTip:"<<gic_qtip<<",";//树的提示功能开启
	code.mJson<<"rootVisible:"<<gic_rootVisible<<",";
	code.mJson<<"expand:"<<false<<",";//展开树的所有结点
	code.mJson<<"useArrows:"<<gic_useArrows<<",";
	code.mJson<<"gic_saveData:"<<gic_saveData<<",";
	code.mJson<<"tre_objsync:\""<<gic_objsync<<"\",";
	code.mJson<<"gic_property:\""<<gic_property<<"\",";
	code.mJson<<"dragGroup:\""<<gic_dragGroup<<"\",";
	code.mJson<<"dropGroup:\""<<gic_dropGroup<<"\",";
	code.mJson<<"enableDD:"<<true<<",";
	code.mJson<<"ddGroup:\""<<gic_ddGroup<<"\",";
	code.mJson<<"tre_objtpl:\""<<tre_objtpl<<"\",";
	code.mJson<<"tre_objidbd:\""<<tre_objidbd<<"\",";
	code.mJson<<"enableD:"<<gic_enableD<<",";
	code.mJson<<"enableCheckbox:"<<gic_enableCheckbox<<",";
	code.mJson<<"gic_enableChildrenDrop:"<<gic_enableChildrenDrop<<",";
	code.mJson<<"gic_nodeOnlyCopy:"<<gic_nodeOnlyCopy<<",";
	code.mJson<<"autoScroll :true,";//记录较多时使用滚动条
	code.mJson<<"hidden:"<<gic_hidden<<",";
	code.mJson<<"title:\""<<gic_title<<"\",";
	code.mJson<<"lbbd:\""<<gic_lbbd<<"\",";
	code.mJson<<"autoWidth:"<<gic_autoWidth<<",";
	code.mJson<<"autoHeight:"<<gic_autoHeight<<",";
	//code.mJson<<"loader:new Aos.XmlTreeLoader(),";
	code.mJson<<"root:\""<<gic_rootText<<"\"";
		//root  config end
	return true;
}

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
// vpd:
// <treeview gic_type="gic_html_treeview" gic_lines="true|false" ...>
//  <csslist>
//  	<css className="xxxx" classValue="xxxx"></css>
//  	<css className="xxxx" classValue="xxxx"></css>
//  	...
//  </csslist>
// 	<nodes>
// 		<node text="xxxx" expanded="true|false" leaf="true|false" iconCls="xxxx">
// 			<node text="xxxx" expanded="true|false" leaf="true|false" iconCls="xxxx"/>
// 			<node text="xxxx" expanded="true|false" leaf="true|false" iconCls="xxxx"/>
// 			<node text="xxxx" expanded="true|false" leaf="true|false" iconCls="xxxx"/>
// 			<node text="xxxx" expanded="true|false" leaf="true|false" iconCls="xxxx"/>
// 		</node>
// 		<node text="xxxx" expanded="true|false" leaf="true|false" iconCls="xxxx"/>
// 		<node text="xxxx" expanded="true|false" leaf="true|false" iconCls="xxxx"/>
// 		<node text="xxxx" expanded="true|false" leaf="true|false" iconCls="xxxx"/>
// 	</nodes>
// </treeview>
//   
//
// Modification History:
// 10/01/2011: Created by Jozhi Peng
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicHtmlTreeView.h"

#include "DbQuery/Query.h"
#include "HtmlModules/DclDb.h"
#include "HtmlUtil/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

const int sgContainerMemberPsize = 200;

AosGicHtmlTreeView::AosGicHtmlTreeView(const bool flag)
:
AosGic(AOSGIC_HTMLTREEVIEW, AosGicType::eHtmlTreeView, flag),
mRootVisible(false),
mLines(false),
mTpls(0),
mDefaultSrc(""),
mTreeType("")
{
	mDefaultSrc = "http://eeclouds.com/js/Ext/extjs/resources/images/default/s.gif";
}


AosGicHtmlTreeView::~AosGicHtmlTreeView()
{
}


bool	
AosGicHtmlTreeView::generateCode(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		const OmnString &parentid,
		AosHtmlCode &code)
{
	mClearNodeOverCls = vpd->getAttrBool("gic_clearNodeOverCls", false);
	mClearNodeSelectCls = vpd->getAttrBool("gic_clearNodeOverCls", false);
	mLines = vpd->getAttrBool("gic_lines", false);
	mRootVisible = vpd->getAttrBool("gic_rootVisible", false);
	mTreeType = vpd->getAttrStr("gic_treetype", "static");
	mNodeOverCls = vpd->getAttrStr("gic_node_overCls", "");
	mLeafOverCls = vpd->getAttrStr("gic_leaf_overCls", "");
	mNodeSelectCls = vpd->getAttrStr("gic_node_selectCls", "");
	mLeafSelectCls = vpd->getAttrStr("gic_leaf_selectCls", "");
	code.mJson << ",gic_lines: " <<	mLines 
			   << ",gic_clearNodeOverCls: " << mClearNodeOverCls
			   << ",gic_clearNodeSelectCls: " << mClearNodeSelectCls
			   << ",gic_rootVisible: " << mRootVisible
			   << ",gic_node_overCls:\"" << mNodeOverCls << "\""
			   << ",gic_leaf_overCls:\"" << mLeafOverCls << "\""
			   << ",gic_node_selectCls:\"" << mNodeSelectCls << "\""
			   << ",gic_leaf_selectCls:\"" << mLeafSelectCls << "\""
			   << ",gic_treetype: \"" << mTreeType << "\"";

	OmnString line_class= AOSCSS_ROOT;
	if (mLines)
	{
		line_class<< " " << AOSCSS_LINE;
	}
	else
	{
		line_class<< " " << AOSCSS_NO_LINE;
	}
	OmnString htmlcode;
	htmlcode << "<ul class=\"" << line_class << "\">";
	if (!mRootVisible)
	{
		htmlcode << "<div class=\"" << AOSCSS_ROOT_NODE << "\">";
	}

	mTpls = vpd->getFirstChild("nodes");
	aos_assert_r(mTpls, false);

	if (mTreeType ==  "static")
	{
		constructHtmlStatic(htmlcode, true, true, mTpls, "");
	}
	else
	{
		AosDataColPtr dcl = AosDataCol::getInstance(vpd);
		if (dcl)
		{
			AosXmlTagPtr doc = dcl->retrieveData(htmlPtr, vpd, obj, "");
			if (doc)
			{
				AosXmlTagPtr datas = doc->getFirstChild("Contents");
				if (datas)
				{
					code.mJson << ",gic_data: '" << datas->toString() << "'";
					if (mTreeType == "bytpl")
					{
						constructHtmlByTpl(htmlcode, true, true, mTpls, datas, "");
					}
					if (mTreeType == "byfilemgr")
					{
						constructHtmlByFileMgr(htmlcode, true, true, datas, "");
					}
					if (mTreeType == "byobj")
					{
						constructHtmlByObj(htmlcode, true, true, datas, "");
					}
				}
			}
		}
	}

	if (!mRootVisible)
	{
		htmlcode << "</div></ul>";
	}
	else
	{
		htmlcode << "</ul></li>";
	}
	code.mHtml << htmlcode;
	return true;
}


bool
AosGicHtmlTreeView:: constructHtmlStr(
		OmnString &htmlcode,
		const Node &node)
{
	//Node: 
	//		isRoot
	//		end
	//		leaf
	//		expanded
	//		text
	//		icon
	//		iconCls
	//		elNodeCls
	//		elLeafCls
	//		indentNode
	//
	if (node.isRoot && !mRootVisible)
	{
		return true;
	}

	OmnString ctNode_style;
	OmnString ecNode_class;
	OmnString iconNode_class;
	OmnString elNode_class;

	if (!node.leaf)
	{
		if (node.expanded)
		{
			elNode_class << AOSCSS_ELNODE_EXPANDED;
			if (!node.end)
			{
				ecNode_class << AOSCSS_ECNODE_MINUS;
			}
			else
			{
				ecNode_class << AOSCSS_ECNODE_END_MINUS;
			}
			ctNode_style << AOSSTYLE_CTNODE_DISPLAY_BLOCK;
		}
		else
		{
			elNode_class << AOSCSS_ELNODE_COLLAPSED;
			if (!node.end)
			{
				ecNode_class << AOSCSS_ECNODE_PLUS;
			}
			else
			{
				ecNode_class << AOSCSS_ECNODE_END_PLUS;
			}
			ctNode_style << AOSSTYLE_CTNODE_DISPLAY_NONE;
		}
		if (node.elNodeCls != "")
		{
			elNode_class << " " << node.elNodeCls;
		}
	}
	else
	{
		if (!node.end)
		{
			ecNode_class << AOSCSS_ECNODE_LEAF;
		}
		else
		{
			ecNode_class << AOSCSS_ECNODE_END_LEAF;
		}

		elNode_class << AOSCSS_ELNODE_LEAF;
		ctNode_style << AOSSTYLE_CTNODE_DISPLAY_NONE;
		if (node.elLeafCls != "")
		{
			elNode_class << " " << node.elLeafCls;
		}
	}

	iconNode_class << AOSCSS_ICONNODE;
	if (node.iconCls != "")
	{
		iconNode_class << " " << node.iconCls;
	}

	htmlcode << "<li class=\"" << AOSCSS_LINODE << "\">"
			 << "<div class=\"" << elNode_class << "\">"
			 << "<span class=\"" << AOSCSS_INDENTNODE << "\">"
			 << node.indentNode
			 << "</span>"
			 << "<img class=\"" << ecNode_class << "\" src=\"" << mDefaultSrc << "\">"
			 << "<img class=\"" << iconNode_class << "\" src=\"" << node.icon << "\">"
			 << "<a class=\"" << AOSCSS_TEXTNODE << "\">"
			 << "<span>" << node.text << "</span>"
			 << "</a>"
			 << "</div>"
			 << "<ul class=\"" << AOSCSS_CTNODE "\" style=\"" << ctNode_style << "\">";
	return true;
}


/*
bool
AosGicHtmlTreeView:: constructHtmlStr(
		OmnString &htmlcode,
		bool const &isRoot,
		bool const &end,
		bool const &leaf,
		bool const &expanded,
		const OmnString &text,
		const OmnString &icon,
		const OmnString &iconCls,
		const OmnString &indentNode)
{
	if (isRoot && !mRootVisible)
	{
		return true;
	}
	OmnString ctNode_style = "";
	OmnString ecNode_class = "x-tree-ec-icon";
	OmnString iconNode_class= "x-tree-node-icon";
	OmnString elNode_class = "x-tree-node-el x-unselectable";

	if (iconCls != "")
	{
		iconNode_class << " " << iconCls;
	}
	if (!leaf)
	{
		if (expanded)
		{
			elNode_class << " " << "x-tree-node-expanded";
			if (!end)
			{
				ecNode_class << " " << "x-tree-elbow-minus";
			}
			else
			{
				ecNode_class << " " << "x-tree-elbow-end-minus";
			}
			ctNode_style << "display:block;";
		}
		else
		{
			elNode_class << " " << "x-tree-node-collapsed";
			if (!end)
			{
				ecNode_class << " " << "x-tree-elbow-plus";
			}
			else
			{
				ecNode_class << " " << "x-tree-elbow-end-plus";
			}
			ctNode_style << "display:none;";
		}
	}
	else
	{
		if (!end)
		{
			ecNode_class << " " << "x-tree-elbow";
		}
		else
		{
			ecNode_class << " " << "x-tree-elbow-end";
		}
		elNode_class << " " << "x-tree-node-leaf file";
		ctNode_style << "display:none;";
	}

	htmlcode << "<li class=\"x-tree-node\">"
			 << "<div class=\"" << elNode_class << "\">"
			 << "<span class=\"x-tree-node-indent\">"
			 << indentNode
			 << "</span>"
			 << "<img class=\"" << ecNode_class << "\" src=\"" << mDefaultSrc << "\">"
			 << "<img class=\"" << iconNode_class << "\" src=\"" << icon << "\">"
			 << "<a class=\"x-tree-node-anchor\">"
			 << "<span>" << text << "</span>"
			 << "</a>"
			 << "</div>"
			 << "<ul class=\"x-tree-node-ct\" style=\"" << ctNode_style << "\">";
	return true;
}
*/

bool 
AosGicHtmlTreeView:: constructHtmlStatic(
		OmnString &htmlcode,
		const bool &_isRoot,
		const bool &_end,
		const AosXmlTagPtr &_tpls,
		const OmnString &_indentNode)
{
	aos_assert_r(_tpls, false);

	Node node;
	node.isRoot = _isRoot;
	node.end = _end;
	node.leaf = _tpls->getAttrBool("leaf", false);
	node.expanded = _tpls->getAttrBool("expanded", false);
	node.text = _tpls->getAttrStr("text", "newnode");
	node.icon = _tpls->getAttrStr("icon", mDefaultSrc);
	node.iconCls = _tpls->getAttrStr("iconCls", "");
	node.indentNode = _indentNode;
	node.elNodeCls = _tpls->getAttrStr("elNodeCls", "");
	node.elLeafCls = _tpls->getAttrStr("elLeafCls", "");

	constructHtmlStr(htmlcode, node);

	if (!node.leaf)
	{
		OmnString child_indentNode;
		OmnString indentClass;
		if (node.end)
		{
			indentClass << AOSCSS_TREE_ICON;
		}
		else
		{
			indentClass << AOSCSS_TREE_LINE;
		}
		if (node.isRoot && !mRootVisible)
		{
			child_indentNode = "";
		}
		else
		{
			child_indentNode << node.indentNode << "<img class=\"" << indentClass << "\" src=\"" << mDefaultSrc << "\">";
		}
		bool child_end = false;
		int tpls_len = _tpls->getNumSubtags();
		for(int i=0; i<tpls_len; i++)
		{
			if (i == tpls_len-1)
			{
				child_end = true;
			}
			AosXmlTagPtr tpls = _tpls->getChild(i);
			constructHtmlStatic(htmlcode, false, child_end, tpls, child_indentNode);
		}
	}
	htmlcode << "</ul>";
	htmlcode << "</li>";
	return true;
}
bool 
AosGicHtmlTreeView:: constructHtmlByTpl(
		OmnString &htmlcode,
		const bool &_isRoot,
		const bool &_end,
		const AosXmlTagPtr &_tpl,
		const AosXmlTagPtr &_obj,
		const OmnString &_indentNode)
{
	aos_assert_r(_tpl && _obj, false);
	OmnString text = "";
	OmnString iconCls = "";
	OmnString icon = "";
	bool expanded = false;
	bool leaf = false;

	text = _tpl->getAttrStr("text", "newnode");
	iconCls = _tpl->getAttrStr("iconCls", "");
	expanded = _tpl->getAttrBool("expanded", false);
	leaf = _tpl->getAttrBool("leaf", false);
	icon = _tpl->getAttrStr("icon", mDefaultSrc);

	OmnString textbd = _tpl->getAttrStr("textbd", "");
	OmnString iconClsbd = _tpl->getAttrStr("iconClsbd", "");
	OmnString expandedbd = _tpl->getAttrStr("expandedbd", "");
	OmnString leafbd = _tpl->getAttrStr("leafbd", "");
	OmnString iconbd = _tpl->getAttrStr("iconbd", "");

	bool exist;
	text = _obj->xpathQuery(textbd, exist, text);
	iconCls = _obj->xpathQuery(iconClsbd, exist, iconCls);
	icon = _obj->xpathQuery(iconbd, exist, icon);

	OmnString ee = expanded?"true":"false";
	expanded = (_obj->xpathQuery(expandedbd, exist, ee))=="true";
	OmnString ll = leaf?"true":"false";
	leaf = (_obj->xpathQuery(leafbd, exist, ll))=="true";

	Node node;
	node.isRoot = _isRoot;
	node.end = _end;
	node.leaf = leaf;
	node.expanded = expanded;
	node.text = text;
	node.icon = icon;
	node.iconCls = iconCls;
	node.indentNode = _indentNode;
	node.elNodeCls = _tpl->getAttrStr("elNodeCls", "");
	node.elLeafCls = _tpl->getAttrStr("elLeafCls", "");

	constructHtmlStr(htmlcode, node);

	if (!node.leaf)
	{
		OmnString child_indentNode;
		OmnString indentClass;
		if (node.end)
		{
			indentClass << AOSCSS_TREE_ICON;
		}
		else
		{
			indentClass << AOSCSS_TREE_LINE;
		}

		if (node.isRoot && !mRootVisible)
		{
			child_indentNode = "";
		}
		else
		{
			child_indentNode << node.indentNode << "<img class=\"" << indentClass << "\" src=\"" << mDefaultSrc << "\">";
		}

		bool child_end = false;
		OmnString childbindtype = _tpl->getAttrStr("childbdtype", "tagname");
		int obj_len = _obj->getNumSubtags();
		for(int i=0; i<obj_len; i++)
		{
			if (i == obj_len-1)
			{
				child_end = true;
			}
			AosXmlTagPtr obj = _obj->getChild(i);
			AosXmlTagPtr tpl;
			if (childbindtype == "attribute")
			{
				OmnString attrname = _tpl->getAttrStr("attrname", "");
				tpl = _tpl->getChildByAttr("tagbd", attrname);
			}
			else if (childbindtype == "tagname")
			{
				tpl = _tpl->getChildByAttr("tagbd", obj->getTagname());
			}
			if (tpl)
			{
				constructHtmlByTpl(htmlcode, false, child_end, tpl, obj, child_indentNode);
			}
		}
	}
	htmlcode << "</ul>";
	htmlcode << "</li>";
	return true;
}


bool
AosGicHtmlTreeView:: constructHtmlByObj(
		OmnString &htmlcode,
		const bool &_isRoot,
		const bool &_end,
		const AosXmlTagPtr &_obj,
		const OmnString &_indentNode)
{
	aos_assert_r(_obj, false);
	OmnString text = "";
	OmnString iconCls = "";
	OmnString icon = mDefaultSrc;
	bool expanded = false;
	bool leaf = false;
	text = _obj->getTagname();
	iconCls = "";
	expanded = false;
	int obj_len = _obj->getNumSubtags();
	leaf = obj_len==0?true:false;



	Node node;
	node.isRoot = _isRoot;
	node.end = _end;
	node.leaf = leaf;
	node.expanded = expanded;
	node.text = text;
	node.icon = icon;
	node.iconCls = iconCls;
	node.indentNode = _indentNode;
	node.elNodeCls = "";
	node.elLeafCls = "";

	constructHtmlStr(htmlcode, node);

	if (!node.leaf)
	{
		OmnString child_indentNode;
		OmnString indentClass;
		if (node.end)
		{
			indentClass << AOSCSS_TREE_ICON;
		}
		else
		{
			indentClass << AOSCSS_TREE_LINE;
		}

		if (node.isRoot && !mRootVisible)
		{
			child_indentNode = "";
		}
		else
		{
			child_indentNode << node.indentNode << "<img class=\"" << indentClass << "\" src=\"" << mDefaultSrc << "\">";
		}
		bool child_end = false;
		for(int i=0; i<obj_len; i++)
		{
			if (i == obj_len-1)
			{
				child_end = true;
			}
			AosXmlTagPtr obj = _obj->getChild(i);
			constructHtmlByObj(htmlcode, false, child_end, obj, child_indentNode);
		}
	}
	htmlcode << "</ul>";
	htmlcode << "</li>";
	return true;
}

bool
AosGicHtmlTreeView:: constructHtmlByFileMgr(
		OmnString &htmlcode,
		const bool &_isRoot,
		const bool &_end,
		const AosXmlTagPtr &_obj,
		const OmnString &_indentNode)
{
	aos_assert_r(_obj && mTpls, false);
	OmnString text = "";
	OmnString iconCls = "";
	OmnString icon = mDefaultSrc;
	bool expanded = false;
	bool leaf = false;

	text = mTpls->getAttrStr("text", "newnode");
	iconCls = mTpls->getAttrStr("iconCls", "");
	leaf = mTpls->getAttrBool("leaf", false);
	icon = mTpls->getAttrStr("icon", mDefaultSrc);

	OmnString textbd = mTpls->getAttrStr("textbd", "");
	OmnString iconClsbd = mTpls->getAttrStr("iconClsbd", "");
	OmnString leafbd = mTpls->getAttrStr("leafbd", "");
	OmnString iconbd = mTpls->getAttrStr("iconbd", "");

	bool exist;
	text = _obj->xpathQuery(textbd, exist, text);
	iconCls = _obj->xpathQuery(iconClsbd, exist, iconCls);
	icon = _obj->xpathQuery(iconbd, exist, icon);
	OmnString zky_otype = _obj->xpathQuery(leafbd, exist, "");
	leaf = (zky_otype=="zky_ctnr" || _isRoot)?false:true;

	Node node;
	node.isRoot = _isRoot;
	node.end = _end;
	node.leaf = leaf;
	node.expanded = expanded;
	node.text = text;
	node.icon = icon;
	node.iconCls = iconCls;
	node.indentNode = _indentNode;
	node.elNodeCls = "";
	node.elLeafCls = "";

	constructHtmlStr(htmlcode, node);
	if (!node.leaf && node.isRoot)
	{
		OmnString child_indentNode;
		OmnString indentClass;
		if (node.end)
		{
			indentClass << AOSCSS_TREE_ICON;
		}
		else
		{
			indentClass << AOSCSS_TREE_LINE;
		}
		child_indentNode << node.indentNode << "<img class=\"" << indentClass << "\" src=\"" << mDefaultSrc << "\">";
		bool child_end = false;
		int obj_len = _obj->getNumSubtags();
		for(int i=0; i<obj_len; i++)
		{
			if (i == obj_len-1)
			{
				child_end = true;
			}
			AosXmlTagPtr obj = _obj->getChild(i);
			constructHtmlByFileMgr(htmlcode, false, child_end, obj, child_indentNode);
		}
	}
	htmlcode << "</ul>";
	htmlcode << "</li>";
	return true;
}

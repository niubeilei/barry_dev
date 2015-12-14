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
// 10/01/2011: Created by Jozhi Peng
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_GICs_GicHtmlTreeView_h
#define Aos_GICs_GicHtmlTreeView_h

#include "GICs/GIC.h"
#include "SEUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "HtmlUtil/HtmlUtil.h"

#define AOSCSS_ROOT						"x-tree-root-ct"

#define AOSCSS_LINE						"x-tree-lines"

#define AOSCSS_NO_LINE					"x-tree-no-lines"

#define AOSCSS_ROOT_NODE				"x-tree-root-node"

#define AOSCSS_ECNODE_END_MINUS			"x-tree-ec-icon x-tree-elbow-end-minus"
#define AOSCSS_ECNODE_MINUS				"x-tree-ec-icon x-tree-elbow-minus"

#define AOSCSS_ECNODE_END_PLUS			"x-tree-ec-icon x-tree-elbow-end-plus"
#define AOSCSS_ECNODE_PLUS				"x-tree-ec-icon x-tree-elbow-plus"

#define AOSCSS_ECNODE_END_LEAF			"x-tree-ec-icon x-tree-elbow-end"
#define AOSCSS_ECNODE_LEAF				"x-tree-ec-icon x-tree-elbow"

#define AOSCSS_ELNODE_EXPANDED			"x-tree-node-el x-unselectable x-tree-node-expanded"
#define AOSCSS_ELNODE_COLLAPSED			"x-tree-node-el x-unselectable x-tree-node-collapsed"

#define AOSCSS_ELNODE_LEAF				"x-tree-node-el x-unselectable x-tree-node-leaf file"

#define AOSCSS_ICONNODE					"x-tree-node-icon"

#define AOSCSS_INDENTNODE				"x-tree-node-indent"

#define AOSCSS_TEXTNODE					"x-tree-node-anchor"

#define AOSCSS_LINODE					"x-tree-node"

#define AOSCSS_CTNODE					"x-tree-node-ct"

#define AOSCSS_TREE_ICON				"x-tree-icon"

#define AOSCSS_TREE_LINE				"x-tree-elbow-line"

#define AOSSTYLE_CTNODE_DISPLAY_NONE	"display:none;"
#define AOSSTYLE_CTNODE_DISPLAY_BLOCK	"display:block;"

class AosGicHtmlTreeView : public AosGic
{
private:
	bool			mClearNodeOverCls;
	bool			mClearNodeSelectCls;
	bool			mRootVisible;
	bool 			mLines;
	AosXmlTagPtr 	mTpls;
	OmnString 		mDefaultSrc;
	OmnString 		mTreeType;
	OmnString		mNodeOverCls;
	OmnString		mLeafOverCls;
	OmnString		mLeafSelectCls;
	OmnString		mNodeSelectCls;
	
	struct Node
	{
		bool 			isRoot;
		bool 			end;
		bool 			leaf;
		bool 			expanded;
		OmnString 		text;
		OmnString		icon;
		OmnString		iconCls;
		OmnString		elNodeCls;
		OmnString		elLeafCls;
		OmnString		indentNode;
	};
public:
	AosGicHtmlTreeView(const bool flag);
	~AosGicHtmlTreeView();

	virtual bool	
	generateCode(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd, 
		const AosXmlTagPtr &obj, 
		const OmnString &parentid, 
		AosHtmlCode &code);
private:
	bool
	constructHtmlStr(
		OmnString &htmlcode,
		const Node &node);

	bool
	constructHtmlStr(
		OmnString &htmlcode,
		bool const &isRoot,
		bool const &end,
		bool const &leaf,
		bool const &expanded,
		const OmnString &text,
		const OmnString &icon,
		const OmnString &iconCls,
		const OmnString &indentNode);

	bool 
	constructHtmlStatic(
		OmnString &htmlcode,
		const bool &_isRoot,
		const bool &_end,
		const AosXmlTagPtr &_tpl,
		const OmnString &_indentNode);

	bool 
	constructHtmlByTpl(
		OmnString &htmlcode,
		const bool &_isRoot,
		const bool &_end,
		const AosXmlTagPtr &_tpl,
		const AosXmlTagPtr &_obj,
		const OmnString &_indentNode);

	bool
	constructHtmlByObj(
		OmnString &htmlcode,
		const bool &_isRoot,
		const bool &_end,
		const AosXmlTagPtr &_obj,
		const OmnString &_indentNode);

	bool
	constructHtmlByFileMgr(
		OmnString &htmlcode,
		const bool &_isRoot,
		const bool &_end,
		const AosXmlTagPtr &_obj,
		const OmnString &_indentNode);
};

#endif


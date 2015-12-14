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
// This tree assumes the following HTML structure:
// 1. The root tag is a 'ul' tag. Its class name must be AOSCLASSNAME_JQUERY_TREE.
// 2. The next level tags are all '<li>' tag. Each defines an entry, either 
//    leaf or node entry. If it is a leaf entry, it should be:
//    	<li id="xxx"><span>name</span></li>
//    If it is node, it should be:
//    	<li id="xxx">
//    		<span>name</span>
//    		<ul>
//    			<li .../>	
//    			<li .../>	
//    			...
//    		</ul>
//    	</li>
//    Entries can be recursively defined.
//    			
// 	<ul id="tt-tree" class="easyui-tree" animate="true" dnd="true">
// 		<li id="1">
// 			<span>Folder</span>
// 			<ul>
// 				<li id="2" state="closed">
// 					<span>Sub Folder 1</span>
// 					<ul>
// 						<li id="3">
// 							<span><a href="#">File 11</a></span>
// 						</li>
// 						<li id="4">
// 							<span>File 12</span>
// 						</li>
// 						<li id="5">
// 							<span>File 13</span>
// 						</li>
// 					</ul>
// 				</li>
// 				<li id="6">
// 					<span>File 2</span>
// 				</li>
// 				<li id="7">
// 					<span>File 3</span>
// 				</li>
// 				<li id="8">File 4</li>
// 				<li id="9">File 5</li>
// 			</ul>
// 		</li>
// 		<li id="10">
// 			<span>File21</span>
// 			<ul>
// 				<li id="11">
// 					<span>teszt</span>
// 				</li>
// 			</ul>
// 		</li>
// 	</ul>
//   
// Modification History:
// 07/04/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicTreeJquery.h"

#include "HtmlUtil/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"


AosGicTreeJquery::AosGicTreeJquery(const bool flag)
:
AosGic(AOSGIC_PROMPT, AosGicType::ePrompt, flag)
{
}


AosGicTreeJquery::~AosGicTreeJquery()
{
}


bool	
AosGicTreeJquery::generateCode(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		const OmnString &parentId,
		AosHtmlCode &code)
{
	if (vpd->getAttrBool(AOSTAG_EMBEDDED, false))
	{
		createEmbeddedTree(htmlPtr, vpd, obj, parentId, code);
	}

	OmnNotImplementedYet;
	return false;
}


bool
AosGicTreeJquery::createEmbeddedTree(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		const OmnString &parentId,
		AosHtmlCode &code)
{
	// This means the tree is embedded in the VPD itself. 
	vpd->setAttr("class", AOSGIC_CLASSNAME_JQUERY_TREE);

	OmnString valbd = vpd->getAttrStr("pmt_query");
	OmnString gic_value;
	if (valbd != "" && obj)
	{
		gic_value = obj->getAttrStr(valbd, vpd->getAttrStr("pmt_text"));
	}
	else
	{
		gic_value = vpd->getAttrStr("pmt_text");
	}
	OmnString div_id = AosGetHtmlElemId();
	OmnString label_id = AosGetHtmlElemId();
	code.mHtml << "<div id=\"" << label_id << "\">"	<< gic_value << "</div>";
	aos_assert_r(convertToJson(vpd, obj, code.mJson, div_id, label_id, gic_value), false);
	return true;
}


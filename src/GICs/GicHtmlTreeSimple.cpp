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
// This is an HTML implementation of simple tree. The HTML format is:
// 	<div ...>
// 	  <ul class="zkygic_00100">
//  	<li><a href="#">First link</a>
//    		<ul style="display:none">
//     			<li><a href="#">First link</a>
//      			<ul style="display:none">
//       				<li><a href="#">First link</a></li>
//        				<li><a href="#">Second link</a></li>
//         				<li><a href="#">Third link</a></li>
//          			<li><a href="#">Fourth link</a></li>
//           			<li><a href="#">Fifth link</a></li>
//            		</ul>
//             		<zkysimtree class="collapsed" id="xxx" 
//             			onclick="AosTreeNodeClicked(this.id)"/>
//              </li>
//              <li><a href="#">Second link</a></li>
//              <li><a href="#">Third link</a>
//              	<ul style="display:none">
//                  	<li><a href="#">First link</a></li>
//                   	<li><a href="#">Second link</a></li>
//                    	<li><a href="#">Third link</a></li>
//                     	<li><a href="#">Fourth link</a></li>
//                      <li><a href="#">Fifth link</a></li>
//                  </ul>
//                  <zkysimtree class="zkytree_clps" id="xxx" 
//                  	onclick="AosTreeNodeClicked(this.id)"/>
//               </li>
//               ...
//			</ul>
//          <zkysimtree class="zkytree_clps" id="xxx" 
//             	onclick="AosTreeNodeClicked(this.id)"/>
//		</li>
//	  </ul>
//  </div>
//
// Initially all <ul>'s display is set to "none", meaning it does not show.
// Only the top level <li> shows a link. The special tag <zkysimtree> is used to control
// tree displays. When its class is set to "zkytree_clps", it is shown as an icon. Otherwise,
// it is shown as another icon. 
// 	#zkytree_simple li zkysimtree, #zkytree_simple li zkysimtree.zkytree_clps{
// 			background:url(collapsed.gif) no-repeat 0 0;}
//
//  #zkytree_simple li zkysimtree.zkytree_expand{background:url(expanded.gif) no-repeat 0 0;}
//
// The callback function AosTreeNodeClicked(...) is used to toggle between zkytree_clps
// and zkytree_expand.
//
// The simple tree shows only one text through <a> tag. 
//
// Modification History:
// 08/20/2011: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicHtmlTreeSimple.h"

#include <iostream>
#include "Alarm/Alarm.h"
#include "Alarm/HtmlError.h"
#include "ErrorMgr/ErrmsgId.h"
#include "HtmlUtil/HtmlClassNames.h"
#include "HtmlModules/DataCol.h"
#include "HtmlModules/DclDb.h"
#include "HtmlModules/DclStr.h"
#include "Util/StrParser.h"
//#include "ValueSel/ValueSelector.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"

// Static Member Data
OmnString AosGicHtmlTreeSimple::smEmptyTreeNotes = "Tree Is Empty";

const OmnString sgNoLabel = "No Label";

const OmnString sgNodeMethod1 = "method1";
const OmnString sgNodeMethod2 = "method2";
const OmnString sgDefaultNodeMethod = sgNodeMethod1;

// Empty Tree Policy
const OmnString sgEmptyTreePolicy_AddNote = "addnote";
const OmnString sgEmptyTreePolicy_EmbedTree = "embedtree";
const OmnString sgDefaultEmptyTreePolicy = sgEmptyTreePolicy_AddNote;

AosGicHtmlTreeSimple::AosGicHtmlTreeSimple(const bool regflag)
:
AosGic(AOSGIC_HTMLSIMPLETREE, AosGicType::eHtmlTreeSimple, regflag)
{
}


AosGicHtmlTreeSimple::~AosGicHtmlTreeSimple()
{
}


bool	
AosGicHtmlTreeSimple::generateCode(
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
	AosRundataPtr rdata = htmlPtr->getRundata();
	generateJsCode(htmlPtr, vpd, obj, parentid, code, rdata);
	generateCssCode(htmlPtr, vpd, obj, parentid, code, rdata);
	generateHtmlCode(htmlPtr, vpd, obj, parentid, code, rdata);
	generateJsonCode(htmlPtr, vpd, obj, parentid, code, rdata);
	return true;
}


bool
AosGicHtmlTreeSimple::generateJsCode(
		const AosHtmlReqProcPtr &htmlPtr,
		const AosXmlTagPtr &vpd, 
		const AosXmlTagPtr &obj,
		const OmnString &parentid,
		AosHtmlCode &code, 
		const AosRundataPtr &rdata)
{
	return false;
}


bool
AosGicHtmlTreeSimple::generateCssCode(
		const AosHtmlReqProcPtr &htmlPtr,
		const AosXmlTagPtr &vpd, 
		const AosXmlTagPtr &obj,
		const OmnString &parentid,
		AosHtmlCode &code, 
		const AosRundataPtr &rdata)
{
	// The Tree CSS code consists of two parts: the main part that normally
	// do not change at all; and GIC specific part that may be changed 
	// based on GIC configurations.
	//
	// In the current implementations, we do not need to generate CSS code.
	return true;
}


bool
AosGicHtmlTreeSimple::generateHtmlCode(
		const AosHtmlReqProcPtr &htmlPtr,
		const AosXmlTagPtr &vpd, 
		const AosXmlTagPtr &obj,
		const OmnString &parentid,
		AosHtmlCode &code, 
		const AosRundataPtr &rdata)
{
	OmnString htmlcode;
	AosDataColPtr dcl = AosDataCol::getInstance(vpd);
	AosXmlTagPtr root;
	AosXmlTagPtr doc_cont;
	AosXmlTagPtr record;
	if (dcl)
	{
		root = dcl->retrieveData(htmlPtr, vpd, obj, "");
		if (!root || !(doc_cont = root->getFirstChild("Contents")))
		{
			AosSetError(rdata, AosErrmsgId::eFailedRetrieveData);
			OmnAlarm << "Failed to retrieve the doc definition: " 
				<< vpd->toString() << enderr;
			OmnString htmlcode = AosHtmlAlarm(rdata->getErrmsg());
			code.mHtml << htmlcode;
			return false;
		}
	
		record = doc_cont->getFirstChild();
	}

	if (!record)
	{
		// This means it is an empty tree. Need to generate empty tree
		// Check whether it should use an embedded tree
		OmnString method = vpd->getAttrStr(AOSTAG_EMPTYTREE_POLICY, sgDefaultEmptyTreePolicy);
		if (method == sgEmptyTreePolicy_EmbedTree)
		{
			// This means that the VPD defines a default tree 
			// The VPD should be in the form:
			// 	<vpd ...>
			// 		...
			// 		<AOSTAG_DEFAULT_TREE>
			// 			<entry .../>
			// 			<entry .../>
			// 			...
			// 		</AOSTAG_DEFAULT_TREE>
			// 	</vpd>
			AosXmlTagPtr rr = vpd->getFirstChild(AOSTAG_DEFAULT_TREE);
			if (rr)
			{
				doc_cont = rr;
				record = doc_cont->getFirstChild();
			}
		}
	}

	if (!record)
	{
		// It is still an empty tree. 
		return generateEmptyTreeCode(htmlPtr, vpd, obj, parentid, code, rdata);
	}
	
	// The tree is not empty. Ready to create it.
	//AosValueSelector label_selector;
	htmlcode = "<ul class=\"";
	htmlcode << AOSHTMLCLASSNAME_HTMLTREE_SIMPLE << "\" "
		<< AOSTAG_CODE_LOADED << "=\"false\">";

	// Retrieve the label value selector
	AosXmlTagPtr label_value_tag = vpd->getFirstChild(AOSTAG_LABEL_SELECTOR);
	if (label_value_tag)
	{
		//label_selector.setConfig(label_value_tag, rdata);
	}
	else
	{
		// There is no label selector. The label is the value of one of the
		// attributes.
		OmnString aname = vpd->getAttrStr(AOSTAG_LABEL_ANAME);
		if (aname == "")
		{
			OmnAlarm << "Missing label specification!";
			//label_selector.setSelectByConstant(sgNoLabel);
		}
		else
		{
			//label_selector.setSelectByAttr(aname);
		}
	}

	// The format is
	// 	<div ...>									// Already created
	// 	  	<ul class="zkygic_00100">
	//  		<li><a href="#">xxx</a>
	//    			<ul style="display:none">		// If it has children
	//     		<li><a href="#">xxx</a>				// If it does not have children
	//     		...
	//          <zkysimtree class="zkytree_clps" id="xxx" 
	//             	onclick="AosTreeNodeClicked(this.id)"/>
	//     	</ul>
	//  </div>
	while (record)
	{
		//htmlcode << "<li><a href=\"#\">"
		//	<< label_selector.getValue(record, rdata) << "</a>";

		// Check whether it is a node or leaf
		if (isNode(record, vpd, obj, rdata))
		{
			// Need to create the <ul> tag.
			htmlcode << "<ul style=\"display:none\">"
				<< "</ul>";
		}
		htmlcode << "</li>";

		record = doc_cont->getNextChild();
	}

	// Create the control tag:
	// 	<zkysimtree class="zkytree_clps" id="xxx"
	// 		onclick="AosTreeNodeClicked(this.id)"/>
	htmlcode << "<zkysimtree class=\"zkytree_clps\" id=\""
		<< htmlPtr->getInstanceId() << "\" onclick=\"AosTreeNodeClicked(this.id)\"/>";

	htmlcode << "</ul>";
	code.mHtml << htmlcode;
	return true;
}


bool
AosGicHtmlTreeSimple::generateJsonCode(
		const AosHtmlReqProcPtr &htmlPtr,
		const AosXmlTagPtr &vpd, 
		const AosXmlTagPtr &obj,
		const OmnString &parentid,
		AosHtmlCode &code, 
		const AosRundataPtr &rdata)
{
	// Json code is used to create the JavaScript object. It should be in the form:
	// 		name:value,
	// 		name:value,
	// 		...
	// This class will generate the following Json code:
	// 		empty
	
	// OmnString str=",";
	// str	<< "gic_border:\"" << gic_border << "\"," 
	// 	<< "gic_padding:\"" << gic_padding << "\","
	// 	<< "gic_alert:\"" << gic_alert << "\","
	// 	<< "gic_bordcolor:\"" << gic_bordcolor << "\"," 
	// 	<< "showPaging:" << showPaging;
	// code.mJson << str;
	return true;
}


bool
AosGicHtmlTreeSimple::isNode(
		const AosXmlTagPtr &record, 
		const AosXmlTagPtr &vpd, 
		const AosXmlTagPtr &obj, 
		const AosRundataPtr &rdata)
{
	// This function determines whether the 'record' is a node or leaf. 
	// In the current implementations, it supports the following methods.
	// 1. Method 1: 
	//    When an attribute's value equals to one of the specified values, it is a node.
	// 2. Method 2:
	//    When an attribute's value equals to one of the specified values, it is a leaf.
	OmnString method = vpd->getAttrStr(AOSTAG_DETERM_NODE_METHOD, sgDefaultNodeMethod);
	if (method == sgNodeMethod2)
	{
		// It is a leaf if the value of AOSTAG_NODE_ATTRNAME equals to one of
		// the values specified in AOSTAG_NODE_VALUES. 
		OmnString aname = vpd->getAttrStr(AOSTAG_NODE_ATTRNAME);
		if (aname == "")
		{
			// It defaults to leaf
			return false;
		}
		OmnString value = vpd->getAttrStr(aname);
		OmnString values = vpd->getAttrStr(AOSTAG_NODE_VALUES);
		return !OmnStrParser1::containWord(values, value, ", ");
	}

	// This method (sgNodeMethod1) serves as the default.
	// It is a node if the value of AOSTAG_NODE_ATTRNAME equals to one of
	// the values specified in AOSTAG_NODE_VALUES. 
	OmnString aname = vpd->getAttrStr(AOSTAG_NODE_ATTRNAME);
	if (aname == "")
	{
		// It defaults to leaf
		return false;
	}
	OmnString value = vpd->getAttrStr(aname);
	OmnString values = vpd->getAttrStr(AOSTAG_NODE_VALUES);
	return OmnStrParser1::containWord(values, value, ", ");
}


bool 
AosGicHtmlTreeSimple::generateEmptyTreeCode(
		const AosHtmlReqProcPtr &htmlPtr,
		const AosXmlTagPtr &vpd, 
		const AosXmlTagPtr &obj,
		const OmnString &parentid,
		AosHtmlCode &code, 
		const AosRundataPtr &rdata)
{
	// There are several ways of generating empty tree code, which is
	// controlled by the vpd attribute AOSTAG_EMPTYTREE_POLICY. 
	OmnString method = vpd->getAttrStr(AOSTAG_EMPTYTREE_POLICY, sgDefaultEmptyTreePolicy);
	if (method == sgEmptyTreePolicy_EmbedTree)
	{
		// It should never happen since it has been checked. 
		AosSetError(rdata, AosErrmsgId::eInternalError);
 		code.mHtml << AosHtmlAlarm(rdata->getErrmsg());
		return true;
	}

	// It generates the following code:
	// 	<p class=AOSGICCLASSNAME_EMPTYTREE>No data yet</p>
	code.mHtml << "<p class=\""
		<< AOSHTMLCLASSNAME_EMPTYTREE << "\">"
		<< smEmptyTreeNotes
		<< "</p>";
	return true;
}


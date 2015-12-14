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
// 07/08/2010: Created by Cody
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicHtmlTabMenu.h"

#include "HtmlUtil/HtmlUtil.h"
#include "HtmlLayoutMgr/Layout.h"
#include "HtmlLayoutMgr/AllLayout.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "HtmlModules/DictionaryHandler.h"


AosGicHtmlTabMenu::AosGicHtmlTabMenu(const bool flag)
:
AosGic(AOSGIC_HTMLTABMENU, AosGicType::eHtmlTabMenu, flag)
{
}


AosGicHtmlTabMenu::~AosGicHtmlTabMenu()
{
}


bool	
AosGicHtmlTabMenu::generateCode(
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
	// 	5. Json code
	// 	
	// 	html:
	// 	<div id="div1" style="position">
	// 		<ul ...>
	// 			<li ...>
	// 			...
	// 		</ul>
	//  </div>
	//	JSOn:
	//	{
	//		el : 'gic_el',
	//		xtype : 'gic_button',
	//		x : gic_x,
	//		y : gic_y, 
	//		text : 'gic_value',
	//		width : gic_width,
	//		height : gic_height,
	//      paneid : paneid,
	//      panelid : panelid
	//	}
	//	code.mCss
	
	//OmnString cls_name, styleStr;
	//cls_name << AOSGIC_CLS_TABMENU << "-" << gic_eleid;

	//OmnString css;
	//css << "." << cls_name << "{" << styleStr << "}";
	
	OmnString gic_eleid = AosGetHtmlElemId();

	OmnString ulcssStr, ulc_name, css;
	//ulc_name << AOSGIC_CLS_TABMENU << "-" << gic_eleid;
	ulc_name << "" << "-" << gic_eleid;
	int ulbw = vpd->getAttrInt("gic_ulbw", -1);
	if (ulbw != -1) ulcssStr << "border-width:" << ulbw << "px;";

	OmnString ulbs = vpd->getAttrStr("gic_ulbs");
	if (ulbs != "") ulcssStr << "border-style:" << ulbs << ";";

	OmnString ulbc = AosHtmlUtil::getWebColor(vpd->getAttrStr("gic_ulbc"));
	if (ulbc != "") ulcssStr << "border-color:" << ulbc << ";";

	OmnString h3acssStr, h3a_name;
	h3a_name << ulc_name << " li h3 a";

	OmnString h3abc = AosHtmlUtil::getWebColor(vpd->getAttrStr("gic_headbgc"));
	if (h3abc != "") h3acssStr << "background:" << h3abc << ";";

	int h3abbw = vpd->getAttrInt("gic_h3abbw", -1);
	if (h3abbw != -1) h3acssStr << "border-bottm-width:" << h3abbw << "px;";
	
	OmnString h3abcl = AosHtmlUtil::getWebColor(vpd->getAttrStr("gic_h3abcl"));
	if (h3abcl != "") h3acssStr << "border-color:" << h3abcl << ";";
	
	int h3afsz = vpd->getAttrInt("gic_h3afsz", -1);
	if (h3afsz != -1) h3acssStr << "font-size:" << h3afsz << ";";

	OmnString h3afc = AosHtmlUtil::getWebColor(vpd->getAttrStr("gic_h3afc"));
	if (h3afc != "") h3acssStr << "color:" << h3afc << ";";
	
	OmnString h3afst = vpd->getAttrStr("gic_h3afst");
	if (h3afst != "") h3acssStr << "font-style:" << h3afst << ";";

	OmnString h3actcssStr, h3act_name;
	h3act_name << ulc_name << " li.active h3 a";
	
	OmnString h3actbc = AosHtmlUtil::getWebColor(vpd->getAttrStr("gic_h3actbc"));
	if (h3actbc != "") h3actcssStr << "background:" << h3actbc <<  ";";

	css << "." << ulc_name << "{" << ulcssStr << "}";
	css << "." << h3a_name << "{" << h3acssStr << "}";	
	css << "." << h3act_name << "{" << h3actcssStr << "}";
	
	
	
	OmnString html = "<ul class=\"";
	//html << AOSGIC_CLS_TABMENU << " " << ulc_name << "\">";
	html << "" << " " << ulc_name << "\">";

	AosXmlTagPtr creators = vpd->getFirstChild("creators");
	if (!creators) return false;
	AosXmlTagPtr column = creators->getFirstChild("column");
	bool toogle = vpd->getAttrBool("gic_toogle", true);
	int index = 1;
	OmnString vpdJson = "{}";
	OmnString fvpdname;
	while (column)
	{
		OmnString con="内容", dftname = "标题";
		dftname << index;
		OmnString hname = column->getAttrStr("zky_hname", dftname); 
		OmnString vpdname = column->getAttrStr("zky_vpdname", "");
		AosXmlTagPtr contents = column->getFirstChild("contents");
		if (index == 1 && vpdname != "")
		{
			fvpdname = vpdname;
			AosXmlTagPtr vpd, vpdroot, obj;
			AosHtmlCode vpdCode;
			bool status = htmlPtr->getVpd(vpdname,vpd, vpdroot);
			if(status)
			{
				OmnString vpdPath = htmlPtr->createVpdPath(vpdname);
				OmnString flag = htmlPtr->getInsertFlag();
				htmlPtr->appendVpdArray(vpd, obj, parentid, flag, vpdPath, 200, 200);

				AosLayout::createLayoutVpd(
						htmlPtr, vpd, obj, parentid, vpdCode, 200,200);//gic_bwidth, gic_bheight);
				contents = 0;
				con = vpdCode.mHtml;
				OmnString crtVpdNum = "";
				crtVpdNum << htmlPtr->getVpdArrayLength()-1;
				OmnString aosvpdStr = "aosvpd : aosvpd[";
				aosvpdStr << crtVpdNum << "],";
				vpdJson = vpdCode.mJson;
				vpdJson.insert(aosvpdStr,1);
			}
		}

		if (contents)
		{
			con = contents->getNodeText();
			int f1 = con.findSubString("|&&|",0);
			int f2 = con.findSubString("|&|",0);
			if(f1 != -1)
			{
				con.replace("|&&|","\"",true);
			}
			if(f2 != -1)
			{
				con.replace("|&|","\'",true);
			}
			if(!htmlPtr->isHomePage())
			{
				bool t = true;
				con.replace("\'", "\\'", t);
			}
		}

		html << "<li><h3>" << hname << "</h3><div class=\"panel\">" << con << "</div></li>";
		column = creators->getNextChild();
		index++;
	}
	html << "</ul>";
	/*OmnString html = "<ul class=\"";
	    html << AOSGIC_CLS_TABMENU << "\" id= \"test10001\">"
		    << "<li><h3>handle 1</h3><div class=\"panel\">date one</div></li>"
	   		<< "<li><h3>handle 2</h3><div class=\"panel\">date two</div></li>"
			<< "</ul>";*/
	index--;
	code.mHtml << html;
	code.mJson << ",gic_toogle:" << toogle
		       << ",vpdJson:" << vpdJson
		       << ",gic_columns:" << index
			   << ",gic_eleid:\"" << gic_eleid << "\""
			   << ",gic_fvpdname:\"" << fvpdname << "\"";
	code.mCss << css;

	return true;
}


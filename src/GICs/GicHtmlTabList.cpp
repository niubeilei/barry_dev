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
// 2011/02/19: Created by Ken
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicHtmlTabList.h"

#include "HtmlLayoutMgr/Layout.h"
#include "HtmlLayoutMgr/AllLayout.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/StrParser.h"
#include "HtmlModules/DclDb.h"

//static AosGicPtr sgGic = new AosGicHtmlTabList();

AosGicHtmlTabList::AosGicHtmlTabList(const bool flag)
:
AosGic(AOSGIC_HTMLTABLIST, AosGicType::eHtmlTabList, flag)
{
}


AosGicHtmlTabList::~AosGicHtmlTabList()
{
}


bool	
AosGicHtmlTabList::generateCode(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		const OmnString &parentid,
		AosHtmlCode &code)
{
	OmnString grid_css_random;
	grid_css_random << "_" << AosGetHtmlElemId();

    AosXmlTagPtr creators = vpd->getFirstChild("creators");
	if(!creators)
	{
		return false;
	}

	int gic_tabnums = 0;
	gic_tabnums = creators->getNumSubtags();
 
	OmnString gic_padding = vpd->getAttrStr("gic_padding","1");
	OmnString gic_height = vpd->getAttrStr("gic_height","");
	OmnString gic_width = vpd->getAttrStr("gic_width","");
	OmnString gic_expand_flag = vpd->getAttrStr("gic_expand_flag","false");

	OmnString gic_border = vpd->getAttrStr("gic_border","1");
	OmnString gic_bordcolor = vpd->getAttrStr("gic_bordcolor","#D0D0D0");
	OmnString head_style= vpd->getAttrStr("head_style","normal");
 	OmnString head_aln = vpd->getAttrStr("head_aln","center");
 	OmnString head_color = vpd->getAttrStr("head_color","black");
 	OmnString head_size = vpd->getAttrStr("head_size","15");
 	OmnString head_height = vpd->getAttrStr("head_height","20");
 	OmnString head_weight = vpd->getAttrStr("head_weight","normal");
 	OmnString gic_sel = vpd->getAttrStr("gic_sel","single");
 	OmnString gic_tip = vpd->getAttrStr("gic_tip","");
 	OmnString gic_bdbgcolor = vpd->getAttrStr("gic_bdbgcolor","#f2f2f2");
	OmnString gic_hdbgcolor = vpd->getAttrStr("gic_hdbgcolor","#D0D0D0");
	int gic_num = vpd->getAttrInt("gic_num",-1); 

	OmnString cssStr;
	cssStr << ".tablist" << grid_css_random << "{overflow:hidden;background:" << gic_bdbgcolor<<";height:"<< gic_height;
	//	   border:1px solid #D0D0D0;
	if(gic_sel!="single")
	{
		cssStr << ";overflow-y:auto";
	}
	if(gic_border != "")
	{
		cssStr << ";border:"<< gic_border <<"px solid";
	}
	if(gic_bordcolor != "")
	{
		cssStr << ";color:"<< gic_bordcolor ;
	}
	cssStr << ";}";
	cssStr << ".tablist_table" << grid_css_random << "{width:100%;table-layout:fixed;overflow:hidden;}";
	cssStr <<".expand{";
	if(gic_expand_flag == "true")
	{
		cssStr << "background-image: url(\"/lps-4.7.2/extjs/resources/images/default/grid/sort_asc.gif\");";
	}
	cssStr << "background-repeat:no-repeat;height:20px;width:20px;cursor:pointer;}";
	cssStr <<".collapse{";
	if(gic_expand_flag == "true")
	{
		cssStr << "background-image: url(\"/lps-4.7.2/extjs/resources/images/default/grid/sort_asc.gif\");";
	}
	cssStr << "background-repeat:no-repeat;height:20px;width:20px;cursor:pointer;}";
	int hh = atoi(gic_height.data())-atoi(head_height.data())*gic_tabnums-4*gic_tabnums;
	cssStr <<".collapse_tr{display:none;}";
    cssStr <<".head" << grid_css_random 
//		   <<"{background-image:url(\"/lps-4.7.2/extjs/resources/images/gray/panel/light-hd.gif\")" 
		   <<"{background:" << gic_hdbgcolor 
		   <<";font-size:" << head_size <<"px;padding-left:5px;padding-right:5px;color: "
		   << head_color << ";text-align:" << head_aln << ";height:"<< head_height <<"px;line-height:"<<head_height 
		   << "px;font-style:" << head_style << "; font-weight:"<< head_weight<<";}";
	cssStr <<".head" << grid_css_random << " td div{text-align:"<< head_aln<< ";}";
	code.mCss << cssStr;

	OmnString listHtmlCode;
	listHtmlCode << "<div class=\"tablist" << grid_css_random << "\">"
				 << "<table class=\"tablist_table" << grid_css_random << "\">";
	listHtmlCode << "<tbody>";
	AosXmlTagPtr column = creators->getFirstChild();
	int flag = 0;
	OmnString gic_fvpdname = "";
	OmnString vpdJson = "{}";
	while(column)
	{
		flag++;
		OmnString tr;
		OmnString gic_cont = "";
 		OmnString gic_title = column->getAttrStr("gic_title","title");
		AosXmlTagPtr content = column->getFirstChild("contents");
		OmnString vpdname = column->getAttrStr("gic_vpdname","");
		AosXmlTagPtr vpd, obj,vpdroot;
		AosHtmlCode vpdCode;
		if(vpdname != "")
		{
			bool status = htmlPtr->getVpd(vpdname,vpd,vpdroot);
			if(status)
			{
				AosLayout::createLayoutVpd(
						htmlPtr, vpd, obj, parentid, vpdCode, 200,200);//gic_bwidth, gic_bheight);
				content = 0;
				if(gic_num == flag)				
				{
					gic_fvpdname = vpdname;	
					/*
				    OmnString crtVpdNum = "";
				    crtVpdNum << htmlPtr->getVpdArrayLength()-1;
				    OmnString aosvpdStr = "aosvpd : aosvpd[";
				    aosvpdStr << crtVpdNum << "],";
				    vpdJson = vpdCode.mJson;
				    vpdJson.insert(aosvpdStr,1);
					gic_cont = vpdCode.mHtml;*/
				}
//				gic_cont = "";
			}
		}
		if(content)
		{
			OmnString text = content->getNodeText();
			text = generateTemplate(text, htmlPtr);
			gic_cont << text;
		}
		tr << "<tr  class=\"head" << grid_css_random << "\" index = \"" 
		   << flag << "\"><td style=\"padding:" << gic_padding << "px\">";
		listHtmlCode << tr 
			         << "<div class=\"hd" << grid_css_random 
					 << "\" style=\"width:100%;white-space:normal;word-break:break-all;" 
					 << "word-wrap:break-word;overflow:hidden;\">"
					 << gic_title 
					 << "</div></td>";
		/*
		listHtmlCode << "<td style=\"width:20px;\">"
		             << "<div class=\"collapse\" index =\"" << flag <<"\">"
			         << "</div></td>";
		*/
    	listHtmlCode << "</tr>";
		listHtmlCode << "<tr class=\"collapse_tr\" "; 
//		if(gic_sel =="single")
//		{
//			listHtmlCode <<	"style=\"height:"<< hh<<"px;\"";
//		}
		listHtmlCode <<	"><td colspan=\"1\">"
		             << "<div id="
					 << flag << grid_css_random
					 << " style=\"width:100%;"; 
//		if(gic_sel =="single")
//		{
//			listHtmlCode <<	" height:"<< hh<<"px;";
//		}
		listHtmlCode <<	"\">"
					 <<	gic_cont
			 	     << "</div></td></tr>";
		column = creators->getNextChild();
	}
	listHtmlCode << "</tbody></table></div>";
	code.mHtml << listHtmlCode;

	OmnString str;
	str << ","
		<< "gic_border:\"" << gic_border << "\"," 
		<< "gic_bordcolor:\"" << gic_bordcolor << "\"," 
		<< "vpdCssRandom:\"" << grid_css_random <<"\","
		<< "gic_padding:\"" << gic_padding << "\","
		<< "vpdCssRandom:\"" << grid_css_random <<"\","
		<< "gic_expand_flag:\"" << gic_expand_flag <<"\","
		<< "gic_height:"<<gic_height<<","
		<< "gic_width:"<<gic_width<<","
	 	<< "gic_tip : \""<< gic_tip <<"\"," 
		<< "gic_sel : \""<< gic_sel <<"\","
	 	<< "gic_bdbgcolor : \""<< gic_bdbgcolor <<"\"," 
	 	<< "gic_hdbgcolor : \""<< gic_hdbgcolor <<"\"," 
		<< "head_style:\""<<head_style<<"\","
		<< "head_weight:\""<<head_weight<<"\","
		<< "head_aln:\""<<head_aln<<"\","
		<< "head_color:\""<<head_color<<"\","
		<< "head_size:\""<<head_size<<"\","
		<< "head_height:\""<<head_height<<"\","
		<< "gic_tabnums:\""<< gic_tabnums<<"\","
		<< "gic_hh:\""<< hh<<"\","
		<<"vpdJson:" << vpdJson << ","
		<< "gic_fvpdname:\""<< gic_fvpdname<<"\","
	 	<< "gic_num : \""<< gic_num <<"\"" ;
//	str	<< "items:"<< items;
	code.mJson << str;
	return true;
}

OmnString
AosGicHtmlTabList::generateTemplate(OmnString &cont, const AosHtmlReqProcPtr &htmlPtr)
{
	int f1 = cont.findSubString("|&&|", 0);
	int f2 = cont.findSubString("|&|", 0);
	int f3 = cont.findSubString("&lt;", 0);
	int f4 = cont.findSubString("&gt;", 0);
	if(f1 != -1)
	{
		cont.replace("|&&|", "\"", true);
	}
	if(f2 != -1){
		cont.replace("|&|", "\'", true);
	}
	if(f3 != -1){
	    cont.replace("&lt;", "<", true);
	}
    if(f4 != -1){
        cont.replace("&gt;", ">", true);
    }
	if(!htmlPtr->isHomePage())
	{
		bool t = true;
		cont.replace("\'", "\\'", t);
	}
	return cont;
}

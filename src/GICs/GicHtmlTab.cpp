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
#include "GICs/GicHtmlTab.h"
#include "HtmlLayoutMgr/Layout.h"
#include "HtmlLayoutMgr/AllLayout.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/StrParser.h"
#include "HtmlModules/DclDb.h"

//static AosGicPtr sgGic = new AosGicHtmlTab();

AosGicHtmlTab::AosGicHtmlTab(const bool flag)
:
AosGic(AOSGIC_HTMLTAB, AosGicType::eHtmlTab, flag)
{
}


AosGicHtmlTab::~AosGicHtmlTab()
{
	cout << __FILE__ << ":" << __LINE__ << "************: " << endl;
}


bool	
AosGicHtmlTab::generateCode(
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
	//test
    AosXmlTagPtr creators = vpd->getFirstChild("creators");
	if(!creators)
	{
		return false;
	}

	AosXmlTagPtr column = creators->getFirstChild();
	if(!column)
	{
		return false;
	}

	int gic_tabnums = 0;
	gic_tabnums = creators->getNumSubtags();
	OmnString gic_mouseEvent = vpd->getAttrStr("gic_mouseEvent", "mouseover");
	OmnString gic_width = vpd->getAttrStr("gic_width","200");
	OmnString gic_height = vpd->getAttrStr("gic_height","400");
	OmnString gic_hheight = vpd->getAttrStr("gic_hheight","50");
    OmnString gic_hwidth = "";
	OmnString gic_unify = vpd->getAttrStr("gic_unify","false");
	if(gic_unify == "true")
	{
		gic_hwidth = vpd->getAttrStr("gic_hwidth","64");
	}
	OmnString gic_removeable = vpd->getAttrStr("gic_removeable","false");
	OmnString rmable = "none";
	if(gic_removeable == "true")
	{
		rmable = "inline";//"<a style=\"color:#ff0000;margin-left:10px\">x</a>"; 
	}
	OmnString removeable = "";
	removeable << "<a style=\"color:#ff0000;margin-left:10px;display:"<<rmable<<";\">x</a>";

	OmnString gic_borderw = vpd->getAttrStr("gic_borderw","1px");
	OmnString gic_bheight = "";
	gic_bheight << atoi(gic_height.data())-atoi(gic_hheight.data())-2*atoi(gic_borderw.data());
	OmnString gic_bwidth = "";
	gic_bwidth << atoi(gic_width.data()) - 2*atoi(gic_borderw.data());
	OmnString gic_fontsize = vpd->getAttrStr("gic_fontsize","12");
	OmnString gic_fontcolor = vpd->getAttrStr("gic_fontcolor","black");
	OmnString gic_fontosize = vpd->getAttrStr("gic_fontosize","18");
	OmnString gic_fontocolor = vpd->getAttrStr("gic_fontocolor","blur");
	OmnString gic_bg = vpd->getAttrStr("gic_bg");
	OmnString gic_bgo = vpd->getAttrStr("gic_bgo");
	OmnString path = htmlPtr->getImagePath();
	OmnString gic_bgstyle = vpd->getAttrStr("gic_bgstyle","image_3/eo50557.gif) repeat 0 -1050px");
	OmnString gic_bgimg = vpd->getAttrStr("gic_bgimg","image_4/bt10372.jpg");
	if(gic_bg == "")
    {
		gic_bg <<"url(" <<path << gic_bgimg <<") repeat-x";
	}
	OmnString gic_bgostyle = vpd->getAttrStr("gic_bgostyle","image_3/do50565.png) repeat 0 -140px");
	if(gic_bgo == "")
	{
	    gic_bgo <<"url(" <<path << gic_bgostyle;
	}
	OmnString gic_linecolor = vpd->getAttrStr("gic_linecolor","#3E17FF");
	OmnString tab_headhtml = "";
	OmnString headid = "thead_"; 
	headid << AosGetHtmlElemId();
	OmnString bodyid = "tbody_";
	bodyid << AosGetHtmlElemId();
	OmnString bstyle = "";
	bstyle << gic_borderw <<" solid " << gic_linecolor;
	//int borderw = gic_tabnums*2*atoi(gic_borderw);
	//int ulw = atoi(gic_width)-40-borderw;
	int activenum = 0;
	int divwidth = atoi(gic_width.data());
	activenum = getActiveNum(divwidth,vpd);
	OmnString prenext = "none";
	OmnString ulstyle = "list-style-type:none;overflow:hidden;padding:0;marging:0;";
	if(gic_tabnums>activenum)
	{
		prenext << "block";
		ulstyle <<"margin-left:20;margin-right:20;";
		divwidth = divwidth -50;
		activenum = getActiveNum(divwidth,vpd);
	}
	OmnString prevdiv = "";
	prevdiv <<"<div name=\"pre\" style=\"width:20;height:"<<gic_hheight<<";display:"<<prenext
		<<";left:0;top:0;position:absolute;background:url("<<path<<"image_3/ao9649.gif);\"></div>";
    OmnString nextdiv = "";
	nextdiv <<"<div name=\"next\" style=\"width:20;height:"<<gic_hheight<<";display:"<<prenext
		<<";right:0;top:0;position:absolute;background:url("<<path<<"image_3/ao9650.gif);\"></div>";
	tab_headhtml << "<div style=\"width:100%;height:"<<gic_hheight<<";border-left:"<<
		bstyle<<";\">"<<prevdiv<<nextdiv<<"<ul style=\""<<ulstyle<<"\">";
	OmnString tab_contents = "";
	int index=0;
	OmnString gic_activetab;
	OmnString headocss = "";
	OmnString gic_textpt = vpd->getAttrStr("gic_textpt","1px");
	OmnString spn = "";
	spn <<"<span style=\"white-space:nowrap;display:inline-block;padding-top:"<<gic_textpt<<";\">";
	OmnString gic_csstext="";
	gic_csstext <<"cursor:pointer;color:"<<gic_fontcolor<<";border-right:"<<bstyle<<";display:block"<<
		";border-top:"<<bstyle<<";font-Size:"<<gic_fontsize<<";text-align:center;padding:0 1px 0 0;float:left;";
	OmnString gic_bodycolor = vpd->getAttrStr("gic_bodycolor","#ffffff");
	OmnString divcss = "";
	divcss <<"border:" <<bstyle <<";background:"<<gic_bodycolor<<";"; 
   	OmnString vpdJson = "{}"; 
	column = creators->getFirstChild();
	OmnString fvpdname;
	while(column)
	{
	    OmnString title = column->getAttrStr("name","first");
		OmnString objid = column->getAttrStr("objid","");
		if(objid == "")
		{
			int newobjid = (rand()%10)*1000 + (rand()%10)*100 + (rand()%10)*10;
			objid << newobjid;
			column->setAttr("objid",objid);
		}
		OmnString display = "none";
		OmnString head_display = "block";
		OmnString csstext = gic_csstext;
		OmnString width = column->getAttrStr("width","64");
		if(gic_hwidth!="")
		{
			width = gic_hwidth;
		}
		csstext <<"width:"<<width<<";";
		if(index==0)
		{
		    display = "block";
			gic_activetab = "0";
			csstext << "background:"<<gic_bgo<<";height:"<<gic_hheight<<";";
		}
		else
		{
			OmnString overh = "";
			overh << atoi(gic_hheight.data())-atoi(gic_borderw.data());
			csstext <<"background:" << gic_bg<<";height:"<<overh<<";";
		}
		if(index>=activenum)
		{
			head_display = "none";
			csstext << "display:"<<head_display;
		}
		else
		{
			csstext <<"display:"<<head_display;
		}
		AosXmlTagPtr contents = column->getFirstChild("contents");
		OmnString con = "";
		OmnString vpdname = column->getAttrStr("gic_vpdname","");
		OmnString oname = column->getAttrStr("gic_objname","");
		AosXmlTagPtr vpd, vpdroot, obj;
		AosHtmlCode vpdCode;
		if(index==0 && vpdname != "")
		{
			fvpdname = vpdname;
			bool status = htmlPtr->getVpd(vpdname, vpd, vpdroot);
			if(status)
			{
				OmnString vpdPath = htmlPtr->createVpdPath(vpdname);
                OmnString flag = htmlPtr->getInsertFlag();
				htmlPtr->appendVpdArray(vpd, obj, parentid, flag, vpdPath, 200, 200);
				
				/*
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
				code.mCss << vpdCode.mCss;
				*/
				contents = 0;
				con = flag;
				vpdJson = flag;
			}
		}

		if(contents)
		{
		    con = contents->getNodeText();
			//int findHandle = con.findSubString("gAosHtmlHandler",0);
			//if(findHandle != -1)
			//{
			  //  htmlPtr->addGic("gAosHtmlHandler");
			//}
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
	//	OmnString divcss = "";
	//	divcss <<"border-left:" <<bstyle <<";border-right:" << bstyle <<";border-bottom:" << bstyle <<";";
		tab_headhtml << "<li name=\""<<headid<<"_"<<index<<"\" objid=\"" <<objid 
			<<"\" vpdname=\""<<vpdname
			<<"\" oname=\""<<oname
			<< "\" style=\"" << csstext << "\">" << spn<<title << "</span>" <<removeable<<"</li>";
		tab_contents << "<div name=\""<<bodyid<<"_"<<index<<"\" style=\"width:"<<gic_bwidth<<";height:" << 
			gic_bheight << ";display:" << display <<";overflow:auto;" <<divcss <<"\">" << con << "</div>";
		column = creators->getNextChild();
		index++;
	}
    code.mHtml << tab_headhtml << "</ul></div>" << tab_contents ;//<< "</div";
	code.mJson <<",gic_hheight:\"" << gic_hheight <<"\""
			   <<",gic_bheight:\"" << gic_bheight <<"\""
			   <<",gic_hwidth:\"" << gic_hwidth <<"\""
			   <<",gic_bwidth:\"" << gic_bwidth <<"\""
			   <<",headid:\"" << headid <<"\""
			   <<",bodyid:\"" << bodyid <<"\""
			   <<",gic_bg:\"" << gic_bg <<"\""
			   <<",gic_bgo:\"" << gic_bgo <<"\""
			   <<",gic_bgimg:\"" << gic_bgimg <<"\""
			   <<",gic_bgostyle:\"" << gic_bgostyle  <<"\""
			   <<",gic_textpt:\"" << gic_textpt <<"\""
			   <<",gic_linecolor:\"" << gic_linecolor <<"\""
			   <<",gic_borderw:\"" << gic_borderw <<"\""
			   <<",gic_fvpdname:\"" << fvpdname <<"\""
			   <<",vpdJson:" << vpdJson 
			   <<",gic_fontsize:\"" << gic_fontsize <<"\""
			   <<",gic_fontosize:\"" << gic_fontosize <<"\""
			   <<",gic_fontcolor:\"" << gic_fontcolor <<"\""
			   <<",gic_bodycolor:\"" << gic_bodycolor <<"\""
			   <<",gic_activetab:\"" << gic_activetab <<"\""
			   <<",gic_csstext:\"" << gic_csstext <<"\""
			   <<",gic_divcss:\"" << divcss <<"\""
			   <<",gic_mouseEvent:\"" << gic_mouseEvent <<"\""
			   <<",gic_unify:" << gic_unify
			   <<",gic_removeable:" << gic_removeable
			   <<",columns:" << index;
	return true;
}

int
AosGicHtmlTab::getActiveNum(const int &divwidth,const AosXmlTagPtr &vpd)
{
	AosXmlTagPtr creators = vpd->getFirstChild("creators");
	AosXmlTagPtr col = creators->getFirstChild();//column;
	OmnString gic_hwidth="";
	OmnString gic_unify = vpd->getAttrStr("gic_unify","false");
	if(gic_unify == "true")
	{
	    gic_hwidth = vpd->getAttrStr("gic_hwidth","64");
	}
	OmnString gic_borderw = vpd->getAttrStr("gic_borderw","1px");
//	OmnString gic_width = vpd->getAttrStr("gic_width","200");
//	int divwidth = atoi(gic_width);
	int hwidth = 0;
	int activenum=0;
	while(col)
	{
		OmnString width = col->getAttrStr("width");
		if(gic_unify=="true")
		{
			width = gic_hwidth;
		}
		hwidth += atoi(width.data()) + 2*atoi(gic_borderw.data());
		if(hwidth < divwidth)
		{
			activenum++;
		}
		else
		{
			break;
		}
		col = creators->getNextChild();
	}
	return activenum;
}

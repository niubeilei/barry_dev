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
//var config=
//{
//  el:'img1',
//  xtype:'gic_postit',
//  sel:true,
//  bgimg:'K3.jpg',
//  fontsize:23,
//  content:'To do...',
//  width:136,
//  height:120
//}
//
// Modification History:
// 08/07/2010: Created by felicia
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicPostIt.h"
#include "HtmlUtil/HtmlUtil.h"
#include "HtmlServer/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

// static AosGicPtr sgGic = new AosGicPostIt();

AosGicPostIt::AosGicPostIt(const bool flag)
:
AosGic(AOSGIC_POSTIT, AosGicType::ePostit, flag)
{
}


AosGicPostIt::~AosGicPostIt()
{
}


bool	
AosGicPostIt::generateCode(
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
	// convertToJson(vpd, code.mJson);
	OmnString gic_id=AosGetHtmlElemId();
	vpd->setAttr("gic_id",gic_id);
	OmnString gic_nailid=AosGetHtmlElemId();
	OmnString gic_nailimg=vpd->getAttrStr("gic_pinsrc");
	OmnString gic_edtimg=vpd->getAttrStr("gic_edtbgsrc");
	OmnString gic_delimg=vpd->getAttrStr("gic_delbgsrc");
	OmnString gic_bgimg = vpd->getAttrStr("gic_bgImgSrc");
	OmnString path = htmlPtr->getImagePath();
	OmnString gic_content;
	OmnString content;
	AosXmlTagPtr contentChild = vpd->getFirstChild("contents");
    if(contentChild){
              content = contentChild->getNodeText();
    }
	AosXmlTagPtr xml;
	OmnString content_bd = vpd->getAttrStr("gic_cntbd");
	/*if(content_bd !="" && obj)
	{
	    gic_content = obj->getAttrStr(content_bd,vpd->getAttrStr("gic_content"));
	}
	else
	{
	    gic_content = vpd->getAttrStr("gic_content");
	}*/
	AosGetXmlValue(obj, content_bd,content, 
			gic_content, xml);
	int gic_left = vpd->getAttrInt("gic_lfs",24);
	int gic_right = vpd->getAttrInt("gic_rgts",34);
	int gic_top = vpd->getAttrInt("gic_tps",85);
	int gic_bottom = vpd->getAttrInt("gic_btms",85);
    OmnString gic_txtid = AosGetHtmlElemId();
    OmnString gic_pmtbgc = vpd->getAttrStr("gic_pmtbgc","#ffffff");
	if(gic_pmtbgc!="") gic_pmtbgc = AosHtmlUtil::getWebColor(gic_pmtbgc);
	OmnString pmtbgc;
	if(gic_pmtbgc != "")
	{
	    pmtbgc << "#" << gic_pmtbgc.substr(2, gic_pmtbgc.length());
	}
	else
	{
	   pmtbgc = "transparent";
	}
	code.mHtml <<"<img src=\""<<path<<gic_bgimg<<"\" width=\"100%\" height=\"100%\" id=\"img\"/>"<<
	           "<textarea id=\""<<gic_txtid<<"\" wrap=PHYSICAL  "<<
	           "style=\"border:0;position:absolute;visibility:visible;width:80%;height:60%;overflow:hidden;left:"<<
			   gic_left<<";rigth:"<<gic_right<<";top:"<<gic_top<<";bottom:"<<gic_bottom
			   <<";background:transparent;z-index:1;\">"<<
	           gic_content<<"</textarea>"<<
	           "<img src=\""<<path<<gic_nailimg<<"\" id=\""<<gic_nailid<<"\" style=\"position:absolute;width:25px;height:25px;left:50%;top:2%;visibility:hidden;\" />"<<
	          "<span style=\"visibility:hidden;left:20%;top:90%;position:absolute;\"></span>"<<
			  "<div style=\"border:0;position:absolute;z-index:0;visibility:visible;width:80%;height:60%;overflow:hidden;left:"
			  <<gic_left<<";rigth:"<<gic_right<<";top:"<<gic_top<<";bottom:"<<gic_bottom<<";\">";
	OmnString gic_contentbd = vpd->getAttrStr("gic_cntbd");
	OmnString gic_nail = vpd->getAttrStr("gic_addpin","false");
	OmnString gic_sel = vpd->getAttrStr("gic_isdblpmt");
	int gic_fontsize = vpd->getAttrInt("gic_pmts",12);
	OmnString gic_fontfamily = vpd->getAttrStr("gic_pmtft");
	OmnString gic_fontcolor = vpd->getAttrStr("gic_cntfc","#000000");
	if(gic_fontcolor!="") gic_fontcolor = AosHtmlUtil::getWebColor(gic_fontcolor);
	int gic_nailX = vpd->getAttrInt("gic_pinx",10);
	int gic_nailY = vpd->getAttrInt("gic_piny",5);
	OmnString gic_type = vpd->getAttrStr("gic_type");
    int gic_nailsize = vpd->getAttrInt("gic_pinw",25);
	OmnString gic_showtool = vpd->getAttrStr("gic_isshowtoolft","false");
	OmnString gic_showtoolimg = vpd->getAttrStr("gic_isshowtool","false");
	OmnString gic_fontStyle = vpd->getAttrStr("gic_pmtfts");
	OmnString gic_showtime = vpd->getAttrStr("gic_isshowtm","false");
	int gic_timex = vpd->getAttrInt("gic_tmposx",40);
	int gic_timey = vpd->getAttrInt("gic_tmposy",260);
	OmnString gic_edttxt = vpd->getAttrStr("gic_edttxt","edit");
	OmnString gic_deltxt = vpd->getAttrStr("gic_deltxt","close");
	int gic_tly = vpd->getAttrInt("gic_tooly",35);
	int gic_delx = vpd->getAttrInt("gic_delx",115);
	int gic_editx = vpd->getAttrInt("gic_editx",15);
	int gic_tlww = vpd->getAttrInt("gic_tlww",20);
	int gic_tlhh = vpd->getAttrInt("gic_tlhh",15);
	int gic_tlfontsize = vpd->getAttrInt("gic_tlfs",10);
	OmnString gic_delbgsrc = vpd->getAttrStr("gic_delbgsrc");
	OmnString gic_edtbgsrc = vpd->getAttrStr("gic_edtbgsrc");
    OmnString gic_edtbgc = vpd->getAttrStr("gic_edtbgc","#ffffff");
	if(gic_edtbgc!="") gic_edtbgc = AosHtmlUtil::getWebColor(gic_edtbgc);
    OmnString gic_edtopt = vpd->getAttrStr("gic_edtopt","1");
    OmnString gic_pmtopt = vpd->getAttrStr("gic_pmtopt","1");
	AosXmlTagPtr actionsChild = vpd->getFirstChild("actions");
    OmnString actions;
	if(actionsChild){
         actions=actionsChild->toString();
    }
	code.mJson<<",";
	code.mJson<<"bgimg:\'"<<gic_bgimg<<"\',";
	code.mJson<<"content:\'"<<gic_content<<"\',";
	code.mJson<<"contentbd:\'"<<gic_contentbd<<"\',";
	code.mJson<<"edtbgc:\'"<<gic_edtbgc<<"\',edtopt:\'"<<gic_edtopt;
	code.mJson<<"\',pmtbgc:\'"<<gic_pmtbgc<<"\',pmtopt:\'"<<gic_pmtopt;
	code.mJson<<"\',edttxt:\'"<<gic_edttxt;
	code.mJson<<"\',deltxt:\'"<<gic_deltxt;
	code.mJson<<"\',tly:"<<gic_tly;
	code.mJson<<",delx:"<<gic_delx<<",editx:"<<gic_editx<<",tlwidth:"<<gic_tlww;
	code.mJson<<",tlheight:"<<gic_tlhh<<",tlfontsize:"<<gic_tlfontsize;
	code.mJson<<",delbgsrc:\'"<<gic_delbgsrc<<"\',edtbgsrc:\'"<<gic_edtbgsrc<<"\'";
	code.mJson<<",left:"<<gic_left;
	code.mJson<<",right:"<<gic_right;
	code.mJson<<",top:"<<gic_top;
	code.mJson<<",showtool:"<<gic_showtool;
	code.mJson<<",showtoolimg:"<<gic_showtoolimg;
	code.mJson<<",nimg:\'"<<gic_nailimg;
	code.mJson<<"\',nailsize:"<<gic_nailsize;
	code.mJson<<",bottom:"<<gic_bottom;
	code.mJson<<",naiX:"<<gic_nailX;
	code.mJson<<",nailY:"<<gic_nailY;
	code.mJson<<",timex:"<<gic_timex;
	code.mJson<<",timey:"<<gic_timey;
	code.mJson<<",sel:"<<gic_sel;
    code.mJson<<",nail:"<<gic_nail;
	code.mJson<<",showtime:"<<gic_showtime;
	code.mJson<<",fontsize:"<<gic_fontsize;
	code.mJson<<",fontStyle:\'"<<gic_fontStyle;
	code.mJson<<"\',fontcolor:\'"<<gic_fontcolor;
	code.mJson<<"\',fontFamily:\'"<<gic_fontfamily<<"\'";
	return true;
}


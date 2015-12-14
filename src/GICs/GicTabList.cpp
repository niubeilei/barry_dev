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
// 07/08/2010: Created by Wynn
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicTabList.h"
#include "HtmlModules/DclDb.h"
#include "HtmlUtil/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

// static AosGicPtr sgGic = new AosGicTabList();

AosGicTabList::AosGicTabList(const bool flag)
:
AosGic(AOSGIC_TABLIST, AosGicType::eTabList, flag)
{
	OmnString errmsg;
	AosGicType::addName(AOSGIC_CONTACTLIST, mGicType, errmsg);
}


AosGicTabList::~AosGicTabList()
{
}


bool	
AosGicTabList::generateCode(
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

	//convertToJson(vpd, jsonStr);
//	OmnString gic_value = vpd->getAttrStr("gic_value", "ClickableString");
//	OmnString gic_id = vpd->getAttrStr("gic_id"); 
	aos_assert_r(convertToJson(vpd,obj,code), false);
	return true;
}

bool	
AosGicTabList::convertToJson(
		const AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		AosHtmlCode &code)
{
	OmnString valbd1 = vpd->getAttrStr("gic_valuebd");
	OmnString gic_value;
	if (valbd1 != "" && obj)
	{
		gic_value = obj->getAttrStr(valbd1, vpd->getAttrStr("gic_value"));
	}
	else
	{
		gic_value = vpd->getAttrStr("gic_value");
	}
//	OmnString gic_id = vpd->getAttrStr("gic_id"); 
//	code.mCss << ".x-panel-tl,.x-panel-tr{background-image:url('') !important;background:#f2f2f2 !important}"
//			  << ".x-panel-header {background-image:url('') !important;background:#f2f2f2 !important}";
	// We assume 'gic_type' is always 'gic_tablist'
 	OmnString pmt_text = vpd->getAttrStr("pmt_text","title");
 	OmnString gic_lbaln = vpd->getAttrStr("pmt_align","left");
 	OmnString gic_lbfclr = vpd->getAttrStr("pmt_fgcolor","red");
 	OmnString gic_lbfsz = vpd->getAttrStr("pmt_fsize","2");
 	OmnString gic_lbfstl = vpd->getAttrStr("pmt_fstyle","bold");
 	OmnString gic_sel = vpd->getAttrStr("gic_sel","");
 	OmnString gic_tip = vpd->getAttrStr("gic_tip","");
 	OmnString panel_fgcolor = vpd->getAttrStr("panel_fgcolor","#f2f2f2");
	OmnString tsl_intab = vpd->getAttrStr("gic_num"); 
	OmnString tsl_bgclr = vpd->getAttrStr("gic_bgcolor", "#f2f2f2");
/*	AosXmlTagPtr content_vpd = vpd->getFirstChild("datacol");
	OmnString tsl_tabs;
	OmnString rsep;
	OmnString asep;
	OmnString fsep;
	OmnString valbd;
	OmnString content;
	if(content_vpd)
	{
		content = content_vpd->toString();
		tsl_tabs = content_vpd->getAttrStr("cmp_contents"); 
		rsep = content_vpd->getAttrStr("cmp_entry_sep","|$$|"); 
		fsep = content_vpd->getAttrStr("cmp_field_sep","|$|"); 
		asep = content_vpd->getAttrStr("cmp_attr_sep","|^|"); 
		valbd = content_vpd->getAttrStr("cmp_valbd"); 
		OmnScreen << "--------- : " << content_vpd->getTagname() << endl;
	}
  */ 
   
//  int gicnum = gicNum(tsl_tabs);
    /*
	OmnString items;
	items <<"[";
	int i;
	for(i=0;i<gicnum;i++)
	{
	   if(i!=0) items <<",";
	   htmlPtr->createAndSwitchGic();
	   items << config;
       
	} 
	items <<"]";
	*/
	//AosXmlTagPtr docRoot = dcl->retrieveData(vpd, "");
	//OmnScreen << docRoot->getFirstChild()->getData()<< endl;
	OmnString str;
	str << ",";
//	str << "tsl_tabs : \'"<< tsl_tabs <<"\'," ;
	str << "text : \'"<< pmt_text <<"\'," ;
	str << "gic_lbaln : \'"<< gic_lbaln <<"\'," ;
	str << "gic_tip : \'"<< gic_tip <<"\'," ;
	str << "gic_sel : \'"<< gic_sel <<"\'," ;
	str << "panel_fgcolor : \'"<< panel_fgcolor <<"\'," ;
	str << "gic_lbfclr : \'"<< gic_lbfclr <<"\'," ;
	str << "gic_lbfsz : \'"<< gic_lbfsz <<"\'," ;
	str << "gic_lbfstl : \'"<< gic_lbfstl <<"\'," ;
	str << "tsl_bgclr : \'"<< tsl_bgclr <<"\'," ;
	if(gic_value != "")
		str << "titles : \'" << gic_value << "\',"; 
	str << "tsl_intab : \'"<< tsl_intab <<"\'" ;
//	str << "content : \'"<< content <<"\'" ;
/*	str << "cmp : { " ;
	str << "mcontent : \'" << tsl_tabs << "\'," ;
	str << "rsep : \'" << rsep << "\'," ;
	str << "asep : \'" << asep << "\'," ;
	str << "fsep : \'" << fsep << "\'," ;
	str << "valbd : \'" << valbd <<"\'" ;
	str << "}" ;
	*/
//	str	<< "items:"<< items;
	str << "" ;

	code.mJson << str;
	return true;
}

/*
AosGicTabList::gicNum(OmnString str)
{
   int num = 0;
   OmnString s1,s2;
   s1 << "|$$|"; s2 << ":";
   str.replace(s1,s2,true);
   int flag = str.find(':',false);
	while(flag>0)
	{
		str = str.substr(flag+1,str.length()-1);
		flag = str.find(':',false);
		num++;
	}
	return num;
}
AosGicTabList::changToArray(
			OmnString str,
			OmnString temp[])
{
	int i = 0;
	OmnString s1 ,s2;
	s1 << "|$$|";
	s2 << ",";
	str.replace(s1,s2,true);
	int flage = str.find(',',false);
	while(flage>0)
	{
		temp[i] = str.substr(0,flage-1);
		str = str.substr(flage+1,str.length()-1);
		flage = str.find(',',false);
		i++;
	}
	if(str!="")
	temp[i] = str;
	return true;
}
*/

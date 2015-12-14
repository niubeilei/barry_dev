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
// 08/21/2010: Created by Cody
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicNewsList.h"
#include "HtmlModules/DclDb.h"

#include "HtmlUtil/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

// static AosGicPtr sgGic = new AosGicNewsList();

AosGicNewsList::AosGicNewsList(const bool flag)
:
AosGic(AOSGIC_NEWSLIST, AosGicType::eNewsList, flag)
{
}


AosGicNewsList::~AosGicNewsList()
{
}


bool	
AosGicNewsList::generateCode(
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
	// 	
	// 	html:
	OmnString valbd = vpd->getAttrStr("gic_valued");
	OmnString gic_value;
	const OmnString str_datacol = "str_datacol";
	const OmnString db_datacol = "db_datacol"; 
	OmnString gic_id2 = AosGetHtmlElemId();
	OmnString gic_vsp = vpd->getAttrStr("gic_vs", "2");
	OmnString gic_hsp = vpd->getAttrStr("gic_hs", "2");
	OmnString gic_roff = vpd->getAttrStr("gic_rinset", "2");
	OmnString gic_loff = vpd->getAttrStr("gic_linset", "2");
	OmnString gic_boff = vpd->getAttrStr("gic_binset", "2");
	OmnString gic_toff = vpd->getAttrStr("gic_tinset", "2");
	//是否显示滚动条
	OmnString gic_sb = vpd->getAttrStr("gic_scrollBar_v", "true");
	//底部的工具条
	OmnString gic_btbar = vpd->getAttrStr("gic_btbar", "false");
	OmnString cmp_fgcbd;
	OmnString cmp_value_bind;
	OmnString gic_html, field;


	AosXmlTagPtr datacol = vpd->getFirstChild("creators");
	OmnString colType, cmp_tname, cmp_query, cmp_order
		, cmp_fnames, cmp_tnamebd, cmp_psize, cmp_qrm, cmp_dftqrm
		, cmp_anames, cmp_entry_sep, cmp_field_sep, cmp_contents;
	if(datacol)
	{
		datacol = datacol->getFirstChild();	
	}
	OmnString cmp_width = datacol->getAttrStr("cmp_width");
	OmnString cmp_height = datacol->getAttrStr("cmp_height");
	cmp_value_bind = datacol->getAttrStr("cmp_value_bind", "@classname");
	cmp_fgcbd = datacol->getAttrStr("cmp_fgcbd", "@color");
	
	//datacol中的属性
    datacol = vpd->getFirstChild("datacol");
	colType = datacol->getAttrStr("gic_type");
	if(colType == db_datacol)
	{
		cmp_tname = datacol->getAttrStr("cmp_tname", "");
		cmp_query = datacol->getAttrStr("cmp_query", "");
		cmp_order = datacol->getAttrStr("cmp_order", "");
		cmp_fnames = datacol->getAttrStr("cmp_fnames","");
		cmp_tnamebd = datacol->getAttrStr("cmp_tnamebd","");
		cmp_psize = datacol->getAttrStr("cmp_psize","");
		cmp_qrm = datacol->getAttrStr("cmp_qrm","");
		cmp_dftqrm = datacol->getAttrStr("cmp_dftqrm","");
	}
	else if(str_datacol == colType)
	{
		cmp_anames = datacol->getAttrStr("cmp_anames", "");
		cmp_entry_sep = datacol->getAttrStr("cmp_entry_sep", ",");
		cmp_field_sep = datacol->getAttrStr("cmp_field_sep", ":");
		cmp_contents = datacol->getAttrStr("cmp_contents");
	}
	

	field << "[{name : \"name\", mapping : \""<<cmp_value_bind
		  <<"\"}, {name : \"color\", mapping : \""<<cmp_fgcbd<<"\"}]";
//	gic_html << "<div class=\'item\' style=\'width:"<<cmp_width<<"px;height:"<<cmp_height
//		     << "px;\'><span style=\'color:{color};\'>{name}</span></div>";
	gic_html << vpd->getAttrStr("gic_html", "");
	//css
	//有文字
	code.mCss << "#a" << gic_id2 << " .item{display : inline-block;margin-left:"<< gic_vsp 
			  << "px;margin-top:" << gic_hsp << "px;text-align : center;}";
	code.mCss << "#a" << gic_id2 << " .item{*display : inline;}";
	code.mCss << "#a" << gic_id2 << "{background-color:transparent;padding-right:"
			  << gic_roff << "px;padding-left:" << gic_loff
			  << "px;padding-top:" << gic_toff << "px;padding-bottom:"
			  << gic_boff <<"px;}";
	code.mCss << "#a" << gic_id2 << " .item span{line-height:"<< cmp_height <<"px;margin-top:"<<gic_hsp<<"px;}";
	code.mCss << "#a" << gic_id2 << " div.icon-hover{border:0px solid #dddddd; background-color:#efefef;}";
	code.mCss << "#a" << gic_id2 << " div.x-view-selected{border:0px solid #99bbe8;background-color:#eff5fb;}";

	//json
 code.mJson << ","
			<< "id_2:\"a" << gic_id2 << "\","
			<< "cmp : {";
 if(db_datacol == colType)
 {
 	code.mJson	<<"cmp_tname : \"" << cmp_tname << "\"," 
				<< "cmp_query : \"" << cmp_query << "\","
				<< "cmp_order : \"" << cmp_order << "\","
				<< "cmp_fnames : \"" << cmp_fnames <<"\","
				<< "cmp_tnamebd : \"" << cmp_tnamebd << "\","
				<< "cmp_psize : \"" << cmp_psize << "\","
				<< "cmp_qrm : \"" << cmp_qrm << "\","
				<< "cmp_dftqrm : \"" << cmp_dftqrm << "\"";
 }			
 else
 {
 	code.mJson << "anames : \"" << cmp_anames << "\","
			   << "rsep : \"" << cmp_entry_sep << "\","
			   << "fsep : \"" << cmp_field_sep << "\","
			   << "mcontent : \"" << cmp_contents << "\"";
 }	 		
 code.mJson	<< "},"
			<< "gic_html : \"" << gic_html << "\","
			<< "field : " << field << ","
			<< "autoScroll : " << gic_sb << ","
			<< "gic_btbar : " << gic_btbar << ","
			<< "colType : \"" << colType << "\"";
	
	return true;
}


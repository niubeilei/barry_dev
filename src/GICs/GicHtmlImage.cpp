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
#include "GICs/GicHtmlImage.h"

#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

// static AosGicPtr sgGic = new AosGicHtmlImage();

AosGicHtmlImage::AosGicHtmlImage(const bool flag)
:
AosGic(AOSGIC_HTMLIMAGE, AosGicType::eHtmlImage, flag)
{
}


AosGicHtmlImage::~AosGicHtmlImage()
{
}


bool	
AosGicHtmlImage::generateCode(
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
	//////////////////////////////////////
	
	OmnString srcbd = vpd->getAttrStr("img_src_bind");
	OmnString src;
	if (srcbd != "" && obj)
		src = obj->getAttrStr(srcbd, vpd->getAttrStr("img_src"));
	else
		src = vpd->getAttrStr("img_src", "system/es1073.jpg");
	
	OmnString img_original = vpd->getAttrStr("img_original", "false");
	OmnString img_ncsrc = vpd->getAttrStr("img_ncsrc", "false");
	if(img_original != "false")
	{
		img_ncsrc = "true";
		const char pattern  = '/';
		int index = src.indexOf(0, pattern, true);
		OmnString sub1 = src.substr(0, index+1); 
		OmnString sub2 = src.substr(index+3, src.length()-1); 
		src = sub1 <<  "o"  << sub2;

	}
	// compute ratio
	OmnString horlyt = vpd->getAttrStr("hor_layout", "center");
	OmnString varlyt = vpd->getAttrStr("var_layout", "middle");
	const char *hor = horlyt.data();
	const char *var = varlyt.data();
	OmnString rat = vpd->getAttrStr("img_nkratio", "false");
	OmnString img_cursor = vpd->getAttrStr("img_cursor", "default");

	float ow = vpd->getAttrInt("gic_owidth", 128);
	float oh = vpd->getAttrInt("gic_oheight", 96);
	float ww = vpd->getAttrInt("gic_width", 128);
	float hh = vpd->getAttrInt("gic_height", 128);
	int lf = 0, tp = 0, dw = ww, dh = hh;

	if(rat == "false")
	{
		if((ww/hh) < (ow/oh))
		{
			dh = oh * (ww/ow);
			switch (var[0])
			{
			case 't':
				 tp = 0;
				 break;
			case 'b':
				 tp = hh - dh;
				 break;
			default:
				 tp = (hh - dh)/2;
				 break;
			}
		}
		else
		{
			dw = ow * (hh/oh);
			switch (hor[0])
			{
			case 'l':
				 lf = 0;
				 break;
			case 'r':
				 lf = ww - dw;
				 break;
			default:
				 lf = (ww - dw)/2;
				 break;
			}
		}
	}
	//end
	OmnString opc = vpd->getAttrStr("img_opct", "1");

	OmnString gic_id = AosGetHtmlElemId(); 
	OmnString imageHtmlCode;
	OmnString path = htmlPtr->getImagePath();

	OmnString fullpath = path;
	fullpath << src;
	OmnString http = "http://";
	if(src.indexOf(http, 0) == 0) fullpath = src;
	bool gic_video = vpd->getAttrBool("gic_video", false);
	OmnString gic_video_type = vpd->getAttrStr("gic_video_type");

	if (!gic_video)
	{
		imageHtmlCode << "<div style=\"top:"<< tp <<"px; left:" << lf << "px; width:"<< dw 
			<< "px; height:" << dh << "px; opacity: "<< opc <<"; position: absolute\" >"
			<< "<img style=\"position: absolute; width:100%; height:100%;cursor:" << img_cursor <<";\" src=\"" << fullpath 
			<< "\" onload=\"pngImageLoad(this);\" onerror=\"error(this)\"/>"
			<< "</div>";
		code.mHtml << imageHtmlCode ;

		code.mJson  
			<< ",src: \"" << src 
			<< "\",srcbd: \"" << srcbd 
			<< "\",ratio:" << vpd->getAttrStr("img_nkratio", "false") 
			<< ",dressup:\"" << vpd->getAttrStr("img_style", "No") 
			<< "\",gic_iszoom:" << vpd->getAttrStr("gic_iszoom", "false")
			<< ",gic_zoomww:\"" << vpd->getAttrStr("gic_zoomww", "150")
			<< "\",gic_zoomhh:\"" << vpd->getAttrStr("gic_zoomhh", "150")
			<< "\",gic_zoomrad:\"" << vpd->getAttrStr("gic_zoomrad", "75")
			<< "\",gic_zoomsdw:" << vpd->getAttrStr("gic_zoomsdw", "false")
			<< ",opacity:\"" << vpd->getAttrStr("img_opct", "1") 
			<< "\",sft_opc:\"" << vpd->getAttrStr("sft_opc", "1") 
			<< "\",duration:\"" << vpd->getAttrStr("img_anitime", "500") 
			<< "\",img_anitype:\"" << vpd->getAttrStr("img_anitype", "No")
			<< "\",img_fcolor:\"" << vpd->getAttrStr("img_fcolor", "ffff00") 
			<< "\",img_anchor:\"" << vpd->getAttrStr("img_anchor", "t") 
			<< "\",img_easing:\"" << vpd->getAttrStr("img_easing", "easeOut")
			<< "\",img_evt:\"" << vpd->getAttrStr("img_evt", "mouseover")
			<< "\",img_clip:" << vpd->getAttrStr("img_clip", "false")
			<< ",img_tips:\"" << vpd->getAttrStr("img_tips", "")
			<< "\",hor_layout:\"" << vpd->getAttrStr("hor_layout", "center")
			<< "\",var_layout:\"" << vpd->getAttrStr("var_layout", "middle")
			<< "\",img_ncsrc: " << img_ncsrc 
			<< ",img_original: " << img_original 
			<< ",img_movable:" << vpd->getAttrStr("img_movable", "false") 
			<< ",gic_oheight:" << oh 
			<< ",gic_owidth:" << ow 
			<< ",gic_zmult:" << vpd->getAttrInt("gic_zmult", 2) 
			<< ",fscreen:" << vpd->getAttrInt("img_w_fscreen", 0)
			<< ",img_fripple:" << vpd->getAttrInt("img_fripple", 1);
	}
	else
	{
		imageHtmlCode << "<div style=\"position:absolute\"></div>";
		code.mHtml << imageHtmlCode ;
		code.mJson << ",gic_video:" << gic_video
			<< ",gic_video_type:\"" << gic_video_type << "\"";
	}
	return true;
}


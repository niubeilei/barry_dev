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
// 2011/02/19 Created by Ken
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicHtmlFramer.h"

#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

// static AosGicPtr sgGic = new AosGicHtmlFramer();

AosGicHtmlFramer::AosGicHtmlFramer(const bool flag)
:
AosGic(AOSGIC_HTMLFRAMER, AosGicType::eHtmlFramer, flag)
{
}


AosGicHtmlFramer::~AosGicHtmlFramer()
{
}


bool	
AosGicHtmlFramer::generateCode(
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
	//convertToJson(vpd,code.mJson);
	//test
	//get parent width and height	
	OmnString width = vpd->getAttrStr("gic_width");
	OmnString height = vpd->getAttrStr("gic_height");

	AosXmlTagPtr framertag=vpd->getFirstChild("framer");
	OmnString gic_type = framertag->getAttrStr("gic_type");
	OmnString src = framertag->getAttrStr("cmp_src");
	OmnString srcsize = framertag->getAttrStr("cmp_sizedef");
	OmnString xoffset = framertag->getAttrStr("cmp_xoffset");
	OmnString yoffset = framertag->getAttrStr("cmp_yoffset");
	OmnStrParser1 parser(srcsize, ":");
	int gic_width = atoi(width.data());
	int gic_height = atoi(height.data());
	int cmp_xoffset = atoi(xoffset.data());
	int cmp_yoffset = atoi(yoffset.data());
	int ww;
	int hh;
	u32 len1;
	u32 len2;

	OmnString wd1=parser.nextWord();
	OmnString wd2=parser.nextWord();
	len1=wd1.length();
	len2=wd2.length();


	wd1.parseInt(0,len1, ww);
	wd2.parseInt(0,len2, hh);
	//int ww=atoi(parser.nextWord());
	//int hh=atoi(parser.nextWord());
//	int mSizes[]={ww, hh, 1, hh, ww, hh, ww, 1,ww, 1, ww, hh, 1, hh, ww, hh};
	mSizes[0]=ww;
	mSizes[1]=hh;
	mSizes[2]=1;
	mSizes[3]=hh;
	mSizes[4]=ww;
	mSizes[5]=hh;
	mSizes[6]=ww;
	mSizes[7]=1;
	mSizes[8]=ww;
	mSizes[9]=1;
	mSizes[10]=ww;
	mSizes[11]=hh;
	mSizes[12]=1;
	mSizes[13]=hh;
	mSizes[14]=ww;
	mSizes[15]=hh;


	// Create the inner view
	int ww5 = mSizes[10];
	int ww7 = mSizes[14];
	int hh0 = 0;
	int ww1 = gic_width - ww5 - ww7;

	// Upper left Corner
	hh0 = mSizes[1];
	int ww2 = mSizes[4];
	int hh2 = mSizes[5];

	// Top Edge
	int hh1 = mSizes[3];

	// Upper Right Corner
	// Left Edge
	int hh5=mSizes[11];
	int ww3 = mSizes[6];
	int hh3 = gic_height - hh0 - hh5;
	
	// Right Edg
	int ww8 = mSizes[14];
	int hh7 = mSizes[15];
	int ww4 = mSizes[8];
	int hh4 = gic_height - hh0 - hh7;

	// Left Lower Corner
	// Bottom Edge
	int ww6 = gic_width - ww5 - ww8;
	int hh6 = mSizes[13];

	// Right bottom corner
	
	//create innerview 
	int innervX = cmp_xoffset + ww5 - 2;
	int innervY = cmp_yoffset + hh0 - 2;
	int innervW = ww1+4;
	int innervH = hh3+5;

	OmnString framerhtml;
	framerhtml << "<div style=\"width: "<<gic_width<<"; height: "<<gic_height<<"; position: relative;\" name=\"htmlframer\">"
	<<	"<img class=\" x-abs-layout-item\" style=\"width: "<<ww5<<"; height: "<<hh0<<";\" name=\"left_top\" src=\"/lps-4.7.2/images/Framers/"<<src<<"/left_top.png\">"
	<<	"<img class=\" x-abs-layout-item\" style=\"width: "<<ww1<<"; height: "<<hh1<<"; left: "<<ww5<<"; top: 0px;\" name=\"top\" src=\"/lps-4.7.2/images/Framers/"<<src<<"/top.png\">"
	<<	"<img class=\" x-abs-layout-item\" style=\"width: "<<ww2<<"; height: "<<hh2<<"; left: "<<ww5+ww1<<"; top: 0px;\" name=\"right_top\" src=\"/lps-4.7.2/images/Framers/"<<src<<"/right_top.png\">"
	<<	"<img class=\" x-abs-layout-item\" style=\"width: "<<ww3<<"; height: "<<hh3<<"; left: 0px; top: "<<hh0<<";\" name=\"left\" src=\"/lps-4.7.2/images/Framers/"<<src<<"/left.png\">"
	<<	"<img class=\" x-abs-layout-item\" style=\"width: "<<ww4<<"; height: "<<hh4<<"; left: "<<ww5+ww1<<"; top: "<<hh0<<";\" name=\"right\" src=\"/lps-4.7.2/images/Framers/"<<src<<"/right.png\">"
	<<	"<img class=\" x-abs-layout-item\" style=\"width: "<<ww5<<"; height: "<<hh5<<"; left: 0px; top: "<<hh0+hh3<<";\" name=\"left_bottom\" src=\"/lps-4.7.2/images/Framers/"<<src<<"/left_bottom.png\">"
	<<	"<img class=\" x-abs-layout-item\" style=\"width: "<<ww6<<"; height: "<<hh6<<"; left: "<<ww5<<"; top: "<<hh0+hh3<<";\" name=\"bottom\" src=\"/lps-4.7.2/images/Framers/"<<src<<"/bottom.png\">"
	<<	"<img class=\" x-abs-layout-item\" style=\"width: "<<ww7<<"; height: "<<hh7<<"; left: "<<ww5+ww6<<"; top: "<<hh0+hh3<<";\" name=\"right_bottom\" src=\"/lps-4.7.2/images/Framers/"<<src<<"/right_bottom.png\">"
	<<	"<div style=\"position: absolute;background-color: transparent; width: "<<innervW<<"; height: "<<innervH<<"; left: "<<innervX<<"; top: "<<innervY<<";\"  name=\"innerview\" class=\"x-panel-body x-panel-body-noheader\">";
	code.mHtml << framerhtml ;
	code.mJson << "," ;
	code.mJson << "cmp_src : \'" << src<< "\'," ;
	code.mJson << "cmp_sizedef : \'" << srcsize<< "\'" ;
	return true;
}

bool	
AosGicHtmlFramer::getFramerCode(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		const OmnString &parentid,
		AosHtmlCode &code)
{
	
	//code.mHtml.setLength(code.mHtml.indexOf(code.mHtml.length(), '<', true));
	//AosXmlTagPtr framertag=vpd->getFirstChild("framer");
	generateCode(htmlPtr, vpd, obj, parentid, code);
//	code.mHtml.trim(5);
	return true;


}


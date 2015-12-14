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
#include "GICs/GicHtmlRank.h"

#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

// static AosGicPtr sgGic = new AosGicHtmlRank();

AosGicHtmlRank::AosGicHtmlRank(const bool flag)
:
AosGic(AOSGIC_HTMLRANK, AosGicType::eHtmlRank, flag)
{
}


AosGicHtmlRank::~AosGicHtmlRank()
{
}

bool	
AosGicHtmlRank::generateCode(
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
	
	mPath = htmlPtr->getImagePath();
//	mPath = "http://yunyuyan.com/lps-4.7.2/images/";

	//flag = full, show all star
	//flag = parts, show parts star
	OmnString flag = vpd->getAttrStr("gic_fullpart");
	OmnString ishow;
	ishow << vpd->getAttrStr("gic_showstars", "3.5");
	double value;
	u32 tlen = ishow.length();
	ishow.parseDouble(0, tlen, value);

	int itotal, ifull, ihalf, inull = 0;
	itotal = vpd->getAttrInt("gic_totalstars", 5);
	ihalf = value > (int)value ? 1:0;
	ifull = (int)value;
	if(flag == "full")
	{
		inull = itotal - ifull - ihalf;
	}


	OmnString fsrcbd = vpd->getAttrStr("gic_fstarsrcbd");
	OmnString hsrcbd = vpd->getAttrStr("gic_hstarsrcbd");
	OmnString nsrcbd = vpd->getAttrStr("gic_nstarsrcbd");

	OmnString fsrc = vpd->getAttrStr("gic_fstarsrc", "image_7/ao16385.jpg");
	OmnString hsrc = vpd->getAttrStr("gic_hstarsrc", "image_7/ao16383.jpg");
	OmnString nsrc = vpd->getAttrStr("gic_nstarsrc", "image_7/ao16384.jpg");

	OmnString src;
	if (fsrcbd != "" && obj)
		fsrc = obj->getAttrStr(fsrcbd, fsrc);
	if (hsrcbd != "" && obj)
		hsrc = obj->getAttrStr(hsrcbd, hsrc);
	if (nsrcbd != "" && obj)
		nsrc = obj->getAttrStr(nsrcbd, nsrc);

	//size by define origin image
	OmnString sizestyle = vpd->getAttrStr("gic_setsizestyle", "define"); //"oimage"
	int owidth = vpd->getAttrInt("gic_owidth", 32);
	int oheight = vpd->getAttrInt("gic_oheight", 32);
	//size by define yourself
	int dwidth = vpd->getAttrInt("gic_dwidth", 32);
	int dheight = vpd->getAttrInt("gic_dheight", 32);
	mWidth = dwidth;
	mHeight = dheight;
	if(sizestyle == "oimage")	
	{
		mWidth = owidth;
		mHeight = oheight;
	}


	OmnString htmlcode;
	for(int i=0; i<ifull; ++i)
	{
		htmlcode << createImageHtml(i, fsrc);
	}

	for(int i=ifull; i<ifull+ihalf; ++i)
	{
		htmlcode << createImageHtml(i, hsrc);
	}

	for(int i=ifull+ihalf; i<ifull+ihalf+inull; ++i)
	{
		htmlcode << createImageHtml(i, nsrc);
	}

	code.mHtml << htmlcode;

	code.mJson  
		<< ", gic_fstarsrcbd: \"" << fsrcbd 
		<< "\", gic_hstarsrcbd: \"" << hsrcbd 
		<< "\", gic_nstarsrcbd: \"" << nsrcbd 
		<< "\", gic_fullpart: \"" << flag 
		<< "\", gic_totalstars: " << itotal 
		<< ", gic_showstars: " << ishow 
		<< ", gic_fstarsrc: \"" << fsrc 
		<< "\", gic_hstarsrc: \"" << hsrc 
		<< "\", gic_nstarsrc: \"" << nsrc 
		<< "\", gic_setsizestyle: \"" << sizestyle 
		<< "\", gic_owidth: " << owidth 
		<< ", gic_oheight: " << oheight 
		<< ", gic_dwidth: " << dwidth 
		<< ", gic_dheight: " << dheight; 
	return true;
}


OmnString
AosGicHtmlRank::createImageHtml(const int num, const OmnString &src)
{
	OmnString str;
	str << "<img style=\"position: absolute; left:" << num * mWidth 
		<< "; width:" << mWidth << "; height: "<< mHeight << ";\" src=\""
		<< mPath << src
		<< "\" onload=\"pngImageLoad(this);\"/>";
	return str;
}




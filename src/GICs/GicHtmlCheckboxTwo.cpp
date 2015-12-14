//////////////////////////////////////////////////////////////////////////// //
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved. 
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
// Description:
// Modification History:
// 29/08/2010: Created by Brian 
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicHtmlCheckboxTwo.h"

#include "HtmlServer/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "HtmlUtil/HtmlUtil.h"
#include "HtmlModules/DclDb.h"
#include "Util/String.h"

AosGicHtmlCheckboxTwo::AosGicHtmlCheckboxTwo(const bool flag)
:
AosGic(AOSGIC_HTMLCHECKBOXTWO, AosGicType::eHtmlCheckBoxTwo, flag),
mData(0),
mMain(""),
mNext(""),
mHeadTip("")
{
}


AosGicHtmlCheckboxTwo::~AosGicHtmlCheckboxTwo()
{
}


bool	
AosGicHtmlCheckboxTwo::generateCode(
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

	//init member data
	OmnString str = "Choice will be show here!";
	mHeadTip = vpd->getAttrStr("tip",str);
	mMain = vpd->getAttrStr("main","main");
	mNext = vpd->getAttrStr("next","next");

	//get data form db 
	bool drslt = getData(vpd,obj,htmlPtr);
	if(!drslt)
	{
		OmnAlarm << "Failed to get data!" << enderr;
		return false;
	}

	bool hrslt = createHtmlCode(code);
	if(!hrslt)
	{
		OmnAlarm << "Failed to create html code1" << enderr;
		return false;
	}

	bool jrslt = createJsonCode(vpd, code);
	if(!jrslt)
	{
		OmnAlarm << "Failed to create json code!" << enderr;
		return false;
	}
    return true;
}


bool
AosGicHtmlCheckboxTwo::getData(
				AosXmlTagPtr &vpd,
				const AosXmlTagPtr &obj,
				const AosHtmlReqProcPtr &htmlPtr)
{
	//this function get data by db,the new htmlcheckboxtwo's
	//data is all from db.
	aos_assert_r(vpd, false);

    AosDclDb dcl;
	AosXmlTagPtr doc = dcl.retrieveData(htmlPtr, vpd, obj, "");
	if (doc)
	{
		mData = doc->getFirstChild("Contents");
		aos_assert_r(mData, false);
		return true;
	}
	return false;
}

bool
AosGicHtmlCheckboxTwo::createHtmlCode(AosHtmlCode &code)
{	
	//this function generate html code 
	//it contain two parts,one is the header,it will show
	//all choices; another is the body of the main choice.
	//the form is :
	//<div>
	//	<div>header</div>
	//	<div>body</div>
	//</div>
	
	code.mHtml << "<div>";

	//create header
	code.mHtml << "<div>"
			   << "<div>"
			   << "<label>"
	           << mHeadTip
			   << "</label>"
			   << "</div>"
			   << "</div>";

	//create body
	code.mHtml << "<div>";
	AosXmlTagPtr child;
	while(child = mData->getNextChild())
	{
		OmnString next = child->getAttrStr(mNext);
		OmnString label = child->getAttrStr(mMain);
		if (next == "")
		{
			code.mHtml << "<div>"
					   << "<input type=\"checkbox\" />"
					   << "<label>" << label << "</label>"
				   	<< "</div>";
		}
	}
	code.mHtml << "</div>";

	//there is a area for show next panel when mouse move over
	code.mHtml << "<div style=\"visibility:hidden;\" ></div></div>";
	return true;
}


bool
AosGicHtmlCheckboxTwo::createJsonCode(
		AosXmlTagPtr &vpd,
		AosHtmlCode &code)
{
	//this function create json code for client .
	
	code.mJson << ","
	           << "main : \""         << mMain                  << "\","
	           << "next : \""         << mNext                  << "\","
	           << "xml : \""         << mData->toString()       << "\"";

	return true;
}


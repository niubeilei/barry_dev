//////////////////////////////////////////////////////////////////////////// //
//// Copyright (C) 2005 // Packet Engineering, Inc. All rights reserved. 
//// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
// Description:
// Modification History:
// 07/26/2010: Created by Brian 
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicHtmlRadio.h"

#include "HtmlServer/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "HtmlUtil/HtmlUtil.h"
#include "Util/String.h"
#include "alarm_c/alarm.h"
#include "HtmlModules/DclDb.h"
#include "SearchEngineAdmin/SengAdmin.h"

#include <string.h>
#include <stdio.h>

AosGicHtmlRadio::AosGicHtmlRadio(const bool flag)
:
AosGic(AOSGIC_HTMLRADIO, AosGicType::eHtmlRadio, flag),
mData(0),
mDataSource(""),
mLabel(""),
mValue(""),
mTip(""),
mFontStyle(""),
mFontSize(""),
mColor(""),
mObj(""),
mDefRadio(""),
mId(""),
mCsp(0),
mRsp(0)
{
	mData = NULL ;
}


AosGicHtmlRadio::~AosGicHtmlRadio()
{
}


bool	
AosGicHtmlRadio::generateCode(
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
	//This function converts the VPD to json form
	
	//get tip form vpd
	mTip = vpd->getAttrStr("tip","");

	//get label from vpd
	mLabel = vpd->getAttrStr("label","name");

	//get value form vpd
	mValue = vpd->getAttrStr("value","value");

    //get font_style form vpd
	mFontStyle = vpd->getAttrStr("ptg_fstyle","normal");

    //get color form vpd
	mColor = vpd->getAttrStr("ptg_fgcolor","black");

    //get font_size form vpd
	mFontSize = vpd->getAttrStr("ptg_fsize","12");
	
	mWidth = vpd->getAttrStr("gic_width");
	
	mHeight = vpd->getAttrStr("gic_height");
	aos_assert_r(mWidth != "" && mHeight != "", false);

    //get obj form vpd
	mObj = vpd->getAttrStr("obj","");
    
	//get default checked radio form vpd
	mDefRadio = vpd->getAttrStr("def_radio");

	mRsp = vpd->getAttrInt("rsp",0);
	
	mCsp = vpd->getAttrInt("csp",0);
	//get data source from vpd
	mDataSource = vpd->getAttrStr("datasrc","vpd");
    if(mDataSource == "vpd")
	{
		//mData is this form
		//<items>
		//    <item name="xxx" value="xxx" tip="xxx"/>
		//    <item name="xxx" value="xxx" tip="xxx"/>
		//    <item name="xxx" value="xxx" tip="xxx"/>
		//    ......
		//    <item name="xxx" value="xxx" tip="xxx"/>
		//</items>
		mData = vpd->getFirstChild("items");
		aos_assert_r(mData,false);
	}
	else
	{
		//mData form data collect 
		//data collect has two forms: str_datacol ,db_datacol
		AosXmlTagPtr datacol = vpd->getFirstChild("datacol");
		aos_assert_r(datacol,false);
        OmnString gic_type = datacol->getAttrStr("gic_type");
		aos_assert_r(gic_type != "",false);

		//data from db_datacol
        if(gic_type == "db_datacol")
		{
			//data should be this form
			//<Contents>
			//    <item mLabel="xxx" mValue="xxx" mTip="xxx" /> 
			//    <item mLabel="xxx" mValue="xxx" mTip="xxx" />
			//    ......
			//    <item mLabel="xxx" mValue="xxx" mTip="xxx" />
			//</Contents>
		    AosDclDb dcl;
			AosXmlTagPtr doc = dcl.retrieveData(htmlPtr, vpd, obj, "");
			if(!doc)
			{
			     OmnAlarm << "Failed to retrieve the doc definition: " << vpd->toString() << enderr;
				 return false ;
			}
            else
			{
			     mData = doc->getFirstChild("Contents");
				 if(!mData)
				 {
                      OmnAlarm << "Failed to get the Contents Node form doc" << enderr;
					  return false ;
				 }
			}
		}
		else if(gic_type == "str_datacol")
		{
			//str_datacol should be this form
			//cmp_contents = 
			//"label|$|value|$|tip|$$|
			//"label|$|value|$|tip|$$|
			//"label|$|value|$|tip|$$|
			//"label|$|value|$|tip"
			OmnString cmp_entry_sep = datacol->getAttrStr("cmp_entry_sep","|$$|");
		    OmnString cmp_attr_sep = datacol->getAttrStr("cmp_field_sep","|$|");
			OmnString cmp_contents = datacol->getAttrStr("cmp_contents");
			OmnString content = "<content>" ;
			bool flag = true ;
			OmnString ss = "" ;
			ss << cmp_entry_sep << "," << cmp_attr_sep ;
			const char * split = ss.data();
			OmnString p = strtok ((char *)cmp_contents.data(), split); 
			while(p!=NULL)
			{
				if(flag)
				{
			        content << "<item "
						    << mLabel
							<< "=\""
							<< p
							<< "\" ";
					flag = false ;
				}
				else
				{
				    content << mValue
						    << "=\""
							<< p
						    << "\" />" ;
					flag = true ;
				}
				p = strtok(NULL,split);
			} 
			content << "</content>" ;
            AosXmlParser parser;
			mData = parser.parse(content, "" AosMemoryCheckerArgs);
			aos_assert_r(mData,false);
			mData = mData->getFirstChild();
			aos_assert_r(mData,false);
		}
	}
	doLayout(vpd,code);
    createJsonCode(vpd,obj,code);	
	return true;
}


bool
AosGicHtmlRadio::doLayout(
	                 const AosXmlTagPtr &vpd,
					 AosHtmlCode &code)
{
	// this function create html code
	if(mDataSource == "vpd")
	{
	    mLabel = "name" ;
		mValue = "value" ;
		mTip = "tip" ;
	}
	
	//1.get obj 
	OmnString valueAttr ;
	if((mObj != "")&&(mValue != ""))
	{
	    u32 siteid = 100;
	    OmnString ssid ;
	    u64 urldocid = 0;
	    AosXmlTagPtr objBind = AosSengAdmin::getSelf()->retrieveDocByObjid(siteid, ssid, urldocid, mObj);
	    if(objBind)
	    {
	        valueAttr = objBind->getAttrStr(mValue);
	    }
	}

	mId = AosGetHtmlElemId();
	AosXmlTagPtr content = mData->getFirstChild();
	if (!content)
	{
		OmnAlarm << "The vpd missing contents node" << enderr;
		return false;
	}
	code.mHtml << "<div style=\"position:relative;"
			   << "width:" << mWidth << "px;"
			   << "height:" << mHeight << "px;"
	           << "font-size:" << mFontSize        << "px;"
			   << setStyle(mFontStyle)          
			   << "color:"     << mColor           <<";\" >";
    while(content)
	{
	    OmnString left = content->getAttrStr("left");
	    OmnString top = content->getAttrStr("top");
        OmnString name = content->getAttrStr(mLabel);
        OmnString value = content->getAttrStr(mValue);
		OmnString tip = content->getAttrStr(mTip);
        code.mHtml << "<div style=\"position:absolute;"
		           << "left:"      << left             << ";" 
				   << "top:"       << top              << ";\""
				   << "tip=\""     << tip              << "\">"
				   << "<input type=\"radio\" "
				   << "name=\"" << mId << "\"";
        if((valueAttr == value) || (mDefRadio == value))
		{
		    code.mHtml << "flag=\"true\" "
				       << "checked " ;
		}
	    else
		{
			code.mHtml << "flag=\"false\" ";
		}
        code.mHtml << " value=\"" << value << "\">"
                   << "<label>"<< name << "</label>"
				   << "</div>" ;
		content = mData->getNextChild();
	}
	code.mHtml << "</div>";
	return true ;
}


bool	
AosGicHtmlRadio::createJsonCode(
		AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		AosHtmlCode &code)
{
    OmnString vabd = vpd->getAttrStr("value_bind","src");
    OmnString layout = vpd->getAttrStr("layout","row");
	OmnString gic_var_bd = vpd->getAttrStr("gic_var_bd", "gic_var_radio_bd");
	OmnString gic_var_count = vpd->getAttrStr("gic_var_count", "gic_var_radio_count");
	OmnString gic_other = vpd->getAttrStr("gic_other", "false");
	OmnString gic_olabel = vpd->getAttrStr("gic_olabel", "其他");
	OmnString def_radio = vpd->getAttrStr("def_radio");
    OmnString row,column;
	if (layout == "row")
	{
		row = vpd->getAttrStr("rcnum","4");
		aos_assert_r(row != "0",false);
		column = "0";
	}
	else
	{
		column = vpd->getAttrStr("rcnum","1");
		aos_assert_r(column != "0",false);
		row = "0";
	}
	code.mJson << ","
		       << "datasrc : \"" << mDataSource << "\", "
			   << "label:\"" << mLabel << "\", "
			   << "value:\"" << mValue << "\", "
			   << "tip:\"" << mTip << "\", "
			   << "row:" << row << ", "
			   << "column:" << column << ", "
			   << "csp:" << mCsp << ", "
			   << "rsp:" << mRsp << ", "
			   << "checked:\"" << mDefRadio << "\", "
			   << "layout:\"" << layout << "\", "
			   << "gic_other:\"" << gic_other << "\", "
			   << "gic_olabel:\"" << gic_olabel << "\", "
			   << "gic_var_bd:\"" << gic_var_bd << "\", "
			   << "gic_var_count:\"" << gic_var_count << "\", "
			   << "def_radio:\"" << def_radio << "\", "
			   << "nameId:\"" << mId << "\", "
			   << "vabd:\"" << vabd << "\" ";
	return true ;
}


	
OmnString
AosGicHtmlRadio::setStyle(const OmnString &style)
{
	OmnString str ;
	if(style == "normal")
	{
	     str << "font-weight:normal;";
		 str << "font-style:normal;";
	}
	else if(style == "plain")
	{
	     str << "font-weight:normal;";
		 str << "font-style:normal;";
	}
	else if(style == "bold")
	{
	     str << "font-weight:bold;";
		 str << "font-style:normal;";
	}
	else if(style == "italic")
	{
	     str << "font-weight:normal;";
		 str << "font-style:italic;";
	}
	else if(style == "bolditalic")
	{
	     str << "font-weight:bold;";
		 str << "font-style:italic;";
	}
	return str ;
}

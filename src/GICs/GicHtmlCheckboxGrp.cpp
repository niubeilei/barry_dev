//////////////////////////////////////////////////////////////////////////// //
//// Copyright (C) 2005 // Packet Engineering, Inc. All rights reserved. 
//// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
// Description:
// Modification History:
// 07/26/2010: Created by Brian 
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicHtmlCheckboxGrp.h"

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
#include "XmlUtil/Ptrs.h"

#include <string.h>
#include <stdio.h>
#include <map>

AosGicHtmlCheckboxGrp::AosGicHtmlCheckboxGrp(const bool flag)
:
AosGic(AOSGIC_HTMLCHECKBOXGRP, AosGicType::eHtmlCheckboxGrp, flag),
mData(0),
mDataSource(""),
mLabel(""),
mValue(""),
mTip(""),
mFontStyle(""),
mFontSize(""),
mColor(""),
mObj(""),
mDefCheckbox(""),
mCsp(0),
mRsp(0)
{
	mData = NULL ;
}


AosGicHtmlCheckboxGrp::~AosGicHtmlCheckboxGrp()
{
}


bool	
AosGicHtmlCheckboxGrp::generateCode(
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

    //get obj form vpd
	mObj = vpd->getAttrStr("obj","");
    
	//get default checked radio form vpd
	mDefCheckbox = vpd->getAttrStr("def_checkbox");
	
	mRsp = vpd->getAttrInt("rsp", 2);
	
	mCsp = vpd->getAttrInt("csp", 2);

	mOther = vpd->getAttrStr("gic_other","false");

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
			     OmnAlarm << "Failed to retrieve the doc definition: " 
					      << vpd->toString() << enderr;
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
	doLayout(vpd, code);
    createJsonCode(vpd, obj, code);	
	return true;
}


bool
AosGicHtmlCheckboxGrp::doLayout(
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
	    OmnString ssid;
		u64 urldocid = 0;
	    AosXmlTagPtr objBind = AosSengAdmin::getSelf()->retrieveDocByObjid(siteid, ssid, urldocid, mObj);
	    if(objBind)
	    {
	        valueAttr = objBind->getAttrStr(mValue);
	    }
	}
	AosXmlTagPtr content = mData->getFirstChild();
	if (!content)
	{
		OmnAlarm << "Vpd Missing body." << enderr;
		return false;
	}
	code.mHtml << "<div style=\"position:relative;"
	           << "font-size:" << mFontSize        << ";"
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
				   << "<input type=\"checkbox\" " ;
		if((valueAttr.findSubString(value, 0, false)!= -1) || 
		(mDefCheckbox.findSubString(value, 0, false)!= -1))
		{
		    code.mHtml << "flag=\"true\""
				       << " checked" ;
		}
	    else
		{
			code.mHtml << "flag=\"false\" ";
		}
        code.mHtml << " value=\"" << value << "\" style=\"float:left;\">"
				   << "<div style=\"width:5px;float:left;\">&nbsp;</div>"
                   << "<label style=\"float:left\">"<< name << "</label>"
				   << "</div>" ;
		content = mData->getNextChild();
	}
	code.mHtml << "</div>";
	return true ;
}


bool	
AosGicHtmlCheckboxGrp::createJsonCode(
		AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		AosHtmlCode &code)
{
    OmnString vabd = vpd->getAttrStr("value_bind","src");
    OmnString layout = vpd->getAttrStr("layout","row");
	OmnString gic_olabel = vpd->getAttrStr("gic_olabel", "其他");
	OmnString gic_var_bd = vpd->getAttrStr("gic_var_bd", "gic_var_checkbox_bd");
	OmnString gic_var_count = vpd->getAttrStr("gic_var_count", "gic_html_count");
    OmnString row,column;
	if (layout == "row")
	{
		row = vpd->getAttrStr("rcnum", "4");
		aos_assert_r(row != "0", false);
		column = "0";
	}
	else
	{
		column = vpd->getAttrStr("rcnum", "1");
		aos_assert_r(column != "0", false);
		row = "0";
	}
	code.mJson << ","
		       << "datasrc : \""         << mDataSource       << "\","
			   << "label:\""             << mLabel            << "\","
			   << "value:\""             << mValue            << "\","
			   << "tip:\""               << mTip              << "\","
			   << "row:"                 << row               << ","
			   << "column:"              << column            << ","
			   << "checked:\""           << mDefCheckbox      << "\","
			   << "csp:"                 << mCsp              << ","
			   << "rsp:"                 << mRsp              << ","
			   << "layout:\""            << layout            << "\","
			   << "gic_other:\""         << mOther            << "\","
			   << "gic_olabel:\""        << gic_olabel        << "\","
			   << "gic_var_bd:\""        << gic_var_bd        << "\","
			   << "gic_var_count:\""     << gic_var_count     << "\","
			   << "vabd:\""              << vabd              << "\"";
	return true ;
}


	
OmnString
AosGicHtmlCheckboxGrp::setStyle(const OmnString &style)
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

	
OmnString 
AosGicHtmlCheckboxGrp::getXml(const AosRandomRulePtr &rule)
{
	mRule = rule;

    OmnString xml, name, value,
			  nodename = rule->mNodeName;

	xml << "<" << nodename << " ";
	XmlGenRuleValueIter itr;
	for (itr = rule->mAttrs_r.begin(); itr != rule->mAttrs_r.end(); ++itr)
	{
		name = itr->first;
		value = (itr->second).getRandomValue();

		xml << name << "=\"" << value << "\" ";
	}
	for (itr = rule->mAttrs_o.begin(); itr != rule->mAttrs_o.end(); ++itr)
	{
		name = itr->first;
		value = (itr->second).getRandomValue();

		if (rand()%10 < 8)
		{
			xml << name << "=\"" << value << "\" ";
		}
	}
	xml << ">";
	map<OmnString, AosRandomRulePtr>::iterator nit;
	for(nit = rule->mNodes_r.begin();nit!= rule->mNodes_r.end();nit++)
	{
		xml << "<items>";
		int r = rand()%5;
		for(int i=0; i<r; i++)
		{
			xml << getXml(nit->second);
		}
		xml << "</items>";
	}
	xml << "</" << nodename << ">";
	return xml;
}



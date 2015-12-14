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
// 2011/02/23 Created by Phnix
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicHtmlCombox.h"

#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/StrParser.h"
#include "HtmlModules/DclDb.h"


AosGicHtmlCombox::AosGicHtmlCombox(const bool flag)
:
AosGic(AOSGIC_HTMLCOMBOX, AosGicType::eHtmlCombox, flag)
{
}


AosGicHtmlCombox::~AosGicHtmlCombox()
{
}


bool
AosGicHtmlCombox::generateDclData(
		const AosHtmlReqProcPtr &htmlPtr,
		const AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		const OmnString &cmb_text_bind,
		const OmnString &cmb_value_query_dcl,
		OmnString &sOptions)
{
	aos_assert_r(vpd, false);

	AosXmlTagPtr datacol = vpd->getFirstChild("datacol");
	aos_assert_r(datacol, false);
	
	OmnString dcl_type = datacol->getAttrStr("gic_type", "");
	if(dcl_type != "db_datacol") return false;
	
	if (cmb_text_bind.data()[0] != '@' || cmb_value_query_dcl.data()[0] != '@') return false;
	OmnString text_bind = cmb_text_bind.substr(1);
	OmnString value_bind = cmb_value_query_dcl.substr(1);
	
	AosDclDb dcl;
	AosXmlTagPtr doc = dcl.retrieveData(htmlPtr, vpd, obj, "");
	if(!doc)
	{
		OmnAlarm << "Failed to retrieve the doc definition: " << vpd->toString() << enderr;
		return false;
	}
		
	AosXmlTagPtr doc_cont = doc->getFirstChild("Contents");
	if(!doc_cont)
	{
		OmnAlarm << "Failed to get the Contents Node form doc" << enderr;
		return false;
	}
						
	OmnString text, value;
	set<OmnString> mTextSet;
	set<OmnString>::iterator itr;
	AosXmlTagPtr record = doc_cont->getFirstChild();
	while(record)
	{
		text = record->getAttrStr(text_bind);
		value = record->getAttrStr(value_bind);
		if (text != "" && value != "")
		{
			itr = mTextSet.find(text);
			if(itr == mTextSet.end())
			{
				mTextSet.insert(text);
				sOptions << "<option value=\"" << value << "\">" << text << "</option>";
			}
		}
		record = doc_cont->getNextChild();
	}
	return true;
}


bool
AosGicHtmlCombox::generateCmbData(
		const OmnString &cmb_entries,
		const OmnString &cmb_entry_type,
		OmnString &sOptions)
{
	AosStrSplit split;
	bool finished;
	OmnString parts[100];
	OmnString text, value;
	set<OmnString> mTextSet;
	set<OmnString>::iterator itr;

	int num = split.splitStr(cmb_entries.data(), "|$$|", parts, 100, finished);
	for (int i=0; i<num; i++)
	{
		OmnString each_entery[2];
		int n = split.splitStr(parts[i].data(), "|$|", each_entery, 2, finished);
		if (n != 2)
		{
			OmnAlarm << "data format error!";
			continue;
		}

		if (cmb_entry_type == "no")
		{
			text = each_entery[0];
			value = each_entery[0];
		}
		else
		{
			text = each_entery[0];
			value = each_entery[1];
		}
		
		if (text != "" && value != "")
		{
			itr = mTextSet.find(text);
			if(itr == mTextSet.end())
			{
				mTextSet.insert(text);
				sOptions << "<option value=\"" << value << "\">" << text << "</option>";
			}
		}
	}
	return true;
}

bool	
AosGicHtmlCombox::generateCode(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		const OmnString &parentid,
		AosHtmlCode &code)
{
	bool select_first = vpd->getAttrBool("select_first", false);
	
	OmnString sOptions;
	if(!select_first) sOptions << "<option style=\"color:#bbb;\" value=\"\">--------</option>";
	
	bool slc_dataC = vpd->getAttrBool("slc_dataC", false);
	OmnString cmb_entries = vpd->getAttrStr("cmb_entries");		//when don't use dataCol, cmb_entries will be effect
	OmnString cmb_entry_type = vpd->getAttrStr("cmb_entry_type", "nv");		
	OmnString cmb_text_bind = vpd->getAttrStr("cmb_text_bind");
	OmnString cmb_value_query_dcl = vpd->getAttrStr("cmb_value_query_dcl");
	if (slc_dataC)
	{
		generateDclData(htmlPtr, vpd, obj, cmb_text_bind, cmb_value_query_dcl, sOptions);
	}
	else
	{
		generateCmbData(cmb_entries, cmb_entry_type, sOptions);
	}
	
	OmnString gic_eleid = AosGetHtmlElemId();
	OmnString combox_cls_name, styleStr;
	combox_cls_name << AOSGIC_CLS_COMBOX << "-" << gic_eleid;
	
	OmnString cmb_fcolor = AosHtmlUtil::getWebColor(vpd->getAttrStr("cmb_fcolor"));
	if(cmb_fcolor != "") styleStr << "color:" << cmb_fcolor << ";";
	
	int cmb_fsize = vpd->getAttrInt("cmb_fsize", -1);
	if(cmb_fsize > 0) styleStr << "font-size:" << cmb_fsize << ";";
	
	OmnString cmb_fstyle = vpd->getAttrStr("cmb_fstyle");
	if (cmb_fstyle == "bold") {
		styleStr << "font-weight:bold;";
	} else if (cmb_fstyle == "italic") {
		styleStr << "font-style:italic;";
	} else if (cmb_fstyle == "bolditalic") {
		styleStr << "font-weight:bold;font-style:italic;";
	}
	
	OmnString bg_color = AosHtmlUtil::getWebColor(vpd->getAttrStr("bg_color"));
	if(bg_color != "") styleStr << "background-color:" << bg_color << ";";
	
	OmnString css;
	css << "." << combox_cls_name << "{" << styleStr << "}";
	
	OmnString gic_self_inputCls = vpd->getAttrStr("gic_self_inputCls");
	OmnString gic_self_selectCls = vpd->getAttrStr("gic_self_selectCls");
	
	int gic_width = vpd->getAttrInt("gic_width", 260);
	if(gic_width <= 20) gic_width = 20;
	int gic_height = vpd->getAttrInt("gic_height", 22);

	bool cmb_editable = vpd->getAttrBool("cmb_editable", false);
	bool cmb_newAdd = vpd->getAttrBool("cmb_newadd", false);

	OmnString html;
	html << "<select class=\"" << AOSGIC_CLS_COMBOX << " " << AOSGIC_CLS_COMBOX_SELECT << " " << combox_cls_name << " " << gic_self_selectCls << "\" "
		 << "style=\"width:" << gic_width << "px;height:" << gic_height << "px;\">" << sOptions << "</select>"
		 << "<input class=\"" << AOSGIC_CLS_COMBOX << " " << AOSGIC_CLS_COMBOX_INPUT << " " << combox_cls_name << " " << gic_self_inputCls << "\" "
		 << (!cmb_editable ? "readOnly" : "")
		 << " style=\"width:" << (gic_width - 17) << "px;height:" << gic_height << "px;line-height:" << (gic_height - 2) << "px;\">";

	OmnString cmb_dft_text = vpd->getAttrStr("cmb_dft_text", "");		// default text
	OmnString cmb_dft_value = vpd->getAttrStr("cmb_dft_value", "");		// default bind the display 
	OmnString cmb_value_query = vpd->getAttrStr("cmb_value_query", "");		

	OmnString json;
	json << ","
		 << "gic_eleid:\"" << gic_eleid << "\","
		 << "gic_self_inputCls:\"" << gic_self_inputCls << "\","
		 << "gic_self_selectCls:\"" << gic_self_selectCls << "\","
		 << "slc_dataC:" << slc_dataC << ","								// use the backend data or not
		 << "cmb_editable:" << cmb_editable << ","							// combox can edit or not
		 << "cmb_newAdd:" << cmb_newAdd << ","								// can add a new value or not
		 << "select_first:" << select_first << ","							// default select the first option
		 << "cmb_entries:\"" << cmb_entries << "\","						// data which don't use backend datacol
		 << "cmb_entry_type:\"" << cmb_entry_type << "\","					// "nv":name and value, "vo":nameonly, means name == value
		 << "cmb_dft_text:\"" << cmb_dft_text << "\","						// default text to display
		 << "cmb_dft_value:\"" << cmb_dft_value << "\","					// default value, which will display when combobox appear 默认数值
		 << "cmb_value_query:\"" << cmb_value_query << "\","				// data bind, like '@zky_gaga'
		 << "cmb_text_bind:\"" << cmb_text_bind << "\","					// bind dataCol name, to show
		 << "cmb_value_query_dcl:\"" << cmb_value_query_dcl << "\","		// bind dataCol value in backend 
		 << "cmb_fcolor:\"" << cmb_fcolor << "\","
		 << "cmb_fstyle:\"" << cmb_fstyle << "\","
		 << "cmb_fsize:" << cmb_fsize << ","
		 << "bg_color:\"" << bg_color << "\"";
	
	code.mHtml << html;
	code.mCss << css;
	code.mJson << json;
	return true;
}

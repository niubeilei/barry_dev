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
#include "GICs/GicHtmlComboxNew.h"

#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/StrParser.h"
#include "HtmlModules/DclDb.h"

AosGicHtmlComboxNew::AosGicHtmlComboxNew(const bool flag)
:
AosGic(AOSGIC_HTMLCOMBOXNEW, AosGicType::eHtmlComboxNew, flag)
{
}


AosGicHtmlComboxNew::~AosGicHtmlComboxNew()
{
}


bool	
AosGicHtmlComboxNew::generateCode(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		const OmnString &parentid,
		AosHtmlCode &code)
{
	AosXmlTagPtr datacol = vpd->getFirstChild("datacol");
	OmnString slc_dataC = vpd->getAttrStr("slc_dataC", "false");
	OmnString cmb_entry_type = vpd->getAttrStr("cmb_entry_type", "nv");		
	OmnString gic_querygic = vpd->getAttrStr("gic_querygic","false");

	OmnString text_query_dcl = vpd->getAttrStr("cmb_text_bind", "");
	OmnString value_query_dcl = vpd->getAttrStr("cmb_value_query_dcl", "");


	OmnString dft_show_indb = vpd->getAttrStr("cmb_dft_value", "");		// default bind the display 
	//OmnString gic_dftvbd = vpd->getAttrStr("gic_dftvbd", "");			// default bind the value

	OmnString dftFstLine = vpd->getAttrStr("fst_line", "");
	OmnString dcl_text, dcl_value, jsonArray, cmb_entries;
	//when don't use dataCol, cmb_entries will be effect

	OmnString mOverColor = vpd->getAttrStr("mouse_over_color", "#3399ff");
	OmnString mOutColor = vpd->getAttrStr("mouse_out_color", "white");
	OmnString pull_bg_color = vpd->getAttrStr("pull_bg_color", "white");
	OmnString text_color = vpd->getAttrStr("text_color", "black");
	OmnString text_bg_color = vpd->getAttrStr("text_bg_color", "white");

	map<OmnString, OmnString> tvmap;
	map<OmnString, OmnString>::iterator it;

	OmnString gic_width = vpd->getAttrStr("gic_width", "242");
	OmnString gic_height = vpd->getAttrStr("gic_height","22");
	OmnString cmb_editable = vpd->getAttrStr("cmb_editable", "false");
	OmnString dft_display_text = vpd->getAttrStr("cmb_dft_text", "");		// default text
	OmnString bind_to_obj = vpd->getAttrStr("cmb_value_query", "");		
	OmnString gic_lstn = vpd->getAttrStr("gic_lstn"); 
	OmnString cmb_newAdd = vpd->getAttrStr("cmb_newadd", "false");
	OmnString addStr = vpd->getAttrStr("addStr", "新增(nAdd)");
	if (dftFstLine != "")
	{
		jsonArray << "{text:'" << dftFstLine << "', value:''},"
			<< "{text: '----', value:'dftHLine'},";
	}
	else
	{
		/* this means if no dftFstLine,  the will be empty , no default.*/
		jsonArray = "";
	}

	if (slc_dataC == "true" && datacol)
	{
		OmnString dcl_type = datacol->getAttrStr("gic_type", "");
		if(dcl_type == "db_datacol")
		{
			AosDclDb dcl;
			AosXmlTagPtr doc = dcl.retrieveData(htmlPtr, vpd, obj, "");
			if(!doc)
			{
				OmnAlarm << "Failed to retrieve the doc definition: " << vpd->toString() << enderr;
			}
			else
			{
				AosXmlTagPtr doc_cont = doc->getFirstChild("Contents");
				if(!doc_cont)
				{
					OmnAlarm << "Failed to get the Contents Node form doc" << enderr;
					/* not return here, becuse it will return data below */
				}
				else
				{
					AosXmlTagPtr record = doc_cont->getFirstChild();
					while(record)
					{
						/* use xpath */
						/* use map to store text && value */
						/* in datacol mode, just use name/value mode, if wanna use nameonly mode,
						 * just let text_query_dcl be the same with value_query_dcl */
						bool Bool;
						dcl_text = record->xpathQuery(text_query_dcl, Bool, "");
						dcl_value = record->xpathQuery(value_query_dcl, Bool, "");
						if (dcl_text != "")
							tvmap.insert(pair<OmnString, OmnString>(dcl_text, dcl_value));
						record = doc_cont->getNextChild();
					}
					// time to create jasonArray
					for (it=tvmap.begin(); it!=tvmap.end(); it++)
					{
						if ((*it).second == "")
						{
							jsonArray << "{text:'" << (*it).first <<"', value:''},";
						}
						else
							jsonArray << "{text:'" << (*it).first <<"', value:'" << (*it).second << "'},";
					}

				}
			}
		}
	}
	else
	{ 
		cmb_entries = vpd->getAttrStr("cmb_entries", "");		
		AosStrSplit parser(cmb_entries, "|$$|");
		OmnString word;
		AosStrSplit split;
		bool finished;
		while ((word = parser.nextWord()) != "")
		{
			OmnString each_entery[2];
			int n = split.splitStr(word.data(), "|$|", each_entery, 2, finished);
			if (n != 2)
			{
				OmnAlarm << "data format error!";
			}
			if (cmb_entry_type == "no")
			{
				tvmap.insert(pair<OmnString, OmnString>(each_entery[0], each_entery[0]));
				jsonArray << "{text:'" << each_entery[0] <<"', value:'" << each_entery[0] << "'},";
			}
			else
			{
				tvmap.insert(pair<OmnString, OmnString>(each_entery[0], each_entery[1]));
				jsonArray << "{text:'" << each_entery[0] <<"', value:'" << each_entery[1] << "'},";
			}
		}
		/*
		if (cmb_newAdd == "true")
		{
		}
		*/
	}

	if (jsonArray.length()<2)
		jsonArray="";

	//jsonArray = jsonArray.substr(0, jsonArray.length()-2);
	//else

	// adjust the relatonship with dft_display_text, dft_show_indb
	bool isFromDb = false;
	OmnString dft_value_indb;
	OmnString dft_show_text, dft_show_value;
	if (dft_show_indb != "")
	{
		if (tvmap.count(dft_show_indb) == 1)
		{
			// means have dft_show_indb in db
			dft_show_text = dft_show_indb;
			dft_show_value = tvmap[dft_show_indb];
			isFromDb = true;
		}
	}
	else if (dft_display_text != "")
	{
		dft_show_text = dft_display_text;
	}

	OmnString dft_inputStyle;
	dft_inputStyle << "position:absolute; width:241px; height:21px; left:0px; padding: 1px 0 0 5px; border-width:1px 0 1px 1px;"
		 << "border-color:#7F9DB9; border-style:solid none solid solid;z-Index:2;";

	OmnString dft_imgStyle;
	dft_imgStyle <<  "position:absolute; border-style: none; padding: 0; left: 238px; margin: 0;"
		<< "display: inline; vertical-align: bottom; border-left-color:buttonface;"
		<< " background-repeat:no-repeat; width:21px;";

	OmnString inputStyle = vpd->getAttrStr("inputStyle", dft_inputStyle);
	OmnString imgStyle = vpd->getAttrStr("imgStyle", dft_imgStyle);

	code.mHtml << "<input type=\"text\"style=\"" << inputStyle <<"\"/>"
		<< "<img src=\"/lps-4.7.2/extjs/resources/images/dropdown.gif\" style=\"" << imgStyle << "\"/>";

	code.mJson << ",slc_dataC:\"" << slc_dataC	//use the backend data or not
		<< "\",cmb_entries:\"" << cmb_entries	//data which don't use backend datacol
		<< "\",cmb_editable:" << cmb_editable		// combox can edit or not
		<< ",cmb_entry_type:\"" << cmb_entry_type	// "nv":name and value, "vo":nameonly, means name == value
		<< "\",dft_show_indb:\"" << dft_show_indb	// default value, which will display when combobox appear 默认数值
		<< "\",dft_display_text:\"" << dft_display_text // default text to display
		<< "\",bind_to_obj :\"" << bind_to_obj // data bind, like '@zky_gaga'
		<< "\",text_query_dcl:\"" << text_query_dcl	// bind dataCol name, to show
		<< "\",value_query_dcl:\"" << value_query_dcl// bind dataCol value in backend 
		<< "\",cmb_newAdd:\"" << cmb_newAdd		// can add a new value or not
		<< "\",addStr:\"" << addStr
		<< "\",gic_querygic:" << gic_querygic
		<< ",gic_lstn:\"" << gic_lstn
		<< "\",jsonArray:\"" << jsonArray 
		<< "\",isFromDb:" << isFromDb
		<< ",dft_show_text:\"" <<dft_show_text 
		<< "\",dft_show_value:\"" <<dft_show_value
		<< "\",mOutColor:\"" <<mOutColor
		<< "\",mOverColor:\"" <<mOverColor
		<< "\",pull_bg_color:\"" <<pull_bg_color
		<< "\",text_color:\"" <<text_color
		<< "\",dftFstLine:\"" <<dftFstLine
		<< "\",text_bg_color:\"" <<text_bg_color
		<<"\"";

	return true;
}

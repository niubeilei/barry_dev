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
#include "GICs/GicHtmlList.h"

#include <boost/regex.hpp>
#include <iostream>
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/StrParser.h"
#include "HtmlModules/DataCol.h"
#include "HtmlModules/DclDb.h"
#include "HtmlModules/DclStr.h"


using boost::regex;
// static AosGicPtr sgGic = new AosGicHtmlList();

AosGicHtmlList::AosGicHtmlList(const bool flag)
:
AosGic(AOSGIC_HTMLLIST, AosGicType::eHtmlList, flag)
{
}


AosGicHtmlList::~AosGicHtmlList()
{
}


bool	
AosGicHtmlList::retrieveContainers(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		const OmnString &parentid,
		AosHtmlCode &code)
{
	// It retrieves containers from the list.
	// 1. Data Collector may have containers. 
	// 2. Actions may have containers.
// AosGic::retrieveContainers(htmlPtr, vpd, obj, parentid, code);
	AosDataColPtr dcl = AosDataCol::getInstance(vpd);
	if (!dcl) return true;
	return dcl->retrieveContainers(htmlPtr, vpd, obj, parentid, OmnString(""), code);
}


bool	
AosGicHtmlList::generateCode(
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
	//test
	
	OmnString grid_css_random;
	grid_css_random << "_" << AosGetHtmlElemId();

	OmnString gic_alert = vpd->getAttrStr("gic_alert","没有数据");
	OmnString gic_border = vpd->getAttrStr("gic_border","1");
	OmnString gic_padding = vpd->getAttrStr("gic_padding","1");
	OmnString gic_bordcolor = vpd->getAttrStr("gic_bordcolor","#3636FF");
	OmnString gic_hdbgcolor = vpd->getAttrStr("gic_hdbgcolor","#ffffff");
	OmnString gic_bdbgcolor = vpd->getAttrStr("gic_bdbgcolor","#ffffff");
	OmnString gic_mouseovercolor = vpd->getAttrStr("gic_mouseovercolor","#f2f2f2");
	OmnString gic_mousedowncolor = vpd->getAttrStr("gic_mousedowncolor","#f5ff96");
	OmnString gic_singlecolor = vpd->getAttrStr("gic_singlecolor","#f2f2f2");
	OmnString gic_evencolor = vpd->getAttrStr("gic_evencolor","#ffffff");
	OmnString gic_str_len = vpd->getAttrStr("gic_str_len","");
	OmnString gic_buildhead = vpd->getAttrStr("gic_buildhead","false");
	OmnString head_style= vpd->getAttrStr("head_style","normal");
 	OmnString head_aln = vpd->getAttrStr("head_aln","left");
 	OmnString head_color = vpd->getAttrStr("head_color","black");
 	OmnString head_size = vpd->getAttrStr("head_size","15");
 	OmnString head_height = vpd->getAttrStr("head_height","30");
 	OmnString head_weight = vpd->getAttrStr("head_weight","bold");
	OmnString gic_lstn = vpd->getAttrStr("gic_lstn","");
	//OmnString gic_height = vpd->getAttrStr("gic_height","");
	//OmnString gic_width = vpd->getAttrStr("gic_width","");
	OmnString gic_width= code.gic_width;		// Ketty 2011/09/23  for temp.
	OmnString gic_height = code.gic_height;
	OmnString gic_title = vpd->getAttrStr("gic_title","");
	OmnString gic_view = vpd->getAttrStr("gic_view","single");
	OmnString gic_expand_flag = vpd->getAttrStr("gic_expand_flag","false");
	OmnString gic_ensunique = vpd->getAttrStr("gic_ensunique","false");
	OmnString gic_mberid = vpd->getAttrStr("gic_mberid","");
	OmnString gic_cursor = vpd->getAttrStr("gic_cursor","auto");
	AosXmlTagPtr paging = vpd->getFirstChild("paging");
	OmnString showPaging = "false";
	if(paging)
	{
		showPaging = paging->getAttrStr("page_isshowpaging", "false");
	}
	int table_height = atoi(gic_height.data())-25;
	OmnString cssStr;
	cssStr << ".htmllist_table" << grid_css_random << "{width:100%;table-layout:fixed;overflow:hidden;border-collapse:collapse;"
		   << "cursor:" << gic_cursor << ";}"
	 	   << ".htmllist" << grid_css_random << "{overflow-y:auto;background:" << gic_bdbgcolor<<";";
	if(gic_border != "")
	{
		cssStr << "border:"<< gic_border <<"px solid;";
	}
	if(gic_bordcolor != "")
	{
		cssStr << "color:"<< gic_bordcolor << ";";
	}
	if(showPaging=="true")
	{
		cssStr << "height:"<< table_height << ";";
	}
	else
	{
		cssStr << "height:"<< gic_height << ";";
	}
	cssStr << "}";
	cssStr <<".htmllist_td" << grid_css_random <<"{background:" << gic_singlecolor << "; padding:0}"
	       <<".htmllist_td_alt" << grid_css_random <<"{background:"<< gic_evencolor  <<";padding: 0;}";
    cssStr <<".head" << grid_css_random <<"{background: "<< gic_hdbgcolor 
		   <<";font-size:" << head_size <<"px;padding-left:5px;padding-right:5px;color: "
		   << head_color << ";text-align:" << head_aln << ";height:"<< head_height <<"px;line-height:"<<head_height 
		   << "px;font-style:" << head_style << "; font-weight:"<< head_weight<<"}";
	cssStr <<".over" << grid_css_random <<"{background: "<< gic_mouseovercolor <<";}";
	cssStr <<".down" << grid_css_random <<"{background: "<< gic_mousedowncolor <<";}";
	cssStr <<".expand{background-image: url(\"/lps-4.7.2/extjs/resources/images/default/grid/sort_asc.gif\");background-repeat:no-repeat;height:20px;width:20px;cursor:pointer;}";
	cssStr <<".collapse{background-image: url(\"/lps-4.7.2/extjs/resources/images/default/grid/sort_desc.gif\");background-repeat:no-repeat;height:20px;width:20px;cursor:pointer;}";
	cssStr <<".collapse_tr" << grid_css_random <<"{display:none;}";
	code.mCss << cssStr;
	OmnString temp;
	OmnString data;
	OmnString gic_expand_cont;
	OmnString listHtmlCode;
	AosXmlTagPtr expand_cont = vpd->getFirstChild("expandcontent");
	if(expand_cont)
	{
		OmnString text_1 = expand_cont->getNodeText();
		text_1 = generateTemplate(text_1, htmlPtr);
		gic_expand_cont << text_1;
	}
	AosXmlTagPtr contents = vpd->getFirstChild("contents");
	if(contents)
	{
		OmnString text = contents->getNodeText();
		temp = generateTemplate(text, htmlPtr);
	}
	
	listHtmlCode << "<div class=\"htmllist" << grid_css_random << "\">"
				 << "<table class=\"htmllist_table" << grid_css_random << "\">";
	if(gic_buildhead == "true")
	{
		listHtmlCode << "<caption class=\"head" << grid_css_random <<"\">"
	    		     << gic_title <<"</caption>";
	}
	listHtmlCode << "<tbody>";

	AosDataColPtr dcl = AosDataCol::getInstance(vpd);
	if(!dcl)
	{
		return false;
	}
	AosXmlTagPtr doc = dcl->retrieveData(htmlPtr, vpd, obj, "");
	if(!doc)
	{
		OmnAlarm << "Failed to retrieve the doc definition: " << vpd->toString() << enderr;
//		listHtmlCode << "<tr><td>" << gic_alert << "</td></tr>";
	}
	else
	{
		AosXmlTagPtr doc_cont = doc->getFirstChild("Contents");
		if(!doc_cont)
		{
			OmnAlarm << "Failed to get the Contents Node form doc" << enderr;
//			listHtmlCode << "<tr><td>" << gic_alert << "</td></tr>";
		}
		else
		{
			data << doc_cont->toString();
			int flag = 0;
			AosXmlTagPtr record = doc_cont->getFirstChild();
			if(!record)
			{
				listHtmlCode << "<tr><td>" << gic_alert << "</td></tr>";
			}
			while(record)
			{
				flag++;
				OmnString tr;
				OmnString td;
				if(flag%2==0)
				{
					tr << "<tr class=\"htmllist_td_alt" << grid_css_random << "\" index = \"" 
					   << flag << "\"><td style=\"padding:" << gic_padding << "px\">";
				}
				else
				{
					tr << "<tr class=\"htmllist_td" << grid_css_random << "\" index = \"" 
					   << flag << "\"><td style=\"padding:"  << gic_padding << "px\">";
				}
				listHtmlCode << tr 
					         //<< "<div style=\"width:100%;white-space:normal;word-break:break-all;" 
					         << "<div style=\"width:100%;white-space:normal;" 
							 << "word-wrap:break-word;overflow:hidden;\">"
							 << createHtmlCode(temp, gic_str_len,record,htmlPtr) 
							 << "</div></td>";
				if(gic_expand_flag == "true")
				{
					listHtmlCode << "<td style=\"width:20px;\">"
					             << "<div class=\"collapse\" index =\"" << flag <<"\">"
						         << "</div></td>";
				}
				listHtmlCode << "</tr>";
				if(gic_expand_flag == "true")
				{
					listHtmlCode << "<tr class=\"collapse_tr" << grid_css_random << "\"><td colspan=\"2\">"
					             << "<div style=\"width:100%;overflow:hidden;\">" 
								 << createHtmlCode(gic_expand_cont, "no",record,htmlPtr)
						 	     << "</div></td></tr>";
				}
				record = doc_cont->getNextChild();
			}
		}
	}
	listHtmlCode << "</tbody></table></div>";
	AosHandleImagePtr handleImage;
	handleImage->processImage(listHtmlCode);

	code.mHtml << listHtmlCode;
	OmnString str=",";
	str	<< "gic_border:\"" << gic_border << "\"," 
		<< "gic_padding:\"" << gic_padding << "\","
		<< "gic_alert:\"" << gic_alert << "\","
		<< "gic_bordcolor:\"" << gic_bordcolor << "\"," 
		<< "gic_hdbgcolor:\"" << gic_hdbgcolor << "\","
		<< "gic_bdbgcolor:\"" << gic_bdbgcolor << "\","
		<< "gic_mouseovercolor:\"" << gic_mouseovercolor << "\","
		<< "gic_mousedowncolor:\"" << gic_mousedowncolor << "\","
		<< "gic_singlecolor:\"" << gic_singlecolor << "\","
		<< "gic_evencolor:\"" << gic_evencolor << "\","
		<< "gic_str_len:\"" << gic_str_len << "\","
		<< "vpdCssRandom:\"" << grid_css_random <<"\","
//		<< "gic_expand_cont:'" << gic_expand_cont <<"',"
		<< "gic_expand_flag:\"" << gic_expand_flag <<"\","
		//<< "gic_height:"<<gic_height<<","
		//<< "gic_width:"<<gic_width<<","
		<< "gic_buildhead:"<<gic_buildhead<<","
		<< "gic_ensunique:"<<gic_ensunique<<","
		<< "gic_mberid:\""<<gic_mberid<<"\","
		<< "gic_cursor:\""<< gic_cursor <<"\","
		<< "head_style:\""<<head_style<<"\","
		<< "head_weight:\""<<head_weight<<"\","
		<< "head_aln:\""<<head_aln<<"\","
		<< "head_color:\""<<head_color<<"\","
		<< "head_size:\""<<head_size<<"\","
		<< "head_height:\""<<head_height<<"\","
		<< "gic_lstn:\""<<gic_lstn<<"\","
		<< "gic_title:\"" << gic_title << "\","
		<< "gic_view:\"" << gic_view << "\","
//		<< "gic_cont:'" << temp << "',"
		<< "gic_data:'" << data << "',"
		<< "showPaging:" << showPaging;
	code.mJson << str;
	return true;
}

OmnString
AosGicHtmlList::createHtmlCode(const OmnString &temp,const OmnString &str_len, const AosXmlTagPtr &record,const AosHtmlReqProcPtr &htmlPtr)
{
	OmnString str = temp;
	
	int p1 = str.findSubString("${", 0);
	int p2 = str.findSubString("}",p1);
	while(p1 != -1 && p2 != -1)
	{
		OmnString cont = str.substr(p1+2, p2-1);
		//name:type:dft
		
		OmnString value;

		AosStrSplit split;
		OmnString parts[3];
		split.splitStr(cont, ":", parts, 3);

		if(parts[1] == "gen")
		{
			value << "${" << parts[0] << "}";
		}
		else if(parts[1] == "noreplace")
		{
			bool Bool;
			value = record->xpathQuery(parts[0], Bool, value);
			if(value == "")
			{
				value = parts[0];	
			}
		}
		else
		{
			bool Bool;
			value = record->xpathQuery(parts[0], Bool, parts[2]);
			value.replace("|&&|", "\"", true);
			value.replace("|&|", "\'", true);
		}

//		if(str_len!="no"&&str_len!="")
//		{
//			value = getNewValue(value,str_len);
//		}
		str.replace(p1, p2 - p1 + 1, value);

		p1 = str.findSubString("${", p1+1);
		p2 = str.findSubString("}",p1);
	}

	//felicia 2011/05/09 for readmore
	OmnStrParser1 parser("");
	parser.subHtml(str);
	
	if(!htmlPtr->isHomePage())
	{
		bool t = true;
		str.replace("\'", "\\'", t);
	}
	
	return str;
}

OmnString
AosGicHtmlList::getNewValue(OmnString &val,const OmnString &str_len)
{
	string value = val.data();
	string str_length = str_len.data();
	string r;
	r = "((((\\w+\\s)|([\\x80-\\xff]{3}))[^\\w\\x80-\\xff]*){"
	  + str_length
	  + "}).*";
	try
	{
		boost::regex reg(r);
		boost::smatch m;
		string rr = "$1...";
		value = boost::regex_replace(value, reg, rr);
	}
	catch(const boost::bad_expression& e)
	{
		cout << "That's not a valid regular expression!" << e.what() << endl;
	}
	catch(...)
	{
		cout << "regex error" << endl;
	}
	OmnString vv = OmnString(value.data());
    return vv;
}


OmnString
AosGicHtmlList::generateTemplate(OmnString &cont, const AosHtmlReqProcPtr &htmlPtr)
{
//    int findHandle = cont.findSubString("gAosHtmlHandler",0);
//	if(findHandle != -1)
//	{
//		htmlPtr->addGic("gAosHtmlHandler");//js file id;
//	}
	int f1 = cont.findSubString("|&&|", 0);
	int f2 = cont.findSubString("|&|", 0);
	int f3 = cont.findSubString("&lt;", 0);
	int f4 = cont.findSubString("&gt;", 0);
	if(f1 != -1)
	{
		cont.replace("|&&|", "\"", true);
	}
	if(f2 != -1){
//		if(htmlPtr->isHomePage())
//		{
			cont.replace("|&|", "\'", true);
//		}
//		else
//		{
//			cont.replace("|&|", "'", true);
//		}
	}
	if(f3 != -1){
	    cont.replace("&lt;", "<", true);
	}
    if(f4 != -1){
        cont.replace("&gt;", ">", true);
    }

	return cont;
}

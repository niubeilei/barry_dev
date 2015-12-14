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
// 2011/02/19: Created by cody
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicHtmlDataView.h"

#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/StrParser.h"
#include "HtmlModules/DataCol.h"
#include "HtmlModules/DclDb.h"
#include "HtmlModules/DclStr.h"
#include "HtmlUtil/HtmlUtil.h"
#include "HtmlServer/HtmlUtil.h"


AosGicHtmlDataView::AosGicHtmlDataView(const bool flag)
:
AosGic(AOSGIC_HTMLDATAVIEW, AosGicType::eHtmlDataView, flag)
{
}


AosGicHtmlDataView::~AosGicHtmlDataView()
{
}


bool	
AosGicHtmlDataView::generateCode(
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
	AosXmlTagPtr creators = vpd->getFirstChild("creators");
	AosXmlTagPtr datacol = vpd->getFirstChild("datacol");
	AosXmlTagPtr paging = vpd->getFirstChild("paging");
	OmnString gic_id2 = AosGetHtmlElemId();
	OmnString gic_width = vpd->getAttrStr("gic_width");
	OmnString gic_height = vpd->getAttrStr("gic_height");
	OmnString gic_left = vpd->getAttrStr("gic_left", "5");
	OmnString gic_top = vpd->getAttrStr("gic_top", "5");
	OmnString gic_bottom = vpd->getAttrStr("gic_bottom", "5");
	OmnString gic_hspace = vpd->getAttrStr("gic_hspace", "0");
	OmnString gic_vspace = vpd->getAttrStr("gic_vspace", "0");
	OmnString gic_hscroll = vpd->getAttrStr("gic_hscroll", "0");
	OmnString gic_vscroll = vpd->getAttrStr("gic_vscroll", "0");
	OmnString gic_right = vpd->getAttrStr("gic_right", "5");
	OmnString gic_fixwidth = vpd->getAttrStr("gic_fixwidth", "0");
	OmnString gic_fixheight = vpd->getAttrStr("gic_fixheight", "0");
	OmnString gic_bgColor = vpd->getAttrStr("gic_bgColor1");
	OmnString withoutBgc = vpd->getAttrStr("withoutBgc", "false");
	OmnString gic_overbgc = vpd->getAttrStr("gic_overbgc");
	OmnString gic_clickbgc = vpd->getAttrStr("gic_clickbgc");
	OmnString gic_rolling = vpd->getAttrStr("gic_rolling");
	OmnString gic_lstn = vpd->getAttrStr("gic_lstn");
	OmnString rows = vpd->getAttrStr("rows", "0");
	OmnString gic_handler = vpd->getAttrStr("gic_handler", "flase");
	OmnString columns = vpd->getAttrStr("columns", "0");
	OmnString gic_draggroup = vpd->getAttrStr("gic_draggroup");
	OmnString gic_dropgroup = vpd->getAttrStr("gic_dropgroup");
	OmnString gic_itemsmove = vpd->getAttrStr("gic_itemsmove", "0");
	OmnString gic_border = vpd->getAttrStr("gic_border", "0");
	OmnString gic_borCol = vpd->getAttrStr("gic_borCol", "#000000");
	OmnString selectMode = vpd->getAttrStr("selectMode", "1");
	OmnString keepRatio = vpd->getAttrStr("keepRatio", "0");
	OmnString trashid = vpd->getAttrStr("trashid");
	OmnString gic_iconl = vpd->getAttrStr("gic_iconl");
	OmnString useWay = vpd->getAttrStr("useWay", "4");
	OmnString clickMove = vpd->getAttrStr("clickMove");
	OmnString itemLeftBorder = vpd->getAttrStr("itemLeftBorder", "0");
	OmnString itemRightBorder = vpd->getAttrStr("itemRightBorder", "0");
	OmnString itemTopBorder = vpd->getAttrStr("itemTopBorder", "0");
	OmnString itemBottomBorder = vpd->getAttrStr("itemBottomBorder", "0");
	OmnString gic_borderCol = vpd->getAttrStr("gic_borderCol", "#000000");
	OmnString showAlert = vpd->getAttrStr("showAlert");
	OmnString checkdb = vpd->getAttrStr("checkdb");
	OmnString checked_val = vpd->getAttrStr("checked_val");
	OmnString deldb = vpd->getAttrStr("deldb");
	OmnString gic_mberid = vpd->getAttrStr("gic_mberid");
	OmnString gic_ensunique = vpd->getAttrStr("gic_ensunique");
	OmnString css, content, bodyHtml, dcl_type, html, content1, str, dataStr, endHtml;
	OmnString hscroll = "hidden", vscroll = "hidden";
	OmnString isPaging = "false", itemBorder, bgColor;
	int ddWidth = 0, ddHeight = 0, dataNum = 0, showNum, rr, cc, contentHeight, contentWidth;
	AosXmlTagPtr creator, contents, doc_cont;
	AosHandleImagePtr handleImage;
	if(paging)
	{
		isPaging = paging->getAttrStr("page_isshowpaging", "false");
	}
	contentHeight = gic_height.toInt() - gic_top.toInt() - gic_bottom.toInt();
	contentWidth = gic_width.toInt() - gic_left.toInt() - gic_right.toInt();
	if(creators)
	{
		creator = creators->getFirstChild("creator");
		if(creator)
		{
			contents = creator->getFirstChild("contents");
		}
		if(contents)
		{
			content = contents->getNodeText();
			content1 = handdleStartImg(content, htmlPtr);
			if(!(content1 == content))
			{
				contents->setNodeText("", content1, true);
			}
			content = content1;
			generateTemplate(content, htmlPtr);		
		}
	}
	bodyHtml << "<div class=\"content\">";
	bodyHtml << "<ul class=\"dd\">";
	bool flag = AosGicHtmlDataView::isInt(rows);
	if(flag)
	{
		rr = rows.toInt();
	}
	else
	{
		rr = 0;
	}
	flag = AosGicHtmlDataView::isInt(columns);
	if(flag)
	{
		cc = columns.toInt();
	}
	else
	{
		cc = 0;
	}
	showNum = cc * rr;
	if(gic_rolling == "1" || gic_rolling == "2")
	{
		showNum = 0;
	}
	if(datacol)
	{
		dcl_type = datacol->getAttrStr("gic_type");
		AosXmlTagPtr doc;
		AosDataColPtr dcl = AosDataCol::getInstance(vpd);
		if(!dcl)
		{
			return false;
		}
		doc= dcl->retrieveData(htmlPtr, vpd, obj, "");
		if(doc)
		{
			doc_cont = doc->getFirstChild("Contents");
			//lynch 2011/0423
			if(doc_cont)
			{
				AosXmlTagPtr record = doc_cont->getFirstChild();
				while(record)
				{
					if(showNum == 0 || showNum > dataNum)
					{
						OmnString preHtml = createHtmlCode(content, record);
						bodyHtml << "<li class=\"item\" style=\"list-style:none outside none;\">" << preHtml << "</li>";
						record = doc_cont->getNextChild();
						dataNum++;
					}
					else
					{
						break;
					}
				}
			}
		}		
	}
	if(gic_hscroll == "1")
	{
		hscroll = "auto";
	}	
	if(gic_vscroll == "1")
	{
		vscroll = "auto";
	}
	int itemWidth = (gic_fixwidth.toInt() + 6 + gic_vspace.toInt()),
		itemHeight = (gic_fixheight.toInt() + 6 + gic_hspace.toInt());
	
	if(itemLeftBorder == "0")
	{
		itemBorder << "";
	}
	else if(itemLeftBorder == "1")
	{	
		itemWidth++;
		itemBorder << "border-left:" << gic_borderCol << " 1px solid;";
	}
	else if(itemLeftBorder == "2")
	{
		itemWidth++;
		itemBorder <<  "border-left:" << gic_borderCol << " 1px dotted;";	
	}
	if(itemRightBorder == "0")
	{
		itemBorder << "";
	}
	else if(itemRightBorder == "1")
	{	
		itemWidth++;
		itemBorder << "border-right:" << gic_borderCol << " 1px solid;";
	}
	else if(itemRightBorder == "2")
	{
		itemWidth++;
		itemBorder <<  "border-right:" << gic_borderCol << " 1px dotted;";	
	}
	if(itemTopBorder == "0")
	{
		itemBorder << "";
	}
	else if(itemTopBorder == "1")
	{	
		itemHeight++;
		itemBorder << "border-top:" << gic_borderCol << " 1px solid;";
	}
	else if(itemTopBorder == "2")
	{
		itemHeight++;
		itemBorder <<  "border-top:" << gic_borderCol << " 1px dotted;";	
	}
	if(itemBottomBorder == "0")
	{
		itemBorder << "";
	}
	else if(itemBottomBorder == "1")
	{	
		itemHeight++;
		itemBorder << "border-bottom:" << gic_borderCol << " 1px solid;";
	}
	else if(itemBottomBorder == "2")
	{
		itemHeight++;
		itemBorder <<  "border-bottom:" << gic_borderCol << " 1px dotted;";	
	}
	
	if(rr != 0 && cc != 0)
	{
		ddWidth = cc * itemWidth;
		ddHeight = rr * itemHeight;
	}
	else if(rr != 0 && cc == 0)
	{
		cc = dataNum / rr;
		if((dataNum%rr) != 0)
		{
			cc++;
		}
		ddWidth = cc * itemWidth;
		ddHeight = rr * itemHeight;
	}
	else if(rr == 0 && cc != 0)
	{
		ddWidth = cc * itemWidth;
	}
	else
	{
		ddWidth = 0;
		ddHeight = 0;
	}
	if((dataNum * itemWidth) < gic_width.toInt() && ddWidth != 0)
	{
		hscroll = "hidden";
	}
	if((dataNum * itemHeight) < gic_height.toInt() && ddHeight != 0)
	{
		vscroll = "hidden";
	}
	if(gic_rolling == "2" || gic_rolling == "1")
	{
		//rolling
		hscroll = "hidden";
		vscroll = "hidden";
		isPaging = "false";
	}
	else
	{
		if(isPaging == "true")
		{
			contentHeight -= 20;
		}
	}
	if(gic_rolling == "2")
	{
		//horizol
		gic_height = "";
		gic_height << (gic_fixheight.toInt() + 6 + gic_hspace.toInt() 
						+ gic_top.toInt() + gic_bottom.toInt());
	}
	else if(gic_rolling == "1")
	{
		//vertical
		gic_width = "";
		gic_width << (gic_right.toInt() + gic_left.toInt() 
						+ gic_fixwidth.toInt() + 6 + gic_vspace.toInt());
	}
	if(contentHeight < 0)
	{
		contentHeight = 1;
	}
	contentWidth = contentWidth > 0 ? contentWidth : 1;
	bodyHtml << "</ul>";	
	if(dataNum == 0 && showAlert != "")
	{
		bodyHtml << "<div id=\"emptyMsg\" style=\"z-index:1;position:absolute;top:5px;\">" << showAlert << "</div>";
	}
	bodyHtml << "</div>";
	gic_bgColor = AosHtmlUtil::getWebColor(gic_bgColor);
	gic_overbgc = AosHtmlUtil::getWebColor(gic_overbgc);
	gic_clickbgc = AosHtmlUtil::getWebColor(gic_clickbgc);
	gic_borCol = AosHtmlUtil::getWebColor(gic_borCol);
	bgColor = gic_bgColor;
	html << bodyHtml;
	if(withoutBgc == "true")
	{
		bgColor = "transparent";
	}
	//css start
	css << ".a" << gic_id2 << "{font-size:12px;height:" << gic_height << "px;width:" << gic_width << "px;}"
		<< ".a" << gic_id2 << " div.content{height:" << contentHeight << "px;width:" << contentWidth 
		<< "px;background-color:" << bgColor << ";overflow-x:" << hscroll << ";overflow-y:" 
		<< vscroll << ";padding-left : " << gic_left << "px;padding-bottom:" << gic_bottom 
		<< "px;padding-top:" << gic_top << "px;padding-right:" << gic_right << "px;";
	if(gic_border == "1")
	{
		css << "border:1px solid " << gic_borCol << ";";
	}
	else if(gic_border == "2")
	{
		css << "border:3px solid " << gic_borCol << ";";	
	}
	else if(gic_border == "3")
	{
		css << "border:1px dotted " << gic_borCol << ";";
	}
	else if(gic_border == "4")
	{
		css << "border:3px dotted " << gic_borCol << ";";	
	}
	css << "}";
	css	<< ".a" << gic_id2 << " div.content ul.dd{margin:0px;padding:0px;list-style:none outside none;overflow:hidden;";
	if(gic_rolling  == "2")
	{
		css << "height:100%;width:4000%}";
	}
	else if(ddWidth != 0 && gic_rolling != "1")
	{
		css << "width:" << ddWidth << "px;";
		if(ddHeight != 0)
		{
			css << "height:" << ddHeight << "px;}";
		}
		else
		{
			css << "}";
		}
	}
	else if(gic_rolling == "1")
	{
		css << "width:100%;}";
	}
	else
	{
		css << "width:100%;height:100%;}";
	}
	css	<< ".a" << gic_id2 << " div.content ul.dd li.item{display:inline;list-style:none outside none;margin-bottom:" 
				<< gic_hspace << "px;"
				<< "margin-left:" << gic_vspace << "px;padding:3px;float:left;";
	if(gic_handler == "true")
	{
		css << "cursor:pointer;";
	}
	css << itemBorder; 
	css	<< "}";
	css << ".a" << gic_id2 << " div.content ul.dd .hover{background-color:" << gic_overbgc << ";}"; 
	css << ".a" << gic_id2 << " div.content ul.dd .click{background-color:" << gic_clickbgc << ";}";
	//css end
	handleImage->processImage(html);
	endHtml << " class=\"a" << gic_id2 << "\">" << html;
	str = code.mHtml;
	if(doc_cont)
	{
		dataStr = doc_cont->toString();
	}
	string ss(str.data()); 
	int last = ss.find_last_of(">");
	code.mHtml = "";
	code.mHtml << ss.substr(0, last);
	code.mHtml << endHtml;
	code.mJson << ","
			   << "className : \"a" << gic_id2 << "\","
			   << "gic_left : \"" << gic_left << "\","
			   << "gic_right : \"" << gic_right << "\","
			   << "gic_top : \"" << gic_top << "\","
			   << "gic_bottom : \"" << gic_bottom << "\","
			   << "gic_fixheight : \"" << gic_fixheight << "\","
			   << "gic_fixwidth : \"" << gic_fixwidth << "\","
			   << "gic_vspace : \"" << gic_vspace << "\","
			   << "gic_hspace : \"" << gic_hspace << "\","
			   << "gic_vscroll : \"" << gic_vscroll << "\","
			   << "gic_hscroll : \"" << gic_hscroll << "\","
			   << "gic_rolling : \"" << gic_rolling << "\","
			   << "data : \'" << dataStr << "\'," 
			   << "gic_lstn : \'" << gic_lstn << "\'," 
			   << "gic_overbgc : \'" << gic_overbgc << "\'," 
			   << "gic_clickbgc : \'" << gic_clickbgc << "\'," 
			   << "gic_lstn : \'" << gic_lstn << "\'," 
			   << "gic_bgColor1 : \'" << gic_bgColor << "\'," 
			   << "rows : \'" << rows << "\'," 
			   << "columns : \'" << columns << "\'," 
			   << "dataNum : \'" << dataNum << "\'," 
			   << "showNum : \'" << showNum << "\',"
			   << "gic_handler : \"" << gic_handler << "\","
			   << "isPaging : \'" << isPaging << "\'," 
			   << "gic_border : \'" << gic_border << "\'," 
			   << "gic_borCol : \'" << gic_borCol << "\'," 
			   << "gic_draggroup : \'" << gic_draggroup << "\'," 
			   << "gic_dropgroup : \'" << gic_dropgroup << "\'," 
			   << "gic_itemsmove : \'" << gic_itemsmove << "\'," 
			   << "withoutBgc : \'" << withoutBgc << "\'," 
			   << "selectMode : \'" << selectMode << "\'," 
			   << "keepRatio : \'" << keepRatio << "\'," 
			   << "trashid : \'" << trashid << "\'," 
			   << "gic_iconl : \'" << gic_iconl << "\'," 
			   << "useWay : \'" << useWay << "\'," 
			   << "clickMove : \'" << clickMove << "\'," 
			   << "itemLeftBorder : \'" << itemLeftBorder << "\'," 
			   << "itemRightBorder : \'" << itemRightBorder << "\'," 
			   << "itemTopBorder : \'" << itemTopBorder << "\'," 
			   << "itemBottomBorder : \'" << itemBottomBorder << "\'," 
			   << "gic_borderCol : \'" << gic_borderCol << "\'," 
			   << "showAlert : \'" << showAlert << "\'," 
			   << "deldb : \'" << deldb << "\'," 
			   << "checkdb : \'" << checkdb << "\'," 
			   << "checked_val : \'" << checked_val << "\'," 
			   << "gic_mberid : \'" << gic_mberid << "\'," 
			   << "gic_ensunique : \'" << gic_ensunique << "\'," 
			   << "content : \"" << content1 << "\"";
	code.mCss << css;
	return true;
}

OmnString AosGicHtmlDataView::handdleStartImg(const OmnString &html, const AosHtmlReqProcPtr &htmlPtr)
{
	string str(html.data());
	//this is for src="http://..."
	//string r = "(<img[^><]+src\\s*=\\s*[\'\"]?)(\\/[\\w\\.\\/]+)([\'\"]?[^><]*>)";
	string r = "(<img[^><]+src\\s*=\\s*((\\|&\\|)|(\\|&&\\|))?)(\\/[\\w\\.\\/\\$]+)(((\\|&\\|)|(\\|&&\\|))?[^><]*>)";
	OmnString dd = html;
	string ss(htmlPtr->getImagePath().data());
	if(ss.length()>1)
	{
		ss = ss.substr(0, ss.length()-1);
	}
	ss = "$1" + ss + "$5$6";
	try
	{
		boost::regex reg(r);
		str = boost::regex_replace(str, reg, ss);
		dd = "";
		dd << str;
	}
	catch(const boost::bad_expression& e)
	{
		OmnAlarm << "That's not a valid regular expression!" << enderr;
	}
	return dd;
}

bool AosGicHtmlDataView::isInt(const OmnString &temp)
{
	const char* carr = temp.data();
	int len = temp.length();
	for(int i = 0; i < len; i++)
	{
		if(carr[i] >= '0' && carr[i] <= '9')
		{
			continue;
		}
		else
		{
			return false;
		}
	}
	return true;
}

OmnString
AosGicHtmlDataView::createHtmlCode(const OmnString &temp, const AosXmlTagPtr &record)
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
			value = getRecordValue(record, parts[0], value);
			if(value == "")
			{
				value = parts[0];	
			}
		}
		else
		{
			value = getRecordValue(record, parts[0], parts[2]);
		}

		str.replace(p1, p2 - p1 + 1, value);

		p1 = str.findSubString("${", p1+1);
		p2 = str.findSubString("}",p1);
	}
	return str;
}

OmnString
AosGicHtmlDataView::getRecordValue(const AosXmlTagPtr &record, const OmnString &path, OmnString &dft)
{
	OmnString str;
	AosXmlTagPtr rec = record;
	OmnStrParser1 parser(path, "/", false, false);
	OmnString p;
	while ( rec && (p = parser.nextWord()) != "")
	{
		if(p.data()[0] == '@')
		{
			p = p.substr(1, p.length()-1);
			str = rec->getAttrStr(p, dft);
		}
		else if(p.indexOf("_$text", 0) != -1)
		{
			str = rec->getNodeText();
		}
		else
		{
			rec = rec->getFirstChild(p);
		}
	}
	return str;
}

OmnString
AosGicHtmlDataView::generateTemplate(OmnString &cont, const AosHtmlReqProcPtr &htmlPtr)
{
    //int findHandle = cont.findSubString("gAosHtmlHandler",0);
	//if(findHandle != -1)
	//{
	//	htmlPtr->addGic("gAosHtmlHandler");//js file id;
	//}
	int f1 = cont.findSubString("|&&|", 0);
	int f2 = cont.findSubString("|&|", 0);
	if(f1 != -1)
	{
		cont.replace("|&&|", "\"", true);
	}
	if(f2 != -1){
		cont.replace("|&|", "\'", true);
	}
	if(!htmlPtr->isHomePage())
	{
		bool t = true;
		cont.replace("\'", "\\'", t);
	}
	return cont;
}

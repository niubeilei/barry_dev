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
// 2011/02/19: Created by Wynn 
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicHtmlGroupList.h"

#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/StrParser.h"
#include "HtmlModules/DclDb.h"

// static AosGicPtr sgGic = new AosGicHtmlGroupList();

AosGicHtmlGroupList::AosGicHtmlGroupList(const bool flag)
:
AosGic(AOSGIC_HTMLGROUPLIST, AosGicType::eHtmlGroupList, flag)
{
}


AosGicHtmlGroupList::~AosGicHtmlGroupList()
{
}


bool	
AosGicHtmlGroupList::generateCode(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		const OmnString &parentid,
		AosHtmlCode &code)
{
	OmnString grid_css_random;
	grid_css_random << "_" << AosGetHtmlElemId();

	OmnString gic_border = vpd->getAttrStr("gic_border","1");
	OmnString gic_bordcolor = vpd->getAttrStr("gic_bordcolor","#3636FF");
	OmnString gic_hdbgcolor = vpd->getAttrStr("gic_hdbgcolor","#ffffff");
	OmnString gic_bdbgcolor = vpd->getAttrStr("gic_bdbgcolor","#ffffff");
	OmnString gic_mouseovercolor = vpd->getAttrStr("gic_mouseovercolor","#f2f2f2");
	OmnString gic_mousedowncolor = vpd->getAttrStr("gic_mousedowncolor","#f5ff96");
	OmnString gic_singlecolor = vpd->getAttrStr("gic_singlecolor","#f2f2f2");
	OmnString gic_evencolor = vpd->getAttrStr("gic_evencolor","#ffffff");
	OmnString gic_morebt = vpd->getAttrStr("gic_morebt","true");
	OmnString gic_str_len = vpd->getAttrStr("gic_str_len","200");
	OmnString gic_autoheight = vpd->getAttrStr("gic_autoheight","false");
	OmnString gic_autowidth = vpd->getAttrStr("gic_autowidth","false");
	OmnString gic_buildhead = vpd->getAttrStr("gic_buildhead","false");
	OmnString head_style= vpd->getAttrStr("head_style","normal");
 	OmnString head_aln = vpd->getAttrStr("head_aln","left");
 	OmnString head_color = vpd->getAttrStr("head_color","black");
 	OmnString head_size = vpd->getAttrStr("head_size","15");
 	OmnString head_weight = vpd->getAttrStr("head_weight","bold");
	OmnString gic_lstn = vpd->getAttrStr("gic_lstn","");
	OmnString gic_height = vpd->getAttrStr("gic_height","");
	if(gic_autoheight == "true")
	{
		OmnString ss;
		ss << "height:" << gic_height << "px;";
		code.mHtml.replace(ss,"height:auto;",false);
	}
	OmnString str=",";
	OmnString cssStr;
	cssStr << ".group_table" << grid_css_random << "{width:100%;table-layout:fixed;overflow:hidden;background-color:transparent;border-collapse:collapse;cursor:pointer;}"
	 	   << ".groups" << grid_css_random << "{width:100%;height:100%;overflow:hidden;background:" << gic_bdbgcolor<<";";
	if(gic_border != "")
	{
		cssStr << "border:"<< gic_border <<"px solid;";
	}
	if(gic_bordcolor != "")
	{
		cssStr << "color:"<< gic_bordcolor << ";";
	}
	cssStr << "}";
	if(gic_singlecolor != "" && gic_evencolor != "")
	{
		 cssStr <<".grouplist_td" << grid_css_random <<"{background:" << gic_singlecolor << ";font-size:15px;color: #4f6b72; padding:0}"
		        <<".grouplist_td_alt" << grid_css_random <<"{background:"<< gic_evencolor  <<";font-size:15px;padding: 0;}";
	}
    cssStr <<".head" << grid_css_random <<"{background: "<< gic_hdbgcolor 
		   <<";font-size:" << head_size << "px;color: "
		   << head_color << ";text-align:" << head_aln 
		   << ";font-style:" << head_style << "; font-weight:"<< head_weight<<";}";
	if(gic_mouseovercolor != "")
	{
		 cssStr <<".over" << grid_css_random <<"{background: "<< gic_mouseovercolor <<";}";
	}
	if(gic_mousedowncolor != "")
	{
		 cssStr <<".down" << grid_css_random <<"{background: "<< gic_mousedowncolor <<";}";
	}
	code.mCss << cssStr;
	AosXmlTagPtr groups = vpd->getFirstChild("creators");
	if(!groups)	return false;
	AosXmlTagPtr group = groups->getFirstChild();
	if(!group)	return false;
	int i=0;
	int group_index = 0;
//	OmnString contents[20];
	OmnString listHtmlCode;
	OmnString cmp;
	OmnString conts;
	OmnString datas;
	OmnString objids;
	OmnString titles;
	listHtmlCode << "<div class=\"groups" << grid_css_random << "\">";
	while(group)
	{
		group_index++;
		OmnString iii = group->getAttrStr("objid","");
		objids << iii <<"[sep]";
		AosXmlTagPtr content = group->getFirstChild("contents");
		OmnString temp;
		if(content)
		{
			OmnString text = content->getNodeText();
			conts << text << "[sep]";
			temp = generateTemplate(text, htmlPtr);
		}
		OmnString title = group->getAttrStr("title","");
		titles << title <<"[sep]";
		OmnString tt = title;
		if(gic_buildhead == "false")
		{
			tt = "";
		}
		listHtmlCode << "<div class=\"" << group_index << "_group" << grid_css_random << "\">"
					 << "<table class=\"group_table" << grid_css_random << "\"><thead></thead>"
					 << "<tbody>";
/*			tt << "<div style=\"font-size:" << head_size 
			   << "px;font-weight:" << head_style << ";text-align:"
			   <<head_aln <<";color:"<< head_color << ";\">"
			   << title << "</div>";
			   */
		listHtmlCode <<"<tr><th class=\"head" << grid_css_random <<"\">"
	    			 << tt <<"</th></tr>";
		AosXmlTagPtr datacol = group->getFirstChild("datacol");
		if(datacol)
		{
			OmnString dcl_type = datacol->getAttrStr("gic_type","");
			if(dcl_type == "db_datacol")
			{
				OmnString cmp_tname = datacol->getAttrStr("cmp_tname");
				OmnString cmp_query = datacol->getAttrStr("cmp_query");
				OmnString cmp_order = datacol->getAttrStr("cmp_order");
			    OmnString cmp_fnames = datacol->getAttrStr("cmp_fnames");
			    OmnString cmp_psize = datacol->getAttrStr("cmp_psize","20");
				OmnString cmp_nobuffer = datacol->getAttrStr("cmp_nobuffer","false");
				cmp << "cmp_tname:" << cmp_tname << ","
				    << "cmp_query:" << cmp_query << ","
			        << "cmp_order:" << cmp_order << ","
			        << "cmp_fnames:" << cmp_fnames << ","
			        << "cmp_psize:" << cmp_psize << ","
			        << "cmp_nobuffer:" << cmp_nobuffer <<"[sep]" ;

				AosDclDb dcl;
				AosXmlTagPtr doc = dcl.retrieveData(htmlPtr, group, obj, "");

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
					}
					else
					{
						datas << doc_cont->toString() << "[sep]";
						int flag = 0;
						AosXmlTagPtr record = doc_cont->getFirstChild();
						while(record)
						{
							flag++;
							OmnString tr;
							OmnString td;
							td << "<td class=\"normal" << grid_css_random << "\" index = \"" << flag << "\">";
							if(flag%2==0)
							{
								tr << "<tr class=\"grouplist_td_alt" << grid_css_random << "\">" << td;
							}
							else
							{
								tr << "<tr class=\"grouplist_td" << grid_css_random <<"\">" << td;
							}
							listHtmlCode << tr << "<div style=\"white-space:normal;word-break:break-all;word-wrap:break-word;\">"
										 << createHtmlCode(temp, gic_str_len,record) 
										 << "</div>"<<"</td></tr>";
							record = doc_cont->getNextChild();
						}
						if(gic_morebt =="true")
						{
							listHtmlCode << "<tr><td>"
										 << "<div class=\"bottom-links\">"
										 << "<a href=\"http://www.baidu.com\">View more</a></div></td></tr>";
						}
						listHtmlCode << "</tbody></table></div>";
					}
				}
			}
		}
		group = groups->getNextChild();
		if(group)
		{
			listHtmlCode << "<hr>";
		}
		i++;
	}
	code.mHtml << listHtmlCode <<"</div>";
	str	<< "gic_border:\"" << gic_border << "\"," 
		<< "gic_bordcolor:\"" << gic_bordcolor << "\"," 
		<< "gic_hdbgcolor:\"" << gic_hdbgcolor << "\","
		<< "gic_bdbgcolor:\"" << gic_bdbgcolor << "\","
		<< "gic_mouseovercolor:\"" << gic_mouseovercolor << "\","
		<< "gic_mousedowncolor:\"" << gic_mousedowncolor << "\","
		<< "gic_singlecolor:\"" << gic_singlecolor << "\","
		<< "gic_evencolor:\"" << gic_evencolor << "\","
		<< "gic_str_len:\"" << gic_str_len << "\","
		<< "vpdCssRandom:\"" << grid_css_random <<"\","
		<< "gic_autoheight:"<<gic_autoheight<<","
		<< "gic_autowidth:"<<gic_autowidth<<","
		<< "gic_buildhead:"<<gic_buildhead<<","
		<< "head_style:\""<<head_style<<"\","
		<< "head_weight:\""<<head_weight<<"\","
		<< "head_aln:\""<<head_aln<<"\","
		<< "head_color:\""<<head_color<<"\","
		<< "head_size:\""<<head_size<<"\","
		<< "gic_lstn:\""<<gic_lstn<<"\","
		<< "cmp:\"" << cmp << "\","
		<< "conts:\"" << conts << "\","
		<< "objids:\"" << objids << "\","
		<< "titles:\"" << titles << "\","
		<< "datas:\'" << datas << "\',"
		<< "gic_morebt:" << gic_morebt;
	code.mJson << str;
	return true;
}

OmnString
AosGicHtmlGroupList::createHtmlCode(const OmnString &temp,const OmnString &str_len, const AosXmlTagPtr &record)
{
	//OmnString tt = "\${@zky_objid}${@zky_tnail}";
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
		}
//		value = "gg gk ku kh khu lo hu j ih jl h ih eeh g u hu hu u hu gg ygy fd";
/*		
		int len = 0;
		OmnString arr[50];
		changToArray(value, " ", arr, len);
		int nowLeng = len;
		if(str_len)
		{
			int needLeng = atoi(str_len);
			OmnString nowWord;
			if(nowLeng > needLeng)
			{
				for(int k=0;k<needLeng;k++)
				{
			    	nowWord << arr[k] << " ";
				}
				nowWord << "...";
				value = nowWord;
			}
		}
		*/
		str.replace(p1, p2 - p1 + 1, value);

		p1 = str.findSubString("${", p1+1);
		p2 = str.findSubString("}",p1);
	}
	return str;
}

bool
AosGicHtmlGroupList::changToArray(
    OmnString &str,
    OmnString sep,
    OmnString temp[],
    int &len)
{
	int i = 0;
	str.replace(sep, ",", true);
	int flage = str.find(',', false);
	while(flage>0)
	{
		temp[i] = str.substr(0,flage-1);
		str = str.substr(flage+1,str.length());
		flage = str.find(',',false); 
		i++;
	}
	if(str !="")
	{
		temp[i] = str;
		len = i+1;
	}
	else
	{
		len = i;
	}
	return true;
 }

OmnString
AosGicHtmlGroupList::getRecordValue(const AosXmlTagPtr &record, const OmnString &path, OmnString &dft)
{
	OmnString str;
	AosXmlTagPtr rec = record;
	OmnStrParser1 parser(path, "/", false, false);
	OmnString p;
	while ((p = parser.nextWord()) != "")
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
AosGicHtmlGroupList::generateTemplate(OmnString &cont, const AosHtmlReqProcPtr &htmlPtr)
{
    //int findHandle = cont.findSubString("gAosHtmlHandler",0);
	//if(findHandle != -1)
	//{
	//	htmlPtr->addGic("gAosHtmlHandler");//js file id;
	//}
	int f1 = cont.findSubString("|&&|", 0);
	int f2 = cont.findSubString("|&|", 0);
	int f3 = cont.findSubString("&lt;", 0);
	int f4 = cont.findSubString("&gt;", 0);
	if(f1 != -1)
	{
		cont.replace("|&&|", "\"", true);
	}
	if(f2 != -1){
		cont.replace("|&|", "\'", true);
	}
	if(f3 != -1){
	    cont.replace("&lt;", "<", true);
	}
    if(f4 != -1){
        cont.replace("&gt;", ">", true);
    }

	if(!htmlPtr->isHomePage())
	{
		bool t = true;
		cont.replace("\'", "\\'", t);
	}
	return cont;
}

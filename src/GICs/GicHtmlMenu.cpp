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
#include "GICs/GicHtmlMenu.h"
#include "HtmlUtil/HtmlUtil.h" 
#include "HtmlServer/HtmlUtil.h" 
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/StrParser.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include <iostream>
#include <boost/regex.hpp>
using namespace boost ;
using namespace std ;

//static AosGicPtr sgGic = new AosGicHtmlMenu();

AosGicHtmlMenu::AosGicHtmlMenu(const bool flag)
:
AosGic(AOSGIC_HTMLMENU, AosGicType::eHtmlMenu, flag)
{
}


AosGicHtmlMenu::~AosGicHtmlMenu()
{
}

OmnString 
AosGicHtmlMenu::replace(OmnString &cont)
{
	int f1 = cont.findSubString("&lt;", 0);
	int f2 = cont.findSubString("&gt;", 0);
	int f3 = cont.findSubString("&quot;", 0);
	int f4 = cont.findSubString("xmlns=\"http://www.w3.org/1999/xhtml\"", 0);
	if(f1 != -1)
	{
		cont.replace("&lt;", "<", true);
	}
	if(f2 != -1)
	{
		cont.replace("&gt;", ">", true);
	}
	if(f3 != -1)
	{
		cont.replace("&quot;", "\"", true);
	}
	if(f4 != -1)
	{
		cont.replace("xmlns=\"http://www.w3.org/1999/xhtml\"", " ", true);
	}
	return cont;
}

string&
AosGicHtmlMenu::replaceTow(string &cont)
{
	static std::string r = "<(\\w+)([^/>]*)(/>)";
	try
	{
		boost::regex reg(r);
		boost::smatch m;
		cont = boost::regex_replace(cont, reg, "<$1$2></$1>");
		return cont;
	}
	catch(const boost::bad_expression& e)
	{
		cout << "That's not a valid regular expression!" << e.what() << endl;
		return cont;
	}
}

OmnString 
AosGicHtmlMenu::ParseDate(AosXmlTagPtr &node ,OmnString &id , bool flage, OmnString &cla)
{
	if(node->getFirstChild("record"))
	{
		AosXmlTagPtr node2 = node;
		AosXmlTagPtr rec = node->getFirstChild("record");
		OmnString ul;
		OmnString ul2;
		if(flage)
		{
			ul << "<div id = \""<<id<<"\" class=\""<<cla<<"\" style=\"display:none;\"><ul class=\"menu\" >";
		}
		else
		{
			ul << "<div><ul>";
		}
		while(rec)
		{
			OmnString zky_objid = rec->getAttrStr("zky_objid","");
			OmnString zky_name = rec->getAttrStr("zky_name","");
			AosXmlTagPtr actions = rec->getFirstChild("actions");
			string action = actions->toString().data();
			action = replaceTow(action);
			OmnString xx = "xx";
			OmnString child = ParseDate(rec,xx,false,xx);
			OmnString li = "<li>";
			if(child != "false")
			{
				li << "<a href =\"#\" class=\"parent\"><span>" << zky_name << "</span></a>";
			}
			else
			{
				li << "<a href =\"#\"><span>" << zky_name << "</span></a>";
			}
			   li<< "<span style=\"display:none;\">" << action << "</span>";
			if(child != "false")
			{
				li << child;
			}
			li << "</li>";
			ul << li;
			rec = node->getNextChild("record");
		}
		ul << "</ul></div>";
		if(flage)
		{
			ul2 << "<div id=\""<<id<<"_2\" class=\""<<cla<<"\"><ul class=\"menu\" >";
			AosXmlTagPtr rec2 = node2->getFirstChild("record");
			while(rec2)
			{
				OmnString zky_name = rec2->getAttrStr("zky_name","");
				OmnString li = "<li>";
				li << "<a class=\"parent\"><span>" << zky_name << "</span></a></li>";
				ul2 << li;
				rec2 = node->getNextChild("record");
			}
			ul2 <<"</ul></div>";
		}
		ul << ul2;
		return ul ;
	}
	else
	{
		return "false";
	}
}

bool	
AosGicHtmlMenu::generateCode(
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
	OmnString gic_id = AosGetHtmlElemId();
	 /*value bd*/
	OmnString gic_width  = vpd->getAttrStr("gic_width","200");
	OmnString gic_height = vpd->getAttrStr("gic_height","25");
	OmnString gic_style = vpd->getAttrStr("gic_style","1");
	OmnString def_data ;
	def_data 
			  << "<contents ><record zky_objid=\"objtable_001\" zky_name=\"Home\"><actions></actions>"
			  << "<record zky_objid=\"objtable_0011\" zky_name=\"Welcome\"><actions></actions>"
			  << "<record zky_objid=\"objtable_00111\" zky_name=\"Weme\"><actions></actions></record></record>"
			  << "<record zky_objid=\"objtable_0012\" zky_name=\"to\"><actions></actions></record>"
			  << "<record zky_objid=\"objtable_0013\" zky_name=\"yunyuyan\"><actions></actions></record></record>"
			  << "<record zky_objid=\"objtable_002\" zky_name=\"zykie\"><actions></actions>"
			  << "<record zky_objid=\"objtable_00222\" zky_name=\"Welcome\"><actions></actions></record>"
			  << "<record zky_objid=\"objtable_00224\" zky_name=\"to\"><actions></actions></record>"
			  << "<record zky_objid=\"objtable_00223\" zky_name=\"zykie\"><actions></actions></record>"
			  << "<record zky_objid=\"objtable_0022\" zky_name=\"wzykie.com\"><actions></actions></record></record>"
			  << "<record zky_objid=\"objtable_003\" zky_name=\"yunyuyan\"><actions></actions>"
			  << "</record></contents>";
	OmnString gic_data = vpd->getAttrStr("gic_data",def_data);
	gic_data = replace(gic_data);
	AosXmlParser parsers;
	AosXmlTagPtr Fatherxml = parsers.parse(gic_data, "" AosMemoryCheckerArgs);
	OmnString style = gic_style;

	OmnStrParser1 parser(style, ",");
	OmnString id = "menu_";
			  id << gic_id;
	OmnString cla = "menuclass";
			  cla << gic_style;
	OmnString data = ParseDate(Fatherxml,id,true,cla);
/********************************************************************************/
	code.mHtml << data ;
	OmnString str;
	str 
		<< ",xtype:\'" << vpd->getAttrStr("gic_type") <<"\'"
		<< ",ulid:\'" << id <<"\'"
		<< ",gic_style:\'" << gic_style <<"\'"
		<< ",gic_data:\'" << gic_data <<"\'";
	code.mJson << str;
	return true;
}


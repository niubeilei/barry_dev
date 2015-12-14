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
// 07/08/2010: Created by Henry
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicSimMenu.h"
#include "HtmlUtil/HtmlUtil.h"
#include "HtmlServer/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include <iostream>
#include <boost/regex.hpp>
using boost::regex;
using namespace std;
// static AosGicPtr sgGic = new AosGicSimMenu();

AosGicSimMenu::AosGicSimMenu(const bool flag)
:
AosGic(AOSGIC_SIMMENU, AosGicType::eSimMenu, flag)
{
}


AosGicSimMenu::~AosGicSimMenu()
{
}

OmnString
AosGicSimMenu::replace(OmnString &cont)
{
	int f1 = cont.findSubString("&lt;", 0);
	int f2 = cont.findSubString("&gt;", 0);
	int f3 = cont.findSubString("&quot;", 0);
	int f4 = cont.findSubString("xmlns=\"http://www.w3.org/1999/xhtml\"", 0);

	// Jackie, 09/26/2011
	int f5 = cont.findSubString("&amp;", 0);

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

	// Jackie, 09/26/2011
	if (f5 != -1)
	{
		cont.replace("&amp;", "&", true);
	}
	return cont;
}

string&
AosGicSimMenu::replaceTow(string &cont)
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

bool	
AosGicSimMenu::generateCode(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
        const OmnString &parentid,		
		AosHtmlCode &code)//,
		//const AosRundataPtr &rdata)
{
	// This function will generate:
	// 	1. HTML code
	// 	2. CSS code
	// 	3. JavaScript Code
	// 	4. Flash code
		//	AosXmlTagPtr thevpd = vpd->getFirstChild("gic_data");
			OmnString gic_width  = vpd->getAttrStr("gic_width","600");
			OmnString gic_height  = vpd->getAttrStr("gic_height","35");

			OmnString path = htmlPtr->getImagePath();
			int gic_def = vpd->getAttrInt("gic_def",1);
			OmnString gic_menubgimg = vpd->getAttrStr("gic_menubgimg","");
			OmnString gic_mousebgimg = vpd->getAttrStr("gic_mousebgimg","");
			OmnString gic_clickbgimg = vpd->getAttrStr("gic_clickbgimg","");
			OmnString gic_menubgColor = vpd->getAttrStr("gic_menubgColor","transparent");
			if(gic_menubgColor != "")
			{
				gic_menubgColor = AosHtmlUtil::getWebColor(gic_menubgColor);
			}
			OmnString gic_mousebgColor = vpd->getAttrStr("gic_mousebgColor","transparent");
			if(gic_mousebgColor != "")
			{
				gic_mousebgColor = AosHtmlUtil::getWebColor(gic_mousebgColor);
			}
			OmnString gic_clickbgColor = vpd->getAttrStr("gic_clickbgColor","transparent");
			if(gic_clickbgColor != "")
			{
				gic_clickbgColor = AosHtmlUtil::getWebColor(gic_clickbgColor);
			}
			OmnString gic_menuColor = vpd->getAttrStr("gic_menuColor","#000000");
			if(gic_menuColor != "")
			{
				gic_menuColor = AosHtmlUtil::getWebColor(gic_menuColor);
			}
			OmnString gic_menuMouseColor = vpd->getAttrStr("gic_menuMouseColor","#000000");
			if(gic_menuMouseColor != "")
			{
				gic_menuMouseColor = AosHtmlUtil::getWebColor(gic_menuMouseColor);
			}
			OmnString gic_menuClickColor = vpd->getAttrStr("gic_menuClickColor","#000000");
			if(gic_menuClickColor != "")
			{
				gic_menuClickColor = AosHtmlUtil::getWebColor(gic_menuClickColor);
			}
			OmnString gic_menuFsize = vpd->getAttrStr("gic_menuFsize","12");
			OmnString gic_menuMouseFsize = vpd->getAttrStr("gic_menuMouseFsize","12");
			OmnString gic_menuClickFsize = vpd->getAttrStr("gic_menuClickFsize","12");
			OmnString gic_menuFstyle = vpd->getAttrStr("gic_menuFstyle","plain");
			OmnString gic_menuMouseFstyle = vpd->getAttrStr("gic_menuMouseFstyle","plain");
			OmnString gic_menuClickFstyle = vpd->getAttrStr("gic_menuClickFstyle","plain");
			OmnString gic_menufixedwidth = vpd->getAttrStr("gic_menufixedwidth","false");
			OmnString gic_menuwidth = vpd->getAttrStr("gic_menuwidth","150");
			OmnString gic_textalign = vpd->getAttrStr("gic_textalign","center");
			OmnString gic_leftSpace = vpd->getAttrStr("gic_leftSpace","");
			OmnString gic_rightSpace = vpd->getAttrStr("gic_rightSpace","");
			OmnString gic_menuSeparated = vpd->getAttrStr("gic_menuSeparated","true");
			OmnString gic_menuSeparatedwidth = vpd->getAttrStr("gic_menuSeparatedwidth","2");
			OmnString gic_menuSeparatedColor = vpd->getAttrStr("gic_menuSeparatedColor","#FFFFFF");
			if(gic_menuSeparatedColor != "")
			{
				gic_menuSeparatedColor = AosHtmlUtil::getWebColor(gic_menuSeparatedColor);
			}
			OmnString gic_menuSeparatedimg = vpd->getAttrStr("gic_menuSeparatedimg");
			//OmnString gic_scrollType = vpd->getAttrStr("gic_scrollType","up");
			OmnString del_data;
			del_data << "<contents ><record zky_objid=\"objtable_001\" zky_name=\"Home\"><actions></actions></record>"
	                 << "<record zky_objid=\"objtable_002\" zky_name=\"zykie\"><actions></actions></record>"
					 << "<record zky_objid=\"objtable_003\" zky_name=\"yunyuyan\"><actions></actions>"
					 << "</record></contents>";
			OmnString gic_data = vpd->getAttrStr("gic_data",del_data);
			gic_data = replace(gic_data);
			AosXmlParser parsers;
			AosXmlTagPtr Fatherxml = parsers.parse(gic_data, "" AosMemoryCheckerArgs);
			if (!Fatherxml)
			{
				// Jackie, 09/26/2011
				// It is possible that some old data escaped '&'. 
				gic_data = replace(gic_data);
				Fatherxml = parsers.parse(gic_data, "" AosMemoryCheckerArgs);
				if (!Fatherxml)
				{
					OmnAlarm << "Not an XML doc!" << enderr;
					return false;
				}
			}
			Fatherxml = Fatherxml->getFirstChild();
			int nums = Fatherxml->getNumSubtags();
			//OmnString liwidth;
			//liwidth << (atoi(gic_width)-atoi(gic_menuSeparatedwidth))/nums;
			OmnString divwidth = gic_menuwidth;
			//divwidth << (atoi(gic_width)-(nums-1)*atoi(gic_menuSeparatedwidth))/nums;
			//divwidth << atoi(liwidth) - atoi(gic_menuSeparatedwidth);
			OmnString ulstyle;
			ulstyle <<"<ul style=\"cursor:pointer;height:" << gic_height << ";width:" << gic_width << ";\">";
			if(Fatherxml->getFirstChild("record"))
			{
				AosXmlTagPtr Fatherxml2 = Fatherxml;
				AosXmlTagPtr rec = Fatherxml->getFirstChild("record");
				OmnString norstyle;
				norstyle <<"display:block;float:left;height:" <<gic_height << ";";
				OmnString listyle;
				listyle << "<li style=\"" << norstyle << "line-height:" << gic_height <<
			    	"px;overflow:hidden;position:relative;";//width:" << liwidth << ";\">";
				OmnString divstyle;
				divstyle <<"<div style=\"" << norstyle << "text-align:" << gic_textalign <<";width:" 
					<< divwidth <<";";
				if(gic_textalign == "left")
				{

					divstyle << "padding-left:" << gic_leftSpace << ";";
				}
				else if(gic_textalign == "right")
				{
					divstyle << "padding-right:" << gic_rightSpace << ";";
				}
				//divstyle << "color:" << gic_menuColor << ";font-size:" << gic_menuFsize << ";";
				OmnString sepstyle;
				sepstyle << "<div style=\"float:left;height:" << gic_height  << ";width:" << gic_menuSeparatedwidth 
					<<";background-color:" << gic_menuSeparatedColor << ";";
				if(gic_menuSeparatedimg != "")
				{
					OmnString url = path;
					if (gic_menuSeparatedimg.indexOf("http://", 0) == 0) url = "";
					sepstyle << "background-image:url(" << url << gic_menuSeparatedimg << ");";
				}
				if(gic_menuSeparated == "true")
				{
					sepstyle << "display:block;\"></div>";
				}
				else
				{
					sepstyle << "display:none;\"></div>";
				}
				int active = 1;
				while(rec)
				{
					OmnString zky_objid = rec->getAttrStr("zky_objid", "");
					OmnString zky_name = rec->getAttrStr("zky_name", "");
					AosXmlTagPtr actions = rec->getFirstChild("actions");
					string action = actions->toString().data();
					action = replaceTow(action);
					OmnString menustyle = divstyle;
					OmnString liwidth;
					if(active != nums && gic_menuSeparated == "true")
					{
						liwidth << atoi(divwidth.data()) + atoi(gic_menuSeparatedwidth.data());
					}
					else
					{
						liwidth << divwidth;
					}
					if(active == gic_def)
					{
						menustyle << "background-color:" << gic_clickbgColor << ";";
						menustyle << "color:" << gic_menuClickColor << ";";
						menustyle << "font-size:" << gic_menuClickFsize << ";";
						if(gic_menuClickFstyle == "italic")
						{
							menustyle << "font-style:" << gic_menuClickFstyle << ";";
						}
						else if(gic_menuClickFstyle == "bold")
						{
							menustyle << "font-weight:" << gic_menuClickFstyle << ";";
						}
						else if(gic_menuClickFstyle == "bolditalic")
						{
							menustyle << "font-style:italic;font-weight:bold;";
						}
						if(gic_clickbgimg != "")
						{
							OmnString url = path;
							if (gic_clickbgimg.indexOf("http://", 0) == 0) url = "";
							menustyle << "background-image:url(" << url << gic_clickbgimg << ");";
						}
					}
					else
					{
						menustyle << "background-color:" << gic_menubgColor << ";";
						menustyle << "color:" << gic_menuColor << ";";
						menustyle << "font-size:" << gic_menuFsize << ";";
						if(gic_menuFstyle == "italic")
						{
							menustyle << "font-style:" << gic_menuFstyle << ";";
						}
						else if(gic_menuFstyle == "bold")
						{
							menustyle << "font-weight:" << gic_menuFstyle << ";";
						}
						else if(gic_menuFstyle == "bolditalic")
						{
							menustyle << "font-style:italic;font-weight:bold;";
						}
						if(gic_menubgimg != "")
						{
							OmnString url = path;
							if (gic_menubgimg.indexOf("http://", 0) == 0) url = "";
							menustyle << "background-image:url(" << url << gic_menubgimg << ");";
						}
					}
					menustyle << "\">";
					OmnString li;
					li << listyle << "width:" << liwidth << ";\">";
					li << menustyle << zky_name << "</div>";
					if(active != nums)
					{
						li << sepstyle;
					}
					li << "<div style=\"display:none;\">" << action << "</div>";
					li << "</li>";
					ulstyle << li;
					active++;
					rec = Fatherxml->getNextChild("record");
				}
				ulstyle << "</ul>";
			}

	OmnString str;
	str 
		<< ",xtype:\'" << vpd->getAttrStr("gic_type") <<"\'"
		<< ",gic_def:\'" << gic_def<<"\'" 
		<< ",gic_menubgimg:\'" << gic_menubgimg<<"\'" 
		<< ",gic_mousebgimg:\'" << gic_mousebgimg<<"\'"
		<< ",gic_clickbgimg:\'" << gic_clickbgimg<<"\'"
		<< ",gic_menubgColor:\'" << gic_menubgColor<<"\'"
		<< ",gic_mousebgColor:\'" << gic_mousebgColor<<"\'"
		<< ",gic_clickbgColor:\'" << gic_clickbgColor<<"\'"
		<< ",gic_menuColor:\'" << gic_menuColor<<"\'"
		<< ",gic_menuMouseColor:\'" << gic_menuMouseColor<<"\'"
		<< ",gic_menuClickColor:\'" << gic_menuClickColor<<"\'"
		<< ",gic_menuFsize:\'" << gic_menuFsize<<"\'"
		<< ",gic_menuMouseFsize:\'" << gic_menuMouseFsize<<"\'"
		<< ",gic_menuClickFsize:\'" << gic_menuClickFsize<<"\'"
		<< ",gic_menuFstyle:\'" << gic_menuFstyle<<"\'"
		<< ",gic_menuMouseFstyle:\'" << gic_menuMouseFstyle<<"\'"
		<< ",gic_menuClickFstyle:\'" << gic_menuClickFstyle<<"\'"
		<< ",gic_menufixedwidth:\'" << gic_menufixedwidth<<"\'"
		<< ",gic_menuwidth:\'" << gic_menuwidth<<"\'"
		<< ",gic_textalign:\'" << gic_textalign<<"\'"
		<< ",gic_leftSpace:\'" << gic_leftSpace<<"\'"
		<< ",gic_rightSpace:\'" << gic_rightSpace<<"\'"
		<< ",gic_menuSeparated:\'" << gic_menuSeparated<<"\'"
		<< ",gic_menuSeparatedwidth:\'" << gic_menuSeparatedwidth<<"\'"
		<< ",gic_menuSeparatedColor:\'" << gic_menuSeparatedColor<<"\'"
		<< ",gic_menuSeparatedimg:\'" << gic_menuSeparatedimg<<"\'"
		<< ",gic_data:\'" << gic_data <<"\'";

	code.mHtml << ulstyle;
	code.mJson << str;
	return true;
}


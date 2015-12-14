///////////////////////////////////////////////////////////////////////////
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
// 07/08/2010: Created by John Huang
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicTabMenu.h"
#include "SearchEngineAdmin/SengAdmin.h"
#include "GICs/GIC.h"
#include "HtmlUtil/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/StrSplit.h"
#include "Util/String.h"

// static AosGicPtr sgGic = new AosGicTabMenu();

AosGicTabMenu::AosGicTabMenu(const bool flag)
:
AosGic(AOSGIC_TABMENU, AosGicType::eTabMenu, flag)
{

	OmnString errmsg;
	AosGicType::addName(AOSGIC_ADVTAB, mGicType, errmsg);
}


AosGicTabMenu::~AosGicTabMenu()
{
}


bool	
AosGicTabMenu::generateCode(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		const OmnString &parentid,
		AosHtmlCode &code)
{
	OmnString valbd = vpd->getAttrStr("gic_valuebd");
	OmnString gic_valuebd = obj?obj->getAttrStr(valbd):"";
	if(gic_valuebd=="")
	{
		gic_valuebd = vpd->getAttrStr(valbd);
	}

	OmnString gic_labels = vpd->getAttrStr("gic_labels","Tab_A|$|Tab_B|$|Tab_C|$|+");
	OmnString gic_layout = vpd->getAttrStr("gic_layout","top");
	OmnString gic_vpdnames = vpd->getAttrStr("gic_vpdnames","_0|$|_1|$|_2");
	int gic_dft_entry = vpd->getAttrInt("gic_dft_entry",0);
	int gic_menu_height = vpd->getAttrInt("gic_menu_height",60);
	int gic_menu_width = vpd->getAttrInt("gic_menu_width",80);
	int gic_menubar_h = vpd->getAttrInt("gic_menubar_h",50);
	int gic_menubar_w = vpd->getAttrInt("gic_menubar_w",500);
	int gic_menubar_fsize = vpd->getAttrInt("gic_menubar_fsize",12);
	int gic_scrolltime = vpd->getAttrInt("gic_scrolltime",0);
	
	OmnString tabConfig;
	tabConfig << "{";
	
	OmnString temp;
	//OmnString labs[20];
	vector<OmnString> labs;
	temp = gic_labels;
	changToArray(temp,labs);
	//OmnString vpds[20];
	vector<OmnString> vpds;
	temp = gic_vpdnames;
	changToArray(temp,vpds);

	AosXmlTagPtr  panels= vpd->getFirstChild("panels");
	u32 i = 0;
	while(i < labs.size() && labs[i] != "" && labs[i]!="+")
	{
		if (vpds[i] == "")
		{
			i++;	
			continue;
		}
		if(panels)
		{
			AosXmlTagPtr tab = panels->getFirstChild(vpds[i]);
			if(tab)
				tab = tab->getFirstChild("gic_creators");
			else
			{
				OmnString tmp = "_";
				tmp << i;
				if(vpds[i] != tmp)
				{
					if(i!=0 && i<vpds.size())
						tabConfig << ",";
					if( i < vpds.size())
						tabConfig << i << ":{vpdname:'" << vpds[i] <<"'}";
				}
			}
			if(tab)
			{
				AosHtmlCode tabCode;
				if(i!=0)
				{
					tabConfig << ",";
				}	
				tabConfig << i<<":{";
				AosXmlTagPtr gic = tab->getFirstChild();
				while(gic)
				{
					AosXmlTagPtr mobj; 
					//AosGic::createGic(htmlPtr,gic,mobj,parentid,tabCode);
					AosGic::createGic(htmlPtr,gic,mobj,parentid,tabCode, -1, -1); //Ketty 2011/09/22
					gic = tab->getNextChild();
					if(gic)
						tabCode.mJson << ",";
				}
				tabConfig << "items:[" <<tabCode.mJson <<"],html:'"<<tabCode.mHtml<<"'}";
			}
		}
		i++;
	}	
	tabConfig << "}";

	OmnString items;
	if(gic_layout == "top")
		  items << "[{xtype:'panel',border:false,region:'north',layout:'table',lablayout:'horizontal'},";
	if(gic_layout == "left")
		  items << "[{xtype:'panel',border:false,region:'west',layout:'table',lablayout:'horizontal'},";
	if(gic_layout == "right")
		  items << "[{xtype:'panel',border:false,region:'east',layout:'table',lablayout:'horizontal'},";
	if(gic_layout == "bottom")
		  items << "[{xtype:'panel',border:false,region:'east',layout:'table',lablayout:'horizontal'},";
	items << "{xtype:'aos_pane',border:true,region:'center',layout:'card',isDefault:true}]";
	OmnString str;
	str	<< ","
		<< "gic_dft_entry:"	 	<< gic_dft_entry	 <<","
		<< "gic_layout:'" 		<< gic_layout 		 <<"',"
		<< "gic_vpdnames:'" 	<< gic_vpdnames 	 <<"',"
		<< "gic_menu_width:" 	<< gic_menu_width	 <<","
		<< "gic_menu_height:" 	<< gic_menu_height	 <<","
		<< "gic_menubar_w:" 	<< gic_menubar_w 	 <<","
		<< "gic_menubar_h:" 	<< gic_menubar_h 	 <<","
		<< "gic_menubar_fsize:" << gic_menubar_fsize <<","
		<< "gic_scrolltime:"    << gic_scrolltime	 <<","
		<< "gic_labels:'" 		<< gic_labels	 	 <<"',"
		<< "items:" 			<< items	 	 	 <<","
		<< "tabConfig:" 		<< tabConfig;

	code.mJson << str;
	return true;
}


bool
AosGicTabMenu::changToArray(
			OmnString &str,
			vector<OmnString> &temp)
{
	// OmnString s1 ,s2;
	// s1 << "|$|";
	// s2 << ",";
	// str.replace(s1,s2,true);

	AosStrSplit parser(str, "|$|");
	OmnString word;
    while(parser.hasMore())
    {	
		word = parser.nextWord();
        temp.push_back(word);
    }
	return true;

	/* 08/15/2011 lynch
	int flage = str.find(',',false);
	while(flage>0)
	{
		if (flage - 1 > 0)
		{
			temp[i] = str.substr(0,flage-1);
			i++;
			str = str.substr(flage+1,str.length()-1);
			flage = str.find(',',false);
		}
	}
	if(str!="")
	{
		temp[i] = str;
		return i;
	}
	else
	{
		return i-1;
	}
	*/
}

//////////////////////////////////////////////////////////////////////////////////
//
//	Copyright (C) 2005
//
//	Define html compiler util
//	create by Ken Lee 2011/04/11
///////////////////////////////////////////////////////////////////////////////////
#include "HtmlServer/HtmlRetrieveSites.h"

#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "XmlUtil/XmlTag.h"
#include "SEUtil/DocTags.h"
#include "SearchEngineAdmin/SengAdmin.h"

AosHtmlRetrieveSites::AosHtmlRetrieveSites()
{
}


AosHtmlRetrieveSites::~AosHtmlRetrieveSites()
{
}


bool
AosHtmlRetrieveSites::retrieveAllSites(
	OmnString &xmlStr,
	const AosXmlTagPtr &cookies,
	const u32 siteid,
	const OmnString &sessionId,
	const u64 &urldoc_docid,
	const OmnString &isInEditor,
	const OmnString &vpdname,
	OmnString &errmsg)
{
	//mVpdnames.clear();
	// ex: create one list...
	// follow the list format to create xml
	// list is a vector<OmnString>
	// very have the format is : "mVpdNumber, gic_type, subvpdname, vpdname";
	// for example : 1, gic_imgrolling, yunyuyan_zhuce, yunyuayn_zheye
	// 
	mVpdArray.clear();
	mVpdnamelist.clear();
	map<OmnString, int>::iterator it;

	mVpdArray.push_back(vpdname);

	mPageNumber = 0;
	OmnString intervpdname;
	intervpdname << mPageNumber << "," << " "<< "," << " " << "," << vpdname << ",root";
	mVpdnamelist.push_back(intervpdname);
	
	for (int i=0; i<(int)mVpdArray.size(); i++)
	{
		mPageNumber = i+1;
		AosXmlTagPtr response = AosSengAdmin::getSelf()->retrieveDoc(
				siteid, mVpdArray[i], sessionId, urldoc_docid, isInEditor, cookies);
		if (!response)
			continue;
		
		AosXmlTagPtr subXml = response->getFirstChild("Contents");
		OmnString superVpdname;
		if (subXml)
		{
			AosXmlTagPtr subVpd = subXml->getFirstChild();
			if (subVpd)
			{
				superVpdname = mVpdArray[i];
				retrieveSites(subVpd, errmsg, superVpdname);
			}
		}
    }
	convertToXml(mVpdnamelist, xmlStr, errmsg);
	return true;
}


bool
AosHtmlRetrieveSites::retrieveAllSites(
	const AosHtmlReqProcPtr &htmlPtr,
	const AosXmlTagPtr &cookies,
	const u32 siteid,
	const OmnString &sessionId,
	const u64 &urldoc_docid,
	const OmnString &isInEditor,
	const OmnString &vpdname,
	OmnString &errmsg)
{
	//mVpdnames.clear();
	// ex: create one list...
	// follow the list format to create xml
	// list is a vector<OmnString>
	// very have the format is : "mVpdNumber, gic_type, subvpdname, vpdname";
	// for example : 1, gic_imgrolling, yunyuyan_zhuce, yunyuayn_zheye
	// 
	mVpdArray.clear();
	mVpdnamelist.clear();
	map<OmnString, int>::iterator it;

	mVpdArray.push_back(vpdname);

	mPageNumber = 0;
	OmnString intervpdname;
	intervpdname << mPageNumber << "," << " "<< "," << " " << "," << vpdname << ",root";
	mVpdnamelist.push_back(intervpdname);
	
	for (int i=0; i<(int)mVpdArray.size(); i++)
	{
		mPageNumber = i+1;
		AosXmlTagPtr response = AosSengAdmin::getSelf()->retrieveDoc(
				siteid, mVpdArray[i], sessionId, urldoc_docid, isInEditor, cookies);
		if (!response)
			continue;
		
		AosXmlTagPtr subXml = response->getFirstChild("Contents");
		OmnString superVpdname;
		if (subXml)
		{
			AosXmlTagPtr subVpd = subXml->getFirstChild();
			if (subVpd)
			{
				AosXmlTagPtr vpd = subVpd->clone(AosMemoryCheckerArgsBegin);
				AosXmlTagPtr obj = vpd->getFirstChild("embedobj");
				OmnString parentId;
				OmnString flag = htmlPtr->getInsertFlag();
				htmlPtr->appendVpdArray(vpd, obj, parentId, flag, mVpdArray[i], 1024, 768);
				superVpdname = mVpdArray[i];
				retrieveSites(vpd, errmsg, superVpdname);
			}
		}
    }
	//convertToXml(mVpdnamelist, xmlStr, errmsg);
	return true;
}

bool
AosHtmlRetrieveSites::retrieveAllSites(
	OmnString &xmlStr,
	const OmnString &vpdname,
	OmnString &errmsg,
	const AosRundataPtr &rdata)
{
	//mVpdnames.clear();
	// ex: create one list...
	// follow the list format to create xml
	// list is a vector<OmnString>
	// very have the format is : "mVpdNumber, gic_type, subvpdname, vpdname";
	// for example : 1, gic_imgrolling, yunyuyan_zhuce, yunyuayn_zheye
	// 
	mVpdArray.clear();
	mVpdnamelist.clear();
	map<OmnString, int>::iterator it;

	mVpdArray.push_back(vpdname);

	mPageNumber = 0;
	OmnString intervpdname;
	intervpdname << mPageNumber << "," << " "<< "," << " " << "," << vpdname << ",root";
	mVpdnamelist.push_back(intervpdname);
	
	for (int i=0; i<(int)mVpdArray.size(); i++)
	{
		mPageNumber = i+1;
		// Chen Ding, 11/24/2012
		// AosXmlTagPtr subXml = AosDocClient::getSelf()->getDocByObjid(mVpdArray[i], rdata);
		AosXmlTagPtr subXml = AosGetDocByObjid(mVpdArray[i], rdata);
		if (!subXml)
			continue;
		
		//AosXmlTagPtr subXml = response->getFirstChild("Contents");
		OmnString superVpdname;
		AosXmlTagPtr subVpd = subXml->getFirstChild();
		if (subVpd)
		{
			superVpdname = mVpdArray[i];
			retrieveSites(subVpd, errmsg, superVpdname);
		}
    }
	convertToXml(mVpdnamelist, xmlStr, errmsg);
	return true;
}

bool
AosHtmlRetrieveSites::getAllSites(
	OmnString &xmlStr,
	const AosXmlTagPtr &cookies,
	const u32 siteid,
	const OmnString &sessionId,
	const u64 &urldocid,
	const OmnString &isInEditor,
	const OmnString &vpdname,
	OmnString &errmsg)
{
	OmnString inst_objid = vpdname;
	AosXmlTagPtr inst_doc = AosSengAdmin::getSelf()->retrieveDocByObjid(siteid, sessionId, urldocid, inst_objid);
	if (!inst_doc)
	{
		errmsg << "Failed to get the inst through the inst objid : " << inst_objid;
		return false;
	}
	OmnString url = inst_doc->getNodeText("zky_url");
	OmnString url_objid = AosObjid::createUrlObjid(url);
	AosXmlTagPtr urldoc = AosSengAdmin::getSelf()->retrieveDocByObjid(siteid, sessionId, urldocid, url_objid);
	if (!urldoc)
	{
	    errmsg << "Failed to get the urldoc through the url: " << vpdname;
	    return false;
	}
	OmnString appid = urldoc->getNodeText("zky_appid");
	if(appid == "")
	{
		errmsg << "The app name is null!";
		return false;
	}
	OmnString instid = urldoc->getNodeText("zky_instid");
	if(instid == "")
	{
		errmsg << "The instance name is null!";
		return false;
	}
	AosXmlTagPtr appdoc = AosSengAdmin::getSelf()->retrieveDocByObjid(siteid, sessionId, urldocid, appid);
	if(!appdoc)
	{
		errmsg << "Failed to get the app doc through the url: " << vpdname;
		return false;
	}
	OmnString appdocid = appdoc->getAttrStr(AOSTAG_DOCID);
	OmnString xmldocid = appdoc->getAttrStr("zky_xmldocid");
	if(xmldocid == "")
	{
		errmsg << "Failed to get the xmldocid through the docid: " << appdocid;
		return false;
	}
	int xml_docid = atoi(xmldocid.data());
	AosXmlTagPtr xmldoc = AosSengAdmin::getSelf()->retrieveDocByDocid(siteid, sessionId, urldocid, xml_docid);
	if(!xmldoc)
	{
		errmsg << "Failed to get the xmldoc through the docid: " << xmldocid;
		return false;
	}
	OmnString xmlappid;
	xmlappid << AOSTAG_APPINST << "_" << instid;
	xmldoc->setAttr("appid", xmlappid);
	xmldoc->setAttr("ctnrname", instid);
	xmlStr << xmldoc->toString();
	return true;
}

bool
AosHtmlRetrieveSites::retrieveSites(
		const AosXmlTagPtr &vpd,
		OmnString &errmsg,
		const OmnString &superVpdname)
{
	aos_assert_r(vpd, false);

	// get new xml which send to fontend.
	//AosXmlParser parser;
	//AosXmlTagPtr newXml = parser.parse(data, "");
	//
	//	<root vpdname="xxx">
	//		<record vpdname="xxx">
	//			<record vpdname="xxx" gic_type="xxx">
	//			<record vpdname="xxx"/>
	//			...
	//			<record vpdname="xxx"/>
	//		</record>
	//		<record vpdname="xxx"/>
	//		...
	//	</root>
	//

	// retrieve all the vpd names in AosXmlTagPtr vpd...
	// get vpd names list
	// convert list to xml format
	
	OmnString vpdtype = vpd->getAttrStr("container_type");

    if (vpdtype == "")
    {
        vpdtype = vpd->getAttrStr("pane_type");
    }

	if (vpdtype == "")
	{
	    vpdtype = vpd->getAttrStr("panel_type");
    }

	if (vpdtype == "")
	{
		vpdtype = vpd->getAttrStr("gic_type");
		if(vpdtype != "")
		{
			vpdtype = "gic";
		}
	}

	if (vpdtype == "")
	{
		//check whether it has "gic_creators" subtag;
	    AosXmlTagPtr gic_creators = vpd->getFirstChild("gic_creators");
	    if (gic_creators)
	    {
			vpdtype = "atomic_panel";
	    }
	}

	if (vpdtype == "editor_panel" || vpdtype == "atomic_panel")
	{
		procPanel(vpd, superVpdname);
	}
	else if (vpdtype == "normal_pane")
	{
		procPane(vpd, superVpdname);
	}
	else if (vpdtype == "PWT" || vpdtype == "pwt" || vpdtype == "comp_pane")
	{
		procCompoundPane(vpd, superVpdname);
	}
	else if (vpdtype == "gic")
	{
		procGic(vpd, vpd->getAttrStr("gic_type"), superVpdname);
	}
	else 
	{
		OmnAlarm << "the vpd, " << vpd->getAttrStr("zky_objid") << ", have no vpdtype." << enderr;
	}
	return true;
}


bool
AosHtmlRetrieveSites::procCompoundPane(
	const AosXmlTagPtr &compoundPane,
	const OmnString &superVpdname)
{
	AosXmlTagPtr panes = compoundPane->getFirstChild("panes");
	if (!panes)
	{
		return false;
	}
	AosXmlTagPtr pane = panes->getFirstChild();
	while (pane)
	{
		OmnString pane_type = pane->getAttrStr("pane_type");
		if(pane_type == "PWT" || pane_type == "pwt" || pane_type == "comp_pane")
		{
			procCompoundPane(pane, superVpdname);
		}
		else if(pane_type == "normal_pane")
		{
			procPane(pane, superVpdname);
		}
		pane = panes->getNextChild();
	}
	return true;
}


bool
AosHtmlRetrieveSites::procPane(
	const AosXmlTagPtr &pane,
	const OmnString &superVpdname)
{
	OmnString panel_vpd_name = pane->getAttrStr("panel_vpd_name");
	bool insertFlag = true;
	if (panel_vpd_name != "")
	{
		for (int i = 0; i < (int)mVpdArray.size(); i++)
		{
			if (panel_vpd_name == mVpdArray[i])
				insertFlag = false;
		}
		if (insertFlag)
		{
			mVpdArray.push_back(panel_vpd_name);

			OmnString intervpdname;
			intervpdname << mPageNumber << "," << " ,"<< superVpdname << "," << panel_vpd_name << ",pane";
			mVpdnamelist.push_back(intervpdname);
		}
	}
	else
	{
		AosXmlTagPtr panel = pane->getFirstChild();
		while (panel)
		{
			procPanel(panel, superVpdname);
			panel = pane->getNextChild();
		}
	}
	return true;
}

bool
AosHtmlRetrieveSites::procPanel(
	const AosXmlTagPtr &panel,
	const OmnString &superVpdname)
{
    // This function assumes 'vpd' is a panel. It loops over
	// all of its GICs to check whether it contains new vpds.
	// If yes, it adds the VPD and vpdname into mVpdnames
	// and mVpds.
	AosXmlTagPtr gic_creators = panel->getFirstChild("gic_creators");
	AosXmlTagPtr gic;
	if (gic_creators)
	    gic = gic_creators->getFirstChild();

	while(gic)
	{
		OmnString gic_type = gic->getAttrStr("gic_type");
		// the code is for the special condition... 
		// gic_type == "gic_simple_list". 
		// when attribute tip=="true", tip_vpdname="sc_mouse_over_image" ..start...
		OmnString tip = gic->getAttrStr("tip");
		if (tip == "true")
		{
			OmnString tip_vpdname = gic->getAttrStr("tip_vpdname");	
			bool insertFlag;
			if (tip_vpdname != "")
			{
				for (int i=0; i<(int)mVpdArray.size(); i++)
				{
					if (tip_vpdname == mVpdArray[i])
						insertFlag = false;
				}
				if (insertFlag)
				{
					mVpdArray.push_back(tip_vpdname);

					OmnString intervpdname;
					intervpdname << mPageNumber << "," << gic_type << "," << superVpdname << "," << tip_vpdname << ",action";
					mVpdnamelist.push_back(intervpdname);
				}
		//		doubleVpdname.oldVpdname = tip_vpdname;
		//		doubleVpdname.newVpdname = "";
		//		bool rslt = doesVpdnameExist(doubleVpdname);

		//		if (!rslt)
		//		{
		//			addToMap(doubleVpdname);
		//
		//		}
		//		gic->setAttr("tip_vpdname", doubleVpdname.newVpdname);
			}
		}
		// this condition end... 
		// the result for why not check gic_type, because the gic_simple_list html 
		// version also have this codition...
		if (gic_type == "")
		{
			OmnAlarm << "This gic" << gic_type << " don't have gic_type" << enderr;
			gic = gic_creators->getNextChild();
			continue;
		}
		
		procGic(gic, gic_type, superVpdname);
		gic = gic_creators->getNextChild();
    }
	return true;
}

bool
AosHtmlRetrieveSites::procGic(
	const AosXmlTagPtr &gic,
	const OmnString &gic_type,
	const OmnString &superVpdname)
{

	if(gic_type == "gic_floatingvpd")
	{
		// if gic_type is "gic_floatingvpd", it will have gic_vpdname, 
		// insert to mVpdArray...
		OmnString vpdname = gic->getAttrStr("gic_vpdname");
		bool insertFlag = true;
		if (vpdname!= "")
		{
			for (int i=0; i<(int)mVpdArray.size(); i++)
			{
				if (vpdname == mVpdArray[i])
					insertFlag = false;
			}
			if (insertFlag)
			{
				mVpdArray.push_back(vpdname);

				OmnString intervpdname;
				intervpdname << mPageNumber << "," << " ,"<< superVpdname << "," << vpdname << ",gic";
				//intervpdname << mPageNumber << "," << " "<< "," << vpdname;
				mVpdnamelist.push_back(intervpdname);
			}
		}
	}
	else if(gic_type == "gic_tabmenu" || gic_type == "gic_adv_tab")
	{
		// gic_vpdnames="_0|$|_1|$|_2|$|_3|$|_4"
		OmnString vpdname = gic->getAttrStr("gic_vpdname");
		OmnStrParser1 parser(vpdname, "|$|");
		OmnString subVpdname;
		bool insertFlag = true;
		while ((subVpdname = parser.nextWord()) != "")
		{

			if (vpdname!= "")
			{
				for (int i=0; i<(int)mVpdArray.size(); i++)
				{
					if (vpdname == mVpdArray[i])
						insertFlag = false;
				}
				if (insertFlag)
				{
					mVpdArray.push_back(vpdname);

					OmnString intervpdname;
					intervpdname << mPageNumber << "," << " ,"<< superVpdname << "," << vpdname <<",gic";
					mVpdnamelist.push_back(intervpdname);
				}
			}
		}
	}
	else if (gic_type == "gic_container")
	{
		// if gic_type is gic_container, this gic like a panel
		procPanel(gic, superVpdname);
	}
	else if (gic_type == "gic_tablist" || gic_type == "gic_contactlist")
	{
		// gic_vpdnames
		// need to achieve
	}
	else if (gic_type == "gic_breadcrumbs" || 
			gic_type == "gic_verticalmenu" ||
			gic_type == "gic_sim_menu" ||
			gic_type == "gic_html_menu" ||
			gic_type == "gic_html_multimenu" ||
			gic_type == "gic_html_twolevelnav" ||
			gic_type == "gic_scrollmenu") 
	{
		// this condition is to parse the special gic,
		// this are all menu, like gic_breadcrumbs. gic_html_menu
		// its all gic data is stored in attribute gic_data
		// so should parse gic_data...
		AosXmlTagPtr gic_breadcrumbs;
		OmnString cont;
		if (gic_type == "gic_html_multimenu" || gic_type == "gic_html_twolevelnav")
		{
			cont = gic->getNodeText("contents");
		}
		else
		{
			cont = gic->getAttrStr("gic_data");
			if (cont != "")
			{
				int f1 = cont.findSubString("&lt;", 0);
				int f2 = cont.findSubString("&gt;", 0);
				int f3 = cont.findSubString("&quot;", 0);
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
			}
		}
		AosXmlParser parser;
		AosXmlTagPtr root = parser.parse(cont, "" AosMemoryCheckerArgs);
		if (!root || !(gic_breadcrumbs = root->getFirstChild()))
		{
			OmnScreen << "gic_breadcrumbs don't have exists";
			return false;
		}
		AosXmlTagPtr record = gic_breadcrumbs->getFirstChild("record");
		AosXmlTagPtr smallRecord;
		while (record)
		{
			AosXmlTagPtr actions = record->getFirstChild("actions");
			if (!actions)
			{
				OmnScreen << " This gic :" << gic_type << " don't have actions!" <<endl;
				record = gic_breadcrumbs->getNextChild();
				continue;
			}
			AosXmlTagPtr action = actions->getFirstChild("action");
			while (action)
			{
				OmnString actionid = action->getAttrStr("gic_type", "");
				if (actionid == "act_vpd" || actionid == "act_open_dialog")
				{
					procAction(action, gic_type, superVpdname);
				}
    		    action = actions->getNextChild();
    		}

			smallRecord = record->getFirstChild("record");
			while (smallRecord)
			{
				actions = smallRecord->getFirstChild("actions");
				if (actions)
				{
					AosXmlTagPtr action = actions->getFirstChild("action");
					while (action)
					{
						OmnString actionid = action->getAttrStr("gic_type", "");
						if (actionid == "act_vpd" || actionid == "act_open_dialog")
						{
							procAction(action, gic_type, superVpdname);
						}
    				    action = actions->getNextChild();
    				}
					actions = smallRecord->getNextChild();
				}
				smallRecord = record->getNextChild();
			}

			record = gic_breadcrumbs->getNextChild();
		}

		AosXmlTagPtr action_br = gic->getFirstChild("action");
		while (action_br && (action_br->getTagname() == "action") )
		{
			OmnString actionid = action_br->getAttrStr("gic_type", "");
			if (actionid == "act_vpd" || actionid == "act_open_dialog")
			{
				procAction(action_br, gic_type, superVpdname);
			}
        	action_br = gic->getNextChild();
		}
		return true;
	}
	
	AosXmlTagPtr actions = gic->getFirstChild("actions");
	if (!actions)
	{
		OmnScreen << " This gic :" << gic_type << " don't have actions!" <<endl;
		return false;
	}
	AosXmlTagPtr action = actions->getFirstChild("action");
	while (action)
	{
		OmnString actionid = action->getAttrStr("gic_type", "");
		if (actionid == "act_vpd" || actionid == "act_open_dialog")
		{
			procAction(action, gic_type, superVpdname);
		}
        action = actions->getNextChild();
    }
	return true;
}



bool
AosHtmlRetrieveSites::procAction(
	const AosXmlTagPtr &action,
	const OmnString &gic_type,
	const OmnString &superVpdname)
{
	AosXmlTagPtr parm = action->getFirstChild(AOSTAG_PARM);
	bool insertFlag = true;
	while (parm)
	{
		OmnString pname = parm->getAttrStr(AOSTAG_PNAME);
		if((pname == "vpd_name" || pname == "vpdname") &&
				parm->getAttrStr(AOSTAG_ID) == "const")
		{
			OmnString vpdname = parm->getNodeText();
			for (int i=0; i<(int)mVpdArray.size(); i++)
			{
				if (vpdname == mVpdArray[i])
					insertFlag = false;
			}
			if (insertFlag)
			{
				mVpdArray.push_back(vpdname);

				OmnString intervpdname;
				intervpdname << mPageNumber << "," << gic_type << "," << superVpdname << "," << vpdname << ",action";
				mVpdnamelist.push_back(intervpdname);
			}
		}
		parm = action->getNextChild();
	}
	return true;
}


bool
AosHtmlRetrieveSites::convertToXml(
		const std::vector<OmnString> &mVpdnamelist,
		OmnString &xmlStr,
		OmnString &errmsg)
{
	int ss = mVpdnamelist.size();
	for (int i=0; i<ss; i++)
	{
		AosStrSplit split;
		OmnString parts[5];
		int nn = split.splitStr(mVpdnamelist[i], ",", parts, 5);
		if (nn != 5)
		{
			errmsg = "Format is Error!";
			return false;
		}
		OmnString gic_type = parts[1];
		OmnString superVpdname = parts[2];
		OmnString vpdname = parts[3];
		OmnString gic_frome = parts[4];

		int part0 = atoi(parts[0].data());

		if (part0 == 0)
		{
			xmlStr << "<root vpdname=\"" << vpdname << "\">";
		}

		if (part0 == 1)
		{
			if (gic_type == " ")
				xmlStr << "<record vpdname=\"" << vpdname << "\"  gic_frome=\"" << gic_frome << "\"></record>";
			else
				xmlStr << "<record vpdname=\"" << vpdname << "\" gic_type=\"" << gic_type << "\" gic_frome=\"" << gic_frome << "\"></record>";
		}

		if (part0 > 1)
		{
			int subpos = xmlStr.findSubString(superVpdname, 0);
			OmnString strtmp;
			if (gic_type == " ")
				strtmp << "<record vpdname=\"" << vpdname << "\"  gic_frome=\"" << gic_frome << "\"></record>";
			else
				strtmp << "<record vpdname=\"" << vpdname << "\" gic_type=\"" << gic_type << "\" gic_frome=\"" << gic_frome << "\"></record>";

			subpos = xmlStr.findSubString(">", subpos);
			bool rslt = xmlStr.insert(strtmp, subpos+1);
			if (!rslt)
			{
				errmsg << "Can't insert: " << vpdname << "to" << xmlStr << "</root>";
				return false;
			}
		}
	}
	xmlStr << "</root>";
	return true;
}

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
// 03/28/2011: Created by Phnix
////////////////////////////////////////////////////////////////////////////
#include "SEModules/CodeComp.h"

#include "SEInterfaces/DocClientObj.h"
#include "Rundata/Rundata.h"
#include "SearchEngine/DocServer.h"
#include "Security/Session.h"
#include "SEModules/CopyVpd.h"
#include "SEUtilServer/CloudidSvr.h"
#include "SEUtil/DocTags.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/UtUtil.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/IILClientObj.h"

AosCodeComp::AosCodeComp()
{
}

AosCodeComp::~AosCodeComp()
{
}


bool 
AosCodeComp::copyVpd(
		const AosRundataPtr &rdata,
		const OmnString &objid,
		const OmnString &objid_resolve_type,
		const OmnString &stradd,
		const OmnString &tags,
		const OmnString &container)
{
	// This function recursively copies the vpd 'objid' and all the
	// vpds this vpd uses.

	u64 userid = rdata->getUserid();
	OmnString cid = AosCloudidSvr::getCloudid(userid, rdata);

	mVpdsList.clear();
	mObjidResolveType = objid_resolve_type;
	mStrAdd = stradd;
	mTags = tags;
	mContainer = container;

	// Add the vpd objid to the list
	DoubleVpdName doubleVpdName;
	doubleVpdName.oldVpdname = objid;
	addToVpdList(doubleVpdName, rdata);

	// Process all the VPDs in the list.
	for (u32 i=0; i<mVpdsList.size(); i++)
	{
		AosXmlTagPtr firstObj = AosDocClientObj::getDocClient()->getDocByObjid(
				mVpdsList[i].oldVpdname, rdata);

		//AosSetError(rdata, "failed_ret_vpd_001");
		//OmnAlarm << rdata->getErrmsg() << enderr;
		if (!firstObj)
		{
			if (i == 0) return false;
			mVpdsList[i].flag = true;
			continue;
		}

		AosXmlTagPtr cloned_doc = firstObj->clone(AosMemoryCheckerArgsBegin);
		cloned_doc->setAttr(AOSTAG_OBJID, mVpdsList[i].newVpdname);
		retrieveMemberVpds(cloned_doc, rdata);
		
		// Prepare the vpd
		prepareVpd(cloned_doc);
		cloned_doc->setAttr(AOSTAG_CTNR_PUBLIC, "true");
		OmnString docstr = cloned_doc->toString();

		mVpdsList[i].flag = true;
		//AosXmlTagPtr newDoc = AosDocClient::getSelf()->createDocSafe1(rdata, docstr, 
		//			cid, "", true, false, false, false, true, false);
		AosXmlTagPtr newDoc = AosDocClientObj::getDocClient()->createDocSafe1(rdata, docstr, 
					cid, "", true, false, false, false, true, true);
	}

	bool rst = checkList();
	if (!rst)
	{
		rdata->setError() << "Failed To ONE_KEY_COPY this obj: " << objid;
		OmnAlarm << rdata->getErrmsg() << enderr;
	}
	else
	{
		OmnString &str = rdata->getResults();
		str = "";
		str << "<Contents>";
		for (u32 i=0; i<mVpdsList.size(); i++)
		{
			str << "<record oldname=\"" << mVpdsList[i].oldVpdname
				<< "\" newname=\"" << mVpdsList[i].newVpdname << "\"/>";
		}
		str << "</Contents>";
	}
	return true;
}


bool
AosCodeComp::procCompoundPane(const AosXmlTagPtr &compoundPane, const AosRundataPtr &rdata)
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
			procCompoundPane(pane, rdata);
		}
		else if(pane_type == "normal_pane")
		{
			procPane(pane, rdata);
		}
		pane = panes->getNextChild();
	}
	return true;
}


bool
AosCodeComp::procPane(const AosXmlTagPtr &pane, const AosRundataPtr &rdata)
{
	OmnString panel_vpd_name = pane->getAttrStr("panel_vpd_name");
	if (panel_vpd_name != "")
	{

		DoubleVpdName doubleVpdName;
		doubleVpdName.oldVpdname = panel_vpd_name;
		doubleVpdName.newVpdname = "";
		bool rslt = doesVpdnameExist(doubleVpdName);
		if (!rslt)
		{
			addToVpdList(doubleVpdName, rdata);
		}
		pane->setAttr("panel_vpd_name", doubleVpdName.newVpdname);

		OmnScreen << pane->toString() << endl;
	}
	else
	{
		AosXmlTagPtr panel = pane->getFirstChild("panel");
		while (panel)
		{
			procPanel(panel, rdata);
			panel = pane->getNextChild();
		}
	}
	return true;
}


bool
AosCodeComp::procPanel(const AosXmlTagPtr &panel, const AosRundataPtr &rdata)
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
		if (gic_type == "")
		{
			OmnScreen << "This gic:  " << gic_type << " don't have gic_type" << endl;
			gic = gic_creators->getNextChild();
			continue;
		}
		
		procGic(gic, gic_type, rdata);
		gic = gic_creators->getNextChild();
    }
	return true;
}

bool
AosCodeComp::procGic(
	const AosXmlTagPtr &gic,
	const OmnString &gic_type, 
	const AosRundataPtr &rdata)
{
	//felicia, 2011-11-26 for htmlcomposer action
	// 1. for grid
	AosXmlTagPtr creators = gic->getFirstChild("creators");
	OmnString cont;
	AosXmlTagPtr contents;
	if (creators)
	{
		AosXmlTagPtr column = creators->getFirstChild("column");
		while(column)
		{
			if (gic_type == "gic_html_tab")
			{
				OmnString vpdname = column->getAttrStr("gic_vpdname");
				if (vpdname!= "")
				{
					DoubleVpdName doubleVpdName;
					doubleVpdName.oldVpdname = vpdname;
					doubleVpdName.newVpdname = "";
					bool rslt = doesVpdnameExist(doubleVpdName);
					if (!rslt)
					{
						addToVpdList(doubleVpdName, rdata);
					}
					column->setAttr("gic_vpdname", doubleVpdName.newVpdname);
				}
			}
			contents = column->getFirstChild("contents");
			if (contents)
			{
				cont = contents->getNodeText();
				procComposerAction(cont, gic_type, rdata);
				contents->setNodeText(cont, true);
			}
			column = creators->getNextChild("column");
		}
	}

	// 2. for html, dataview, list
	if (gic_type == "gic_html_dataview" && creators)
	{
		AosXmlTagPtr creator = creators->getFirstChild("creator");
		if (creator)
		{
			contents = creator->getFirstChild("contents");
		}
	}
	else
	{
		contents = gic->getFirstChild("contents");
	}
	
	if (contents && (gic_type == "gic_html_html" 
				|| gic_type == "gic_html" 
				|| gic_type == "gic_html_list"
				|| gic_type == "gic_html_dataview"))
	{
		cont = contents->getNodeText();
		procComposerAction(cont, gic_type, rdata);
		contents->setNodeText(cont, true);
	}

	if(gic_type == "gic_floatingvpd")
	{
		// if gic_type is "gic_floatingvpd", it will have gic_vpdname, 
		// insert to mVpdArray...
		OmnString vpdname = gic->getAttrStr("gic_vpdname");
		if (vpdname!= "")
		{
			DoubleVpdName doubleVpdName;
			doubleVpdName.oldVpdname = vpdname;
			doubleVpdName.newVpdname = "";
			bool rslt = doesVpdnameExist(doubleVpdName);
			if (!rslt)
			{
				addToVpdList(doubleVpdName, rdata);
			}
			gic->setAttr("gic_vpdname", doubleVpdName.newVpdname);
		}
	}
	else if(gic_type == "gic_tabmenu" || gic_type == "gic_adv_tab")
	{
		// gic_vpdnames="_0|$|_1|$|_2|$|_3|$|_4"
		OmnString vpdname = gic->getAttrStr("gic_vpdname");
		AosStrSplit parser(vpdname, "|$|");
		OmnString subVpdname;
		vector<OmnString> tabVector;
		while ((subVpdname = parser.nextWord()) != "")
		{

			if (subVpdname!= "")
			{
				DoubleVpdName doubleVpdName;
				doubleVpdName.oldVpdname = subVpdname;
				doubleVpdName.newVpdname = "";

				bool rslt = doesVpdnameExist(doubleVpdName);
				if (!rslt)
				{
					addToVpdList(doubleVpdName, rdata);
				}
				tabVector.push_back(doubleVpdName.newVpdname);
			}
		}
		OmnString strSet;
		u32 i=0;
		for(i=0; i<tabVector.size(); i++)
		{
			strSet << tabVector[i];
			if (i < tabVector.size()-1)
			{
				strSet << "|$|";
			}
		}

		gic->setAttr("gic_vpdname", strSet);
	}
	else if (gic_type == "gic_container")
	{
		// if gic_type is gic_container, this gic like a panel
		procPanel(gic, rdata);
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
		// gic_breadcrumbs format is strange..
		// <record>
		//  	<actions>
		//  	</actions>
		//  	<record>
		//  	</record>
		//  	<record>
		//  	</record>
		// </record>
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
				procAction(action, gic_type, rdata);
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
						procAction(action, gic_type, rdata);
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
			procAction(action_br, gic_type, rdata);
        	action_br = gic->getNextChild();
		}
		
		OmnString rootStr = root->toString();
		if (gic_type == "gic_html_multimenu" || gic_type == "gic_html_twolevelnav")
		{
			if (contents)
			{
				contents->setNodeText(rootStr, true);
			}
			//cont = gic->getNodeText("contents");
		}
		else
		{
			// change rootStr to &quot; &lt;...format
			if (rootStr != "")
			{
				int f1 = rootStr.findSubString("<", 0);
				int f2 = rootStr.findSubString(">", 0);
				int f3 = rootStr.findSubString("\"", 0);
				if(f1 != -1) 
				{
					rootStr.replace("<", "&lt;", true);
				}
				if(f2 != -1) 
				{
					rootStr.replace(">", "&gt;", true);
				}
				if(f3 != -1) 
				{
					rootStr.replace("\"", "&quot;", true);
				}
			}
			gic->setAttr("gic_data", rootStr);
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
		procAction(action, gic_type, rdata);
        action = actions->getNextChild();
    }
	return true;
}

void
AosCodeComp::procComposerAction(
		OmnString &cont,
		const OmnString &gic_type,
		const AosRundataPtr &rdata)
{
	int find, findend, end;
	find = cont.findSubString("<actions", 0);
	while (find != -1)
	{
		findend = cont.findSubString("</actions>", find+4);
		if (findend == -1)
		{
			find = cont.findSubString("<actions", find+4);
			continue;
		}
		end = findend + 9;
		if (findend != -1 && findend > find)
		{
			OmnString actions = cont.substr(find, end);
			OmnString preactions = actions;
			actions.replace("|&&|", "\"", true);
			actions.replace("|&|", "\"", true);
			AosXmlParser parser;
			AosXmlTagPtr actdoc = parser.parse(actions, "" AosMemoryCheckerArgs);
			if (actdoc)
			{
				AosXmlTagPtr action = actdoc->getFirstChild("action");
				while (action)
				{
				    procAction(action, gic_type, rdata);
				    action = actdoc->getNextChild();
				}
				OmnString newactions = actdoc->toString();
				newactions.replace("\"", "|&&|", true);
				newactions.replace("\'", "|&|", true);
				cont.replace(preactions, newactions, false);
			}
		}
		find = cont.findSubString("<actions", end-1);
	}
}

bool
AosCodeComp::procAction(
	const AosXmlTagPtr &action,
	const OmnString &gic_type, 
	const AosRundataPtr &rdata)
{
	AosXmlTagPtr parm = action->getFirstChild(AOSTAG_PARM);
	while (parm)
	{
		OmnString pname = parm->getAttrStr(AOSTAG_PNAME);
		if((pname == "vpdname" || pname == "vpd_name") &&
				parm->getAttrStr(AOSTAG_ID) == "const")
		{
			OmnString vpdname = parm->getNodeText();

			DoubleVpdName doubleVpdName;
			doubleVpdName.oldVpdname = vpdname;
			doubleVpdName.newVpdname = "";
			bool rslt = doesVpdnameExist(doubleVpdName);

			if (!rslt)
			{
				addToVpdList(doubleVpdName, rdata);
			}
			parm->setNodeText("", doubleVpdName.newVpdname, false);
		}

		parm = action->getNextChild();
	}
	return true;
}


bool 
AosCodeComp::doesVpdnameExist(DoubleVpdName &vpdInfo)
{
	for (u32 i = 0; i < mVpdsList.size(); i++)
	{
		if (vpdInfo.oldVpdname == mVpdsList[i].oldVpdname)
		{
			vpdInfo.newVpdname = mVpdsList[i].newVpdname;
			vpdInfo.flag = false;
			return true;
		}
	}
	return false;
}


bool
AosCodeComp::retrieveMemberVpds(const AosXmlTagPtr &vpd, const AosRundataPtr &rdata)
{
	aos_assert_r(vpd, false);

	// retrieve all the vpd names in AosXmlTagPtr vpd...
	// get vpd names list
	// convert list to xml format
	OmnString newXmlStr;

	OmnString vpdtype = vpd->getAttrStr("container_type");
OmnString objid = vpd->getAttrStr("zky_objid");
if (objid != "")
{
	OmnScreen << "vpdname == " << objid << endl;
}

    if (vpdtype == "")
        vpdtype = vpd->getAttrStr("pane_type");

	if (vpdtype == "")
	    vpdtype = vpd->getAttrStr("panel_type");

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

	//felicia,2012/05/29 for AOSTAG_AFTLGI_VPD
	OmnString vpdname = vpd->getAttrStr(AOSTAG_AFTLGI_VPD, "");
	if (vpdname != "")
	{
		DoubleVpdName doubleVpdName;
		doubleVpdName.oldVpdname = vpdname;
		doubleVpdName.newVpdname = "";
		bool rslt = doesVpdnameExist(doubleVpdName);
		if (!rslt)
		{
			addToVpdList(doubleVpdName, rdata);
		}
		vpd->setAttr(AOSTAG_AFTLGI_VPD, doubleVpdName.newVpdname);
	}


	if (vpdtype == "editor_panel" || vpdtype == "atomic_panel")
	{
		procPanel(vpd, rdata);
	}
	else if (vpdtype == "normal_pane")
	{
		procPane(vpd, rdata);
	}
	else if (vpdtype == "PWT" || vpdtype == "pwt" || vpdtype == "comp_pane")
	{
		procCompoundPane(vpd, rdata);
	}
	else if (vpdtype == "gic")
	{
		procGic(vpd, vpd->getAttrStr("gic_type"), rdata);
	}
	else 
	{
		OmnScreen << "the vpd, " << vpd->getAttrStr("zky_objid") 
			<< ", have no vpdtype." << endl;
	}

	return true;
}


bool
AosCodeComp::checkList()
{
	for (u32 i = 0; i < mVpdsList.size(); i++)
	{
		if (mVpdsList[i].flag == false)
		{
			return false;
		}
	}
	return true;
}


bool
AosCodeComp::addToVpdList(DoubleVpdName &vpdInfo, const AosRundataPtr &rdata)
{
	// 1. Create the new VPD objid.
	vpdInfo.newVpdname = AosCopyVpd::getNextVpdObjid(
			mObjidResolveType, vpdInfo.oldVpdname, mStrAdd, rdata);
	vpdInfo.flag = false;
	aos_assert_r(vpdInfo.newVpdname != "", false);

	// 2. Ensure the new vpd objid is unique
	u64 docid = AOS_INVDID;
	int startidx, endidx, index = 0;
	OmnString oldidx, suffidx, objid = vpdInfo.newVpdname;
	bool isunique = false;
	//bool rslt = AosIILClient::getSelf()->getDocidByObjid(
	bool rslt = AosIILClientObj::getIILClient()->getDocidByObjid(
			rdata->getSiteid(), objid, docid, isunique, rdata);
	aos_assert_r(rslt, false);
	while (docid != AOS_INVDID)
	{
		// The new objid is used by someone. Need to change it to the form:
		// 	objid(ddd)
		// where 'ddd' is an integer.
		suffidx = "";
		startidx = objid.indexOf(0, '(', true);
		endidx = objid.indexOf(0, ')', true);
		if (startidx < 0 || endidx <0 || endidx <= startidx+1) 
		{
			suffidx << "(" << index << ")";
			objid << suffidx;
		}
		else
		{
			suffidx << "(" << ++index << ")";
			const char *data = objid.data();
			OmnString id(data, startidx);
			id << suffidx;
			objid = id;
		}
		//rslt = AosIILClient::getSelf()->getDocidByObjid(
		rslt = AosIILClientObj::getIILClient()->getDocidByObjid(
			rdata->getSiteid(), objid, docid, isunique, rdata);
	}
	vpdInfo.newVpdname = objid;
	mVpdsList.push_back(vpdInfo);
	return true;
}


bool
AosCodeComp::prepareVpd(const AosXmlTagPtr &vpd)
{
	vpd->removeAttr(AOSTAG_DOCID);

	if (mTags != "")
	{
		OmnString tags = vpd->getAttrStr(AOSTAG_TAG);
		OmnString tt = AosComposeByWords(mTags, tags, ",");
		vpd->setAttr(AOSTAG_TAG, tt);
	}

	if (mContainer != "")
	{
		vpd->setAttr(AOSTAG_PARENTC, mContainer);
	}
	return true;
}


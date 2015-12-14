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
// 07/07/2010: Created by Lynch Yang
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicContainer.h"

#include "HtmlUtil/HtmlUtil.h"
#include "HtmlServer/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
//#include "Thread/ThreadShellMgr.h"
#include "Thread/ThreadPool.h"
#include <vector>

//static AosGicPtr sgGic = new AosGicContainer();

static OmnThreadPoolPtr sgThreadPool = OmnNew OmnThreadPool("GicContainer", __FILE__, __LINE__);

AosGicContainer::AosGicContainer(const bool flag)
:
AosGic(AOSGIC_CONTAINER, AosGicType::eContainer, flag)
{
}


AosGicContainer::~AosGicContainer()
{
}


bool	
AosGicContainer::generateCode(
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
	// Ketty 2012/02/09
	AosXmlTagPtr lytChild = vpd->getFirstChild("layoutmgr");
	OmnString layoutType;
	if (lytChild)
	{
		layoutType = lytChild->getAttrStrSimp("lm_name", "lm_fixed");
	}
	else
	{
		layoutType = "lm_fixed";
	}

	code.mJson <<",gic_isflash:" << vpd->getAttrStrSimp("gic_isflash", "false") 
		<<",gcn_owngics:" << vpd->getAttrStrSimp("gcn_owngics", "true") 
		<<",gcn_cvpd:\"" << vpd->getAttrStrSimp("gcn_cvpd") << "\""
		<<",gcn_overcolor:\"" << AosHtmlUtil::getWebColor(vpd->getAttrStrSimp("gcn_overcolor", "transparent")) << "\""
		<<",gcn_downcolor:\"" << AosHtmlUtil::getWebColor(vpd->getAttrStrSimp("gcn_downcolor", "transparent")) << "\""
		<<",gcn_bgcolor:\"" << AosHtmlUtil::getWebColor(vpd->getAttrStrSimp("gcn_bgcolor", "transparent")) << "\""
		<<",border_show:" << vpd->getAttrStrSimp("border_show", "false") 
		<<",border_size:\"" << vpd->getAttrStrSimp("border_size", "1") << "\""
		<<",border_color:\"" << AosHtmlUtil::getWebColor(vpd->getAttrStrSimp("border_color", "#66ff88")) << "\""
		<<",isGroup:" << "true"
		<<",mlLayoutType:\"" << layoutType << "\""
		<<",comps: ["; 
	
	if (obj)
	{
		AosRundataPtr rdata = htmlPtr->getRundata();
		OmnString infor = rdata->getArg1("createobj");
		if (infor == "objgic")
		{
			bool rslt = createObj(htmlPtr, vpd, obj, parentid, code, rdata);
			return rslt;
		}
		else if (infor == "subgic")
		{
			rdata->setArg1("createobj", "objgic");
			bool rslt = createSubGic(htmlPtr, vpd, obj, parentid, code, rdata);
			return rslt;
		}
	}
	
	
	AosXmlTagPtr thevpd = vpd->getFirstChild("gic_creators");
	if(thevpd)
	{
		AosXmlTagPtr nChild = thevpd->getFirstChild();
		while (nChild)
		{
			AosGic::createGic(htmlPtr, nChild, obj, parentid, code, -1, -1);	//Ketty 2011/09/22
			nChild = thevpd->getNextChild();
			if (nChild){
				code.mJson<<",";
			}
		}
	}
	code.mJson << "]";
	return true;
}

bool	
AosGicContainer::createObj(
		const AosHtmlReqProcPtr &htmlPtr,
	    AosXmlTagPtr &vpd,
  		const AosXmlTagPtr &obj,
		const OmnString &parentid,
 		AosHtmlCode &code,
		const AosRundataPtr &rdata)
{
	aos_assert_r(obj, false);
	bool hascreated = false;
		
	obj->setAttrSimp("zky_hascreate__xmleditor", "true");

	AosXmlTagPtr childvpd, root;
	int num = htmlPtr->getVpdNum();
	
	OmnString rgicid = obj->getAttrStrSimp("zky_rgicid__xmleditor", "");
	OmnString objx, objy, objw, objh;
	OmnString subobjid = obj->getAttrStrSimp("zky_subobjid__xmleditor", "");
	OmnString objpath = "", addgic = "", lastgic = "", path = "";
	
	rdata->setArg1("createobj", " ");
	bool hasnode = false;
	bool has_rgicid = false;
	AosXmlTagPtr thevpd = vpd->getFirstChild("gic_creators");
	if(thevpd)
	{
		AosXmlTagPtr nChild = thevpd->getFirstChild();
		while (nChild)
		{
			objpath = addgic = lastgic = "";
			nChild->setAttrSimp("gic_notrld", "true");
			if (rgicid != "")
			{
				if (rgicid == nChild->getAttrStrSimp("gic_instid", ""))
				{
					has_rgicid = true;
					nChild = thevpd->getNextChild();
					if (!nChild)
					{
						hasnode = true;
					}
					continue;
				}
			}
			OmnString gic_type = nChild->getAttrStrSimp("gic_type", "");
			if (gic_type == "gic_container")
			{
				getDefAttr(nChild, objpath, addgic, lastgic);
			}
			if (objpath != "")
			{
				htmlPtr->setVpdNum();
				int vpdnum = htmlPtr->getVpdNum();
				AosHtmlCode gicCode;
				AosXmlTagPtr cobj;
				if (objpath != "noneobj__xmleditor")
				{
					cobj = obj->xpathGetChild(objpath);
					setPos("zky_gicpos__xmleditor", cobj, nChild);
					if (addgic == "addwh__xmleditor")
					{
						addPos(cobj, nChild);
					}
					rdata->setArg1("createobj", "objgic");
					rdata->setArg1("createlastgic", "lastgic");
				}
				else
				{
					cobj = obj;
					//rdata->setArg1("createobj", "subgic");
					rdata->setArg1("createobj", "objgic");
					if (addgic == "addwh__xmleditor")
					{
						addPos(cobj, nChild);
					}
					else if (addgic == "addchildswh__xmleditor")
					{
						addChildPos(cobj, nChild);
					}
				}
				aos_assert_r(cobj, false);
				
				//thread shell
				if (gic_type == "container")
				{
//					OmnString path = cobj->getAttrStr("zky_objpath__xmleditor", "");
//					aos_assert_r(path != "", false);
//					AosStrSplit split(path, "/");          
//					vector<OmnString> strs= split.entriesV();
//					if (strs.size() > 4)
					if (isDeepLevel(cobj))
					{
						vector<OmnThrdShellProcPtr> runners;
						OmnThrdShellProcPtr runner = OmnNew	createSubContainer(this, htmlPtr, nChild, cobj, parentid, gicCode);
						runners.push_back(runner);
OmnScreen << " @@@@@ sgThreadPool  11111 " << endl;
						cobj->setAttrSimp("zky_has_looped", "true");
						sgThreadPool->procSync(runners);
						gicCode = mGicCode;
						//OmnThreadShellMgr::getSelf()->procSync(runners);
					}
					else
					{
						AosGic::createGic(htmlPtr, nChild, cobj, parentid, gicCode, -1, -1);	//Ketty 2011/09/22
					}
				}
				else
				{
					AosGic::createGic(htmlPtr, nChild, cobj, parentid, gicCode, -1, -1);	//Ketty 2011/09/22
				}
				code.mJs << "aosvpd[" << vpdnum << "] = gXmlParser.String2Xml(\'" << nChild->toString() << "\');";
				code.mJs << "aosobj[" << vpdnum << "] = gXmlParser.String2Xml(\'" << cobj->toString() << "\');";
				code.append(gicCode);
				rdata->setArg1("createobj", " ");
			}
			else
			{
				//thread shell
				if (gic_type == "container")
				{
//					OmnString path = obj->getAttrStr("zky_objpath__xmleditor", "");
//					aos_assert_r(path != "", false);
//					AosStrSplit split(path, "/");          
//					vector<OmnString> strs= split.entriesV();
//					if (strs.size() > 4)
					if (isDeepLevel(obj))
					{
						vector<OmnThrdShellProcPtr> runners;
						OmnThrdShellProcPtr runner = OmnNew	createSubContainer(this, htmlPtr, nChild, obj, parentid, code);
						runners.push_back(runner);
OmnScreen << " @@@@@ sgThreadPool  22222 " << endl;
						obj->setAttrSimp("zky_has_looped", "true");
						sgThreadPool->procSync(runners);
						code = mGicCode;
						//OmnThreadShellMgr::getSelf()->procSync(runners);
					}
					else
					{
						AosGic::createGic(htmlPtr, nChild, obj, parentid, code, -1, -1);	//Ketty 2011/09/22
					}
				}
				else
				{
					AosGic::createGic(htmlPtr, nChild, obj, parentid, code, -1, -1);	//Ketty 2011/09/22
				}
				//AosGic::createGic(htmlPtr, nChild, obj, parentid, code, -1, -1);	//Ketty 2011/09/22
			}
			nChild = thevpd->getNextChild();
			if (nChild){
				code.mJson<<",";
			}
		}
	}
	rdata->setArg1("createobj", "objgic");
	
	if (subobjid != "")
	{
		/*AosXmlTagPtr childobj = obj->getFirstChild();
		if (!childobj)
		{
			if (hasnode)
			{
				code.mJson = code.mJson.subString(0, code.mJson.length()-1);
			}
			code.mJson << "]";
			return false;
		}
		OmnString vpdname = childobj->getAttrStrSimp("zky_ctobjid__xmleditor", "");
		while(vpdname == "")
		{
			childobj = obj->getNextChild();
			vpdname = childobj->getAttrStrSimp("zky_ctobjid__xmleditor", "");
		}
		*/

		if (!has_rgicid)
		{
			if (hasnode)
			{
				code.mJson = code.mJson.subString(0, code.mJson.length()-1);
			}
			code.mJson << "]";
			code.mJson <<",aosobj: aosobj[" << num << "]"
					   <<",aosvpd: aosvpd[" << num << "]";
			return true;
		}

		bool rslt = htmlPtr->getVpd(subobjid, childvpd, root);
	
		if (!rslt)
		{
			if (hasnode)
			{
				code.mJson = code.mJson.subString(0, code.mJson.length()-1);
			}
			code.mJson << "]";
			return false;
		}
		if (childvpd->getAttrStrSimp("gic_type", "") != "gic_container")
		{
			if (hasnode)
			{
				code.mJson = code.mJson.subString(0, code.mJson.length()-1);
			}
			code.mJson << "]";
			return false;
		}

		if (!hasnode)
		{
			code.mJson << ",";
		}

		objpath = addgic = lastgic = "";
		getDefAttr(childvpd, objpath, addgic, lastgic);
		AosXmlTagPtr childobj;
		if (objpath == "noneobj__xmleditor")
		{
			childobj = obj;
		}
		else
		{
			childobj = obj->getFirstChild();
			if (!childobj)
			{
				if (hasnode)
				{
					code.mJson = code.mJson.subString(0, code.mJson.length()-1);
				}
				code.mJson << "]";
				return false;
			}
			OmnString vpdname = childobj->getAttrStrSimp("zky_ctobjid__xmleditor", "");
			while(vpdname == "")
			{
				childobj = obj->getNextChild();
				if (!childobj) break;
				vpdname = childobj->getAttrStrSimp("zky_ctobjid__xmleditor", "");
			}
			//childobj = obj->xpathGetChild(objpath);
		}
		aos_assert_r(childobj, false);
		htmlPtr->setVpdNum();
		int vpdnum = htmlPtr->getVpdNum();

		rslt = setPos("zky_subgicpos__xmleditor", obj, childvpd);
		aos_assert_r(rslt, false);

		if (rgicid != "")
		{
			childvpd->setAttrSimp("gic_instid", rgicid);
		}


		AosHtmlCode gicCode;

		//OmnString child_rgicid = childobj->getAttrStrSimp("zky_rgicid__xmleditor", "");
		if (addgic == "objgic__xmleditor")
		{
			rdata->setArg1("createobj", "objgic");
			//thread shell
//			OmnString path = childobj->getAttrStr("zky_objpath__xmleditor", "");
//			aos_assert_r(path != "", false);
//			AosStrSplit split(path, "/");          
//			vector<OmnString> strs= split.entriesV();
//			if (strs.size() > 4)
			if (isDeepLevel(childobj))
			{
				vector<OmnThrdShellProcPtr> runners;
				OmnThrdShellProcPtr runner = OmnNew	createSubContainer(this, htmlPtr, childvpd, childobj, parentid, gicCode);
				runners.push_back(runner);

OmnScreen << " @@@@@ sgThreadPool  33333 " << endl;
				childobj->setAttrSimp("zky_has_looped", "true");
				sgThreadPool->procSync(runners);
				gicCode = mGicCode;
				//OmnThreadShellMgr::getSelf()->procSync(runners);
			}
			else
			{
				AosGic::createGic(htmlPtr, childvpd, childobj, parentid, gicCode, -1, -1);	//Ketty 2011/09/22
			}
			//AosGic::createGic(htmlPtr, childvpd, childobj, parentid, gicCode, -1, -1);
		}
		else
		{
			rdata->setArg1("createobj", "subgic");
			//thread shell
//			OmnString path = obj->getAttrStr("zky_objpath__xmleditor", "");
//			aos_assert_r(path != "", false);
//			AosStrSplit split(path, "/");          
//			vector<OmnString> strs= split.entriesV();
//			if (strs.size() > 4)
			if (isDeepLevel(obj))
			{
				vector<OmnThrdShellProcPtr> runners;
				OmnThrdShellProcPtr runner = OmnNew	createSubContainer(this, htmlPtr, childvpd, obj, parentid, gicCode);
				runners.push_back(runner);
OmnScreen << " @@@@@ sgThreadPool  44444 " << endl;
				obj->setAttrSimp("zky_has_looped", "true");
				sgThreadPool->procSync(runners);
				gicCode = mGicCode;
				//OmnThreadShellMgr::getSelf()->procSync(runners);
			}
			else
			{
				AosGic::createGic(htmlPtr, childvpd, obj, parentid, gicCode, -1, -1);	//Ketty 2011/09/22
			}
			//AosGic::createGic(htmlPtr, childvpd, obj, parentid, gicCode, -1, -1);
		}
			
		//rdata->setArg1("createobj", "subgic");
		//AosGic::createGic(htmlPtr, childvpd, obj, parentid, gicCode, -1, -1);

		code.mJs << "aosvpd[" << vpdnum << "] = gXmlParser.String2Xml(\'" << childvpd->toString() << "\');";
		//code.mJs << "aosobj[" << vpdnum << "] = gXmlParser.String2Xml(\'" << obj->toString() << "\');";
		code.mJs << "aosobj[" << vpdnum << "] = gXmlParser.String2Xml(\'" << childobj->toString() << "\');";
		code.append(gicCode);
	}
	else
	{
		AosXmlTagPtr childobj = obj->getFirstChild();

		AosXmlTagPtr mchild = childobj;
		while(childobj)
		{
			hascreated = childobj->getAttrBool("zky_hascreate__xmleditor", false);
			if (hascreated)
			{
				childobj = obj->getNextChild();
				continue;
			}
			OmnString vpdname = childobj->getAttrStrSimp("zky_ctobjid__xmleditor", "");
			if (vpdname == "")
			{
				childobj = obj->getNextChild();
				continue;
			}
			bool rslt = htmlPtr->getVpd(vpdname, childvpd, root);  
			
			if (!rslt)
			{
				if (hasnode)
				{
					code.mJson = code.mJson.subString(0, code.mJson.length()-1);
				}
				code.mJson << "]";
				return false;
			}
			if (childvpd->getAttrStrSimp("gic_type", "") != "gic_container")
			{
				if (hasnode)
				{
					code.mJson = code.mJson.subString(0, code.mJson.length()-1);
				}
				code.mJson << "]";
				return false;
			}
			
			if (!hasnode)
			{
				code.mJson << ",";
			}

			htmlPtr->setVpdNum();
			int vpdnum = htmlPtr->getVpdNum();

			rslt = setPos("zky_gicpos__xmleditor", childobj, childvpd);
			aos_assert_r(rslt, false);
			AosHtmlCode gicCode;
			//thread shell
//			OmnString path = childobj->getAttrStr("zky_objpath__xmleditor", "");
//			aos_assert_r(path != "", false);
//			AosStrSplit split(path, "/");          
//			vector<OmnString> strs= split.entriesV();
//			if (strs.size() > 4)
			if (isDeepLevel(childobj))
			{
				vector<OmnThrdShellProcPtr> runners;
				OmnThrdShellProcPtr runner = OmnNew	createSubContainer(this, htmlPtr, childvpd, childobj, parentid, gicCode);
				runners.push_back(runner);
OmnScreen << " @@@@@ sgThreadPool  55555 " << endl;
				childobj->setAttrSimp("zky_has_looped", "true");
				sgThreadPool->procSync(runners);
				gicCode = mGicCode;
				//OmnThreadShellMgr::getSelf()->procSync(runners);
			}
			else
			{
				AosGic::createGic(htmlPtr, childvpd, childobj, parentid, gicCode, -1, -1);	//Ketty 2011/09/22
			}
			//AosGic::createGic(htmlPtr, childvpd, childobj, parentid, gicCode, -1, -1);

			code.mJs << "aosvpd[" << vpdnum << "] = gXmlParser.String2Xml(\'" << childvpd->toString() << "\');";
			code.mJs << "aosobj[" << vpdnum << "] = gXmlParser.String2Xml(\'" << childobj->toString() << "\');";
			code.append(gicCode);
			childobj = obj->getNextChild();
		}
	}

	code.mJson << "]";
	code.mJson <<",aosobj: aosobj[" << num << "]"
			   <<",aosvpd: aosvpd[" << num << "]";
	return true;
}

bool
AosGicContainer::setPos(
		const OmnString attrname,
		const AosXmlTagPtr &obj,
		const AosXmlTagPtr &vpd)
{
	aos_assert_r(obj, false);
	aos_assert_r(vpd, false);
	OmnString objx, objy, objw, objh;
	OmnString pos = obj->getAttrStrSimp(attrname.data(), "");
	if (pos != "")
	{
		AosStrSplit split;
		bool finished;
		OmnString pair[4];
		split.splitStr(pos.data(), "|$|", pair, 4, finished);
		objx = pair[0];
		objy = pair[1];
		objw = pair[2];
		objh = pair[3];
	}
	if (objx == "") objx = vpd->getAttrStrSimp("gic_x", "0");
	if (objy == "") objy = vpd->getAttrStrSimp("gic_y", "0");
	if (objw == "") objw = vpd->getAttrStrSimp("gic_width", "0");
	if (objh == "") objy = vpd->getAttrStrSimp("gic_height", "0");
	vpd->setAttrSimp("gic_x", objx);
	vpd->setAttrSimp("gic_y", objy);
	vpd->setAttrSimp("gic_width", objw);
	vpd->setAttrSimp("gic_height", objh);
	vpd->setAttrSimp("gic_notrld", "true");
	
	return true;
}

bool
AosGicContainer::addPos(
		const AosXmlTagPtr &obj,
		const AosXmlTagPtr &vpd)
{
	aos_assert_r(obj, false);
	aos_assert_r(vpd, false);
	OmnString objh, objh2;
	objh = vpd->getAttrStrSimp("gic_height", "0");

	OmnString pos2 = obj->getAttrStrSimp("zky_subgicpos__xmleditor", "");
	if (pos2 != "")
	{
		AosStrSplit split;
		bool finished;
		OmnString pair[4];
		split.splitStr(pos2.data(), "|$|", pair, 4, finished);
		objh2 = pair[3];
	}
	if (objh2 != "")
	{
		int h = atoi(objh.data()) + atoi(objh2.data());
		objh = "";
		objh << h;
	}
	
	vpd->setAttrSimp("gic_height", objh);
	
	return true;
}

bool
AosGicContainer::addChildPos(
		const AosXmlTagPtr &obj,
		const AosXmlTagPtr &vpd)
{
	aos_assert_r(obj, false);
	aos_assert_r(vpd, false);
	OmnString objh, objh2;
	objh = vpd->getAttrStrSimp("gic_height", "0");

	AosXmlTagPtr child = obj->getFirstChild();
	while(child)
	{
		OmnString pos2 = child->getAttrStrSimp("zky_gicpos__xmleditor", "");
		if (pos2 != "")
		{
			AosStrSplit split;
			bool finished;
			OmnString pair[4];
			split.splitStr(pos2.data(), "|$|", pair, 4, finished);
			objh2 = pair[3];
		}
		if (objh2 != "")
		{
			int h = atoi(objh.data()) + atoi(objh2.data());
			objh = "";
			objh << h;
		}
		child = obj->getNextChild();
	}
	
	vpd->setAttrSimp("gic_height", objh);
	return true;
}

void
AosGicContainer::getDefAttr(
		const AosXmlTagPtr &nChild,
		OmnString &objpath,
		OmnString &addgic,
		OmnString &lastgic)
{
	OmnString gic_dftattr = nChild->getAttrStrSimp("gic_selfDefAttr", "");
	objpath = addgic = lastgic = "";
	if (gic_dftattr != "")
	{
		AosStrSplit split;
		OmnString attrs[20];
		bool finished;
		int nums = split.splitStr(gic_dftattr.data(), "|$$|", attrs, 20, finished);
		if (nums >= 1)
		{
			OmnString names[3];
			for(int i=0; i< nums; i++)
			{
				int strnum = split.splitStr(attrs[i].data(), "|$|", names, 3, finished);
				if (strnum >= 1)
				{
					if (names[0] == "objpath")
					{
						objpath = names[1];
					}

					if (names[0] == "addgic")
					{
						addgic = names[1];
					}

					if (names[0] == "lastgic")
					{
						lastgic = names[1];
					}
				}
			}
		}
	}
}

bool	
AosGicContainer::createSubGic(
		const AosHtmlReqProcPtr &htmlPtr,
	    AosXmlTagPtr &vpd,
  		const AosXmlTagPtr &obj,
		const OmnString &parentid,
 		AosHtmlCode &code,
		const AosRundataPtr &rdata)
{
	aos_assert_r(obj, false);
	OmnString objpath = "", addgic = "", lastgic = "";
	AosXmlTagPtr newobj = obj;
	AosXmlTagPtr thevpd = vpd->getFirstChild("gic_creators");
	getDefAttr(vpd, objpath, addgic, lastgic);
	if (lastgic != "")
	{
		newobj = obj->xpathGetChild(lastgic);
		aos_assert_r(newobj, false);
		objpath = addgic = lastgic = "";
	}
	int vpdnum = htmlPtr->getVpdNum();
	if(thevpd)
	{
		vector<AosXmlTagPtr> objs;
		AosXmlTagPtr nChild = thevpd->getFirstChild();
		int childnum = 0;
		while (nChild)
		{
			getDefAttr(nChild, objpath, addgic, lastgic);
			
			OmnString gic_type = nChild->getAttrStrSimp("gic_type", "");
			nChild->setAttrSimp("gic_notrld", "true");
			
			if (gic_type == "gic_container" && objpath != "")// && objpath != "noneobj__xmleditor")//isObj)
			{
				int subtags = obj->getNumSubtags();
				if (subtags > 0)
				{
					AosXmlTagPtr childobj;
					if (objpath == "noneobj__xmleditor")
					{
						childobj = newobj;
					}
					else if (lastgic != "")
					{
						aos_assert_r(newobj, false);
						childobj = newobj->xpathGetChild(lastgic);
						aos_assert_r(childobj, false);
					}
					else
					{
						childobj = obj->getChild(childnum);
					}
					if (childobj)
					{
						if (objpath != "noneobj__xmleditor")
						{
							bool rslt = setPos("zky_gicpos__xmleditor", childobj, nChild);
							aos_assert_r(rslt, false);
							rdata->setArg1("createobj", "objgic");
						}
						else
						{
							rdata->setArg1("createobj", "subgic");
						}
						htmlPtr->setVpdNum();
						int avpdnum = htmlPtr->getVpdNum();
						//thread shell
						if (gic_type == "container")
						{
//							OmnString path = childobj->getAttrStr("zky_objpath__xmleditor", "");
//							aos_assert_r(path != "", false);
//							AosStrSplit split(path, "/");          
//							vector<OmnString> strs= split.entriesV();
//							if (strs.size() > 4)
							if (isDeepLevel(childobj))
							{
								vector<OmnThrdShellProcPtr> runners;
								OmnThrdShellProcPtr runner = OmnNew	createSubContainer(this, htmlPtr, nChild, childobj, parentid, code);
								runners.push_back(runner);
OmnScreen << " @@@@@ sgThreadPool  66666 " << endl;
								childobj->setAttrSimp("zky_has_looped", "true");
								sgThreadPool->procSync(runners);
								code = mGicCode;
								//OmnThreadShellMgr::getSelf()->procSync(runners);
							}
							else
							{
								AosGic::createGic(htmlPtr, nChild, childobj, parentid, code, -1, -1);	//Ketty 2011/09/22
							}
						}
						else
						{
							AosGic::createGic(htmlPtr, nChild, childobj, parentid, code, -1, -1);	//Ketty 2011/09/22
						}
						//AosGic::createGic(htmlPtr, nChild, childobj, parentid, code, -1, -1);
						code.mJs << "aosvpd[" << avpdnum << "] = gXmlParser.String2Xml(\'" << nChild->toString() << "\');";
						code.mJs << "aosobj[" << avpdnum << "] = gXmlParser.String2Xml(\'" << childobj->toString() << "\');";
						childnum++;
					}
				}
				else
				{
					rdata->setArg1("createobj", "");
					AosGic::createGic(htmlPtr, nChild, obj, parentid, code, -1, -1);	//Ketty 2011/09/22
					rdata->setArg1("createobj", "objgic");
				}
			}
			/*
			else if(gic_type == "gic_container" && objpath == "noneobj__xmleditor")
			{
				objs.push_back(obj);
				childnum++;
				rdata->setArg1("createobj", "");
				AosGic::createGic(htmlPtr, nChild, obj, parentid, code, -1, -1);
				rdata->setArg1("createobj", "objgic");
			}
			*/
			else
			{
				if (gic_type == "gic_container" && addgic != "")//isAddGic)//addgic != "")
				{
					if (addgic == "noneobj__xmleditor")
					{
						AosXmlTagPtr childobj = obj;
						objs.push_back(childobj);
					}
					else
					{
						AosXmlTagPtr childobj = obj->getChild(childnum);
						aos_assert_r(childobj, false);
						objs.push_back(childobj);
						childnum++;
					}
				}
				rdata->setArg1("createobj", "");
				AosGic::createGic(htmlPtr, nChild, obj, parentid, code, -1, -1);	//Ketty 2011/09/22
				rdata->setArg1("createobj", "objgic");
			}
			nChild = thevpd->getNextChild();
			if (nChild){
				code.mJson<<",";
			}
		}

		for(u32 i=0; i < objs.size(); i++)
		{
			if (objs[i])
			{
				AosXmlTagPtr childobj = objs[i]->getFirstChild();
				OmnString objx, objy, objw, objh;
				while(childobj)
				{
					bool hascreated = childobj->getAttrBool("zky_hascreate__xmleditor", false);
					if (hascreated)
					{
						childobj = objs[i]->getNextChild();
						continue;
					}
					OmnString vpdname = childobj->getAttrStrSimp("zky_ctobjid__xmleditor", "");
					if (vpdname == "")
					{
						childobj = obj->getNextChild();
						continue;
					}
					AosXmlTagPtr childvpd, root;
					bool rslt = htmlPtr->getVpd(vpdname, childvpd, root);  
					
					if (!rslt)
					{
						code.mJson << "]";
						return false;
					}
					if (childvpd->getAttrStrSimp("gic_type", "") != "gic_container")
					{
						code.mJson << "]";
						return false;
					}

					code.mJson << ",";

					htmlPtr->setVpdNum();
					int newnum = htmlPtr->getVpdNum();

					rslt = setPos("zky_gicpos__xmleditor", childobj, childvpd);
					aos_assert_r(rslt, false);
					AosHtmlCode gicCode;
					rdata->setArg1("createobj", "objgic");
					//thread shell
//					OmnString path = childobj->getAttrStr("zky_objpath__xmleditor", "");
//					aos_assert_r(path != "", false);
//					AosStrSplit split(path, "/");          
//					vector<OmnString> strs= split.entriesV();
//					if (strs.size() > 4)
					if (isDeepLevel(childobj))
					{
						vector<OmnThrdShellProcPtr> runners;
						OmnThrdShellProcPtr runner = OmnNew	createSubContainer(this, htmlPtr, childvpd, childobj, parentid, gicCode);
						runners.push_back(runner);
OmnScreen << " @@@@@ sgThreadPool  77777 " << endl;
						childobj->setAttrSimp("zky_has_looped", "true");
						sgThreadPool->procSync(runners);
						//OmnThreadShellMgr::getSelf()->procSync(runners);
						gicCode = mGicCode;
					}
					else
					{
						AosGic::createGic(htmlPtr, childvpd, childobj, parentid, gicCode, -1, -1);	//Ketty 2011/09/22
					}
					//AosGic::createGic(htmlPtr, childvpd, childobj, parentid, gicCode, -1, -1);

					code.mJs << "aosvpd[" << newnum << "] = gXmlParser.String2Xml(\'" << childvpd->toString() << "\');";
					code.mJs << "aosobj[" << newnum << "] = gXmlParser.String2Xml(\'" << childobj->toString() << "\');";
					code.append(gicCode);
					childobj = objs[i]->getNextChild();
				}
			}
		}

		objs.clear();
	}

	code.mJson << "]";
	code.mJson <<",aosobj: aosobj[" << vpdnum << "]"
			   <<",aosvpd: aosvpd[" << vpdnum << "]";
	return true;
}

bool
AosGicContainer::isDeepLevel(const AosXmlTagPtr &obj)
{
	aos_assert_r(obj, false);
	int level = 0;
	AosXmlTag * pnode = obj->getParentTag();
	while(pnode && level < 5)
	{
		if (pnode->getTagname() == "embedobj") break;
		bool hasLooped = pnode->getAttrBool("zky_has_looped", false);
		if (hasLooped)
		{
			break;
		}
		level++;
		pnode = pnode->getParentTag();
	}
	if (level >= 5) return true;
	return false;
}

bool
AosGicContainer::createSubContainer::run()
{
	AosGic::createGic(mHtmlPtr, mVpd, mObj, mParentId, mCode, -1, -1);
	mCaller->setGicCode(mCode);
	return true;
}



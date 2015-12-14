////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 05/04/2011 by Ken Li
////////////////////////////////////////////////////////////////////////////
#include "SearchEngineAdmin/SystemDb.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
#include "SearchEngineAdmin/SengAdmin.h"
#include "SEUtil/Siteid.h"
#include "Util/StrParser.h"
#include "Porting/Sleep.h"
#include "Util/File.h"
#include "XmlUtil/SeXmlParser.h"


AosSystemDb::AosSystemDb()
:
mLock(OmnNew OmnMutex()),
mRemotePort(-1),
mTransId(AosSengAdmin::eInitTransId)
{
}


AosSystemDb::~AosSystemDb()
{
}


bool      	
AosSystemDb::start()
{
	return true;
}


bool        
AosSystemDb::stop()
{
	return true;
}


OmnRslt     
AosSystemDb::config(const OmnXmlParserPtr &def)
{
	return true;
}


bool
AosSystemDb::copySystemDb(
		const u32 siteid)
{
	return true;
}


bool
AosSystemDb::createSystemDb(
		const u64 &startDocid,
		const u64 &endDocid,
		const OmnString &ctnrStr,
		const u32 siteid)
{
	// All system data are docs whose parent containers are child of
	// the system container sgSystemContainer. This function creates
	// all the docs under 'sgSystemContainer'. 
	// 
	// It reads in all the docs. For each doc, it checks whether it is
	// in one of the system containers. If yes, create the doc. 
	aos_assert_r(siteid != 0, false);
	AosRundataPtr rdata = OmnApp::getRundata();
	rdata->setSiteid(siteid);

	sgSystemContainer = "zky_systemdb";
	sgImgPath = "/usr/local/openlaszlo/lps-4.7.2/Server/lps-4.7.2/images";
	sgSystemImageDir = "sysImage";
	sgGroupSize = 1000;
	sgSleepLength = 10;
	
	OmnConnBuffPtr docbuff;
	AosXmlParser parser;
	int group = 0;
	u64 docid = 0;
	
	u64 userid;
	OmnString ssid;
	AosXmlTagPtr userdoc;
	u64 urldocid;
	AosSengAdmin::getSelf()->login("root", "1804289383",
			"zky_sysuser_100", ssid, urldocid, userid, userdoc, siteid);

	ctnrVect.clear();
	ctnrVect.push_back("zky_giccreator");
	ctnrVect.push_back("zky_gicdesc");
	ctnrVect.push_back("zky_act_editor");
	ctnrVect.push_back("zky_cuteEditor");
	ctnrVect.push_back("zky_query");
	ctnrVect.push_back("zky_actdef");
	ctnrVect.push_back("zky_actparm");
	ctnrVect.push_back("zky_tpl");
	ctnrVect.push_back("sysdata");

	OmnStrParser1 strParser(ctnrStr, ",");
	OmnString ctnrWord;
	while((ctnrWord = strParser.nextWord()) != "")
	{
		ctnrVect.push_back(ctnrWord);
	}

	for (docid = startDocid; docid <= endDocid; docid++)
	{
		if (group >= sgGroupSize)
		{
			if (docid > 5000)
			{
				OmnSleep(sgSleepLength);
			}
			group = 0;
		}

		AosSengAdmin::DocStatus status;
		if (!AosSengAdmin::getSelf()->readDoc(docid, docbuff, false, status, rdata)) continue;
		
		if(status == AosSengAdmin::eBad || status == AosSengAdmin::eDeleted)
		{
			continue;
		}

		AosXmlTagPtr doc = parser.parse(docbuff, "" AosMemoryCheckerArgs);
		if (!doc)
		{
			OmnAlarm << "Failed to parse the doc: " 
				<< docid << ":" << docbuff->getData() << enderr;
			continue;
		}
		
		if (doc->getAttrStr(AOSTAG_OBJID) == "")
		{
			OmnAlarm << "Doc missing objid: " << doc->toString() << enderr;
			continue;
		}

		if (doc->getAttrU64(AOSTAG_DOCID, AOS_INVDID) == AOS_INVDID)
		{
			OmnAlarm << "Doc missing docid: " << doc->toString() << enderr;
			continue;
		}
		OmnString parent_ctnr = doc->getAttrStr(AOSTAG_PARENTC);
		if (!isSystemContainer(parent_ctnr))
		{
			continue;
		}

	    OmnString objid = doc->getAttrStr(AOSTAG_OBJID);
		//OmnScreen << "Read doc: " << objid << endl;
		
		// Process the images
		aos_assert_r(processImages(doc), false);

		// It is a system doc. Create it.
		doc->removeAttr(AOSTAG_DOCID);
		doc->removeAttr(AOSTAG_VERSION, 3, false);
		doc->removeAttr(AOSTAG_MTIME, 3, false);
		doc->removeAttr(AOSTAG_MT_EPOCH, 3, false);
		doc->removeAttr(AOSTAG_COUNTERCM);
		doc->removeAttr(AOSTAG_COUNTERDW);
		doc->removeAttr(AOSTAG_COUNTERLK);
		doc->removeAttr(AOSTAG_COUNTERRC);
		doc->removeAttr(AOSTAG_COUNTERWR);
		doc->removeAttr(AOSTAG_COUNTERUP);
		doc->removeAttr(AOSTAG_COUNTERRD);
		doc->removeAttr(AOSTAG_FULLDOC_HASH);
		
		doc->setAttr(AOSTAG_CTNR_PUBLIC, "true");
		doc->setAttr(AOSTAG_PUBLIC_DOC, "true");
		
		aos_assert_r(AosSengAdmin::getSelf()->sendCreateObjReq2(siteid, 
					(char *)doc->getData(), ssid,urldocid), false);

		group++;
	}
	int i = 0;
	OmnString user, uname;
	while(i<10)
	{
		uname = "root";
		uname << i;

		user = "";
		user << "<embedobj zky_heditor=\"9\" zky_category=\"other\" zky_siteid=\"100\"  zky_unit=\"zykie\" zky_sex=\"man\" zky_usrtp=\"zky_root\" zky_uname=\""<<uname<<"\" login_pass_pw=\"12345\"  zky_pfolder=\"yunyuyan_folder\" zky_usrctnr=\"yunyuyan_account\"  zky_realnm=\""<<uname<<"\" zky_objimg=\"a2/et14430.jpg\"  zky_objnm=\""<<uname<<"\"  _zt1t=\"5\"  zky_otype=\"zky_uact\" zky_place=\"anhui\" zky_usrst=\"active\" zky_pctrs=\"yunyuyan_account,ssx_account\"  zky_objimg1=\"img/ds800.png\" zkytkt_access=\"zkytkt_mgr\" zky_email=\"453757465@qq.com\" zky_email1=\"453757465@qq.com\"><zky_passwd>12345</zky_passwd></embedobj>";

		AosXmlTagPtr udoc = parser.parse(user,"" AosMemoryCheckerArgs);
		if(udoc)
		{
			AosXmlTagPtr logindoc = AosSengAdmin::getSelf()->createUserAcct(udoc,ssid,urldocid,siteid);
		}
		++i;
	}
	OmnFile::writeToFile("cpImageScript", 0, fileStr.length(), (char *)fileStr.data());
	return true;
}


bool
AosSystemDb::processImages(AosXmlTagPtr &doc)
{
	// It searches the doc 'doc' for attributes whose name 
	// is "img_src" and its value is in the form:
	// 		dirname/ABxxx.CCC
	// where 'dirname' is a string, A is [a, b, c, d, e], B is [t, y, s, m, l, h, o, i], 
	// a:	y, t, i, o
	// b:	y, t, i, s, o
	// c:	y, t, i, s, m, o
	// d:	y, t, i, s, m, l, o
	// e:	y, t, i, s, m, l, h, o
	// and CCC is one of [jpg, png, gif]
	doc->resetAttrNameValueLoop();
	
	bool finished = false, ff;
	const int eMaxRsltLen = 5;
	int idx = -1, num;
	char ch[8] = {'h','l','m','s','i','t','y','o'};
	OmnString rslts[eMaxRsltLen];
	OmnString name, value, value2, fname, imgType, new_path;
	AosXmlTagPtr tag;
	AosStrSplit split;
	while (!finished)
	{
		// aos_assert_r(doc->nextAttr(tag, "", name, value, finished, false, true), false);
		aos_assert_r(doc->nextAttr("", name, value, finished, false, true), false);
		if (finished) return true;

		if (name=="img_src" || name=="gic_nmmg" || name=="gic_facesrc" || name=="gic_mfacesrc" || name=="gic_bgsrc" || name == "gic_modelsrc" || name=="gic_bmodelsrc" || name=="gic_rmsrc" || name=="gic_ovmg" || name=="gic_dwmg" || name=="zky_objimg")
		{
			value.toLower();
			num = split.splitStr(value.data(), "/", rslts, eMaxRsltLen, ff);
			if(num <= 1)
			{
				continue;
			}
			if(value.indexOf("http://", 0) != -1)
			{
				continue;
			}
			
			idx = value.indexOf(0, '.', true);
			if(idx < 0)
			{
				continue;
			}
			imgType = value.substr(idx+1, 0);
			if (imgType == "jpg" || imgType == "png" || imgType == "gif")
			{
				// Need to move the image to the system container
				idx = value.indexOf(0, '/', true);
				value2 = value;
				fname = rslts[num-1];
				
				int i = 0;
				switch(fname.getBuffer()[0])
				{
					case 'a' : i = 4;break;
					case 'b' : i = 3;break;
					case 'c' : i = 2;break;
					case 'd' : i = 1;break;
					case 'e' : i = 0;break;
				}

				while(i < 8)
				{
					value2.getBuffer()[idx+2] = ch[i];
					fname.getBuffer()[1] = ch[i];

					fileStr << "cp " << sgImgPath << '/' << value2
							<< " " << sgImgPath << '/' << sgSystemImageDir << '/' << fname << "\n";

					if ((value.data()[idx+2] - ch[i]) == 0)
					{
						new_path = sgSystemImageDir;
						new_path << '/' << fname;
						tag->setAttr(name, new_path);
					}
					i++;
				}
			}
		}
	}
	return true;	
}


bool
AosSystemDb::isSystemContainer(const OmnString &ctnr)
{
	OmnStrParser1 strParser(ctnr, ",");
	OmnString ctnrWord;
	while((ctnrWord = strParser.nextWord()) != "")
	{
		for(int i=0; i < (int)ctnrVect.size(); i++)
		{
			if(ctnrWord == ctnrVect[i])
			{
				return true;	
			}
		}
	}
	return false;
}


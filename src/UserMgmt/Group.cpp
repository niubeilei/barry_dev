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
// This file is generated automatically by the ProgramAid facility. 
//
// Modification History:
// 08/18/2010: Created by cding
////////////////////////////////////////////////////////////////////////////
#include "UserMgmt/Group.h"


#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Database/DbRecord.h"
#include "DataStore/StoreMgr.h"
#include "DataStore/DataStore.h"
#include "SEInterfaces/DocClientObj.h"
#include "Rundata/Rundata.h"
#include "Tracer/Tracer.h"
#include "SEModules/ObjMgr.h"
#include "SEUtil/SeTypes.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlDoc.h"



AosGroup::AosGroup()
{
}

AosGroup::~AosGroup()
{
}


bool
AosGroup::deleteGroups(
		const AosRundataPtr &rdata)
{
	AosXmlTagPtr root = rdata->getReceivedDoc();
	u64 docid = root->getAttrU64(AOSTAG_DOCID, AOS_INVDID);

	if (docid == AOS_INVDID)
	{
		OmnAlarm << "Missing creator in the Group object!" << enderr;
		rdata->setError();
		return 0;
	}
	//get new member;
	OmnString memberOld = root->getAttrStr(AOSTAG_GROUP_MEMBER, 0);

	if (memberOld == "")
	{
		OmnAlarm << "Missing creator in the Group object!" << enderr;
		rdata->setError();
		return 0;
	}

	aos_assert_r(delGroup(rdata, memberOld),false);

	rdata->setOk();
	return true;
}


bool
AosGroup::createGroups(
		const AosRundataPtr &rdata)
{
	AosXmlTagPtr root = rdata->getReceivedDoc();
	//OmnString siteid = root->getAttrStr (AOSTAG_SITEID, AOS_INVDID);
	//get new member;
	OmnString memberNew = root->getAttrStr(AOSTAG_GROUP_MEMBER, 0);

	if (memberNew == "")
	{
		OmnAlarm << "Missing creator in the Group object!" << enderr;
		rdata->setError();
		return 0;
	}

	aos_assert_r(addGroup(rdata, memberNew),false);

	rdata->setOk();
	return true;
}


bool
AosGroup::modifyGroups(
		const AosRundataPtr &rdata)
{
	AosXmlTagPtr root = rdata->getReceivedDoc();
	// It retrieves the old version of the object and compares
	// who are added/deleted.
	u64 docid = root->getAttrU64(AOSTAG_DOCID, AOS_INVDID);
	OmnString objid = root->getAttrStr(AOSTAG_OBJID);

	if (docid == AOS_INVDID)
	{
		OmnAlarm << "Missing creator in the Group object!" << enderr;
		rdata->setError();
		return 0;
	}

	AosXmlTagPtr xmlold = AosDocClientObj::getDocClient()->getDoc(docid, objid, rdata);

	//get new member;
	OmnString memberNew = root->getAttrStr(AOSTAG_GROUP_MEMBER, 0);

	if (memberNew == "")
	{
		OmnAlarm << "Missing creator in the Group object!" << enderr;
		rdata->setError();
		return 0;
	}

	//get old member;
	OmnString memberOld = xmlold->getAttrStr(AOSTAG_GROUP_MEMBER,0);
		
	if (memberOld == "")
	{
		OmnAlarm << "Missing creator in the Group object!" << enderr;
		rdata->setError();
		return 0;

	}
		
	//Comparing strings
	if(memberNew.compare1(memberOld)== 0) return true;

	//Remove the same element;
	OmnString mNew = removeSame(memberNew, memberOld);
	aos_assert_r(addGroup(rdata, mNew),false);

	OmnString mOld = removeSame(memberOld, memberNew);
	aos_assert_r(delGroup(rdata, mOld),false);

	rdata->setOk();
	return true;
}


bool 
AosGroup::addGroup(
		const AosRundataPtr &rdata,
		const OmnString &meNew)
{
	AosXmlTagPtr root = rdata->getReceivedDoc();
	u32 siteid = rdata->getSiteid();

	OmnStrParser1 parser1(meNew, ",");
	OmnString word;

	//group name
	OmnString gname = root -> getAttrStr(AOSTAG_GROUP_NAME);
	if (gname == "")
	{
		OmnAlarm << "Missing creator in the Group object!" << enderr;
		rdata->setError();
		return 0;
	}
	AosXmlTagPtr doc;
	//add
	while ((word = parser1.nextWord()) != "")
	{
		doc = AosDocClientObj::getDocClient()->getDocByObjid(word, rdata);

		OmnString dgroup = doc->getAttrStr(AOSTAG_GROUPS);

		if (dgroup.compare1("null")==0) dgroup ="";

		OmnStrParser1 newParser(dgroup,",");
		OmnString word1;
		bool found = true;
		while ((word1 = newParser.nextWord())!="")
		{
			if (gname.compare1(word1)== 0) 
			found = false;
		}
		if (found)
		{
			if (dgroup == "") dgroup << gname;
			else dgroup << "," <<gname;
		
			u64 userid = rdata->setUserid(AosObjMgr::getSuperUserDocid(siteid, rdata));
			OmnString appname = rdata->setAppname(AOSAPPNAME_SYSTEM);
			AosDocClientObj::getDocClient()->modifyAttrStr1(rdata, 
				doc->getAttrU64(AOSTAG_DOCID, 0), 
				doc->getAttrStr(AOSTAG_OBJID), 
				AOSTAG_GROUPS, dgroup, 
				"", false, true, true);
			rdata->setUserid(userid);
			rdata->setAppname(appname);
		}

	}
	rdata->setOk();
	return true;
}


bool 
AosGroup::delGroup(
		const AosRundataPtr &rdata,
		const OmnString &meOld )
{
	u32 siteid = rdata->getSiteid();
	AosXmlTagPtr root = rdata->getReceivedDoc();

	//group name
	OmnString gname = root -> getAttrStr(AOSTAG_GROUP_NAME);
	if (gname == "")
	{
		OmnAlarm << "Missing creator in the Group object!" << enderr;
		rdata->setError();
		return 0;
	}
	//del
	OmnStrParser1 parser1(meOld,",");
	OmnString oldword;

	AosXmlTagPtr doc1;
	while ((oldword = parser1.nextWord()) != "")
	{
		doc1 = AosDocClientObj::getDocClient()->getDocByObjid(oldword, rdata);
		if (!doc1) continue;

		OmnString tmp("");
		OmnString dgroup = doc1->getAttrStr(AOSTAG_GROUPS);

		OmnStrParser1 oldParser(dgroup,",");
		OmnString word1;

		while ((word1 = oldParser.nextWord())!="")
		{
			if(gname.compare1(word1)!= 0 )
			{
				if (tmp == "") tmp <<word1;
				else tmp<<","<<word1;
			}
		}
		if (tmp =="") tmp = "null";

		u64 userid = rdata->setUserid(AosObjMgr::getSuperUserDocid(siteid, rdata));
		OmnString appname = rdata->setAppname(AOSAPPNAME_SYSTEM);
		AosDocClientObj::getDocClient()->modifyAttrStr1(rdata,
				doc1->getAttrU64(AOSTAG_DOCID, 0), 
				doc1->getAttrStr(AOSTAG_OBJID), 
				AOSTAG_GROUPS, 
				tmp, "null", false, true, true);
		rdata->setAppname(appname);
		rdata->setUserid(userid);
	}
	rdata->setOk();
	return true;
}
		
OmnString 
AosGroup::removeSame(
		const OmnString &str1,
		const OmnString &str2
		)
{
	//'meNew', 'meOld': "groupname,groupname,...,groupname"
	//比较str1, str2.返回str1中是str1字符串中与str2相同�?	
	OmnString reStr("");
	bool found;
	OmnStrParser1 parser(str1, ",");
	OmnString word;
	while ((word = parser.nextWord()) != "")
	{
		OmnStrParser1 old(str2, ",");
		OmnString wordold;
		found = true;
		while ((wordold = old.nextWord())!="")
		{
			if(word.compare1(wordold)== 0) 
			{
				found = false;
			    continue;	
			}
		}
		if (found)
		{
			if (reStr=="") reStr <<word;
			else reStr<<","<<word;
		}

	}
	return reStr;
}

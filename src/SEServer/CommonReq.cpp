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
// 01/04/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEServer/CommonReq.h"

#include "Alarm/Alarm.h"
#include "SEServer/SeReqProc.h"
#include "Util/StrSplit.h"
#include "XmlUtil/XmlTag.h"


bool
AosCommonReq::getFileNames(
		AosSeReqProc *proc,
		const AosWebRequestPtr &req, 
		const AosXmlTagPtr &childelem) 
{
	OmnNotImplementedYet;
	/*
	static const int lsDefaultPageSize =50;
	static const OmnString lsDefaultSep = "|^|";

	// It retrieves file names. 'childelem' should be in the form:
	// 	<operation opr="getFileNames'
	// 		sep="xxx"
	// 		recursive="xxx"
	// 		dirnames="xxx"
	// 		psize="xxx"
	// 		startidx="xxx" (optional)/>
	errcode = eAosXmlInt_General;
	OmnString pp = childelem->getAttrStr("psize");
	if (pp == "")
	{
		errmsg = "Missing page size!";
		OmnAlarm << errmsg << enderr;
		return false;
	}
	int pagesize = atoi(pp);
	if (pagesize <= 0) pagesize = lsDefaultPageSize;

	pp = childelem->getAttrStr("startidx");
	int index = (pp != "")?atoi(pp.data()):0;
	if (index < 0) index = 0;

	pp = childelem->getAttrStr("recursive");
	bool recur = (pp != "")?(pp.data()[0] != '0'):true;

	OmnString sep = childelem->getAttrStr("sep");
	if (sep == "") sep = lsDefaultSep;

	OmnString dirs = childelem->getAttrStr("dirnames");

	OmnFile ff;
	std::list<OmnString> thelist;
	int total = ff.getFilesMulti(dirs, thelist, sep, recur);

	// Calculate the number of entries for this request
	int num = thelist.size() - index;
	if (num < 0) num = 0;
	if (num > pagesize) num = pagesize;

	contents = "<Contents total=\"";
	contents << total << "\" num=\"" << num << "\">";

	// Skip the first index-1 entries
	std::list<OmnString>::iterator it;
	if (num > 0)
	{
		int crt_idx = 0;
		int num_procd = 0;
		for (it=thelist.begin(); it != thelist.end(); it++)
		{
			if (crt_idx < index) continue;
			if (num_procd != 0) contents << sep;
			contents << *it;
			num_procd++;
			if (num_procd >= num) break;
		}
	}
	contents << "</Contents>";
	errcode = eAosXmlInt_Ok;
	*/
	return true;
}


bool
AosCommonReq::createComment(
		const AosXmlTagPtr &cmd,
		OmnString &contents,
		OmnString &errmsg,
		AosXmlRc &errcode) 
{
	/*
	// This function creates a comment. The comment is defined
	// as an XML object that should have already been extracted
	// into 'mXmlObj'. 
	// <cmd commented="xxx"
	// 		icommented="xxx"
	// 		cmt_thread="xxx"/>
	errcode = eAosXmlInt_General;

	mTablename = "objtable";

	// 1. Get a new objid
	if (!sgIdGen) sgIdGen = OmnNew AosIdGen("objtableid");
	OmnString id = sgIdGen->getNextId();
	mName = "objtable_b";
	mName << sgIdGen->getNextId();	
	mXmlObj->SetAttribute("objid", mName);

	mTags = mXmlObj->Attribute("__tags");
	OmnString tags1 = mXmlObj->Attribute("__tags1");
	if (tags1 != "")
	{
		if (mTags != "") mTags << ",";
		mTags << tags1;
	}
	checkTags(mTags); 
	mTags.escapeSymbal('\'', '\'');

	mDesc = mXmlObj->Attribute("desc");
	mDesc.escapeSymbal('\'', '\'');
	mShortname = mXmlObj->Attribute("__name");
	mShortname.escapeSymbal('\'', '\'');
	mSubtype = mXmlObj->Attribute("__stype");
	mSubtype.escapeSymbal('\'', '\'');
	mThumbnail = mXmlObj->Attribute("__tnail");
	mThumbnail.escapeSymbal('\'', '\'');

	// Retrieve the parent container, which can be defined
	// either in 'cmd' (as 'container') or in mXmlObj 
	// (as '__pctnrs')
	mContainer = cmd->Attribute("container");
	if (mContainer == "") 
	{
		mContainer = mXmlObj->Attribute("__pctnrs");
		mContainer.escapeSymbal('\'', '\'');
	}
	else
	{
		mContainer.escapeSymbal('\'', '\'');
		mXmlObj->SetAttribute("__pctnrs", mContainer.data());
	}
	// If the container is not empty, need to add a tag:
	// 		ztg_pctnr_<container dataid>
	// This can be used to query all the data that is in 
	// the container. 
	mContainer.escapeSymbal('\'', '\'');
	if (mContainer == "") mContainer = AOSDATAID_COMMENTS;
	addDataidTag(AOSZTG_PCTNR, mContainer, errmsg, "Invalid Containe ID");

	// Retrieve the commented object
	OmnString commented = cmd->Attribute("zky_cmtobj");
	if (commented == "") commented = mXmlObj->Attribute(AOSTAG_COMMENTED);
	aos_assert_r(checkNotEmpty(commented, errmsg, 
		"Missing the commented object DataId"), false);
	addDataidTag(AOSZTG_COMMENTED, commented, errmsg, "Invalid Commented Dataid");
	// aos_assert_r(updateCounter(AOSTAG_CNTCM, commented, 1, errmsg), false);
	aos_assert_r(updateCounter("zky_ctnrs", commented, 1, errmsg), false);

	OmnString icommented = cmd->Attribute("zky_icmobj");
	if (icommented == "") icommented = mXmlObj->Attribute(AOSTAG_ICOMMENTED);
	addDataidTag(AOSZTG ICOMMENTED, icommented, errmsg, "Invalid iCommented Dataid");
	if (icommented != "")
	{
		// aos_assert_r(updateCounter(AOSTAG_CNTCM, icommented, 1, errmsg), false);
		aos_assert_r(updateCounter("zky_ctnrs", icommented, 1, errmsg), false);
	}

	OmnString cmtThread = cmd->Attribute("zky_cmthrd");
	if (cmtThread == "") cmtThread = mXmlObj->Attribute(AOSTAG_CMTTHREAD);
	addDataidTag(AOSZTG_CMTTHREAD, cmtThread, errmsg, "Invalid ThreadID");

	mVVPD = cmd->Attribute("vvpd");
	if (mVVPD == "") mVVPD = mXmlObj->Attribute("vvpd");
	mVVPD.escapeSymbal('\'', '\'');
	mEVPD = cmd->Attribute("evpd");
	if (mEVPD == "") mEVPD = mXmlObj->Attribute("evpd");
	mEVPD.escapeSymbal('\'', '\'');
	mObjType = AOSOTYPE_COMMENT;
	mOrderName = mXmlObj->Attribute("__ofname");

	// Save the object
	aos_assert_r(saveTable("objtable", errmsg), false);

	OmnString ordervalue = mXmlObj->Attribute(mOrderName);
	AosTag tagobj(mObjType, mName, mTags,
		mShortname, mThumbnail, ordervalue, mDesc, mUser, mXml);
	tagobj.addEntries();

	errcode = eAosXmlInt_Ok;
	contents = "<results objid=\"";
	contents << mName << "\"/>";
	return true;
	*/
	OmnNotImplementedYet;
	return false;
}


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
// 07/28/2011	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#include "DocTrans/IndexMgrModifyDocTrans.h"

#include "SEInterfaces/DocSvrObj.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"

AosIndexMgrModifyDocTrans::AosIndexMgrModifyDocTrans(const bool regflag)
:
AosDocTrans(AosTransType::eIndexMgrModifyDoc, regflag)
{
}


AosIndexMgrModifyDocTrans::AosIndexMgrModifyDocTrans(
		const u64 docid,
		const int level, 
		const int period,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosDocTrans(AosTransType::eIndexMgrModifyDoc, docid, need_save, need_resp, snap_id),
mDocid(docid),
mLevel(level),
mPeriod(period)
{
}


AosIndexMgrModifyDocTrans::~AosIndexMgrModifyDocTrans()
{
}


bool
AosIndexMgrModifyDocTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mDocid = buff->getU64(0);
	
	setDistId(mDocid);
	return true;
}


bool
AosIndexMgrModifyDocTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setU64(mDocid);
	return true;
}


AosTransPtr
AosIndexMgrModifyDocTrans::clone()
{
	return OmnNew AosIndexMgrModifyDocTrans(false);
}


bool
AosIndexMgrModifyDocTrans::proc()
{
	// This function is the server side implementations of AosDocSvr::modifyObj(...)

	// Ketty 2012/11/30
	setFinishLater();
	
	AosXmlTagPtr doc = AosDocSvrObj::getDocSvr()->getDoc(mDocid, mSnapshotId, mRdata);	
	if (!doc)
	{
		AosSetError(mRdata, "indexmgr_failed_retrieve_doc") 
			<< "Docid: " << mDocid 
			<< "Level: " << mLevel
			<< "Period: " << mPeriod << enderr;
		return false;
	}
	
	AosXmlTagPtr timeplan_xml = doc->getFirstChild(AOSTAG_TIME_PLAN);
	aos_assert_r(timeplan_xml, false);
	AosXmlTagPtr tag = timeplan_xml->getFirstChild();
	while (tag)
	{
		if (tag->getAttrInt("level", -1) != mLevel)
		{
			tag = timeplan_xml->getNextChild();
			continue;
		}

		// Note that it adds the periods that are defined in 'level' 
		// but not in tag. This means that it does not override 
		// 'tag'. This is extremely important because the same doc
		// may have been modified by someone else.
		OmnString ss = tag->getAttrStr(AOSTAG_PERIODS);
		OmnString orig_periods = ss;
		if (ss == "")
		{
			ss << mPeriod;
		}
		else
		{
			vector<OmnString> periods;
			AosSplitStr(ss, ",", periods, eMaxPeriods);
			ss = "";
			bool added = false;
			for (u32 i=0; i<periods.size(); i++)
			{
				if (i>0) ss << ",";
				int p = atoi(periods[i].data());
				if (p < mPeriod)
				{
					ss << p;
				}
				else if (p == mPeriod)
				{
					// The period is already there. Do nothing.
					return true;
				}
				else
				{
					added = true;
					ss << mPeriod;
					for (u32 m=i; m<periods.size(); m++)
					{
						ss << "," << atoi(periods[m].data());
					}
					break;
				}
			}

			if (!added) ss << "," << mPeriod;
		}

		tag->setAttr(AOSTAG_PERIODS, ss);

		bool rslt = AosDocSvrObj::getDocSvr()->modifyObj(mRdata, doc, mDocid, getTransId(), mSnapshotId);
		if (!rslt)
		{
			AosSetError(mRdata, "indexmgr_failed_saving_doc") << enderr;
			return false;
		}
		return true;
	}

	// The level was not in the time plan, which is an error.
	AosSetError(mRdata, "indexmgr_level_not_found")
		<< "Docid: " << mDocid 
		<< "Level: " << mLevel 
		<< "Period: " << mPeriod 
		<< doc << enderr;
	return false;
}



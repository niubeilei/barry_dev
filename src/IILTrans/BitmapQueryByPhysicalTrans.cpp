////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 10/05/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
/*
#include "IILTrans/BitmapQueryByPhysicalTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "SEInterfaces/BitmapObj.h"
#include "SEInterfaces/BitmapMgrObj.h"
#include "SEInterfaces/QueryRsltObj.h"
#include "SEInterfaces/IILMgrObj.h" 
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"


// Ketty 2013/03/29
AosIILTransBitmapQueryByPhysical::AosIILTransBitmapQueryByPhysical(const bool flag)
:
AosIILTrans(AosIILFuncType::eBitmapQueryByPhysical, flag AosMemoryCheckerArgs),
mPhysicalId(-1)
{
}


bool
AosIILTransBitmapQueryByPhysical::initTrans(
		const AosXmlTransPtr &trans,
		const AosXmlTagPtr &trans_doc,
		const AosRundataPtr &rdata)
{
	aos_assert_r(trans_doc, false);

	mTransId = trans->getTransId();	
	bool rslt = AosIILTrans::initTransBaseById(trans_doc, rdata);
	if(!rslt) return rslt;

	mPhysicalId = trans_doc->getAttrInt(AOSTAG_PHYSICALID, -1);
	aos_assert_r(mPhysicalId != -1, false);
	aos_assert_r(mPhysicalId != AosGetSelfServerId(), false);
	
	int iilnum = trans_doc->getAttrInt("iilnum", -1);
	aos_assert_r(iilnum > 1, false);

	AosXmlTagPtr iilnames_tag = trans_doc->getFirstChild("iilnames");
	aos_assert_r(iilnames_tag, false);

	OmnString iilname;
	AosXmlTagPtr iilname_tag = iilnames_tag->getFirstChild();
	while(iilname_tag)
	{
		iilname = iilname_tag->getNodeText();
		aos_assert_r(iilname != "", false);

		mIILNames.push_back(iilname);
		iilname_tag = iilnames_tag->getNextChild();
	}
	aos_assert_r(iilnum == (int)mIILNames.size(), false);
	
	AosXmlTagPtr bitmap_idlists_tag = trans_doc->getFirstChild("bitmap_idlists");
	aos_assert_r(bitmap_idlists_tag, false);

	AosQueryRsltObjPtr bitmap_idlist;
	AosXmlTagPtr bitmap_idlist_tag = bitmap_idlists_tag->getFirstChild();
	while(bitmap_idlist_tag)
	{
		bitmap_idlist = AosQueryRsltObj::getQueryRsltStatic();
		bitmap_idlist->serializeFromXml(bitmap_idlist_tag);
		mBitmapIdLists.push_back(bitmap_idlist);
		bitmap_idlist_tag = bitmap_idlists_tag->getNextChild();
	}
	aos_assert_r(iilnum == (int)mBitmapIdLists.size(), false);

	AosXmlTagPtr partial_bitmaps_tag = trans_doc->getFirstChild("partial_bitmaps");
	aos_assert_r(partial_bitmaps_tag, false);

	AosBitmapObjPtr partial_bitmap;
	AosXmlTagPtr partial_bitmap_tag = partial_bitmaps_tag->getFirstChild();
	while(partial_bitmap_tag)
	{
		partial_bitmap = AosBitmapObj::getBitmapStatic();
		AosBuffPtr buff = partial_bitmap_tag->getNodeTextBinaryUnCopy(AosMemoryCheckerArgsBegin);
		partial_bitmap->loadFromBuff(buff);
		mPartialBitmaps.push_back(partial_bitmap);
		partial_bitmap_tag = partial_bitmaps_tag->getNextChild();
	}
	aos_assert_r(iilnum == (int)mPartialBitmaps.size(), false);
		
	return true;
}


bool
AosIILTransBitmapQueryByPhysical::serializeFrom(const AosBuffPtr &buff)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosIILTransBitmapQueryByPhysical::serializeTo(const AosBuffPtr &buff)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosIILTransBitmapQueryByPhysical::proc(const AosIILObjPtr &iilobj, const AosRundataPtr &rdata)
{	
	aos_assert_rr(iilobj, rdata, false);

	AosBitmapObjPtr bitmap_rslt_phy;
	AosBuffPtr buff = OmnNew AosBuff(2048 AosMemoryCheckerArgs);
	bitmap_rslt_phy->saveToBuff(buff);

	// vector<OmnString>				mIILNames;
	// vector<AosQueryRsltObjPtr>		mBitmapIdLists;
	// vector<AosBitmapObjPtr>		mPartialBitmaps;
	//
	// For each IIL, it retrieves all the bitmaps defined in 
	// mBitmapIdList from all sections. The retrieved bitmaps 
	// are ORed with the partial bitmaps. 
	//
	// It then sections this bitmap. It uses this section list
	// to filter the next IIL. 
	//
	// It returns the ANDed bitmaps of all the IILs. 

	bool rslt = true;
	OmnString str;
	str << "<rsp rslt =\"" << rslt << "\" >";

	str << "<bitmap><![BDATA[";
	str << buff->dataLen() << ":";
	str.append(buff->data(), buff->dataLen());
	str << "]]></bitmap>";

	str << "</rsp>";

	AosBitmapMgrObj::returnBitmapStatic(bitmap_rslt_phy);

	rdata->setContents(str);
	return rslt;
}


u64 
AosIILTransBitmapQueryByPhysical::getIILID(const AosRundataPtr &rdata)
{
	return getIILIDById(rdata);
}


AosIILTransPtr 
AosIILTransBitmapQueryByPhysical::clone()
{
	return OmnNew AosIILTransBitmapQueryByPhysical(false);
}


int
AosIILTransBitmapQueryByPhysical::getSerializeSize() const 
{
	OmnNotImplementedYet;
	return 0;
}


AosIILType 
AosIILTransBitmapQueryByPhysical::getIILType() const
{
	return eAosIILType_BigStr;
}
*/


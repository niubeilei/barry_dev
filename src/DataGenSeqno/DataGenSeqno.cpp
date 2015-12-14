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
// This is a utility to select docs.
//
// Modification History:
// 11/01/2011	Created by Linda Lin 
////////////////////////////////////////////////////////////////////////////
#include "DataGenSeqno/DataGenSeqno.h"

#include "alarm_c/alarm.h"
#include "API/AosApi.h"
#include "DataGenSeqno/AllDataGenSeqno.h"
#include "Rundata/Rundata.h"
#include "SEModules/ObjMgr.h"
#include "SEInterfaces/DocClientObj.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"
#include "ValueSel/VsUtil.h"

extern AosDataGenSeqnoPtr	sgSeqno[AosDataGenSeqno::eMax+1];
static OmnMutex				sgLock;

AosDataGenSeqno::AosDataGenSeqno(
		const OmnString &name,
		const AosDataGenSeqnoType::E type, 
		const bool reg)
:
mType(type),
mLock(OmnNew OmnMutex())
{
	AosDataGenSeqnoPtr thisptr(this, false);
	if (reg) registerseqno(name, thisptr);
}


AosDataGenSeqno::~AosDataGenSeqno()
{
}


bool
AosDataGenSeqno::registerseqno(const OmnString &name, const AosDataGenSeqnoPtr &seqno)
{
	AosDataGenSeqnoType::E type = seqno->getType();
	if (!AosDataGenSeqnoType::isValid(type))
	{
		OmnAlarm << "Invalid Doc Selector id: " << type << enderr;
		return false;
	}

	mLock->lock();
	if (sgSeqno[type])
	{
		mLock->unlock();
		OmnAlarm << "Doc Selector already registered: " << name << ":" << type << enderr;
		return false;
	}

	sgSeqno[type] = seqno;
	bool rslt = AosDataGenSeqnoType::addName(name, seqno->mType);
	mLock->unlock();
	return rslt;
}

bool
AosDataGenSeqno::createSeqnoStatic(
			AosValueRslt &value, 
			const AosXmlTagPtr &sdoc,
			const AosRundataPtr &rdata)
{
	OmnString id = sdoc->getAttrStr(AOSTAG_SEQNO_TYPE, "");
	aos_assert_rr(id != "", rdata, false);
	AosDataGenSeqnoType::E type = AosDataGenSeqnoType::toEnum(id);
	if (!AosDataGenSeqnoType::isValid(type))
	{
		AosSetError(rdata, AosErrmsgId::eInternalError);
		OmnAlarm << rdata->getErrmsg() << "Unrecognized DataGen Type: " << id << enderr;
		return false;
	}

	sgLock.lock();
	AosDataGenSeqnoPtr uu = sgSeqno[type];
	sgLock.unlock();
	aos_assert_rr(uu, rdata, false);
	return uu->createSeqno(value, sdoc, rdata);
}

bool
AosDataGenSeqno::modifyDoc(const AosXmlTagPtr &doc, const AosRundataPtr &rdata)
{
	AosXmlTagPtr dd = doc->getRoot();
	u64 userid = rdata->setUserid(AosObjMgr::getSysRootDocid(rdata->getSiteid(), rdata));
	bool rslt =  AosModifyDoc(dd, rdata);
	rdata->setUserid(userid);
	return rslt;
}


OmnString
AosDataGenSeqno::getAttrStrValue(
		const AosXmlTagPtr &sdoc, 
		const OmnString &tagname, 
		const OmnString &dft,
		const AosRundataPtr &rdata)
{
	// <tagname zky_value_type="">
	// ...
	// </tagname>
	OmnString value = sdoc->getAttrStr(tagname, dft);
	if (value == dft)
	{
		AosXmlTagPtr value_def = sdoc->xpathGetChild(tagname);
		if (value_def)
		{
			AosValueRslt valueRslt; 
			if (!AosValueSel::getValueStatic(valueRslt, value_def, rdata))
			{
				return dft; 
			}
			value = valueRslt.getStr();
		}
	}
	return value;
}


u64
AosDataGenSeqno::getAttrU64Value(
		const AosXmlTagPtr &sdoc, 
		const OmnString &tagname, 
		const u64 &dft,
		const AosRundataPtr &rdata)
{
	//<tagname zky_value_type="">
	//...
	//</tagname>
	u64 value = sdoc->getAttrU64(tagname, dft);
	if (value == dft)
	{
		AosXmlTagPtr value_def = sdoc->xpathGetChild(tagname);
		if (value_def)
		{
			AosValueRslt valueRslt; 
			if (!AosValueSel::getValueStatic(valueRslt, value_def, rdata))
			{
				return dft;
			}
			if (!valueRslt.getU64()) return dft;
		}
	}
	return value;
}

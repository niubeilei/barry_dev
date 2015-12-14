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
// 2014/05/15 Created By Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "IILTrans/GetMapValuesTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "IILTrans/IILTransInclude.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"


AosIILTransGetMapValues::AosIILTransGetMapValues(const bool flag)
:
AosIILTrans(AosTransType::eGetMapValues, flag AosMemoryCheckerArgs)
{
}


AosIILTransGetMapValues::AosIILTransGetMapValues(
		const u64 &iilid,
		const set<OmnString> &keys,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosIILTrans(AosTransType::eGetMapValues, iilid,
	false, snap_id, need_save, need_resp AosMemoryCheckerArgs),
mKeys(keys)
{
}


AosIILTransGetMapValues::AosIILTransGetMapValues(
		const OmnString &iilname,
		const set<OmnString> &keys,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosIILTrans(AosTransType::eGetMapValues, iilname,
	false, snap_id, need_save, need_resp AosMemoryCheckerArgs),
mKeys(keys)
{
}


AosTransPtr 
AosIILTransGetMapValues::clone()
{
	return OmnNew AosIILTransGetMapValues(false);
}


bool
AosIILTransGetMapValues::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	u32 size = mKeys.size();
	buff->setU32(size);

	set<OmnString>::iterator itr_t = mKeys.begin();
	for (; itr_t !=  mKeys.end(); itr_t++) 
	{
		buff->setOmnStr(*itr_t);
	}

	return true;
}


bool
AosIILTransGetMapValues::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	u32 size = buff->getU32(0);

	mKeys.clear();
	for(u32 i=0; i<size; i++)
	{
		OmnString key = buff->getOmnStr("");
		mKeys.insert(key);
	}

	return true;
}


bool
AosIILTransGetMapValues::proc(
		const AosIILObjPtr &iilobj,
		AosBuffPtr &resp_buff,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilobj, false);
	aos_assert_r(iilobj->getIILType() == eAosIILType_BigStr, false);

	vector<u64> values;
	u32 size = mKeys.size();

	int64_t idx, iilidx;
	OmnString key;
	u64 value = 0;
	bool isunique = false;
	bool rslt;
	iilobj->lockIIL();
	set<OmnString>::iterator itr_t = mKeys.begin();
	for (; itr_t != mKeys.end(); itr_t++)
	{
		key = *itr_t;
		value = 0;
		isunique = false;
		if (key == "")
		{
			continue;
		}

		AosIILIdx the_idx;
		rslt = iilobj->nextDocidPrivFind(the_idx, key, eAosOpr_eq, value, isunique, false, rdata); 
		if (rslt && value != 0 && isunique)
		{
			values.push_back(value);
		}
		else if (value > 0 && !isunique)
		{
			iilobj->unlockIIL();
			OmnAlarm << "key is not unique:" << mIILName << ":" << key << " : " << value << enderr;
			return false;
		}
		else
		{		
			value = 1000 + iilobj->getNumDocs();
			rslt = iilobj->addDocPriv(key, value, true, true, rdata);
			if (!rslt)
			{
				iilobj->unlockIIL();
				OmnAlarm << "key is not unique:" << mIILName << ":" << key << " : " << value << enderr;
				return false;
			}

			values.push_back(value);
		}
	}
	iilobj->unlockIIL();

	resp_buff = OmnNew AosBuff(sizeof(u64) * size + 20 AosMemoryCheckerArgs);
	resp_buff->setU8(rslt);
	resp_buff->setU32(size);
	for (u32 i=0; i<size; i++)
	{
		resp_buff->setU64(values[i]);
	}

	return true;
}


AosIILType 
AosIILTransGetMapValues::getIILType() const
{
	return eAosIILType_BigStr;
}


int
AosIILTransGetMapValues::getSerializeSize() const
{
	OmnNotImplementedYet;
	return 0;
}


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
// 12/18/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SmartObj/SlotMgr/SobjSlotMgr.h"

#include "SEInterfaces/DocClientObj.h"
#include "Porting/Sleep.h"
#include "SEUtil/DocTags.h"
#include "SEInterfaces/SeLogClientObj.h"
#include "SmartObj/SmartObjNames.h"
#include "SmartObj/SlotMgr/SlotMgrSlab.h"
#include "Util/Value.h"


AosSobjSlotMgr::AosSobjSlotMgr(const bool flag)
:
AosSmartObj(AOS_SMARTOBJNAME_SLOTMGR, flag),
mMinStart(-1),
mMinStartFlag(false),
mMaxStart(-1),
mMaxStartFlag(false),
mMinSize(-1),
mMinSizeFlag(false),
mMaxSize(-1),
mMaxSizeFlag(false),
mLogErrors(false),
mIsDirty(false)
{
}


AosSobjSlotMgr::AosSobjSlotMgr(const OmnString &objid, const AosRundataPtr &rdata)
:
AosSmartObj(AOS_SMARTOBJNAME_SLOTMGR, false),
mMinStart(-1),
mMinStartFlag(false),
mMaxStart(-1),
mMaxStartFlag(false),
mMinSize(-1),
mMinSizeFlag(false),
mMaxSize(-1),
mMaxSizeFlag(false),
mLogErrors(false),
mObjid(objid),
mIsDirty(false)
{
	mDoc = AosDocClientObj::getDocClient()->getDocByObjid(objid, rdata);
	if (!mDoc || !init(mDoc, rdata))
	{
		OmnThrowException(rdata->getErrmsg());
	}
}


AosSobjSlotMgr::AosSobjSlotMgr(const AosXmlTagPtr &doc, const AosRundataPtr &rdata)
:
AosSmartObj(AOS_SMARTOBJNAME_SLOTMGR, false),
mMinStart(-1),
mMinStartFlag(false),
mMaxStart(-1),
mMaxStartFlag(false),
mMinSize(-1),
mMinSizeFlag(false),
mMaxSize(-1),
mMaxSizeFlag(false),
mLogErrors(false),
mIsDirty(false),
mDoc(doc)
{
	if (!mDoc)
	{
		OmnThrowException("Doc is null");
	}

	mObjid = doc->getAttrStr(AOSTAG_OBJID);
	bool rslt = init(doc, rdata);
	if (!rslt)
	{
		OmnThrowException(rdata->getErrmsg());
	}
}


AosSobjSlotMgr::~AosSobjSlotMgr()
{
}


bool
AosSobjSlotMgr::init(const AosXmlTagPtr &doc, const AosRundataPtr &rdata)
{
	aos_assert_rr(doc, rdata, false);
	mMinStartFlag = doc->getAttrBool(AOSTAG_MIN_START_FLAG, true);
	mMinStart = doc->getAttrInt64(AOSTAG_MIN_START, eDftMinStart);

	mMaxStartFlag = doc->getAttrBool(AOSTAG_MAX_START_FLAG, false);
	mMaxStart = doc->getAttrInt64(AOSTAG_MAX_START, eDftMaxStart);

	mMinSizeFlag = doc->getAttrBool(AOSTAG_MIN_SIZE_FLAG, true);
	mMinSize = doc->getAttrInt64(AOSTAG_MIN_SIZE, eDftMinSize);

	mMaxSizeFlag = doc->getAttrBool(AOSTAG_MAX_SIZE_FLAG, false);
	mMaxSize = doc->getAttrInt64(AOSTAG_MAX_SIZE, eDftMaxSize);

	mInvalidStart = doc->getAttrBool(AOSTAG_INVALID_START, -1);
	mInvalidSize = doc->getAttrBool(AOSTAG_INVALID_SIZE, 0);

	mLogErrors = doc->getAttrBool(AOSTAG_LOG_ERRORS, true);
	mLogOperations = doc->getAttrBool(AOSTAG_LOG_OPERATIONS, true);

	mOprCtnrObjid = doc->getAttrStr(AOSTAG_OPERATION_CTNR_OBJID, "");
	mOprLogname = doc->getAttrStr(AOSTAG_OPERATION_LOGNAME, "");
	mErrorCtnrObjid = doc->getAttrStr(AOSTAG_ERROR_CTNR_OBJID, "");
	mErrorLogname = doc->getAttrStr(AOSTAG_ERROR_LOGNAME, "");

	if (mOprCtnrObjid == "")
	{
		mOprCtnrObjid = doc->getAttrStr(AOSTAG_PARENTC);
	}

	if (mErrorCtnrObjid == "")
	{
		mErrorCtnrObjid = mOprCtnrObjid;
	}

	AosXmlTagPtr slabs = doc->getFirstChild(AOSTAG_SLABS);
	aos_assert_rr(slabs, rdata, false);
	AosXmlTagPtr slab_xml = slabs->getFirstChild();
	AosSobjSlotMgrPtr thisptr(this, false);
	mSlabs.clear();
	int idx = 0;
	while (slab_xml)
	{
		try
		{
			AosSlotMgrSlabPtr slab = OmnNew AosSlotMgrSlab(idx, thisptr, slab_xml, rdata);
			mSlabs.push_back(slab);
		}

		catch (...)
		{
			OmnAlarm << "Failed to create slab: " << slab_xml->toString() << enderr;
		}

		slab_xml = slabs->getNextChild();
	}

	return true;
}


bool
AosSobjSlotMgr::run(
		const int command, 
		AosValue &value,
		const AosXmlTagPtr &parms, 
		const AosRundataPtr &rdata) 
{
	aos_assert_r(rdata, false);
	switch (command)
	{
	case eAddSlot:
		 return addSlot(value, parms, rdata);
		 
	case eGetSlot:
		 return getSlot(value, parms, rdata);
		 
	case eSave:
		 return save(value, parms, rdata);
		 
	default:
		 break;
	}

	AosSetError(rdata, AOSLT_INTERNAL_ERROR);
	OmnAlarm << rdata->getErrmsg() << ". Unrecognized command: " << command << enderr;
	return false;
}


bool
AosSobjSlotMgr::addSlot(
		AosValue &value,
		const AosXmlTagPtr &parms, 
		const AosRundataPtr &rdata) 
{
	// This function adds a slot to the class. Note that slots are managed
	// based on the size. 'parms' should be in the form:
	// 	<parm start="xxx"
	// 		  size="xxx"/>
	int64_t start = parms->getAttrInt64(AOSTAG_START, mInvalidStart);
	int64_t size = parms->getAttrInt64(AOSTAG_SIZE, mInvalidSize);
	if (!isStartValid(AOSLT_ADD_SLOT, start, rdata)) return false;
	if (!isSizeValid(AOSLT_ADD_SLOT, size, rdata)) return false;

	mLock->lock();
	AosSlotMgrSlabPtr slab = getSlab(size, rdata); 
	if (!slab)
	{
		mLock->unlock();
		return false;
	}
		
	if (!addEntry(start, size, rdata)) 
	{
		mLock->unlock();
		return false;
	}

	bool rslt = slab->addSlot(start, size, parms, rdata);
	if (!rslt)
	{
		if (mLogErrors)
		{
			logError(AOSLT_ADD_SLOT, AOSLT_ERROR, rdata->getErrmsg(), rdata);
		}
		mLock->unlock();
		return false;
	}

	mIsDirty = true;
	if (mLogOperations) 
	{
		logAdd(start, size, parms, rdata);
	}
	mLock->unlock();
	return true;
}


AosSlotMgrSlabPtr
AosSobjSlotMgr::getSlab(
		const int64_t size, 
		const AosRundataPtr &rdata)
{
	// Sizes are normalized based on the configurations. It assumes
	// that there are not many sizes. Normalized sizes are managed through
	// an array that is sorted.
	u32 left = 0;
	u32 right = mSlabs.size();
	u32 idx = (left + right) >> 1;
	while (left <= right)
	{
		if (size < mSlabs[idx]->getMinSize())
		{
			if (idx == 0) 
			{
				AosSetError(rdata, AOSLT_INVALID_SIZE);
				OmnAlarm << rdata->getErrmsg() << ". Size: " << size
					<< ". Obj: " << toString() << enderr;
				if (mLogErrors)
				{
					OmnString errmsg = AOSLT_INVALID_SIZE;
					errmsg << ": " << size;
					logError(AOSLT_GET_SLAB, AOSLT_ERROR, errmsg, rdata);
				}
				return 0;
			}
			right = idx-1;
		}
		else if (size > mSlabs[idx]->getMaxSize())
		{
			if (idx == mSlabs.size()) 
			{
				AosSetError(rdata, AOSLT_INVALID_SIZE);
				OmnAlarm << rdata->getErrmsg() << ". Size: " << size
					<< ". Obj: " << toString() << enderr;
				if (mLogErrors)
				{
					OmnString errmsg = AOSLT_INVALID_SIZE;
					errmsg << ": " << size;
					logError(AOSLT_GET_SLAB, AOSLT_ERROR, errmsg, rdata);
				}
				return 0;
			}
			left = idx+1;
		}
		else
		{
			return mSlabs[idx];
		}
		idx = (left + right) >> 1;
	}

	AosSetError(rdata, AOSLT_INVALID_SIZE);
	OmnAlarm << rdata->getErrmsg() << ". Size: " << size
		<< ". Obj: " << toString() << enderr;
	if (mLogErrors)
	{
		OmnString errmsg = AOSLT_INVALID_SIZE;
		errmsg << ": " << size;
		logError(AOSLT_GET_SLAB, AOSLT_ERROR, errmsg, rdata);
	}
	return 0;
}


bool
AosSobjSlotMgr::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
    while (state == OmnThrdStatus::eActive)
    {
		OmnSleep(1000);
	}
	return true;
}


bool    
AosSobjSlotMgr::signal(const int threadLogicId)
{
	return true;
}


bool    
AosSobjSlotMgr::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


bool
AosSobjSlotMgr::getSlot(
		AosValue &value,
		const AosXmlTagPtr &parms, 
		const AosRundataPtr &rdata) 
{
	// This function get a slot from the class.
	int64_t size = value.getInt64(eSizeIdx, mInvalidSize);
	if (!isSizeValid(AOSLT_GET_SLOT, size, rdata)) return false;

	mLock->lock();
	AosSlotMgrSlabPtr slab = getSlab(size, rdata); 
	if (!slab)
	{
		mLock->unlock();
		OmnAlarm << "Failed retrieving slab: " << size << ". Obj: " << toString() << enderr;
		return false;
	}
		
	int64_t start;
	bool rslt = slab->getSlot(start, size, parms, rdata);
	if (rslt)
	{
		mIsDirty = true;
		removeEntry(start, size, rdata);
		value.setInt64(eStatusIdx, 1);
		value.setInt64(eStartIdx, start);
		value.setInt64(eSizeIdx, size);
		if (mLogOperations) logGet(start, size, AOSLT_ERROR, parms, rdata);
		mLock->unlock();
		return true;
	}

	// Did not find it. Check whether it needs to get the 
	// next (bigger) size. 
	int next_tries = slab->getNextTries();
	int idx = slab->getIndex();
	aos_assert_r(idx >= 0, false);
	slab = 0;
	for (int i=0; i<next_tries; i++)
	{
		idx++;
		if ((u32)idx >= mSlabs.size())
		{
			if (mLogErrors)
			{
				OmnString errmsg = AOSLT_FAILED_GET_SLOT;
				errmsg << ": " << size;
				logError(AOSLT_GET_SLOT, AOSLT_ERROR, errmsg, rdata);
			}
			mLock->unlock();
			value.setInt64(eStatusIdx, 0);
			return true;
		}

		slab = mSlabs[idx];
		if (!slab)
		{
			if (mLogErrors)
			{
				OmnString errmsg = AOSLT_INTERNAL_ERROR;
				errmsg << ": " << idx;
				logError(AOSLT_GET_SLOT, AOSLT_ERROR, errmsg, rdata);
			}
			mLock->unlock();
			AosSetError(rdata, AOSLT_INTERNAL_ERROR);
			OmnAlarm << "slab is null: " << toString() << enderr;
			return false;
		}

		rslt = slab->getSlot(start, size, parms, rdata);
		if (rslt)
		{
			// Got the slot.
			if (mLogOperations)
			{
				logGet(start, size, AOSLT_STATUS_OK, parms, rdata);
			}
			mIsDirty = true;
			removeEntry(start, size, rdata);
			mLock->unlock();
			value.setInt64(eStatusIdx, 1);
			value.setInt64(eStartIdx, start);
			value.setInt64(eSizeIdx, size);
		}
	}

	// Did not find it.
	if (mLogErrors)
	{
		OmnString errmsg = AOSLT_FAILED_GET_SLOT;
		errmsg << ": " << size;
		logError(AOSLT_GET_SLOT, AOSLT_ERROR, errmsg, rdata);
	}
	value.setInt64(eStatusIdx, 0);
	mLock->unlock();
	return true;
}


void
AosSobjSlotMgr::logError(
		const OmnString &operation, 
		const OmnString &status, 
		const OmnString &errmsg, 
		const AosRundataPtr &rdata)
{
	OmnString docstr = "<log ";
	docstr << AOSTAG_PARENTC << "=\"" << mErrorCtnrObjid << "\" "
		<< AOSTAG_OPERATION << "=\"" << operation << "\" "
		<< AOSTAG_STATUS << "=\"" << status << "\">";
	if (errmsg != "")
	{
		docstr << "<![CDATA[" << errmsg << "]]></log>";
	}
	AosSeLogClientObj::getSelf()->addLog(mErrorCtnrObjid, mErrorLogname, docstr, rdata);
}


void
AosSobjSlotMgr::logAdd(
		const int64_t &start, 
		const int64_t &size, 
		const AosXmlTagPtr &parms, 
		const AosRundataPtr &rdata)
{
	OmnString docstr = "<log ";
	docstr << AOSTAG_PARENTC << "=\"" << mOprCtnrObjid << "\" "
		<< AOSTAG_OPERATION << "=\"" << AOSLT_ADD_SLOT << "\" "
		<< AOSTAG_STATUS << "=\"" << AOSLT_STATUS_OK << "\" "
		<< AOSTAG_START << "=\"" << start << "\" "
		<< AOSTAG_SIZE << "=\"" << size << "\"/>";
	AosSeLogClientObj::getSelf()->addLog(mOprCtnrObjid, mOprLogname, docstr, rdata);
}


void
AosSobjSlotMgr::logGet(
		const int64_t &start, 
		const int64_t &size, 
		const OmnString &status,
		const AosXmlTagPtr &parms, 
		const AosRundataPtr &rdata)
{
	OmnString docstr = "<log ";
	docstr << AOSTAG_PARENTC << "=\"" << mOprCtnrObjid << "\" "
		<< AOSTAG_OPERATION << "=\"" << AOSLT_GET_SLOT << "\" "
		<< AOSTAG_STATUS << "=\"" << AOSLT_STATUS_OK << "\" "
		<< AOSTAG_START << "=\"" << start << "\" "
		<< AOSTAG_SIZE << "=\"" << size << "\"/>";
	AosSeLogClientObj::getSelf()->addLog(mOprCtnrObjid, mOprLogname, docstr, rdata);
}


bool 
AosSobjSlotMgr::save(AosValue &value, const AosXmlTagPtr &parms, const AosRundataPtr &rdata)
{
	// This function assumes that the class is locked!!!.
	aos_assert_rr(mDoc, rdata, false);
	if (!mIsDirty) return true;
	mDoc->removeNode(AOSTAG_SLABS, false, true);
	OmnString contents = "<";
	contents << AOSTAG_SLABS <<">";
	for (u32 i=0; i<mSlabs.size(); i++)
	{
		mSlabs[i]->serializeTo(contents, rdata);
	}
	contents << "</" << AOSTAG_SLABS << ">";
	mDoc->addNode1(AOSTAG_SLABS, contents);
	OmnString docstr = mDoc->toString();
	return AosDocClientObj::getDocClient()->saveDoc(docstr, false, rdata);
}


OmnString
AosSobjSlotMgr::toString() const
{
	OmnString str;
	str << "MinStart: " << mMinStartFlag << ":" << mMinStart
		<< "\nMaxStart: " << mMaxStartFlag << ":" << mMaxStart
		<< "\nMinSize: " << mMinSizeFlag << ":" << mMinSize
		<< "\nMaxSize: " << mMaxSizeFlag << ":" << mMaxSize
		<< "\nLogOperations: " << mLogOperations
		<< "\nLogErrors: " << mLogErrors
		<< "\nInvalidStart: " << mInvalidStart
		<< "\nInvalidSize: " << mInvalidSize
		<< "\nObjid: " << mObjid
		<< "\nOprCtnrObjid: " << mOprCtnrObjid << ":" << mOprLogname
		<< "\nErrorCtnrObjid: " << mErrorCtnrObjid << ":" << mErrorLogname;
	return str;
}


AosSmartObjPtr 
AosSobjSlotMgr::clone()
{
	return OmnNew AosSobjSlotMgr(false);
}


bool
AosSobjSlotMgr::addEntry(
		const int64_t &start, 
		const int64_t &size, 
		const AosRundataPtr &rdata)
{
	// This function adds an entry [start, size] to mEntries. mEntries
	// are sorted based on 'start'.
	u32 left = 0;
	u32 right = mEntries.size();
	u32 idx = (left + right) >> 1;
	if (mEntries.size() == 0)
	{
		mEntries.push_back(entry_t(start, size));
		return true;
	}

	while (left < right)
	{
		if (size < mEntries[idx].start)
		{
			if (idx == 0) 
			{
				if (start + size > mEntries[0].start)
				{
					// This means that this entry overlaps with the next one.
					// This is an error.
					AosSetError(rdata, AOSLT_SIZE_TOO_BIG);
					OmnAlarm << rdata->getErrmsg() << ". Start: " 
						<< start << ":" << size 
						<< ":" << mEntries[0].start << enderr;
					return false;
				}

				mEntries.insert(mEntries.begin(), entry_t(start, size));
				return true;
			}
			right = idx-1;
		}
		else if (size > mEntries[idx].start)
		{
			if (idx == mEntries.size()) 
			{
				mEntries.push_back(entry_t(start, size));
				return true;
			}
			left = idx+1;
		}
		else
		{
			// This means the start is incorrect.
			AosSetError(rdata, AOSLT_DUPLICATED_START);
			OmnAlarm << rdata->getErrmsg() << ". Start: " << start << ":" << size << enderr;
			return false;
		}
		idx = (left + right) >> 1;
	}

	if (left != right)
	{
		AosSetError(rdata, AOSLT_INTERNAL_ERROR);
		OmnAlarm << rdata->getErrmsg() << ". Start: " << start << ":" << size << enderr;
		return false;
	}

	if (start == mEntries[idx].start)
	{
		AosSetError(rdata, AOSLT_DUPLICATED_START);
		OmnAlarm << rdata->getErrmsg() << ". Start: " << start << ":" << size << enderr;
		return false;
	}

	if (start < mEntries[idx].start)
	{
		if (start + size > mEntries[idx].start)
		{
			// This means that this entry overlaps with the next one.
			// This is an error.
			AosSetError(rdata, AOSLT_SIZE_TOO_BIG);
			OmnAlarm << rdata->getErrmsg() << ". Start: " 
				<< start << ":" << size 
				<< ":" << mEntries[0].start << enderr;
			return false;
		}

		// insert before idx
		mEntries.insert(mEntries.begin() + idx, entry_t(start, size));
		return true;
	}
	
	// insert after idx
	if (idx == mEntries.size()-1)
	{
		mEntries.push_back(entry_t(start, size));
		return true;
	}

	if (start + size > mEntries[idx+1].start)
	{
		// This means that this entry overlaps with the next one.
		// This is an error.
		AosSetError(rdata, AOSLT_SIZE_TOO_BIG);
		OmnAlarm << rdata->getErrmsg() << ". Start: " 
			<< start << ":" << size 
			<< ":" << mEntries[0].start << enderr;
		return false;
	}
	mEntries.insert(mEntries.begin() + idx + 1, entry_t(start, size));
	return true;
}


bool
AosSobjSlotMgr::removeEntry(
		const int64_t &start, 
		const int64_t &size, 
		const AosRundataPtr &rdata)
{
	// This function removes the entry [start, size]. mEntries
	// are sorted based on 'start'.
	u32 left = 0;
	u32 right = mEntries.size();
	u32 idx = (left + right) >> 1;

	while (left <= right)
	{
		if (size < mEntries[idx].start)
		{
			if (idx == 0) 
			{
				AosSetError(rdata, AOSLT_ENTRY_NOT_FOUND);
				OmnAlarm << rdata->getErrmsg() << ". Start: " 
					<< start << ":" << size << enderr;
				return false;
			}
			right = idx-1;
		}
		else if (size > mEntries[idx].start)
		{
			if (idx == mEntries.size()) 
			{
				AosSetError(rdata, AOSLT_ENTRY_NOT_FOUND);
				OmnAlarm << rdata->getErrmsg() << ". Start: " 
					<< start << ":" << size << enderr;
				return false;
			}
			left = idx+1;
		}
		else
		{
			mEntries.erase(mEntries.begin() + idx);
			return true;
		}
		idx = (left + right) >> 1;
	}

	AosSetError(rdata, AOSLT_ENTRY_NOT_FOUND);
	OmnAlarm << rdata->getErrmsg() << ". Start: " << start << ":" << size << enderr;
	return false;
}


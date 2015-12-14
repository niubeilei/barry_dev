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
// This file contains all the unused member functions for IILMgr.
//
// Modification History:
// 08/12/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////

/*
AosIILPtr
AosIILMgr::loadIILByIDPriv(
		const u64 &iilid, 
		const u64 &wordid,
		const AosIILType &iiltype, 
		const char *word,
		const int len,
		const bool IILMgrLocked,
		bool &status,
		const AosRundataPtr &rdata)
{
	status = false;
	if(needPopIIL(iiltype))
	//if(AosIIL::getTotalIILs(iiltype) >= (int)mMaxIILs)
	{
		char bb[AosIIL::eIILHeaderMemsize+10];
		AosBuff buff(bb, AosIIL::eIILHeaderMemsize, 0);
		if (!AosIIL::staticReadHeaderBuff(iilid, buff))
		{
			rdata->setError() << "Failed to read buff!";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return 0;
		}

		AosIILType itype;
		if (!AosIILType_toCode(itype, buff.getChar(eAosIILType_Invalid)))
		{
			rdata->setError() << "Incorrect iiltype: " << itype;
			OmnAlarm << rdata->getErrmsg() << enderr;
			return 0;
		}

		if (itype == eAosIILType_Invalid)
		{
			rdata->setError() << "Invalid IIL type!";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return 0;
		}

		if (iiltype != eAosIILType_Invalid && itype != iiltype)
		{
			rdata->setError() << "Type mismatch:" << itype << ":" << iiltype;
			OmnAlarm << rdata->getErrmsg() << enderr;
			return 0;
		}

		// Need to pop an IIL. Since calling 'popIILPriv()' may locking
		// the IIL, need to unlock this class before calling it.
		AosIILPtr iil = popIILPriv(wordid, iilid, iiltype, false, IILMgrLocked, rdata);
		
		if (iil) 
		{
			if (!iil->loadFromFileSafe1(iilid, IILMgrLocked, rdata))
			{
				rdata->setError() << "Failed to retrieve the IIL: " 
					<< wordid << ":" << iil.getPtr();
				OmnAlarm << rdata->getErrmsg() << enderr;
				return iil;
			}

//			if(wordid)
//			{
//				iil->setPtrToWordHashByIILMgr();
//			}

			// Chen Ding, 05/05/2011
			mIILIDHash[iilid] = iil;
			status = true;
			return iil;
		}
	}

	AOSIILCOUNT_LOADS;

	// Chen Ding, 08/03/2011
	// Check whether it is in the hash.
	// if (mIILIDHash.count(iilid))
	IILIDHashItr itr = mIILIDHash.find(iilid);
	if (itr != mIILIDHash.end())
	{
		AosIILPtr iil = itr->second;
		iil->addRefCountByIILMgr();
		status = true;
		//shawn do we need to check iiltype?
		return iil;
	}
	AosIILPtr iil = AosIIL::staticLoadFromFile1(iilid, IILMgrLocked, rdata);
	if (!iil)
	{
		// The IIL should be there, but it failed to create it. 
		// This is a serious problem. 
		OmnString www(word, len);
		rdata->setError() << "Failed to retrieve the IIL: " << www << ":" << iiltype;
		OmnAlarm << rdata->getErrmsg() << enderr;

		// Try to repair the IIL by recreating the IIL
		iil = AosIIL::staticCreateIIL(wordid, iilid, iiltype, www, IILMgrLocked, rdata);
		aos_assert_r(iil, 0);
	}

//	if (wordid)
//	{
//		iil->setPtrToWordHashByIILMgr();
//	}

	// Chen Ding, 05/05/2011
	mIILIDHash[iilid] = iil;
	iil->addRefCountByIILMgr();
	int idx = mTotalIILs++;
	mIILs[idx] = iil;
	status = true;
	return iil;
}
*/

	
/*
AosIILPtr
AosIILMgr::retrieveIIL(
		const char *word, 
		const int len, 
		const AosIILType &iiltype, 
		const bool createFlag,
		const AosRundataPtr &rdata)
{
	// This function gets the IIL associated with 'wordid'.
	// If the IIL is not already in memory, it will 
	// load the IIL if 'loadFlag' is set to true.
	//
	// IMPORTANT: If 'createFlag' is false, the IIL was already
	// created. Failing to retrieve the IIL is considered an error
	// by this function.
	//
	// For each Docid IIL, if it is not a sorted list, there is a companion
	// sorted IIL for it. 
	// Whether an IIL is loaded in memory or where
	// the IIL is (that is, the file seqno and offset) can be determined
	// by the Word hash table. If the IIL has not been loaded in 
	// memory, we will use the Word hash table to get the IILID
	// and retrieve the IIL into memory. If the IIL is already
	// loaded in memory, we need to check whether the loaded is the
	// sorted. If not, the list should tell whether the sorted version 
	// was loaded in memory (through the member data 'mCompIIL'. 
	// If mCompIIL is not null, that's the IIL we want to retrieve. 
	// Otherwise, the same seqno and offset may be used to retrieve
	// the companion list (note that we assume that a sorted list uses
	// exactly the same amount of disk space as the original list, 
	// i.e., as far as memory/disk usage is concerned, they are 
	// identical). 
	//
	if(len <=0 || len >= AosIILUtil::eMaxStrValueLen)
	{
		char www[AosIILUtil::eMaxStrValueLen+1];
		strncpy(www,word,AosIILUtil::eMaxStrValueLen);
		www[AosIILUtil::eMaxStrValueLen] = 0;
		OmnScreen << "word len not right:" << len << ":" << www;
		return 0;
	}

	if (mShowLocking) OmnScreen << "Locking" << endl;
	AOSLMTR_ENTER(mLockMonitor);
	AOSLMTR_LOCK(mLockMonitor);
	AOSIILCOUNT_TRIES;
	if (mIsStopping)
	{
		if (mShowLocking) OmnScreen << "Unlocking" << endl;
		AOSIILCOUNT_MISS;
		AOSLMTR_UNLOCK(mLockMonitor);
		AOSLMTR_FINISH(mLockMonitor);
		return 0;
	}
	AOSLMTR_ATTEMPT(mLockMonitor);
	u64	wordid = AosWordClient::getSelf()->getWordId((u8 *)word, len, false);
	AOSLMTR_ATTEMPT(mLockMonitor);
	if (wordid == AOS_INVWID)
	{
		if (!createFlag) 
		{
			if (mShowLocking) OmnScreen << "Unlocking" << endl;
			AOSLMTR_UNLOCK(mLockMonitor);
			AOSLMTR_FINISH(mLockMonitor);
			AOSIILCOUNT_MISS;
			return 0;
		}

		aos_assert_rk(iiltype > eAosIILType_Invalid &&
				iiltype < eAosIILType_Total, mLock, 0);

		// As soon as an iil is created, it is locked.
		AOSLMTR_ATTEMPT(mLockMonitor);
		AosIILPtr iil = createIIL(word, len, iiltype, false, true, rdata);
		AOSLMTR_ATTEMPT(mLockMonitor);
		if (mShowLocking) OmnScreen << "Unlocking" << endl;
		AOSLMTR_UNLOCK(mLockMonitor);
		aos_assert_r(iil, 0);

		AOSLMTR_FINISH(mLockMonitor);
		return iil;
	}

	u64 ptr = 0;
	int count = 0;
	AosIILPtr iil;
	while (count++ < eMaxRetrieveIILTries)
	{
		AOSLMTR_ATTEMPT(mLockMonitor);
		if (!AosWordClient::getSelf()->getPtr(wordid, ptr))
		{
			AOSLMTR_ATTEMPT(mLockMonitor);
			if (mShowLocking) OmnScreen << "Unlocking" << endl;
			AOSLMTR_UNLOCK(mLockMonitor);
			rdata->setError() << "Failed to get ptr: " << wordid;
			OmnAlarm << rdata->getErrmsg() << enderr;
			AOSLMTR_FINISH(mLockMonitor);
			return 0;
		}
		AOSLMTR_ATTEMPT(mLockMonitor);

		// Currently, 'ptr' is arranged below.
		// Byte 0-3:	an integer used as the real offset
		// Next 12 bits:used as the sequence number
		// Byte 5:		The highest bit identifies whether 
		// 			 	the IIL is already loaded into memory (1)
		// 			 	or not (0). If yes, Byte 0-3 is the pointer to 
		// 			 	the IIL. 

		// 3. Determine whether the IIL is already in memory. 
		if (mCheckPtr)
		{
			AosIIL *ppp = (AosIIL *)ptr;
			bool found = false;
			if (ppp)
			{
				for (u32 i=0; i<mTotalIILs; i++)
				{
					if (mIILs[i].getPtr() == ppp)
					{
						found = true;
						break;
					}
				}
				aos_assert_rk(found, mLock, 0);
			}
		}

		iil = (AosIIL *)ptr;
		if (!iil)
		{
			// The IIL has not been loaded into memory yet. 
			break;
		}

		// The IIL has been loaded into memory. Need to lock the iil.
		// But this is very dangerous because if the iil was locked
		// by someone, trying to lock the iil will cause this class
		// to wait for the iil to unlock while keeping this class
		// being locked. 
		//
		// Need to lock 'iil' to memory, which means that the iil shall not
		// be reused for other iil. To prevent lockup, it is important
		// that 'addRefCount()' and 'removeRefCount()' shall be called
		// when 'mLock' is locked. IILs uses a counter (i.e., reference
		// counter) to determine whether an IIL can be re-used for other
		// IILs. 
		//
		// !!!!!!!!!!!!!!!!!!!IMPORTANT!!!!!!!!!!!!!!!!!!!!!!!!
		// It is extremely important that 'addRefCount()' and 'removeRefCount()'
		// shall be called only when 'mLock' is locked. Otherwise, it 
		// can cause unpredictable problems.
		AOSLMTR_ATTEMPT(mLockMonitor);
		u32 refcount = iil->addRefCountByIILMgr();
		AOSLMTR_ATTEMPT(mLockMonitor);

		// Check whether the IIL is still associated with 'wordid'.
		if (iil->getWordIdSafe() != wordid)
		{
			// It is no longer associated with 'wordid'. Need to 
			// return 'iil' and try it again.
			AOSLMTR_ATTEMPT(mLockMonitor);
			if (mShowLocking) OmnScreen << "Unlocking" << endl;
			AOSLMTR_UNLOCK(mLockMonitor);
			returnIILPublic(iil, rdata);
			iil = 0;
			if (mShowLocking) OmnScreen << "Locking" << endl;
			AOSLMTR_LOCK(mLockMonitor);
			continue;
		}
		AOSLMTR_ATTEMPT(mLockMonitor);

		// To ensure everything is the same as before, 
		// we will re-retrieve the ptr and check it again.
		u64 ptr1;
		if (!AosWordClient::getSelf()->getPtr(wordid, ptr1))
		{
			// Should never happen
			AOSLMTR_ATTEMPT(mLockMonitor);

			char www[AosIILUtil::eMaxStrValueLen+1];
			strncpy(www,word,AosIILUtil::eMaxStrValueLen);
			www[AosIILUtil::eMaxStrValueLen] = 0;
			rdata->setError() << "Ptrs mismatch: " << www << ":" << iiltype;
			OmnAlarm << rdata->getErrmsg() << enderr;

			if (mShowLocking) OmnScreen << "Unlocking" << endl;
			AOSLMTR_UNLOCK(mLockMonitor);
			returnIILPublic(iil, rdata);
			AOSLMTR_ATTEMPT(mLockMonitor);
			iil = 0;
			rdata->setError() << "Failed to get ptr: " << wordid;
			OmnAlarm << rdata->getErrmsg() << enderr;
			AOSLMTR_FINISH(mLockMonitor);
			return 0;
		}

		AOSLMTR_ATTEMPT(mLockMonitor);
		if (ptr != ptr1)
		{
			// Something has changed. This should normally not 
			// happen. We are not sure whether it could ever 
			// happen. But to be careful, we will return 'iil'
			// and give it another try.

			char www[AosIILUtil::eMaxStrValueLen+1];
			strncpy(www,word,AosIILUtil::eMaxStrValueLen);
			www[AosIILUtil::eMaxStrValueLen] = 0;

			rdata->setError() << "Ptrs mismatch: " << www << ":" << iiltype;
			OmnAlarm << rdata->getErrmsg() << enderr;

			if (mShowLocking) OmnScreen << "Unlocking" << endl;
			AOSLMTR_UNLOCK(mLockMonitor);
			returnIILPublic(iil, rdata);
			if (mShowLocking) OmnScreen << "Locking" << endl;
			AOSLMTR_LOCK(mLockMonitor);
			iil = 0;
			continue;
		}

		// Looks good. Safe to proceed.
		if (refcount == 1)
		{
			// The IIL is in the iil pool. Need to remove it
			// from the iil pool.
			AOSLMTR_ATTEMPT(mLockMonitor);
			removeFromIILListPriv(iil, rdata);
			AOSLMTR_ATTEMPT(mLockMonitor);
		}
		if (mShowLocking) OmnScreen << "Unlocking" << endl;
		
		AOSIILCOUNT_HITS;
		AOSLMTR_UNLOCK(mLockMonitor);
		AOSLMTR_FINISH(mLockMonitor);
		return iil;
	}
	
	AOSLMTR_ATTEMPT(mLockMonitor);
	aos_assert_rk(count < eMaxRetrieveIILTries, mLock, 0);

	// Not loaded in memory yet. Load it.
	u64 iilid = (ptr >> 32);
	bool status;
	iil = loadIILByIDPriv(iilid, wordid, iiltype, word, len, true, status, rdata);
	AOSLMTR_ATTEMPT(mLockMonitor);
	if (mShowLocking) OmnScreen << "Unlocking" << endl;
	AOSLMTR_UNLOCK(mLockMonitor);
	if (!status)
	{
		returnIILPublic(iil, rdata);
		AOSLMTR_FINISH(mLockMonitor);
		return 0;
	}
	AOSLMTR_FINISH(mLockMonitor);
	return iil;
}
*/


/*
bool	
AosIILMgr::querySafe(
		const u64 &iilid,
		const AosQueryRsltObjPtr query_rslt,
		const AosBitmapObjPtr query_bitmap,
		const AosOpr opr,
		const u64 &value, 
		const AosRundataPtr &rdata)
{
	AosIILType iiltype = eAosIILType_Invalid;
	AosIILPtr iil = loadIILByIDPublic(iilid, iiltype, rdata);
	aos_assert_r(iil, false);
	
	if(iil->getIILType() != eAosIILType_U64)
	{
		OmnScreen << "Iil type mismatch. expect to get a u64 iil:" << iiltype << endl;
		returnIILPublic(iil, rdata);
		return false;
	}
	
	AosIILU64Ptr iilu64 = (AosIILU64*)iil.getPtr();
	bool rslt = iilu64->querySafe(query_rslt, query_bitmap, opr, value, rdata);
	returnIILPublic(iil, rdata);
	return rslt;
}
*/


/*
bool
AosIILMgr::addDoc(
		const char *word, 
		const int len, 
		const u64 &docid,
		const AosRundataPtr &rdata) 
{
	if(len <=0 || len >= AosIILUtil::eMaxStrValueLen)
	{
		char www[AosIILUtil::eMaxStrValueLen+1];
		strncpy(www,word,AosIILUtil::eMaxStrValueLen);
		www[AosIILUtil::eMaxStrValueLen] = 0;
		OmnScreen << "word len not right:" << len << ":" << www;	
		return false;
	}
	
	if (mShowLog)
	{
		OmnString ww(word, len);
		OmnScreen << "Add word: " << ww << ":" << docid << endl;
	}

	if (mCheckRef) aos_assert_r(AosIIL::staticNumIILRefs() == 0, false);

	AosIILPtr iil = retrieveIIL(word, len, eAosIILType_Hit, true, rdata);
	aos_assert_r(iil, false);
	aos_assert_r(iil->getCompIIL()->getIILID() > 0 , false);

	if (iil->getIILType() != eAosIILType_Hit)
	{
		OmnString ww(word, len);
		OmnScreen << "Word: " << ww << endl;

		rdata->setError() << "IIL type mismatch: " << iil->getIILType() << ":eAosIILType_Hit";
		OmnAlarm << rdata->getErrmsg() << enderr;
		
		returnIILPublic(iil, rdata);
		return false;
	}

	AosIILHit *hitiil = (AosIILHit *)iil.getPtr();
	bool rslt = hitiil->addDocSafe(docid, rdata);
	aos_assert_r(iil->getCompIIL()->getIILID() > 0 , false);

	returnIILPublic(iil, rdata);
	if (mCheckRef) aos_assert_r(AosIIL::staticNumIILRefs() == 0, false);
	return rslt;
}


bool        
AosIILMgr::addValueDoc(
		 const OmnString &name,
		 const OmnString &value,
		 const u64 &docid, 
		 const bool value_unique, 
		 const bool docid_unique,
		 const AosRundataPtr &rdata)
{
	// This function adds a doc to a value-IIL. The tag is 
	// 'tagname' + 'attrname'. The value is 'value'. 
	// If the IIL has not been created yet, it will create it.
	if(name.length() <= 0 || name.length() >= AosIILUtil::eMaxStrValueLen)
	{
		OmnScreen << "Add word: " << name << ":" << value << ":" << docid << endl;
		return false;
	}

	if(!(value.length() > 0 && value.length() < AosIILUtil::eMaxStrValueLen))
	{
		OmnString value2;
		if(value.length() > 300)
		{
			value2 = value.subString(0,300);
		}else
		{
			value2 = value;
		}
		OmnScreen << "The value is too long. Len: " << value.length() << 
			" , Word:" << value2 << " , docid:" << docid << endl;
		return false;
	}

//u64 t1 = OmnGetTimestamp();
	if (mShowLog) OmnScreen << "Add str value: " << name << ":" << value << ":" << docid << endl;
	if (mCheckRef) aos_assert_r(AosIIL::staticNumIILRefs() == 0, false);
	
	AosIILPtr iil = retrieveIIL(name.data(), name.length(), eAosIILType_Str, true, rdata);
//u64 t2 = OmnGetTimestamp();
	aos_assert_r(iil, false);
	if (iil->getIILType() != eAosIILType_Str)
	{
		// This should never happen. In our search engine, one keyword
		// maps to one and only one IIL type. The mapping is established
		// when one of the 'IILMgr::addDoc(...)' function is called. If
		// a keyword is used to index, say, u32, it should not be used
		// to re-index 'str'. When this error happens, it is probably
		// the case that the same keyword is used inconsistently. 
		rdata->setError() << "Retrieved is not strIIL: " << iil->getIILType() << ":" << name;
		OmnAlarm << rdata->getErrmsg() << enderr;
		
		returnIILPublic(iil, rdata);
		return false;
	}

	AosIILStr *striil = (AosIILStr *)iil.getPtr();
	bool rslt = striil->addDocSafe(value, docid, value_unique, docid_unique, false, rdata);
	if (mCheckRef) aos_assert_r(AosIIL::staticNumIILRefs() == 0, false);
//u64 t3 = OmnGetTimestamp();
	returnIILPublic(iil, rdata);

//	if(t2-t1 > 200000) cout << "=======Loading time: " << t2 - t1
//		<< ", name:" << name << ", value:" << value << ", iilid:" << docid << endl;
//  if(t3-t2 > 200000) cout << "=======Process time: " << t3 - t2 
//		<< ", name:" << name << ", value:" << value << ", iilid:" << docid << endl;

	return rslt;
}


bool
AosIILMgr::addValueDoc(
		const OmnString &iilname, 
		const OmnString &value, 
		const OmnString &docid, 
		const bool value_unique,
		const bool docid_unique,
		const AosRundataPtr &rdata)
{
	u64 did = atoll(docid.data());
	return addValueDoc(iilname, value, did, value_unique, docid_unique, rdata);
}


bool
AosIILMgr::addValueDoc(
		const OmnString &iilname, 
		const OmnString &attrname, 
		const OmnString &value, 
		const u64 &docid, 
		const bool value_unique,
		const bool docid_unique,
		const AosRundataPtr &rdata)
{
	aos_assert_r(attrname != "", false);
	OmnString ww = iilname;
	ww << attrname;
	return addValueDoc(ww, value, docid, value_unique, docid_unique, rdata);
}


bool        
AosIILMgr::addU64ValueDoc(
		 const OmnString &word,
		 const u64 &value,
		 const u64 &docid, 
		 const bool value_unique,
		 const bool docid_unique,
		 const AosRundataPtr &rdata)
{
	if(word.length() <= 0 || word.length() >= AosIILUtil::eMaxStrValueLen)
	{
		OmnScreen << "Add word: " << word << ":" << value << ":" << docid << endl;
		return false;
	}

	if (mShowLog) OmnScreen << "Add u32: " << word << ": " << value << ":" << docid << endl;
	if (mCheckRef) aos_assert_r(AosIIL::staticNumIILRefs() == 0, false);

	AosIILPtr iil = retrieveIIL(word.data(), word.length(), eAosIILType_U64, true, rdata);
	aos_assert_r(iil, false);
	if (iil->getIILType() != eAosIILType_U64)
	{
		rdata->setError() << "IIL type mismatch: " << iil->getIILType()	<< ": eAosIILType_U64";
		OmnAlarm << rdata->getErrmsg() << enderr;

		returnIILPublic(iil, rdata);
		return false;
	}

	AosIILU64 *u64iil = (AosIILU64 *)iil.getPtr();
	bool rslt = u64iil->addDocSafe(value, docid, value_unique, docid_unique,false, rdata);
	if (mCheckRef) aos_assert_r(AosIIL::staticNumIILRefs() == 0, false);
	returnIILPublic(iil, rdata);
    // if(u2-u1 > 200000) cout << "=======u2 - u1:" << u2 - u1 << endl;
    // if(u3-u2 > 200000) cout << "=======u3 - u2:" << u3 - u2 << endl;
    // if(u4-u3 > 200000) cout << "=======u4 - u3:" << u4 - u3 << endl;
	return rslt;
}

	
bool
AosIILMgr::addU64ValueDoc(
		const OmnString &iilname, 
		const OmnString &attrname,
		const u64 value, 
		const u64 &docid, 
		const bool value_unique,
		const bool docid_unique,
		const AosRundataPtr &rdata)
{
	aos_assert_r(attrname != "", false);	
	OmnString in = iilname;
	in << attrname;
	return addU64ValueDoc(in, value, docid, value_unique, docid_unique, rdata);
}


bool 		
AosIILMgr::removeDoc(
		const char *word, 
		const int len, 
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	if(len <=0 || len >= AosIILUtil::eMaxStrValueLen)
	{
		char www[AosIILUtil::eMaxStrValueLen+1];
		strncpy(www,word,AosIILUtil::eMaxStrValueLen);
		www[AosIILUtil::eMaxStrValueLen] = 0;
		OmnScreen << "word len not right:" << len << ":" << www;	
		return false;
	}
	// It removes the docid from the IIL identified by 'word'.
	if (mShowLog)
	{
		OmnString ww(word, len);
		OmnScreen << "Remove doc: " << ww << ":" << docid << endl;
	}

	if (mCheckRef) aos_assert_r(AosIIL::staticNumIILRefs() == 0, false);
	AosIILPtr iil = retrieveIIL(word, len, eAosIILType_Hit, false, rdata);
	if (!iil)
	{
		rdata->setError() << "Failed to retrieve the IIL for: " << word;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	if (iil->getIILType() != eAosIILType_Hit)
	{
		rdata->setError() << "IIL type mismatch: " << iil->getIILType() << ":eAosIILType_Hit";
		OmnAlarm << rdata->getErrmsg() << enderr;

		returnIILPublic(iil, rdata);
		return false;
	}

	AosIILHit *hitiil = (AosIILHit *)iil.getPtr();
	bool rslt = hitiil->removeDocSafe(docid, rdata);

	returnIILPublic(iil, rdata);
	if (mCheckRef) aos_assert_r(AosIIL::staticNumIILRefs() == 0, false);
	return rslt;
}


bool
AosIILMgr::removeDoc(                    
		const OmnString &name1,   
		const OmnString &name2,   
		const u64 &docid,         
		const AosRundataPtr &rdata)
{
	if (name2 == "") return true;
	OmnString tag = name1;                            
	tag << name2;                                     
	return removeDoc(tag.data(), tag.length(), docid, rdata);
}


bool
AosIILMgr::removeValueDoc(
		const OmnString &name, 
		const OmnString &value, 
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	if(name.length() <= 0 || name.length() >= AosIILUtil::eMaxStrValueLen)
	{
		OmnScreen << "remove doc from word: " << name << ":" << value << ":" << docid << endl;
		return false;
	}
	// It removes the docid from the IIL identified by 'word'.
	if (mShowLog) OmnScreen << "Remove val: " << name << ":" << value << ":" << docid << endl;
	if (mCheckRef) aos_assert_r(AosIIL::staticNumIILRefs() == 0, false);
	AosIILPtr iil = retrieveIIL(name.data(), name.length(), eAosIILType_Str, false, rdata);

	// Note that it is possible that the entry does not exist. 
	// In the current implementation, we do not treat it as an error.
	// But if wordid != AOS_INVWID, there should be an IIL.
	if (!iil) return true;

	if (iil->getIILType() != eAosIILType_Str)
	{
		rdata->setError() << "IIL type mismatch: " << iil->getIILType()
			<< ": eAosIILType_Str. name=" << name << ". value=" << value;
		OmnAlarm << rdata->getErrmsg() << enderr;
		returnIILPublic(iil, rdata);
		return false;
	}

	AosIILStr *striil = (AosIILStr *)iil.getPtr();
	bool rslt = striil->removeDocSafe(value, docid, rdata);
	returnIILPublic(iil, rdata);
	if (mCheckRef) aos_assert_r(AosIIL::staticNumIILRefs() == 0, false);
	return rslt;
}


bool
AosIILMgr::removeValueDoc(
		const OmnString &name1,    
		const OmnString &name2,    
		const OmnString &value,    
		const u64 &docid,          
		const AosRundataPtr &rdata)
{
	aos_assert_r(name2 != "", false);            
	OmnString iilname = name1;                   
	iilname << name2;                            
	return removeValueDoc(iilname, value, docid, rdata);
}


bool
AosIILMgr::removeU64ValueDoc(
		const OmnString &name, 
		const u64 &value, 
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	if(name.length() <= 0 || name.length() >= AosIILUtil::eMaxStrValueLen)
	{
		OmnScreen << "remove doc from word: " << name << ":" << value 
			<< ":" << docid << endl;
		return false;
	}
	// It removes the docid from the IIL identified by 'word'.
	if (mShowLog) OmnScreen << "Remove val: " << name << ":" << value << ":" << docid << endl;
	if (mCheckRef) aos_assert_r(AosIIL::staticNumIILRefs() == 0, false);
	AosIILPtr iil = retrieveIIL(name.data(), name.length(), eAosIILType_U64, false, rdata);

	// Note that it is possible that the entry does not exist. 
	// In the current implementation, we do not treat it as an error.
	// But if wordid != AOS_INVWID, there should be an IIL.
	if (!iil) return true;

	if (iil->getIILType() != eAosIILType_U64)
	{
		rdata->setError() << "IIL type mismatch: " << iil->getIILType() << ": eAosIILType_Str";
		OmnAlarm << rdata->getErrmsg() << enderr;
		returnIILPublic(iil, rdata);
		return false;
	}

	AosIILU64 *u64iil = (AosIILU64*)iil.getPtr();
	bool rslt = u64iil->removeDocSafe(value, docid, rdata);
	returnIILPublic(iil, rdata);
	if (mCheckRef) aos_assert_r(AosIIL::staticNumIILRefs() == 0, false);
	return rslt;
}


bool
AosIILMgr::modifyValueDoc(
		const OmnString &name, 
		const OmnString &oldvalue, 
		const OmnString &newvalue, 
		const bool value_unique,
		const bool docid_unique,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	if(name.length() <= 0 || name.length() >= AosIILUtil::eMaxStrValueLen)
	{
		OmnScreen << "Mod word: " << name << ":" << oldvalue 
			<< ":" << newvalue << ":" << docid << endl;
		return false;
	}

	// It removes the docid from the IIL identified by 'word'.
	if (mShowLog)
	{
		OmnScreen << "Modify strval: " << name << ":" 
			<< oldvalue << ":" <<docid << ";" << newvalue << ":" << docid << endl;
	}
	aos_assert_r(newvalue.length() < AosIILUtil::eMaxStrValueLen, false);
	if (mCheckRef) aos_assert_r(AosIIL::staticNumIILRefs() == 0, false);
	AosIILPtr iil = retrieveIIL(name.data(), name.length(), eAosIILType_Str, false, rdata);
	if (!iil)
	{
		rdata->setError() << "Failed to retrieve IIL: " << name << ":" << oldvalue << ":" << docid <<";" << newvalue << ":" << docid;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	if (iil->getIILType() != eAosIILType_Str)
	{
		rdata->setError() << "IIL type mismatch: " << iil->getIILType() << ": eAosIILType_Str";
		OmnAlarm << rdata->getErrmsg() << enderr;
		returnIILPublic(iil, rdata);
		return false;
	}

	AosIILStr *striil = (AosIILStr *)iil.getPtr();
	bool rslt = striil->modifyDocSafe(oldvalue, 
		newvalue, docid, value_unique, docid_unique, false, rdata);
	returnIILPublic(iil, rdata);
	if (mCheckRef) aos_assert_r(AosIIL::staticNumIILRefs() == 0, false);
	return rslt;
}


bool
AosIILMgr::modifyValueDoc(
		const OmnString &name, 
		const OmnString &oldvalue, 
		const OmnString &newvalue, 
		const bool value_unique,
		const bool docid_unique,
		const OmnString &docid,
		const AosRundataPtr &rdata)
{
	u64 did = atoll(docid.data());
	return modifyValueDoc(name, oldvalue, newvalue, value_unique, docid_unique, did, rdata);
}


bool		
AosIILMgr::modifyU64ValueDoc(
		const OmnString &name, 
		const u64 &oldvalue, 
		const u64 &newvalue, 
		const bool value_unique,
		const bool docid_unique,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	if(name.length() <= 0 || name.length() >= AosIILUtil::eMaxStrValueLen)
	{
		OmnScreen << "Mod word: " << name << ":" << oldvalue 
			<< ":" << newvalue << ":" << docid << endl;
		return false;
	}

	// It removes the docid from the IIL identified by 'word'.
	if (mShowLog)
	{
		OmnScreen << "Modify u64: " << name << ":" 
			<< oldvalue << ":" << docid << ";" << newvalue << ":" << docid << endl;
	}
	if (mCheckRef) aos_assert_r(AosIIL::staticNumIILRefs() == 0, false);
	AosIILPtr iil = retrieveIIL(name.data(), name.length(), eAosIILType_U64, false, rdata);
	aos_assert_r(iil, false);

	if (iil->getIILType() != eAosIILType_U64)
	{
		rdata->setError() << "IIL type mismatch: " << iil->getIILType()	<< ": eAosIILType_U64. " << name << ":" << oldvalue << ":" << newvalue;
		OmnAlarm << rdata->getErrmsg();
		returnIILPublic(iil, rdata);
		return false;
	}

	AosIILU64 *u64iil = (AosIILU64 *)iil.getPtr();
	bool rslt = u64iil->modifyDocSafe(oldvalue,
		newvalue, docid, value_unique, docid_unique,false, rdata);
	returnIILPublic(iil,rdata);
	if (mCheckRef) aos_assert_r(AosIIL::staticNumIILRefs() == 0, false);
	return rslt;
}


bool
AosIILMgr::modifyU64ValueDoc(
		const OmnString &name, 
		const u64 &oldvalue, 
		const u64 &newvalue, 
		const bool value_unique,
		const bool docid_unique,
		const OmnString &docid,
		const AosRundataPtr &rdata)
{
	u64 did = atoll(docid.data());
	return modifyU64ValueDoc(name, oldvalue, newvalue, value_unique, docid_unique, did, rdata);
}


bool		
AosIILMgr::setValueDocUnique(
		const OmnString &iilname, 
		const OmnString &key,
		const u64 &docid, 
		const bool must_same,
		const AosRundataPtr &rdata)
{
	if(iilname.length() <= 0 || iilname.length() >= AosIILUtil::eMaxStrValueLen)
	{
		OmnScreen << "length too long when setValueDocUnique : " << iilname << ":" << key << ":" << docid << endl;
		return false;
	}
	
	AosIILPtr iil = retrieveIIL(iilname.data(), iilname.length(), eAosIILType_Str, true, rdata);
	aos_assert_r(iil, false);
	
	AosIILStrPtr iilstr = (AosIILStr*)iil.getPtr();
	bool rslt = iilstr->setValueDocUnique(key, docid, must_same, rdata);
	returnIILPublic(iil, rdata);
	if (!rslt)
	{
		rdata->setError() << "Failed setting doc value: " << iilname << ":" << key << ":" << docid;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	rdata->setOk();
	return rslt;
}


bool		
AosIILMgr::setU64ValueDocUnique(
		const OmnString &iilname, 
		const u64 &key,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	if(iilname.length() <= 0 || iilname.length() >= AosIILUtil::eMaxStrValueLen)
	{
		OmnScreen << "length too long when setValueDocUnique : " << iilname << ":" << key << ":" << docid << endl;
		return false;
	}

	//1. get the iil
	AosIILPtr iil = retrieveIIL(iilname.data(), iilname.length(), eAosIILType_U64, true, rdata);
	aos_assert_r(iil, false);
	
	AosIILU64Ptr iilu64 = (AosIILU64*)iil.getPtr();
	bool rslt = iilu64->setValueDocUnique(key, docid, rdata);
	returnIILPublic(iil, rdata);
	aos_assert_r(rslt, false);
	return rslt;
}


bool	
AosIILMgr::updateKeyedValue(
		const OmnString &iilname, 
		const u64 &key,
		const bool &flag,
		const u64 &delta, 
		u64 &new_value,
		const AosRundataPtr &rdata)
{
	if(iilname.length() <= 0 || iilname.length() >= AosIILUtil::eMaxStrValueLen)
	{
		OmnScreen << "iilname too long in func updateKeyedValue: " << iilname << endl;
		return false;
	}

	//1. get the iil
	AosIILPtr iil = retrieveIIL(iilname.data(), iilname.length(), eAosIILType_U64, true, rdata);
	aos_assert_r(iil, false);
	AosIILU64Ptr iilu64 = (AosIILU64 *)iil.getPtr();

	bool rslt = iilu64->updateKeyedValue(key, flag, delta, new_value, rdata);	
	returnIILPublic(iil, rdata);
	aos_assert_r(rslt, false);
	return true;
}


bool 
AosIILMgr::updateKeyedValue(
		const OmnString &iilname,
		const bool createiil,
		const u64 &key,
		const u64 &value,
		const AosRundataPtr &rdata)
{
	if(iilname.length() <= 0 || iilname.length() >= AosIILUtil::eMaxStrValueLen)
	{
		OmnScreen << "iilname too long in func updateKeyedValue: " << iilname << endl;
		return false;
	}

	AosIILPtr iil = retrieveIIL(iilname, iilname.length(), eAosIILType_U64, createiil, rdata);
	aos_assert_r(iil, false);

	AosIILU64Ptr iilu64 = (AosIILU64 *)iil.getPtr();
	iilu64->updateKeyedValue(key, value, rdata);
	returnIILPublic(iil, rdata);
	return true;	
}


bool
AosIILMgr::appendManualOrder(
		const OmnString &iilname,
		const u64 &docid,
		u64 &value,
		const AosRundataPtr &rdata)
{
	if(iilname.length() <= 0 || iilname.length() >= AosIILUtil::eMaxStrValueLen)
	{
		OmnScreen << "length too long when incrementDocidSafe : " << iilname << ":" << value << ":" << docid << endl;
		return false;
	}

	AosIILPtr iil = retrieveIIL(iilname.data(), iilname.length(), eAosIILType_U64, true, rdata);
	aos_assert_r(iil, false);
	if (iil->getIILType() != eAosIILType_U64)
	{
		rdata->setError() << "IIL type mismatch: " << iil->getIILType() << ":eAosIILType_U64";
		OmnAlarm << rdata->getErrmsg() << enderr;
		returnIILPublic(iil, rdata);
		return false;
	}

	AosIILU64 *treeiil = (AosIILU64 *)iil.getPtr();
	bool rslt = treeiil->appendManualOrder(docid, value, rdata);
	returnIILPublic(iil, rdata);
	return rslt;
}


bool 
AosIILMgr::moveManualOrder(
		const OmnString &iilname,
		u64 &value1,
		const u64 &docid1, 
		u64 &value2, 
		const u64 &docid2,
		const OmnString flag,
		const AosRundataPtr &rdata)
{
	if(iilname.length() <= 0 || iilname.length() >= AosIILUtil::eMaxStrValueLen)
	{
		OmnScreen << "length too long when incrementDocidSafe : " << iilname << endl;
		return false;
	}

	AosIILPtr iil = retrieveIIL(iilname.data(), iilname.length(), eAosIILType_U64, true, rdata);
	aos_assert_r(iil, false);
	if (iil->getIILType() != eAosIILType_U64)
	{
		OmnAlarm << "IIL type mismatch: " << iil->getIILType() << ":eAosIILType_U64" << enderr;
		returnIILPublic(iil, rdata);
		return false;
	}

	AosIILU64 *treeiil = (AosIILU64 *)iil.getPtr();
	bool rslt = treeiil->moveManualOrder(value1, docid1, value2, docid2, flag, rdata);
	returnIILPublic(iil, rdata);
	return rslt;
}


bool
AosIILMgr::swapManualOrder(
		const OmnString &iilname,
		const u64 &value1, 
		const u64 &docid1,
		const u64 &value2,
		const u64 &docid2,
		const AosRundataPtr &rdata)
{
	if(iilname.length() <= 0 || iilname.length() >= AosIILUtil::eMaxStrValueLen)
	{
		OmnScreen << "length too long when incrementDocidSafe : " << iilname << ":" << value1 
			<< ":" << value2 << ":" << docid1 << ":" << docid2 << endl;
		return false;
	}

	AosIILPtr iil = retrieveIIL(iilname.data(), iilname.length(), eAosIILType_U64, true, rdata);
	aos_assert_r(iil, false);
	if (iil->getIILType() != eAosIILType_U64)
	{
		OmnAlarm << "IIL type mismatch: " << iil->getIILType() 
			<< ":eAosIILType_U64" << enderr;
		returnIILPublic(iil, rdata);
		return false;
	}

	AosIILU64 *treeiil = (AosIILU64 *)iil.getPtr();
	bool rslt = treeiil->swapManualOrder(value1, docid1, value2, docid2, rdata);
	returnIILPublic(iil, rdata);
	return rslt;
}


bool
AosIILMgr::removeManualOrder(
		const OmnString &iilname, 
		const u64 &value, 
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	if(iilname.length() <= 0 || iilname.length() >= AosIILUtil::eMaxStrValueLen)
	{
		OmnScreen << "length too long when incrementDocidSafe : " << iilname << ":" << value << ":" << docid << endl;
		return false;
	}

	AosIILPtr iil = retrieveIIL(iilname.data(), iilname.length(), eAosIILType_U64, true, rdata);
	aos_assert_r(iil, false);

	AosIILU64 *treeiil = (AosIILU64 *)iil.getPtr();
	bool rslt = treeiil->removeManualOrder(value, docid, rdata);
	returnIILPublic(iil, rdata);
	return rslt;
}


bool
AosIILMgr::getDocid(
		const OmnString &iilname,
		const OmnString &key,
		u64 &docid,
		bool &isunique,
		const bool removeflag,
		const AosRundataPtr &rdata)
{
	// This function retrieves the docid from the IIL 'iilname', 
	// which is a keyed IIL. The entry is identified by 'key'. 
	// If not found, which may or may not be an error, AOS_INVDID 
	// is returned. If multiple entries
	// are found, 'duplicated' is set to true and the first docid
	// is rturned. Otherwise, the found docid is unique. It
	// returns the docid.
	if(iilname.length() <= 0 || iilname.length() >= AosIILUtil::eMaxStrValueLen)
	{
		OmnScreen << "length too long when getDocid : " << iilname << ":" << docid << endl;
		return false;
	}

	docid = AOS_INVDID;
	if (key == "")
	{
		rdata->setError() << "Key is empty";
		isunique = false;
		return false;
	}

	if (mCheckRef) aos_assert_r(AosIIL::staticNumIILRefs() == 0, false);

	// Retrieve the IIL
	AosIILPtr iil = retrieveIIL(iilname.data(), iilname.length(), eAosIILType_Str, false, rdata);
	if (!iil)
	{
		// No IIL is found. 
		rdata->setError() << AOSERR_NOENTRY << ":" << key;
		isunique = false;
		return false;
	}

	if (iil->getIILType() != eAosIILType_Str)
	{
		rdata->setError() << "IILType mismatch: " << iil->getIILType() << ": eAosIILType_Str";
		OmnAlarm << rdata->getErrmsg() << enderr;
		returnIILPublic(iil, rdata);
		isunique = false;
		return false;
	}
	AosIILStrPtr striil = (AosIILStr*)iil.getPtr();
	
	// Check whether the doc is in the IIL (identified by 'key')
	int idx = -10;
	int iilidx = -10;
	bool rslt = striil->nextDocidSafe(idx, iilidx, false, eAosOpr_eq, key, docid, isunique, rdata); 
	if (removeflag && docid != AOS_INVDID)
	{
		// Found one and need to remove it
		striil->removeDocSafe(key, docid, rdata);
	}

	returnIILPublic(iil, rdata);
	if (mCheckRef) aos_assert_r(AosIIL::staticNumIILRefs() == 0, false);
	rdata->setOk();
	return rslt;
}


bool
AosIILMgr::getDocid(
		const OmnString &tagname,
		const OmnString &attrname,
		const OmnString &key,
		u64 &docid,
		bool &isunique,
		const bool removeflag,
		const AosRundataPtr &rdata)
{
	aos_assert_r(key != "", false);
	
	OmnString iilname = tagname;
	iilname << attrname;
	return getDocid(iilname, key, docid, isunique, removeflag, rdata);
}


bool
AosIILMgr::getDocidByObjid(
		const OmnString &siteid,
		const OmnString &objid,
		u64 &docid,
		bool &isunique,
		const bool removeflag,
		const AosRundataPtr &rdata)
{
	OmnString iilname = AosIILName::composeObjidListingName(siteid);
	return getDocid(iilname, objid, docid, isunique, removeflag, rdata);
}


bool
AosIILMgr::getDocid(
		const OmnString &iilname, 
		const u64 &key, 
		u64 &docid, 
		const u64 &dft, 
		bool &found,
		bool &is_unique,
		const AosRundataPtr &rdata)
{
	// This function retrieves the docid from the IIL 'iilname', 
	// The entry is identified by 'key'.  If not found, it returns false 'dft'.
	// is returned. If multiple entries are found, 'is_unique' is set to false. 
	if(iilname.length() <= 0 || iilname.length() >= AosIILUtil::eMaxStrValueLen)
	{
		OmnScreen << "length too long when getDocid : " << iilname << ":" << docid << endl;
		return false;
	}

	docid = dft;
	// Retrieve the IIL
	AosIILPtr iil = retrieveIIL(iilname.data(), iilname.length(), eAosIILType_U64, false, rdata);
	aos_assert_r(iil, false);
	AosIILU64Ptr u64iil = (AosIILU64*)iil.getPtr();
	
	// Check whether the doc is in the IIL (identified by 'key')
	int idx = -10;
	int iilidx = -10;
	u64 kkk = key;
	bool rslt = u64iil->nextDocidSafe(idx, iilidx, false, eAosOpr_eq, kkk, docid, is_unique, rdata); 
	returnIILPublic(iil, rdata);
	if (idx < 0)
	{
		// Did not find it. 
		found = false;
		docid = dft;
	}
	else
	{
		found = true;
	}
	aos_assert_r(rslt, false);
	return true;
}


bool
AosIILMgr::getDocid(
		const OmnString &iilname, 
		const OmnString &key, 
		u64 &docid, 
		const u64 &dft, 
		bool &found,
		bool &is_unique,
		const AosRundataPtr &rdata)
{
	// This function retrieves the docid from the IIL 'iilname', 
	// The entry is identified by 'key'.  If not found, it returns false 'dft'.
	// is returned. If multiple entries are found, 'is_unique' is set to false. 
	if(iilname.length() <= 0 || iilname.length() >= AosIILUtil::eMaxStrValueLen)
	{
		OmnScreen << "length too long when getDocid : " << iilname << ":" << docid << endl;
		return false;
	}

	docid = dft;
	// Retrieve the IIL
	AosIILPtr iil = retrieveIIL(iilname.data(), iilname.length(), eAosIILType_Str, false, rdata);
	aos_assert_r(iil, false);
	AosIILStrPtr striil = (AosIILStr*)iil.getPtr();
	
	// Check whether the doc is in the IIL (identified by 'key')
	int idx = -10;
	int iilidx = -10;
	OmnString kkk = key;
	bool rslt = striil->nextDocidSafe(idx, iilidx, false, eAosOpr_eq, kkk, docid, is_unique, rdata); 
	returnIILPublic(iil, rdata);
	if (idx < 0)
	{
		// Did not find it. 
		found = false;
		docid = dft;
	}
	else
	{
		found = true;
	}
	aos_assert_r(rslt, false);
	return true;
}


bool	
AosIILMgr::incrementDocid(
		const OmnString &iilname,
		const OmnString &key, 
		u64 &incValue,
		const AosRundataPtr &rdata)
{
	AosIILPtr iil = retrieveIIL(iilname, strlen(iilname), eAosIILType_Str, true, rdata);
	aos_assert_r(iil, false);

	AosIILStrPtr iilstr = (AosIILStr*)iil.getPtr();
	bool rslt = iilstr->incrementDocidSafe1(key, incValue, rdata);
	returnIILPublic(iil, rdata);
	return rslt;
}


bool
AosIILMgr::incrementDocid(
		const OmnString &iilname,
		const u64 &key, 
		const u64 &incValue, 
		const u64 &initValue,
		u64 &new_value,
		const AosRundataPtr &rdata) 
{
	if(iilname.length() <= 0 || iilname.length() >= AosIILUtil::eMaxStrValueLen)
	{
		OmnScreen << "length too long when incrementDocidSafe : " << iilname << ":" << key << endl;
		return false;
	}

	// This function increments the counter by 'value'. 
	AosIILPtr iil = retrieveIIL(iilname.data(), iilname.length(), eAosIILType_U64, true, rdata);
	aos_assert_r(iil, false);

	AosIILU64 *u64iil = (AosIILU64 *)iil.getPtr();
	bool rslt = u64iil->incrementDocid(key, incValue, initValue, new_value, rdata);
	returnIILPublic(iil, rdata);
	return rslt;
}


bool
AosIILMgr::bindCloudid(
		const OmnString &cloudid, 
		const u64 &docid, 
		const AosRundataPtr &rdata)
{
	// This function binds 'cloudid' with 'docid'. If the cloudid is already
	// bound to another docid, it is an error.
	aos_assert_rr(cloudid != "", rdata, false);
	OmnString siteid = rdata->getSiteid();
	aos_assert_rr(siteid != "", rdata, false);
	OmnString iilname = AosIILName::composeCloudidListingName(siteid);
	aos_assert_rr(iilname != "", rdata, false);

	AosIILPtr iil = retrieveIIL(iilname.data(), iilname.length(), eAosIILType_Str, true, rdata);
	aos_assert_rr(iil, rdata, false);
	AosIILStrPtr striil = (AosIILStr*)iil.getPtr();

	bool added = striil->addDocSafe(cloudid, docid, true, true, false, rdata);
	if (added)
	{
		// The objid is added to the iil. 
		returnIILPublic(iil, rdata);
		rdata->setOk();
		return true;
	}

	// This fails the operation.
	returnIILPublic(iil, rdata);
	rdata->setError() << "Cloudid is used by someone else: " << cloudid;
	return false;
}


bool 
AosIILMgr::bindObjid(
		OmnString &objid, 
		const u64 &docid,
		bool &objid_changed,
		const bool resolve,
		const AosRundataPtr &rdata)
{
	// This function binds 'objid' with 'docid'. If the objid is used by someone else, 
	// 'resolve' determines whether to resolve the objid or not.
	aos_assert_rr(objid != "", rdata, false);
	OmnString siteid = rdata->getSiteid();
	aos_assert_rr(siteid != "", rdata, false);
	OmnString iilname = AosIILName::composeObjidListingName(siteid);
	aos_assert_rr(iilname != "", rdata, false);

	objid_changed = false;
	AosIILPtr iil = retrieveIIL(iilname.data(), iilname.length(), eAosIILType_Str, true, rdata);
	aos_assert_rr(iil, rdata, false);
	bool rslt = bindObjidPriv(iil, objid, docid, objid_changed, resolve, rdata);
	returnIILPublic(iil, rdata);
	return rslt;
}


bool
AosIILMgr::queryU64ValueSafe(
		const OmnString &iilname,
		vector<u64> &values,
		const AosOpr opr,
		const u64 &value,
		const bool unique_value,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilname!="", false);

	AosIILPtr iil = retrieveIIL(iilname.data(), iilname.length(), eAosIILType_U64, true, rdata);
	aos_assert_r(iil, false);

	AosIILU64 *u64iil = (AosIILU64 *)iil.getPtr();
	bool rslt = u64iil->queryValueSafe(values, opr, value, unique_value, rdata);
	returnIILPublic(iil, rdata);
	return rslt;
}


u64
AosIILMgr::getCtnrMemDocid(
		const OmnString &siteid, 
		const OmnString &ctnr_objid, 
		const OmnString &keyname,
		const OmnString &keyvalue,
		bool &duplicated, 
		const AosRundataPtr &rdata)
{
	// This function retrieves the XML document for the object that
	// is in the container 'ctnr_objid', where the container is on
	// the site 'siteid'. 

	// 1. Retrieve the container docid
	if (siteid == "")
	{
		rdata->setError() << "SiteId is missing!";
		return AOS_INVDID;
	}

	if (ctnr_objid == "")
	{
		rdata->setError() << "No container specified!";
		return AOS_INVDID;
	}

	u64 docid = AOS_INVDID;	
	bool isunique;
	bool rslt = getDocidByObjid(siteid, ctnr_objid, docid, isunique, false, rdata);
	if (!rslt || docid == AOS_INVDID)
	{
		rdata->setError() << "Invalid container: " << ctnr_objid << ". SiteId: " << siteid;
		return AOS_INVDID;
	}

	if (!isunique)
	{
		rdata->setError() << "The container is not unique: " << ctnr_objid;
		return AOS_INVDID;
	}

	// 2. Retrieve the container IIL:
	// 		[AOSZTG_CONTAINER + siteid + ":" + container_objid]
	// OmnString ctnr_iilname = AOSZTG_CONTAINER;
	// ctnr_iilname << siteid << ":" << ctnr_objid;
	OmnString ctnr_iilname = AosIILName::composeContainerObjidListName(siteid, ctnr_objid);
	AosIILPtr ctnr_iil = getIILPublic(ctnr_iilname, rdata);
	if (!ctnr_iil)
	{
		rdata->setError() << "Failed to retrieve the container: " << siteid << ":" << ctnr_objid;
		return AOS_INVDID;
	}

	// 3. Retrieve the IIL:
	OmnString iilname = AosIILName::composeAttrIILName(keyname);
	AosIILPtr iil = AosIILMgrSelf->getIILPublic(iilname, rdata);
	if (!iil)
	{
		rdata->setError() << "Object not found (001): " << keyname << " = " << keyvalue;
		returnIILPublic(ctnr_iil, rdata);
		return AOS_INVDID;
	}

	// 4. The IIL must be a string IIL
	if (iil->getIILType() != eAosIILType_Str)
	{
		rdata->setError() << "Object not found (002): " << keyname << " = " << keyvalue;
		returnIILPublic(ctnr_iil, rdata);
		returnIILPublic(iil, rdata);
		return AOS_INVDID;
	}

	AosIILStrPtr striil = (AosIILStr*)iil.getPtr();

	int idx = -10;
	int iilidx = -10;
	u64 did;
	bool found = false;
	while (striil->nextDocidSafe(idx, iilidx, false, eAosOpr_eq, keyvalue, did, isunique, rdata) &&
		   did != AOS_INVDID && idx >= 0)
	{
		if(idx == -5)break;
		if (ctnr_iil->docExistSafe(did, rdata))	
		{
			found = true;
			break;
		}
	}

	if (!found)
	{
		rdata->setError() << "Object not found (003). SiteId: "
			<< siteid << "; Container: " << ctnr_objid
			<< "; Key Name: " << keyname 
			<< "; Key Value: " << keyvalue;
		returnIILPublic(ctnr_iil, rdata);
		returnIILPublic(iil, rdata);
		return AOS_INVDID;
	}
		
	returnIILPublic(ctnr_iil, rdata);
	returnIILPublic(iil, rdata);
	return did;
}


u64
AosIILMgr::getCtnrMemDocid(
		const OmnString &siteid,
		const OmnString &ctnr_objid,
		const OmnString &keyname1,
		const OmnString &keyvalue1,
		const OmnString &keyname2,
		const OmnString &keyvalue2,
		bool &duplicated,
		const AosRundataPtr &rdata)
{
	// This function retrieves the XML document for the object that
	// is in the container 'ctnr_objid', where the container is on
	// the site 'siteid'.

	// 1. Retrieve the container docid
	if (siteid == "")
	{
		rdata->setError() << "SiteId is missing!";
		return AOS_INVDID;
	}

	if (ctnr_objid == "")
	{
		rdata->setError() << "No container specified!";
		return AOS_INVDID;
	}

	u64 docid = AOS_INVDID;
	bool isunique;
	bool rslt = getDocidByObjid(siteid, ctnr_objid, docid, isunique, false, rdata);
	if (!rslt || docid == AOS_INVDID)
	{
		rdata->setError() << "Invalid container: " << ctnr_objid << ". SiteId: " << siteid;
		return AOS_INVDID;
	}

	if (!isunique)
	{
		rdata->setError() << "The container is not unique: " << ctnr_objid;
		return AOS_INVDID;
	}

	// 2. Retrieve the container IIL:
	// 		[AOSZTG_CONTAINER + siteid + ":" + container_objid]
	// OmnString ctnr_iilname = AOSZTG_CONTAINER;
	// ctnr_iilname << siteid << ":" << ctnr_objid;
	OmnString ctnr_iilname = AosIILName::composeContainerObjidListName(siteid, ctnr_objid);
	AosIILPtr ctnr_iil = getIILPublic(ctnr_iilname, rdata);
	if (!ctnr_iil)
	{
		rdata->setError() << "Failed to retrieve the container: " << siteid << ":" << ctnr_objid;
		return AOS_INVDID;
	}

	// 3. Retrieve the IIL:
	OmnString iilname1 = AosIILName::composeAttrIILName(keyname1);
	AosIILPtr iil = AosIILMgrSelf->getIILPublic(iilname1, rdata);
	if (!iil)
	{
		rdata->setError() << "Object not found (001): " << keyname1 << " = " << keyvalue1;
		returnIILPublic(ctnr_iil, rdata);
		return AOS_INVDID;
	}

	// 4. The IIL must be a string IIL
	if (iil->getIILType() != eAosIILType_Str)
	{
		rdata->setError() << "Object not found (002): " << keyname1 << " = " << keyvalue1;
		returnIILPublic(ctnr_iil, rdata);
		returnIILPublic(iil, rdata);
		return AOS_INVDID;
	}

	AosIILStrPtr striil = (AosIILStr*)iil.getPtr();

	// 4. Retrieve the third IIL:
	OmnString iilname2 = AosIILName::composeAttrIILName(keyname2);
	AosIILPtr iil2 = AosIILMgrSelf->getIILPublic(iilname2, rdata);
	if (!iil2)
	{
		rdata->setError() << "Object not found (001): " << keyname2 << " = " << keyvalue2;
		returnIILPublic(ctnr_iil, rdata);
		returnIILPublic(iil, rdata);
		return AOS_INVDID;
	}

	// 4. The IIL must be a string IIL
	if (iil2->getIILType() != eAosIILType_Str)
	{
		rdata->setError() << "Object not found (002): " << keyname2 << " = " << keyvalue2;
		returnIILPublic(ctnr_iil, rdata);
		returnIILPublic(iil, rdata);
		returnIILPublic(iil2, rdata);
		return AOS_INVDID;
	}

	AosIILStrPtr striil2 = (AosIILStr*)iil2.getPtr();

	int idx = -10;
	int iilidx = -10;
	u64 did;
	bool found = false;
	while (striil->nextDocidSafe(idx, iilidx, false, eAosOpr_eq,
			keyvalue1, did, isunique, rdata) &&
		   did != AOS_INVDID && idx >= 0)
	{
		if (ctnr_iil->docExistSafe(did, rdata) &&
			striil2->checkDocSafe(eAosOpr_eq, keyvalue2, did, rdata))
		{
			found = true;
			break;
		}
	}

	if (!found)
	{
		rdata->setError() << "Object not found (003). SiteId: "
			<< siteid << "; Container: " << ctnr_objid
			<< "; Key Name1: " << keyname1
			<< "; Key Value1: " << keyvalue1
			<< "; Key Name2: " << keyname2
			<< "; Key Value2: " << keyvalue2;
		returnIILPublic(ctnr_iil, rdata);
		returnIILPublic(iil, rdata);
		returnIILPublic(iil2, rdata);
		return AOS_INVDID;
	}

	returnIILPublic(ctnr_iil, rdata);
	returnIILPublic(iil, rdata);
	returnIILPublic(iil2, rdata);
	return did;
}
*/



/*
AosIILPtr
AosIILMgr::getIILPublic(
		const OmnString &word,
		const bool crt_flag,
		const AosIILType iiltype,
		const AosRundataPtr &rdata)

{
	// IMPORTANT: It is extremely important that the caller
	// will NEVER fail unlocking 'iil', without causing
	// locking other classes. 
	AosIILPtr iil = retrieveIIL(word.data(), word.length(), iiltype, crt_flag, rdata);
	return iil;
}


AosIILPtr
AosIILMgr::getIILPublic(
		const OmnString &word,
		const AosRundataPtr &rdata)

{
	// IMPORTANT: It is extremely important that the caller
	// will NEVER fail unlocking 'iil', without causing
	// locking other classes. 
	return getIILPublic(word, false, eAosIILType_Invalid, rdata);
}
*/


/*
bool
AosIILMgr::getDocidByPrefix(
		const OmnString &iilname,
		int &idx,
		int &iilidx,
		const bool reverse,
		const OmnString &prefix,
		u64 &docid,
		const AosRundataPtr &rdata)
{
	docid = AOS_INVDID;
	aos_assert_r(prefix != "", false);

	if (mCheckRef) aos_assert_r(AosIIL::staticNumIILRefs() == 0, false);
	// Retrieve the IIL
	AosIILPtr iil = retrieveIIL(iilname.data(), iilname.length(), eAosIILType_Str, false, rdata);
	if (!iil)
	{
		// No IIL is found.
		return false;
	}

	if (iil->getIILType() != eAosIILType_Str)
	{
		rdata->setError() << "Internal error!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		returnIILPublic(iil, rdata);
		return false;
	}
	AosIILStrPtr striil = (AosIILStr*)iil.getPtr();
	// Check whether the doc is in the IIL (identified by 'key')
	bool rslt = striil->nextDocidPrefixSafe(idx, iilidx, reverse, prefix, docid, rdata);

	returnIILPublic(iil, rdata);
	if (mCheckRef) aos_assert_r(AosIIL::staticNumIILRefs() == 0, false);
	return rslt;
}


bool
AosIILMgr::getDocidByPrefix(
		const u64 &iilid,
		int &idx,
		int &iilidx,
		const bool reverse,
		const OmnString &prefix,
		u64 &docid,
		const AosRundataPtr &rdata)
{
	docid = AOS_INVDID;
	aos_assert_r(prefix != "", false);
	
	AosIILType type = eAosIILType_Str;
	AosIILPtr iil = loadIILByIDPublic(iilid, type, rdata);
	aos_assert_r(iil, false);
	if (iil->getIILType() != eAosIILType_Str)
	{
		returnIILPublic(iil, rdata);
		return false;
	}

	AosIILStrPtr striil = (AosIILStr*)iil.getPtr();
	bool rslt = striil->nextDocidPrefixSafe(idx, iilidx, reverse, prefix, docid, rdata);
	returnIILPublic(iil, rdata);
	if (mCheckRef) aos_assert_r(AosIIL::staticNumIILRefs() == 0, false);
	return rslt;
}


bool
AosIILMgr::removeDocByPrefix(
		const OmnString &iilname,
		const OmnString &prefix,
		const AosRundataPtr &rdata)
{
	if(iilname.length() <= 0 || iilname.length() >= AosIILUtil::eMaxStrValueLen)
	{
		OmnScreen << "length too long when incrementDocidSafe : " << iilname << endl;
		return false;
	}

	aos_assert_r(prefix != "", false);
	if (mCheckRef) aos_assert_r(AosIIL::staticNumIILRefs() == 0, false);
	// Retrieve the IIL
	AosIILPtr iil = retrieveIIL(iilname.data(), iilname.length(), eAosIILType_Str, false, rdata);
	if (!iil)
	{
		// No IIL is found.
		return false;
	}

	if (iil->getIILType() != eAosIILType_Str)
	{
		OmnAlarm << "Internal error!" << enderr;
		returnIILPublic(iil, rdata);
		return false;
	}
	AosIILStrPtr striil = (AosIILStr*)iil.getPtr();
	// Check whether the doc is in the IIL (identified by 'key')
	bool rslt = striil->removeDocByPrefixSafe(prefix, rdata);

	returnIILPublic(iil, rdata);
	if (mCheckRef) aos_assert_r(AosIIL::staticNumIILRefs() == 0, false);
	return rslt;
}


bool
AosIILMgr::removeDocByPrefix(
		const u64 &iilid,
		const OmnString &prefix,
		const AosRundataPtr &rdata)
{
	aos_assert_r(prefix != "", false);
	if (mCheckRef) aos_assert_r(AosIIL::staticNumIILRefs() == 0, false);
	
	AosIILType type = eAosIILType_Str;
	AosIILPtr iil = loadIILByIDPublic(iilid, type, rdata);
	aos_assert_r(iil, false);
	if (iil->getIILType() != eAosIILType_Str)
	{
		returnIILPublic(iil, rdata);
		return false;
	}
	
	AosIILStrPtr striil = (AosIILStr*)iil.getPtr();
	// Check whether the doc is in the IIL (identified by 'key')
	bool rslt = striil->removeDocByPrefixSafe(prefix, rdata);

	returnIILPublic(iil, rdata);
	if (mCheckRef) aos_assert_r(AosIIL::staticNumIILRefs() == 0, false);
	return rslt;
}


bool
AosIILMgr::getDocidByValue(
		int &idx,
		int &iilidx,
		const bool reverse,
		const OmnString &iilname,
		const OmnString &value,
		u64 &docid,
		const AosRundataPtr &rdata)
{
	docid = AOS_INVDID;
	aos_assert_r(value != "", false);
	if (mCheckRef) aos_assert_r(AosIIL::staticNumIILRefs() == 0, false);
	// Retrieve the IIL
	AosIILPtr iil = retrieveIIL(iilname.data(), iilname.length(), eAosIILType_Str, false, rdata);
	if (!iil)
	{
		// No IIL is found.
		return false;
	}

	if (iil->getIILType() != eAosIILType_Str)
	{
		OmnAlarm << "Internal error!" << enderr;
		returnIILPublic(iil, rdata);
		return false;
	}
	AosIILStrPtr striil = (AosIILStr*)iil.getPtr();

	// Check whether the doc is in the IIL (identified by 'key')
	bool unique = false;
	bool rslt = striil->nextDocidSafe(idx, iilidx, reverse, eAosOpr_eq, value, docid, unique, rdata);
	returnIILPublic(iil, rdata);
	if (mCheckRef) aos_assert_r(AosIIL::staticNumIILRefs() == 0, false);
	return rslt;
}


bool
AosIILMgr::getDocidByValue(
		const u64 &iilid,
		int &idx,
		int &iilidx,
		const bool reverse,
		const OmnString &value,
		u64 &docid,
		const AosRundataPtr &rdata)
{
	docid = AOS_INVDID;
	
	AosIILType type = eAosIILType_Str;
	AosIILPtr iil = loadIILByIDPublic(iilid, type, rdata);
	aos_assert_r(iil, false);
	if (iil->getIILType() != eAosIILType_Str)
	{
		returnIILPublic(iil, rdata);
		return false;
	}
	AosIILStrPtr striil = (AosIILStr*)iil.getPtr();
	bool unique = false;
	bool rslt = striil->nextDocidSafe(idx, iilidx, reverse, eAosOpr_eq, value, docid, unique, rdata);
	returnIILPublic(iil, rdata);
	if (mCheckRef) aos_assert_r(AosIIL::staticNumIILRefs() == 0, false);
	return rslt;
}


bool        
AosIILMgr::addValueDoc(
		 u64 &iilid,
		 const OmnString &value,
		 const u64 &docid, 
		 const bool value_unique, 
		 const bool docid_unique, 
		 const AosRundataPtr &rdata)
{
	if(!(value.length() > 0 && value.length() < AosIILUtil::eMaxStrValueLen))
	{
		OmnString value2;
		if(value.length() > 300)
		{
			value2 = value.subString(0,300);
		}else
		{
			value2 = value;
		}
		OmnScreen << "The value is too long. Len: " << value.length() << 
			" , Word:" << value2 << " , docid:" << docid << endl;
		return false;
	}

	AosIILPtr iil;
	if (iilid == 0)
	{
		iil = createIILPublic(eAosIILType_Str);
		aos_assert_rr(iil, rdata, false);
		iilid = iil->getIILID();
	}
	else
	{
		AosIILType type = eAosIILType_Str;
		iil = loadIILByIDPublic(iilid, type);
		aos_assert_rr(iil, rdata, false);
	}
	if (iil->getIILType() != eAosIILType_Str)
	{
		rdata->setError() << "Retrieved is not strIIL: " 
			<< iil->getIILType() 
			<< ":" << value << ":" << docid;
		OmnAlarm << rdata->getErrmsg() << enderr;
		returnIILPublic(iil);
		return false;
	}
	AosIILStr *striil = (AosIILStr *)iil.getPtr();
	bool rslt = striil->addDocSafe(
			value, docid, value_unique, docid_unique, false);
	if (mCheckRef) aos_assert_r(AosIIL::staticNumIILRefs() == 0, false);
	returnIILPublic(iil);

	if (!rslt)
	{
		rdata->setError() << "Failed adding the value: " 
			<< value << ":" << docid;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	rdata->setOk();
	return true;
}


bool        
AosIILMgr::addValueDoc(
		 u64 &iilid,
		 const u64 &value,
		 const u64 &docid, 
		 const bool value_unique,
		 const bool docid_unique, 
		 const AosRundataPtr &rdata)
{
	if (mCheckRef) aos_assert_r(AosIIL::staticNumIILRefs() == 0, false);
	AosIILPtr iil;
	if (iilid == 0)
	{
		iil = createIILPublic(eAosIILType_U64);
		aos_assert_rr(iil, rdata, false);
		iilid = iil->getIILID();
	}
	else
	{
		AosIILType type = eAosIILType_U64;
		iil = loadIILByIDPublic(iilid, type);
		aos_assert_rr(iil, rdata, false);
	}

	if (iil->getIILType() != eAosIILType_U64)
	{
		rdata->setError() << "IIL type mismatch: " << iil->getIILType()
			<< ": eAosIILType_U64";
		OmnAlarm << rdata->getErrmsg() << enderr;
		returnIILPublic(iil);
		return false;
	}

	AosIILU64 *u64iil = (AosIILU64 *)iil.getPtr();

	bool rslt = u64iil->addDocSafe(value, docid, value_unique, docid_unique,false);
	if (mCheckRef) aos_assert_r(AosIIL::staticNumIILRefs() == 0, false);
	returnIILPublic(iil);
	if (!rslt)
	{
		rdata->setError() << "Failed adding value: " 
			<< value << ":" << docid;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	rdata->setOk();
	return true;
}

u64
AosIILMgr::getIILIdPublic(
		const OmnString &word,
		const bool crt_flag,
		const AosIILType iiltype)

{
	AosIILType type = iiltype;
	AosIILPtr iil = retrieveIIL(word.data(), word.length(), type, crt_flag);
	if(!iil)
	{
		return 0;
	}
	u64 iilid = iil->getIILID();
	returnIILPublic(iil);
	if(type != iiltype)
	{
		return 0;		
	}
	return iilid;
}


bool
AosIILMgr::getDocid(
		const u64 &iilid, 
		const OmnString &key, 
		u64 &docid, 
		const u64 &dft, 
		bool &found,
		bool &is_unique)
{
	// This function retrieves the docid from the IIL 'iild', 
	// The entry is identified by 'key'.  If not found, it returns false 'dft'.
	// is returned. If multiple entries are found, 'is_unique' is set to false. 
	docid = dft;

	// Retrieve the IIL
	AosIILType iiltype = eAosIILType_Str;
	AosIILPtr iil = loadIILByIDPublic(iilid, iiltype);
	aos_assert_r(iil, false);
	if (iil->getIILType() != eAosIILType_Str)
	{
		returnIILPublic(iil);
		OmnAlarm << "Retrieved is not a string IIL: " << iil->getIILType() << enderr;
		return 0;
	}

	AosIILStrPtr striil = (AosIILStr*)iil.getPtr();
	
	// Check whether the doc is in the IIL (identified by 'key')
	int idx = -10;
	int iilidx = -10;
	OmnString kkk = key;
	bool rslt = striil->nextDocidSafe(idx, iilidx, false, eAosOpr_eq, kkk, docid, is_unique); 
	returnIILPublic(iil);
	if (idx < 0)
	{
		// Did not find it. 
		found = false;
		docid = dft;
	}
	else
	{
		found = true;
	}
	aos_assert_r(rslt, false);
	return true;
}


AosXmlTagPtr	
AosIILMgr::getDoc(const u64 &docid) 
{
	//AosXmlDocPtr header = OmnNew AosXmlDoc();
	//bool syntaxError;
	//AosXmlTagPtr xml = header->loadXmlFromFile(docid, syntaxError); //Linda
	//AosXmlTagPtr xml = AosXmlDoc::readFromFile(docid);
	return AosDocMgr::getSelf()->getDoc(docid, "");
}


AosXmlTagPtr	
AosIILMgr::getDoc(
		const OmnString &siteid, 
		const OmnString &objid)
{
	// bool isunique;
	// u64 docid;
	// bool rslt = getDocid(siteid, objid, docid, isunique);
	// if (!rslt) return 0;
	// if (!isunique) return 0;
	// if (docid == AOS_INVDID) return 0;
	return AosDocMgr::getSelf()->getDoc(siteid, objid);
}


AosIILPtr
AosIILMgr::createSubiilDocidSafe(const bool locked)
{
	if (mShowLocking)OmnScreen << "Locking" << endl;
	if (!locked) 
	{
		AOSLMTR_ENTER(mLockMonitor);
		AOSLMTR_LOCK(mLockMonitor);
	}
	AosIILDocidPtr iil = getIILDocidPriv(true);
	u64 iilid = AosSeIdGenMgr::getSelf()->nextIILId();
	iil->setIILID(iilid);
	iil->setDirty(true);

	if (mShowLocking)OmnScreen << "Unlocking" << endl;
	if (!locked) 
	{
		AOSLMTR_UNLOCK(mLockMonitor);
		AOSLMTR_FINISH(mLockMonitor);
	}
	return iil;
}

AosIILPtr 
AosIILMgr::processIILLogs(
		const u64 &iilid, 
		const u64 &wordid,
		const AosIILLogBuffPtr &logs, 
		const OmnFilePtr &file)
{
	AosIILPtr iil = retrieveIIL(iilid, wordid);
	aos_assert_r(iil, false);
	iil->processIILLogs(logs, file);
	return iil;
}

bool 
AosIILMgr::checkMember(
		const OmnString &siteid, 
		const OmnString &ctnr_objid
		)
{
	//1.get iil
	//2.check mNumDocs is equal to 0, and isSingleIIL 
	OmnString ctnr_iilname = AosIILName::composeContainerObjidListName(siteid, ctnr_objid);
	AosIILPtr ctnr_iil = getIILPublic(ctnr_iilname);
	aos_assert_r(ctnr_iil, false);
	if(ctnr_iil->getNumDocs() == 0 && ctnr_iil->isSingleIIL())
	{
		return false;
	}
	return true;
}


// Chen Ding, 2011/02/24
bool
AosIILMgr::procIILLogs(
		const vector<AosIILLogPtr> &list,
		const OmnString &type)
{
	if(list.size() == 0) return true;
	int iilid = list[0]->mIILID;
	// we need to maintain IIL by id, then we can use it.
	const char* temp = type.data();
	aos_assert_r(type.length() > 0 ,false);

	AosIILPtr iil;
	AosIILHitPtr hitiil;
	AosIILStrPtr striil;
	AosIILU64Ptr u64iil;
	
	switch(temp[0])
	{
		case 'H':
			// get iil by id
			mLock->lock();
			iil = getIILByIDPriv(iilid);
			mLock->unlock();
			if(iil.isNull())
			{
				break;
			}
			aos_assert_r(iil->getIILType() == eAosIILType_Hit,false);
			hitiil = (AosIILHit*)iil.getPtr();
			hitiil->procIILLogs(list);
			break;

		case 'S':
			mLock->lock();
			iil = getIILByIDPriv(iilid);
			if(iil.isNull())
			{
				break;
			}
			mLock->unlock();
			aos_assert_r(iil->getIILType() == eAosIILType_Str,false);
			striil = (AosIILStr*)iil.getPtr();
			striil->procIILLogs(list);
			break;

		case 'U':
			mLock->lock();
			iil = getIILByIDPriv(iilid);
			if(iil.isNull())
			{
				break;
			}
			mLock->unlock();
			aos_assert_r(iil->getIILType() == eAosIILType_U64,false);
			u64iil = (AosIILU64*)iil.getPtr();
			u64iil->procIILLogs(list);
			break;  

		default:
			OmnAlarm << "Unrecognized Type: " << type << enderr;
			return false;
	}
	return true;
}


AosIILPtr 
AosIILMgr::getIILByIDPriv(const u64 &iilid)
{
	hash_map<u64,AosIILPtr,AosU64Hash1>::iterator itr = mIILIDHash.find(iilid);

	if(itr != mIILIDHash.end())
	{
		AosIILPtr iil = itr->second;
		aos_assert_r(iil,0);
		u32 refcount = iil->addRefCountByIILMgr();
		if(refcount == 1)
		{
			removeFromIILListPriv(iil);
		}
		return iil;
	}
	return 0;
}


bool
AosIILMgr::getLastValue(const OmnString &iilname, u64 &value)
{
	if(iilname.length() <= 0 || iilname.length() >= AosIILUtil::eMaxStrValueLen)
	{
		OmnScreen << "length too long when incrementDocidSafe : " << iilname << ":" << value 
			<< endl;
		return false;
	}

	AosIILPtr iil = retrieveIIL(iilname.data(), iilname.length(), eAosIILType_U64, true);
	aos_assert_r(iil, false);
	if (iil->getIILType() != eAosIILType_U64)
	{
		OmnAlarm << "Retrieved is not strIIL: " << iil->getIILType() 
			<< ":" << iilname << enderr;
		returnIILPublic(iil);
		return false;
	}
	AosIILU64 *u64iil = (AosIILU64 *)iil.getPtr();

	bool rslt = u64iil->getLastValue(value);
	returnIILPublic(iil);
	return rslt;
}


// Tank 05/05/2011
// Shawn 05/10/2011 Can not link seserver.exe without this function implimentation 
AosIILPtr
AosIILMgr::retrieveIIL1(const u64 &iilid, const u64 &wordid)
{
	u64 ptr = 0;
	AOSLMTR_ENTER(mLockMonitor);
	AOSLMTR_LOCK(mLockMonitor);
	AOSIILCOUNT_TRIES;
	if (mIsStopping)
	{
		AOSIILCOUNT_MISS;
		AOSLMTR_UNLOCK(mLockMonitor);
		AOSLMTR_FINISH(mLockMonitor);
		return 0;
	}
	AOSLMTR_ATTEMPT(mLockMonitor);
	if (wordid == AOS_INVWID)
	{
		AOSLMTR_UNLOCK(mLockMonitor);
		AOSLMTR_FINISH(mLockMonitor);
		AOSIILCOUNT_MISS;
		return 0;
	}

	int count = 0;
	AosIILPtr iil;
	while (count++ < eMaxRetrieveIILTries)
	{
		AOSLMTR_ATTEMPT(mLockMonitor);
		if (!AosWordClient::getSelf()->getPtr(wordid, ptr))
		{
			AOSLMTR_ATTEMPT(mLockMonitor);
			if (mShowLocking) OmnScreen << "Unlocking" << endl;
			AOSLMTR_UNLOCK(mLockMonitor);
			OmnAlarm << "Failed to get ptr: " << wordid << enderr;
			AOSLMTR_FINISH(mLockMonitor);
			return 0;
		}
		AOSLMTR_ATTEMPT(mLockMonitor);

		iil = (AosIIL *)ptr;
		if (!iil)
		{
			// The IIL has not been loaded into memory yet. 
			break;
		}

		AOSLMTR_ATTEMPT(mLockMonitor);
		u32 refcount = iil->addRefCountByIILMgr();
		AOSLMTR_ATTEMPT(mLockMonitor);

		// Check whether the IIL is still associated with 'wordid'.
		if (iil->getWordIdSafe() != wordid)
		{
			// It is no longer associated with 'wordid'. Need to 
			// return 'iil' and try it again.
			AOSLMTR_ATTEMPT(mLockMonitor);
			AOSLMTR_UNLOCK(mLockMonitor);
			returnIILPublic(iil);
			iil = 0;
			AOSLMTR_LOCK(mLockMonitor);
			continue;
		}
		AOSLMTR_ATTEMPT(mLockMonitor);

		// To ensure everything is the same as before, 
		// we will re-retrieve the ptr and check it again.
		u64 ptr1;
		if (!AosWordClient::getSelf()->getPtr(wordid, ptr1))
		{
			// Should never happen
			AOSLMTR_ATTEMPT(mLockMonitor);

			AOSLMTR_UNLOCK(mLockMonitor);
			returnIILPublic(iil);
			AOSLMTR_ATTEMPT(mLockMonitor);
			iil = 0;
			AOSLMTR_FINISH(mLockMonitor);
			return 0;
		}

		AOSLMTR_ATTEMPT(mLockMonitor);
		if (ptr != ptr1)
		{
			// Something has changed. This should normally not 
			// happen. We are not sure whether it could ever 
			// happen. But to be careful, we will return 'iil'
			// and give it another try.

			AOSLMTR_UNLOCK(mLockMonitor);
			returnIILPublic(iil);
			AOSLMTR_LOCK(mLockMonitor);
			iil = 0;
			continue;
		}

		// Looks good. Safe to proceed.
		if (refcount == 1)
		{
			// The IIL is in the idle pool. Need to remove it
			// from the idle pool.
			AOSLMTR_ATTEMPT(mLockMonitor);
			// Shawn 05/10/2011
			removeFromIILListPriv(iil);
			AOSLMTR_ATTEMPT(mLockMonitor);
		}
		
		AOSIILCOUNT_HITS;
		AOSLMTR_UNLOCK(mLockMonitor);
		AOSLMTR_FINISH(mLockMonitor);
		return iil;
	}
	
	AOSLMTR_ATTEMPT(mLockMonitor);
	aos_assert_rk(count < eMaxRetrieveIILTries, mLock, 0);

	// Retrieve the IIL type
	char bb[AosIIL::eIILHeaderMemsize+10];
	AosBuff buff(bb, AosIIL::eIILHeaderMemsize, 0);
	if (!AosIIL::staticReadHeaderBuff(iilid, buff))
	{
		AOSLMTR_UNLOCK(mLockMonitor);
		AOSLMTR_FINISH(mLockMonitor);
		OmnAlarm << "Failed to read buff!" << enderr;
		return 0;
	}

	AosIILType itype;
	if (!AosIILType_toCode(itype, buff.getChar(eAosIILType_Invalid)))
	{
		OmnAlarm << "Incorrect iiltype: " << itype << enderr;
		AOSLMTR_UNLOCK(mLockMonitor);
		AOSLMTR_FINISH(mLockMonitor);
		return 0;
	}

	if (itype == eAosIILType_Invalid)
	{
		OmnAlarm << "Invalid IIL type!" << enderr;
		AOSLMTR_UNLOCK(mLockMonitor);
		AOSLMTR_FINISH(mLockMonitor);
		return 0;
	}

	// Not loaded in memory yet. Load it.
	bool status;
	iil = loadIILByIDPriv1(iilid, wordid, itype, 0, 0, true, status);
	AOSLMTR_ATTEMPT(mLockMonitor);
	AOSLMTR_UNLOCK(mLockMonitor);
	if (!status)
	{
		returnIILPublic(iil);
		AOSLMTR_FINISH(mLockMonitor);
		return 0;
	}
	AOSLMTR_FINISH(mLockMonitor);
	return iil;
}


u64
AosIILMgr::getIILIDPublic(const char *word, 
		const int len, 
		u64 &wordid,
		const AosIILType iiltype,
		const bool createFlag)
{
	if(len <= 0 || len >= AosIILUtil::eMaxStrValueLen)
	{
		if(len >= 0)
		{
			char www[101];
			memcpy(www, word, len);
			www[len] = 0;
			OmnScreen << "Add word: " << www << endl;
		}
		return false;
	}

	u64 ptr = 0;
	AOSLMTR_ENTER(mLockMonitor);
	AOSLMTR_LOCK(mLockMonitor);
	AOSIILCOUNT_TRIES;
	if (mIsStopping)
	{
		AOSIILCOUNT_MISS;
		AOSLMTR_UNLOCK(mLockMonitor);
		AOSLMTR_FINISH(mLockMonitor);
		return 0;
	}

	wordid = AosWordClient::getSelf()->getWordId((u8 *)word, len, true);
	if (wordid)
	{
		if (!AosWordClient::getSelf()->getPtr(wordid, ptr))
		{
			AOSLMTR_UNLOCK(mLockMonitor);
			AOSLMTR_FINISH(mLockMonitor);
			OmnAlarm << "Failed to get ptr: " << wordid << enderr;
			return 0;
		}

		if (ptr)
		{
			u64 iilid = (ptr >> 32);
			if (iilid) 
			{
				AOSLMTR_UNLOCK(mLockMonitor);
				AOSLMTR_FINISH(mLockMonitor);
				return iilid;
			}
		}
	}

	u64 iilid = AosSeIdGenMgr::getSelf()->nextIILId();
	u64 pp = (iilid << 32);
	AosWordClient::getSelf()->setPtr1(wordid, pp);
	AOSLMTR_UNLOCK(mLockMonitor);
	AOSLMTR_FINISH(mLockMonitor);
	return iilid;
}


// Chen Ding, 06/26/2011
bool 
AosIILMgr::modifyValueDoc(
		u64 &iilid, 
		const u64 &oldvalue, 
		const u64 &newvalue, 
		const u64 &docid, 
		const bool value_unique, 
		const bool docid_unique,
		const AosRundataPtr &rdata)
{
	AosIILPtr iil;
	if (iilid == 0)
	{
		iil = createIILPublic(eAosIILType_U64);
		aos_assert_rr(iil, rdata, false);
		iilid = iil->getIILID();
	}
	else
	{
		AosIILType type = eAosIILType_U64;
		iil = loadIILByIDPublic(iilid, type);
		aos_assert_rr(iil, rdata, false);
	}

	if (iil->getIILType() != eAosIILType_U64)
	{
		rdata->setError() << "IIL type mismatch: " << iil->getIILType();
		OmnAlarm << rdata->getErrmsg() << enderr;
		returnIILPublic(iil);
		return false;
	}

	AosIILU64 *u64iil = (AosIILU64 *)iil.getPtr();
	bool rslt = u64iil->modifyDocSafe(oldvalue,
		newvalue, docid, value_unique, docid_unique, false);
	returnIILPublic(iil);
	if (!rslt)
	{
		rdata->setError() << "Failed modifying doc: " 
			<< oldvalue << ":" << newvalue << ":" << docid
			<< ":" << value_unique << ":" << docid_unique;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	rdata->setOk();
	return true;
}

int
AosIILMgr::getDocids(
		const u64 &iilid, 
		const OmnString &key, 
		u64 *docids,
		const int arraysize,
		const AosRundataPtr &rdata) 
{
	if (mCheckRef) aos_assert_r(AosIIL::staticNumIILRefs() == 0, false);

	AosIILType type = eAosIILType_Str;
	AosIILPtr iil = loadIILByIDPublic(iilid, type, rdata);
	aos_assert_r(iil, false);
	if (iil->getIILType() != eAosIILType_Str)
	{
		returnIILPublic(iil, rdata);
		return false;
	}

	AosIILStrPtr striil = (AosIILStr*)iil.getPtr();
	int numfound = striil->getDocidsSafe(eAosOpr_eq, key, docids, arraysize, rdata); 
	returnIILPublic(iil, rdata);
	if (mCheckRef) aos_assert_r(AosIIL::staticNumIILRefs() == 0, false);
	return numfound;
}

int
AosIILMgr::getDocids(
		const OmnString &iilname, 
		const OmnString &key, 
		u64 *docids,
		const int arraysize,
		const AosRundataPtr &rdata) 
{
	if(iilname.length() <= 0 || iilname.length() >= AosIILUtil::eMaxStrValueLen)
	{
		OmnScreen << "length too long when getDocid : " << iilname << endl;
		return AOS_INVDID;
	}

	aos_assert_r(key != "", AOS_INVDID);
	if (mCheckRef) aos_assert_r(AosIIL::staticNumIILRefs() == 0, false);

	// Retrieve the IIL
	AosIILPtr iil = retrieveIIL(iilname.data(), iilname.length(), eAosIILType_Str, false, rdata);
	if (!iil)
	{
		// No IIL is found. 
		rdata->setError() << AOSERR_NOENTRY << ": " << key;
		return false;
	}

	if (iil->getIILType() != eAosIILType_Str)
	{
		rdata->setError() << "Internal error!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		returnIILPublic(iil, rdata);
		return false;
	}
	AosIILStrPtr striil = (AosIILStr*)iil.getPtr();
	
	// Check whether the doc is in the IIL (identified by 'key')
	int numfound = striil->getDocidsSafe(eAosOpr_eq, key, docids, arraysize, rdata); 
	returnIILPublic(iil, rdata);
	if (mCheckRef) aos_assert_r(AosIIL::staticNumIILRefs() == 0, false);
	return numfound;
}
*/
				
			
/*
bool	
AosIILMgr::getValueSafeStr(
		const u64 &iilid,
		int &crtIdx,
		int &crtiilidx,
		const bool order,	
		const AosQueryRsltObjPtr &query_rslt,
		const AosRundataPtr &rdata)
{	
	aos_assert_r(query_rslt,false);		
	if(query_rslt->isFull())	
	{
		return true;	
	}		
	AosIILType type = eAosIILType_Invalid;	
	AosIILPtr iil = loadIILByIDPublic(iilid, type, rdata);	
	if (!iil || type != eAosIILType_Str)	
	{		
		// No IIL is found. 		
		rdata->setError() << AOSERR_NOENTRY << ": " << iilid << ":" << type;		
		OmnAlarm << rdata->getErrmsg() << enderr;		
		return false;	
	}	
	
	AosIILStrPtr striil = (AosIILStr*)iil.getPtr();		
	u64 docid = AOS_INVDID;		
	while(!query_rslt->isFull())	
	{		
		docid = striil->nextDocIdSafe2(crtIdx,crtiilidx,!order, rdata);		
		if(docid == AOS_INVDID)		
		{			
			// no more docs			
			break;		
		}				
		query_rslt->appendDocid(docid);
	}		
	returnIILPublic(iil,rdata);	
	return true;
}


bool
AosIILMgr::docExistSafe(
		const u64 &iilid,
		const u64 &docid, 
		const AosRundataPtr &rdata)
{
	AosIILType type = eAosIILType_Invalid;
	AosIILPtr iil = loadIILByIDPublic(iilid, type, true, false, rdata);
	aos_assert_r(iil, false);
	
	bool rslt = iil->docExistSafe(docid, rdata);
	returnIILPublic(iil, rdata, true);
	return rslt;
}						 


bool
AosIILMgr::checkDocSafe(
		const u64 &iilid,
		const AosOpr opr,
		const OmnString &value,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	AosIILType type = eAosIILType_Str;
	AosIILPtr iil = loadIILByIDPublic(iilid, type, true, false, rdata);
	aos_assert_r(iil, false);
	if (iil->getIILType() != eAosIILType_CompStr)
	{
		returnIILPublic(iil, rdata, true);
		return false;
	}
	
	AosIILCompStr *strcompiil = (AosIILCompStr *)iil.getPtr();
	bool rslt = strcompiil->checkDocSafe(docid, value, opr, rdata);
	returnIILPublic(iil, rdata, true);
	return rslt;
}


bool
AosIILMgr::addDoc(
		const u64 &iilid,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	AosIILType type = eAosIILType_Hit;
	AosIILPtr iil = loadIILByIDPublic(iilid, type, false, false, rdata);
	aos_assert_r(iil, false);
	if (iil->getIILType() != eAosIILType_Hit)
	{
		returnIILPublic(iil, rdata, true);
		return false;
	}

	AosIILHit *hitiil = (AosIILHit *)iil.getPtr();
	bool rslt = hitiil->addDocSafe(docid, rdata);
	returnIILPublic(iil, rdata, true);
	if (mCheckRef) aos_assert_r(AosIIL::staticNumIILRefs() == 0, false);
	return rslt;
}


bool
AosIILMgr::addValueDoc(
		const u64 &iilid,
		const OmnString &value,
		const u64 &docid,
		const bool &value_unique,
		const bool &docid_unique,
		const AosRundataPtr &rdata) 
{
	if(value.length() <=0 || value.length()>= AosIILUtil::eMaxStrValueLen)
	{
		OmnString value2;
		if(value.length() > 300)
		{
			value2 = value.subString(0,300);
		}else
		{
			value2 = value;
		}
		OmnScreen << "word len not right:" <<value.length()<< ":" << value2;
		return false;
	}

	AosIILType type = eAosIILType_Str;
	AosIILPtr iil = loadIILByIDPublic(iilid, type, false, false, rdata);
	aos_assert_r(iil, false);
	if (iil->getIILType() != eAosIILType_Str)
	{
		rdata->setError() << "Retrieved is not strIIL: " 
			<< iil->getIILType() 
			<< ":" << value << ":" << docid;
		OmnAlarm << rdata->getErrmsg() << enderr;
		returnIILPublic(iil, rdata, true);
		return false;
	}

	AosIILStr *striil = (AosIILStr *)iil.getPtr();
	bool rslt = striil->addDocSafe(value, docid, value_unique, docid_unique, false, rdata);
	returnIILPublic(iil, rdata, true);
	if (mCheckRef) aos_assert_r(AosIIL::staticNumIILRefs() == 0, false);

	if (!rslt)
	{
		rdata->setError() << "Failed adding the value: " 
			<< value << ":" << docid;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	rdata->setOk();
	return rslt;
}


bool
AosIILMgr::addU64ValueDoc(
		const u64 &iilid,
		const u64 &value,
		const u64 &docid,
		const bool &value_unique,
		const bool &docid_unique,
		const AosRundataPtr &rdata) 
{
	AosIILType type = eAosIILType_U64;
	AosIILPtr iil = loadIILByIDPublic(iilid, type, false, false, rdata);
	aos_assert_r(iil, false);
	if (iil->getIILType() != eAosIILType_U64)
	{
		rdata->setError() << "IIL type mismatch: " << iil->getIILType()
			<< ": eAosIILType_U64";
		OmnAlarm << rdata->getErrmsg() << enderr;
		returnIILPublic(iil, rdata, true);
		return false;
	}

	AosIILU64 *u64iil = (AosIILU64 *)iil.getPtr();
	bool rslt = u64iil->addDocSafe(value, docid, value_unique, docid_unique, false, rdata);
	returnIILPublic(iil, rdata, true);
	if (mCheckRef) aos_assert_r(AosIIL::staticNumIILRefs() == 0, false);
	if (!rslt)
	{
		rdata->setError() << "Failed adding value: " 
			<< value << ":" << docid;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	rdata->setOk();
	return rslt;
}


bool
AosIILMgr::removeDoc(
		const u64 &iilid,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	AosIILType type = eAosIILType_Hit;
	AosIILPtr iil = loadIILByIDPublic(iilid, type, false, false, rdata);
	aos_assert_r(iil, false);
	if (iil->getIILType() != eAosIILType_Hit)
	{
		returnIILPublic(iil, rdata, true);
		return false;
	}

	AosIILHit *hitiil = (AosIILHit *)iil.getPtr();
	bool rslt = hitiil->removeDocSafe(docid, rdata);
	returnIILPublic(iil, rdata, true);
	if (mCheckRef) aos_assert_r(AosIIL::staticNumIILRefs() == 0, false);
	return rslt;
}


bool
AosIILMgr::removeValueDoc(
		const u64 &iilid,
		const OmnString &value,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	AosIILType type = eAosIILType_Str;
	AosIILPtr iil = loadIILByIDPublic(iilid, type, false, false, rdata);
	aos_assert_r(iil, false);
	if (iil->getIILType() != eAosIILType_Str)
	{
		returnIILPublic(iil, rdata, true);
		return false;
	}

	AosIILStr *striil = (AosIILStr *)iil.getPtr();
	bool rslt = striil->removeDocSafe(value, docid, rdata);
	returnIILPublic(iil, rdata, true);
	if (mCheckRef) aos_assert_r(AosIIL::staticNumIILRefs() == 0, false);
	return rslt;
}

	
bool
AosIILMgr::removeU64ValueDoc(
		const u64 &iilid,
		const u64 &value,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	AosIILType type = eAosIILType_U64;
	AosIILPtr iil = loadIILByIDPublic(iilid, type, false, false, rdata);
	aos_assert_r(iil, false);
	if (iil->getIILType() != eAosIILType_U64)
	{
		returnIILPublic(iil, rdata, true);
		return false;
	}

	AosIILU64 *u64iil = (AosIILU64 *)iil.getPtr();
	bool rslt = u64iil->removeDocSafe(value, docid, rdata);
	returnIILPublic(iil, rdata, true);
	if (mCheckRef) aos_assert_r(AosIIL::staticNumIILRefs() == 0, false);
	return rslt;
}


bool
AosIILMgr::modifyValueDoc(
		const u64 &iilid,
		const OmnString &oldvalue, 
		const OmnString &newvalue, 
		const bool value_unique,
		const bool docid_unique,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	int len1 = oldvalue.length();
	int len2 = newvalue.length();
	if (mShowLog)
	{
		OmnScreen << "Modify strval: " << iilid<< ":" 
			<< oldvalue << ":" <<docid << ";" << newvalue << ":" << docid << endl;
	}
	if(len1 <= 0 || len1>= AosIILUtil::eMaxStrValueLen)
	{
		OmnScreen << "old value too long: " << oldvalue << ":" << oldvalue.subString(0,300)<< ":" << docid << endl;
		return false;
	}
	
	if(len2 <= 0 || len2>= AosIILUtil::eMaxStrValueLen)
	{
		OmnScreen << "old value too long: " << newvalue << ":" << newvalue.subString(0,300)<< ":" << docid << endl;
		return false;
	}

	AosIILType type = eAosIILType_Str;
	AosIILPtr iil = loadIILByIDPublic(iilid, type, false, false, rdata);
	aos_assert_r(iil, false);
	if (iil->getIILType() != eAosIILType_Str)
	{
		returnIILPublic(iil, rdata, true);
		return false;
	}

	AosIILStr *striil = (AosIILStr *)iil.getPtr();
	bool rslt = striil->modifyDocSafe(oldvalue, newvalue, docid, value_unique, docid_unique, false, rdata);
	returnIILPublic(iil, rdata, true);
	if (mCheckRef) aos_assert_r(AosIIL::staticNumIILRefs() == 0, false);
	return rslt;
}


bool		
AosIILMgr::modifyU64ValueDoc(
		const u64 &iilid,	
		const u64 &oldvalue, 
		const u64 &newvalue, 
		const bool value_unique,
		const bool docid_unique,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	AosIILType type = eAosIILType_U64;
	AosIILPtr iil = loadIILByIDPublic(iilid, type, false, false, rdata);
	aos_assert_r(iil, false);
	if (iil->getIILType() != eAosIILType_U64)
	{
		returnIILPublic(iil, rdata, true);
		return false;
	}
	
	AosIILU64 *u64iil = (AosIILU64 *)iil.getPtr();
	bool rslt = u64iil->modifyDocSafe(oldvalue, newvalue, docid, value_unique, docid_unique,false, rdata);
	returnIILPublic(iil, rdata, true);
	if (mCheckRef) aos_assert_r(AosIIL::staticNumIILRefs() == 0, false);
	if(!rslt)
	{
		rdata->setError() << "Failed modifying doc: " 
			<< oldvalue << ":" << newvalue << ":" << docid
			<< ":" << value_unique << ":" << docid_unique;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	rdata->setOk();
	return rslt;
}


bool    
AosIILMgr::addDescendant(
		const u64 iilid,
		vector<u64> ancestors,
		const u64 &docid,
		const u64 &parent_docid,
		const AosRundataPtr &rdata)
{
	AosIILType type = eAosIILType_U64;
	AosIILPtr iil = loadIILByIDPublic(iilid, type, false, false, rdata);
	if (!iil)
	{
		rdata->setError() << "Load IIL Failed:" ;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	if (iil->getIILType() != eAosIILType_U64)
	{
		rdata->setError() << "Type mismatch:" ;
		OmnAlarm << rdata->getErrmsg() << enderr;
		returnIILPublic(iil, rdata, true);
		return false;
	}
	
	AosIILU64Ptr iilu64 = (AosIILU64*)iil.getPtr();
	ancestors.push_back(parent_docid);
	for (size_t i = 0; i < ancestors.size(); i++)
	{
		bool rslt = iilu64->addDocSafe(ancestors[i], docid, false, false, false, rdata);
OmnScreen << "Add descendant: " << docid  << ", " << ancestors[i] << endl;
		if (!rslt)
		{
			OmnAlarm << enderr;
			break;
		}
	}

	returnIILPublic(iil, rdata, true);
	return true;
}


bool    
AosIILMgr::removeDescendant(
		const u64 iilid,
		vector<u64> ancestors,
		const u64 &docid,
		const u64 &parent_docid,
		const AosRundataPtr &rdata)
{
	AosIILType type = eAosIILType_U64;
	AosIILPtr iil = loadIILByIDPublic(iilid, type, false, false, rdata);
	if (!iil)
	{
		rdata->setError() << "Load IIL Failed:" ;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	if (iil->getIILType() != eAosIILType_U64)
	{
		rdata->setError() << "Type mismatch:" ;
		OmnAlarm << rdata->getErrmsg() << enderr;
		returnIILPublic(iil, rdata, true);
		return false;
	}

	AosIILU64Ptr iilu64 = (AosIILU64*)iil.getPtr();
	//1. add parentdocid : docid
	//2. find pparentdocid of the parentdocid from ancesster iil
	//3. pparentdocid : docid
	//4. loop all parent.
	ancestors.push_back(parent_docid);
	for (size_t i = 0; i < ancestors.size(); i++)
	{
		bool rslt = iilu64->removeDocSafe(ancestors[i], docid, rdata);
		if (!rslt)
		{
			OmnAlarm << enderr;
			break;
		}
	}

	returnIILPublic(iil, rdata, true);
	return true;
}


bool    
AosIILMgr::addAncestor(
		const u64 iilid,
		const u64 &docid,
		const u64 &parent_docid,
		const AosRundataPtr &rdata)
{
	AosIILType type = eAosIILType_U64;
	AosIILPtr iil = loadIILByIDPublic(iilid, type, false, false, rdata);
	if (!iil)
	{
		rdata->setError() << "Load IIL Failed:" ;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	if (iil->getIILType() != eAosIILType_U64)
	{
		rdata->setError() << "Type mismatch:" ;
		OmnAlarm << rdata->getErrmsg() << enderr;
		returnIILPublic(iil, rdata, true);
		return false;
	}

	AosIILU64Ptr iilu64 = (AosIILU64*)iil.getPtr();
	//1. add docid : parentdocid
	//2. find pparentdocid of the parentdocid from ancesster iil
	//3. add docid : pparentdocid
	//4. loop all parent.
	//vector<u64> pids(16);
	vector<u64> pids;
	pids.reserve(16);
	pids.push_back(parent_docid);
	u64 pdid = parent_docid;
	int idx = -10;
	int iilidx = -10;
	bool rslt = false;
	bool unique;
	while(1)
	{
		u64 did = 0;
		rslt = iilu64->nextDocidSafe(idx, iilidx, false, eAosOpr_eq, pdid, did, unique, rdata);
		if (!rslt)
		{
			OmnAlarm << enderr;
			break;
		}
		if (idx == -5  || iilidx == -5 || !did)
		{
			break;
		}
		pids.push_back(did);
	}

	if (rslt)
	{
		for (size_t i = 0; i < pids.size(); i++)
		{
			rslt = iilu64->addDocSafe(docid, pids[i], false, false, false, rdata);
OmnScreen << "Add ancestor : " << docid << ", " << pids[i] << endl;
			if (!rslt)
			{
				OmnAlarm << enderr;
				break;
			}
		}
	}

	returnIILPublic(iil, rdata, true);
	return true;
}


bool    
AosIILMgr::removeAncestor(
		const u64 iilid,
		const u64 &docid,
		const u64 &parent_docid,
		const AosRundataPtr &rdata)
{
	AosIILType type = eAosIILType_U64;
	AosIILPtr iil = loadIILByIDPublic(iilid, type, false, false, rdata);
	if (!iil)
	{
		rdata->setError() << "Load IIL Failed:" ;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	if (iil->getIILType() != eAosIILType_U64)
	{
		rdata->setError() << "Type mismatch:" ;
		OmnAlarm << rdata->getErrmsg() << enderr;
		returnIILPublic(iil, rdata, true);
		return false;
	}

	AosIILU64Ptr iilu64 = (AosIILU64*)iil.getPtr();
	//1. add docid: parentdocid 
	//2. find pparentdocid of the parentdocid from ancesster iil
	//3. add docid:pparentdocid
	//4. loop all parent.
	//vector<u64> pids(16);
	vector<u64> pids;
	pids.reserve(16);
	pids.push_back(parent_docid);
	u64 pdid = parent_docid;
	int idx = -10;
	int iilidx = -10;
	bool rslt = false;
	bool unique;
	while(1)
	{
		u64 did = 0;
		rslt = iilu64->nextDocidSafe(idx, iilidx, false, eAosOpr_eq, pdid, did, unique, rdata);
		if (!rslt)
		{
			OmnAlarm << enderr;
			break;
		}
		if (idx == -5  || iilidx == -5 || !did)
		{
			break;
		}
		pids.push_back(did);
	}

	if (rslt)
	{
		for (size_t i = 0; i < pids.size(); i++)
		{
			rslt = iilu64->removeDocSafe(docid, pids[i], rdata);
			if (!rslt)
			{
				OmnAlarm << enderr;
				break;
			}
		}
	}

	returnIILPublic(iil, rdata, true);
	return true;
}


vector<u64>
AosIILMgr::getAncestor(
		const u64 iilid,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	AosIILType type = eAosIILType_U64;
	AosIILPtr iil = loadIILByIDPublic(iilid, type, false, false, rdata);
	if (!iil)
	{
		rdata->setError() << "Load IIL Failed:" ;
		OmnAlarm << rdata->getErrmsg() << enderr;
		vector<u64> tmp;
		return tmp;
	}

	AosIILU64Ptr iilu64 = (AosIILU64*)iil.getPtr();
	//1. add docid: parentdocid 
	//2. find pparentdocid of the parentdocid from ancesster iil
	//3. add docid:pparentdocid
	//4. loop all parent.
	vector<u64> pids;
	pids.reserve(16);
	u64 pdid = docid;
	int idx = -10;
	int iilidx = -10;
	bool rslt = false;
	bool unique;
	while(1)
	{
		u64 did = 0;
		rslt = iilu64->nextDocidSafe(idx, iilidx, false, eAosOpr_eq, pdid, did, unique, rdata);
		if (!rslt)
		{
			OmnAlarm << enderr;
			break;
		}
		if (idx == -5  || iilidx == -5 || !did)
		{
			break;
		}
		pids.push_back(did);
	}
	returnIILPublic(iil, rdata, true);
	return pids;
}


bool		
AosIILMgr::setValueDocUnique(
		const u64 &iilid, 
		const OmnString &key,
		const u64 &docid, 
		const bool must_same,
		const AosRundataPtr &rdata)
{
	AosIILType type = eAosIILType_Str;
	AosIILPtr iil = loadIILByIDPublic(iilid, type, false, false, rdata);
	aos_assert_r(iil, false);
	if (iil->getIILType() != eAosIILType_Str)
	{
		rdata->setError() << "Type mismatch:" 
			<< iil->getIILType() << ":" << eAosIILType_Str << ":" << docid;
		OmnAlarm << rdata->getErrmsg() << enderr;
		returnIILPublic(iil, rdata, true);
		return false;
	}
	
	AosIILStrPtr iilstr = (AosIILStr*)iil.getPtr();
	bool rslt = iilstr->setValueDocUnique(key, docid, must_same, rdata);
	returnIILPublic(iil, rdata, true);
	if (!rslt)
	{
		rdata->setError() << "Failed setting doc value: " << iilid << ":" << key << ":" << docid;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	rdata->setOk();
	return rslt;
}


bool		
AosIILMgr::setU64ValueDocUnique(
		const u64 &iilid, 
		const u64 &key,
		const u64 &docid, 
		const AosRundataPtr &rdata)
{
	AosIILType type = eAosIILType_U64;
	AosIILPtr iil = loadIILByIDPublic(iilid, type, false, false, rdata);
	aos_assert_r(iil, false);
	if (iil->getIILType() != eAosIILType_U64)
	{
		returnIILPublic(iil, rdata, true);
		return false;
	}
	
	AosIILU64Ptr iilu64 = (AosIILU64*)iil.getPtr();
	bool rslt = iilu64->setValueDocUnique(key, docid, false, rdata);
	returnIILPublic(iil, rdata, true);
	if (!rslt)
	{
		rdata->setError() << "Failed setting doc value: " << iilid << ":" << key << ":" << docid;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	rdata->setOk();
	return rslt;
}


bool
AosIILMgr::updateKeyedValue(
		const u64 &iilid,
		const u64 &key,
		const bool &inc_flag,
		const u64 &delta,
		u64 &new_value,
		const AosRundataPtr &rdata)
{
	AosIILType type = eAosIILType_U64;
	AosIILPtr iil = loadIILByIDPublic(iilid, type, false, false, rdata);
	aos_assert_r(iil, false);
	if (iil->getIILType() != eAosIILType_U64)
	{
		returnIILPublic(iil, rdata, true);
		return false;
	}

	AosIILU64Ptr iilu64 = (AosIILU64 *)iil.getPtr();
	bool rslt = iilu64->updateKeyedValue(key, inc_flag, delta, new_value, rdata);
	returnIILPublic(iil, rdata, true);
	return rslt;
}

bool
AosIILMgr::updateKeyedValue(
		const u64 &iilid,
		const u64 &key,
		const u64 &value,
		const AosRundataPtr &rdata)
{
	AosIILType type = eAosIILType_U64;
	AosIILPtr iil = loadIILByIDPublic(iilid, type, false, false, rdata);
	aos_assert_r(iil, false);
	if (iil->getIILType() != eAosIILType_U64)
	{
		returnIILPublic(iil, rdata, true);
		return false;
	}

	AosIILU64Ptr iilu64 = (AosIILU64 *)iil.getPtr();
	iilu64->updateKeyedValue(key, value, rdata);
	returnIILPublic(iil, rdata, true);
	return true;
}


bool
AosIILMgr::appendManualOrder(
		const u64 &iilid, 
		const u64 &docid, 
		u64 &value,
		const AosRundataPtr &rdata)
{
	AosIILType type = eAosIILType_U64;
	AosIILPtr iil = loadIILByIDPublic(iilid, type, false, false, rdata);
	aos_assert_r(iil, false);
	if (iil->getIILType() != eAosIILType_U64)
	{
		returnIILPublic(iil, rdata, true);
		return false;
	}
	
	AosIILU64 *treeiil = (AosIILU64 *)iil.getPtr();
	bool rslt = treeiil->appendManualOrder(docid, value, rdata);
	returnIILPublic(iil, rdata, true);
	return rslt;
}


bool
AosIILMgr::moveManualOrder(
		const u64 &iilid,
		u64 &value1,
		const u64 &docid1,
		u64 &value2,
		const u64 &docid2,
		const OmnString flag,
		const AosRundataPtr &rdata)
{
	AosIILType type = eAosIILType_U64;
	AosIILPtr iil = loadIILByIDPublic(iilid, type, false, false, rdata);
	aos_assert_r(iil, false);
	if (iil->getIILType() != eAosIILType_U64)
	{
		returnIILPublic(iil, rdata, true);
		return false;
	}

	AosIILU64 *treeiil = (AosIILU64 *)iil.getPtr();
	bool rslt = treeiil->moveManualOrder(value1, docid1, value2, docid2, flag, rdata);
	returnIILPublic(iil, rdata, true);
	return rslt;
}


bool
AosIILMgr::swapManualOrder(
		const u64 &iilid,
		const u64 &value1, 
		const u64 &docid1,
		const u64 &value2,
		const u64 &docid2,
		const AosRundataPtr &rdata)
{
	AosIILType type = eAosIILType_U64;
	AosIILPtr iil = loadIILByIDPublic(iilid, type, false, false, rdata);
	aos_assert_r(iil, false);
	if (iil->getIILType() != eAosIILType_U64)
	{
		returnIILPublic(iil, rdata, true);
		return false;
	}

	AosIILU64 *treeiil = (AosIILU64 *)iil.getPtr();
	bool rslt = treeiil->swapManualOrder(value1, docid1, value2, docid2, rdata);
	returnIILPublic(iil, rdata, true);
	return rslt;
}


bool
AosIILMgr::removeManualOrder(
		const u64 &iilid,
		const u64 &value, 
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	AosIILType type = eAosIILType_U64;
	AosIILPtr iil = loadIILByIDPublic(iilid, type, false, false, rdata);
	aos_assert_r(iil, false);
	if (iil->getIILType() != eAosIILType_U64)
	{
		returnIILPublic(iil, rdata, true);
		return false;
	}

	AosIILU64 *treeiil = (AosIILU64 *)iil.getPtr();
	bool rslt = treeiil->removeManualOrder(value, docid, rdata);
	returnIILPublic(iil, rdata, true);
	return rslt;
}


bool
AosIILMgr::getDocid(
		const u64 &iilid,
		const OmnString &key, 
		u64 &docid,
		bool &isunique,
		const bool removeflag,
		const AosRundataPtr &rdata)
{
	AosIILType type = eAosIILType_Str;
	AosIILPtr iil = loadIILByIDPublic(iilid, type, false, false, rdata);
	aos_assert_r(iil, false);
	if (iil->getIILType() != eAosIILType_Str)
	{
		returnIILPublic(iil, rdata, true);
		return false;
	}
	
	AosIILStrPtr striil = (AosIILStr*)iil.getPtr();
	int idx = -10;
	int iilidx = -10;
	bool rslt = striil->nextDocidSafe(idx, iilidx, false, eAosOpr_eq, key, docid, isunique, rdata);
	if (removeflag && docid != AOS_INVDID)
	{
		// Found one and need to remove it
		striil->removeDocSafe(key, docid, rdata);
	}
	returnIILPublic(iil, rdata, true);
	if (mCheckRef) aos_assert_r(AosIIL::staticNumIILRefs() == 0, false);
	rdata->setOk();
	return rslt;
}


bool
AosIILMgr::getDocid(
		const u64 &iilid, 
		const u64 &key, 
		u64 &docid, 
		const u64 &dft, 
		bool &found,
		bool &is_unique,
		const AosRundataPtr &rdata)
{
	docid = dft;
	found = false;
	
	AosIILType type = eAosIILType_U64;
	AosIILPtr iil = loadIILByIDPublic(iilid, type, false, false, rdata);
	aos_assert_r(iil, false);
	if (iil->getIILType() != eAosIILType_U64)
	{
		returnIILPublic(iil, rdata, true);
		return false;
	}
	
	AosIILU64Ptr u64iil = (AosIILU64*)iil.getPtr();
	int idx = -10;
	int iilidx = -10;
	u64 kkk = key;
	bool rslt = u64iil->nextDocidSafe(idx, iilidx, false, eAosOpr_eq, kkk, docid, is_unique, rdata); 
	returnIILPublic(iil, rdata, true);
	if (idx < 0)
	{
		found = false;
		docid = dft;
	}
	else
	{
		found = true;
	}
	return rslt;
}


bool
AosIILMgr::getDocid(
		const u64 &iilid, 
		const OmnString &key, 
		u64 &docid, 
		const u64 &dft, 
		bool &found,
		bool &is_unique,
		const AosRundataPtr &rdata)
{
	docid = dft;
	found = false;
	
	AosIILType type = eAosIILType_Str;
	AosIILPtr iil = loadIILByIDPublic(iilid, type, false, false, rdata);
	aos_assert_r(iil, false);
	if (iil->getIILType() != eAosIILType_Str)
	{
		returnIILPublic(iil, rdata, true);
		return false;
	}
	
	AosIILStrPtr striil = (AosIILStr*)iil.getPtr();
	int idx = -10;
	int iilidx = -10;
	bool rslt = striil->nextDocidSafe(idx, iilidx, false, eAosOpr_eq, key, docid, is_unique, rdata); 
	returnIILPublic(iil, rdata, true);
	if (idx < 0)
	{
		found = false;
		docid = dft;
	}
	else
	{
		found = true;
	}
	return rslt;
}


bool	
AosIILMgr::incrementDocid(
		const u64 &iilid,
		const bool createiil,
		const bool isPersis,
		const OmnString &key, 
		u64 &value,
		const u64 &incValue,
		const u64 &init_value,
		const bool add_flag,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(iilid, rdata, false);
	AosIILType type = eAosIILType_Str;
	AosIILPtr iil = loadIILByIDPublic(iilid, type, false, false, rdata); 
	bool rslt;
	if(!iil && createiil)
	{
		u64 id = iilid;
		iil = createIILPublic1(id, type, isPersis, false, rdata);
	}
	aos_assert_r(iil, false);
	if (iil->getIILType() != eAosIILType_Str)
	{
		returnIILPublic(iil, rdata, true);
		return false;
	}

	AosIILStrPtr iilstr = (AosIILStr*)iil.getPtr();
	rslt = iilstr->incrementDocidSafe1(key, value, incValue, init_value, add_flag, rdata);
	returnIILPublic(iil, rdata, true);
	return rslt;
}


bool
AosIILMgr::incrementDocid(
		const u64 &iilid,
		const u64 &key, 
		const u64 &incValue, 
		const u64 &initValue,
		u64 &new_value,
		const AosRundataPtr &rdata) 
{
	AosIILType type = eAosIILType_U64;
	AosIILPtr iil = loadIILByIDPublic(iilid, type, false, false, rdata);
	aos_assert_r(iil, false);
	if (iil->getIILType() != eAosIILType_U64)
	{
		returnIILPublic(iil, rdata, true);
		return false;
	}
	
	AosIILU64 *u64iil = (AosIILU64 *)iil.getPtr();
	bool rslt = u64iil->incrementDocid(key, incValue, initValue, new_value, rdata);
	returnIILPublic(iil, rdata, true);
	return rslt;
}


bool
AosIILMgr::bindCloudid(
		const u64 &iilid,
		const OmnString &cloudid, 
		const u64 &docid, 
		const AosRundataPtr &rdata)
{
	AosIILType type = eAosIILType_Str;
	AosIILPtr iil = loadIILByIDPublic(iilid, type, false, false, rdata);
	aos_assert_r(iil, false);
	if (iil->getIILType() != eAosIILType_Str)
	{
		returnIILPublic(iil, rdata, true);
		return false;
	}
	bool rslt = AosIILMgr::bindCloudid(iil, cloudid, docid, rdata);
	returnIILPublic(iil, rdata, true);
	return rslt;
}


bool 
AosIILMgr::bindObjid(
		const u64 &iilid,
		OmnString &objid, 
		const u64 &docid,
		bool &objid_changed,
		const bool resolve,
		const AosRundataPtr &rdata)
{
	// This function binds 'objid' with 'docid'. If the objid is used by someone else, 
	// 'resolve' determines whether to resolve the objid or not.
	AosIILType type = eAosIILType_Str;
	AosIILPtr iil = loadIILByIDPublic(iilid, type, false, false, rdata);
	aos_assert_r(iil, false);

	bool rslt = bindObjidPriv(iil, objid, docid, objid_changed, resolve, rdata);
	returnIILPublic(iil, rdata, true);
	return rslt;
}


bool
AosIILMgr::queryValueSafe(
		const u64 &iilid,
		vector<OmnString> &values,
		const AosOpr opr,
		const OmnString &value,
		const bool unique_value,
		const AosRundataPtr &rdata)
{
	AosIILType type = eAosIILType_Str;
	AosIILPtr iil = loadIILByIDPublic(iilid, type, false, false, rdata);
	aos_assert_r(iil, false);
	if (iil->getIILType() != eAosIILType_Str)
	{
		returnIILPublic(iil, rdata, true);
		return false;
	}
	
	AosIILStrPtr striil = (AosIILStr*)iil.getPtr();
	bool rslt = striil->queryValueSafe(values, opr, value, unique_value, rdata);
	returnIILPublic(iil, rdata, true);
	return rslt;
}


bool
AosIILMgr::queryU64ValueSafe(
		const u64 &iilid,
		vector<u64> &values,
		const AosOpr opr,
		const u64 &value,
		const bool unique_value,
		const AosRundataPtr &rdata)
{
	AosIILType type = eAosIILType_U64;
	AosIILPtr iil = loadIILByIDPublic(iilid, type, false, false, rdata);
	aos_assert_r(iil, false);
	if (iil->getIILType() != eAosIILType_U64)
	{
		returnIILPublic(iil, rdata, true);
		return false;
	}
	AosIILU64Ptr u64iil = (AosIILU64*)iil.getPtr();
	bool rslt = u64iil->queryValueSafe(values, opr, value, unique_value, rdata);
	returnIILPublic(iil, rdata, true);
	return rslt;
}


u64
AosIILMgr::getCtnrMemDocid(
		const u64 &iilid,
		const OmnString &keyname,
		const OmnString &keyvalue,
		bool &duplicated, 
		const AosRundataPtr &rdata)
{
	AosIILType iiltype = eAosIILType_Str;
	AosIILPtr iil = loadIILByIDPublic(iilid, iiltype, false, false, rdata);
	aos_assert_r(iil, false);
	if(iil->getIILType() != eAosIILType_Str)
	{
		OmnScreen << "Iil type mismatch. expect to get a hit iil:" << iiltype << endl;
		returnIILPublic(iil,rdata, true);
		return 0;
	}

	OmnString iilname = AosIILName::composeAttrIILName(keyname);
	AosQueryRsltObjPtr query_rslt;
	AosBitmapObjPtr query_bitmap = AosQueryRsltMgr::getSelf()->getBitmap();
	bool rslt = AosIILClient::getSelf()->querySafe(iilname, query_rslt, query_bitmap, eAosOpr_eq, keyvalue, rdata);
	if(!rslt || query_bitmap->isEmpty())
	{
		AosQueryRsltMgr::getSelf()->returnBitmap(query_bitmap);
		query_bitmap = 0;
		return 0;
	}

	AosIILStrPtr iilstr = (AosIILStr*)iil.getPtr();
	AosBitmapObjPtr query_bitmap2 = AosQueryRsltMgr::getSelf()->getBitmap();
	rslt = iilstr->querySafe(0, query_bitmap2, eAosOpr_an, "", rdata);
	if(!rslt || query_bitmap2->isEmpty())
	{
		AosQueryRsltMgr::getSelf()->returnBitmap(query_bitmap);
		AosQueryRsltMgr::getSelf()->returnBitmap(query_bitmap2);
		query_bitmap = 0;
		query_bitmap2 = 0;
		returnIILPublic(iil, rdata);
		return 0;
	}
	
	AosBitmap::countRsltAnd(query_bitmap, query_bitmap2);
	query_bitmap->reset();
	u64 did = query_bitmap->nextDocid();
	if(did == AOS_INVDID)
	{
		rdata->setError() << "Object not found (003). Key Name: " << keyname << "; Key Value: " << keyvalue;
	}
		
	AosQueryRsltMgr::getSelf()->returnBitmap(query_bitmap);
	AosQueryRsltMgr::getSelf()->returnBitmap(query_bitmap2);
	query_bitmap = 0;
	query_bitmap2 = 0;
	returnIILPublic(iil, rdata);
	return did;
}


u64
AosIILMgr::getCtnrMemDocid(
		const u64 &iilid,
		const OmnString &keyname1,
		const OmnString &keyvalue1,
		const OmnString &keyname2,
		const OmnString &keyvalue2,
		bool &duplicated, 
		const AosRundataPtr &rdata)
{
	AosIILType iiltype = eAosIILType_Str;
	AosIILPtr iil = loadIILByIDPublic(iilid, iiltype, false, false, rdata);
	aos_assert_r(iil, false);
	if(iil->getIILType() != eAosIILType_Str)
	{
		OmnScreen << "Iil type mismatch. expect to get a hit iil:" << iiltype << endl;
		returnIILPublic(iil,rdata);
		return false;
	}

	OmnString iilname1 = AosIILName::composeAttrIILName(keyname1);
	AosBitmapObjPtr query_bitmap = AosQueryRsltMgr::getSelf()->getBitmap();
	bool rslt = AosIILClient::getSelf()->querySafe(iilname1, 0, query_bitmap, eAosOpr_eq, keyvalue1, rdata);
	if(!rslt || query_bitmap->isEmpty())
	{
		AosQueryRsltMgr::getSelf()->returnBitmap(query_bitmap);
		query_bitmap = 0;
		returnIILPublic(iil, rdata);
		return 0;
	}
	
	OmnString iilname2 = AosIILName::composeAttrIILName(keyname2);
	rslt = AosIILClient::getSelf()->querySafe(iilname2, 0, query_bitmap, eAosOpr_eq, keyvalue2, rdata);
	if(!rslt || query_bitmap->isEmpty())
	{
		AosQueryRsltMgr::getSelf()->returnBitmap(query_bitmap);
		query_bitmap = 0;
		returnIILPublic(iil, rdata);
		return 0;
	}
	
	AosIILStrPtr iilstr = (AosIILStr*)iil.getPtr();
	AosBitmapObjPtr query_bitmap2 = AosQueryRsltMgr::getSelf()->getBitmap();
	rslt = iilstr->querySafe(0, query_bitmap2, eAosOpr_an, "", rdata);
	if(!rslt ||  query_bitmap2->isEmpty())
	{
		AosQueryRsltMgr::getSelf()->returnBitmap(query_bitmap);
		AosQueryRsltMgr::getSelf()->returnBitmap(query_bitmap2);
		query_bitmap = 0;
		query_bitmap2 = 0;
		returnIILPublic(iil, rdata);
		return 0;
	}

	AosBitmap::countRsltAnd(query_bitmap, query_bitmap2);
	query_bitmap->reset();
	u64 did = query_bitmap->nextDocid();
	if(did == AOS_INVDID)
	{
		rdata->setError() << "Object not found (003). Key Name1: " << keyname1 << "; Key Value1: " << keyvalue1
			<< "Key Name2: " << keyname2 << "; Key Value2: " << keyvalue2;
	}
		
	AosQueryRsltMgr::getSelf()->returnBitmap(query_bitmap);
	AosQueryRsltMgr::getSelf()->returnBitmap(query_bitmap2);
	query_bitmap = 0;
	query_bitmap2 = 0;
	returnIILPublic(iil, rdata);
	return did;
}


bool
AosIILMgr::removeU64DocByValue(
		const u64 &iilid,
        const u64 &value,
        int &entriesRemoved,
		const AosRundataPtr &rdata)
{
	AosIILType type = eAosIILType_U64;
	AosIILPtr iil = loadIILByIDPublic(iilid, type, false, false, rdata);
	aos_assert_r(iil, false);
	if (iil->getIILType() != eAosIILType_U64)
	{
		returnIILPublic(iil, rdata, true);
		return false;
	}
	AosIILU64Ptr u64iil = (AosIILU64*)iil.getPtr();
	bool rslt = u64iil->removeDocByValueSafe(value, entriesRemoved, rdata);
	returnIILPublic(iil, rdata, true);
	return rslt;
}


AosIILType 	
AosIILMgr::getIILType(
		const u64 &iilid,
		const AosRundataPtr &rdata)
{
	// This function retrieves the IIL type identified by 'iilid'. 
	// The iil should not be comp iil. 
	AosIILType type = eAosIILType_Invalid;
	aos_assert_rr(!AosIIL::isCompIIL(iilid), rdata, type);

	AOSLMTR_ENTER(mLockMonitor);
	AOSLMTR_LOCK(mLockMonitor);
	AosIILPtr iil;

	IILIDHashItr itr = mIILIDHash.find(iilid);
	if(itr != mIILIDHash.end())
	{
		iil = itr->second;
		type = iil->getIILType();
		AOSLMTR_UNLOCK(mLockMonitor);
		AOSLMTR_FINISH(mLockMonitor);
		return type;
	}
	AOSLMTR_UNLOCK(mLockMonitor);
	AOSLMTR_FINISH(mLockMonitor);

	char bb[AosIIL::eIILHeaderMemsize+10];
	AosBuff buff(bb, AosIIL::eIILHeaderMemsize, 0, 0 AosMemoryCheckerArgs);
	if (!AosIIL::staticReadHeaderBuff(iilid, buff, rdata))
	{
		return type;
	}

	if (!AosIILType_toCode(type, buff.getChar(eAosIILType_Invalid)))
	{
		type = eAosIILType_Invalid;
	}
	return type;	
}


int		
AosIILMgr::getTotalNumDocsSafe(
		const u64 &iilid, 
		const AosRundataPtr &rdata)
{
	AosIILType iiltype = eAosIILType_Invalid;
	AosIILPtr iil = loadIILByIDPublic(iilid, iiltype, false, false, rdata);
	if(!iil)
	{
		return 0;
	}
	int num = iil->getTotalNumDocsSafe();
	returnIILPublic(iil, rdata, true);	
	return num;	
}


int		
AosIILMgr::getTotalSafe(
		const u64 &iilid,
		const AosOpr opr, 
		const OmnString &value, 
		const AosRundataPtr &rdata)
{
	AosIILType iiltype = eAosIILType_Invalid;
	AosIILPtr iil = loadIILByIDPublic(iilid, iiltype, false, false, rdata);
	if(!iil)
	{
		return 0;
	}
	int num = iil->getTotalSafe(value, opr);
	returnIILPublic(iil, rdata, true);	
	return num;	
}						 


bool	
AosIILMgr::querySafe(
		const u64 &iilid,
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap, 
		const AosRundataPtr &rdata)
{
	AosIILType iiltype = eAosIILType_Hit;
	AosIILPtr iil = loadIILByIDPublic(iilid, iiltype, false, false, rdata);
	aos_assert_r(iil, false);
	if(iil->getIILType() != eAosIILType_Hit)
	{
		OmnScreen << "Iil type mismatch. expect to get a hit iil:" << iiltype << endl;
		returnIILPublic(iil,rdata, true);	
		return false;
	}
	AosIILHitPtr hitiil = (AosIILHit*)iil.getPtr();
	bool rslt = hitiil->querySafe(query_rslt, query_bitmap, rdata);
	returnIILPublic(iil, rdata, true);	
	return rslt;
}


bool	
AosIILMgr::querySafeByIILID(
		const u64 &iilid,
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosOpr opr,
		const OmnString &value, 
		const AosRundataPtr &rdata)
{
	AosIILType iiltype = eAosIILType_Invalid;
	AosIILPtr iil = loadIILByIDPublic(iilid, iiltype, false, false, rdata);
	aos_assert_r(iil, false);

	switch (iiltype)
	{
	case eAosIILType_Str:
		 {
			AosIILStrPtr iilstr = (AosIILStr*)iil.getPtr();
			bool rslt = iilstr->querySafe(query_rslt, query_bitmap, opr, value, rdata);
			returnIILPublic(iil, rdata, true);
			return rslt;
		 }
		 break;

	case eAosIILType_U64:
		 {
			AosIILU64Ptr iilu64 = (AosIILU64*)iil.getPtr();
			u64 vv = atoll(value.data());
			bool rslt = iilu64->querySafe(query_rslt, query_bitmap, opr, vv, rdata);
			returnIILPublic(iil, rdata, true);
			return rslt;
		 }
		 break;


	case eAosIILType_Hit:
		 {
			 AosIILHitPtr iilhit = (AosIILHit*)iil.getPtr();
			 bool rslt = iilhit->querySafe(query_rslt, query_bitmap, rdata);
			 returnIILPublic(iil, rdata, true);
			 return rslt;
		 }
		 break;

	default:
		 break;
	}

	rdata->setError() << "Iil type mismatch." << iiltype;
	OmnAlarm << rdata->getErrmsg() << enderr;
	returnIILPublic(iil, rdata, true);
	return false;
}


int		
AosIILMgr::getNumDocsSafe(
		const u64 &iilid, 
		const AosRundataPtr &rdata)
{
	AosIILType iiltype = eAosIILType_Invalid;
	AosIILPtr iil = loadIILByIDPublic(iilid, iiltype, false, false, rdata);
	if(!iil)
	{
		return 0;
	}
	int numdoc = iil->getNumDocsSafe();
	returnIILPublic(iil, rdata, true);	
	return numdoc;	
}			
			

bool	
AosIILMgr::nextDocidSafe(
		const u64 &iilid,
		int &idx, 
		int &iilidx,
		const bool reverse, 
		const AosOpr opr,
		u64 &value,
		u64 &docid, 
		bool &isunique, 
		const AosRundataPtr &rdata)
{
	AosIILType iiltype = eAosIILType_Invalid;
	AosIILPtr iil = loadIILByIDPublic(iilid,iiltype,false, false, rdata);
	if(!iil)
	{
		return false;
	}
	if(iiltype != eAosIILType_U64)
	{
		OmnScreen << "Iil type mismatch. expect to get a u64 iil:" << iiltype << endl;
		returnIILPublic(iil, rdata, true);	
		return false;
	}
	
	AosIILU64Ptr iilu64 = (AosIILU64*)iil.getPtr();
	bool rslt = iilu64->nextDocidSafe(idx, 
						iilidx,
						reverse, 
						opr,
						value,
						docid, 
						isunique,
						rdata);
	returnIILPublic(iil, rdata, true);	
	return rslt;
}						


bool	
AosIILMgr::nextDocidSafe(
		const u64 &iilid,
		int &idx, 
		int &iilidx,
		const bool reverse, 
		const AosOpr opr,
		const OmnString &value,
		u64 &docid, 
		bool &isunique, 
		const AosRundataPtr &rdata)
{
	AosIILType iiltype = eAosIILType_Invalid;
	AosIILPtr iil = loadIILByIDPublic(iilid, iiltype, false, false, rdata);
	if(!iil)
	{
		return false;
	}
	if(iiltype != eAosIILType_Str)
	{
		OmnScreen << "Iil type mismatch. expect to get a str iil:" << iiltype << endl;
		returnIILPublic(iil, rdata, true);	
		return false;
	}

	AosIILStrPtr iilstr = (AosIILStr*)iil.getPtr();
	bool rslt = iilstr->nextDocidSafe(idx, 
						iilidx,
						reverse, 
						opr,
						value,
						docid, 
						isunique,
						rdata);
	returnIILPublic(iil, rdata, true);	
	return rslt;
}						


bool	
AosIILMgr::nextDocidSafe(
		const u64 &iilid,
		int &idx, 
		int &iilidx,
		const bool reverse, 
		const AosOpr opr1,
		const OmnString &value1,
		const AosOpr opr2,
		const OmnString &value2,
		u64 &docid, 
		bool &isunique, 
		const AosRundataPtr &rdata)
{
	AosIILType iiltype = eAosIILType_Invalid;
	AosIILPtr iil = loadIILByIDPublic(iilid, iiltype, false, false, rdata);
	if(!iil)
	{
		return false;
	}

	if(iiltype != eAosIILType_Str)
	{
		OmnScreen << "Iil type mismatch. expect to get a str iil:" << iiltype << endl;
		returnIILPublic(iil, rdata, true);	
		return false;
	}

	AosIILStrPtr iilstr = (AosIILStr*)iil.getPtr();
	bool rslt = iilstr->nextDocidSafe(idx, 
						iilidx,
						reverse, 
						opr1,
						value1,
						opr2,
						value2,
						docid, 
						isunique,
						rdata);

	returnIILPublic(iil, rdata, true);	
	return rslt;
}						


bool	
AosIILMgr::nextDocidSafe(
		const u64 &iilid,
		int &idx, 
		int &iilidx,
		const bool reverse, 
		const AosOpr opr1,
		const u64 &value1,
		const AosOpr opr2,
		const u64 &value2,
		u64 &docid, 
		bool &isunique, 
		const AosRundataPtr &rdata)
{
	AosIILType iiltype = eAosIILType_Invalid;
	AosIILPtr iil = loadIILByIDPublic(iilid, iiltype, false, false, rdata);
	if(!iil)
	{
		return false;
	}
	if(iiltype != eAosIILType_U64)
	{
		OmnScreen << "Iil type mismatch. expect to get a u64 iil:" << iiltype << endl;
		returnIILPublic(iil, rdata, true);	
		return false;
	}
	
	AosIILU64Ptr iilu64 = (AosIILU64*)iil.getPtr();
	bool rslt = iilu64->nextDocidSafe(idx, 
						iilidx,
						reverse, 
						opr1,
						value1,
						opr2,
						value2,
						docid, 
						isunique,
						rdata);
	returnIILPublic(iil, rdata, true);	
	return rslt;
}						


u64 	
AosIILMgr::getNextEntrySafe(
		const u64 &iilid,
		int &idx, 
		int &iilidx, 
		const AosRundataPtr &rdata)
{
	AosIILType iiltype = eAosIILType_Invalid;
	AosIILPtr iil = loadIILByIDPublic(iilid, iiltype, false, false, rdata);
	if(!iil)
	{
		return AOS_INVDID;
	}
	if(iiltype != eAosIILType_Hit)
	{
		OmnScreen << "Iil type mismatch. expect to get a hit iil:" << iiltype << endl;
		returnIILPublic(iil, rdata, true);	
		return false;
	}

	AosIILHitPtr hitiil = (AosIILHit*)iil.getPtr();
	u64 ret = hitiil->getNextEntrySafe(idx,iilidx,rdata);

	returnIILPublic(iil, rdata, true);	
	return ret;
}


u64		
AosIILMgr::nextDocIdSafe2(
		const u64 &iilid,
		int &idx,
		int &iilidx,
		const bool reverse, 
		const AosRundataPtr &rdata)
{
	AosIILType iiltype = eAosIILType_Invalid;
	AosIILPtr iil = loadIILByIDPublic(iilid, iiltype, false, false, rdata);
	if(!iil)
	{
		return AOS_INVDID;
	}
	u64 ret = iil->nextDocIdSafe2(idx,
						   iilidx,
						   reverse);
	returnIILPublic(iil, rdata, true);	
	return ret;	
}						   


void			
AosIILMgr::exportIILStrSafe(
		const u64 &iilid,
		vector<OmnString> 	&values,
		vector<u64>			&docids,
		const AosRundataPtr &rdata)
{
	AosIILType type = eAosIILType_Str;
	AosIILPtr iil = loadIILByIDPublic(iilid, type, false, false, rdata);
	aos_assert(iil);
	if (iil->getIILType() != eAosIILType_Str)
	{
		returnIILPublic(iil, rdata, true);
		return;
	}
	
	AosIILStrPtr striil = (AosIILStr*)iil.getPtr();
	striil->exportIILSafe(values,docids,rdata);
	returnIILPublic(iil, rdata, true);
	return;
}


void			
AosIILMgr::exportIILU64Safe(
		const u64 &iilid,
		vector<u64> 		&values,
		vector<u64>			&docids,
		const AosRundataPtr &rdata)
{
	AosIILType type = eAosIILType_U64;
	AosIILPtr iil = loadIILByIDPublic(iilid, type, false, false, rdata);
	aos_assert(iil);
	if (iil->getIILType() != eAosIILType_U64)
	{
		returnIILPublic(iil, rdata, true);
		return;
	}
	
	AosIILU64Ptr striil = (AosIILU64*)iil.getPtr();
	striil->exportIILSafe(values,docids,rdata);
	returnIILPublic(iil, rdata, true);
	return;
}


bool
AosIILMgr::saveCompIIL(
		const u64 &iilid,
		const bool forcesave, 
		AosDocTransPtr &docTrans,
		const AosRundataPtr &rdata)
{
	AosIILType iiltype;
	AosIILPtr compiil = loadIILByIDPublic(iilid, iiltype, true, forcesave, rdata);
	if (compiil)
	{
		bool rslt = compiil->saveToFileSafe(forcesave, rdata);
		returnIILPublic(compiil, rdata);
		aos_assert_r(rslt, false);
	}
	return true;
}


AosIILPtr
AosIILMgr::createRootIILPublic(
		const u64 &iilid,
		const AosIILType iiltype,
		const bool isPersis,
		const AosRundataPtr &rdata)
{
	// This function creates the iil 'iilid'. If the IIL is already created, 
	// it is an error. If the IIL needs comp iil, it will create it. The
	// IILID should not be 0. 
	aos_assert_rr(iilid > 0, rdata, 0);

	AosIILType type = iiltype;
	bool isNumAlpha = false; 
	if (type == eAosIILType_NumAlpha)
	{                               
		type = eAosIILType_Str;     
		isNumAlpha = true;          
	}
	
	if (AosIIL::staticIsIILCreated(iilid, type, rdata))
	{
		// It has already been created. 
		OmnAlarm << "IIL already created: " << iilid << ":" << iiltype << enderr;
		return true;
	}

	AosIILPtr iil;
	try
	{
		iil = createNewIILPriv(0, iilid, type, isPersis, rdata);
		iil->setDirty(true);
		iil->setNew(true);
	}

	catch (...)
	{
		AosSetError(rdata, AosErrmsgId::eExceptionCreateIIL);
		OmnAlarm << rdata->getErrmsg() << ". IILID: " << iilid
			<< ". IILType: " << iiltype << enderr;
		return 0;
	}

	if (isNumAlpha)                                    
	{                                                 
		AosIILStr *striil = (AosIILStr *)iil.getPtr();
		striil->setNumAlpha();                        
	}

	AosIILPtr compiil; 
	if (iil->haveCompIIL())
	{
		try
		{
			compiil = createNewIILPriv(0, iilid, getCompType(type), isPersis, rdata);
			compiil->setDirty(true);
			compiil->setNew(true);
		}

		catch (...)
		{
			AosSetError(rdata, AosErrmsgId::eExceptionCreateIIL);
			OmnAlarm << rdata->getErrmsg() << ". IILID: " << iilid
				<< ". IILType: " << iiltype << enderr;
			return 0;
		}
		
		if (isNumAlpha)                                    
		{                                                 
			AosIILCompStr *compstriil = (AosIILCompStr *)iil.getPtr();
			compstriil->setNumAlpha();                        
		}
	}
	
	// Put the IILs into the queue
	AOSLMTR_ENTER(mLockMonitor);
	AOSLMTR_LOCK(mLockMonitor);
	iil->setCreationOnly(true);
	mIILIDHash[iil->getIILIDWithCompFlag()] = iil;
	if (compiil)
	{
		compiil->setCreationOnly(true);
		mIILIDHash[compiil->getIILIDWithCompFlag()] = compiil;
	}
	AOSLMTR_UNLOCK(mLockMonitor);
	AOSLMTR_FINISH(mLockMonitor);

	return iil;
}

void
AosIILMgr::appendDebugInfo(const OmnString &fname)
{
	sgSaveIILLogs << ":" << fname;
}


void
AosIILMgr::appendDebugInfo(const int line)
{
	sgSaveIILLogs << ":" << line;
}
*/

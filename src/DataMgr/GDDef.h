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
// 05/06/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_DataMgr_GDDef_h
#define Omn_DataMgr_GDDef_h

#include "Obj/ObjDb.h"
#include "Util/RCObjImp.h"
#include <string>
#include <list>


class TiXmlElement;

class AosGDDef : virtual public OmnDbObj
{
	OmnDefineRCObject;

	enum
	{
		eWaitInterval = 500000,
		eMaxLockTries = 20
	};

private:
	u64				mSeqno;
    OmnString 		mTablename;
    OmnString 		mName;
	OmnString		mTagging;
	OmnString		mDescription;
	OmnString		mKeyFields;
	OmnString		mTaggingFname;
	OmnString		mNameFname;
	OmnString		mDescFname;
	int				mMaxLimit;
	OmnString		mNoDelete;
	OmnString		mReadOnly;
	OmnString		mLog;
	OmnString		mStat;
	OmnString		mObjListing;
	OmnString		mIndexedFields;
	OmnString		mTranslators;
	OmnString		mArchive;
	OmnString		mCreateTime;
	OmnString		mModTime;
	OmnString		mReadLock;
	OmnString		mRLUser;
	OmnString		mRLTime;
	OmnString		mWriteLock;
	OmnString		mWLUser;
	OmnString		mWLTime;
	OmnString		mNewObjLock;
	OmnString		mNOLUser;
	OmnString		mNOLTime;

public:
	AosGDDef();
	AosGDDef(const OmnString &tablename);
	~AosGDDef();

	// OmnObjDb Interface
	virtual OmnRslt		serializeFromRecord(const OmnDbRecordPtr &record);
	virtual OmnString	retrieveStmt() const;
	virtual OmnString	insertStmt() const;
	virtual OmnString	updateStmt() const;
	virtual OmnString	removeStmt() const;
	virtual OmnString	removeAllStmt() const;
	virtual OmnString	existStmt() const;
	virtual int			objSize() const {return sizeof(*this);}

	bool		allowDelete(const OmnString &username) const;
	bool		isReadOnly() const {return mReadOnly == "true";}
	bool 		getGDDef(const OmnString &tablename);
	OmnString	getTablename() const {return mTablename;}
	OmnString	getReadLock() const {return mReadLock;}
	OmnString	getRLUser() const {return mRLUser;}
	OmnString	getRLTime() const {return mRLTime;}
	OmnString	getWriteLock() const {return mWriteLock;}
	OmnString	getWLUser() const {return mRLUser;}
	OmnString	getWLTime() const {return mRLTime;}
	OmnString	getNewObjLock() const {return mNewObjLock;}
	OmnString	getNOLUser() const {return mNOLUser;}
	OmnString	getNOLTime() const {return mNOLTime;}
	OmnString	getTaggingFname() const {return mTaggingFname;}
	OmnString	getNameFname() const {return mNameFname;}
	OmnString	getDescFname() const {return mDescFname;}
	OmnString	getIndexedFields() const {return mIndexedFields;}
	int			getTranslator(const OmnString &id, std::list<OmnString> &trans) const;
	OmnString	getKeyfields() const {return mKeyFields;}
	int			getMaxLimit() const {return mMaxLimit;}
	bool		maxReached() const;
	bool 		lockForReading(const OmnString &username, OmnString &errmsg);
	bool 		lockForWriting(const OmnString &username, OmnString &errmsg);
	bool 		lockForNewObj(const OmnString &username, OmnString &errmsg);
	bool 		unlockReading(const OmnString &username, OmnString &errmsg);
	bool 		unlockWriting(const OmnString &username, OmnString &errmsg);
	bool 		unlockNewObj(const OmnString &username, OmnString &errmsg);
	bool 		isLockedForReading(const OmnString &username);
	bool 		isLockedForWriting(const OmnString &username);
	bool 		isLockedForNewObj(const OmnString &username);
	bool 		modifyGroup(
					const OmnString &tablename,
					const OmnString &contents, 
					const bool unlock_write,
					OmnString &errmsg);
	bool 		createGroupRecord(
					const OmnString &tablename,
					TiXmlElement *node, 
					OmnString &errmsg);
	bool 		lockGroup(
					TiXmlElement *node, 
					const OmnString &username,
					OmnString &errmsg);
	OmnString	getObjListingFieldnames(const OmnString &id);
	OmnString	getObjListingOrder(const OmnString &id);

private:
	bool 		lockGroup(const OmnString &stmt);
	bool 		checkValue(const OmnString &fname, const OmnString &value);
};

#endif


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
#ifndef Omn_DataMgr_GDObj_h
#define Omn_DataMgr_GDObj_h

#include "DataMgr/Ptrs.h"
#include "Obj/ObjDb.h"
#include "Util/RCObjImp.h"
#include "XmlInterface/XmlRc.h"
#include <string>

class TiXmlElement;
class TiXmlDocument;

class AosGDObj : virtual public OmnDbObj
{
	OmnDefineRCObject;
	
	enum
	{
		eMaxIndexedFields = 50,
		eMaxPageSize = 50,
		eMaxFieldNames = 100,
		eMaxLockTries = 20,
		eWaitInterval = 500000
	};

private:
    OmnString 		mTablename;

    u64				mSeqno;
	OmnString		mObjid;
	OmnString		mName;
	OmnString		mTagging;
	OmnString		mDescription;
	OmnString		mCreator;
	OmnString		mCreateTime;
	OmnString		mModifier;
	OmnString		mModTime;
	OmnString		mXml;
	OmnString		mReadLock;
	OmnString		mRLUser;
	OmnString		mRLTime;
	OmnString		mWriteLock;
	OmnString		mWLUser;
	OmnString		mWLTime;

	char *			mIndexedFields[eMaxIndexedFields];
	OmnString		mIndexedValues[eMaxIndexedFields];
	int				mNumIndexedFields;
	TiXmlDocument *	mXmlDoc;
	TiXmlElement *	mXmlobj;

public:
	AosGDObj();
	~AosGDObj();

	// OmnObjDb Interface
	virtual OmnRslt		serializeFromRecord(const OmnDbRecordPtr &record);
	virtual OmnString	retrieveStmt() const;
	virtual OmnString	insertStmt() const;
	virtual OmnString	updateStmt() const;
	virtual OmnString	removeStmt() const;
	virtual OmnString	removeAllStmt() const;
	virtual OmnString	existStmt() const;
	virtual int			objSize() const {return sizeof(*this);}

	void		setCreator(const OmnString &c) {mCreator = c;}
	void		setModifier(const OmnString &c) {mModifier = c;}
	OmnString	getReadLock() const {return mReadLock;}
	OmnString	getRLUser() const {return mRLUser;}
	OmnString	getRLTime() const {return mRLTime;}
	OmnString	getWriteLock() const {return mWriteLock;}
	OmnString	getWLUser() const {return mWLUser;}
	OmnString	getWLTime() const {return mWLTime;}
	TiXmlElement *	getXmlobj();

	bool 	parse(
				const OmnString &tablename,
				const OmnString &contents,
				const AosGDDefPtr &def,
				OmnString &errmsg);
	bool 	setIndexedFields(const OmnString &indexedFields);
	int 	getTotalRecords(const OmnString &tablename);
	bool 	retrieveList(
				TiXmlElement *node, 
				const AosGDDefPtr &groupDef,
				AosXmlRc &errcode, 
				OmnString &errmsg, 
				OmnString &results);
	bool 	retrieveObjBySeqno(
				const OmnString &seqno, 
				const OmnString &tablename);
	bool	addObjectToDb();
	bool	deleteObjBySeqno(const OmnString &seqno);
	bool	isLockedForReading(const OmnString &username) const;
	bool	isLockedForWriting(const OmnString &username) const;
	bool 	lockForReading(const OmnString &username);
	bool 	lockForWriting(const OmnString &username);
	bool 	lockForReadWrite(const OmnString &username);
	bool 	unlockReading(const OmnString &username, OmnString &errmsg);
	bool 	unlockWriting(const OmnString &username, OmnString &errmsg);
	bool 	constructResponse( TiXmlElement *xmlobj, OmnString &response);
	bool 	getObjBySeqno(const OmnString &seqno, OmnString &errmsg);
	bool 	lockObj( TiXmlElement *node, const OmnString &username, OmnString &errmsg);
	bool 	modifyObject(
				const OmnString &username,
				const OmnString &contents, 
				const bool unlock_write,
				OmnString &errmsg);
	bool 	checkValue(const OmnString &fname, const OmnString &value);
	OmnString getCreateTableStmt(
				const OmnString &tablename, 
				const char *indexed_fields, 
				OmnString &errmsg);

private:
	bool 	lockObj(const OmnString &stmt);
	bool 	changeRecordForAdd();
};

#endif


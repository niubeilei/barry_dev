////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ObjDb.h
// Description:
//	A DbObj is an object that can be serialized to and from a database. 
//  The database can be any type, such as MySQL, Oracle, etc. 
//  
//  This class defines an interface. Any object that wants to be persistent
//  in databases should derive from this class.    
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Obj_DbObj_h
#define Omn_Obj_DbObj_h

#include "Database/Ptrs.h"
#include "DataStore/StoreId.h"
#include "DataStore/Ptrs.h"
#include "Debug/Rslt.h"
#include "Util/Object.h"
#include "Obj/Ptrs.h"
#include "Util1/Time.h"


class OmnDbObj : virtual public OmnObject
{
public:
	enum DbStatus
	{
		eInactive,
		eActive,
		eLock,

		eUnknown,
	};

protected:
	OmnStoreId::E	mStoreId;
	u64				mTimeCreated;
	u64				mTimeAccessed;

public:
	OmnDbObj(const OmnClassId::E classId = OmnClassId::eOmnUnknown, 
			 const OmnStoreId::E storeId = OmnStoreId::eGeneral) 
		:
	OmnObject(classId),
	mStoreId(storeId)
	{
	}

	virtual ~OmnDbObj() {}

	// 
	// OmnDbObj interface
	//
	virtual OmnRslt		serializeFromRecord(const OmnDbRecordPtr &record);
	virtual OmnString	insertStmt() const;
	virtual OmnString	updateStmt() const;
	virtual OmnString	removeStmt() const;
	virtual OmnString	retrieveStmt() const;
	virtual OmnString	removeAllStmt() const;
	virtual OmnString	existStmt() const;
	virtual OmnRslt		updateToDb(const OmnString &stmt) const;

	virtual OmnRslt		serializeFromDb();
    virtual OmnRslt		serializeFromDb(const OmnString &stmt);

	OmnString		toString() const;
	OmnDataStorePtr	getStore() const;

	bool		existInDb() const {return existInDb(this->existStmt());}
	virtual OmnRslt		addToDb() {return addToDb(this->insertStmt());}
	bool		saveToDb() const {return addToDb(this->insertStmt());}
	OmnRslt		removeFromDb() const {return removeFromDb(this->removeStmt());}
	OmnRslt		removeAllFromDb() const {return removeFromDb(this->removeAllStmt());}
	OmnRslt		updateToDb() const {return updateToDb(this->updateStmt());}
	OmnDbRecordPtr	getRecord(const OmnString &stmt);

	bool		existInDb(const OmnString &stmt) const;
	OmnRslt		addToDb(const OmnString &stmt) const;
	OmnRslt		removeFromDb(const OmnString &stmt) const;
	u64			getCreationTime() const {return mTimeCreated;}
	u64			getAccessTime() const {return mTimeAccessed;}
	bool		retrieveFromDb();
	
	void		objAccessed() {mTimeAccessed = OmnTime::getSecTick();}
	OmnRslt		retrieveRecord(const OmnString &stmt, OmnDbRecordPtr &record) const;
	OmnRslt		retrieveRecords(const OmnString &stmt, OmnDbTablePtr &table) const;
	OmnRslt		serializeFromDb(OmnDbObjPtr &obj);

};
#endif

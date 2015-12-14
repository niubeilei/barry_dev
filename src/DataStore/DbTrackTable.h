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
// 	Created: 12/19/2008 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataStore_DbTrackTable_h
#define Aos_DataStore_DbTrackTable_h

#include "Database/Ptrs.h"
#include "Debug/Debug.h"
#include "Obj/ObjDb.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"


class AosDbTrackTable : virtual public OmnDbObj
{
	OmnDefineRCObject;

public:
	enum E
	{
		eTypeMin = 0, 
		eInvalidType = 'I',

		eAdd = 'A',
		eDelete = 'D', 
		eModify = 'M',

		eTypeMax = 250
	};

private:
	u32			mSeqno;
	E			mType;
	OmnString	mTableName;
	OmnString	mXml;

public:
	AosDbTrackTable();
	AosDbTrackTable(const AosDbTrackTable::E type, 
			const OmnString &tablename); 
	virtual ~AosDbTrackTable();

	static int	pollChanges(const u32 snappoint, const OmnString &tname);

	// OmnObjDb Interface
	virtual OmnRslt		serializeFromRecord(const OmnDbRecordPtr &record);
	virtual OmnString	retrieveStmt() const;
	virtual OmnString	insertStmt() const;
	virtual OmnString	updateStmt() const;
	virtual OmnString	existStmt() const;
	virtual OmnString	removeStmt() const;
	virtual OmnString	removeAllStmt() const;
	virtual int			objSize() const {return sizeof(*this);}

	void	startAssemble();
	void	endAssemble();
	void	addField(const OmnString &fname, const char value);
	void	addField(const OmnString &fname, const int value);
	void	addField(const OmnString &fname, const u32 value);
	void	addField(const OmnString &fname, const OmnString &value, const bool);
};

#endif


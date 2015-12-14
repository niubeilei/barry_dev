////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: DbTableSQLite.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_SQLite_DbTableSQLite_h
#define Omn_SQLite_DbTableSQLite_h

#include "Database/DbTable.h"
#include "Util/RCObjImp.h"
#include "Util/BasicTypes.h"
#include "Porting/SQLiteInclude.h"


class OmnDbTableSQLite : public OmnDbTable
{
	OmnDefineRCObject;

private:
	char		 **	mDbRslt;
	int				mCrtIndex;
	int				mNumFields;
    int             mNumRows;
	bool			mDeleteFlag;

public:
	OmnDbTableSQLite(char **dbrslt, int numRows, int numFields)
	:
	mDbRslt(dbrslt),
	mNumFields(numFields),
	mNumRows(numRows),
	mDeleteFlag(true)
	{
	}
	virtual ~OmnDbTableSQLite();

    virtual void            reset();
    virtual bool            hasMore() const;
    virtual OmnDbRecordPtr  next();
    virtual int             entries() const;
	void 					setFlag(const bool b) {mDeleteFlag = b;}
};
#endif


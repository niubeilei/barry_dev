////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: DbRcdMySql.h
// Description:
//	This is the database record for MySQL.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Database_DbRcdMySql_h
#define Omn_Database_DbRcdMySql_h

#include "aosUtil/Types.h"
#include "Database/DbRecord.h"
#include "Database/Ptrs.h"
#include "Util/RCObjImp.h"
#include "util_c/types.h"
#include "Porting/MySqlInclude.h"



class OmnDbRcdMySql : public OmnDbRecord 
{
	OmnDefineRCObject;

private:
	MYSQL_ROW 			mRow;
	int					mNumFields;
	OmnDbTablePtr		mTable;			// The table the record belongs

public:
	OmnDbRcdMySql(MYSQL_ROW row, 
				  const int numFields,
				  const OmnDbTablePtr &table)
	:
	mRow(row),
	mNumFields(numFields),
	mTable(table)
	{
	}

	virtual ~OmnDbRcdMySql();
	
	virtual int				getNumFields() const {return mNumFields;}
	virtual OmnString		getStr(const int index, const OmnString &d, OmnRslt &) const;
	virtual char			getChar(const int index, const char d, OmnRslt &) const;
	virtual bool			getBool(const int index, const bool d, OmnRslt &) const;
	virtual int				getInt(const int index, const int d, OmnRslt &) const;
	virtual u32             getU32(const int index, const u32 d, OmnRslt &) const;
	virtual int64_t			getInt64(const int index, const int64_t &d, OmnRslt &) const;
	virtual u64				getU64(const int index, const u64 &d, OmnRslt &) const;
	virtual double			getDouble(const int index, const double d, OmnRslt &) const;
	virtual OmnIpAddr		getAddr(const int index, const OmnString &, OmnRslt &) const;
	virtual OmnDateTime		getDateTime(const int index, OmnRslt &) const;
	virtual void			getBinary(const int index, 
								char *buff, 
								const uint size, 
								OmnRslt &rslt) const;
	virtual OmnString		toString() const;

	// Chen Ding, 06/09/2011
	virtual bool getValue(const int idx, const OmnString &, OmnString &)const;
};
#endif


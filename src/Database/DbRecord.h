////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: DbRecord.h
// Description:
//	A DbRecord is a list of memory:   
//
// Modification History:
// 04/28/2008 Added the member function 'setFlag()' to solve the problem
// 			  that when query a single record, the table may be deleted
// 			  when returning the record, causing memory corruption. To
// 			  solve this problem, we set a flag. If querying a single
// 			  record, we will delegate the deletion of the memory to 
// 			  the record. See the implementation for SQLite as examples.
// 			  As of this writing, MySQL was not changed yet (need to change).
// 			  Chen Ding 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Database_DbRecord_h
#define Omn_Database_DbRecord_h

#include "aosUtil/Types.h"
#include "Database/Ptrs.h"
#include "Debug/Rslt.h"
#include "Porting/LongTypes.h"
#include "Util/RCObject.h"
#include "Util/String.h"
#include "Util/BasicTypes.h"
#include "Util/IpAddr.h"
#include "Util1/DateTime.h"



class OmnDbRecord : public OmnRCObject
{
public:
	OmnDbRecord();
	virtual ~OmnDbRecord();

	virtual int				getNumFields() const = 0;
	virtual OmnString		getStr(const int, const OmnString &d, OmnRslt &) const = 0;
	virtual char			getChar(const int, const char d, OmnRslt &) const = 0;
	virtual bool			getBool(const int index, const bool d, OmnRslt &) const = 0;
	virtual int				getInt(const int index, const int d, OmnRslt &) const = 0;
	virtual u32				getU32(const int index, const u32 d, OmnRslt &) const = 0;
	virtual int64_t			getInt64(const int, const int64_t &d, OmnRslt &) const = 0;
	virtual u64				getU64(const int index, const u64 &d, OmnRslt &) const = 0;
	virtual double			getDouble(const int index, const double, OmnRslt &) const = 0;
	virtual OmnIpAddr		getAddr(const int, const OmnString &, OmnRslt &) const = 0;
	virtual OmnDateTime		getDateTime(const int, OmnRslt &) const = 0;
	virtual void			getBinary(const int index, 
								char *buff, 
								const uint size, 
								OmnRslt &rslt) const = 0;
	virtual OmnString		toString() const = 0;

	// Chen Ding, 04/28/2008
	virtual void			setFlag(const bool b) {}

	// Chen Ding, 06/09/2011
	virtual bool getValue(const int idx, const OmnString &type, OmnString &value) const = 0;

	static bool retrieveRecord(const OmnString &stmt, OmnDbRecordPtr &record);
};
#endif


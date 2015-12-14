////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: DbTable.h
// Description:
//   
//
// Modification History:
// 04/28/2008 Modified by Chen Ding (see the comments in DbRecord.h). 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Database_DbTable_h
#define Omn_Database_DbTable_h

#include "Database/Ptrs.h"
#include "Util/RCObject.h"



class OmnDbTable : public OmnRCObject
{
public:
	OmnDbTable();
	virtual ~OmnDbTable();

	virtual void			reset() = 0;
	virtual bool			hasMore() const = 0;
	virtual OmnDbRecordPtr	next() = 0;
	virtual OmnDbRecordPtr	next2() = 0;
	virtual OmnDbRecordPtr	getRecord(const int idx) = 0;
	virtual int				entries() const = 0;

	// Chen Ding, 04/28/2008
	virtual void			setFlag(const bool b) = 0;
};
#endif


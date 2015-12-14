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
// 06/29/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_TorturerCond_CondCheckTable_h
#define Omn_TorturerCond_CondCheckTable_h

#include "TorturerConds/Cond.h"
#include "Util/String.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"

typedef bool (*AosTableCheckFunc)(const AosGenRecordPtr &record,
                             void *data,
							 const int index);

class AosTortCondCheckTable : virtual public AosTortCond
{
	OmnDefineRCObject;

protected:
	AosGenTablePtr		mTable;
	AosTableCheckFunc	mFunc;
	void *				mData;
	int					mNumEntries;

public:
	AosTortCondCheckTable(const AosGenTablePtr &table, 
						  AosTableCheckFunc func, 
						  void *data, 
						  const int len);
	~AosTortCondCheckTable();

	virtual bool		check(const AosCondData &data, 
				  		  bool &rslt, 
				    	  OmnString &errmsg) const;
	virtual CheckTime 	getCheckTime() const {return eCheckAfterExecution;}
};

#endif


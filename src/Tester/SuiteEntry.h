////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SuiteEntry.h
// Description:
//	A SuiteEntry is a collection of Tester.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef OMN_Tester_SuiteEntry_h
#define OMN_Tester_SuiteEntry_h

#include "Obj/ObjDb.h"
#include "Tester/Ptrs.h"
#include "Util/ValList.h"
#include "Util/RCObjImp.h"



/*
class OmnSuiteEntry : public OmnDbObj
{
	OmnDefineRCObject;

private:
	OmnString		mSuiteName;
	OmnString		mPackageName;

public:
	OmnSuiteEntry()
		:
	OmnDbObj(OmnStoreId::eTest)
	{
	}

	OmnSuiteEntry(const OmnString &suiteId, 
				  const OmnString &packageName = "noname")
		:
	OmnDbObj(OmnStoreId::eTest),
	mSuiteName(suiteId),
	mPackageName(packageName)
	{
	}

	virtual ~OmnSuiteEntry()
	{
	}

	// 
	// OmnDbObj interface
	//
    virtual OmnRslt serializeFromDb();
	OmnString	insertStmt() const;
	OmnString	removeStmt() const;

	virtual OmnString	toString() const;
	OmnRslt		retrieveEntriesForSuite(const OmnString &suiteid, 
										OmnSmartList<OmnTestPkgPtr> &packages) const;

private:
	OmnString	sqlAllFields() const;
};
*/

#endif

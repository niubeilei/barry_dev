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
// The super class for all container type semantics data. 
//
// Modification History:
// 11/27/2007: Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SemanticData_VarContainer_h
#define Aos_SemanticData_VarContainer_h

#include "aosUtil/Types.h"
#include "RVG/Ptrs.h"
#include "SemanticData/Var.h"
#include "SemanticData/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Util/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"

#include <list>


class AosVarContainer : virtual public AosVar
{
	OmnDefineRCObject;

private:
	OmnMutexPtr					mLock;
	std::list<AosVarPtr>		mVars;
	std::list<AosTablePtr>		mTables;

public:
	AosVarContainer(const std::string &name);
	~AosVarContainer();

	AosVarPtr		getVar(const std::string &name);
	AosValuePtr		getValue(const std::string &name);
	AosTablePtr 	getTable(const std::string &name);	
	AosClassObjPtr 	getClassObj(const std::string &name);

	bool		exist(const std::string &name) const;

	bool		addVar(const AosVarPtr &);
	bool		addTable(const AosTablePtr &);

	bool		isTableEmpty(const std::string &name) const;
	virtual AosVarType::E	getType() const;
};
#endif


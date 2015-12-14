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
// 01/10/2008: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_ObjSet_ObjSetBasic_h
#define Aos_ObjSet_ObjSetBasic_h

#include "aosUtil/Types.h"
#include "ObjSet/ObjSet.h"
#include "ObjSet/ObjSetUtil.h"
#include "RVG/Ptrs.h"
#include "Util/RCObjImp.h"


class AosObjSetBasic : public AosObjSet
{
	OmnDefineRCObject;

public:
	enum
	{
		eDefaultDeleteFlag = 0,
		eDefaultModifyFlag = 0,
		eDefaultRecreateFlag = 0
	};

private:
	std::string				mClassName;
	AosRecordCreateFunc		mRecordCreator;
	u32						mMax;
	AosTablePtr				mTable;
	u8						mDeleteFlag;
	u8						mRecreateFlag;
	u8						mModifyFlag;

public:
	AosObjSetBasic(const std::string &className, 
				   AosRecordCreateFunc creationFunc, 
				   const u32 max);
	virtual ~AosObjSetBasic();

	virtual AosEventRc::E	procEvent(const AosEventPtr &event);

private:
	AosEventRc::E objCreated(const AosEventObjCreatedPtr &event);
	AosEventRc::E objDeleted(const AosEventObjDeletedPtr &event);
	AosEventRc::E objRenamed(const AosEventObjRenamedPtr &event);
	AosEventRc::E objModified(const AosEventObjModifiedPtr &event);
	AosEventRc::E objRecreated(const AosEventObjRecreatedPtr &event);
};

#endif


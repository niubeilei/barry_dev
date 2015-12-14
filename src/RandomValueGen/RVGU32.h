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
// 02/18/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_RandomValueGen_RVGU32_h
#define Omn_RandomValueGen_RVGU32_h

#include "Parms/RVG.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "Util/DynArray.h"



class AosRVGU32 : public AosRVG
{
	OmnDefineRCObject;
public:
	enum
	{
	};

private:
	u32			mMin;
	u32			mMax;
	OmnString	mBadValue;
	bool		mInsertFlag;

	u32			mCorrectPct;

public:
	AosRVGU32();
	AosRVGU32(const OmnString &name,
	        const u32 fieldIndex, 
			const AosRVGKeyType keyType,
			const u32 min,
			const u32 max,
			const bool insertFlag,
			const u32 badValue);
	virtual ~AosRVGU32();

	virtual OmnString nextValue(const AosGenTablePtr &data, 
				bool &isCorrect, 
				OmnString &errmsg);
	virtual OmnString nextValue(const AosGenRecordPtr &data, 
				bool &isCorrect, 
				OmnString &errmsg);
	virtual OmnString nextValue(bool &isCorrect, OmnString &errmsg);
	virtual OmnString getNextValueFuncCall() const;

	OmnString 	nextExist(const AosGenTablePtr &data, 
					bool &isCorrect, 
					OmnString &errmsg);
	OmnString 	nextNew(const AosGenTablePtr &data, 
					bool &isCorrect, 
					const bool insertFlag,
					OmnString &errmsg);
	OmnString 	nextNotKey(const AosGenRecordPtr &data, 
					bool &isCorrect, 
					OmnString &errmsg);

private:
};

#endif


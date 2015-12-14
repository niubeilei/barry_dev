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
// 07/07/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_RandomValueGen_RVGStrPair_h
#define Omn_RandomValueGen_RVGStrPair_h

#include "Random/Ptrs.h"
#include "Parms/RVGStr.h"
#include "TorturerConds/Util.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "Util/ValList.h"
#include "Util/DynArray.h"


class AosRVGStrPair : public AosRVGStr
{
	OmnDefineRCObject;
	enum
	{
		eDefaultUseSingleValuePct = 20
	};

private:
	OmnString		mSeparator;
	OmnString		mPairType;
	int64_t			mMinValue;
	int64_t 		mMaxValue;
	AosTortCondOpr	mOperator;
	int				mUseSingleValuePct;

public:
	AosRVGStrPair(const AosCliCmdTorturerPtr &cmd);
	virtual ~AosRVGStrPair();

	static AosRVGStrPairPtr createInstance(
									const AosCliCmdTorturerPtr &cmd,
									const OmnXmlItemPtr &def, 
									const OmnString &cmdTag,
									OmnVList<AosGenTablePtr> &tables);
	virtual bool 	config(const OmnXmlItemPtr &def, 
							const OmnString &cmdTag,
							OmnVList<AosGenTablePtr> &tables);
	virtual AosRVGType getRVGType() const {return eAosRVGType_StrPair;}
	virtual OmnString genValue();
private:
};

#endif


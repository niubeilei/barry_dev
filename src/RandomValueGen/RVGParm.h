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
// 07/08/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_RandomValueGen_RVGParm_h
#define Omn_RandomValueGen_RVGParm_h

#include "Random/Ptrs.h"
#include "Parms/RVG.h"
#include "TorturerConds/Util.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "Util/ValList.h"
#include "Util/DynArray.h"


class AosRVGParm : public AosRVG
{
	OmnDefineRCObject;

	enum
	{
		eDefaultParmWeight = 50,
		eMaxParms = 50
	};

	enum Method
	{
		eInvalidMethod,

		eSingleValue
	};

private:
	OmnDynArray<AosRVGPtr>	mParms;
	Method					mMethod;
	OmnString				mBadValue;
	AosRandomIntegerPtr		mParmSelector;
	u16						mWeights[eMaxParms];
	AosRVGPtr				mLastParm;

public:
	AosRVGParm(const AosCliCmdTorturerPtr &cmd);
	virtual ~AosRVGParm();

	static AosRVGParmPtr createInstance(
							const AosCliCmdTorturerPtr &cmd,
							const OmnXmlItemPtr &def, 
							const OmnString &cmdTag,
							OmnVList<AosGenTablePtr> &tables);
	virtual bool 	config(const AosCliCmdTorturerPtr &cmd, 
							const OmnXmlItemPtr &def, 
							const OmnString &cmdTag,
							OmnVList<AosGenTablePtr> &tables);
	virtual AosRVGType getRVGType() const {return eAosRVGType_RVGParm;}
	virtual OmnString 	getNextValueFuncCall() const;
	virtual bool 		newRecordAdded(const AosGenRecordPtr &newRecord) const;
	virtual bool 		nextStr(
							OmnString &value,
							const AosGenTablePtr &data, 
							const AosGenRecordPtr &record, 
							bool &isCorrect, 
							const bool correctOnly,
							const bool selectFromRecord,
							AosRVGReturnCode &rcode,
							OmnString &errmsg);
	virtual bool 		getCrtValueAsArg(OmnString &value, 
						    OmnString &decl, 
						    const u32 argIndex) const;
	
	Method		getMethod(const OmnString &m) const;

private:
	bool 		createSelector();
	bool 		nextSingleStr(OmnString &value,
					 const AosGenTablePtr &table,
					 const AosGenRecordPtr &record,
					 bool &isCorrect, 
					 const bool correctOnly,
					 const bool selectFromRecord,
					 AosRVGReturnCode &rcode,
					 OmnString &errmsg);
};

#endif


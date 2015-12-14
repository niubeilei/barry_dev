////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2013/10/25 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_StructProc_StructProc_h
#define Aos_StructProc_StructProc_h

#include "Util/DataTypes.h"
#include "Jimo/Jimo.h"
#include "Jimo/Ptrs.h"
#include "DataStructs/Ptrs.h"
#include "Util/ValueRslt.h"
#include "SEInterfaces/StructProcObj.h"

#include <vector>

class AosStructProc : public AosStructProcObj 
{
	OmnString				mType;

public:
	enum
	{
		eMinDftValue = 0xff
	};

protected:
	vector<AosValueRslt>	mOldValueRslt;
	vector<AosValueRslt>	mNewValueRslt;
	int						mDftValue;

public:
	AosStructProc();
	AosStructProc(const OmnString &type, const int version); 
	~AosStructProc();

	virtual AosJimoPtr cloneJimo()  const{OmnNotImplementedYet; return NULL;};

	virtual void config(const AosXmlTagPtr &conf);

	virtual bool setFieldValue(
			char *array,
			const int64_t data_len,
			const int pos,
			const int64_t &value,
			AosDataType::E data_type,
			const u64 &stat_id);

	virtual vector<AosValueRslt> & getNewValues();
	virtual vector<AosValueRslt> & getOldValues();

	virtual void clearValuesRslt();

	virtual int64_t calculateGeneralValue(const int64_t &new_value, const int64_t &old_value);

	virtual int getDftValue();

	static OmnString getJimoClassname(const OmnString &type);


};
#endif


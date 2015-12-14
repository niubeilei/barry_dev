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
// Modification History:
// 2013/12/18 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataStructs_Jimos_StructProcMin_h
#define Aos_DataStructs_Jimos_StructProcMin_h

#include "DataStructs/StructProc.h"
#include "Rundata/Ptrs.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"

class AosStructProcMin : public AosStructProc
{
	OmnDefineRCObject;
	
	int  			mVersion;
	char *			mInvalidStatValue;
public:
	AosStructProcMin(const int version);

	AosStructProcMin(const AosStructProcMin &proc);

	~AosStructProcMin();

	virtual AosJimoPtr cloneJimo()  const;

	virtual void config(const AosXmlTagPtr &conf);

	virtual bool setFieldValue(
			char *array,
			const int64_t data_len,
			const int pos,
			const int64_t &value,
			AosDataType::E data_type,
			const u64 &statid);

	virtual int64_t calculateGeneralValue(const int64_t &new_value, const int64_t &old_value);

	virtual int getDftValue(){return mDftValue;}
};
#endif




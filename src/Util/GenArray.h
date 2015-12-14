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
// AosGenArray contains an array of OmnStrings, integers, and/or
// AosGenTables. 
// Each of them serves as a data field. Each data field is accessed
// through indices. When creating the instance, one tells how many
// each type of fields is. Each instance of this class serves as a record. 
//
// Modification History:
// 2014/01/03: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Util_GenArray_h
#define Util_GenArray_h

#include "aosUtil/Types.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/DynArray.h"


class AosGenArray : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	AosBuffPtr		mBuff;
	AosDataType::E	mDataType;
	int				mNumElems;

public:
	AosGenArray();
	AosGenArray(const AosDataType::E type, 
			const char *data, 
			const int len);
	~AosGenArray();

	bool getElem(const int idx, AosValueRslt &value);
};

#endif


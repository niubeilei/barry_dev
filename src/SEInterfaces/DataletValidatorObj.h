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
// 2014/09/01 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEInterfaces_DataletValidatorObj_h
#define AOS_SEInterfaces_DataletValidatorObj_h

#include "Jimo/Jimo.h"
#include "Jimo/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"

class AosDataRecord;


class AosDataletValidatorObj : public AosJimo
{
protected:

public:
	AosDataletValidatorObj(const int version);
	~AosDataletValidatorObj();

	virtual bool validate(AosRundata *rdata, 
						AosDataRecord *record,
						AosDataRecord *container) = 0;

	static AosDataletValidatorObjPtr createValidator(
						AosRundata *rdata, 
						const OmnString &classname, 
						const int version);
};
#endif


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
// 2015/01/29 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Container_Column_h
#define AOS_Container_Column_h

#include "JQLStatement/Ptrs.h"

#include "SEInterfaces/ColumnObj.h"


class AosColumn : public AosColumnObj
{
	OmnDefineRCObject;

private:
	AosContainerPtr	mTable;

public:
	AosColumn();
	~AosColumn();

	bool			isFixed() const;
	OmnString		getName() const;
	AosDataType::E	getDataType() const;
	OmnString		getAlias() const;
	bool			isNull() const;
	bool			getValue(AosRundata *rdata, AosValueRslt &value);
	int				getIndex() const;
	bool			isKey() const;
};
#endif


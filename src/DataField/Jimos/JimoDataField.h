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
// 2013/07/28 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef Aos_DataField_Jimos_JimoDataField_h
#define Aos_DataField_Jimos_JimoDataField_h

#include "DataField/DataField.h"


class AosJimoDataField : public AosJimo, public AosDataField
{
public:
	AosJimoDataField(const int version);
	~AosJimoDataField();
	virtual AosDataFieldObjPtr clone(const AosRundataPtr &rdata) const;

	// AosJimo Interface
	virtual AosJimoPtr cloneJimo() const;

	virtual bool serializeTo(
						const AosRundataPtr &rdata, 
						const AosBuffPtr &buff);

	virtual bool serializeFrom(
						const AosRundataPtr &rdata, 
						const AosBuffPtr &buff);

	virtual bool config(
						const AosXmlTagPtr &def,
						const AosDataRecordObjPtr &record,
						const AosRundataPtr &rdata);
};

#endif
#endif

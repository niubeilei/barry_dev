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
// 2012/12/31 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Groupby_GroupFieldU64_h
#define Aos_Groupby_GroupFieldU64_h

#include "Groupby/GroupField.h"


class AosGroupFieldU64 : public AosGroupField
{
	bool		mHasValidValue;
	u64			mCrtValue;
	u64			mPrevValue;

public:
	AosGroupFieldU64();
	~AosGroupFieldU64();
	
	virtual bool	updateData(
						const AosDataRecordObjPtr &record, 
						bool &updated, 
						const AosRundataPtr &rdata);
	virtual bool 	setRecordByPrevValue(
						const AosDataRecordObjPtr &record, 
						const AosRundataPtr &rdata);
	virtual bool	saveAndClear();

	static AosGroupFieldPtr	create(
						const AosXmlTagPtr &def,
						const AosRundataPtr &rdata);

private:
	bool			config(
						const AosXmlTagPtr &def,
						const AosRundataPtr &rdata);

	bool 			getU64Priv(
						u64 &value,
						const AosDataRecordObjPtr &record,
						const AosRundataPtr &rdata);
};

#endif

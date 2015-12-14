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
// 2012/02/23	Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SdocAction_ActSnapShot_h
#define Aos_SdocAction_ActSnapShot_h

#include "Actions/SdocAction.h"
#include "Actions/Ptrs.h"
#include "IILUtil/IILUtil.h"
#include "Rundata/Ptrs.h"
#include "TransUtil/Ptrs.h"



class AosActSnapShot : virtual public AosSdocAction 
{
private:
	OmnString					mSnapShotType;
	OmnString					mModeId;
	OmnString					mKey;
	vector<u64>					mFileIds;
	vector<int>					mPhysicalIds;


public:
	AosActSnapShot(const bool flag);
	AosActSnapShot(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	~AosActSnapShot();

	virtual bool run(
			const AosTaskObjPtr &task, 
			const AosXmlTagPtr &sdoc,
			const AosRundataPtr &rdata);
	
	virtual AosActionObjPtr clone(
			const AosXmlTagPtr &def,
			const AosRundataPtr &rdata) const;

private:
	bool 	config(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	bool	createIILSnapShot(const AosRundataPtr &rdata);
	bool	createDocSnapShot(const AosRundataPtr &rdata);
	bool	commitSnapShot(const AosRundataPtr &rdata);
	bool	deleteFiles(const AosRundataPtr &rdata);
};
#endif


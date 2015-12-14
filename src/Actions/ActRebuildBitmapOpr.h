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
#ifndef Aos_SdocAction_ActRebuildBitmapOpr_h
#define Aos_SdocAction_ActRebuildBitmapOpr_h

#include "Actions/SdocAction.h"
#include "Actions/Ptrs.h"
#include "SEInterfaces/ActionCaller.h"
#include "IILUtil/IILUtil.h"
#include "Thread/Ptrs.h"
#include "Thread/ThrdShellProc.h"
#include "Rundata/Ptrs.h"
#include "TransUtil/Ptrs.h"



class AosActRebuildBitmapOpr : virtual public AosSdocAction 
{
private:
	enum
	{
		eMaxBuffSize = 100000000  //100M
		//eMaxBuffSize = 10000000  //10M
	};

	OmnString					mIILName;
	AosRundataPtr				mRundata;

public:
	AosActRebuildBitmapOpr(const bool flag);
	AosActRebuildBitmapOpr(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	~AosActRebuildBitmapOpr();


	virtual bool run(
			const AosTaskObjPtr &task, 
			const AosXmlTagPtr &sdoc,
			const AosRundataPtr &rdata);
	
	virtual AosActionObjPtr clone(
			const AosXmlTagPtr &def,
			const AosRundataPtr &rdata) const;
private:
};
#endif


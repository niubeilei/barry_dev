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
// 03/09/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_CommandProc_CommandProc_h
#define Aos_CommandProc_CommandProc_h

#include "SEInterfaces/CommandProcObj.h"

class AosCommandProc : public AosCommandProcObj
{
private:
	OmnString		mJimoName;

protected:
	OmnString			mName;

public:
	AosCommandProc(); 
	AosCommandProc(
		const AosRundataPtr &rdata,
		const u32 version, 
		const OmnString &name);
	virtual ~AosCommandProc(){}

	// AosJimo Interface
	virtual OmnString toString() const;
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


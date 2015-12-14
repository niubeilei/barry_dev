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
// 2013/05/26 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DynTemplate_DynTemplate_h
#define Aos_DynTemplate_DynTemplate_h

#include "Rundata/Rundata.h"
#include "SEInterfaces/DLLActionObj.h"
#include "Util/ValueRslt.h"

class AosDLLActionDynTemplate : public AosDLLActionObj
{
public:
	AosDLLActionDynTemplate(
			const AosRundataPtr &rdata, 
			const AosXmlTagPtr &sdoc, 
			const OmnString &libname,
			const OmnString &method,
			const OmnString &version);
	~AosDLLActionDynTemplate();

	// Action interface
	virtual bool serializeTo(AosBuffPtr &buff, const AosRundataPtr &rdata);
	virtual bool serializeFrom(const AosBuffPtr &buff, const AosRundataPtr &rdata);
	virtual bool abortAction(const AosRundataPtr &rdata);
	virtual bool run(const AosXmlTagPtr &def, const AosRundataPtr &rundata);
	virtual bool run(const AosValueRslt &value, const AosRundataPtr &rdata);
	virtual bool run(const AosDataRecordObjPtr &record, const AosRundataPtr &rdata);
	virtual bool run(const AosDataBlobObjPtr &blob, const AosRundataPtr &rdata);
	virtual bool run(const char *data, const int len, const AosRundataPtr &rdata);
	virtual bool run(const AosBuffPtr &buff, const AosRundataPtr &rdata);
	virtual bool run(const AosTaskObjPtr &task, 
					const AosXmlTagPtr &sdoc,
					const AosTaskDataObjPtr &def, 
					const AosRundataPtr &rundata);

private:
	bool config( 		const AosRundataPtr &rdata,
						const AosXmlTagPtr &def);
};
#endif

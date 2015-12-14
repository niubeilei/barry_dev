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
// 2015/03/13 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_IDO_ClusterIdo_h
#define Aos_IDO_ClusterIdo_h

#include "SEInterfaces/JimoCaller.h"
#include "UtilData/RepPolicy.h"

#include "JimoCall/Ptrs.h"
#include "SEInterfaces/Ptrs.h"


class AosRundata;

class AosClusterIDO : public AosJimoCaller
{
public:
	virtual bool getCubeEndpointIDs(AosRundata *rdata, 
							const u64 docid, 
							const OmnString &objid, 
							vector<int> &endpoint_ids, 
							const AosRepPolicy &replic_policy) const = 0;
	virtual OmnString getClusterName() const = 0;
	virtual u64 nextJimoCallID(AosRundata *rdata) = 0;
	virtual bool eraseJimoCall(AosRundata *rdata, const u64 call_id) = 0;
	virtual bool makeJimoCall(AosRundata *rdata, 
							const int endpoint_id,
							const AosJimoCallPtr &jimo_call, 
							AosBuff *buff) = 0;
	virtual bool modifyCubeAllocation(
								AosRundata *rdata, 
								const int cube_id, 
								const vector<int> &endpoint_ids) = 0;

	virtual bool moveCube(AosRundata *rdata, 
								const int cube_id,
								const int from_endpoint_id,
								const int to_endpoint_id) = 0;

};
#endif

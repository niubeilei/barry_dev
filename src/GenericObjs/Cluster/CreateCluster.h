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
// 2014/12/08 Created by Barry Niu 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_GenericObjs_Cluster_CreateCluster_h
#define Aos_GenericObjs_Cluster_CreateCluster_h

#include "DataRecord/RecordParmList.h"
#include "DataRecord/Ptrs.h"

#include "GenericObjs/GenericMethod.h"
#include "GenericObjs/Ptrs.h"

#include "SEUtil/ParmNames.h"
#include "SEUtil/ParmValues.h"
#include "SEInterfaces/GenericValueObj.h"
#include "SEInterfaces/Ptrs.h"

#include <vector>
#include <map>


class AosMethodCreateCluster : public AosGenericMethod
{
	OmnDefineRCObject;
private:
	typedef hash_map<const OmnString, int, Omn_Str_hash, compare_str> map_t;
	typedef hash_map<const OmnString, int, Omn_Str_hash, compare_str>::iterator itr_t;

	map_t 		mMap;
	

protected:
public:
	AosMethodCreateCluster(const int version);
	~AosMethodCreateCluster();

	virtual bool	config(
						const AosRundataPtr &rdata,
						const AosXmlTagPtr &worker_doc,
						const AosXmlTagPtr &jimo_doc);

	virtual AosJimoPtr cloneJimo() const;

	virtual bool	proc( AosRundata *rdata,
						const OmnString &cluster_name, 
						const vector<AosGenericValueObjPtr> &parms);

private:

	OmnString removeLeadingQuotes(const OmnString &str);
	OmnString removeTailQuotes(const OmnString &str);
};
#endif


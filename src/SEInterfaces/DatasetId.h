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
// 04/29/2012 Created by Chen Ding
// 2013/07/28 Moved by Chen Ding from Dataset 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_DatasetId_h
#define Aos_SEInterfaces_DatasetId_h

#include "Util/String.h"

#define AOSDATASET_DUMMY						"dummy"
#define AOSDATASET_DATASCANNER					"data_scanner"
#define AOSDATASET_STAT							"dataset_stat"		// Ketty 2014/05/05
#define AOSDATASET_STAT_QUERY_SINGLE			"dataset_stat_query_single"		// Ketty 2014/05/05
#define AOSDATASET_QUERY_SINGLE					"query_single"
#define AOSDATASET_QUERY_NEST					"query_nest"
#define AOSDATASET_QUERY_INNER_JOIN				"query_inner_join"
#define AOSDATASET_DATAMATRIX				    "data_matrix"

#define AOSDATASET_EXTERNALDOC					"external_doc"
#define AOSDATASET_READDOC						"dataset_readdoc"
#define AOSDATASET_DATASETSYNCHER				"dataset_syncher"


class AosDatasetId
{
public:
	enum E
	{
		eInvalid, 
		eDataScanner,
		eQuerySingle,
		eQueryNest,
		eQueryInnerJoin,

		eExternalDoc,

		eMax
	};


public:
	inline static bool isValid(const E code)
	{
		return code > eInvalid && code < eMax;
	}
	static E toEnum(const OmnString &id);
	static bool addName(const OmnString &name, const E e);
	static bool check();
};
#endif


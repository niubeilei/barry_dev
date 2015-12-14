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
// 2015/01/22 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEInterfaces_SyncherType_h
#define AOS_SEInterfaces_SyncherType_h

#include "Util/String.h"

#define AOSSYNCHER_TYPE_INVALID 				"invalid"
#define AOSSYNCHER_TYPE_NULL 					"null"
#define AOSSYNCHER_TYPE_EXAMPLE 				"example"
#define AOSSYNCHER_TYPE_TABLE	 				"table"
#define AOSSYNCHER_TYPE_INDEX	 				"index"
#define AOSSYNCHER_TYPE_DOC	 					"doc"
#define AOSSYNCHER_TYPE_INDEX_EXECUTOR			"index_executor"
#define AOSSYNCHER_TYPE_MAP						"map"
#define AOSSYNCHER_TYPE_STAT					"stat"
#define AOSSYNCHER_TYPE_JOIN					"join"
#define AOSSYNCHER_TYPE_SUMMAP_EXECUTOR			"summap_executor"
#define AOSSYNCHER_TYPE_COUNTMAP_EXECUTOR		"countmap_executor"
#define AOSSYNCHER_TYPE_MAXMAP_EXECUTOR			"maxmap_executor"
#define AOSSYNCHER_TYPE_MINMAP_EXECUTOR			"minmap_executor"

struct AosSyncherType
{
public:
	enum E
	{
		eInvalid,

		eNull,
		eExample,
		eTable,
		eIndex, 
		eIndexExecutor, 
		eMap,
		eStat,
		eJoin,
		eSumMapExecutor,
		eCountMapExecutor,
		eMaxMapExecutor,
		eMinMapExecutor,
		eDoc,
		
		eMaxSyncherType
	};

	static bool isValid(const E code)
	{
		return code > eInvalid && code < eMaxSyncherType;
	}

	static OmnString getTypeStr(const E type)
	{
		switch (type)
		{
		case eNull:						return AOSSYNCHER_TYPE_NULL;
		case eExample:					return AOSSYNCHER_TYPE_EXAMPLE;
		case eTable:					return AOSSYNCHER_TYPE_TABLE;
		case eIndex:					return AOSSYNCHER_TYPE_INDEX;
		case eIndexExecutor:			return AOSSYNCHER_TYPE_INDEX_EXECUTOR;
		case eMap:						return AOSSYNCHER_TYPE_MAP;
		case eStat:						return AOSSYNCHER_TYPE_STAT;
		case eJoin:						return AOSSYNCHER_TYPE_JOIN;
		case eSumMapExecutor:			return AOSSYNCHER_TYPE_SUMMAP_EXECUTOR;
		case eCountMapExecutor:			return AOSSYNCHER_TYPE_COUNTMAP_EXECUTOR;
		case eDoc:						return AOSSYNCHER_TYPE_DOC;
		case eMaxMapExecutor:			return AOSSYNCHER_TYPE_MAXMAP_EXECUTOR;
		case eMinMapExecutor:			return AOSSYNCHER_TYPE_MINMAP_EXECUTOR;
		default:						return AOSSYNCHER_TYPE_INVALID;
		}
		return AOSSYNCHER_TYPE_INVALID;
	}

	static E toEnum(const OmnString &name)
	{
		if (name == AOSSYNCHER_TYPE_NULL) 					return eNull;
		else if (name == AOSSYNCHER_TYPE_EXAMPLE) 			return eExample;
		else if (name == AOSSYNCHER_TYPE_TABLE) 			return eTable;
		else if (name == AOSSYNCHER_TYPE_DOC) 				return eDoc;
		else if (name == AOSSYNCHER_TYPE_INDEX) 			return eIndex;
		else if (name == AOSSYNCHER_TYPE_INDEX_EXECUTOR) 	return eIndexExecutor;
		else if (name == AOSSYNCHER_TYPE_MAP) 				return eMap;
		else if (name == AOSSYNCHER_TYPE_STAT) 				return eStat;
		else if (name == AOSSYNCHER_TYPE_JOIN) 				return eJoin;
		else if (name == AOSSYNCHER_TYPE_SUMMAP_EXECUTOR) 	return eSumMapExecutor;
		else if (name == AOSSYNCHER_TYPE_COUNTMAP_EXECUTOR) return eCountMapExecutor;
		else if (name == AOSSYNCHER_TYPE_COUNTMAP_EXECUTOR) return eCountMapExecutor;
		else if (name == AOSSYNCHER_TYPE_MAXMAP_EXECUTOR) 	return eMaxMapExecutor;
		else if (name == AOSSYNCHER_TYPE_MINMAP_EXECUTOR) 	return eMinMapExecutor;
		else 												return eInvalid;
	}

};

#endif

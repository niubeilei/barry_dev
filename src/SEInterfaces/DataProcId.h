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
// 2013/07/28 Moved by Chen Ding from DataProc 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_DataProcId_h
#define Aos_SEInterfaces_DataProcId_h

#include "Util/String.h"

#define AOSDATAPROC_COMPOSE					"compose"
#define AOSDATAPROC_COUNT					"count"
#define AOSDATAPROC_FILTER					"filter"
#define AOSDATAPROC_FIELD					"field"
#define AOSDATAPROC_TIMED					"timed"
#define AOSDATAPROC_IILINDEXING				"iilidxing"
#define AOSDATAPROC_DOCIDTOFIELDS			"docidtofields"
#define AOSDATAPROC_STAT					"stat"
#define AOSDATAPROC_STATCOMP				"statcomp"
#define AOSDATAPROC_DLLPROC					"dllproc"
#define AOSDATAPROC_SORTER 					"sorter"
#define AOSDATAPROC_RANGE					"range"
#define AOSDATAPROC_CONVERTDOC				"convertdoc"		// Ketty 2013/12/23
#define AOSDATAPROC_JOIN					"join"
#define AOSDATAPROC_STATINDEX				"statindex"
#define AOSDATAPROC_STATDOC					"statdoc"
#define AOSDATAPROC_STATDOCSHUFFLE			"statdocshuffle"
#define AOSDATAPROC_UNICOMDOCUMENT			"unicomdocument"
#define AOSDATAPROC_IILBATCHOPR 			"iilbatchopr"
#define AOSDATAPROC_IF						"if"				// Ken Lee, 2014/07/16
#define AOSDATAPROC_WORDPARSER				"wordparser"		// felicia, 2014/08/05
#define AOSDATAPROC_CARNUM					"carnum"		// felicia, 2014/09/04
#define AOSDATAPROC_JIMO					"jimo"		// felicia, 2014/09/04


class AosDataProcId
{
public:
	enum E
	{
		eInvalid, 

		eCompose,
		eCount,
		eFilter,
		eTimed,
		eIILIndexing,
		eField,
		eStat,
		eStatComp,
		eDLLProc,			// Chen Ding, 2013/05/04
		eSorter,			// Chen Ding, 2013/06/25
		eJimoDataProc,		// Chen Ding, 2013/07/28
		eRange,
		
		eConvertDoc,		// Ketty 2013/12/23
		eJoin,	
		eStatIndex,
		eStatDoc,
		eStatDocShuffle,
		eDocIdToFields,		// Fei Pei 2014/05/16
		eUnicomDocument,
		eIILBatchOpr,
		eIf,				// Ken Lee, 2014/07/16
		eWordParser,		// felicia, 2014/08/05
		eCarNum,			// felicia, 2014/09/04
		eJimo,			

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


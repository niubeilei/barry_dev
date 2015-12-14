////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 04/28/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEInterfaces_TaskDataType_h
#define AOS_SEInterfaces_TaskDataType_h

#include "Util/String.h"

#define AOSTASKDATA_INVALID			"invalid"

#define AOSTASKDATA_AUTO			"auto"
#define AOSTASKDATA_DIR				"dir"
#define AOSTASKDATA_DOC				"doc"
#define AOSTASKDATA_FILE			"file"
#define AOSTASKDATA_NORM			"norm"
#define AOSTASKDATA_UNICOMDIR		"unicomdir"
#define AOSTASKDATA_VIRTUALFILE		"virtualfile"

#define AOSTASKDATA_OUTPUT_IIL		"output_iil"
#define AOSTASKDATA_OUTPUT_DOC		"output_doc"
#define AOSTASKDATA_OUTPUT_DOCID	"output_docid"
#define AOSTASKDATA_OUTPUT_SNAPSHOT	"output_snapshot"
#define AOSTASKDATA_DOC_SNAPSHOT	"doc_snapshot"
#define AOSTASKDATA_IIL_SNAPSHOT	"iil_snapshot"
#define AOSTASKDATA_BUFF			"buff"

//	Create by Young : for TaskMgr/TaskDataDocIds.cpp 	2013-05-21 
#define AOSTASKDATA_DOCIDS			"docids"


class AosTaskDataType
{
public:
	enum E
	{
		eInvalid,

		eAuto,
		eDir,
		eDoc,
		eDocIds,
		eFile,
		eNorm,
		eUnicomDir,
		eVirtualFile,
		
		eOutPutDoc,
		eOutPutDocid,
		eOutPutIIL,
		eOutPutSnapShot,
		eDocSnapShot,
		eIILSnapShot,
		eBuff,

		eMax
	};

	static bool isValid(const E id) {return id > eInvalid && id < eMax;}
	static E toEnum(const OmnString &reqid);
	static OmnString toStr(const E id);
	static bool addName(const OmnString &name, const E eid, OmnString &errmsg);
};

#endif

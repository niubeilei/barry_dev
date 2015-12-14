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
// 2013/11/21 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_DataSplitterObj_h
#define Aos_SEInterfaces_DataSplitterObj_h

#include "Jimo/Jimo.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/NetFileObj.h"
#include "SEInterfaces/Ptrs.h"
#include <vector>
using namespace std;

#define AOSDATASPLITTER_FILE					"file"
#define AOSDATASPLITTER_DIR						"dir"
#define AOSDATASPLITTER_DIR_FILES				"dir_files"
#define AOSDATASPLITTER_FILE_FIX_SIZE			"file_fix_size"
#define AOSDATASPLITTER_DIR_UNICOMDOCUMENT		"dir_unicomdocument"
#define AOSDATASPLITTER_FILES					"files"
#define AOSDATASPLITTER_IIL						"iil"
#define AOSDATASPLITTER_IILBYTASK				"iil_by_task"
#define AOSDATASPLITTER_DUMMY					"dummy"
#define AOSDATASPLITTER_FILES_BY_CUBE			"files_by_cube"
#define AOSDATASPLITTER_DATACOL_BY_CUBE			"datacol_by_cube"
#define AOSDATASPLITTER_DATACOL_BY_SIZE			"datacol_by_size"


class AosDataSplitterObj : public AosJimo
{
public:
	AosDataSplitterObj(const int version);
	~AosDataSplitterObj();

	virtual bool config(
					const AosRundataPtr &rdata, 
					const AosXmlTagPtr &worker_doc) = 0;
	virtual void setFileInfo(const AosFileInfo &fileinfo) {}
	virtual void setJobDocid(const u64 &job_docid) = 0;
	virtual bool split(
					AosRundata *rdata, 
					vector<AosXmlTagPtr> &v) =0;

	static AosDataSplitterObjPtr createDataSplitterStatic(
					AosRundata *rdata, 
					const AosXmlTagPtr &def);
};
#endif


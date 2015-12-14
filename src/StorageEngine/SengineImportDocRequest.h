////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// This type of IIL maintains a list of (string, docid) and is sorted
// based on the string value. 
//
// Modification History:
// 07/09/2012 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_StorageEngine_SengineImportDocRequest_h
#define AOS_StorageEngine_SengineImportDocRequest_h

#include "DataAssembler/DataAssemblerType.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/SengineImportDocObj.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "XmlUtil/XmlDoc.h"
#include "StorageEngine/GroupDocOpr.h"

class AosSengineImportDocRequest : virtual public OmnRCObject 
{
	OmnDefineRCObject;

public:
	u32									sizeid;
	AosSengineImportDocObjPtr			caller;
	u64									reqId;
	int64_t								expectedSize;
	AosDataAssemblerType::E				docAssType;
	u64									fileid;
	int									record_len;
	map<int, u64>						snapIds;
	u64									taskDocid;
	AosGroupDocOpr::E					opr;

public:
	AosSengineImportDocRequest(
			const u32 sid,
			const AosSengineImportDocObjPtr &pp, 
			const u64 &r,
			const int64_t &size,
			const AosDataAssemblerType::E t,
			const u64 &f,
			const int l,
			const map<int, u64> &snap_ids,
			const u64 &task_docid)
	:
	sizeid(sid),
	caller(pp),
	reqId(r),
	expectedSize(size),
	docAssType(t),
	fileid(f),
	record_len(l),
	snapIds(snap_ids),
	taskDocid(task_docid)
	{
	}

	AosSengineImportDocRequest(
			const AosSengineImportDocObjPtr &ptr, 
			const u64 &req_id,
			const u64 &file_id,
			const u64 &task_docid)
	:
	caller(ptr),
	reqId(req_id),
	fileid(file_id),
	taskDocid(task_docid)
	{
	}

	~AosSengineImportDocRequest(){}
};


#endif

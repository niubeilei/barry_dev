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
// 2014/01/11 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_StorageEngine_SengineDocInfo_h
#define Aos_StorageEngine_SengineDocInfo_h

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"


class AosSengineDocInfo : virtual public OmnRCObject 
{
public:
	virtual void 	callback(const bool svr_death) = 0;
	virtual u64		getNumDocs() = 0;

	virtual bool	createMemory(
						const u64 &buff_len,
						const int doc_size) = 0;

	virtual bool	addDoc(
						const char *data, 
						const int len, 
						const u64 &docid,
						const AosRundataPtr &rdata)
	{
		return false;
	}
	virtual bool	addDoc(
						const u64 &docid,
						const int doc_len,
						const u64 &schema_docid,
						const char* doc,
						const AosRundataPtr &rdata)
	{
		return false;
	}

	virtual bool	deleteDoc(
						const u64 &docid,
						const AosRundataPtr &rdata)
	{
		return false;
	}
	virtual bool	updateDoc(
						const char *data, 
						const int len, 
						const u64 &docid,
						const AosRundataPtr &rdata)
	{
		return false;
	}


	virtual bool	sendRequestPublic(const AosRundataPtr &rdata) = 0;
};

#endif

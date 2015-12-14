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
// 2014/09/23 Created By Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEInterfaces_DataSortObj_h
#define AOS_SEInterfaces_DataSortObj_h

#include "Util/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"

using namespace std;

class AosDataSortObj : virtual public OmnRCObject 
{
public:
	virtual void	updateMergedTotal(const i64 total) = 0; 
	virtual void	updateProcTotal(const i64 total) = 0;
	virtual bool	finishCreateFile(const AosRundataPtr &rdata) = 0;
	virtual bool	saveToFile(
						const AosBuffPtr &buff,
						const AosRundataPtr &rdata) = 0;
	virtual bool	createSaveFileTask(
						const AosBuffPtr &buff,
						const AosRundataPtr &rdata) = 0;
	virtual void	deleteFiles(
						const vector<u64> &fileids,
						const AosRundataPtr &rdata) = 0;
	virtual bool	finish(
						const u64 totalentries, 
						const AosRundataPtr &rdata) = 0;
	virtual bool	reset(const AosRundataPtr &rdata) = 0;
	virtual bool	sanitycheck(char *begin, int length) = 0;
	virtual void	setIsFinished(const bool isFinish) = 0;
};

#endif


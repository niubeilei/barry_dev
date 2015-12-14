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
// 	2013/01/22 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEInterfaces_IDTransMap_h
#define AOS_SEInterfaces_IDTransMap_h 

#include "SEInterfaces/IDTransMapCaller.h"
#include "SEInterfaces/Ptrs.h"
#include "Thread/Ptrs.h"
#include "TransUtil/IILTrans.h"
#include "Util/HashUtil.h"
#include "XmlUtil/Ptrs.h"

#include <map>
#include <vector>

using namespace std;

class AosIDTransVector : virtual public OmnRCObject,
						 public vector<AosIILTransPtr>
{
	OmnDefineRCObject;

public:
	AosIDTransVector(){}
	~AosIDTransVector(){}
};


typedef map<u64, AosIDTransVectorPtr> IDMap;
typedef map<u64, AosIDTransVectorPtr>::iterator IDMapItr;


class AosIDTransMapObj : virtual public OmnRCObject 
{

public:
	virtual bool addTrans(
					const u64 &id,
					const u32 siteid,
					const AosIILTransPtr &trans,
					const AosRundataPtr &rdata) = 0;

	virtual bool procAllTrans(const AosRundataPtr &rdata) = 0;
	virtual bool clearIDTransVector(const AosIDTransVectorPtr &p) = 0;
	virtual bool getVectorById(
					const u64 &id,
					const u32 siteid,
					AosIDTransVectorPtr &p) = 0;
	virtual bool checkNeedSave(const u32 &num_trans) = 0;
	virtual bool needProcQueue() = 0;
	virtual bool needProcTrans() = 0;
	virtual bool procTrans(const AosRundataPtr &rdata) = 0;
	virtual bool procQueue(const AosRundataPtr &rdata) = 0;
	virtual bool procQueues(const AosRundataPtr &rdata) = 0;
	virtual bool isFree() = 0;
	virtual bool postProc(const AosRundataPtr &rdata) = 0;
	
	virtual void setIsStopping() = 0;	// Ketty 2013/02/21
	virtual bool cleanBkpVirtual(const AosRundataPtr &rdata, const u32 virtual_id) = 0;		// Ketty 2013/02/21
	virtual void cleanCache() = 0;	// Ken Lee, 2013/05/17
	virtual bool cleanCache( const u32 virtual_id, const AosRundataPtr &rdata) = 0;	// Ken Lee, 2013/06/06
};
#endif

 

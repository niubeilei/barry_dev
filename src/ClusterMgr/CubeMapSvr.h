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
// 2015/03/12 Created by Ketty Guo
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_CubeMgr_CubeMapSvr_h
#define Aos_CubeMgr_CubeMapSvr_h

#include "IDO/IDO.h"
#include "Jimo/Jimo.h"
#include "SEInterfaces/JimoCallPackage.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Jimo/JimoDummy.h"


class AosCubeMapSvr : public AosJimoCallPackage 
{
	OmnDefineRCObject;

private:
	typedef hash_map<const u64, AosCubeMapObjPtr, u64_hash, u64_cmp> map_t;
	typedef hash_map<const u64, AosCubeMapObjPtr, u64_hash, u64_cmp>::iterator itr_t;

	map_t		mMap;
	OmnMutex *  mLock;

public:
	AosCubeMapSvr(const int version);
	~AosCubeMapSvr();

	//Jimo Interface
	virtual AosJimoPtr cloneJimo() const;
	virtual bool config(	const AosRundataPtr &rdata, 
							const AosXmlTagPtr &worker_doc,
							const AosXmlTagPtr &jimo_doc) { return false; }

	// JimOCallPackage Interface
	virtual bool jimoCall(AosRundata *rdata, AosJimoCall &jimo_call);
	virtual bool isValid() const {return true;}

private:
	bool getByDocid(AosRundata *rdata, AosJimoCall &jimo_call);
};
#endif


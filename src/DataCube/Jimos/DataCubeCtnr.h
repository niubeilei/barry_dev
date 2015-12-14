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
// 2013/12/23 Created by Chen Ding
// 2014/01/21 Modifyed by Ketty from DataCubeFileGroup 
//
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataCube_DataCubeCtnr_h
#define Aos_DataCube_DataCubeCtnr_h

#include "DataCube/DataCube.h"
#include "DataCube/Jimos/Ptrs.h"
#include "DataCube/Jimos/DataCubeFile.h"
#include "Thread/Mutex.h"
#include "SEInterfaces/NetFileCltObj.h"


//class AosDataCubeCtnr : public AosDataCube, public AosFileReadListener , public AosDataCubeCallerObj 
class AosDataCubeCtnr : public AosDataCube, public AosDataConnectorCallerObj 
{
	OmnDefineRCObject;

	//enum
	//{
	//	eDftReadBlockSize = 10000   // 10KB
	//};

private:
	u32			mNextDataCubeIdx;
	vector<AosDataConnectorObjPtr>   mDataCubes;
	AosDataConnectorObjPtr			mCrtDataCube;
	AosDataConnectorCallerObjPtr	    mCaller;

public:
	AosDataCubeCtnr(
			const OmnString &type,
			const u32 version);
	~AosDataCubeCtnr();

	// AosDataCubeCallerObj Interface
	virtual void callBack(
			const u64 &reqid,
			const AosBuffDataPtr &buff, 
			bool finished);

	virtual AosDataConnectorObjPtr	nextDataConnector();

	virtual void 		setCaller(const AosDataConnectorCallerObjPtr &caller);
	virtual bool		readData(const u64 reqid, AosRundata *rdata);

protected:
	void 	addDataConnector(const AosDataConnectorObjPtr &data_cube);

private:	
	AosDataConnectorObjPtr getSelfProcNextDataCube();

};
#endif


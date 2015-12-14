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
// 05/44/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataStructs_Testers_Vector2DTester_h
#define Aos_DataStructs_Testers_Vector2DTester_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "TaskMgr/Ptrs.h"
#include "Tester/TestPkg.h"
#include "Util/String.h"
#include "XmlUtil/XmlTag.h"
#include "DataStructs/Vector2D.h"


class AosVector2DTester : virtual public OmnTestPkg
{
	enum
	{
		eMaxX = 100000,
		eAddRcd = 0,
		eAddBatchCell = 1,
		eCheckBatchCell = 2,
		eModBatchCell = 3,
		eBatchMod = 4,
		eOprMax = 5,
		eMaxCell = 11000
	};


private:
	AosRundataPtr 			mRundata;
	//AosBuffPtr				mControlBuff;

	AosVector2DPtr			mVector;
	AosStatIdIDGenPtr       mStatIdIDGen;
	int64_t					mXNum;
	u64						mVaildDocids[eMaxX];
	vector<int64_t>*		mArray[eMaxX];

	//AosStatIdExtIDGenPtr       mStatIdExtIDGen;


public:
	AosVector2DTester();
	~AosVector2DTester();

	virtual bool start();

private:
	bool		config();
	bool		basicTest();

	bool		createControlDoc();
	bool		run();

	bool		addRcd();

	bool		batchAddCell();
	
	AosBuffPtr	createData(
			const u64 &docid, 
			vector<int64_t> *vv);

	bool		batchModCell();
	bool		batchMod();

	AosBuffPtr	modifyData(const u64 &docid, vector<int64_t> *vv);
	AosBuffPtr	newAndModifyData(const u64 &docid, vector<int64_t> *vv);

	bool		batchCheckCell();
	bool		modifyLocal(const u64 &docid, const AosBuffPtr &buff);
	bool		modifyLocal(const AosBuffPtr &buff);
	bool		checkValue(
					const u64 &docid,
					const u64 &start_idx, 
					char *data, 
					const int64_t &data_len,
					vector<int64_t> * vv);

	bool		getStatValue(
					const u64 &docid,
					const u64 &time_slot,
					const AosBuffPtr &buff,
					u64 &start_idx);

	bool	checkEntryAll(
				const u64 &docid,
				vector<int64_t> * vv);

	bool	test();
	bool	test1();

};
#endif

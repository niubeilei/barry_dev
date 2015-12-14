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
// 2013/05/03 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataProcDyn_DLLDataProcStatBasic_h
#define Aos_DataProcDyn_DLLDataProcStatBasic_h

#include "DataProc/DataProc.h"
#include "DataColComp/Ptrs.h"
#include "DataColComp/DataColComp.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/DataTypes.h"
#include "Util/String.h"
#include <vector>
using namespace std;


class AosDLLDataProcStatBasic : public AosDataProc, public AosDLLObj
{
private:
	vector<OmnString>	mKeyFieldNameArray;
	vector<int>			mKeyFieldIdxArray;
	OmnString 			mValueFieldName;
	int 				mValueFieldIdx;
	OmnString			mTimeFieldName;
	int					mTimeFieldIdx;

	AosConditionObjPtr	mFilter;
	OmnString			mSep;
	char 				mStatType;

public:
	AosDLLDataProcStatBasic(const AosRundataPtr &rdata, const AosXmlTagPtr &sdoc);
	~AosDLLDataProcStatBasic();

	virtual AosDataProcObjPtr clone();
	virtual AosDataProcStatus::E procData(
						const AosDataRecordObjPtr &record,
						const u64 &docid,
						const AosDataRecordObjPtr &output,
						const AosRundataPtr &rdata);
	virtual bool resolveDataProc(
						map<OmnString, AosDataAssemblerObjPtr> &asms,
						const AosDataRecordObjPtr &record,
						const AosRundataPtr &rdata);
	virtual AosDataProcObjPtr create(
			            const AosXmlTagPtr &def,
						const AosRundataPtr &rdata);
	
private:
	bool config( 		const AosRundataPtr &rdata,
						const AosXmlTagPtr &def);
};
#endif

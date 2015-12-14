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
// 04/25/2014 Created by Young
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_AosConf_ReduceTaskDataProc_h
#define Aos_AosConf_ReduceTaskDataProc_h

#include "AosConf/DataProc.h"
#include "AosConf/DataRecord.h"
#include "AosConf/CompFun.h"
#include "AosConf/DataProcCreateDatasetDoc.h"

#include <map>

namespace AosConf
{
class ReduceTaskDataProc : public DataProc
{
private:
	map<string, string>			mCollectorAttrs;
	map<string, string>			mAssemblerAttrs;
	map<string, string>			mRecordAttrs;
	boost::shared_ptr<DataRecord>		mDataRecord;
	boost::shared_ptr<CompFun>			mCmpFun;
	string 						mDataRecordConf;

public:
	ReduceTaskDataProc();
	ReduceTaskDataProc(const OmnString &type);
	~ReduceTaskDataProc();

	void 	setCollectorType(const string &type);
	void 	setAssemblerType(const string &type);
	void 	setRecordType(const string &type);
	void	setCollectorAttr(const string &name, const string &value);
	void 	setAssemblerAttr(const string &name, const string &value);
	void 	setRecordAttr(const string &name, const string &value);
	void    setRecord(const boost::shared_ptr<DataRecord> record);
	void    setRecord(const string &dataRecordConf);
	void 	setCompFun(const boost::shared_ptr<CompFun> fun) {mCmpFun = fun;}
	//void 	setCompareFunAttr(const string &key, const string &value);
	//void 	setCompareFunAttr(const string &key, int value);
	//void    setCompareAggrs(const string &aggrs){mCompareAggrs = aggrs;}
	//void 	setCmpFieldAttrs(const DataProcCreateDatasetDoc::AosCmpField &value);
	string 	getConfig();
	boost::shared_ptr<DataRecord> getRecord() {return mDataRecord;}

};
}


#endif

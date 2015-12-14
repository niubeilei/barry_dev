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
#ifndef Aos_AosConf_DataProcCreateFileInfo_h
#define Aos_AosConf_DataProcCreateFileInfo_h

#include "AosConf/DataProc.h"
#include "AosConf/DataRecord.h"
#include "AosConf/DataProcCreateDatasetDoc.h"

#include <map>

namespace AosConf
{
class DataProcCreateFileInfo : public DataProc
{
private:
	map<string, string>			mCollectorAttrs;
	map<string, string>			mAssemblerAttrs;
	map<string, string>			mRecordAttrs;
	map<string, string>			mCompareFunAttrs;
	vector<DataProcCreateDatasetDoc::AosCmpField>			mCmpFieldAttrs;
	boost::shared_ptr<DataRecord>		mDataRecord;

public:
	DataProcCreateFileInfo();
	~DataProcCreateFileInfo();

	void 	setCollectorType(const string &type);
	void 	setAssemblerType(const string &type);
	void 	setRecordType(const string &type);
	void	setCollectorAttr(const string &name, const string &value);
	void 	setAssemblerAttr(const string &name, const string &value);
	void 	setRecordAttr(const string &name, const string &value);
	void    setRecord(const boost::shared_ptr<DataRecord> record);
	void 	setCompareFunAttr(const string &key, const string &value);
	void 	setCmpFieldAttrs(const DataProcCreateDatasetDoc::AosCmpField &value);
	string 	getConfig();

};
}


#endif

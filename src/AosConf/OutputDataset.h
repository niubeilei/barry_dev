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
#ifndef Aos_AosConf_OutputDataset_h
#define Aos_AosConf_OutputDataset_h

#include "AosConf/AosConfig.h"
#include "AosConf/DataRecord.h"
#include "AosConf/CompFun.h"
#include <iostream>
#include <boost/shared_ptr.hpp>
using boost::shared_ptr;

namespace AosConf
{
class OutputDataset : public AosConf::AosConfBase
{
private:
	string 									mShuffleType;
	map<string, string>						mDataCollectorAttrs;
	map<string, string>						mAsmAttrs;
	boost::shared_ptr<DataRecord> 			mOutputRecord;
	boost::shared_ptr<CompFun> 				mCompFun;
public:
	OutputDataset();
	OutputDataset(const AosXmlTagPtr &xml);
	~OutputDataset();

	virtual string getConfig();
	void setShuffleType(const string &type);
	void setRecord(const boost::shared_ptr<DataRecord> &record);
	void setCompFun(const boost::shared_ptr<CompFun> &fun);

	void setCollerctorAttr(const string &name, const string &value);
	void setAsmAttr(const string &name, const string &value);

};
}

#endif




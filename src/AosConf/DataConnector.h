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
#ifndef Aos_AosConf_DataConnector_h
#define Aos_AosConf_DataConnector_h

#include "AosConf/DataConnector.h"
#include "AosConf/DataSplit.h"
#include "AosConf/AosConfig.h"
#include "XmlUtil/Ptrs.h"
#include "AosConf/Task.h"

#include <map>
#include <vector>
#include <boost/shared_ptr.hpp>
using boost::shared_ptr;

namespace AosConf
{
class DataConnector: public AosConf::AosConfBase 
{
private:
	boost::shared_ptr<DataSplit>		mSp;
	string								mDirs;
	string								mFile;
	string								mConds;

public:
	DataConnector() {};
	DataConnector(const AosXmlTagPtr &xml);
	~DataConnector() {};
	void setSplit(boost::shared_ptr<DataSplit> split);
	void setConds(string conds);
	void setFile(string file_conf) {mFile = file_conf;}
	void setDirs(string dirs_conf) {mDirs = dirs_conf;}
	string	getConfig();
};
}

#endif




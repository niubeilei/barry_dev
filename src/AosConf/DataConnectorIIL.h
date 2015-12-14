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
#ifndef Aos_AosConf_DataConnectorIIL_h
#define Aos_AosConf_DataConnectorIIL_h

//#include "AosConf/DataConnectorIIL.h"
#include "AosConf/DataSplit.h"
//#include "AosConf/AosConfig.h"
#include "AosConf/DataConnector.h"
#include "XmlUtil/Ptrs.h"
#include "AosConf/Task.h"

#include <map>
#include <vector>
#include <boost/shared_ptr.hpp>
using boost::shared_ptr;

namespace AosConf
{
class DataConnectorIIL: public DataConnector
{
private:
	boost::shared_ptr<DataSplit>		 mSp;
	string							 	 mOpr;
	string							 	 mValue;
	bool								 mNeedQueryCond;

public:
	DataConnectorIIL() { mNeedQueryCond = false;};
	DataConnectorIIL(const AosXmlTagPtr &xml);
	~DataConnectorIIL() {};

	void setSplit(boost::shared_ptr<DataSplit> split);
	void setQueryCond(const string &opr, const string &value);
	string	getConfig();
};
}

#endif




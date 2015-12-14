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
#ifndef Aos_AosConf_DataScanner_h
#define Aos_AosConf_DataScanner_h

#include "AosConf/DataScanner.h"
#include "AosConf/DataConnector.h"
#include "AosConf/AosConfig.h"

#include <map>
#include <vector>


namespace AosConf
{
class DataScanner: public AosConf::AosConfBase
{
private:
	boost::shared_ptr<DataConnector> 			mConnector;
	vector<string>								mConnNames;

public:
	DataScanner() {};
	DataScanner(const AosXmlTagPtr &xml);
	~DataScanner() {};
	
	void setConnector(boost::shared_ptr<DataConnector> con);
	void setConnectorName(const string &name);
	string	getConfig();
};
}



#endif




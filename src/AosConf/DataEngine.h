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
#ifndef Aos_AosConf_DataEngine_h
#define Aos_AosConf_DataEngine_h

#include "AosConf/AosConfig.h"
#include "AosConf/DataProc.h"

#include <map>
#include <string>
#include <boost/shared_ptr.hpp>
using AosConf::DataProc;

namespace AosConf
{
class DataEngine : public AosConf::AosConfBase
{
protected:
	vector<boost::shared_ptr<DataProc> >   	mDataProcs;
	vector<string>							mDataProcsConf;

public:
	DataEngine() {}
	~DataEngine() {}

	virtual string	getConfig() = 0;
	virtual void 	setDataProc(const boost::shared_ptr<DataProc> &proc);
	virtual void 	setDataProc(const string &conf);


};
}


#endif




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
#ifndef Aos_AosConf_DataEngineJoin_h
#define Aos_AosConf_DataEngineJoin_h

#include "AosConf/AosConfig.h"
#include "AosConf/DataEngine.h"
#include "AosConf/DataProc.h"

#include <map>
#include <vector>
#include <boost/shared_ptr.hpp>

namespace AosConf
{
class DataEngineJoin : public DataEngine
{
private:
	map<string, string>				mConditionAttrs;
	string							mLhsDatasetName;
	string							mRhsDatasetName;
	string							mLhsFieldName;
	string							mRhsFieldName;
	string							mCondType;

public:
	DataEngineJoin();
	~DataEngineJoin() {}

	virtual string getConfig();

	void	setConditionAttr(const string &name, const string &value); 
	void    setLhsDatasetName(const string &name){mLhsDatasetName = name;}
	void    setRhsDatasetName(const string &name){mRhsDatasetName = name;}
	void	setLhsFieldName(const string &name){mLhsFieldName = name;}
	void	setRhsFieldName(const string &name){mRhsFieldName = name;}
	void    setCondType(const string &type){mCondType = type;}

};
}


#endif




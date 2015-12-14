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
#ifndef Aos_AosConf_DataEngineJoinNew_h
#define Aos_AosConf_DataEngineJoinNew_h

#include "AosConf/AosConfig.h"
#include "AosConf/DataEngine.h"
#include "AosConf/DataProc.h"

#include <map>
#include <vector>
#include <boost/shared_ptr.hpp>

using boost::shared_ptr;


namespace AosConf
{
class DataEngineJoinNew : public DataEngine
{
private:
	map<string, vector<boost::shared_ptr<DataProc> > > 	mDataProcs;
	map<string, string>				mConditionAttrs;
	string							mLhsDatasetName;
	string							mRhsDatasetName;
	string							mFirstRefName;
	string							mNextRefName;

public:
	DataEngineJoinNew();
	~DataEngineJoinNew() {}

	void 	setDataProc(const string &name, const boost::shared_ptr<DataProc> &proc);
	void	setConditionAttr(const string &name, const string &value); 
	void    setLhsDatasetName(const string &name){mLhsDatasetName = name;}
	void    setRhsDatasetName(const string &name){mRhsDatasetName = name;}
	void	setFirstRefName(const string &name){mFirstRefName = name;}
	void	setNextRefName(const string &name){mNextRefName = name;}
	string	getConfig();

};
}


#endif




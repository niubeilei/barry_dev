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
#ifndef Aos_AosConf_DataFieldStr_h
#define Aos_AosConf_DataFieldStr_h

#include "AosConf/AosConfig.h"
#include "AosConf/DataField.h"
#include <iostream>
#include <boost/shared_ptr.hpp>
using boost::shared_ptr;

namespace AosConf
{
class DataFieldStr : public AosConf::DataField
{
	bool 		constFlag;
	string 		mConstStr;
	string		mExpr;
	vector<boost::shared_ptr<DataFieldStr> >    mDataFields;
	public:
		DataFieldStr();
		~DataFieldStr();

		DataFieldStr(const AosXmlTagPtr &xml);
		bool setConst(const string &str);
		void setDataField(boost::shared_ptr<DataFieldStr> field);

		virtual string getConfig();
};
}

#endif



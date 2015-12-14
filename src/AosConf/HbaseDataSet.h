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
#ifndef Aos_AosConf_HbaseDataSet_h
#define Aos_AosConf_HbaseDataSet_h

#include "AosConf/AosConfig.h"
#include "AosConf/DataSet.h"
#include "AosConf/DataScanner.h"
#include "AosConf/DataSchema.h"
#include "AosConf/DataSet.h"
#include "XmlUtil/Ptrs.h"

#include <map>
#include <vector>

namespace AosConf
{
class HbaseDataSet : public AosConf::DataSet
{
private:
	boost::shared_ptr<DataScanner>			mScanner;
	boost::shared_ptr<DataSchema>			mSchema;
	boost::shared_ptr<DataSet>				mDataSet;

public:
	HbaseDataSet() {};
	HbaseDataSet(const AosXmlTagPtr &config);
	~HbaseDataSet() {};

	void setScanner(boost::shared_ptr<DataScanner> scanner);
	void setSchema(boost::shared_ptr<DataSchema> schema);
	void setDataSet(boost::shared_ptr<DataSet> dataset);
	boost::shared_ptr<DataSchema> getSchema() {return mSchema;}
	string	getConfig();
};
}



#endif




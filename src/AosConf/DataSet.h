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
#ifndef Aos_AosConf_DataSet_h
#define Aos_AosConf_DataSet_h

#include "AosConf/AosConfig.h"
#include "AosConf/DataScanner.h"
#include "AosConf/DataSchema.h"
#include "XmlUtil/Ptrs.h"

#include <map>
#include <vector>

namespace AosConf
{
class DataSet : public AosConf::AosConfBase
{
//private:
protected:
	vector<string>							mScanNames;
	vector<string>							mSchemaNames;

	string 									mScannerConf;
	string									mSchemaConf;

public:
	DataSet() {};
	DataSet(const AosXmlTagPtr &config);
	~DataSet() {};

	void setScanner(boost::shared_ptr<DataScanner> scanner);
	void setScanner(const string &conf) { mScannerConf = conf; }
	void setSchema(boost::shared_ptr<DataSchema> schema);
	void setSchema(const string &conf) { mSchemaConf = conf; }
	void setScanName(const string &name);
	void setSchemaName(const string &name);

	OmnString getName() {return getAttribute("zky_name");}

	boost::shared_ptr<DataSchema> getSchema();
	string	getConfig();
	boost::shared_ptr<DataSet> clone();
};
}



#endif




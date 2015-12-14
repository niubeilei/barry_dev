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
#ifndef Aos_AosConf_DataRecord_h
#define Aos_AosConf_DataRecord_h

#include "AosConf/DataField.h"
#include "AosConf/AosConfig.h"
#include <map>
#include <vector>
#include <boost/shared_ptr.hpp>
using boost::shared_ptr;

namespace AosConf
{
class DataRecord: public AosConf::AosConfBase
{
protected:
	vector<boost::shared_ptr<DataField> >		mFields;
	vector<string>							mFieldNames;

public:
	DataRecord() {};
	DataRecord(const AosXmlTagPtr &xml);
	~DataRecord() {};

	virtual bool setConfig(const AosXmlTagPtr &xml);
	virtual string	getConfig();
	void setFields(vector<boost::shared_ptr<DataField> > &fields);
	virtual void setField(const boost::shared_ptr<DataField> &field);
	virtual void setField(const string &fieldname);
	virtual vector<boost::shared_ptr<DataField> >& getFields() {return mFields;}
	virtual void setField(const string &name, const string &type, const int length) { };
	virtual int getLength() { return -1; }
};
}

#endif




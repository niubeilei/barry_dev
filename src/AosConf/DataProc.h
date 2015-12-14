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
#ifndef Aos_AosConf_DataProc_h
#define Aos_AosConf_DataProc_h

#include "AosConf/AosConfig.h"
#include "AosConf/DataRecord.h"

#include <map>
#include <string>

namespace AosConf
{
class DataProc : public AosConf::AosConfBase
{
private:
	boost::shared_ptr<DataRecord>		mDataRecord;

public:
	DataProc() {} 
	~DataProc() {}

	//virtual string getConfig() = 0; // Young, 2014/09/24
	virtual string getConfig() {
		string conf = "";
		conf += "<dataproc";
		for (map<string, string>::iterator itr=mAttrs.begin(); 
				itr!=mAttrs.end(); itr++)
		{
			conf += " " + itr->first + "=\"" + itr->second +  "\"";
		}
		conf += "></dataproc>";

		return conf;
	}
	virtual void setRecord(boost::shared_ptr<DataRecord> dr) {
		mDataRecord = dr;	
	}
	virtual boost::shared_ptr<DataRecord> getRecord() {
		return mDataRecord;
	}
};
}


#endif

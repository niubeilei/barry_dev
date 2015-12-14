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
#ifndef Aos_AosConf_DataEngineScanSingle_h
#define Aos_AosConf_DataEngineScanSingle_h

#include "AosConf/AosConfig.h"
#include "AosConf/DataEngine.h"
#include "AosConf/DataProc.h"
#include "AosConf/DataProcField.h"

namespace AosConf
{
class DataEngineScanSingle : public DataEngine
{
private:
	vector<boost::shared_ptr<DataProc> >	mDataProcs;

public:
	DataEngineScanSingle() 
	{
		mAttrs["zky_type"] = "dataengine_scan_singlercd";
	}
	~DataEngineScanSingle() {}

	string getConfig()
	{
		// 1. action node
		string conf = "<action";
		for (map<string, string>::iterator itr=mAttrs.begin(); 
				itr!=mAttrs.end(); itr++)
		{
			conf += " " + itr->first + "=\"" + itr->second +  "\"";
		}
		conf += ">";

		// 2. dataprocs node
		for (size_t i=0; i<mDataProcsConf.size(); i++)
		{
			conf += mDataProcsConf[i];
		}
		conf += "</action>";
		return conf;
	}
};
}


#endif




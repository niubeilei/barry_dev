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
#ifndef Aos_AosConf_DataEngineScanMul_h
#define Aos_AosConf_DataEngineScanMul_h

#include "AosConf/AosConfig.h"
#include "AosConf/DataEngine.h"
#include "AosConf/DataProc.h"
#include "AosConf/DataProcField.h"

namespace AosConf
{
class DataEngineScanMul : public DataEngine
{
private:
	//vector<shared_ptr<DataProc> >	mDataProcs;
	map<string ,vector<boost::shared_ptr<DataProc> > >	mDataProcGroups;

public:
	DataEngineScanMul() {}
	~DataEngineScanMul() {}

	void 	setDataProc(const string &name, const boost::shared_ptr<DataProc> &proc) {
		mDataProcGroups[name].push_back(proc);
	}
	
	string	getConfig() {
		// 1. action node
		string conf = "<action";
		for (map<string, string>::iterator itr=mAttrs.begin(); 
				itr!=mAttrs.end(); itr++)
		{
			conf += " " + itr->first + "=\"" + itr->second +  "\"";
		}
		conf += ">";

		// 2. dataprocs node
		for (map<string ,vector<boost::shared_ptr<DataProc> > >::iterator itr = mDataProcGroups.begin();
				itr!=mDataProcGroups.end(); itr++)
		{
			conf = conf + "<zky_dataprocs" + " " + "zky_input_record_name";
			conf +=  "=\"" + itr->first +  "\"";
			conf += ">";
			
			mDataProcs = itr->second; 
			for (size_t i=0; i<mDataProcs.size(); i++)
			{
				string proc_conf = mDataProcs[i]->getConfig();
				conf += proc_conf;
			}
	/*		for (size_t i=0; i<mDataProcField.size(); i++)
			{
				string proc_conf = mDataProcField[i]->getConfig();
				conf += proc_conf;
			}
	*/
			conf += "</zky_dataprocs>";
		}
		conf += "</action>";
		return conf;
	}

};
}


#endif




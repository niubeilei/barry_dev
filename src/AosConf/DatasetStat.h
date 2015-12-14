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
// 2014/05/09 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_AosConf_DatasetStat_h
#define Aos_AosConf_DatasetStat_h

#include "AosConf/DataSet.h"

namespace AosConf
{
class DatasetStat: public DataSet
{
private:
	string		mQryInfoStr;

public:
	DatasetStat() {
		setAttribute("jimo_objid", "dataset_bystat_jimodoc_v0");
		setTagName("inputdataset"); 	
	}
	~DatasetStat() {}
	
	void	setQryInfoStr(const string &str){ mQryInfoStr = str; };

	string getConfig() 
	{
		if (mTagName == "" ) mTagName = "dataset";
		string conf = "<" + mTagName ;
		for (map<string, string>::iterator itr = mAttrs.begin();
				itr != mAttrs.end(); itr++)
		{
			conf += " " + itr->first + "=\"" + itr->second + "\"";
		}
		conf += ">";
		
		conf += mQryInfoStr;

		conf += mScannerConf;
		conf += mSchemaConf;

		for (u32 i = 0; i < mScanNames.size(); i++) {
			conf += "<datascanner zky_datascanner_name=\"" + mScanNames[i] + "\"/>";
		}

		for (u32 i = 0; i < mSchemaNames.size(); i++) {
			conf += "<dataschema zky_dataschema_name=\"" + mSchemaNames[i] + "\"/>";
		}

		conf += "</" + mTagName + ">";
		return conf;
	}

};
}


#endif

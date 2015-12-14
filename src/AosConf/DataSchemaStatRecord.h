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
#ifndef Aos_AosConf_DataSchemaStatRecord_h
#define Aos_AosConf_DataSchemaStatRecord_h

#include "AosConf/DataSchema.h"
#include "AosConf/DataRecordCtnr.h"

namespace AosConf
{
class DataSchemaStatRecord: public DataSchema
{
	
public:
	DataSchemaStatRecord() {
		setAttribute("zky_dataschema_type", "static");  
		setAttribute("jimo_objid", "dataschema_statrecord_jimodoc_v0");  
	}
	~DataSchemaStatRecord() {}
	

	string 	getConfig() {
		// 1. dataschema compose node
		if (mConf != "") return mConf;
		mConf = "<dataschema ";
		for (map<string, string>::iterator itr=mAttrs.begin();
				itr!=mAttrs.end(); itr++)
		{
			mConf += " " + itr->first + "=\"" + itr->second + "\"";
		}
		mConf += " >";

		mConf += mRecord->getConfig();
		
		mConf += "</dataschema>";
		return mConf;
	}

};
}


#endif

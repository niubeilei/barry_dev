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
#ifndef Aos_AosConf_DataConnectorStatQuery_h
#define Aos_AosConf_DataConnectorStatQuery_h

#include "AosConf/DataConnector.h"

namespace AosConf
{
class DataConnectorStatQuery: public DataConnector
{
private:
	string					mConds;

public:
	DataConnectorStatQuery() {
		setAttribute("jimo_objid", "dataconnector_readdoc_bysort_jimodoc_v0"); 
		setAttribute("doc_type", "stat_doc");
	}
	DataConnectorStatQuery(const AosXmlTagPtr &xml)
	{
		aos_assert(xml);
		vector<pair<OmnString, OmnString> > v = xml->getAttributes();
		for(size_t i = 0; i < v.size(); i++){
			setAttribute(v[i].first, v[i].second);
		}

		AosXmlTagPtr conds = xml->getFirstChild("conds");
		aos_assert(conds);
		addConds(conds->toString());
	}

	~DataConnectorStatQuery() {}
	
	void    addConds(const string &conds){mConds = conds; }

	string 	getConfig() {
		if (mConf != "") return mConf;
		mConf = "<dataconnector ";
		for (map<string, string>::iterator itr=mAttrs.begin();
				itr != mAttrs.end(); itr++)
		{
			mConf += " " + itr->first + "=\"" + itr->second + "\"";
		}
		mConf += ">";

		if(mConds != "")
		{
			mConf += mConds;
		}
		
		mConf += "</dataconnector>";
		return mConf;
	}

};
}

#endif

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
#ifndef Aos_AosConf_DataProcIf_h
#define Aos_AosConf_DataProcIf_h

#include "AosConf/DataProc.h"
#include "AosConf/DataProcCompose.h"
#include "AosConf/DataFieldStr.h"

#include <map>
using AosConf::DataProc;
using AosConf::DataProcCompose;
using AosConf::DataFieldStr;

namespace AosConf
{
class DataProcIf : public DataProc
{
private:
	string 									mCondsText;
	//felicia, 2014/11/19
	//boost::shared_ptr<DataProcCompose>		mKeyDataproc;
	//boost::shared_ptr<DataProcCompose>		mValueDataproc;
	vector<boost::shared_ptr<DataProc> > 	mDataProcs;

//	vector<string> 					mKeyInputFieldNames;
//	string 							mKeyOutputFieldName;
//	string 							mValueInputFieldName;
//	string 							mValueOutputFieldName;

public:
	DataProcIf() {
		mAttrs["type"] = "if";
		mAttrs["otype"] = "dataproc";
	}
	~DataProcIf() {}

	void setCondsText(const string text) {
		mCondsText = text;
	}

	//felicia, 2014/11/19
	void setDataProc(boost::shared_ptr<DataProc> dp)
	{
		mDataProcs.push_back(dp);
	}

/*
	void setKeyDataproc(boost::shared_ptr<DataProcCompose> dp) {
		mKeyDataproc = dp;
	}
	
	void setValueDataproc(boost::shared_ptr<DataProcCompose> dp) {
		mValueDataproc = dp;
	}
*/

//	void setKeyInputFieldName(const string name) {
//		mKeyInputFieldNames.push_back(name);
//	}

//	void setKeyOutputFieldName(const string name) {
//		mKeyOutputFieldName = name;
//	}

//	void setValueInputFieldName(const string name) {
//		mValueInputFieldName = name;
//	}

//	void setValueOutputFieldName(const string name) {
//		mValueOutputFieldName = name;
//	}
	

	/************************DataProcIf XML***************************
	<dataproc zky_otype="dataproc" type="if">
	  <cond type="expr">col>10</cond>
	  <dataprocs cond="true">
		<dataproc zky_otype="dataproc" type="compose">
		  <input>
			<filed zky_input_field_name="xxx"/>
		  </input>
		  <output>
			<field zky_output_field_name="xxx"/>
		  </output>
		</dataproc>
		<dataproc zky_otype="dataproc" type="compose">
		  <input>
			<filed zky_input_field_name="xxx"/>
		  </input>
		  <output>
			<field zky_output_field_name="xxx"/>
		  </output>
		</dataproc>
	  </dataprocs>
	</dataproc>
	*************************DataProcIf XML**************************/

	string 	getConfig() {
		// 1. dataproc compose node
		string conf = "";
		conf += "<dataproc";
		for (map<string, string>::iterator itr=mAttrs.begin(); 
				itr!=mAttrs.end(); itr++)
		{
			conf += " " + itr->first + "=\"" + itr->second +  "\"";
		}
		conf += ">";

		// 2. cond node 
		conf += "<zky_filter zky_type=\"expr\"><![CDATA[";
		conf += mCondsText;
		conf += "]]></zky_filter>";

		// 3. true_procs node
		conf += "<true_procs cond=\"true\">";
		//felicia, 2014/11/19
		//conf += mKeyDataproc->getConfig();
		//conf += mValueDataproc->getConfig();
		for(size_t i=0; i<mDataProcs.size(); i++)
		{
			conf += mDataProcs[i]->getConfig();
		}
		
		// 3.1 key dataproc
		/*
		if (!mKeyInputFieldNames.empty() && 
				mKeyOutputFieldName != "" &&
				mValueInputFieldName != "")
		{
			conf += "<dataproc zky_otype=\"dataproc\" type=\"compose\">";
			conf += 	"<input>";
			for (size_t i=0; i<mKeyInputFieldNames.size(); i++)
			{
				conf +=	"<field zky_input_field_name=\"" + mKeyInputFieldNames[i] + "\" ";
				if (i != 0) conf += "  zky_sep=\"0x01\"";
				conf += "></field>";
			}
			conf +=		"</input>";
			conf += 	"<output>";
			conf +=			"<field zky_output_field_name=\"" + mKeyOutputFieldName + "\"></field>";
			conf +=		"</output>";
			conf += 	"</dataproc>";

			// 3.2 value dataproc
			conf += "<dataproc zky_otype=\"dataproc\" type=\"compose\">";
			conf +=		"<input>";
			conf +=			"<field zky_input_field_name=\"" + mValueInputFieldName + "\"></field>";
			conf +=		"</input>";
			conf +=		"<output>";
			conf +=			"<field zky_output_field_name=\"" + mValueOutputFieldName + "\"></field>";
			conf +=		"</output>";
			conf +=	"</dataproc>";
		}*/

		conf += "</true_procs>";
		
		// 4. false_procs
		conf += "<false_procs>";
		conf += "</false_procs>";

		conf += "</dataproc>";
		return conf;
	}

};
}


#endif


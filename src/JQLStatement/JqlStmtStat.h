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
// 2013/10/09 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_JQLStatement_JqlStmtStat_H
#define AOS_JQLStatement_JqlStmtStat_H

#include "JQLStatement/JqlStatement.h"
#include "JQLStatement/JqlStmtExpr.h"
#include "Util/String.h"
//#include "StatUtil/StatUtil.h"

#include "AosConf/DataRecord.h"            
#include "AosConf/DataScanner.h"           
//#include "AosConf/DataSchemaInternalStat.h" 
#include "AosConf/DataConnectorStatQuery.h" 
#include <boost/make_shared.hpp>
#include <map>

using boost::shared_ptr;
using namespace AosConf;

class AosJqlStmtStat : public AosJqlStatement
{
	#if 0
private:
	OmnString						mStatDocName;
	OmnString						mDataSetName;
	AosExprList*					mKeyFields;
	AosXmlTagPtr					mDatasetDoc;
	map<OmnString, AosXmlTagPtr>	mDatasetDocs;
	OmnString						mPreLevel;
	AosXmlTagPtr					mRecord;
	u32								mMaxThread;

public:
	AosJqlStmtStat();
	~AosJqlStmtStat();

	//member functions
	bool run(const AosRundataPtr run(const AosRundataPtr &rdata)rdata, const AosJimoProgObjPtr run(const AosRundataPtr &rdata)prog);
	AosJqlStatement *clone();
	virtual void dump();

	//getter/setters
	void setStatDocName(OmnString name);
	void setDataSetName(OmnString name);
	void setKeyFields(AosExprList* key_fields);
	void setMaxThread(u32 num){ mMaxThread = num;}

private:
	bool runStat(const AosRundataPtr &rdata);
	AosXmlTagPtr convertToXml(const AosRundataPtr &rdata);
	bool	checkKeyFields(
				const AosXmlTagPtr &tag,
				const AosRundataPtr &rdata);
	AosXmlTagPtr	getStatDoc(const AosRundataPtr &rdata);
	bool			getDatasetDocs(
						const AosXmlTagPtr &tag,
						const AosXmlTagPtr &stat_doc,
						const AosRundataPtr &rdata);
	AosXmlTagPtr	createDataset(
						const vector<string> &field_names,
						const AosXmlTagPtr &measures,
						const OmnString &time_field_name,
						const OmnString &vector2d_str,
						const AosXmlTagPtr &data_record,
						const AosRundataPtr &rdata);
	bool			createDataSchema(
						const vector<string> &field_names,
						const AosXmlTagPtr &measures,
						const OmnString &time_field_name,
						const AosXmlTagPtr &data_record,
						boost::shared_ptr<DataSchemaInternalStat> &data_schema);
	bool			createDataScanner(
						const vector<string> &field_names,
						const AosXmlTagPtr &measures,
						const OmnString &vector2d_str,
						boost::shared_ptr<DataScanner> &data_scanner);
	bool			getStatRecord(
						AosXmlTagPtr &data_record,
						const vector<string> &field_names,
						const OmnString &time_field_name,
						map<OmnString, vector<OmnString> > &func_fields);
	
	// Ketty 2014/06/19
	bool 	initAllStatFieldDefs(
				const vector<string> &stat_keys,
				const AosXmlTagPtr &measures,
				const OmnString &time_field_name,
				const AosXmlTagPtr &data_record,
				vector<StatFieldDef> &all_field_defs);

	// Ketty 2014/06/19
	bool 	initStatFieldDef(
				const OmnString &fname,
				const u32 field_idx,
				StatFieldDef &field_def,
				bool &inited,
				const vector<string> &stat_keys,
				const AosXmlTagPtr &measures,
				const OmnString &time_field_name);

	// Ketty 2014/06/19
	bool 	isStatKeyField(
				const vector<string> &stat_keys,
				const string &fname,
				int &idx);
	
	// Ketty 2014/06/19
	bool 	isMeasureField(
				const AosXmlTagPtr &all_measures,
				const OmnString &fname,
				int &idx);

	bool			procDistinct(
						AosXmlTagPtr &stat_doc,
						const AosRundataPtr &rdata);
	#endif
};

#endif

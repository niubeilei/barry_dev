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
#ifndef AOS_JQLStatement_JqlStmtDataset_H
#define AOS_JQLStatement_JqlStmtDataset_H

#include "JQLStatement/JqlStatement.h"
#include "JQLStatement/JqlStmtExpr.h"
#include "Util/String.h"

class AosJqlStmtDataset : public AosJqlStatement
{
private:
	AosXmlTagPtr 					mDoc;

	//data from JQLParser
	OmnString						mName;
	OmnString						mSchema;
	OmnString						mScanner;
	OmnString						mComment;
	OmnString						mErrmsg;


public:
	AosJqlStmtDataset();
	AosJqlStmtDataset(const OmnString &errmsg);
	~AosJqlStmtDataset(){};

	//getter/setters
	void setComment(OmnString comment);
	void setName(OmnString name);
	void setSchema(OmnString name);
	void setScanner(OmnString name);

	//member functions
	virtual bool run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog);
	virtual AosXmlTagPtr convertToXml(const AosRundataPtr &rdata);
	virtual AosJqlStatement *clone();

	bool createDataset(const AosRundataPtr &rdata);
	bool showDatasets(const AosRundataPtr &rdata);
	bool describeDataset(const AosRundataPtr &rdata);
	bool dropDataset(const AosRundataPtr &rdata);

	static bool convertDatasetConf(
			const AosRundataPtr &rdata,
			AosXmlTagPtr &dataset_doc);

private:
	static AosXmlTagPtr createDataScannerConf(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &def);

	static AosXmlTagPtr createDataSchemaConf(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &def);

	static AosXmlTagPtr createDataRecordConf(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &def);

	static AosXmlTagPtr createDataFieldConf(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &def);

	static AosXmlTagPtr createConnectorConf(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &def);

	static void modifyName(
			const AosXmlTagPtr &def,
			const AosXmlTagPtr &doc,
			const OmnString &aname,
			const OmnString &avalue);

};

#endif

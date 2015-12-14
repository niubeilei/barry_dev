////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Statistics.h
// Description:
//   
//
// Modification History:
// 2014/05/08	Created by Ketty 
////////////////////////////////////////////////////////////////////////////

#ifndef AOS_JQLStatment_JqlSingleStatTable_h
#define AOS_JQLStatment_JqlSingleStatTable_h

#include "StatUtil/Ptrs.h"
//#include "StatUtil/StatUtil.h"
#include "StatUtil/StatTimeUnit.h"

#include "JQLStatement/JqlSelect.h"
#include "JQLStatement/JqlSingleTable.h"
#include "JQLStatement/JqlStatQuery.h"

using boost::shared_ptr;
using namespace AosConf;

class AosJqlSingleStatTable : public AosJqlSingleTable,
							  public AosJqlStatQuery
{
public:
	AosJqlSingleStatTable();
	~AosJqlSingleStatTable();

	static bool checkIsStatQuery(
				const AosRundataPtr &rdata,
				const AosJqlSelectPtr &select,
				const AosExprObjPtr &table_expr,
				const AosExprObjPtr &alias);

	virtual bool init(
				const AosRundataPtr &rdata,
				const AosJqlSelectPtr &select);
	
	virtual bool initInputFields(const AosRundataPtr &rdata);
	
	virtual bool initOutputFieldList(const AosRundataPtr &rdata, const AosJqlSelectPtr &select);
	
	virtual OmnString generateTableConf(const AosRundataPtr &rdata);
	
private:
	static bool initStatDocs(
				const AosRundataPtr &rdata,
				const AosJqlSelectPtr &select,
				vector<AosXmlTagPtr> &all_stat_docs);

	static bool initStatDocs(
				const AosRundataPtr &rdata,
				const OmnString &table_name,
				OmnString &stat_objid,
				vector<AosXmlTagPtr> &all_stat_docs);

	static bool initStatDocsPriv(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &stat_doc,
				vector<AosXmlTagPtr> &all_stat_docs);

	void IsStatTable (bool &isStat) {isStat = true;}
};

#endif

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

#ifndef AOS_JQLStatment_JqlJoinStatTable_h
#define AOS_JQLStatment_JqlJoinStatTable_h

#include "StatUtil/Ptrs.h"
//#include "StatUtil/StatUtil.h"
//#include "StatUtil/StatDefineDoc.h"

#include "JQLStatement/JqlSelect.h"
#include "JQLStatement/JqlJoinTable.h"
#include "JQLStatement/JqlStatQuery.h"


class AosJqlJoinStatTable : public AosJqlJoinTable,
							public AosJqlStatQuery
{

public:
	AosJqlJoinStatTable();
	~AosJqlJoinStatTable();

	static bool checkIsStatQuery(
				const AosRundataPtr &rdata,
				const AosJqlSelectPtr &select);

	virtual bool init(
				const AosRundataPtr &rdata,
				const AosJqlSelectPtr &select);
	
	virtual bool initInputFields(const AosRundataPtr &rdata);
	
	virtual OmnString generateWhereConf(const AosRundataPtr &rdata);
	
	virtual OmnString generateDefaultIndexConf(const AosRundataPtr &rdata);
	
	virtual OmnString generateGroupByConf(const AosRundataPtr &rdata);
	
	virtual OmnString generateOrderByConf(const AosRundataPtr &rdata){ return ""; };
	
	virtual OmnString generateHavingConf(const AosRundataPtr &rdata){ return ""; };
	
	virtual OmnString generateInputDatasetConf(const AosRundataPtr &rdata);
	

private:
	static bool initStatDocs(
				const AosRundataPtr &rdata,
				const AosJqlSelectPtr &select,
				vector<AosXmlTagPtr> &all_stat_docs);

	static bool initStatDocs(
				const AosRundataPtr &rdata,
				const OmnString &join_stat_objid,
				vector<AosXmlTagPtr> &all_stat_docs);
};

#endif

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
// This job splitter splits jobs based on IIL size. The input is an IIL.
// It creates one task for each N number of entries in the IIL.
//
// mFileName:	
// mRecordSize: If the file is fixed length records, this is the record length.
//
// Modification History:
// 2013/05/21 Created by Andy zhang
// ////////////////////////////////////////////////////////////////////////////
#if 0
#include "Job/JobSplitterDocIds.h"

#include "API/AosApi.h"
#include "API/AosApiG.h"
#include "Job/JobSplitterNames.h"
#include "SEInterfaces/NetFileCltObj.h"
#include "SEInterfaces/TaskDataType.h"
#include "SEInterfaces/TaskDataObj.h"
#include "TransUtil/RoundRobin.h"
#include "QueryClient/QueryClient.h"

AosJobSplitterDocIds::AosJobSplitterDocIds(const bool flag)
:
AosJobSplitter(AOSJOBSPLITTER_DOCIDS, AosJobSplitterId::eDocIds, flag),
mInitedQuery(false)
{
}


AosJobSplitterDocIds::~AosJobSplitterDocIds()
{
}


bool
AosJobSplitterDocIds::config(
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	//<splitter splitterid="xxxx">
	//	<entries>
	//		<entry zky_opr="eq" envname="xxx"></entry>
	//		...
	//		<entry zky_opr="eq" envname="xxx"></entry>
	//	</entries>
	//	<query ...>
	//	 ....
	//	</query>
	//</splitter>
	aos_assert_r(sdoc, false);
	
	mQueryXml = sdoc->getFirstChild("query");
	aos_assert_r(mQueryXml, false);

	AosXmlTagPtr entries = sdoc->getFirstChild("entries");
	if(entries)
	{
		AosXmlTagPtr cond = mQueryXml->xpathGetFirstChild("conds/cond");
		AosXmlTagPtr entry = entries->getFirstChild(true);
		while(entry)
		{
			OmnString opr = entry->getAttrStr(AOSTAG_OPR, "");
			aos_assert_r(opr != "",  false);
			OmnString envname = entry->getAttrStr("envname", "");
			aos_assert_r(envname != "", false);
			//Get Env Value
			OmnString evalue = mJobENV[envname];
			aos_assert_r(evalue != "", false);

			OmnString termstr = "<term type=\"arith\" order=\"false\" reverse=\"false\"><selector type=\"attr\" aname=\"";
				termstr << envname << "\"><![CDATA[]]></selector><cond type=\"arith\" ctype=\"const\" zky_opr=\"";
				termstr << opr << "\"><![CDATA[";
				termstr << evalue << "]]></cond></term>";
			AosXmlParser termparser;
			AosXmlTagPtr node = termparser.parse(termstr, "" AosMemoryCheckerArgs);
			cond->addNode(node);
			entry = entries->getNextChild();
		}
	}
	mRowDelimiter = sdoc->getAttrStr(AOSTAG_ROW_DELIMITER);
	return true;
}
		

bool
AosJobSplitterDocIds::splitTasks(
		const AosXmlTagPtr &def,
		const AosTaskDataObjPtr &task_data,
		vector<AosTaskDataObjPtr> &task_datas,
		const AosRundataPtr &rdata)
{
	aos_assert_r(def, false);
	aos_assert_r(task_data, false);

	//Jozhi 2013/09/14
	//task_datas.clear();
	AosTaskDataType::E type = task_data->getTaskDataType();
	if (type != AosTaskDataType::eDocIds)
	{
		//task_datas.push_back(task_data);
		return true;
	}

	if(!mInitedQuery)
	{
		bool r = query(rdata);
		aos_assert_r(r, false);
		mInitedQuery = true;
	}

	OmnString row_delimiter = def->getAttrStr(AOSTAG_ROW_DELIMITER, mRowDelimiter);
	AosTaskDataObjPtr t_data;
	for(size_t i=0; i< mDocids.size(); ++i)
	{
		if (mDocids[i].size() <= 0) continue;
		t_data = AosTaskDataObj::createTaskDataDocids(
			mDocids[i], i, row_delimiter);
		aos_assert_r(t_data, false);
		task_datas.push_back(t_data);
	}
	return true;
}


bool
AosJobSplitterDocIds::query(const AosRundataPtr &rdata) 
{
	mDocids.clear();
	mDocids.resize(AosGetNumPhysicals());

	int total = 0;
	int num;
	int start_idx = 0;
	AosXmlTagPtr record;

	while(1)
	{
		OmnString contents;
		AosQueryReqObjPtr query = AosQueryClient::getSelf()->createQuery(mQueryXml, rdata);
		aos_assert_rr(query && query->isGood(), rdata, false); 

		bool rslt = query->procPublic(mQueryXml, contents, rdata);
		aos_assert_rr(rslt, rdata , false);
		
		//<contents>
		//	<record zky_docid="xxxx"/>
		//	...........
		//</contents>
		AosXmlTagPtr data= AosXmlParser::parse(contents AosMemoryCheckerArgs);
		aos_assert_rr(data, rdata, false);

		total = data->getAttrInt("total", 0);	

		record = data->getFirstChild(true);
		while(record)
		{
			u64 docid = record->getAttrU64(AOSTAG_DOCID, 0);
			aos_assert_r(docid, false);
		
			int physicalid = AosGetPhysicalId(docid); 
			mDocids[physicalid].push_back(docid);
			record = data->getNextChild();
		}

		num = data->getAttrInt("num", 0);
		if (num == 0) break;

		start_idx += num;
		if (start_idx >= total) break;

		mQueryXml->setAttr("start_idx", start_idx);
	}
	return true;
}


AosJobSplitterObjPtr
AosJobSplitterDocIds::create(
		const AosXmlTagPtr &sdoc,
		map<OmnString, OmnString> &job_env,
		const AosRundataPtr &rdata) const
{
	AosJobSplitterDocIds * splitter = OmnNew AosJobSplitterDocIds(false);
	splitter->setJobENV(job_env);
	bool rslt = splitter->config(sdoc, rdata);
	aos_assert_r(rslt, 0);
	return splitter;
}

#endif

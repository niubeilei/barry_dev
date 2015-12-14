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
// 2013/11/01 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Dataset_DatasetSplit_h
#define Aos_Dataset_DatasetSplit_h

#include "Rundata/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "Util/String.h"
#include "SEInterfaces/NetFileObj.h"

#include <vector>

using namespace std;

class AosDatasetSplit 
{
public: 
	enum
	{
		eReadBlockSize = 1000000, //1M
		eFileLength = 100000000000, //100G
		//eFileLength = 800000000, //800M
		eRangeSize = 10000000, //20M
		eReadNumRecord = 10000
	};

public:
	static bool splitMergeFile(
					vector<list<AosXmlTagPtr> > &block_files,
					const AosXmlTagPtr &datacol_tag,
					const AosCompareFunPtr &cmp,
					const u32 total_task_proc,
					list<AosXmlTagPtr> &files,
					const AosRundataPtr &rdata);

	static bool splitMergeFileByRecordFixed(
					vector<list<AosXmlTagPtr> > &block_files,
					const int record_len,
					const AosCompareFunPtr &cmp,
					const u32 total_task_proc,
					vector<AosFileInfo> &file_infos,
					const AosRundataPtr &rdata);

	static bool splitMergeFileByRecordBuff(
					vector<list<AosXmlTagPtr> > &block_files,
					const AosCompareFunPtr &cmp,
					const u32 total_task_proc,
					vector<AosFileInfo> &file_infos,
					const AosRundataPtr &rdata);

	static bool findSegement(
					AosFileInfo &info,
					const int record_len,
					const AosCompareFunPtr &cmp,
					const AosBuffPtr &seg_buff,
					const AosRundataPtr &rdata);

	static bool upperBoundForFixed(
					AosFileInfo &info,
					const int record_len,
					const AosBuffPtr &seg_buff,
					const AosCompareFunPtr &cmp,
					const AosRundataPtr &rdata);

	static bool upperBoundForBuff(
					AosFileInfo &info,
					const AosBuffPtr &seg_buff,
					const AosCompareFunPtr &cmp,
					const AosRundataPtr &rdata);

	static bool	staticSplit(
					const AosRundataPtr &rdata,
					const AosXmlTagPtr &def,
					vector<AosXmlTagPtr> &datasets,
					const u64 &job_docid);

	static bool	splitNorm(
					const AosRundataPtr &rdata, 
					const vector<AosXmlTagPtr> &input_datasets,
					vector<AosXmlTagPtr> &datasets,
					const u64 &job_docid);

	static bool	splitNormJoin(
					const AosRundataPtr &rdata, 
					const vector<AosXmlTagPtr> &input_datasets,
					vector<AosXmlTagPtr> &datasets,
					const u64 &job_docid);

	static bool	splitJoin(
					const AosRundataPtr &rdata, 
					const AosXmlTagPtr &def,
					vector<AosXmlTagPtr> &datasets);

	static bool splitByCubePair(
					const AosRundataPtr &rdata, 
					const AosXmlTagPtr &def,
					vector<AosXmlTagPtr> &datasets,
					const u64 &job_docid);

	static bool splitOneDataset(
					const AosRundataPtr &rdata, 
					const AosXmlTagPtr &dataset_conf,
					vector<AosXmlTagPtr> &datasets,
					const u64 &job_docid);
	static bool splitStatJoin(
					const AosRundataPtr &rdata,
					const AosXmlTagPtr &def,
					vector<AosXmlTagPtr> &datasets,
					const u64 &job_docid);
	static bool splitFileAndQuery(
					const u64 &job_docid,
					const AosXmlTagPtr &dataset_file_tag,
					const AosXmlTagPtr &dataset_query_tag,
					vector<AosXmlTagPtr> &datasets,
					const AosRundataPtr &rdata);
	static bool splitFileJoin(
					const AosRundataPtr &rdata, 
					const vector<AosXmlTagPtr> &input_datasets,
					vector<AosXmlTagPtr> &datasets,
					const u64 &job_docid);

	static bool splitFileByRecordBuff(
					int &num,
					vector<pair<OmnString, int64_t> > &vs,
					const AosFileInfo &info,
					const AosCompareFunPtr &comp,
					const AosRundataPtr &rdata);
	static bool splitFileByRecordFixed(
					int &num, 
					vector<pair<OmnString, int64_t> > &vs,
					const AosFileInfo &info, 
					const int entry_length,
					const AosCompareFunPtr &comp,
					const AosRundataPtr &rdata);
private:
	static bool generateNoSplitDatasetConf(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &conn_conf, 
				AosXmlTagPtr &dataset_conf);

	static bool findFirstRecord(
					int &rcd_len,
					int64_t &data_idx,
					char* data,
					const int64_t data_len);

	static bool checkIsBoundary(char *data, int data_len);

};
#endif


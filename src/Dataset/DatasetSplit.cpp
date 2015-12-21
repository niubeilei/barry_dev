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
// This dataset uses one data scanner as its inputs. The data scanner
// can be any type. Most datasets can be implemented by this class.
//
// Modification History:
// 2013/11/01 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Dataset/DatasetSplit.h"

#include "API/AosApi.h"
#include "SEUtil/IILName.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DataSplitterObj.h"
#include "SEInterfaces/NetFileCltObj.h"
#include "SEInterfaces/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Debug/Debug.h"
#include "DataRecord/RecordBuff2.h"
#include "DataCollector/DataCollectorMgr.h"
#include "Util/BuffArrayVar.h"


static int64_t sgMaxLengthPreTask = 2000000000000LL;
//test
//static int64_t sgMaxLengthPreTask = 200000LL;


bool
AosDatasetSplit::splitMergeFile(
		vector<list<AosXmlTagPtr> > &block_files,
		const AosXmlTagPtr &datacol_tag,
		const AosCompareFunPtr &cmp,
		const u32 total_task_proc,
		list<AosXmlTagPtr> &files,
		const AosRundataPtr &rdata)
{
	OmnString str_type = datacol_tag->getAttrStr("record_type");
	AosDataRecordType::E type = AosDataRecordType::toEnum(str_type);
	aos_assert_r(AosDataRecordType::isValid(type), false);
	vector<AosFileInfo> file_infos;
	AosDiskStat disk_stat;
	list<AosXmlTagPtr>::iterator itr = files.begin();
	while(itr != files.end())
	{
		AosFileInfo info;
		info.serializeFrom(*itr);
		file_infos.push_back(info);
		itr++;
	}
	if (type == AosDataRecordType::eBuff)
	{
		return splitMergeFileByRecordBuff(block_files, cmp, total_task_proc, file_infos, rdata);
	}
	else if (type == AosDataRecordType::eFixedBinary)
	{
		AosXmlTagPtr asm_tag = datacol_tag->getFirstChild("asm");
		aos_assert_r(asm_tag, false);
		AosXmlTagPtr rcd_ctnr = asm_tag->getFirstChild("datarecord");
		aos_assert_r(rcd_ctnr, false);
		AosXmlTagPtr rcd = rcd_ctnr->getFirstChild("datarecord");
		aos_assert_r(rcd, false);
		int record_len = rcd->getAttrInt(AOSTAG_LENGTH, -1);
		aos_assert_r(record_len, false);
		return splitMergeFileByRecordFixed(block_files, record_len, cmp, total_task_proc, file_infos, rdata);
	}
	else
	{
		OmnNotImplementedYet;
	}
	return true;
}

//Jozhi
//for merge file split
bool
AosDatasetSplit::splitMergeFileByRecordFixed(
		vector<list<AosXmlTagPtr> > &block_files,
		const int record_len,
		const AosCompareFunPtr &cmp,
		const u32 total_task_proc,
		vector<AosFileInfo> &file_infos,
		const AosRundataPtr &rdata)
{
	bool rslt = false;
	AosDiskStat disk_stat;
	int64_t block_size = eFileLength / total_task_proc;
	int64_t max_block_size = (block_size - block_size% record_len);
	int64_t offset = 0;
	int64_t read_offset = 0;
	int idx = 0;
	AosFileInfo &first_info = file_infos[0];
	aos_assert_r(first_info.mTotalLen > max_block_size, false);
	do
	{
		first_info.mStartOffset = offset;
		read_offset = offset + max_block_size - record_len;
		AosBuffPtr buff;
		rslt = AosNetFileCltObj::readFileToBuffStatic(
				first_info.mFileId, first_info.mPhysicalId, read_offset,
				record_len, buff, disk_stat, rdata.getPtr());
		aos_assert_r(rslt && buff && buff->dataLen() > 0, false);

		first_info.mFileLen = max_block_size;
		first_info.mCrtBlockIdx = idx;
		OmnString str = first_info.serializeToXmlStr();
		AosXmlTagPtr xml = AosXmlParser::parse(str AosMemoryCheckerArgs);
		aos_assert_r(xml, false);
		list<AosXmlTagPtr> tmp;
		tmp.push_back(xml);
		for(u32 i=1; i<file_infos.size(); i++)
		{
			AosFileInfo &info = file_infos[i];
			bool found = findSegement(info, record_len, cmp, buff, rdata);
			if (!found)
			{
				continue;
			}
			info.mCrtBlockIdx = idx;
			OmnString str = info.serializeToXmlStr();
			AosXmlTagPtr xml = AosXmlParser::parse(str AosMemoryCheckerArgs);
			aos_assert_r(xml, false);
			tmp.push_back(xml);	
			info.mStartOffset += info.mFileLen;
		}
		block_files.push_back(tmp);
		idx++;
		offset += max_block_size;
		if (offset + max_block_size >= first_info.mTotalLen)
		{
			list<AosXmlTagPtr> last_tmp;
			first_info.mStartOffset += first_info.mFileLen;
			first_info.mFileLen = first_info.mTotalLen - first_info.mStartOffset;
			first_info.mCrtBlockIdx = idx;
			OmnString str = first_info.serializeToXmlStr();
			AosXmlTagPtr xml = AosXmlParser::parse(str AosMemoryCheckerArgs);
			aos_assert_r(xml, false);
			last_tmp.push_back(xml);
			for(u32 i=1; i<file_infos.size(); i++)
			{
				AosFileInfo &info = file_infos[i];
				info.mFileLen = info.mTotalLen - info.mStartOffset;
				info.mCrtBlockIdx = idx;
				OmnString str = info.serializeToXmlStr();
				AosXmlTagPtr xml = AosXmlParser::parse(str AosMemoryCheckerArgs);
				aos_assert_r(xml, false);
				last_tmp.push_back(xml);
			}
			block_files.push_back(last_tmp);	
		}
	}while((offset + max_block_size) < first_info.mTotalLen);
	return true;
}


bool
AosDatasetSplit::findSegement(
		AosFileInfo &info,
		const int record_len,
		const AosCompareFunPtr &cmp,
		const AosBuffPtr &seg_buff,
		const AosRundataPtr &rdata)
{
	if (record_len > 0)
	{
		return upperBoundForFixed(info, record_len, seg_buff, cmp, rdata);
	}
	else
	{
		return upperBoundForBuff(info, seg_buff, cmp, rdata);
	}
	return true;
}


bool
AosDatasetSplit::findFirstRecord(
		int &rcd_len,
		int64_t &data_idx,
		char* data,
		const int64_t data_len) 
{
	bool rslt = false;
	rcd_len = -1;
	char* crt = data;
	while(data_idx < data_len)
	{
		crt = data + data_idx;
		rcd_len = *((int*)crt);
		if (checkIsBoundary(crt, data_len - data_idx))
		{
			rslt = AosBuff::decodeRecordBuffLength(rcd_len);
			aos_assert_r(rslt, false);
			break;
		}
		else
		{
			//decode fail
			data_idx++;
		}
	}
	return true;
}


bool
AosDatasetSplit::upperBoundForBuff(
		AosFileInfo &info,
		const AosBuffPtr &seg_buff,
		const AosCompareFunPtr &cmp,
		const AosRundataPtr &rdata)
{
	int64_t size = info.mTotalLen;
	int64_t start_idx = info.mStartOffset;
	if (start_idx == (size - 1))
	{
		return false;
	}

	AosBuffArrayVarPtr array = OmnNew AosBuffArrayVar(true);
	array->setCompareFunc(cmp);
	AosBuffPtr bodyBuff = OmnNew AosBuff(100 AosMemoryCheckerArgs);
	bodyBuff->setBuff(seg_buff->data(), seg_buff->dataLen());

	bool rslt = false;
	AosDiskStat disk_stat;
	AosBuffPtr buff;
	int64_t first = start_idx, last = size - 1;
	int64_t middle, pos = 0, read_offset = 0;
	int rcd_len = -1;
	char* crt;
	while(first < last && (last - first) > eRangeSize)
	{
		middle = (first + last) / 2;
		read_offset = middle;
		rslt = AosNetFileCltObj::readFileToBuffStatic(
			info.mFileId, info.mPhysicalId, read_offset,
			eReadBlockSize, buff, disk_stat, rdata.getPtr());
		aos_assert_r(rslt && buff && buff->dataLen() > 0, false);

		char* data = buff->data();	
		int64_t data_len = buff->dataLen();
		int64_t data_idx = 0;
		rslt = findFirstRecord(rcd_len, data_idx, data, data_len);
		aos_assert_r(rslt && rcd_len > 0, false);
		crt = data+data_idx;
		int buff_len = sizeof(int) + rcd_len;
		bodyBuff->setCrtIdx(seg_buff->dataLen());
		bodyBuff->setBuff(crt, buff_len);
		bodyBuff->setDataLen(seg_buff->dataLen() + buff_len);
		int remain_size = 0;
		rslt = array->setBodyBuff(bodyBuff, remain_size);
		aos_assert_r(rslt, false);
		AosBuffPtr header_buff = array->getHeadBuff();
		aos_assert_r(header_buff, false);
		const char* header_lhs = header_buff->data();
		const char* header_rhs = header_lhs + cmp->size;

		if (cmp->cmp(header_lhs, header_rhs) < 0)
		{
			middle += data_idx;
			last = middle;
			pos = last;
		}
		else
		{
			middle += data_idx;
			first = middle + buff_len;
			pos = first;
		}
	}

	if (first < last && (last - first) < eRangeSize)
	{
		rslt = AosNetFileCltObj::readFileToBuffStatic(
			info.mFileId, info.mPhysicalId, first,
			(last-first + 1), buff, disk_stat, rdata.getPtr());
		aos_assert_r(rslt && buff && buff->dataLen() > 0, false);
		char* data = buff->data();	
		int64_t data_len = buff->dataLen();
		int64_t data_idx = 0;
		bool rslt = findFirstRecord(rcd_len, data_idx, data, data_len);
		aos_assert_r(rslt, false);
		while(data_idx < data_len)
		{
			crt = data + data_idx;
			rcd_len = *((int*)crt);
			rslt = AosBuff::decodeRecordBuffLength(rcd_len);
			aos_assert_r(rslt && rcd_len > 0, false);
			int buff_len = sizeof(int) + rcd_len;
			bodyBuff->setCrtIdx(seg_buff->dataLen());
			bodyBuff->setBuff(crt, buff_len);
			bodyBuff->setDataLen(seg_buff->dataLen() + buff_len);

			int remain_size = 0;
			rslt = array->setBodyBuff(bodyBuff, remain_size);
			aos_assert_r(rslt, false);
			AosBuffPtr header_buff = array->getHeadBuff();
			aos_assert_r(header_buff, false);
			const char* header_lhs = header_buff->data();
			const char* header_rhs = header_lhs + cmp->size;
			pos = first + data_idx;
			if (cmp->cmp(header_lhs, header_rhs) < 0)
			{
				break;
			}
			else
			{
				data_idx += buff_len;
			}
		}
	}

	if (pos == start_idx)
	{
{
	OmnScreen << "var upperBound for debug not found" << endl;
}
		return false;
	}
	else
	{
{
OmnString str(crt + sizeof(int) + sizeof(int), rcd_len);
OmnString seg(seg_buff->data() + sizeof(int) + sizeof(int), rcd_len);
OmnScreen << "var upperBound for debug, seg: " << seg << " ,start: " << start_idx << " ,pos: " << pos << " , " << str << endl;
}
		info.mFileLen = pos - info.mStartOffset;
		return true;
	}

	return true;
}

bool
AosDatasetSplit::upperBoundForFixed(
		AosFileInfo &info,
		const int record_len,
		const AosBuffPtr &seg_buff,
		const AosCompareFunPtr &cmp,
		const AosRundataPtr &rdata)
{
	int64_t size = info.mTotalLen / record_len;
	int64_t start_idx = info.mStartOffset / record_len;
	if (start_idx == (size - 1))
	{
		return false;
	}
	bool rslt = false;
	AosBuffPtr buff;
	AosDiskStat disk_stat;
	int64_t first = start_idx, last = size - 1;
	int64_t middle, pos = 0, read_offset = 0;
	while(first < last)
	{
		middle = (first + last) / 2;
		read_offset = middle*record_len;
		rslt = AosNetFileCltObj::readFileToBuffStatic(
			info.mFileId, info.mPhysicalId, read_offset,
			record_len, buff, disk_stat, rdata.getPtr());
		aos_assert_r(rslt && buff && buff->dataLen() > 0, false);
		if (cmp->cmp(seg_buff->data(), buff->data()) < 0)
		{
			last = middle;
			pos = last;
		}
		else
		{
			first = middle + 1;
			pos = first;
		}
	}

	if (pos == start_idx)
	{
{
	OmnScreen << "fixbin upperBound for debug not found" << endl;
}
		return false;
	}
	else
	{

{
	OmnString str(buff->data(), buff->dataLen());
	OmnString seg(seg_buff->data(), buff->dataLen());
	OmnScreen << "fixbin upperBound for debug, seg: " << seg << " ,start: " << start_idx << " ,pos: " << pos << " , " << str << endl;
}
		info.mFileLen = pos * record_len - info.mStartOffset;
		return true;
	}
}


//Jozhi
//for merge file split
bool
AosDatasetSplit::splitMergeFileByRecordBuff(
		vector<list<AosXmlTagPtr> > &block_files,
		const AosCompareFunPtr &cmp,
		const u32 total_task_proc,
		vector<AosFileInfo> &file_infos,
		const AosRundataPtr &rdata)
{
	bool rslt = false;
	AosDiskStat disk_stat;
	int64_t max_block_size = eFileLength / total_task_proc;
	int64_t offset = 0;
	int64_t read_offset = 0;
	char* data; 
	char* crt;
	int idx = 0;
	AosFileInfo &first_info = file_infos[0];
	aos_assert_r(first_info.mTotalLen > max_block_size, false);
	aos_assert_r(max_block_size > eReadBlockSize, false);
	do
	{
		first_info.mStartOffset = offset;
		read_offset = offset + max_block_size - eReadBlockSize;
		AosBuffPtr buff;
		rslt = AosNetFileCltObj::readFileToBuffStatic(
				first_info.mFileId, first_info.mPhysicalId, read_offset,
				eReadBlockSize, buff, disk_stat, rdata.getPtr());
		aos_assert_r(rslt && buff && buff->dataLen() > 0, false);

		data = buff->data();	
		int64_t data_len = buff->dataLen();
		int64_t data_idx = 0;
		int rcd_len = -1;
		rslt = findFirstRecord(rcd_len, data_idx, data, data_len);
		aos_assert_r(rslt, false);
		crt = data + data_idx;
		int buff_len = sizeof(int) + rcd_len;
		AosBuffPtr record_buff = OmnNew AosBuff(buff_len AosMemoryCheckerArgs);
		record_buff->setBuff(crt, buff_len);
		first_info.mFileLen = (max_block_size - eReadBlockSize) + data_idx + buff_len;
		first_info.mCrtBlockIdx = idx;
		OmnString str = first_info.serializeToXmlStr();
		AosXmlTagPtr xml = AosXmlParser::parse(str AosMemoryCheckerArgs);
		aos_assert_r(xml, false);
		list<AosXmlTagPtr> tmp;
		tmp.push_back(xml);
		for(u32 i=1; i<file_infos.size(); i++)
		{
			AosFileInfo &info = file_infos[i];
			bool found = findSegement(info, -1, cmp, record_buff, rdata);
			if (!found)
			{
				continue;
			}

			info.mCrtBlockIdx = idx;
			OmnString str = info.serializeToXmlStr();
			AosXmlTagPtr xml = AosXmlParser::parse(str AosMemoryCheckerArgs);
			aos_assert_r(xml, false);
			tmp.push_back(xml);	
			info.mStartOffset += info.mFileLen;
		}
		block_files.push_back(tmp);
			//	break;
			//}
			//data_idx++;
		//}
		//aos_assert_r(found_record, false);
		offset = first_info.mStartOffset + first_info.mFileLen;
		idx++;
		if (offset + max_block_size >= first_info.mTotalLen)
		{
			list<AosXmlTagPtr> last_tmp;
			first_info.mStartOffset += first_info.mFileLen;
			first_info.mFileLen = first_info.mTotalLen - first_info.mStartOffset;
			first_info.mCrtBlockIdx = idx;
			OmnString str = first_info.serializeToXmlStr();
			AosXmlTagPtr xml = AosXmlParser::parse(str AosMemoryCheckerArgs);
			aos_assert_r(xml, false);
			last_tmp.push_back(xml);
			for(u32 i=1; i<file_infos.size(); i++)
			{
				AosFileInfo &info = file_infos[i];
				info.mFileLen = info.mTotalLen - info.mStartOffset;
				info.mCrtBlockIdx = idx;
				OmnString str = info.serializeToXmlStr();
				AosXmlTagPtr xml = AosXmlParser::parse(str AosMemoryCheckerArgs);
				aos_assert_r(xml, false);
				last_tmp.push_back(xml);
			}
			block_files.push_back(last_tmp);	
		}
	}while(offset + max_block_size < first_info.mTotalLen);

	return true;
}


bool
AosDatasetSplit::staticSplit(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &def,
		vector<AosXmlTagPtr> &datasets,
		const u64 &job_docid)
{
	aos_assert_r(def, false);
	vector<AosXmlTagPtr> input_datasets;
	AosXmlTagPtr dataset_tag = def->getFirstChild("dataset");
	while(dataset_tag)
	{
		input_datasets.push_back(dataset_tag);
		dataset_tag = def->getNextChild("dataset");
	}

	aos_assert_r(input_datasets.size() > 0 , false);
	bool rslt = false;
	if (input_datasets.size() == 1)
	{
		rslt = AosDatasetSplit::splitNorm(rdata, input_datasets, datasets, job_docid);
	}
	else if (input_datasets.size() > 1)
	{
		AosXmlTagPtr dataset_splitter = def->getFirstChild("dataset_splitter");
		if (dataset_splitter)
		{
			OmnString type =  dataset_splitter->getAttrStr("type");
			aos_assert_r(type != "", false);
			if (type == "stat_join")
			{
				rslt = AosDatasetSplit::splitStatJoin(rdata, def, datasets, job_docid);
			}
			else if (type == "file_join")
			{
				rslt = AosDatasetSplit::splitFileJoin(rdata, input_datasets, datasets, job_docid);
			}
			else
			{
				OmnNotImplementedYet;
			}
		}
		else
		{
			rslt = AosDatasetSplit::splitNormJoin(rdata, input_datasets, datasets, job_docid);
		//	rslt = AosDatasetSplit::splitStatJoin(rdata, def, datasets, job_docid);
		}
	}

	aos_assert_r(rslt, false);
	aos_assert_r(!datasets.empty(), false);

	return true;
}

bool
AosDatasetSplit::splitFileByRecordBuff(
		int &num,
		vector<pair<OmnString, int64_t> > &vs,
		const AosFileInfo &info,
		const AosCompareFunPtr &comp,
		const AosRundataPtr &rdata)
{
	num = info.mFileLen / sgMaxLengthPreTask;
	if (info.mFileLen % sgMaxLengthPreTask > 0) num++;
	if (num <= 1)
	{
		return true;
	}

	bool rslt = false;
	int64_t start_pos = 0;
	AosDiskStat disk_stat;
	AosBuffPtr buff;
	while(start_pos < info.mFileLen)
	{
		start_pos += sgMaxLengthPreTask - eReadBlockSize;
		if (start_pos + eReadBlockSize >= info.mFileLen)
		{
			break;
		}
		if (info.mFileName != "")
		{
			rslt = AosNetFileCltObj::readFileToBuffStatic(
				info.mFileName, info.mPhysicalId, start_pos,
				eReadBlockSize, buff, disk_stat, rdata.getPtr());
		}
		else
		{
			rslt = AosNetFileCltObj::readFileToBuffStatic(
				info.mFileId, info.mPhysicalId, start_pos,
				eReadBlockSize, buff, disk_stat, rdata.getPtr());
		}
		aos_assert_r(rslt, false);
		
		OmnString key;
		char * data = buff->data();
		i64 data_len = buff->dataLen();
		i64 idx = 0;
		int lhs_rcd_len = -1;
		char* crt = data;
		while(idx < data_len)
		{
			crt = data + idx;
			lhs_rcd_len = *((int*)crt);
			if (checkIsBoundary(crt, data_len-idx))
			{
				rslt = AosBuff::decodeRecordBuffLength(lhs_rcd_len);
				aos_assert_r(rslt, false);
				const char* lhs = crt;
				const char* rhs = lhs + (sizeof(int) + lhs_rcd_len);
				int rhs_rcd_len = *((int*)rhs);
				rslt = AosBuff::decodeRecordBuffLength(rhs_rcd_len);
				aos_assert_r(rslt, false);

				AosBuffArrayVarPtr array = OmnNew AosBuffArrayVar(true);
				array->setCompareFunc(comp);
				AosBuffPtr bodyBuff = OmnNew AosBuff(100 AosMemoryCheckerArgs);
				int64_t body_len = lhs_rcd_len + rhs_rcd_len + sizeof(int) * 2;
				bodyBuff->setBuff(lhs, body_len);
				int remain_size = 0;
				rslt = array->setBodyBuff(bodyBuff, remain_size);
				aos_assert_r(rslt, false);
				AosBuffPtr header_buff = array->getHeadBuff();
				aos_assert_r(header_buff, false);
				const char* header_lhs = header_buff->data();
				const char* header_rhs = header_lhs + comp->size;
				if (comp->cmp(header_lhs, header_rhs) != 0)
				{
			 		const char* body_rhs = (const char*)(*(i64*)(header_rhs+sizeof(int))) + (*(int*)header_rhs);
					//JIMODB-1048 
					//by barry 2015/10/27
					key << body_rhs + sizeof(int) + sizeof(int) + sizeof(i8);
					start_pos += idx + sizeof(int) + lhs_rcd_len;
					//aos_assert_r(key != "", false);
					if (key == "")
					{
						key << '\b';
					}
					break;
				}
				else
				{
					idx += sizeof(int) + lhs_rcd_len;
				}
			}
			else
			{
				idx++;
			}
		}
		aos_assert_r(key != "", false);
		vs.push_back(make_pair(key, start_pos));
	}
	num = vs.size() - 1;

	return true;
}

bool
AosDatasetSplit::splitFileByRecordFixed(
		int &num, 
		vector<pair<OmnString, int64_t> > &vs,
		const AosFileInfo &info, 
		const int entry_length,
		const AosCompareFunPtr &comp,
		const AosRundataPtr &rdata)
{
	int64_t total_entry_num = info.mFileLen / entry_length;
	int64_t max_entry_num = sgMaxLengthPreTask / entry_length;

	num = total_entry_num / max_entry_num;
	if (total_entry_num % max_entry_num > 0) num++;
	if (num <= 1)
	{
		return true;
	}

	bool rslt = false;
	int64_t each_entry_num = total_entry_num / num;
	int64_t start_pos = 0;
	AosDiskStat disk_stat;
	AosBuffPtr buff;
	for (int i=1; i<num; i++)
	{
		start_pos = start_pos + (each_entry_num - 1) * entry_length;
		if (info.mFileName != "")
		{
			rslt = AosNetFileCltObj::readFileToBuffStatic(
				info.mFileName, info.mPhysicalId, start_pos,
				entry_length * eReadNumRecord, buff, disk_stat, rdata.getPtr());
		}
		else
		{
			rslt = AosNetFileCltObj::readFileToBuffStatic(
				info.mFileId, info.mPhysicalId, start_pos,
				entry_length * eReadNumRecord, buff, disk_stat, rdata.getPtr());
		}
		aos_assert_r(rslt, false);
		
		OmnString key;
		char * data = buff->data();
		for (int j=0; j<eReadNumRecord-1; j++)
		{
			const char* lhs = data + (j * entry_length);
			const char* rhs = data + ((j + 1) * entry_length);
			if (comp->cmp(lhs, rhs) != 0)
			{
				key << &data[(j+1)*entry_length];
				start_pos = start_pos + (j+1)*entry_length;  
				break;	
			}
		}
		aos_assert_r(key != "", false);
		vs.push_back(make_pair(key, start_pos));
	}
	return true;
}

bool
AosDatasetSplit::splitFileAndQuery(
		const u64 &job_docid,
		const AosXmlTagPtr &dataset_file_tag,
		const AosXmlTagPtr &dataset_query_tag,
		vector<AosXmlTagPtr> &datasets,
		const AosRundataPtr &rdata)
{
	int shuffle_id = -1;
	AosXmlTagPtr datascanner_tag = dataset_file_tag->getFirstChild("datascanner");
	aos_assert_r(datascanner_tag, false);

	AosXmlTagPtr connector_tag = datascanner_tag->getFirstChild("dataconnector");
	aos_assert_r(connector_tag, false);

	shuffle_id = connector_tag->getAttrInt(AOSTAG_SHUFFLE_ID, -1);

	AosXmlTagPtr files = connector_tag->getFirstChild("files");
	aos_assert_r(files, false);

	AosXmlTagPtr file = files->getFirstChild(true);
	//aos_assert_r(file, false);
	if (!file)
	{
		OmnScreen << "splitFileAndQuery no files: " << dataset_file_tag->toString() << endl;
		OmnString xml_str = "";
		xml_str << "<input_datasets>"
				<< dataset_file_tag->toString()
				<< dataset_query_tag->toString()
				<< "</input_datasets>";
		AosXmlTagPtr xml = AosXmlParser::parse(xml_str AosMemoryCheckerArgs);
		aos_assert_r(xml, false);
		datasets.push_back(xml);
		return true;	
	}


	AosFileInfo info;
	info.serializeFrom(file);
	aos_assert_r(info.mFileId > 0 || info.mFileName != "", false);

	bool rslt = true;
	AosDiskStat disk_stat;
	if (info.mFileLen <= 0) 
	{
		int64_t fileLen = 0;
		if (info.mFileName != "")
		{
			rslt = AosNetFileCltObj::getFileLengthStatic(
				info.mFileName, info.mPhysicalId, fileLen, disk_stat, rdata.getPtr()); 
		}
		else
		{
			rslt = AosNetFileCltObj::getFileLengthStatic(
				info.mFileId, info.mPhysicalId, fileLen, disk_stat, rdata.getPtr()); 
		}
		aos_assert_r(rslt, false);

		info.mFileLen = fileLen;
		aos_assert_r(info.mFileLen >= 0, false);
	}

	AosXmlTagPtr schema_conf = dataset_file_tag->getFirstChild("dataschema");
	aos_assert_r(schema_conf, false);
	AosXmlTagPtr rcd_conf = schema_conf->getFirstChild("datarecord");
	aos_assert_r(rcd_conf, false);

	AosXmlTagPtr datascanner_conf = dataset_file_tag->getFirstChild("datascanner");
	aos_assert_r(datascanner_conf, false);
	//AosXmlTagPtr dataconn_conf = datascanner_conf->getFirstChild("dataconnector");
	//aos_assert_r(dataconn_conf, false);
	OmnString datacol_id = datascanner_conf->getAttrStr("zky_name");
	aos_assert_r(datacol_id != "", false);

	AosJobObjPtr job = AosJobMgrObj::getJobMgr()->getStartJob(job_docid, rdata);  
	aos_assert_r(job, false);
	AosDataCollectorMgrPtr colMgr = job->getDataColMgr();
	aos_assert_r(colMgr, false);
	AosLogicTaskObjPtr datacol = colMgr->getLogicTaskLocked(datacol_id, rdata);

	AosCompareFunPtr comp = datacol->getComp();
	aos_assert_r(comp, false);

	int num = 0;
	vector<pair<OmnString, int64_t> > vs;
	AosDataRecordType::E type = AosDataRecordType::toEnum(rcd_conf->getAttrStr("type"));
	if (type == AosDataRecordType::eBuff)
	{
		rslt = splitFileByRecordBuff(num, vs, info, comp, rdata);
		aos_assert_r(rslt, false);
	}
	else if (type == AosDataRecordType::eFixedBinary)
	{
		int entry_length = rcd_conf->getAttrInt(AOSTAG_LENGTH, -1);
		aos_assert_r(entry_length > 0, false);
		rslt = splitFileByRecordFixed(num, vs, info, entry_length, comp, rdata);
		aos_assert_r(rslt, false);
	}
	else
	{
		OmnNotImplementedYet;
	}


	AosXmlTagPtr dataset_query_tag_tmp = dataset_query_tag->clone(AosMemoryCheckerArgsBegin);
	AosXmlTagPtr tag = dataset_query_tag_tmp->getFirstChild("datascanner");
	aos_assert_r(tag, false);
	tag = tag->getFirstChild("dataconnector");
	aos_assert_r(tag, false);

	OmnString org_iil_name;
	if (shuffle_id > -1)
	{
		OmnString iil_name = tag->getAttrStr("zky_iilname");
		org_iil_name = iil_name;
		const char *pos = strstr((const char*)iil_name.data(), "__$group"); 
		if (pos)
		{                                                                    
			iil_name = OmnString(iil_name.data(), pos-iil_name.data());         
			iil_name = AosIILName::composeCubeIILName(shuffle_id, iil_name);   
		}                                                                    
		tag->setAttr("zky_iilname", iil_name);
OmnScreen << "shuffle id is " << shuffle_id << ". iilname is " <<  iil_name << endl;
	}

	aos_assert_r(num >= 0, false);
	if (num <= 1)
	{
		OmnString xml_str = "";
		xml_str << "<input_datasets>"
				<< dataset_file_tag->toString()
				<< dataset_query_tag_tmp->toString()
				<< "</input_datasets>";
		AosXmlTagPtr xml = AosXmlParser::parse(xml_str AosMemoryCheckerArgs);
		aos_assert_r(xml, false);
		datasets.push_back(xml);
		return true;	
	}
	else
	{
		OmnString xml_str;
		AosXmlTagPtr xml;
		for (int i=0; i<num; i++)
		{
			AosFileInfo tmp_info = info;
			tmp_info.mStartOffset = (i==0) ? 0 : vs[i-1].second;
			tmp_info.mFileLen = vs[i-1].second; 
			if (i == 0)
			{
				tmp_info.mFileLen = vs[0].second;
			}
			else if (i == num - 1)
			{
				tmp_info.mFileLen = info.mFileLen - vs[i-1].second;
			}
			else
			{
				tmp_info.mFileLen = vs[i].second - vs[i-1].second;
			}

			AosXmlTagPtr lh = dataset_file_tag->clone(AosMemoryCheckerArgsBegin);
			AosXmlTagPtr rh = dataset_query_tag_tmp->clone(AosMemoryCheckerArgsBegin);

			xml_str = "";
			xml_str << "<files>"
				<< tmp_info.serializeToXmlStr()
				<< "</files>";
			xml = AosXmlParser::parse(xml_str AosMemoryCheckerArgs);
			aos_assert_r(xml, false);

			AosXmlTagPtr lh_scan = lh->getFirstChild("datascanner");
			aos_assert_r(lh_scan, false);
			AosXmlTagPtr lh_conn = lh_scan->getFirstChild("dataconnector");
			aos_assert_r(lh_conn, false);
			lh_conn->removeNode("files", true, true);
			lh_conn->addNode(xml);

			AosXmlTagPtr rh_scan = rh->getFirstChild("datascanner");
			aos_assert_r(rh_scan, false);
			AosXmlTagPtr rh_conn = rh_scan->getFirstChild("dataconnector"); 
			aos_assert_r(rh_conn, false);

			AosXmlTagPtr rh_cond = rh_conn->getFirstChild("query_cond"); 
			aos_assert_r(rh_cond, false);

			OmnString opr, value1, value2;
			if (i == 0)
			{
				opr = "lt";
				value1 = vs[0].first;
			}
			else if (i == num - 1)
			{
				opr = "ge";
				value1 = vs[num - 2].first;
			}
			else
			{
				opr = "r2";
				value1 = vs[i - 1].first;
				value2 = vs[i].first;
			}
			rh_cond->setAttr("zky_opr", opr);
			rh_cond->setNodeText("zky_value", value1, true);
			if (value2 != "") rh_cond->setNodeText("zky_value2", value2, true);

			xml_str = "";
			xml_str << "<input_datasets>"
				<< lh->toString()
				<< rh->toString()
				<< "</input_datasets>";
			xml = AosXmlParser::parse(xml_str AosMemoryCheckerArgs);
			aos_assert_r(xml, false);

			datasets.push_back(xml);
		}
	}

	return true;
}

bool
AosDatasetSplit::splitStatJoin(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &def,
		vector<AosXmlTagPtr> &datasets,
		const u64 &job_docid)
{
	//<datascanner jimo_objid=\"datascanner_parallel_jimodoc_v0\" zky_name=\"dp_groupby_t1_stat_model_output\">
	//	<dataconnector jimo_objid=\"dataconnector_datacol_jimodoc_v0\" zky_datacolid=\"dp_groupby_t1_stat_model_output\">
	//		<split jimo_objid=\"dataspliter_datacolbygroup_jimodoc_v0\"//>
	//  </dataconnector>
	//</datascanner>
	//
	//change to :
	//<datascanner jimo_objid=\"datascanner_parallel_jimodoc_v0\" zky_name=\"dp_groupby_t1_stat_model_output\">
	//	<dataconnector zky_objid=\"dataconnector_idfiles_jimodoc_v0\" 
	//				   zky_otype=\"zkyotp_jimo\" 
	//				   jimo_name=\"jimo_datacube\" 
	//				   jimo_type=\"jimo_datacube\" 
	//				   current_version=\"0\" 
	//				   zky_classname=\"AosDataCubeIdFiles\" 
	//				   type=\"idfiles\" 
	//				   zky_physicalid=\"0\" 
	//				   zky_shuffle_id=\"0\" 
	//				   zky_cubeid=\"0\">
	// 		<files>
	// 		 	<file zky_physicalid=\"1\" 
	// 		 		  zky_from_task_docid=\"5103\" 
	// 		 		  zky_datacolid=\"dp_groupby_t1_stat_model_output\" 
	// 		 		  type=\"output_iil\" 
	// 		 		  zky_storage_fileid=\"9223372036854775910\"//>
	//		</files>
	//      <versions>
	//			<ver_0><![CDATA[libDataCubicJimos.so]]></ver_0>
	//      </versions>
	//	</dataconnector>
	//</datascanner>

	vector<AosXmlTagPtr> input_datasets;
	AosXmlTagPtr dataset_tag = def->getFirstChild("dataset");
	while(dataset_tag)
	{
		input_datasets.push_back(dataset_tag);
		dataset_tag = def->getNextChild("dataset");
	}
	aos_assert_r(input_datasets.size() == 2, false);

	dataset_tag = input_datasets[0];
	aos_assert_r(dataset_tag, false);

	vector<AosXmlTagPtr> datasets1;
	bool rslt = splitOneDataset(rdata, dataset_tag, datasets1, job_docid);
	for (u32 i=0; i<datasets1.size(); i++)
	{
		AosXmlTagPtr dataset_file_tag = datasets1[i];
		aos_assert_r(dataset_file_tag, false);

		AosXmlTagPtr dataset_query_tag = input_datasets[1];
		aos_assert_r(dataset_query_tag, false);
		rslt = splitFileAndQuery(job_docid, dataset_file_tag, dataset_query_tag, datasets, rdata);
		aos_assert_r(rslt, false);
	}
	return true;
}

bool 
AosDatasetSplit::splitNorm(
		const AosRundataPtr &rdata, 
		const vector<AosXmlTagPtr> &input_datasets,
		vector<AosXmlTagPtr> &datasets,
		const u64 &job_docid)
{
	aos_assert_r(input_datasets.size() == 1, false);
	vector<AosXmlTagPtr> datasets1;
	splitOneDataset(rdata, input_datasets[0], datasets1, job_docid);
	
	for (size_t i=0; i<datasets1.size(); i++)
	{
		OmnString xml_str = "";			
		xml_str << "<input_datasets>" << datasets1[i]->toString() 
				<< "</input_datasets>";
		AosXmlTagPtr xml = AosXmlParser::parse(xml_str AosMemoryCheckerArgs);
		aos_assert_r(xml, false);
		datasets.push_back(xml);
	}
	return true;
}

bool 
AosDatasetSplit::splitFileJoin(
		const AosRundataPtr &rdata, 
		const vector<AosXmlTagPtr> &input_datasets,
		vector<AosXmlTagPtr> &datasets,
		const u64 &job_docid)
{
	aos_assert_r(input_datasets.size() == 2, false);
	vector<AosXmlTagPtr> datasets1;
	splitOneDataset(rdata, input_datasets[0], datasets1, job_docid);
	aos_assert_r(datasets1.size() == 1, false);

	vector<AosXmlTagPtr> datasets2;
	splitOneDataset(rdata, input_datasets[1], datasets2, job_docid);
	aos_assert_r(datasets2.size() == 1, false);
	
	for (size_t i=0; i<datasets1.size(); i++)
	{
		OmnString xml_str = "";			
		xml_str << "<input_datasets>" 
			    << datasets1[i]->toString() 
				<< datasets2[i]->toString()
				<< "</input_datasets>";
		AosXmlTagPtr xml = AosXmlParser::parse(xml_str AosMemoryCheckerArgs);
		aos_assert_r(xml, false);
		datasets.push_back(xml);
	}
	return true;
}

bool 
AosDatasetSplit::splitNormJoin(
		const AosRundataPtr &rdata, 
		const vector<AosXmlTagPtr> &input_datasets,
		vector<AosXmlTagPtr> &datasets,
		const u64 &job_docid)
{
	aos_assert_r(input_datasets.size() == 2, false);

	bool rslt = false;
	int shuffle_id = -1;
	vector<AosXmlTagPtr> lhs_datasets;
	rslt = splitOneDataset(rdata, input_datasets[0], lhs_datasets, job_docid);
	aos_assert_r(rslt, false);

	map<int, AosXmlTagPtr> lhs_map;
	for (u32 i = 0; i < lhs_datasets.size(); i++)
	{
		AosXmlTagPtr datascanner_tag = lhs_datasets[i]->getFirstChild("datascanner");
		aos_assert_r(datascanner_tag, false);

		AosXmlTagPtr connector_tag = datascanner_tag->getFirstChild("dataconnector");
		aos_assert_r(connector_tag, false);
		shuffle_id = connector_tag->getAttrInt(AOSTAG_SHUFFLE_ID, 0);
		aos_assert_r(lhs_map.count(shuffle_id) == 0, false);
		lhs_map[shuffle_id] = lhs_datasets[i];
	}

	vector<AosXmlTagPtr> rhs_datasets;
	rslt = splitOneDataset(rdata, input_datasets[1], rhs_datasets, job_docid);
	aos_assert_r(rslt, false);

	map<int, AosXmlTagPtr> rhs_map;
	for (u32 i = 0; i < rhs_datasets.size(); i++)
	{
		AosXmlTagPtr datascanner_tag = rhs_datasets[i]->getFirstChild("datascanner");
		aos_assert_r(datascanner_tag, false);

		AosXmlTagPtr connector_tag = datascanner_tag->getFirstChild("dataconnector");
		aos_assert_r(connector_tag, false);
		shuffle_id = connector_tag->getAttrInt(AOSTAG_SHUFFLE_ID, 0);
		aos_assert_r(rhs_map.count(shuffle_id) == 0, false);
		rhs_map[shuffle_id] = rhs_datasets[i];
	}

	map<int ,AosXmlTagPtr>::iterator itr;

	aos_assert_r(lhs_map.size() == rhs_map.size(), false);
	AosXmlTagPtr lhs, rhs;
	for (itr = lhs_map.begin(); itr != lhs_map.end(); itr++ )
	{
		lhs = itr->second;
		aos_assert_r(rhs_map.count(itr->first) == 1, false);
		rhs = rhs_map[itr->first];

		OmnString xml_str = "";			
		xml_str << "<input_datasets>" 
				<< lhs->toString()
				<< rhs->toString()
				<< "</input_datasets>";
		AosXmlTagPtr xml = AosXmlParser::parse(xml_str AosMemoryCheckerArgs);
		aos_assert_r(xml, false);
		datasets.push_back(xml);
	}

	return true;
}

bool 
AosDatasetSplit::splitOneDataset(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &conf,
		vector<AosXmlTagPtr> &datasets,
		const u64 &job_docid)
{
	AosXmlTagPtr dataset_conf = conf;
	dataset_conf = AosDatasetObj::getDatasetConfig(rdata, dataset_conf);
	aos_assert_r(dataset_conf, false);

	AosXmlTagPtr scanner_conf = dataset_conf->getFirstChild("datascanner");
	if (!scanner_conf)
	{
		datasets.push_back(dataset_conf);
		return true;
	}
	aos_assert_r(scanner_conf, false);
	AosXmlTagPtr conn_conf = scanner_conf->getFirstChild("dataconnector");
	aos_assert_r(conn_conf, false);

	OmnString xml_str;
	AosXmlTagPtr xml;
	AosXmlTagPtr split_conf = conn_conf->getFirstChild("split");
	if (!split_conf) 
	{
		generateNoSplitDatasetConf(rdata, conn_conf, dataset_conf);
		datasets.push_back(dataset_conf);
		OmnTagFuncInfo << "seems no split. Xml config is: << "
			<< xml->toString() << endl;
		return true;
	}

	AosRundata *rdata_raw = rdata.getPtr();
	AosDataSplitterObjPtr splitter = AosCreateDataSplitter(rdata_raw, split_conf);
	aos_assert_r(splitter, false);
	splitter->setJobDocid(job_docid);
	bool rslt = splitter->config(rdata, conn_conf);
	aos_assert_r(rslt, false);

	vector<AosXmlTagPtr> data_units;
	rslt = splitter->split(rdata_raw, data_units);
	aos_assert_r(rslt, false);

	if (data_units.empty())
	{
		datasets.push_back(dataset_conf);
		return true;
	}

	for (size_t i=0; i<data_units.size(); i++)
	{
		xml = dataset_conf->clone(AosMemoryCheckerArgsBegin);
		scanner_conf = xml->getFirstChild("datascanner");
		scanner_conf->removeNode("dataconnector", true, true);
		scanner_conf->addNode(data_units[i]);
		datasets.push_back(xml);
	}
	return true;
}

bool 
AosDatasetSplit::splitByCubePair(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &def,
		vector<AosXmlTagPtr> &datasets,
		const u64 &job_docid)
{
	// Ketty 2014/08/20
	aos_assert_r(def, false);
	
	vector<AosXmlTagPtr> vt_dataset_conf;
	AosXmlTagPtr scanner_conf, conn_conf;
	int cube_id;
	map<u32, OmnString> conf_str_map;
	map<u32, OmnString>::iterator itr;
	OmnString conf_str;

	AosXmlTagPtr dataset_conf = def->getFirstChild("dataset");
	aos_assert_r(dataset_conf, false);
	while(dataset_conf)
	{
		vt_dataset_conf.clear();
		splitOneDataset(rdata, dataset_conf, vt_dataset_conf, job_docid);
		for(u32 i=0; i<vt_dataset_conf.size(); i++)
		{
			scanner_conf = vt_dataset_conf[i]->getFirstChild("datascanner");
			aos_assert_r(scanner_conf, false);
			conn_conf = scanner_conf->getFirstChild("dataconnector");
			aos_assert_r(conn_conf, false);

			cube_id = conn_conf->getAttrInt(AOSTAG_CUBE_ID, -1);

			if (cube_id == -1)
			{
				datasets.push_back(def);
				return true;
			}
			//aos_assert_r(cube_id != -1, false);
		
			itr = conf_str_map.find(cube_id);
			if(itr == conf_str_map.end())
			{
				conf_str = "<input_datasets ";
				conf_str << AOSTAG_CUBE_ID << "=\"" << cube_id << "\" >";

				pair< map<u32, OmnString>::iterator, bool> pr;
				pr = conf_str_map.insert(make_pair(cube_id, conf_str));
				aos_assert_r(pr.second, false);	
				itr = pr.first;
			}
		
			itr->second << vt_dataset_conf[i]->toString(); 
		}

		dataset_conf = def->getNextChild("dataset");
	}
	
	AosXmlParser parser;
	AosXmlTagPtr each_conf;
	for(itr = conf_str_map.begin(); itr != conf_str_map.end(); itr++)
	{
		OmnString & crt_conf_str = itr->second;
		crt_conf_str << "</input_datasets>"; 
		
		each_conf = parser.parse(crt_conf_str, "" AosMemoryCheckerArgs); 
		datasets.push_back(each_conf);
	}
	
	return true;
}


bool
AosDatasetSplit::generateNoSplitDatasetConf(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &conn_conf, 
		AosXmlTagPtr &dataset_conf)
{
	// Ketty 2014/11/24
	aos_assert_r(conn_conf, dataset_conf);
		
	OmnString file_defname = conn_conf->getAttrStr("zky_file_defname", "");	
	if(file_defname == "") return false;
	
	AosXmlTagPtr files_xml = AosGetDocByObjid(file_defname, rdata); 
	aos_assert_r(files_xml, false);

	OmnString str = "<dataconnector zky_objid=\"dataconnector_idfiles_jimodoc_v0\" ";
	str << "zky_otype=\"zkyotp_jimo\" jimo_name=\"jimo_datacube\" " 
		<< "jimo_type=\"jimo_datacube\" current_version=\"0\" "
		<< "zky_classname=\"AosDataCubeIdFiles\" type=\"idfiles\">"
		<< "<files>";
	
	AosXmlTagPtr file_xml = files_xml->getFirstChild(true);
	while(file_xml)
	{
		str << file_xml->toString(); 
		file_xml = files_xml->getNextChild();
	}

	str << "</files>"
		<< "<versions><ver_0>libDataCubicJimos.so</ver_0>"
		<< "</versions></dataconnector>";
	
	AosXmlTagPtr new_conn = AosXmlParser::parse(str AosMemoryCheckerArgs); 
	AosXmlTagPtr scanner_conf = dataset_conf->getFirstChild("datascanner");
	scanner_conf->removeNode("dataconnector", true, true);
	scanner_conf->addNode(new_conn);
	return true;
}


bool
AosDatasetSplit::checkIsBoundary(char *data, int data_len)
{
	int offset = 0;
	int loop = 0;
	while (offset < data_len && loop < 10)
	{
		int rcd_len = *((int*)(data + offset));
		if (AosBuff::decodeRecordBuffLength(rcd_len))
		{
			offset += sizeof(int) + rcd_len;
			loop++;
		}
		else
		{
			return false;
		}
	}
	if (loop < 10 && offset != data_len)
		return false;
	return true;
}


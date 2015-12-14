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
// It asumes:
// 1. Records are fixed length. 
// 2. Records are delimited by a pattern. The default pattern is '\n'.  
//    The delimiter characters never appear in records. 
// 3. The schema is selected:
// 	  a. By length
// 	  b. By contents
// 	  c. By a time value
//
// 2013/10/30 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Schema/Jimos/SchemaFixedLen.h"

#include "API/AosApi.h"
#include "SEInterfaces/SchemaPickerObj.h"
#include "SEInterfaces/ConditionObj.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosSchemaFixedLen_0(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosSchemaFixedLen(version);
		aos_assert_rr(jimo, rdata, 0);
		rdata->setJimo(jimo);
		return jimo;
	}

	catch (...)
	{
		AosSetErrorU(rdata, "Failed creating jimo") << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
}


AosSchemaFixedLen::AosSchemaFixedLen(const int version)
:
AosSchema(AOSSCHEMA_FIXEDLEN, version)
{
}


AosSchemaFixedLen::~AosSchemaFixedLen()
{
}


bool
AosSchemaFixedLen::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc)
{
	// <dataschema 
	// 	  zky_name="xxx" 
	// 	  jimo_objid="dataschema_fixedlen_jimodoc_v0" 
	// 	  pattern="xxx"  
	//    ignore_sub_pattern="true|false"  
	//    last_entry_with_nopattern="true|false" 
	//    max_record_length="xxx" 
	//    skip_invalid_records="true|false">
	//    <datarecords>
	//    	   <record zky_objid="xxx" record_name="xxx" id="xxx"/>
	//    	   <record zky_objid="xxx" record_name="xxx" id="xxx"/>
	//    	   .........
	//    	   <record zky_objid="xxx" record_name="xxx" id="xxx"/>
	//    	   <record zky_objid="xxx" record_name="xxx" id="xxx"/>
	//    	   <record zky_objid="xxx" record_name="xxx" id="xxx"/>
	//    </records>
	//    <schema_picker jimo_objid="xxx"/>
	//	  <AOSTAG_FILTER .../>	// not supported yet.
	// </schema>
	//
	// CREATE STATIC SCHEMA <schema-name>
	// TYPE fixedlen-multi
	// ID START POSITION <ddd> LENGTH <ddd>
	// RECORDS 
	// {
	// 		("vc", record-01),
	// 		("sg", record-02),
	// 		("vg", record-03),
	// 	}
	//
	// </dataschema>

	if (!worker_doc)
	{
		AosSetEntityError(rdata, "schemafxlen_missing_config", "Schema", mSchemaName)
			<< enderr;
		return false;
	}
	mSchemaName = worker_doc->getAttrStr("zky_name");
	AosXmlTagPtr records = worker_doc->getFirstChild("datarecords");
	if (!records)
	{
		AosSetEntityError(rdata, "schemafxlen_missing_records", "Schema", mSchemaName)
			<< worker_doc << enderr;
		return false;
	}

	AosXmlTagPtr picker = worker_doc->getFirstChild("schema_picker");
	if (!picker)
	{
		AosSetEntityError(rdata, "schemafixedlen_miss_spicker", "Schema", mSchemaName)
			<< worker_doc << enderr;
		return false;
	}
	mSchemaPicker = AosSchemaPickerObj::createSchemaPickerStatic(rdata.getPtr(), picker);
	if (!mSchemaPicker)
	{
		AosSetEntityError(rdata, "schemafixedlen_inv_schmpicker", "Schema", mSchemaName)
			<< worker_doc << enderr;
		return false;
	}

   	AosXmlTagPtr tag = records->getFirstChild();
	OmnString sdoc_objid;
	AosXmlTagPtr record;
	AosDataRecordObjPtr rec;
	int idx = 0;
	while(tag)
	{
		//sdoc_objid = tag->getAttrStr(AOSTAG_OBJID, "");
		//if (sdoc_objid == "")
		//{
		//	AosSetEntityError(rdata, "schemafixedlen_missing_sdoc_objid:", "Schema", mSchemaName) 
		//		<< tag << enderr;
		//	return false;
		//}
		//
		//record = AosGetDocByObjid(sdoc_objid, rdata);
		record = tag->getFirstChild(true);
		if (!record)
		{
			AosSetEntityError(rdata, "schemafixedlen_can not find sdoc:", "Schema", mSchemaName) 
				<< "Objid: " << sdoc_objid << tag << enderr;
			return false;
		}
	
		rec = AosDataRecordObj::createDataRecordStatic(record, 0, rdata.getPtr() AosMemoryCheckerArgs);
		if (!rec)
		{
			AosSetEntityError(rdata, "schemafixedlen_failed_create_rcd", "Schema", mSchemaName) 
				<< tag << enderr;
			return false;
		}

		OmnString rcd_name = record->getAttrStr(AOSTAG_NAME, "");
		if (rcd_name != "")
		{
			rec->setRecordName(rcd_name);                 
		}
		mRecords.push_back(rec);
	
		mSchemaPicker->addSchemaIndex(tag, idx);
		idx++;
		tag = records->getNextChild();
	}
/*
	mFilter = 0;
	AosXmlTagPtr filter_tag = worker_doc->getFirstChild(AOSTAG_FILTER);
	if (filter_tag)
	{
	    mFilter = AosConditionObj::getConditionStatic(filter_tag, rdata);
		if (!mFilter)
		{
			AosSetEntityError(rdata, "schemafixlen_faild_create_filter", "Schema", mSchemaName)
				<< tag << enderr;
			return false;
		}
	}
*/

	mPattern = AosParseRowDelimiter(worker_doc->getAttrStr("pattern"));
	if (mPattern == "")
	{
		AosSetEntityError(rdata, "schemafixlen_missing_patter", "Schema", mSchemaName)
			<< tag << enderr;
		return false;
	}

	mPatternLen = mPattern.length();
	if (mPatternLen <= 0)
	{
		AosSetEntityError(rdata, "schemafixlen_pattern_empty", "Schema", mSchemaName)
			<< tag << enderr;
		return false;
	}

	mIgnoreSubPattern = worker_doc->getAttrBool("ignore_sub_pattern", true);

	mLastEntryWithNoPattern = worker_doc->getAttrBool("last_entry_with_nopattern", false);

	mMaxRcdLen = worker_doc->getAttrInt64("max_record_length", eDftMaxRcdLen);
	
	mSkipInvalidRecords = worker_doc->getAttrBool("skip_invalid_records", true);

	//mCheckValidRecord = worker_doc->getAttrBool("check_valid_record", true);
	return true;
}


void 
AosSchemaFixedLen::setTaskDocid(const u64 task_docid)
{
	for(size_t i=0; i<mRecords.size(); i++)
	{
		mRecords[i]->setTaskDocid(task_docid);
	}
}

bool
AosSchemaFixedLen::nextRecordset(
		AosRundata *rdata_raw, 
		AosRecordsetObjPtr &recordset, 
		const AosBuffDataPtr &raw_data, 
		AosDatasetObj *dataset,
		bool &contents_incomplete)
{
	// This function retrieves a recordset from 'raw_data' at the position
	// 'offset'. The input 'raw_data' is the raw data
	// same length and same schema. This function parses the number of
	// records requested by 'recordset' from 'raw_data'. Upon success, 
	// 'offset' moved to the first unconsumed byte, if any, or the end
	// of 'raw_data', 'recordset' holds the parsed records. 
	
	aos_assert_rr(raw_data, rdata_raw, false);
//	aos_assert_rr(recordset, rdata, false);

	const char *pattern_data = mPattern.data();
	aos_assert_rr(pattern_data, rdata_raw, false);
	aos_assert_rr(mPatternLen > 0, rdata_raw, false);

	AosRecordsetObj *recordset_raw;
	if (!recordset)
	{
		recordset = dataset->cloneRecordset(rdata_raw);
		aos_assert_r(recordset, false);
		recordset_raw = recordset.getPtr();
		recordset_raw->holdRecords(rdata_raw, mRecords);
	
		//mSchemaPicker->reset();
		//mSchemaPicker->holdRecords(recordset->getRecords(), rdata);
	}
	else
	{
		recordset_raw = recordset.getPtr();
	}

	recordset->reset();
	recordset->setData(rdata_raw, raw_data->getBuff());
	contents_incomplete = false;
	int64_t crt_start_pos = raw_data->getCrtIdx();
	aos_assert_rr(crt_start_pos >= 0, rdata_raw, false);
	const char * crt_data = raw_data->data();
	int64_t crt_data_len = raw_data->dataLen();
	aos_assert_rr(crt_data, rdata_raw, false);
	aos_assert_rr(crt_data_len - crt_start_pos > 0, rdata_raw, false);

	//int num_rcds_to_add = recordset_raw->getNumRecordsToAdd();
	//aos_assert_rr(num_rcds_to_add > 0, rdata_raw, false);

	int rcd_len;
	bool status = true;
	int64_t crt_pos = crt_start_pos+1;
	if (crt_data_len - crt_pos <= 0)
	{
		rcd_len = crt_data_len - crt_start_pos;
		return procFinish(rdata_raw, raw_data, dataset, recordset_raw, rcd_len, contents_incomplete, crt_data, crt_start_pos, crt_pos, crt_data_len);
	}

	//while (!recordset->isFull() && crt_pos < crt_data_len)
	//while (num_rcds_to_add > 0 && crt_pos < crt_data_len)
	while (crt_pos < crt_data_len)
	{
		// Scan the data all the way to the end of the record or the
		// end of the data.
		while (crt_pos < crt_data_len && crt_data[crt_pos] != pattern_data[0])
		{
			crt_pos++;
		}

		if (crt_pos >= crt_data_len) 
		{
			rcd_len = crt_data_len - crt_start_pos;
			return procFinish(rdata_raw, raw_data, 
				dataset, recordset_raw, rcd_len, contents_incomplete, crt_data, crt_start_pos, crt_pos, crt_data_len);
		}

		// The first character of the record delimiter has been found.
		// Parse the record delimiter
		rcd_len = crt_pos - crt_start_pos;
		int k = 1;
		while (k < mPatternLen)
		{
			crt_pos++;
			if (crt_pos >= crt_data_len) 
			{
				return procFinish(rdata_raw, raw_data, 
					dataset, recordset_raw, rcd_len, contents_incomplete, crt_data, crt_start_pos, crt_pos, crt_data_len);
			}

			if (crt_data[crt_pos] != mPattern[k])
			{
				// The record delimiter mismatches. 
				if (mIgnoreSubPattern)	break;
				crt_pos--;
				if (!handleUnrecogRecord(rdata_raw, dataset, raw_data, true, crt_data, crt_start_pos, crt_pos, crt_data_len)) return false;
				break;
			}
			k++;
		}

		if (k < mPatternLen)
		{
			// The pattern wasn't correct but the error has been processed
			// properly. 'crt_start_pos' and 'crt_pos' now point to the right position.
			continue;
		}
	
		// Valid record delimiter has been found. 'crt_pos' now points to the last
		// delimiter character. 
		// Check whether the record length is too long.
		if (mMaxRcdLen > 0 && rcd_len > mMaxRcdLen)
		{
			// This is an invalid record.
			if (!handleUnrecogRecord(rdata_raw, dataset, raw_data, false, crt_data, crt_start_pos, crt_pos, crt_data_len)) return false;
			continue;
		}

		// A correct record has been found. Ready to create the record. 
		// 'crt_pos' points to the last record delimiter character.
		if (!createRecord(rdata_raw, dataset, raw_data, recordset_raw, rcd_len, crt_data, crt_start_pos, crt_pos, crt_data_len)) return false;
		//num_rcds_to_add = recordset_raw->getNumRecordsToAdd();
	}

	raw_data->setCrtIdx(crt_start_pos);
	return status;
}


bool
AosSchemaFixedLen::createRecord(
		AosRundata *rdata, 
		AosDatasetObj* dataset, 
		const AosBuffDataPtr &raw_data,
		AosRecordsetObj *recordset,
		const int rcd_len,
		const char * crt_data,
		int64_t	&crt_start_pos,
		int64_t &crt_pos,
		int64_t &crt_data_len) 
{
	// This function creates a record. 'crt_start_pos' points to the start
	// of the record. 'crt_pos' points to the last character of the record
	// delimiter.
	aos_assert_rr(mSchemaPicker, rdata, false);
	//AosDataRecordObj * record = mSchemaPicker->pickSchema(
	//		rdata, &crt_data[crt_start_pos], rcd_len+mPatternLen);
	//if (!record)
	//{
		// The record is unrecognized
	//	if (!handleUnrecogRecord(rdata, dataset, raw_data, false)) return false;
	//	return true;
	//}
	int	idx = mSchemaPicker->pickSchemaIndex(rdata, &crt_data[crt_start_pos], rcd_len+mPatternLen);
	if (idx < 0)
	{
		// The record is unrecognized
		if (!handleUnrecogRecord(rdata, dataset, raw_data, false, crt_data, crt_start_pos, crt_pos, crt_data_len)) return false;
		return true;
	}
	//bool data_being_set = false;

	// Check valid records, incomplete ??????????
	/*
	if (mCheckValidRecord)
	{
		AosDataRecordObj * record = recordset->getRawRecord(idx);
		aos_assert_r(record, false);
		record->setData((char *)&crt_data[crt_start_pos], rcd_len+mPatternLen, 0, 0, false);
		data_being_set = true;
		if (!record->isValid(rdata))
		{
			dataset->addInvalidContents(rdata, record);
			crt_start_pos = ++crt_pos;
			crt_pos++;
			return true;
		}
	}
	*/
	// Check filtering
/*
	if (mFilter)
	{
		AosDataRecordObj * record = recordset->getRawRecord(idx);
		aos_assert_r(record, false);
		if (!data_being_set) 
		{
			record->setData((char *)&crt_data[crt_start_pos], rcd_len+mPatternLen, 0, 0, false);
		}
		AosDataRecordObjPtr rec = record;
		if (!mFilter->evalCond(rec, rdata))
		{
			dataset->addFilterContents(rdata, record);
			crt_start_pos = ++crt_pos;
			crt_pos++;
			return true;
		}
	}
*/
//	OmnString rcd_str(&crt_data[crt_start_pos], rcd_len+mPatternLen);
//OmnScreen << " ##### append Record : " << rcd_str << endl;

	AosMetaDataPtr metadata  = raw_data->getMetadata();
	recordset->appendRecord(rdata, idx, crt_start_pos, rcd_len+mPatternLen, metadata);
	crt_start_pos = ++crt_pos;
	crt_pos++;
	return true;
}


bool
AosSchemaFixedLen::handleUnrecogRecord(
		AosRundata *rdata, 
		AosDatasetObj * dataset,
		const AosBuffDataPtr &raw_data,
		const bool incomplete,
		const char * crt_data,
		int64_t	&crt_start_pos,
		int64_t &crt_pos,
		int64_t &crt_data_len) 
{
	// An invalid record is found. The invalid record starts at 
	// 'crt_start_pos' and ends at 'crt_pos'.  This function determines 
	// what to do. It returns false if it should stop the 
	// scanning. Otherwise, it skips the invalid records until
	// it is able to recover the errors. The invalid records
	// are collected to the garbbage collector.

	// It skips the invalid record. It scans all the way to 
	// the next valid delimiter. If a valid delimiter is found, 
	// the next character is the start of a record. 'crt_pos' points 
	// to that position and true is returned.
	if (incomplete)
	{
		while (crt_pos < crt_data_len)
		{
			while (crt_pos < crt_data_len && crt_data[crt_pos] != mPattern[0]) crt_pos++;
			if (crt_pos >= crt_data_len)
			{
				// This is the end of the data. 
				raw_data->setCrtIdx(crt_pos);
				int64_t len = crt_pos - crt_start_pos;
				dataset->addUnrecogContents(rdata, raw_data, crt_data, crt_start_pos, len);
				if (!mSkipInvalidRecords) return false;
				return true;
			}

			int k = 1;
			while (k < mPatternLen)
			{
				crt_pos++;
				if (crt_pos >= crt_data_len || crt_data[crt_pos] != mPattern[k]) break;
				k++;
			}

			if (k >= mPatternLen)
			{
				crt_pos++;
				break;
			}
		}
	}
	else
	{
		crt_pos++;
	}

	int64_t len = crt_pos - crt_start_pos;
	dataset->addUnrecogContents(rdata, raw_data, crt_data, crt_start_pos, len);
	crt_start_pos = crt_pos;
	crt_pos++;
	if (!mSkipInvalidRecords) 
	{
		raw_data->setCrtIdx(crt_pos);
		return false;
	}
	return true;
}


AosJimoPtr 
AosSchemaFixedLen::cloneJimo() const
{
	try
	{
		return OmnNew AosSchemaFixedLen(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating jimo" << enderr;
		return 0;
	}
}


bool
AosSchemaFixedLen::processLastRecord(
		AosRundata *rdata, 
		const AosBuffDataPtr &raw_data, 
		AosDatasetObj *dataset,
		AosRecordsetObj* recordset_raw,
		const int rcd_len,
		bool &contents_incomplete,
		const char * crt_data,
		int64_t	&crt_start_pos,
		int64_t &crt_pos,
		int64_t &crt_data_len) 
{
	if (mLastEntryWithNoPattern)
	{
		// It is the end of the inputs and it is not an error for the 
		// last record not containing the pattern
		createRecord(rdata, dataset, raw_data, recordset_raw, rcd_len, crt_data, crt_start_pos, crt_pos, crt_data_len);	
		contents_incomplete = false;
		raw_data->setCrtIdx(crt_data_len);
		return true;
	}
				
	//It is an error;
	dataset->addUnrecogContents(rdata, raw_data, crt_data, crt_start_pos, crt_pos - crt_start_pos);
	contents_incomplete = false;
	raw_data->setCrtIdx(crt_data_len);
	return true;
}


bool
AosSchemaFixedLen::procFinish(
		AosRundata *rdata, 
		const AosBuffDataPtr &raw_data, 
		AosDatasetObj *dataset,
		AosRecordsetObj* recordset_raw,
		const int rcd_len,
		bool &contents_incomplete,
		const char * crt_data,
		int64_t	&crt_start_pos,
		int64_t &crt_pos,
		int64_t &crt_data_len) 
{
	// It is the end of the data. Schema assumes that the original
	// data may be from a stream and 'raw_data' is only one block of 
	// it. When it comes to this point, we only know that 'raw_data'
	// has run out, but it does not necessarily mean it is the end
	// of the inputs. If it is not, the caller should fetch more data
	// and call this function again.
	if (raw_data->isEOF())  // ??? how to check
	{
//OmnScreen << " ####### End of File ####### " << endl;	
		return processLastRecord(rdata, raw_data, dataset, recordset_raw, 
				rcd_len, contents_incomplete, crt_data, crt_start_pos, crt_pos, crt_data_len);
	}
		
	// It is not the end of the inputs. The remaining contents is an incomplete
	// record. Need to update the offset and return.
	contents_incomplete = true;			
	raw_data->setCrtIdx(crt_start_pos);
	return true;
}
	
bool
AosSchemaFixedLen::getRecord(const OmnString &name, AosDataRecordObjPtr &record)
{
	record = 0;
	OmnString key; 
	aos_assert_r(mRecords.size() > 0, false);
	for(size_t i=0; i<mRecords.size(); i++)
	{
		key = mRecords[i]->getRecordName();
		if (name == key)
		{
			record = mRecords[i];
			break;
		}
	}
	return true;
}
	
bool
AosSchemaFixedLen::getRecords(vector<AosDataRecordObjPtr> &records)
{
	aos_assert_r(mRecords.size() > 0, false);
	records = mRecords;
	return true;
}


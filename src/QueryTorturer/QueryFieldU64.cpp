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
// 2013/09/08	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "BitmapEngine/Testers/QueryFieldStr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"

static bool sgInited = false;

AosQueryFieldStr::AosQueryFieldStr()
:
AosQueryField(),
mStartDocid(0),
mNumDocsToCreate(0)
{
}


AosQueryFieldStr::~AosQueryFieldStr()
{
}


bool
AosQueryFieldStr::init()
{
	if (!sgInited)
	{
		sgLock.lock();
		bool rslt = true;
		if (!sgInited)
		{
			rslt = initStatic();			
			sgInited = true;
		}

		sgLock.unlock();
		aos_assert_r(rslt, false);
	}

	mStartDocid = mTester->getStartDocid();

	mMinValue = 0xffffffffffffffffULL;
	mMaxValue = 0;

	return true;
}


bool
AosQueryFieldStr::initStatic()
{
	bool rslt = createFieldSwappers();
	aos_assert_r(rslt, false);
	return true;
}


bool
AosQueryFieldStr::createFieldSwappers()
{
	mFieldSwaps.resize(12);
	switch (mFieldValueMethod)
	{
	case 2:
		 mFieldSwaps[0] = SwapEntry(2, 14, 3, 12, 7, 10);
		 mFieldSwaps[1] = SwapEntry(3, 14, 4, 12, 6, 10);
		 mFieldSwaps[2] = SwapEntry(1, 14, 3, 12, 5, 10);
		 mFieldSwaps[3] = SwapEntry(4, 14, 2, 12, 6, 10);
		 mFieldSwaps[4] = SwapEntry(6, 14, 4, 12, 2, 10);
		 mFieldSwaps[5] = SwapEntry(7, 14, 6, 12, 5, 10);
		 mFieldSwaps[6] = SwapEntry(7, 14, 5, 12, 3, 10);
		 mFieldSwaps[7] = SwapEntry(2, 19, 3, 15, 7, 10);
		 mFieldSwaps[8] = SwapEntry(3, 19, 4, 15, 6, 13);
		 mFieldSwaps[9] = SwapEntry(1, 19, 3, 15, 5, 13);
		 mFieldSwaps[10] = SwapEntry(4, 19, 2, 15, 6, 13);
		 mFieldSwaps[11] = SwapEntry(6, 19, 4, 15, 2, 13);
		 return true;

	default:
		 break;
	}

	mFieldSwaps[0]  = SwapEntry(0, 2, 1, 3, 0, 0);
	mFieldSwaps[1]  = SwapEntry(0, 2, 1, 4, 0, 0);
	mFieldSwaps[2]  = SwapEntry(0, 2, 1, 5, 0, 0);
	mFieldSwaps[3]  = SwapEntry(0, 2, 1, 6, 0, 0);
	mFieldSwaps[4]  = SwapEntry(0, 2, 1, 7, 0, 0);
	mFieldSwaps[5]  = SwapEntry(0, 3, 1, 4, 0, 0);
	mFieldSwaps[6]  = SwapEntry(0, 3, 1, 5, 0, 0);
	mFieldSwaps[7]  = SwapEntry(0, 3, 1, 6, 0, 0);
	mFieldSwaps[8]  = SwapEntry(0, 3, 1, 7, 0, 0);
	mFieldSwaps[9]  = SwapEntry(0, 4, 1, 5, 0, 0);
	mFieldSwaps[10] = SwapEntry(0, 4, 1, 6, 0, 0);
	mFieldSwaps[11] = SwapEntry(0, 4, 1, 7, 0, 0);
	return true;
}


bool
AosQueryFieldStr::addContents(
		const OmnString &iilname,
		const int record_len,
		const vector<u64> &values, 
		const u64 docid_start) 
{
	OmnScreen << "To add contents. IILName: " << iilname
		<< ". Record Len: " << record_len
		<< ". Start Docid: " << docid_start << endl;

	AosCompareFunPtr comp_func = OmnNew AosFunStrU641(record_len);
	AosBuffArrayPtr buff_array = OmnNew AosBuffArray(
			comp_func, true, false, values.size() * record_len);

	u64 docid = docid_start;
	if (flag)
	{
		for (u32 i=0; i<values.size(); i++)
		{
			char buff[100];
			sprintf(buff, "%010llu", (long long unsigned int)values[i]);
			buff_array->addValue(buff, strlen(buff), docid++, mRundata);
			aos_assert_r(mRundata->getSiteid() == 100, false);
		}
	}	
	else
	{
		for (u32 i=0; i<values.size(); i++)
		{
			OmnString vv;
			vv << values[i];
			buff_array->addValue(vv.data(), vv.length(), docid++, mRundata);
			aos_assert_r(mRundata->getSiteid() == 100, false);
		}
	}

	buff_array->sort();

	AosBuffPtr bb = buff_array->getBuff();
	u64 fileid = createFile(bb, mRundata);
	aos_assert_r(mRundata->getSiteid() == 100, false);
	aos_assert_r(fileid != 0, false);

	OmnString sdoc_str;
	sdoc_str << "<action zky_type=\"iilbatchopr\" "
			 << AOSTAG_IILNAME << "=\"" << iilname << "\">" 
			 << "<datacollector>"
			 << "<asm " << AOSTAG_IILNAME << "=\"" << iilname 
			 << "\" " << AOSTAG_LENGTH << "=\"" << record_len 
			 << "\" " << AOSTAG_TYPE << "=\"stradd\""
			 << " /> "
			 << "</datacollector>"
			 << "<files>"
			 << "<file "
			 << AOSTAG_STORAGE_FILEID << "=\"" << fileid << "\" "
			 << AOSTAG_PHYSICALID << "=\"" << AosGetSelfServerId() << "\" "
			 << AOSTAG_START_POS << "=\"0\" "
			 << AOSTAG_LENGTH << "=\"" << record_len * values.size() << "\"" 
			 << " />"
			 << "</files>"
			 << "</action>";
	
	AosXmlTagPtr action_xml = AosStr2Xml(mRundata, sdoc_str AosMemoryCheckerArgs);
	aos_assert_r(mRundata->getSiteid() == 100, false);
	aos_assert_r(action_xml, false);
	
	mAction = AosActionObj::getAction(action_xml, mRundata);       
	aos_assert_r(mRundata->getSiteid() == 100, false);
	aos_assert_rr(mAction, mRundata, false);

	AosActIILBatchOprPtr act = dynamic_cast<AosActIILBatchOpr*>(mAction.getPtr());
	act->setTestFlag(true);
	AosTaskObjPtr task; 
	bool rslt = mAction->run(task, action_xml, mRundata);
	aos_assert_r(mRundata->getSiteid() == 100, false);
	aos_assert_r(rslt, false);
	while(!act->isFinished())
	{
		OmnSleep(1);
	}
	return true;
}


bool
AosQueryFieldStr::createDocs()
{
	// Conceptually, it creates a number of docs in the following format:
	// 	  <doc zky_docid="ddd" 
	// 	  	gruop_id="ddd" 	// u64
	// 	  	field_str_00="random_str"
	// 	  	...
	// 	  	field_str_nn="random_str"
	// 	  	field_u64_00="random_str"
	// 	  	...
	// 	  	field_u64_nn="random_str"/>
	//
	// Field values are calcuated based on its docid.
	bool rslt = createStrFields();
	aos_assert_r(rslt, false);

	rslt = createU64Fields();
	aos_assert_r(rslt, false);

	// rslt = createGroups();
	// aos_assert_r(rslt, false);

	return true;
}


bool
AosQueryFieldStr::createOneStrField(const int field_idx)
{
	// This function creates one string field. 
	aos_assert_r(field_idx >= 0 && (u32)field_idx < mFieldSwaps.size(), "");
	vector<u64> values;
	int64_t num_docs_created = 0;
	u64 docid = mStartDocid;

	OmnString log;
	OmnString fname = "str_field_";
	fname << field_idx << ".txt";
	OmnFile ff(fname, OmnFile::eCreate AosMemoryCheckerArgs);
	while (num_docs_created < mNumDocsToCreate)
	{
		// Fields are created in blocks in case there are too many 
		// fields. It randomly determines the block size.
		values.clear();
		u32 size = (u32)OmnRandom::nextInt(mMinBlockSize, mMaxBlockSize);
		if (num_docs_created + size > mNumDocsToCreate)
		{
			size = mNumDocsToCreate - num_docs_created;
		}

		for (u32 i=0; i<size; i++)
		{
			// There are (currently) 12 field byte swaps. Each field will
			// use one field byte swap to convert 'docid' into another 
			// u64 value, which is used as the field value.
			u64 vv = getStrValue(field_idx, docid+i);
			log << vv << ":" << docid+i << "\n";
			values.push_back(vv);
			if (sgShowLogLevel2)
			{
				OmnScreen << "Add value for str field: " << size 
					<< ":" << i << ":" << field_idx << ":"
					<< docid+i << ":" << vv << endl;
			}
		}

		OmnString iilname = getStrIILName(field_idx);
		addContents(iilname, mRecordLen, values, docid);
		ff.append(log.data(), log.length(), false);
		log = "";
		docid += size;
		num_docs_created += size;
	}
	return true;
}


bool
AosQueryFieldStr::createOneU64Field(const int field_idx)
{
	// This function creates one string field. 
	vector<u64> values;
	int64_t num_docs_created = 0;
	u64 docid = mStartDocid;
	while (num_docs_created < mNumDocsToCreate)
	{
		// 1. Randomly determine the block size.
		values.clear();
		u32 size = (u32)OmnRandom::nextInt(mMinBlockSize, mMaxBlockSize);
		if (num_docs_created + size > mNumDocsToCreate)
		{
			size = mNumDocsToCreate - num_docs_created;
		}

		for (u32 i=0; i<size; i++)
		{
			values.push_back(getU64Value(field_idx, docid + i));
		}

		OmnString iilname = getU64IILName(field_idx);
		addContents(iilname, mRecordLen, values, docid);
		docid += size;
		num_docs_created += size;
	}
	return true;
}


u64
AosQueryFieldStr::getStrValue(const int field_idx, const u64 docid)
{
	// The value is built based on 'docid' by swapping some bits.
	aos_assert_r(field_idx >= 0 && (u32)field_idx < mFieldSwaps.size(), 0);

	int idx = mFieldSwaps.size() - field_idx - 1;

	u64 new_docid;
	switch (mFieldValueMethod)
	{
	case 2:
		 new_docid = docid;
		 new_docid = new_docid | (((docid >> mFieldSwaps[idx].from[0]) & 0x01) << mFieldSwaps[idx].to[0]);
		 new_docid = new_docid | (((docid >> mFieldSwaps[idx].from[1]) & 0x01) << mFieldSwaps[idx].to[1]);
		 new_docid = new_docid | (((docid >> mFieldSwaps[idx].from[2]) & 0x01) << mFieldSwaps[idx].to[2]);
		 return new_docid;

	default:
		 break;
	}

	u64 value1 = docid;
	u64 value2 = docid;
	u8 *data1 = (u8*)&value1;
	u8 *data2 = (u8*)&value2;
	data2[mFieldSwaps[idx].to[0]] = data1[mFieldSwaps[idx].from[0]];
	data2[mFieldSwaps[idx].to[1]] = data1[mFieldSwaps[idx].from[1]];
	return value2;
}


u64
AosQueryFieldStr::getU64Value(
		const int field_idx, 
		const u64 docid)
{
	// The value is built based on 'docid' by swapping two bytes.
	aos_assert_r(field_idx >= 0 && (u32)field_idx < mFieldSwaps.size(), 0);

	u64 value1 = docid;
	u64 value2 = docid;
	u8 *data1 = (u8*)&value1;
	u8 *data2 = (u8*)&value2;
	data2[mFieldSwaps[field_idx].to[0]] = data1[mFieldSwaps[field_idx].from[0]];
	data2[mFieldSwaps[field_idx].to[1]] = data1[mFieldSwaps[field_idx].from[1]];
	return true;
}


bool 
AosQueryField::setMinMax(const u64 value)
{
	if (value < mMinValue) 
	{
		mMinValue = value;
	}

	if (value > mMaxValue)
	{
		mMaxValue = value;
	}
	return true;
}



OmnString 
AosQueryFieldStr::getU64IILName(const int field_idx)
{
	OmnString iilname = "_zt44_u64_index_";
	iilname << field_idx;
	return iilname;
}
#endif

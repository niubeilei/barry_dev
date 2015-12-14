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
// How to torture: 
// 1. Define a 100000 int record array. 
// 2. Randomly do one of the following:
//    a. Add a record, if possible
//    b. Add a cell by randomly pick a record, and then check whether
//       the record is full. If not, append a value.
//    c. Randomly pick a record and a value in the record. Add a random
//       value to the cell.
//    d. Randomly pick a record, check the values. 
//    e. Batch modify
//
// Modification History:
// 05/24/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataStructs/Testers/Vector2DTester.h"

#include "API/AosApi.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Porting/Sleep.h"
#include "SEInterfaces/Ptrs.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "Util/Ptrs.h"
#include "Util/ValueRslt.h"
#include "Util/DataTypes.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "SEInterfaces/TaskObj.h"

#include "DataStructs/Vector2D.h"
#include "DataStructs/Ptrs.h"
#include "DataStructs/StatIdIDGen.h"
#include "DataStructs/StatIdExtIDGen.h"


AosVector2DTester::AosVector2DTester()
:
mRundata(OmnNew AosRundata(AosMemoryCheckerArgsBegin)),
mVector(0),
mStatIdIDGen(0),
mXNum(0)
{
	mRundata->setSiteid(100);
	mRundata->setUserid(307);
	mName = "Vector2DTester";
}


AosVector2DTester::~AosVector2DTester()
{
}


bool 
AosVector2DTester::start()
{
	cout << "Start Vector2D Tester ..." << endl;
	basicTest();
	return true;
}


bool 
AosVector2DTester::basicTest()
{
	aos_assert_r(config(), false);
	bool rslt = createControlDoc();
	aos_assert_r(rslt, false);

	int tries = 100000;
	while(tries--)
	{
		rslt = run();
		aos_assert_r(rslt, false);
	}
	return true;
}


bool
AosVector2DTester::config()
{
	return true;
}

bool
AosVector2DTester::createControlDoc()
{
	// create ext ctrl doc
	OmnString docstr = "<binary_doc ";
	docstr << AOSTAG_OTYPE << "=\"zky_binarydoc\" "
		<< AOSTAG_CTNR_PUBLIC << "=\"true\" "
		<< AOSTAG_PUBLIC_DOC << "=\"true\""
		<< "zky_doc_per_distblock" << "=\"12500\""
		<< "></binary_doc>";

	AosBuffPtr buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);	
	buff->setInt64(0);

	AosXmlTagPtr doc = AosCreateBinaryDoc(-1, docstr, true, buff, mRundata);
	aos_assert_r(doc, false);
	OmnString  ext_objid = doc->getAttrStr(AOSTAG_OBJID, "");

	// 30 years
	// day: 30years * 366 = 10980 
	// bucket weigth: 100 day
	// index entry num : 10980 / 100 = 110
	//
	// num_index size: sizeof(u64)
	// index entry size: sizeof(u64) + sizeof(u64) = 110 * 16 + 8 = 1768 
	// other
	// key size: 300;
	// General value: sizeof(u64)
	// other_total = 308
	//
	// ext_doc_size = 100day * sizeof(int64_t) = 800;
	// root_doc_size = 800 + 1768 + 308 = 2876; 
	
	// ext zky_doc_per_distblock = AosDistBlock::eDfmDistBlockSize/ext_doc_size = 12500 
	// root zky_doc_per_distblock = AosDistBlock::eDfmDistBlockSize/root_doc_size  = 3477 
	
	int weight = 100;
	int index_size = 1768; 
	int extension_doc_size = 800;
	int stat_doc_size = 2876;

	// create root ctrl doc
	docstr = "<binary_doc ";
	docstr << AOSTAG_OTYPE << "=\"zky_binarydoc\" "
		<< AOSTAG_CTNR_PUBLIC << "=\"true\" "
		<< AOSTAG_PUBLIC_DOC << "=\"true\""
		<< "zky_doc_per_distblock" << "=\"3477\""
		<< "></binary_doc>";

	doc = AosCreateBinaryDoc(-1, docstr, true, buff, mRundata);
	aos_assert_r(doc, false);
	OmnString  root_objid = doc->getAttrStr(AOSTAG_OBJID, "");

	docstr = "<stat_vt2d_conf>";
	docstr << "<vector2d_control "
		<< "root_control_objid" << "=\"" << root_objid << "\" "
		<< "extension_control_objid" << "=\"" << ext_objid << "\" "
		<< AOSTAG_DATA_TYPE << "=\"" << AOSDATATYPE_INT64 << "\" />"
		<< "<vector2d "
		<< "root_stat_doc_size" << "=\"" << stat_doc_size << "\" "
		<< "index_size" << "=\"" << index_size << "\" "
		<< "time_bucket_weight" << "=\"" << weight << "\" "
		<< "start_time_slot" << "=\"0\" "
		<< "extension_doc_size" << "=\"" << extension_doc_size << "\" "
		<< "/>"
		<< "<agr_type type=\"sum\" />"
		<< "</stat_vt2d_conf>";

	AosXmlParser parser;
	AosXmlTagPtr stat_vt2d_conf = parser.parse(docstr, "" AosMemoryCheckerArgs);
	mVector = OmnNew AosVector2D(mRundata, stat_vt2d_conf);
	aos_assert_r(mVector, false);

	mStatIdIDGen = OmnNew AosStatIdIDGen("sdd_1", 100);

	//mStatIdExtIDGen = OmnNew AosStatIdExtIDGen(ext_ctrl_doc, 2000); 
	//for (int i = 0; i < 1000000000; i++)
	//{
	//	int vid = i % AosGetNumCubes();
	//	u64 docid = mStatIdExtIDGen->nextDocid(vid, mRundata);
	//	//OmnScreen << "vid:" << vid << ";docid:" << docid << ";"<< endl;
	//}
	return true;
}


bool
AosVector2DTester::run()
{
	int r = rand() % eOprMax;
	switch(r)
	{
	case eAddRcd:
		 //OmnScreen << "eAddRcd eAddRcd " << endl;
		 addRcd();
		 //batchAddCell();
		 break;

	case eAddBatchCell:
		 //OmnScreen << "eAddBatchCell eAddBatchCell " << endl;
		 ///batchAddCell();
		 break;

	case eCheckBatchCell:
		 //OmnScreen << "eCheckBatchCell eCheckBatchCell " << endl;
		 //batchCheckCell();
		 break;

	case eModBatchCell:
		 //OmnScreen << "eModBatchCell eModBatchCell " << endl;
		 //batchModCell();
		 break;

	case eBatchMod:
		 //OmnScreen << "eBatchMod eBatchMod " << endl;
		 //batchMod();
		 break;
	default:
		 OmnAlarm << "xxx" << enderr;
	}
	return true;
}


bool
AosVector2DTester::addRcd()
{
	mVaildDocids[mXNum] = mStatIdIDGen->nextDocid(mRundata);
	mArray[mXNum] = OmnNew vector<int64_t>; 
	mXNum ++;
	return true;
}

/*
bool
AosVector2DTester::batchAddCell()
{
	if (mXNum == 0) return true;
	int r = rand()%mXNum;
	vector<int64_t> * vv = mArray[r];
	if (vv->size() >= eMaxCell) return true;

	u64 docid = mVaildDocids[r];
	AosBuffPtr buff = createData(docid, vv);
	//return modifyLocal(docid, buff); 
	modifyLocal(docid, buff); 
	OmnScreen << "modifyLocal; docid:" << docid <<";num:" << vv->size() << endl;
	checkEntryAll(docid, vv);
	return true;
}

bool
AosVector2DTester::checkEntryAll(const u64 &docid, vector<int64_t> * vv)
{
	u64 n1 = 0;
	u64 time_slot = 0;
	u64 num = vv->size();
	while(n1 < num)
	{
		u64 start_idx = 0;
		AosBuffPtr buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
		bool rslt = getStatValue(docid, time_slot, 
				buff, start_idx);
		aos_assert_r(rslt, false);
		aos_assert_r(buff->dataLen() > 0, false);

		checkValue(docid, start_idx, buff->data(), buff->dataLen(), vv);

		n1 += 1000;
		time_slot += 1000;
	}
	return true;
}


AosBuffPtr
AosVector2DTester::createData(
		const u64 &docid,
		vector<int64_t> *vv)
{
	AosBuffPtr buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);	
	int r = rand() % 5000;
	for (int i = 0; i< r; i++)
	{
		u64 time_slot = vv->size();
		if (time_slot >= eMaxCell) break;

		buff->setU64(docid);
		time_slot = time_slot * 2;
		buff->setU64(time_slot);

		//int v1 = rand()%1000;
		//vv->push_back(v1);
		//buff->setInt64(v1);
		//vv->push_back(time_slot);
		for (u32 kk = vv->size(); kk <= time_slot; kk++)
		{
			vv->push_back(0);
		}

		(*vv)[time_slot] = time_slot;
		aos_assert_r(vv->size()-1 == time_slot , 0);
		buff->setInt64(time_slot);
	//OmnScreen << "docid:"<< docid << ";timeslot:" << time_slot << ";v1:" << time_slot << endl;
	}
	return buff;
}


bool
AosVector2DTester::batchModCell()
{
	if (mXNum == 0) return true;
	int r = rand()%mXNum;
	vector<int64_t> * vv = mArray[r];
	if (vv->size() <= 0)return true;
	u64 docid = mVaildDocids[r];

	AosBuffPtr buff = modifyData(docid, vv);
	if (buff->dataLen() == 0) return true;
	//return modifyLocal(docid, buff); 
	modifyLocal(docid, buff); 
	OmnScreen << "modifyLocal; docid:" << docid <<";num:" << vv->size() << endl;
	checkEntryAll(docid, vv);
	return true;
}


AosBuffPtr
AosVector2DTester::modifyData(const u64 &docid, vector<int64_t> *vv)
{
	AosBuffPtr buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);	
	if (vv->size() == 0) return buff;
	u32 size = vv->size();
	u32 start = rand() % size;
	u32 end = rand() % size; 
	if (start > end) 
	{
		u32 t = start;
		start = end;
		end = t;
	}

	while(start < end) 
	{
		buff->setU64(docid);
		buff->setU64(start);

		int v1 = rand()%100;
		//int v1 = i;
		(*vv)[start]+= v1;
		buff->setInt64(v1);
		start++;
	}
	return buff;
}


AosBuffPtr
AosVector2DTester::newAndModifyData(const u64 &docid, vector<int64_t> *vv)
{
	AosBuffPtr buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);	
	if (vv->size() == 0) return buff;
	u32 size = vv->size();
	u32 start = rand() % size;
	u32 end = rand() % size; 
	if (start > end) 
	{
		u32 t = start;
		start = end;
		end = t;
	}

	while(start < end) 
	{
		buff->setU64(docid);
		buff->setU64(start);

		int v1 = rand()%100;
		//int v1 = i;
		(*vv)[start]+= v1;
		buff->setInt64(v1);
		start++;
	}

	int r = rand() % 500;
	for (int i = 0; i< r; i++)
	{
		u64 time_slot = vv->size();
		if (time_slot >= eMaxCell) break;
		buff->setU64(docid);
		buff->setU64(time_slot);
		vv->push_back(time_slot);
		buff->setInt64(time_slot);
	}
	return buff;
}



bool
AosVector2DTester::batchCheckCell()
{
	if (mXNum == 0) return true;
	int r = rand()%mXNum;
	u64 docid = mVaildDocids[r];
	vector<int64_t> * vv = mArray[r];
	if (vv->size() == 0) return true;
	u64 time_slot = rand() % vv->size();

	u64 start_idx = 0;
	AosBuffPtr buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
	bool rslt = getStatValue(docid, time_slot, buff, start_idx);
	aos_assert_r(rslt, false);

	//u64 off = time_slot - start_idx;
	//u64 pos = off  * AosDataType::getValueSize(AosDataType::eInt64);
	//aos_assert_r(*(int64_t *)&stat_doc[pos] == (*vv)[time_slot], false);
	checkValue(docid, start_idx, buff->data(), buff->dataLen(), vv);
	return true;
}

bool
AosVector2DTester::getStatValue(
		const u64 &docid,
		const u64 &time_slot,
		const AosBuffPtr &buff,
		u64 &start_idx)
{
	bool rslt = mVector->getStatDoc(mRundata, docid, time_slot, 
			buff, start_idx, mProc);
	aos_assert_r(rslt, false);
	aos_assert_r(buff->dataLen() > 0, false);
	return true;
}


bool
AosVector2DTester::modifyLocal(const u64 &docid, const AosBuffPtr &buff)
{
	return mVector->modifyLocal(mRundata, buff, mProc);
}

bool
AosVector2DTester::modifyLocal(const AosBuffPtr &buff)
{
	return mVector->modifyLocal(mRundata, buff, mProc);
}

bool
AosVector2DTester::checkValue(
		const u64 &docid, 
		const u64 &start_idx, 
		char *data, 
		const int64_t &data_len,
		vector<int64_t> * vv)
{
	u64 idx = start_idx;
	int64_t crt_idx = 0;
	while(crt_idx < data_len)
	{	
		int64_t v1 = *(int64_t *)&data[crt_idx];
		if(idx < vv->size())
		{
			int64_t v2 = (*vv)[idx];
	//OmnScreen << "BB docid:"<< docid << ";timeslot:" << idx << ";v1:" << v1 << ";v2:" << v2 << endl;
			aos_assert_r(v1 == v2, false);
		}
		else
		{
			aos_assert_r(v1 == 0, false);
		}

		crt_idx += AosDataType::getValueSize(AosDataType::eInt64); 
		idx ++;
	}
	return true;
}


bool
AosVector2DTester::batchMod()
{
	if (mXNum == 0) return true;
	int t = rand() %20;
	AosBuffPtr data = OmnNew AosBuff(AosMemoryCheckerArgsBegin);	
	vector<int> v1;
	while(t)
	{
		int r = rand()%mXNum;
		vector<int64_t> * vv = mArray[r];
		if (vv->size() > 0)
		{
			u64 docid = mVaildDocids[r];

			AosBuffPtr buff = newAndModifyData(docid, vv);
			if (buff->dataLen() != 0)
			{
				data->setBuff(buff);
				v1.push_back(r);
			}
		}
		t --;
	}
	//return modifyLocal(data); 
	modifyLocal(data); 
	for (u32 i = 0; i<v1.size(); i++)
	{
		int r = v1[i];
		vector<int64_t> * vv = mArray[r];
		if (vv->size() <= 0)continue;
		u64 docid = mVaildDocids[r];
	OmnScreen << "modifyLocal; docid:" << docid << ";num:" << vv->size() << endl;
		checkEntryAll(docid, vv);
	}
	return true;
}
*/


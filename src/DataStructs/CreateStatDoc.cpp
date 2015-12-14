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
// This class implements a two-dimensional array:
// 	big_array[element_id][value]
// Elements are identified by integers, starting from 0. Elements are
// always appended. The array can grow by appending elements. There 
// is no logical limitations on the size of a big array (i.e., the 
// maximum number of elements a big array may contain), but it is 
// possible to add the maximum limitation on it.
//
// Each element may contain an array of values. Different elements may
// contain different number of values (in a sense, the sizes of elements
// are variable). Values are addressed by indexes.
//
// Modification History:
// 2014/06/26 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#include "DataStructs/Vector2D.h"

#include "API/AosApi.h"
#include "DataStructs/StructProc.h"
#include "DataStructs/DataStructsUtil.h"
#include "DataStructs/RootStatDoc.h"
#include "DataStructs/RemoteDistBlockMgr.h"
#include "DataStructs/LocalDistBlockMgr.h"
#include "Util/DataTypes.h"
#include "Thread/Sem.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadPool.h"

#include "StatTrans/SaveStatBinaryIdTrans.h"

static bool smShowLog = false;

bool 
AosVector2D::modifyLocal(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &buff,
		const int64_t &record_len,
		const int64_t &start_pos,
		const int64_t &end_pos,
		AosStrValueInfo	&statid_info,
		AosStrValueInfo	&time_info,
		AosStrValueInfo	&str_info,
		AosStrValueInfo	&value_info,
		const int num_thrds_running,
		const AosStructProcObjPtr &proc)
{
	// This function is similar to 'modify(...)' except that 
	// the data in 'buff' are all local (otherwise, it is an error).
	// 'buff' is in the following format:
	// 	[docid, time_id, v1, v2, v3, ..., key]
	// 	[docid, time_id, v1, v2, v3, ..., key]
	// 	...
	// 	[docid, time_id, v1, v2, v3, ..., key]
	
	OmnScreen << "AosVector2D AosVector2D ext_ctrl_objid:" << mCtrlDocObjid << ";"<< endl;
	aos_assert_rr(mDistBlockMgr && buff && record_len > 0, rdata, false);

	bool rslt;
	int64_t value;
	OmnString key;
	u64 stat_id, time_slot, time;

	AosBuffPtr entries = OmnNew AosBuff(AosMemoryCheckerArgsBegin);	
	map<u64, AosBuffPtr> kickOutStatDoc;

	int64_t crt_pos = start_pos;
	const char *data = buff->data();
	aos_assert_r(start_pos < end_pos && end_pos <= buff->dataLen(), false);
	aos_assert_rr((end_pos - start_pos) % record_len == 0, rdata, false);

	AosDistBlock dist_block(mRootStatDocSize, mParm.getDftValue());
	while (crt_pos < end_pos)
	{
		stat_id = *(u64*)&data[crt_pos + statid_info.field_offset];
		time = *(u64*)&data[crt_pos + time_info.field_offset];
		value = *(u64*)&data[crt_pos + value_info.field_offset];

		u32 len = strlen(&data[crt_pos + str_info.field_offset]);
		key.assign(&data[crt_pos + str_info.field_offset], len);
		key.normalizeWhiteSpace(true, true);  // trim

		time_slot = time - mStartTimeSlot;

		if (smShowLog)
		{
			OmnString ss;
			ss << "AosVector2D AosVector2D"
				<< ";ext_objid:" << mCtrlDocObjid 
				<< ";thrd_id:" << num_thrds_running
				<< ";statid:" << stat_id 
				<< ";time:" << time 
				<< ";time_slot:" << time_slot 
				<< ";key:" << key 
				<< ";value:" << value 
				<< ";";
			OmnScreen << ss << endl;
		}

		aos_assert_r(stat_id, false);
		rslt = modifyRecord(
				rdata, 
				dist_block, 
				stat_id, 
				time_slot, 
				value, 
				key, 
				entries, 
				kickOutStatDoc, 
				proc);
		aos_assert_r(rslt, false);
		crt_pos += record_len;
	}

	rslt = mDistBlockMgr->saveDistBlock(rdata, dist_block);
	aos_assert_r(rslt, false);

	rslt = modifyKickOutStatDoc(rdata, kickOutStatDoc);
	aos_assert_r(rslt, false);

	rslt = modifyExtensionEntry(rdata, entries, proc);
	aos_assert_r(rslt, false);

	return true;
}


bool
AosVector2D::modifyRecord(
		const AosRundataPtr &rdata,
		AosDistBlock &dist_block,
		const u64 &stat_id,
		const u64 &time_slot,
		const int64_t &value,
		const OmnString &key,
		const AosBuffPtr &entries,
		map<u64, AosBuffPtr> &kickOutStatDoc,
		const AosStructProcObjPtr &proc)
{
	// Ready to process the value. If the value is not in the root
	// stat doc, it will return the following three values:
	// 2. stat_doc_docid (identify the stat doc in which the value resides)
	// 3. ext_time_slot (relative index inside the stat doc)
	
	char *stat_doc = 0;
	int64_t stat_doc_len = 0; 
	bool rslt = mDistBlockMgr->getStatDoc(rdata, dist_block, 
			stat_id, true, stat_doc, stat_doc_len);
	aos_assert_r(rslt, false);
	aos_assert_r(stat_doc && stat_doc_len > 0, false);

	aos_assert_r(key != "", false);
	AosRootStatDoc root_stat_doc(stat_doc, stat_doc_len, mParm, stat_id, key);

	// Chen Ding, 2014/06/26
	bool out_range = false;
	rslt = root_stat_doc.modifyRootStatDoc(
			rdata,	
			time_slot, 
			value,
			dist_block.getDistBlockVid(),
			proc,
			out_range); 
	aos_assert_r(rslt, false);

	// Chen Ding, 2014/06/26
	if (out_range)
	{
		// This is an incorrect record. 
		return true;
	}

	AosRootStatDoc::ValueRslt value_rslt;
	value_rslt = root_stat_doc.getValueRslt();

	if (value_rslt.isKickOut())
	{
		for (u32 i = 0; i< value_rslt.kick_statdocs.size(); i++)
		{
			u64 kick_ext_docid = value_rslt.kick_extdocid[i];
			AosBuffPtr kick_statdoc = value_rslt.kick_statdocs[i];
			aos_assert_r(kick_statdoc && kick_statdoc->dataLen() == mExtensionStatDocSize, false);
			addKickOutStatDoc(kickOutStatDoc, kick_ext_docid, kick_statdoc);
		}
		return true;
	}

	if (value_rslt.ext_docid > 0)
	{
		// The time slot is in an extension doc whose statid 
		// is 'statid'. Need to add it to the buffer.
		addUpdateEntry(
				entries,
				stat_id, 
				value_rslt.ext_docid, 
				value_rslt.ext_slot_idx, 
				value_rslt.is_new,
				value);
		return true;
	}

	return true;
}

bool
AosVector2D::modifyKickOutStatDoc(
		const AosRundataPtr &rdata,
		map<u64, AosBuffPtr> &kickOutStatDoc)
{
	OmnScreen << "AosVector2D; modifyKickOutStatDoc; size:" <<  kickOutStatDoc.size()<< ";"<< endl;
	AosXmlTagPtr doc = 0;
	map<u64, AosBuffPtr>::iterator itr = kickOutStatDoc.begin();
	for (; itr != kickOutStatDoc.end(); ++itr)
	{
		u64 docid = itr->first;
		AosBuffPtr stat_doc = itr->second;
		aos_assert_r(stat_doc && stat_doc->dataLen() == mExtensionStatDocSize, false);
		doc = mDistBlockMgr->createBinaryDoc(rdata, docid, stat_doc);
		aos_assert_r(doc, false);
	}
	kickOutStatDoc.clear();
	return true;
}


bool 
AosVector2D::modifyExtensionEntry(
		const AosRundataPtr &rdata,
		const AosBuffPtr &entries,
		const AosStructProcObjPtr &proc)
{
	AosBuffPtr buff = 0;
	AosXmlTagPtr doc = 0;
	int64_t data_len = entries->dataLen();
	OmnScreen << "AosVector2D; modifyExtensionEntry data_len:" << data_len << ";"<< endl;
	if (data_len == 0) return true;

	aos_assert_rr(data_len % eExtRcdLen == 0, rdata, false);
	aos_assert_r(data_len >= eExtRcdLen, false);

	entries->reset();
	u64 stat_id = entries->getU64(0);
	u64 ext_docid = entries->getU64(0);

	u64 time_slot = entries->getU64(0); 
	int64_t value = entries->getI64(-1); 
	aos_assert_r(value >= 0, false);
	bool is_new = entries->getU8(0);

	bool rslt = getExtStatDoc(
			rdata, 
			stat_id,
			ext_docid, 
			time_slot,
			value,
			is_new,
			doc, 
			buff, 
			proc);
	aos_assert_r(rslt, false);

	while (entries->getCrtIdx() < data_len)
	{
		stat_id = entries->getU64(0);
		u64 crt_docid = entries->getU64(0);

		time_slot = entries->getU64(0); 
		value = entries->getI64(-1); 
		aos_assert_r(value >= 0, false);

		bool crt_is_new = entries->getU8(0);
		if (ext_docid == crt_docid)
		{
			rslt = modifyExtStatDocEntry(
					rdata, 
					buff,
					stat_id, 
					time_slot, 
					value, 
					proc);
			aos_assert_r(rslt, false);
			continue;
		}

		rslt = modifyExtStatDoc(rdata, ext_docid, is_new, doc, buff);
		aos_assert_r(rslt, false);

		ext_docid = crt_docid;
		is_new = crt_is_new;

		rslt = getExtStatDoc(
				rdata, 
				stat_id, 
				crt_docid, 
				time_slot, 
				value, 
				crt_is_new,
				doc, 
				buff, 
				proc);
		aos_assert_r(rslt, false);
	}

	rslt = modifyExtStatDoc(rdata, ext_docid, is_new, doc, buff);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosVector2D::modifyExtStatDoc(
		const AosRundataPtr &rdata,
		const u64 &ext_docid,
		const bool is_new,
		const AosXmlTagPtr &doc,
		const AosBuffPtr &buff)
{	
	if (!is_new)
	{
		aos_assert_r(doc, false);
		aos_assert_r(ext_docid == doc->getAttrU64(AOSTAG_DOCID, 0), false);
		mDistBlockMgr->modifyBinaryDoc(rdata, doc, buff);
	}
	else
	{
		aos_assert_r(!doc, false);
		mDistBlockMgr->createBinaryDoc(rdata, ext_docid, buff);
	}
	return true;
}


bool
AosVector2D::getExtStatDoc(
		const AosRundataPtr &rdata,
		const u64 &stat_id,
		const u64 &ext_docid,
		const u64 &time_slot,
		const int64_t &value,
		const bool is_new,
		AosXmlTagPtr &doc,
		AosBuffPtr &buff,
		const AosStructProcObjPtr &proc)
{
	if (!is_new)
	{
		doc = mDistBlockMgr->retrieveBinaryDoc(
				rdata, ext_docid, buff);
		aos_assert_r(doc, false);
	}
	else
	{
	 	buff = OmnNew AosBuff(mExtensionStatDocSize, 0 AosMemoryCheckerArgs);
		AosDistBlock::setStatDocDftValue(buff->data(), mParm.getDftValue(), mExtensionStatDocSize);
		buff->setDataLen(mExtensionStatDocSize);
		doc = 0;
	}

	int64_t stat_doc_len = buff->dataLen();
	aos_assert_r(stat_doc_len == mExtensionStatDocSize, false);

	bool rslt = modifyExtStatDocEntry(
				rdata, 
				buff,
				stat_id, 
				time_slot, 
				value, 
				proc);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosVector2D::addKickOutStatDoc(
		map<u64, AosBuffPtr> &kickOutStatDoc,
		const u64 &ext_docid,
		const AosBuffPtr &kick_statdoc)
{
	aos_assert_r(ext_docid != AosRootStatDoc::eInvExtStatId, false);
	map<u64, AosBuffPtr>::iterator itr = kickOutStatDoc.find(ext_docid);
	aos_assert_r(itr == kickOutStatDoc.end(), false);
	kickOutStatDoc.insert(make_pair(ext_docid, kick_statdoc));
	return true;
}


bool
AosVector2D::addUpdateEntry(
		const AosBuffPtr &entries,
		const u64 &stat_id,
		const u64 &ext_docid,
		const u64 &time_slot,
		const bool is_new,
		const int64_t &value)
{
	aos_assert_r(ext_docid != AosRootStatDoc::eInvExtStatId, false);
	entries->setU64(stat_id);
	entries->setU64(ext_docid);
	entries->setU64(time_slot);
	entries->setInt64(value);
	entries->setU8(is_new);
	return true;
}


bool 
AosVector2D::modifyExtStatDocEntry(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &buff,
		const u64 &stat_id,
		const u64 &idx, 
		const int64_t &value,
		const AosStructProcObjPtr &proc)
{
	aos_assert_r(buff && buff->dataLen() == mExtensionStatDocSize, false);
	int pos = idx  * mValueSize;
	return proc->setFieldValue(buff->data(), buff->dataLen(), pos, value, mDataType, stat_id);
}


bool 
AosVector2D::modify(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &buff,
		const int64_t &record_len,
		map<OmnString, AosStrValueInfo> &field_info,
		const OmnString	&value_field_name,
		vector<AosValueRslt> &oldvalue_rslt,
		vector<AosValueRslt> &newvalue_rslt)
{
	// This function is similar to 'modify(...)' except that 
	// the data in 'buff' are all local (otherwise, it is an error).
	// 'buff' is in the following format:
	// 	[docid, time_id, v1, v2, v3, ..., key]
	// 	[docid, time_id, v1, v2, v3, ..., key]
	// 	...
	// 	[docid, time_id, v1, v2, v3, ..., key]
	
	OmnScreen << "AosVector2D AosVector2D ext_ctrl_objid:" << mCtrlDocObjid << ";"<< endl;

	aos_assert_rr(buff && record_len > 0, rdata, false);

	map<OmnString, AosStrValueInfo> ::iterator itr;
	itr = field_info.find(AOS_VECTOR2D_DOCID);
	aos_assert_r(itr != field_info.end(), false);
	AosStrValueInfo statid_info = itr->second; 

	itr = field_info.find(AOS_VECTOR2D_TIME);
	aos_assert_r(itr != field_info.end(), false);
	AosStrValueInfo time_info = itr->second;

	itr = field_info.find(AOS_VECTOR2D_STR);
	aos_assert_r(itr != field_info.end(), false);
	AosStrValueInfo str_info = itr->second;

	itr = field_info.find(value_field_name);
	aos_assert_r(itr != field_info.end(), false);
	AosStrValueInfo value_info = itr->second;
	
	// Ketty 2014/07/21
	bool rslt = saveNewStatBinaryId(rdata, buff,
			record_len, statid_info);
	aos_assert_r(rslt, false);

	int64_t start_pos = 0;
	int64_t pos = 0;
	const char *data = buff->data();
	int64_t data_len = buff->dataLen();
	aos_assert_r(data_len >= record_len && data_len % record_len == 0, false);


//modifyLocal(
//			rdata, 
//			buff, 
//			record_len,
//			0,
//			data_len,
//			statid_info,
//			time_info,
//			str_info,
//			value_info,
//			0,
//			mProc);
//mProc->clearValuesRslt();
//return true;


	int num_thrds_running = 0;
	OmnSemPtr sem = OmnNew OmnSem(0);
	vector<OmnThrdShellProcPtr> runners;

	AosDistBlock dist_block(mRootStatDocSize, mParm.getDftValue());
	int docs_per_distblock = dist_block.getDocsPerDistBlock();

	u64 docid = *(u64*)&data[pos + statid_info.field_offset];
	u64 group_id = docid / docs_per_distblock;
	pos += record_len;

	while (pos < data_len)
	{
		docid = *(u64*)&data[pos + statid_info.field_offset];
		u64 gid = docid / docs_per_distblock;
		if (gid == group_id)
		{
			pos += record_len;
			continue;
		}

		// This finishes the group: [start_pos, pos]
		num_thrds_running ++;
		//processOneGroup(start_pos, pos, ...);
OmnScreen << "AosVector2D; groupid:" << group_id << ";" << endl;
		processOneGroup(
				rdata, 
				runners,
				sem,
				buff, 
				start_pos, 
				pos, 
				record_len,
				statid_info, 
				time_info, 
				str_info, 
				value_info,
				num_thrds_running); 

		start_pos = pos;
		group_id = gid;
		pos += record_len;
	}

	//processOneGroup(start_pos, pos, ...);
	num_thrds_running ++;
OmnScreen << "AosVector2D; groupid:" << group_id << ";" << endl;
	processOneGroup(
			rdata, 
			runners,
			sem,
			buff, 
			start_pos, 
			pos, 
			record_len,
			statid_info, 
			time_info, 
			str_info, 
			value_info,
			num_thrds_running); 

	OmnScreen << "AosVector2D num_thrds:" << num_thrds_running << ";"<< endl;
	for (int i = 0; i < num_thrds_running; i++)
	{
		sem->wait();
	}

	for (u32 i = 0; i < runners.size(); i++)
	{
		AosProcThrdRunner* runner = dynamic_cast<AosProcThrdRunner*>(runners[i].getPtr());
		vector<AosValueRslt> old_rslt = runner->getOldValues();
		vector<AosValueRslt> new_rslt = runner->getNewValues();
		oldvalue_rslt.insert(oldvalue_rslt.end(), old_rslt.begin(), old_rslt.end());
		newvalue_rslt.insert(newvalue_rslt.end(), new_rslt.begin(), new_rslt.end());
		runner->clearValuesRslt();
	}
	return true;
}


bool
AosVector2D::processOneGroup(
		const AosRundataPtr &rdata,
		vector<OmnThrdShellProcPtr> &runners,
		const OmnSemPtr &sem,
		const AosBuffPtr &buff,
		const int64_t &start_pos,
		const int64_t &end_pos,
		const int record_len,
		AosStrValueInfo	&statid_info,
		AosStrValueInfo	&time_info,
		AosStrValueInfo	&str_info,
		AosStrValueInfo	&value_info,
		const int num_thrds_running)	
{
	aos_assert_r(start_pos + record_len <= end_pos, false);

	AosJimoPtr jimo = mProc->cloneJimo();
	aos_assert_r(jimo, false);
	AosStructProcObjPtr proc = dynamic_cast<AosStructProcObj*>(jimo.getPtr());

	AosVector2DPtr thisPtr(this, false);
	OmnThrdShellProcPtr runner = OmnNew AosProcThrdRunner(
			thisPtr, proc, sem, num_thrds_running, buff, start_pos, 
			end_pos, record_len, statid_info, time_info, str_info, 
			value_info, rdata);

	runners.push_back(runner);
	OmnThreadPool::runProcAsync(runner);
	return true;
}


bool
AosVector2D::AosProcThrdRunner::procFinished()
{
	mSem->post();
	return true;
}


bool
AosVector2D::AosProcThrdRunner::run()
{
	OmnScreen << "AosVector2D numThrdsRunning:"  << mNumThrdsRunning << ";"<< endl;
	aos_assert_r(mVt2d, false);
	return mVt2d->modifyLocal(
			mRdata, 
			mBuff, 
			mRecordLen,
			mStartPos,
			mEndPos,
			mStatIdInfo,
			mTimeInfo,
			mStrInfo,
			mValueInfo,
			mNumThrdsRunning,
			mJimoProc);
}


vector<AosValueRslt> & 
AosVector2D::AosProcThrdRunner::getNewValues()
{
	return mJimoProc->getNewValues();
}


vector<AosValueRslt> & 
AosVector2D::AosProcThrdRunner::getOldValues()
{
	return mJimoProc->getOldValues();
}


void
AosVector2D::AosProcThrdRunner::clearValuesRslt()
{
	mJimoProc->clearValuesRslt();
}


bool
AosVector2D::getStatDocByTester(
		const AosRundataPtr &rdata,
		vector<u64> &dist_block_docids,
		vector<u64> &stat_ids,
		vector<AosVector2DRecord *> &records,
		const int64_t &s_time,
		const int64_t &e_time)
{
	AosStatTimeUnit::E grpby_time_unit = AosStatTimeUnit::eEpochDay;

	vector<AosStatImRslt> stat_im_rslts;
	vector<AosStatTimeArea> orig_time_conds;
	AosStatTimeArea area;
	area.start_time = s_time;
	area.end_time = e_time;
	area.time_unit = AosStatTimeUnit::eEpochDay;
	orig_time_conds.push_back(area);

	AosStatTimeUnit::init();
	vector<AosStatTimeCond *> time_conds;
	splitTimeSlots(orig_time_conds, time_conds, grpby_time_unit);

	AosDistBlock dist_block(mRootStatDocSize, mParm.getDftValue());
	for (u64 i = 0; i < stat_ids.size(); i++)
	{
		AosVector2DRecord * record = OmnNew AosVector2DRecord();
		record->init(stat_ids[i], grpby_time_unit, mDataType);	
		u64 dist_block_docid = dist_block_docids[i];
		bool rslt =  retrieveRootBucketPrivByTester(
					rdata,
					dist_block_docid,
					dist_block,
					stat_ids[i],
					time_conds,
					stat_im_rslts,
					record);
		aos_assert_r(rslt, false);
		records.push_back(record);
	}

	std::sort(stat_im_rslts.begin(), stat_im_rslts.end());

	bool rslt = retrieveExtBucketPrivByTester(rdata, time_conds, stat_im_rslts);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosVector2D::retrieveRootBucketPrivByTester(
		const AosRundataPtr &rdata,
		const u64 &dist_block_docid,
		AosDistBlock &dist_block,
		const u64 &stat_id,
		vector<AosStatTimeCond *> &time_conds,
		vector<AosStatImRslt> &stat_im_rslts,
		AosVector2DRecord * record)
{
	u32 idx = stat_id / dist_block.getDocsPerDistBlock();
	if (dist_block.getGroupId() != idx || dist_block.getDocid() != dist_block_docid) 
	{
		AosBuffPtr buff;
		AosDocClientObj::getDocClient()->retrieveBinaryDocByStat(
				dist_block_docid, buff, 0, 0, rdata);

		aos_assert_r(buff && buff->dataLen() > 0, false);
		dist_block.set(buff, dist_block_docid, idx, false);
	}

	char * stat_doc = dist_block.getStatDoc(stat_id);
	int64_t stat_doc_len = dist_block.getStatDocSize();
	aos_assert_r(stat_doc && stat_doc_len > 0, false);

	AosRootStatDoc root_stat_doc(stat_doc, stat_doc_len, mParm);
	OmnString keys = root_stat_doc.getStatKey();
	bool rslt = decomposeStatKey(keys, record);
	aos_assert_r(rslt, false);
	
	u64 start_idx = 0;
	u64 ext_docid = 0;
	AosBuffPtr bucket = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
	for (u32 i = 0; i< time_conds.size(); i++)
	{
		start_idx = ext_docid = 0;
		bucket->resetMemory(10);
		rslt = root_stat_doc.getStatDocIndex(
				rdata,
				bucket,
				time_conds[i]->getSlot(), 
				ext_docid, 
				start_idx); 

		if (rslt && ext_docid == 0)
		{
			aos_assert_r(mExtensionStatDocSize <= stat_doc_len 
					&& bucket->dataLen () == mExtensionStatDocSize, false);

			getValuesByBucket(rdata, record, bucket.getPtr(), start_idx, time_conds[i]);
		}
	
		if (rslt && ext_docid > 0 && ext_docid != AosRootStatDoc::eInvExtStatId)
		{
			AosStatImRslt im_rslt(record, stat_id, ext_docid, start_idx, i);
			stat_im_rslts.push_back(im_rslt);
			aos_assert_r(rslt, false);
		}
	}
	return true;
}


bool
AosVector2D::retrieveExtBucketPrivByTester(
		const AosRundataPtr &rdata,
		vector<AosStatTimeCond *> &time_conds,
		vector<AosStatImRslt> &stat_im_rslts)
{
	AosBuffPtr bucket; 
	for (u32 i = 0; i < stat_im_rslts.size(); i++)
	{
		AosDocClientObj::getDocClient()->retrieveBinaryDocByStat(
				stat_im_rslts[i].getExtDistBlockDocid(), bucket, 0, 0, rdata);
		aos_assert_r(bucket && bucket->dataLen () == mExtensionStatDocSize, false);

		int idx = stat_im_rslts[i].getTimeCondIndex();
		getValuesByBucket(
				rdata, 
				stat_im_rslts[i].getRecord(), 
				bucket.getPtr(), 
				stat_im_rslts[i].getExtStartIdx(), 
				time_conds[idx]);
	}
	return true;
}


bool
AosVector2D::saveNewStatBinaryId(
		const AosRundataPtr &rdata,
		const AosBuffPtr &buff,
		const int64_t &record_len,
		AosStrValueInfo &statid_info)
{
	IdVector vt_statid;
	bool rslt = collectNeedSaveStatIds(rdata, buff, record_len,
			statid_info, vt_statid);
	aos_assert_r(rslt, false);
	if(vt_statid.size() == 0)	return true;

	IdVector vt_binary_docid;
	rslt = allocStatBinaryId(rdata, vt_statid, vt_binary_docid);
	aos_assert_r(rslt, false);
	aos_assert_r(vt_statid.size() == vt_binary_docid.size(), false);

	AosBuffPtr new_statid_buff;
	rslt = saveStatBinaryIdToRemote(rdata, vt_statid,
			vt_binary_docid, new_statid_buff);
	aos_assert_r(rslt && new_statid_buff, false);
	
	rslt = mDistBlockMgr->resetStatIdMgr(new_statid_buff);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosVector2D::collectNeedSaveStatIds(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &buff,
		const int64_t &record_len,
		AosStrValueInfo	&statid_info,
		IdVector &vt_new_statid)
{
	const char *data = buff->data();
	int64_t data_len = buff->dataLen();
	aos_assert_r(data_len >= record_len && data_len % record_len == 0, false);
	
	u64 stat_id;
	int64_t crt_pos = 0;
	u64 binary_docid;
	
	u32 grp_idx;
	set<u32> total_grp_idx;
	set<u32>::iterator itr;
	u64 docs_per_distblock = mDistBlockMgr->getDocsPerDistBlock();

	while (crt_pos < data_len)
	{
		stat_id = *(u64*)&data[crt_pos + statid_info.field_offset];
		aos_assert_r(stat_id, false);	
		crt_pos += record_len;
	
		binary_docid = mDistBlockMgr->getEntryByStatId(stat_id);
		if(binary_docid)	continue;
		
		// this stat_docid is new.
		grp_idx = stat_id / docs_per_distblock;
		itr = total_grp_idx.find(grp_idx);
		if(itr != total_grp_idx.end())	continue;

		total_grp_idx.insert(grp_idx);
		vt_new_statid.push_back(stat_id);
	}
	
	return true;
}

bool
AosVector2D::allocStatBinaryId(
		const AosRundataPtr &rdata,
		IdVector &vt_statid,
		IdVector &vt_binary_docid)
{
	vt_binary_docid.clear();	
	u64 docs_per_distblock = mDistBlockMgr->getDocsPerDistBlock();

	u64 stat_id, binary_docid;
	u32 cube_id;
	for(u32 i=0; i<vt_statid.size(); i++)
	{
		stat_id = vt_statid[i];
		cube_id = AosDataStructsUtil::getVirtualIdByStatid(
			stat_id, docs_per_distblock);
		
		binary_docid = AosDataStructsUtil::nextSysDocid(rdata, cube_id);
		aos_assert_r(binary_docid, false);	
		vt_binary_docid.push_back(binary_docid);
	}

	return true;
}

bool
AosVector2D::allocStatBinaryId2(
		const AosRundataPtr &rdata,
		IdVector &vt_statid,
		IdVector &vt_binary_docid)
{
	map<u32, IdVector> statid_grp;
	bool rslt = shufferNewStatids(vt_statid, statid_grp);
	aos_assert_r(rslt, false);

	map<u32, IdVector>::iterator itr = statid_grp.begin();
	for(; itr != statid_grp.end(); itr++)
	{
		//u32 cube_id = itr->first;
		//u32 num = (itr->second).size();
		rslt = false; //AosDataStructsUtil::nextSysDocid(rdata, cube_id,
				//num, vt_binary_docid);
		aos_assert_r(rslt, false);
	}
	aos_assert_r(vt_statid.size() == vt_binary_docid.size(), false);
	return true;	
}


bool
AosVector2D::shufferNewStatids(
		IdVector &vt_statid,
		map<u32, IdVector> &statid_grp)
{
	u32 cube_id;
	u64 stat_id;
	map<u32, IdVector>::iterator itr;
	u64 docs_per_distblock = mDistBlockMgr->getDocsPerDistBlock();
	for(u32 i=0; i<vt_statid.size(); i++)
	{
		stat_id = vt_statid[i];
		cube_id = AosDataStructsUtil::getVirtualIdByStatid(
			stat_id, docs_per_distblock);
		
		itr = statid_grp.find(cube_id);
		if(itr == statid_grp.end())
		{
			IdVector id_v;
			pair<map<u32, IdVector>::iterator, bool> itr_rslt;
			itr_rslt = statid_grp.insert(make_pair(cube_id, id_v));
			aos_assert_r(itr_rslt.second, false);
			itr = itr_rslt.first;
		}
		
		aos_assert_r(itr != statid_grp.end(), false);
		(itr->second).push_back(stat_id);
	}
	return true;
}


bool
AosVector2D::saveStatBinaryIdToRemote(
		const AosRundataPtr &rdata,
		IdVector &vt_statid,
		IdVector &vt_binary_docid,
		AosBuffPtr &new_statid_buff)
{
	AosTransPtr trans = OmnNew AosSaveStatBinaryIdTrans(
			mCtrlDocid, vt_statid, vt_binary_docid);

	bool svr_death;
	AosBuffPtr resp;
	bool rslt = AosSendTrans(rdata, trans, resp, svr_death);
	aos_assert_r(rslt, false);
	if(svr_death)
	{
		OmnAlarm << "cube svr deathed. " << enderr;
		return false;
	}

	aos_assert_r(resp, false);
	rslt = resp->getU8(0);
	aos_assert_r(rslt, false);
	
	u32 buff_len = resp->getU32(0);
	new_statid_buff = resp->getBuff(buff_len, true AosMemoryCheckerArgs);
	aos_assert_r(new_statid_buff, false);
	return true;
}


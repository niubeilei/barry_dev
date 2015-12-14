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
// Example:
// 		value = big_array[1234][567]
//
// One may define the minnimum and maximum value indexes. For instance,
// if the values are epoch days, and the first day ever possible for the
// big array is 12345. The minimum and maximum are used for error checking
// only.
//
// Modification History:
// 2013/12/18 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Vector2D/Vector2D.h"

#include "API/AosApi.h"
#include "Util/DataTypes.h"
#include "Vector2D/Vector2DConn.h"
#include "Vector2DQryRslt/Vt2dQryRsltProc.h"

#include "StatServer/StatCube.h"


//yang

#include "StatDocDist.h"

#include "StatUtil/StatTimeArea.h"


AosVector2D::AosVector2D(const u64 stat_id, const u32 cube_id)
:
mStatId(stat_id),
mCubeId(cube_id),
mHasTimeField(true)
{
	initCounters();
	mTimeAreas.clear();

	//yang
	mStatCubeKey<< stat_id <<"_"<<cube_id;

}

void
AosVector2D::initCounters()
{
	mReadTime1 = 0;
	mReadTime2 = 0;

	mReadNum1 = 0;
	mReadNum2 = 0;
}

AosVector2D::~AosVector2D()
{
}

void
AosVector2D::outputCounters()
{
	OmnScreen << "(Statistics counters : Vector2D ) resolveQryTime--- Time1 : "
		    << mReadTime1 << ", Num: " << mReadNum1 << endl;

	 OmnScreen << "(Statistics counters : Vector2D )  push_back --- Time2 : "
		          << mReadTime2 << ", Num: " << mReadNum2 << endl;

	 if (mVt2dConn)
		 mVt2dConn->outputCounters();
}



bool
AosVector2D::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &conf)
{
	bool rslt;

	AosXmlTagPtr vt2d_info_conf = conf->getFirstChild("vt2d_info", true);
	aos_assert_r(vt2d_info_conf, false);

	rslt = mVt2dInfo.config(rdata, vt2d_info_conf);

	//yang
	mStatFunc=mVt2dInfo.mMeasures[0].mAggrFuncStr;

	aos_assert_r(rslt, false);
	mHasTimeField = (mVt2dInfo.mTimeFieldName != "");

	AosXmlTagPtr conn_conf = conf->getFirstChild("vt2d_conn", true);
	aos_assert_r(conn_conf, false);

	//mVt2dInfo.initVt2dValueDef(mValueDef);
	if(mStatFunc=="dist_count")
	{
		mDistVecFile = OmnNew AosStatDocDist(
				rdata.getPtr(),mStatId, mStatCubeKey,mCubeId, 0, 0, 0,
				0,0);

		mMeasureType = DIST_COUNT;

	}
	else
	{
		mVt2dConn = OmnNew AosVector2DConn(mCubeId,
				getMeasureValueSize(), AosRlbFileType::eNormal);
		rslt = mVt2dConn->config(rdata, conn_conf);
		aos_assert_r(rslt, false);

		mMeasureType = NORMAL;
	}


	return true;
}

u64
AosVector2D::getEncodedRecordFieldOffset(
		const char* data,
		OmnString dfname)
{
	// encoded record format
	// total_len + isnewflaglen + isnewflag + docidlen + docid + keylen + statkey + timelen + time + m1len + m1
	// + m2len + m2 + ...... + mnlen + mn
	// offset:
	// 0         + 8            + 8         + 1        + 8     + 8      + 8  +    + statkey   + 8    + 8    + 8
	// + 8     + 8  + ...... + 8     + 8
	//
	// 0           8              16          17         25

	// original record format
	// isnewflag + docid + statkey + time    + m1   + m2, .....mn
	// offset:
	// 0         + 1     + 8       + keylen  + 8    + 8,.....8
	// 0           1       9        9+keylen   17+keylen
	if(dfname == "sdocid")
	{
		return 8 + 8 + 1 + 8;
	}
	else if(dfname=="isnew")
	{
		return 8 + 8;
	}
	else if(dfname=="statkey")
	{
		return 8 + 8 + 1 + 8 + 8 + 8;
	}
	else if(dfname == "time")
	{
		u64 keylenoffset = 8 + 8 + 1 + 8 + 8;
		u64 keylen = *(u64*)(data + keylenoffset);
		return keylenoffset + 8 + keylen + 8;
	}
	else
	{
		OmnNotImplementedYet
	}
}

u64
AosVector2D::getRecordFieldLen(
		const char* data,
		OmnString dfname)
{
	if(dfname == "sdocid")
	{

	}
	else if(dfname=="isnew")
	{

	}
	else if(dfname=="statkey")
	{
		u64 keylenoffset = 8 + 8 + 1 + 8 + 8;
		u64 keylen = *(u64*)(data + keylenoffset);
		return keylen;
	}
	else if(dfname == "time")
	{

	}
	else
	{

	}
}

AosBuffPtr
AosVector2D::decodeRecord(
		const char* data,
		int& orig_len)
{
	u64 encodedRecordlen = *(u64*)data;
	AosBuffPtr origbuf = OmnNew AosBuff(encodedRecordlen AosMemoryCheckerArgs);
	//mRcdOrigBuffs.push_back(origbuf);
	u64 pos = 8;
	while(pos < encodedRecordlen)
	{
		u64 fieldlen = *(u64*)(data+pos);
		pos += 8;//skip field len
		u64 fieldoffset = pos;
		origbuf->setBuff(data+fieldoffset,fieldlen);
		pos += fieldlen;//move to next field
		orig_len += fieldlen;
	}
	return origbuf;
	//char* tmp = new char[origbuf->dataLen()];
	//memcpy(tmp,origbuf->data(),origbuf->dataLen());
	//return tmp;
}

/*
u64
AosVector2D::getOrigRecordFieldOffset(
		const char* data,
		OmnString dfname)
{
	if(dfname == "sdocid")
	{

	}
	else if(dfname=="isnew")
	{

	}
	else if(dfname=="statkey")
	{
		u64 keylenoffset = 8 + 8 + 1 + 8 + 8;
		u64 keylen = *(u64*)(data + keylenoffset);
		return keylen;
	}
	else if(dfname == "time")
	{

	}
	else
	{

	}
}
*/


bool
AosVector2D::modify(
		AosRundata *rdata,
		AosVt2dModifyInfo &mdf_info,
		const AosBuffPtr &buff)
{
	// This function is similar to 'modify(...)' except that
	// the data in 'buff' are all local (otherwise, it is an error).
	// 'buff' is in the following format:
	// 	[docid, time_id, v1, v2, v3, ..., key]
	// 	[docid, time_id, v1, v2, v3, ..., key]
	// 	...
	// 	[docid, time_id, v1, v2, v3, ..., key]

	/*OmnScreen << "Ketty Stat Print. modify Vector2D."
		<< "cube_id:" << mCubeId << "; "
		<< "vt2d_name:" << mVt2dInfo.mVt2dName << "; "
		<< endl;
    */

	typedef map<OmnString,map<u32,map<u8,vector<OmnString> > > > ktvlist_t;


	if(mdf_info.mMeasureType == AosVt2dModifyInfo::DIST_COUNT)
	{
		//special measures

		int64_t crt_pos = 0, end_pos = buff->dataLen();
		char *data = buff->data();
		//aos_assert_r((buff->dataLen()) % mdf_info.mRecordLen== 0, false);

		ktvlist_t::const_iterator iter;

		ktvlist_t ktvlist=mCube->getKTVList();
		bool rslt;
		for(iter=ktvlist.begin();iter!=ktvlist.end();iter++)
		{
			u32 recordLen = *(u64*)&data[crt_pos];

			u32 docidpos = 9;
			u32 keypos = 17;
			u64 docid;
			docid = *(u64*)&data[crt_pos + docidpos];
			aos_assert_r(docid, false);

			map<u32,map<u8,vector<OmnString> > > tmap=iter->second;
			map<u32,map<u8,vector<OmnString> > >::const_iterator tmapiter=tmap.begin();
			for(;tmapiter!=tmap.end();tmapiter++)
			{
				u32 time=tmapiter->first;
				//aos_assert_r(time, false);

				map<u8,vector<OmnString> > measures=tmapiter->second;
				map<u8,vector<OmnString> >::const_iterator mmapiter = measures.begin();
				
				mDistVecFile->setMeasureCount(measures.size());

				for(;mmapiter!=measures.end();mmapiter++)
				{
					int measure_idx = mmapiter->first;

					aos_assert_r(measures.size() >= measure_idx , false);
					vector<OmnString> vals = mmapiter->second;
					for(int i=0;i<vals.size();i++)
					{
						OmnString val=vals[i];
						u64 newval=val.toU64();
//						OmnScreen << "Vector2D for distcount : " << "docid : " <<docid <<" time: " << time << " measure : " << newval << endl;
						if(mdf_info.mMeasureMapper.mOpr == "insert")
							rslt = mDistVecFile->appendDistValue(rdata,
								docid, time, measure_idx,newval);
						else
							rslt = mDistVecFile->deleteDistValue(rdata,
								docid, time, measure_idx,newval);
					}
				}
			}
			crt_pos += recordLen;
		}
		mDistVecFile->updateFinished(rdata);
		mCube->clearKTVList();
	}
	else
	{
		//normal measure, sum,count
		if(mdf_info.mRecordType == AosDataRecordType::eBuff)
		{
			int64_t crt_pos = 0, end_pos = buff->dataLen();
			char *data = buff->data();

			u64 t1 = OmnGetTimestamp();
			bool rslt;
			u64 s_docid, time;
			while (crt_pos < end_pos)
			{
				u64 recordLen = *(u64*)&data[crt_pos];//record len
				aos_assert_r(buff && recordLen> 0, false);
				
				u64 docidpos = getEncodedRecordFieldOffset(&data[crt_pos],"sdocid");
				s_docid= *(u64*)&data[crt_pos + docidpos];
				aos_assert_r(s_docid, false);

				if(mHasTimeField)
				{
					u64 timepos = getEncodedRecordFieldOffset(&data[crt_pos],"time");
					time = *(u64*)&data[crt_pos + timepos];
				}
				else
				{
					time = 0;
				}
				u64 keylen = getRecordFieldLen(&data[crt_pos],"statkey");

				int orig_len = 0;
				AosBuffPtr origbuf = decodeRecord(data+crt_pos,orig_len);
				char* orig_data = origbuf->data();
				aos_assert_r(orig_data && orig_len> 0, false);

				u64 measureOffset = 1 + 8 + keylen + 8; // isnewflag + docid + statkey + time

				rslt = mVt2dConn->updateRecord(rdata,
						s_docid, time, orig_data + measureOffset,
						orig_len, mdf_info.mMeasureMapper);
				aos_assert_r(rslt, false);

				crt_pos += recordLen;
			}
			mVt2dConn->updateFinished(rdata);

			OmnScreen << "Ketty Stat Print. Save to Vector2D."
				<< "cube_id:" << mCubeId << "; "
				<< "save time:" << OmnGetTimestamp() - t1
				<< endl;
		}
		else
		{
			aos_assert_r(buff && mdf_info.mRecordLen> 0, false);

			int64_t crt_pos = 0, end_pos = buff->dataLen();
			char *data = buff->data();
			aos_assert_r((buff->dataLen()) % mdf_info.mRecordLen== 0, false);

			u64 t1 = OmnGetTimestamp();
			bool rslt;
			u64 s_docid, time;
			while (crt_pos < end_pos)
			{
				s_docid= *(u64*)&data[crt_pos + mdf_info.mSdocidPos];
				aos_assert_r(s_docid, false);

				if(mHasTimeField)
				{
					time = *(u64*)&data[crt_pos + mdf_info.mTimeFieldPos];
				}
				else
				{
					time = 0;
				}
				//grpby_time = AosStatTimeUnit::parseTimeValue(time,
				//	mVt2dInfo.mOrigTimeUnit, mVt2dInfo.mGrpbyTimeUnit);
				//aos_assert_r(grpby_time >=0, false);

				rslt = mVt2dConn->updateRecord(rdata,
						s_docid, time, data + crt_pos,
						mdf_info.mRecordLen, mdf_info.mMeasureMapper);
				aos_assert_r(rslt, false);

				crt_pos += mdf_info.mRecordLen;
			}
			mVt2dConn->updateFinished(rdata);

			OmnScreen << "Ketty Stat Print. Save to Vector2D."
				<< "cube_id:" << mCubeId << "; "
				<< "save time:" << OmnGetTimestamp() - t1
				<< endl;
		}
	}

	return true;
}


bool
AosVector2D::retrieve(
		const AosRundataPtr &rdata,
		vector<u64> &stat_docids,
		vector<AosStatTimeArea> &qry_time_areas,
		const AosVt2dQryRsltPtr &qry_rslt,
		const AosVt2dQryRsltProcPtr &qry_rslt_proc)
{
	vector<AosStatTimeArea> new_time_areas;
	bool rslt = resolveQryTime(qry_time_areas, new_time_areas);
	aos_assert_r(rslt, false);

	//if(new_time_areas.size() == 0 && qry_rslt_proc->getType() != "GroupByAll")
	if(new_time_areas.size() == 0)
	{
		// need retrieve each value by each time.
		AosStatTimeArea crt_time_area(-1, -1, mVt2dInfo.mGrpbyTimeUnit);
		new_time_areas.push_back(crt_time_area);
	}

	// move to mVt2dConn.
	//rslt = qry_rslt_proc->appendVt2dRecords(rdata.getPtr(),
	//		qry_rslt.getPtr(), stat_docids, new_time_areas);
	//aos_assert_r(rslt, false);

	if(new_time_areas.size() == 0)
	{
		OmnNotImplementedYet;
		return false;
		//return retrieveWithGeneral(rdata, stat_docids, qry_rslt, qry_rslt_proc);
	}

	return mVt2dConn->readRecords(rdata.getPtr(), stat_docids,
			new_time_areas, qry_rslt.getPtr(), qry_rslt_proc.getPtr());
}

bool
AosVector2D::retrieve(
		const AosRundataPtr &rdata,
		AosVt2dRecord *rcd,
		u64 sdocid,
		vector<AosStatTimeArea> &qry_time_areas)
{
	u64 tStart, tEnd;
	vector<AosStatTimeArea> new_time_areas;
	tStart = OmnGetTimestamp();
	bool rslt;
	tEnd = OmnGetTimestamp();
	mReadTime1 += tEnd - tStart;
	mReadNum1++;



	if (new_time_areas.size() == 0)
	{
		//init the time area
		rslt = resolveQryTime(qry_time_areas, new_time_areas);
		aos_assert_r(rslt, false);
		if(new_time_areas.size() == 0)
		{
			// need retrieve each value by each time.
			tStart = OmnGetTimestamp();
			AosStatTimeArea crt_time_area(-1, -1, mVt2dInfo.mGrpbyTimeUnit);
			new_time_areas.push_back(crt_time_area);
			tEnd = OmnGetTimestamp();
			mReadTime2 += tEnd - tStart;
			mReadNum2++;
		}
	}


	if(mStatFunc == "dist_count")
	{
		// Ketty 2014/08/30
		//vector<TimeBlockArea> time_block_areas;
		//vector<AosStatTimeArea> new_time_areas;
		//bool rslt = mVt2dConn->splitTimeAreasByTimeBlock(qry_time_areas, time_block_areas, new_time_areas);
		//for(u32 i=0; i<time_block_areas.size(); i++)
		//{
			// Ketty 2014/10/20
			//rslt = vector_file->readIntoVt2dRecord(rdata, rcd, sdocid, qry_time_areas);
			rslt = mDistVecFile->readIntoVt2dRecord(rdata.getPtr(), rcd, sdocid,
					qry_time_areas);
		//}

		OmnTagFuncInfo << endl;
		return true;
	}
	else
		return mVt2dConn->readRecord(rdata.getPtr(), rcd, sdocid, new_time_areas);
}

bool
AosVector2D::resolveQryTime(
		vector<AosStatTimeArea> &orig_time_areas,
		vector<AosStatTimeArea> &new_time_areas)
{
	int64_t start_time = -1, end_time = -1;
	for(u32 i=0; i<orig_time_areas.size(); i++)
	{
		//if(orig_time_areas[i].start_time != -1)
		//{
		//	start_time = AosStatTimeUnit::parseTimeValue(orig_time_areas[i].start_time,
		//		orig_time_areas[i].time_unit, mVt2dInfo.mGrpbyTimeUnit);
		//	aos_assert_r(start_time >=0, false);
		//}

		//if(orig_time_areas[i].end_time != -1)
		//{
		//	end_time = AosStatTimeUnit::parseTimeValue(orig_time_areas[i].end_time,
		//		orig_time_areas[i].time_unit, mVt2dInfo.mGrpbyTimeUnit);
		//	aos_assert_r(end_time >=0, false);
		//}

		start_time = orig_time_areas[i].start_time;
		end_time = orig_time_areas[i].end_time;
		AosStatTimeArea new_area(start_time, end_time, mVt2dInfo.mGrpbyTimeUnit);
		new_time_areas.push_back(new_area);
	}

	return true;
}


//yang,2015/09/08
u64
AosVector2D::getDistCountNum()
{
	return mVt2dInfo.getDistCountMeasureNum();
}
/*
u64
AosVector2D::getMinMaxNum()
{
	return mVt2dInfo.getMinMaxMeasureNum();
}*/

////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2014/01/22 Created by Ketty
////////////////////////////////////////////////////////////////////////////

#include "StatServer/StatCube.h"

#include "Thread/Sem.h"
#include "SEInterfaces/RlbFileType.h"
#include "StatServer/StatKeyConn.h"
#include "StatServer/Vt2dModifyReq.h"
#include "StatServer/Vt2dReadReq.h"
//#include "StatServer/Statistic.h"
#include "Thread/ThreadPool.h"
#include "Vector2DQryRslt/Vt2dQryRslt.h"
#include "Vector2DQryRslt/Vt2dQryRsltProc.h"
#include "Vector2D/Vector2D.h"
#include "Debug/Debug.h"

static int globalPrintCounts = 0;
static int maxPrint = 100;

AosStatCube::AosStatCube()
:
//mNextNewStatId(0),
mStatId(0),
mEngine(0)
//mCubeId(cube_id)
{
	mktvlist.clear();
	initCounters();
}


AosStatCube::~AosStatCube()
{
	if (mEngine)
	{
		delete mEngine;
		mEngine = 0;
	}
}


void
AosStatCube::initCounters()
{
	mModifyTime = 0;
	mModifyNum = 0;
}


void
AosStatCube::outputCounters()
{
	OmnScreen << "(Statistics Counter : stat cube ) Save stat key --- Time:"
		<< mModifyTime << "Num :" << mModifyNum << endl;
}


bool
AosStatCube::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &cube_conf)
{
	aos_assert_r(cube_conf, false);
	bool rslt;
	
	mStatId = cube_conf->getAttrU64("stat_id",0); 
	aos_assert_r(mStatId, false);	
	mCubeId = cube_conf->getAttrU32("cube_id", 0);

	//rslt = initNextNewStatId();
	//aos_assert_r(rslt, false);

	rslt = configKeyConn(rdata, cube_conf);
	aos_assert_r(rslt, false);
	
	rslt = configVector2Ds(rdata, cube_conf);
	aos_assert_r(rslt, false);
	
	//yang
	if(mVector2Ds[0]->isDistCount())
		mKeyConn->setStatFunc("dist_count");

	mVector2Ds[0]->setStatCube(this);

	return true;
}


//bool
//AosStatCube::initNextNewStatId()
//{
//	OmnNotImplementedYet;
//	return false;
//}


bool
AosStatCube::configKeyConn(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &cube_conf)
{
	//<stat_key_conn meta_fileid="xxx"/>
	aos_assert_r(cube_conf, false);
	
	AosXmlTagPtr conn_conf = cube_conf->getFirstChild("stat_key_conn", false); 
	aos_assert_r(conn_conf, false);
	
	mKeyConn = OmnNew AosStatKeyConn(mCubeId, AosRlbFileType::eNormal);
	bool rslt = mKeyConn->config(rdata, conn_conf);
	aos_assert_r(rslt, false);
	
	return true;
}


bool
AosStatCube::configVector2Ds(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &cube_conf)
{
	//<cube cube_id="0">
	//	<stat_key_conn zky_key_conn_config_objid="xxx"/>
	//	<vector2ds>
	//		<vt2d zky_name="vt2d_1" zky_vt2d_conn_config_objid="xxx"/>
	//		...
	//	</vector2ds>
	//</cube>
	bool rslt;
	
	AosXmlTagPtr vt2d_confs = cube_conf->getFirstChild("vector2ds", false); 
	aos_assert_r(vt2d_confs, false);	
	
	AosVector2DPtr vt2d;
	AosXmlTagPtr each_vt2d_cube_conf = vt2d_confs->getFirstChild(true);
	while(each_vt2d_cube_conf)
	{
		vt2d = OmnNew AosVector2D(mStatId, mCubeId);
		rslt = vt2d->config(rdata, each_vt2d_cube_conf);
		aos_assert_r(rslt, false);

		mVector2Ds.push_back(vt2d);
		each_vt2d_cube_conf = vt2d_confs->getNextChild();
	}
	
	OmnTagFuncInfo << endl;
	return true;
}


bool
AosStatCube::modify(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &buff,
		AosStatModifyInfo &stat_mdf_info)
{
	// This function is similar to 'modify(...)' except that 
	// the data in 'buff' are all local (otherwise, it is an error).
	// 'buff' is in the following format:
	// 	[docid, time_id, v1, v2, v3, ..., key]
	// 	[docid, time_id, v1, v2, v3, ..., key]
	// 	...
	// 	[docid, time_id, v1, v2, v3, ..., key]

	//aos_assert_rr(buff && stat_mdf_info.mRecordLen> 0, rdata, false);

	bool rslt = saveNewStatIdKey(rdata.getPtr(), buff, stat_mdf_info);
	aos_assert_r(rslt, false);
	
	vector<AosVt2dModifyInfo> & vt2d_mdf_infos = stat_mdf_info.getVt2dMdfInfos();
	rslt = batchModify(rdata, buff, vt2d_mdf_infos);
	aos_assert_r(rslt, false);
	
	OmnTagFuncInfo << endl;
	return true;
}

void AosStatCube::appendTimeToKey(OmnString key,u64 timeid)
{

}

/*
void AosStatCube::appendValueToKey(OmnString key,OmnString val)
{
	//yang 2014/12/31
	kvlist_t::iterator iter=mkvlist.find(key);
	if(iter==mkvlist.end())
	{
		//not found
		mkvlist[key]=*(new vector<OmnString>);
	}
	mkvlist[key].push_back(val);
}
*/

void AosStatCube::appendValueToKeyTime(OmnString key,u64 timeid,OmnString val)
{

}

void AosStatCube::splitKeyValue(const vector<StatDocidKeyPair>& in_vt_idkey_pair,
		vector<StatDocidKeyPair>& out_vt_idkey_pair)
{
	for(u32 i=0; i<in_vt_idkey_pair.size(); i++)
	{
		OmnString kv=in_vt_idkey_pair[i].second;
		int splitpos=kv.find('\001',false);
		OmnString val=kv.subString(0,splitpos);
		OmnString key=kv.subString(splitpos+1,kv.length()-splitpos-1);
		out_vt_idkey_pair.push_back(make_pair(i+1, key));
		//appendValueToKey(key,val);
	}
}

bool
AosStatCube::saveNewStatIdKey(
		AosRundata *rdata,
		const AosBuffPtr &buff,
		AosStatModifyInfo &mdf_info)
{
	u64 tStart, tEnd;
	vector<StatDocidKeyPair> vt_idkey_pair;
	
	//counter
	tStart = OmnGetTimestamp();
	bool rslt = collectNewStatIdKey(rdata, buff, mdf_info, vt_idkey_pair);
	tEnd = OmnGetTimestamp();
	mModifyTime = tEnd - tStart;
	mModifyNum ++;
	outputCounters();

	aos_assert_r(rslt, false);
	if(vt_idkey_pair.size() == 0)	return true;
	

	//rslt = saveNextNewStatId(rdata);
	//aos_assert_r(rslt, false);

	//u64 t1 = OmnGetTimestamp();
	for(u32 i=0; i<vt_idkey_pair.size(); i++)//maybe 5 million lines
	{
		if(globalPrintCounts < maxPrint)
		{
			OmnScreen << "Ketty Stat Print. Save Stat Each Key."
				<< "stat_docid:" << vt_idkey_pair[i].first << "; "
				<< "stat_key:" << vt_idkey_pair[i].second << "; "
				<< endl;
			globalPrintCounts++;
		}
		rslt = mKeyConn->saveKey(rdata, vt_idkey_pair[i].first,
				vt_idkey_pair[i].second);
		aos_assert_r(rslt, false);
	}
	return true;
}


bool
AosStatCube::collectNewStatIdKey(
		const AosRundataPtr &rdata,
		const AosBuffPtr &buff,
		AosStatModifyInfo &mdf_info,
		vector<StatDocidKeyPair> &vt_idkey_pair)
{
//	buff->dumpData(true,"StatCube for distcount : ");
	const char *data = buff->data();
	int64_t data_len = buff->dataLen();
	/*
	   aos_assert_r(data_len >=0 && (u64)data_len >= mdf_info.mRecordLen &&
	   data_len % mdf_info.mRecordLen == 0, false);
	   */

	u64 stat_id = 0;
	bool is_new_flag;
	int64_t crt_pos = 0;

	u32 keyLen;
	OmnString key;
	while(crt_pos < data_len)
	{
		if(mdf_info.mMeasureType == AosStatModifyInfo::DIST_COUNT)
		{
			u8 measure_count;
			if(mdf_info.mMeasureType == AosStatModifyInfo::DIST_COUNT)
			{
				int distVt2didx = getDistVt2dIdx();
				aos_assert_r(distVt2didx >= 0, false);
				measure_count = mVector2Ds[distVt2didx]->getMeasureInfos()->size();
			}
			////////////////////////////////////////////////////////////////////////////////////////
			// the buff format:
			//  totalLen | isNew | Docid | statKey | timeMeasureLen | timeUnit | measureLen | measure 
			//   ... / totalLen | isNew | ....
			//  every len contain self and is 8 byte
			//  isNew is 1 byte
			//  Docid is 8 byte
			//  statKey is variable-length and has '\000' in the end;
			//  timeUnit is 8 byte
			//  measure	 is 8 byte
			//////////////////////////////////////////////////////////////////////////////////////
			u32 recordLen = *(u64*)&data[crt_pos];
			u64 docid;
			docid = *(u64*)&data[crt_pos + DOCID_POS];
			aos_assert_r(docid, false);

			is_new_flag = (data[crt_pos + ISNEW_POS] == 1);
			if(is_new_flag)
			{
				keyLen = strlen(&data[crt_pos + STATKEY_POS]);
				key.assign(&data[crt_pos + STATKEY_POS], keyLen);
				key.normalizeWhiteSpace(true, true);  // trim
				//collect statkey
				vt_idkey_pair.push_back(make_pair(docid, key));
			}
			else
			{
				keyLen = strlen(&data[crt_pos + STATKEY_POS]);
				key.assign(&data[crt_pos + STATKEY_POS], keyLen);
				key.normalizeWhiteSpace(true, true);  // trim
			}

			ktvlist_t::const_iterator iter=mktvlist.find(key);
			if(iter==mktvlist.end())
				//key not found,allocate a new one
				mktvlist[key]=*(new map<u32,map<u8,vector<OmnString> > >);
			//ttlen = recordLen - rcdlen field - newflag - docid - key - '\0'
			u32 timeValueLen = recordLen - STATKEY_POS - keyLen - 1; // total time len,subtract one '\0' and newflag
			u32 crtTimePos = crt_pos + STATKEY_POS + keyLen + 1; // add one '\0'
			u32 timePos = 0;
			while(timePos < timeValueLen)
			{
				u64 timeMeasureLen = *(u64*)&data[crtTimePos + timePos];
				u64 time = *(u64*)&data[crtTimePos + timePos + 8];
				map<u32,map<u8,vector<OmnString> > > tmap=mktvlist[key];
				map<u32,map<u8,vector<OmnString> > >::const_iterator tmapiter=tmap.find(time);
				if(tmapiter==tmap.end())
					//time not found,allocate a new one
					mktvlist[key][time]=*(new map<u8,vector<OmnString> >);
				u64 crtMeasurePos = crtTimePos + timePos + 8 + 8;
				//tvlen = tlen - len - time
				//u64 tvlen = tlen - 8 - 8; //total value len
				u64 vpos = 0;
				u64 measuresLen = timeMeasureLen - 8 -8;
				while(vpos < measuresLen)
				{
					u64 measureLen = *(u64*)&data[crtMeasurePos + vpos];
					vpos += sizeof(u64);    //skip measure_len
					u8 i=0; //measure index
					while(i<measure_count)
					{
						OmnString val;
						val <<  *(u64*)&data[crtMeasurePos + vpos];
						mktvlist[key][time][i].push_back(val);
						vpos += sizeof(u64); 
						i++;
					}
				}
				timePos += timeMeasureLen;
			}
			crt_pos += recordLen;
		}
		else
		{
			if(mdf_info.mRecordType == AosDataRecordType::eBuff)
			{
				u64 recordLen = *(u64*)&data[crt_pos];//record len
				u32 docidpos = 25;
				u32 keylenpos = 8 + 8 + 1 + 8 + 8;
				u32 keypos = 8 + 8 + 1 + 8 + 8 + 8;
				u64 isNewFlagPos = sizeof(u64) * 2;
				u64 docid;
				docid = *(u64*)&data[crt_pos + docidpos];
				aos_assert_r(docid, false);

				is_new_flag = (data[crt_pos + isNewFlagPos] == 1);
				if(is_new_flag)
				{
					keyLen = *(u64*)(data + crt_pos + keylenpos);//strlen(&data[crt_pos + keypos]);
					key.assign(&data[crt_pos + keypos], keyLen);
					key.normalizeWhiteSpace(true, true);  // trim
					vt_idkey_pair.push_back(make_pair(docid, key));
				}
				//orig_stat_id = stat_id;
				crt_pos += recordLen;
			}
			else
			{
				stat_id = *(u64*)&data[crt_pos + mdf_info.mSdocidPos];
				aos_assert_r(stat_id, false);

				//Comment out the assert method:
				//  1. In cube side, the sdocid is local sdocid without
				//     cube Id prefix
				//
				//  2. The sdoc is directed to the right cube by the parameter
				//     in BatchGetStatDocTrans. Therefore sdocid doesn't need 
				//     to have cube info
				//
				//aos_assert_r(stat_id >> 32 == mCubeId, false);
				stat_id = (u32)stat_id;		// trim the cube_id.
				*(u64*)&data[crt_pos + mdf_info.mSdocidPos] = stat_id;	// trim the cube_id.

				is_new_flag = (data[crt_pos + mdf_info.mIsNewFlagPos] == 1);

				if(is_new_flag)
				{
					// this is new stat doc.
					keyLen = strlen(&data[crt_pos + mdf_info.mKeyPos]);
					key.assign(&data[crt_pos + mdf_info.mKeyPos], keyLen);
					key.normalizeWhiteSpace(true, true);  // trim

					vt_idkey_pair.push_back(make_pair(stat_id, key));
				}

				//orig_stat_id = stat_id;
				crt_pos += mdf_info.mRecordLen;
			}
		}
	}

	OmnTagFuncInfo << endl;
	return true;	
}


//bool
//AosStatCube::saveNextNewStatId(const AosRundataPtr &rdata)
//{
//	OmnNotImplementedYet;
//	return false;
//}


bool
AosStatCube::batchModify(
		const AosRundataPtr &rdata,
		const AosBuffPtr &buff,
		vector<AosVt2dModifyInfo> &vt2d_mdf_infos)
{
	OmnSemPtr sem = OmnNew OmnSem(0);
	//AosStatCubePtr thisPtr(this, false);
	OmnThrdShellProcPtr thrd_runner;
	
	for(u32 i=0; i< vt2d_mdf_infos.size(); i++)
	{
		int vt2d_idx = vt2d_mdf_infos[i].mVt2dIdx;
		aos_assert_r(vt2d_idx >=0 && (u32)vt2d_idx < mVector2Ds.size(), false);
		
		thrd_runner = OmnNew AosVt2dModifyReq(
				rdata, sem, mVector2Ds[vt2d_idx],
				vt2d_mdf_infos[i], buff);
		
		OmnThreadPool::runProcAsync(thrd_runner);
	}

	for (u32 i = 0; i<vt2d_mdf_infos.size(); i++)
	{
		sem->wait();
	}

	OmnTagFuncInfo << "v2d to modify size is: " << vt2d_mdf_infos.size() << endl;
	return true;
}

AosBuffPtr
AosStatCube::retrieve(
		const AosRundataPtr &rdata,
		vector<u64>	&stat_docids,
		AosStatQueryInfo &qry_info)
		//vector<AosStatTimeArea> &qry_time_areas)
{
	u64 tStart,tEnd;
	static u64 mSerializeToCostTime;
	aos_assert_r(stat_docids.size(), 0);

	bool rslt;
	OmnTagFuncInfo << "stat_docids's entries is: " << stat_docids.size() << endl;

	// Ketty 2014/10/19
	/*
	static AosStatQueryInfo qryInfo;
	qryInfo.mQryTimeArea = qry_time_areas;
	if (!mEngine)
	{
		mEngine = new AosStatQryEngine(rdata.getPtr(), &qryInfo);
	}
	*/
	//assume a query will can engine only once for now
	//this means all the needed sdocids will be transfered
	//at the same time
	mEngine = OmnNew AosStatQryEngine(rdata.getPtr(), &qry_info);

	u64 t1 = OmnGetTimestamp();
	OmnScreen << "Ketty Stat Print. Query start, "
		<< "statid_num:" << stat_docids.size() << endl; 

	aos_assert_r(mEngine, 0);
	mEngine->runStat(stat_docids, mKeyConn.getPtr(), &mVector2Ds);
	
	for(size_t i = 0 ; i < mVector2Ds.size(); i++)
	{
		mVector2Ds[i]->clearTimeAreas();
	}

	OmnScreen << "Ketty Stat Print. Query end, "
		<< "time::" << OmnGetTimestamp() - t1 << endl; 

	AosBuffPtr resp = OmnNew AosBuff(100 AosMemoryCheckerArgs);

	u64 tmp_sdocid = mCubeId;
	tmp_sdocid = ((tmp_sdocid << 32) + stat_docids[0]);
	//resp->setU64(stat_docids[0]);	// temp. DataCube stat_doc_opr will use this.
	resp->setU64(tmp_sdocid);	// temp. DataCube stat_doc_opr will use this.
	//having cond
	mEngine->applyHavingCond();
	
	tStart = OmnGetTimestamp();
	rslt = mEngine->serializeTo(rdata, resp.getPtr());
	tEnd = OmnGetTimestamp();
	mSerializeToCostTime = tEnd - tStart;
	OmnScreen << "(Statistics counters : Engine) serializeTo cost --- Time : " 
		<< mSerializeToCostTime << endl;

	mEngine->outputCounters();

	aos_assert_r(rslt, 0);

	return resp;
}

void
AosStatCube::clearKTVList()
{
	mktvlist.clear();
	//for(iter;iter!=mktvlist.end())
	//delete [] element;
}

int
AosStatCube::getDistVt2dIdx()
{
	int vt2dcnt = mVector2Ds.size();
	for(int i=0;i<vt2dcnt;i++)
	{
		AosVector2DPtr vt2d = mVector2Ds[i];
		if(vt2d->isDistCount())
			return i;
	}
	return -1;
}

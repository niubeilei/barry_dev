////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//	
// Modification History:
// 2015/03/27 Created by Crystal Cao 
////////////////////////////////////////////////////////////////////////////
#include "DataCube/Jimos/DataCubeHadoopQuery.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "API/AosApiG.h"
#include "HadoopUtil/HadoopApi.h" 
#include "Rundata/Rundata.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/CodeConvertion.h"
#include "Util/DiskStat.h"
#include "SEInterfaces/DocClientObj.h"  
#include <signal.h>

#if 0
extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosDataCubeHadoopQuery_0(
		const AosRundataPtr &rdata,
		const u32 version) 
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosDataCubeHadoopQuery(version);
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


AosDataCubeHadoopQuery::AosDataCubeHadoopQuery(const u32 version)
:
AosDataCube(AOS_DATACUBETYPE_HADOOP_QUERY, version)
{
}


AosDataCubeHadoopQuery::~AosDataCubeHadoopQuery()
{
}


AosDataConnectorObjPtr 
AosDataCubeHadoopQuery::cloneDataConnector()
{
	return OmnNew AosDataCubeHadoopQuery(*this);
}


AosJimoPtr 
AosDataCubeHadoopQuery::cloneJimo() const
{
	try
	{
		return OmnNew AosDataCubeHadoopQuery(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating jimo" << enderr;
		return 0;
	}
}


bool
AosDataCubeHadoopQuery::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc)
{
	// 'worker_doc' serves as the metadata                                             
                                                                                   
	aos_assert_rr(worker_doc, rdata, false);                                           

	AosXmlTagPtr file_doc = worker_doc->getFirstChild("file");                         
	mIP = file_doc->getAttrStr("ip");                                                  
	mPort = file_doc->getAttrInt("port", 9000);                                        
	aos_assert_rr(mIP!="", rdata, false);                                              
	aos_assert_rr(mPort>0, rdata, false);                                              
	try{
		if(!mFS)
		{
			signal(SIGPIPE, SIG_IGN);
			mFS = hdfsConnect(mIP.data(), mPort); 
			if (!mFS) {                                                            
				AosSetErrorUser(rdata, "Failed to connect to hdfs!\n : ") << enderr;
				return false;
			}
		}
	}
	catch(...)
	{
		AosSetErrorUser(rdata, "Failed to connect hadoop!") << worker_doc->toString() << enderr;
		return false;
	}
    return true;                                                                          
}


bool
AosDataCubeHadoopQuery::start(AosRundata *rdata)
{
	return true;
}

bool
AosDataCubeHadoopQuery::readData(const u64 reqid, AosRundata *rdata)
{
	AosBuffPtr buff =  OmnNew AosBuff(100000 AosMemoryCheckerArgs);
	AosBuffDataPtr buff_data = 0;
	AosDiskStat disk_stat;

	if (mBuff.notNull())
	{
		while(mBuff->getCrtIdx() < mBuff->dataLen())
		{
			u64 docid = mBuff->getU64(0);
			OmnString record = getRecordByDocid(docid, rdata);
			buff->setBuff(record.data(), record.length());
		}

		buff_data = OmnNew AosBuffData(0, buff, disk_stat);
	}

	callBack(reqid, buff_data, true);
	return true;
}

void
AosDataCubeHadoopQuery::callBack(
		const u64 &reqid,
		const AosBuffDataPtr &buff,
		bool finish)
{
	mCaller->callBack(reqid, buff, finish);
}


void
AosDataCubeHadoopQuery::setCaller(const AosDataConnectorCallerObjPtr &caller)
{
	mCaller = caller;
}


bool
AosDataCubeHadoopQuery::setValueBuff(
			const AosBuffPtr &buff,
			AosRundata *rdata)
{
	aos_assert_r(buff, false);
	mBuff = buff;
	mBuff->reset();
	return true;
}

OmnString
AosDataCubeHadoopQuery::getRecordByDocid(u64 &docid, AosRundata *rdata)
{                                                                                       
    //1.get doc by docid
    //2.get filename,offset,length from doc
    //3.get record from hadoop by filename,offset,length
    //4.return record
    OmnString record = "";
    aos_assert_rr(docid, rdata, "");
	AosXmlTagPtr doc;
    doc = AosDocClientObj::getDocClient()->getDocByDocid(docid, rdata); 
    OmnString fileName = doc->getAttrStr("filename");
    tOffset offset = doc->getAttrInt("offset", 0);
    i64 blockSize = doc->getAttrInt64("length", 0);
    hdfsFile file;
    char * block = OmnNew char[blockSize];     
    AosReadHadoopFile(rdata, block, mFS, file, offset, blockSize);
    record << OmnString(block); 
    return record;
}
#endif

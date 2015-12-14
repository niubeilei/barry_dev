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
// 2013/11/24 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataCube/Jimos/DataCubeSQLData.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "TransUtil/Ptrs.h"
#include "Util/CodeConvertion.h"
#include "MySQL/DataStoreMySQL.h"
#include "Database/DbTable.h" 
#include "Database/DbRecord.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosDataCubeSQLData_0(
		const AosRundataPtr &rdata,
		const u32 version) 
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosDataCubeSQLData(version);
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


AosDataCubeSQLData::AosDataCubeSQLData(const u32 version)
:
AosDataCube(AOS_DATACUBETYPE_SQLDATA, version),
mCaller(0),
mNumOfField(0),
mCrtIdx(0),
mNumValues(0),    
mRaw(0),
mBlockSize(0)
{
}


AosDataCubeSQLData::~AosDataCubeSQLData()
{
	if (mMySql)
	{
		bool rslt = mMySql->closeDb();                                                                                               
		aos_assert(rslt);
	}
}


bool
AosDataCubeSQLData::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc)
{
	// worker_doc looks like :                                        
	//  <worker_doc
	//       zky_user="xxxx"                                   
	//       zky_passwd="xxx"                                  
	//       zky_database_ip="xxx"                             
	//       zky_database_prot="xxxx"                          
	//       zky_num_of_field="xx"                             
	//       zky_database_name="xxxx" >                        
	//      <head>xxxx,xxxx,xxxxx,xxxx,xxx,xxx</head>          
	//      <sql><![CDATA[ select * from xxxxx]]></sql>        
	//  </worker_doc>                                                

	aos_assert_rr(worker_doc, rdata, false);
	mBlockSize = worker_doc->getAttrU32("zky_blocksize", eDftBlockSize);

	OmnString user = worker_doc->getAttrStr("zky_user");                       
	aos_assert_r(user != "", false);                          
															   
	OmnString passwd = worker_doc->getAttrStr("zky_passwd");                   
	aos_assert_r(passwd != "", false);                        
															   
	OmnString databaseName = worker_doc->getAttrStr("zky_database_name");      
	aos_assert_r(databaseName != "", false);                  
															   
	OmnString ip = worker_doc->getAttrStr("zky_database_ip", "127.0.0.1");     
	aos_assert_r(ip != "", false);                            
															   
	u32 dataBasePort = worker_doc->getAttrU32("zky_database_prot", 3306);
															   
	mNumOfField = worker_doc->getAttrInt("zky_num_of_field", 0);      
															   
	AosXmlTagPtr headnode = worker_doc->getFirstChild("head");        
	aos_assert_r(headnode, false);                             
															   
	OmnString head = headnode->getNodeText();                  
	AosStrSplit split(head, ",");                              
	mHeads = split.entriesV();                                 
	AosXmlTagPtr sqlnode = worker_doc->getFirstChild("sql");          
	aos_assert_r(sqlnode, false);                              
															   
	mSql = sqlnode->getNodeText();                             
	aos_assert_r(mSql != "", false);                           

	mMySql = OmnNew OmnDataStoreMySQL(user, passwd, ip, databaseName, dataBasePort, "");          
	if (!mMySql)                                                                                                            
	{                                                                                                                      
		rdata->setError() << "Failed to retrieve the store!";                                                              
		return false;                                                                                                      
	} 
	return true;
}


int                                             
AosDataCubeSQLData::getPhysicalId() const           
{                                               
	OmnShouldNeverComeHere;                     
	return -1;                                  
}                                               


int64_t                                         
AosDataCubeSQLData::getFileLength() const           
{                                               
	OmnShouldNeverComeHere;                     
	return -1;                                  
}                                               


OmnString                                       
AosDataCubeSQLData::getFileName() const             
{                                               
	OmnShouldNeverComeHere;                     
	return "";                                  
}                                               


AosDataConnectorObjPtr 
AosDataCubeSQLData::cloneDataConnector()
{
	return OmnNew AosDataCubeSQLData(*this);
}

void
AosDataCubeSQLData::setCaller(const AosDataConnectorCallerObjPtr &caller)
{
	mCaller = caller;	
}


bool
AosDataCubeSQLData::readData(
		const u64 reqid,
		AosRundata *rdata)
{
	if (!mMySql)
	{   
		rdata->setError() << "Failed to retrieve the store!";                                                              
		return false;                                                                                                      
	}                                                                                                                      

	OmnRslt rslt = mMySql->openDb();

	if (!rslt)                                                                                                             
	{                                                                                                                      
		rdata->setError()<< "Failed to retrieve the store!";                                                               
		return false;                                                                                                      
	}                                                                                                                      

	if ( !(mMySql->query2(mSql, mRaw)))
	{                                                                                                                      
		rdata->setError()<< "Failed To Run SQL: " << mSql << enderr;                                                       
		return false;                                                                                                      
	}                                                                                                                      

//	mRecordNum = mySql->getNumOfRecord();

	AosBuffPtr buff = OmnNew AosBuff(200 AosMemoryCheckerArgs); 
	OmnRslt rslt1(true); 
	while(mRaw->hasMore())                                                                                                 
	{                                                                                                                      
		OmnDbRecordPtr record = mRaw->next2();

		aos_assert_r(record, false)
		for(u32 j = 0; j < mNumOfField; j++)                             
		{                                                                
			OmnString value = record->getStr(j, "", rslt1); 
			if (rslt1) break;
			buff->setOmnStr(value);
		}
	}

	AosDiskStat disk_stat;                                              
	AosBuffDataPtr buff_data = OmnNew AosBuffData(0, buff, disk_stat);  
	aos_assert_r(mCaller, false);                                                
	mCaller->callBack(reqid, buff_data, true);                    
	return true;
}


void                        
AosDataCubeSQLData::callback(   
		const AosTransPtr &trans,
		const AosBuffPtr &resp,
		const bool svr_death)   
{                           
	OmnShouldNeverComeHere;
	return ;
}

bool 
AosDataCubeSQLData::readBlock(
		AosRundata *rdata, 
		AosBuffDataPtr &buff_data)
{
	//return rslt;
	OmnShouldNeverComeHere;
	return false;
}


bool 
AosDataCubeSQLData::appendBlock(
		AosRundata *rdata, 
		AosBuffDataPtr &buff_data)
{
	OmnShouldNeverComeHere;
	return false;
}

bool 
AosDataCubeSQLData::writeBlock(
		AosRundata *rdata, 
		const int64_t pos,
		AosBuffDataPtr &buff_data)
{
	OmnShouldNeverComeHere;
	return false;
}

bool 
AosDataCubeSQLData::copyData(
		AosRundata *rdata, 
		const OmnString &from_name,
		const OmnString &to_name)
{
	OmnShouldNeverComeHere;
	return false;
}

bool 
AosDataCubeSQLData::removeData(
		AosRundata *rdata, 
		const OmnString &name)
{
	OmnShouldNeverComeHere;
	return false;
}


bool
AosDataCubeSQLData::serializeTo(
		AosRundata *rdata, 
		const AosBuffPtr &buff)
{
	OmnShouldNeverComeHere
	return false;
}


bool
AosDataCubeSQLData::serializeFrom(
		AosRundata *rdata, 
		const AosBuffPtr &buff)
{
	OmnShouldNeverComeHere
	return false;
}


AosJimoPtr 
AosDataCubeSQLData::cloneJimo() const
{
	try
	{
		return OmnNew AosDataCubeSQLData(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating jimo" << enderr;
		return 0;
	}
}


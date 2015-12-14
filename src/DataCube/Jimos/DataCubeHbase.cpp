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
// 2014/01/21 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "DataCube/Jimos/DataCubeHbase.h"

#if 0
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "API/AosApiG.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <poll.h>

#include <iostream>

#include <boost/lexical_cast.hpp>
#include <boost/make_shared.hpp>
#include <protocol/TBinaryProtocol.h>
#include <transport/TSocket.h>
#include <transport/TTransportUtils.h>

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::hadoop::hbase::thrift;
typedef std::vector<std::string> StrVec;
typedef std::map<std::string,std::string> StrMap;
typedef std::vector<ColumnDescriptor> ColVec;
typedef std::map<std::string,ColumnDescriptor> ColMap;
typedef std::vector<TCell> CellVec;
typedef std::map<std::string,TCell> CellMap;

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosDataCubeHbase_0(
		const AosRundataPtr &rdata,
		const u32 version) 
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosDataCubeHbase(version);
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


AosDataCubeHbase::AosDataCubeHbase(const u32 version)
:
AosDataCube(AOS_DATACUBETYPE_HBASE, version)
{
}


AosDataCubeHbase::~AosDataCubeHbase()
{
}


AosDataConnectorObjPtr 
AosDataCubeHbase::cloneDataConnector()
{
	return OmnNew AosDataCubeHbase(*this);
}


AosJimoPtr 
AosDataCubeHbase::cloneJimo() const
{
	try
	{
		return OmnNew AosDataCubeHbase(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating jimo" << enderr;
		return 0;
	}
}


bool
AosDataCubeHbase::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc)
{
	//  worker_doc format 
	//  <data_cube jimo_objid="datacube_stat" >
	//	</data_cube>
	
	aos_assert_rr(worker_doc, rdata, false);

	OmnString addr = worker_doc->getAttrStr("zky_hbase_addr");
	aos_assert_r(addr != "", false);
	OmnString port = worker_doc->getAttrStr("zky_hbase_port");
	aos_assert_r(port != "", false);
	mTname = worker_doc->getAttrStr("zky_hbase_table_name");
	aos_assert_r(mTname != "", false);

	bool isFramed = false;
	boost::shared_ptr<TTransport> socket(new TSocket(addr.data(), boost::lexical_cast<int>(port.data())));
	boost::shared_ptr<TTransport> transport;

	if (isFramed) {
		transport.reset(new TFramedTransport(socket));
	} else {
		transport.reset(new TBufferedTransport(socket));
	}
	boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));

	mClient = boost::make_shared<HbaseClient>(protocol);
	try {
		transport->open();
	} catch (const TException &tx) {
		std::cerr << "ERROR: " << tx.what() << std::endl;
		return false;
	}
	return true;
}


bool
AosDataCubeHbase::start(AosRundata *rdata)
{
	return true;
}


bool
AosDataCubeHbase::readData(const u64 reqid, AosRundata *rdata)
{
	AosBuffPtr buff =  OmnNew AosBuff(100000 AosMemoryCheckerArgs);
	AosBuffDataPtr buff_data = 0;
	AosDiskStat disk_stat;

	if (mBuff.notNull())
	{
		while(mBuff->getCrtIdx() < mBuff->dataLen())
		{
			OmnString rawkey = mBuff->getOmnStr("");
			OmnString rslt = getRecordByRowid(rawkey);
			buff->setBuff(rslt.data(), rslt.length());
		}

		buff_data = OmnNew AosBuffData(0, buff, disk_stat);
	}

	callBack(reqid, buff_data, true);
	return true;
}


void
AosDataCubeHbase::callBack(
		const u64 &reqid,
		const AosBuffDataPtr &buff,
		bool finish)
{
	mCaller->callBack(reqid, buff, finish);
}


void
AosDataCubeHbase::setCaller(const AosDataConnectorCallerObjPtr &caller)
{
	mCaller = caller;
}


bool
AosDataCubeHbase::setValueBuff(
			const AosBuffPtr &buff,
			AosRundata *rdata)
{
	aos_assert_r(buff, false);
	mBuff = buff;
	mBuff->reset();
	return true;
}


OmnString
AosDataCubeHbase::getRecordByRowid(OmnString rowid)
{
	OmnString record = "";
	record << "\"" << rowid << "\"";
  	const map<Text, Text> dummyAttributes;
    string t(mTname.data());
	string row(rowid.data());
	std::vector<TRowResult> rowResult;

	aos_assert_r(mClient, "")
	mClient->getRow(rowResult, t, row, dummyAttributes);
	if (rowResult.size() < 1)
	{
		OmnString rowid1 = "";

		//sometimes, we will filter out double quotes while
		//hbase will keep them
		rowid1 << "\"" << rowid << "\"";

		//get row based on new string
		string row1(rowid1.data());
		mClient->getRow(rowResult, t, row1, dummyAttributes);
	}

	aos_assert_r(rowResult.size() > 0, "");
	if (rowResult.size() > 0)
	{
		OmnString value = "";
		for (CellMap::const_iterator it = rowResult[0].columns.begin();
		it != rowResult[0].columns.end(); ++it) {

			value = "";
			value << it->second.value;
			//if the value already has double quote. No need to add a new one
			if (value.getChar(0) == '\"')
				record << "," << value << "";
			else
				record << ",\"" << value << "\"";
		}
	}

	record << "\n";
	return record;
}

#endif

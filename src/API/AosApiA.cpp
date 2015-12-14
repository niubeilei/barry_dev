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
// handle the SEServer send request to MsgServer 
//
// Modification History:
// 08/13/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "API/AosApiA.h"

#include "SEInterfaces/IILClientObj.h"
#include "SEInterfaces/SeLogClientObj.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/Buff.h"

using namespace std;

static AosIILClientObjPtr sgIILClient;
static AosSeLogClientObjPtr	sgSeLogClient;

bool AosAddU64ValueDoc(
		const u64 &iilid, 
		const u64 &key, 
		const u64 &docid, 
		const bool value_unique, 
		const bool docid_unique, 
		AosRundata *rdata)
{
	if (!sgIILClient) sgIILClient = AosIILClientObj::getIILClient();
	aos_assert_r(sgIILClient, false);
	return sgIILClient->addU64ValueDoc(
			iilid, key, docid, value_unique, docid_unique, rdata);
}

bool AosAddU64ValueDocToTable(
		const u64 &iilid, 
		const u64 &key, 
		const u64 &docid, 
		const bool value_unique, 
		const bool docid_unique, 
		const AosRundataPtr &rdata)
{
	if (!sgIILClient) sgIILClient = AosIILClientObj::getIILClient();
	aos_assert_r(sgIILClient, false);
	return sgIILClient->addU64ValueDocToTable(
			iilid, key, docid, value_unique, docid_unique, rdata);
}

bool AosAddContainerLog(
		const OmnString &container_objid,
		const OmnString &docstr,
		const AosRundataPtr &rdata)
{
	// Ketty 2012/05/12
	if(!sgSeLogClient)	sgSeLogClient = AosSeLogClientObj::getLogClient();
	aos_assert_r(sgSeLogClient, false);
	return sgSeLogClient->addLog(rdata, container_objid, "", docstr);
}


bool AosAddLog(
		const AosRundataPtr &rdata,
		const OmnString &container_objid,
		const OmnString &log_name,
		const OmnString &docstr)
{
	// Ketty 2012/05/12
	if(!sgSeLogClient)	sgSeLogClient = AosSeLogClientObj::getLogClient();
	aos_assert_r(sgSeLogClient, false);
	return sgSeLogClient->addLog(rdata, container_objid, log_name, docstr);
}


bool AosAddStrValueDoc(
		//vector<AosBuffPtr> &buffs,
		vector<AosTransPtr>  *allTrans,
		const u32 arr_len,
		const OmnString &iilname, 
		const OmnString &value, 
		const u64 &docid, 
		const bool value_unique,
		const bool docid_unique,
		int &physical_id,
		AosRundata *rdata)
{
	if (!sgIILClient) sgIILClient = AosIILClientObj::getIILClient();
	aos_assert_r(sgIILClient, false);
	//return sgIILClient->addStrValueDoc(
	//		buffs, iilname, value, docid, value_unique, docid_unique, physical_id, rdata);
	return sgIILClient->addStrValueDoc(
			allTrans, arr_len, iilname, value, docid, value_unique, docid_unique, physical_id, rdata);
}

bool AosAddStrValueDoc(
				const OmnString &iilname,
				const OmnString &value,
				const u64 &docid,
				const bool value_unique,
				const bool docid_unique,
				const AosRundataPtr &rdata)
{
	if (!sgIILClient) sgIILClient = AosIILClientObj::getIILClient();
	aos_assert_r(sgIILClient, false);
	return sgIILClient->addStrValueDoc(
			iilname, value, docid, value_unique, docid_unique, rdata);
}

bool AosAppendBinaryDoc(
		const OmnString &objid, 
		const AosBuffPtr &buff, 
		AosRundata *rdata)
{
	AosDocClientObjPtr docclient = AosDocClientObj::getDocClient();
	aos_assert_r(docclient, 0);

	return docclient->appendBinaryDoc(objid, buff, rdata);
}

// Chen Ding, 2013/01/13
void AosAddXmlBinaryData(
		OmnString &str, 
		const OmnString &tagname, 
		const char *data, 
		const int len)
{
	str << "<" << tagname << "><![BDATA[" << len << ":";
	str.append(data, len);
	str << "]]></" << tagname << ">";
}


// Chen Ding, 2013/01/13
void AosAddXmlBinaryData(
		OmnString &str, 
		const OmnString &tagname, 
		const AosBuffPtr &buff)
{
	str << "<" << tagname << "><![BDATA[" << buff->dataLen() << ":";
	str.append(buff->data(), buff->dataLen());
	str << "]]></" << tagname << ">";
}

// JACKIE-HADOOP
bool AosAppendBlockToHadoop(
        AosRundata *rdata,
        const OmnString &fname,
        AosBuffPtr &buff,
        const int64_t bytes_to_read)
{
	OmnNotImplementedYet;
	return false;
	/*
    shared_ptr<TSocket> socket(new TSocket("localhost", 9090));
    shared_ptr<TBufferedTransport> transport(new TBufferedTransport(socket));
    shared_ptr<TBinaryProtocol> protocol(new TBinaryProtocol(transport));
    AosHadoopAPIServiceClient client(protocol);

    try {
        transport->open();

        AosRundataApi rdata;
        string filename(fname.data());

		string contents(buff->data());
        client.appendBlock(rdata, filename, contents, bytes_to_read);

        transport->close();
    } catch (TException &tx) {
        printf("ERROR: %s\n", tx.what());
		return false;
    }
    return true;
	*/
}

bool AosAddVector2D(
		const AosRundataPtr &rdata,
		const u64 &sdocid,
		const OmnString &key_str,
		const AosValueRslt &time_rslt,
		const vector<AosValueRslt> &measures)
{
	OmnNotImplementedYet;
	return true;
}

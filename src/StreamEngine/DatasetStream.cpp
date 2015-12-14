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
// 05/14/2012 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "StreamEngine/DatasetStream.h"

#include "API/AosApi.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"  
#include "Rundata/Rundata.h"
#include "JQLStatement/JqlStmtDataset.h"
#include "SEInterfaces/DataRecordObj.h"
#include "API/JimoCreators.h"
#include "StreamEngine/Service.h"
#include "StreamEngine/StreamDataProc.h"
#include "DataRecord/StreamRecordset.h"
#include "Porting/Sleep.h"
#include <boost/make_shared.hpp> 

static map<OmnString, RecordsetList> sgRecordsetMap;
static bool sgDebug = false;

extern "C"
{

	AosJimoPtr AosCreateJimoFunc_AosDatasetStream_0(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosDatasetStream(version);
			aos_assert_r(jimo, 0);
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

//
//constructor/destructors/clone
//
AosDatasetStream::AosDatasetStream(int idx)
:
AosGenericObj(idx)
{
	initCounters();
}

AosDatasetStream::AosDatasetStream(const AosDatasetStream &stream)
:
AosGenericObj(0)
{
	initCounters();
}

AosDatasetStream::~AosDatasetStream()
{
}

AosJimoPtr
AosDatasetStream::clone()                          
{
	return this;
}

AosJimoPtr
AosDatasetStream::clone(
		const AosXmlTagPtr &def,
		const AosRundataPtr &)
{
	return NULL;
}

void
AosDatasetStream::initCounters()
{
	mDatasetStreamTime = 0;
	mDatasetStreamNum = 0;
	mServiceTime = 0;
	mServiceNum = 0;

}


void
AosDatasetStream::outputCounters()
{
	OmnScreen << "DatasetStream Counters : --- Time : " << mDatasetStreamTime 
			<< " Num : " << mDatasetStreamNum << endl;

	OmnScreen << "Service Counters : --- Time : " << mServiceTime 
			<< " Num : " << mServiceNum << endl;
}


AosJimoPtr
AosDatasetStream::cloneJimo() const
{
	return OmnNew AosDatasetStream(*this);
}


bool 
AosDatasetStream::createByJql(
		AosRundata *rdata,
		const OmnString &obj_name,
		const OmnString &jsonstr,
		const AosJimoProgObjPtr &prog)
{
	//save above json into a doc
	//AosJimoScheduler scheduler;
	try
	{
		JSONReader reader;
		JSONValue jsonObj;
		reader.parse(jsonstr, jsonObj);

		OmnString objid = AosObjid::getObjidByJQLDocName(JQLTypes::eDatasetDoc, obj_name);
		OmnString dp_str = "";
		dp_str 
			<< "<DatasetStream " << AOSTAG_CTNR_PUBLIC << "=\"true\" "
			<<      AOSTAG_PUBLIC_DOC << "=\"true\" " << AOSTAG_OBJID << "=\"" << objid << "\">"
			<<          "<![CDATA[" << jsonstr << "]]>"
			<< "</DatasetStream>";

		return AosCreateDoc(dp_str, true, rdata);
	}
	catch (...)
	{
		OmnScreen << "JSONException... " << endl;
	}

	return true;
}


bool 
AosDatasetStream::showByJql(
		AosRundata *rdata,
		const OmnString &objName, 
		const OmnString &jsonstr)
{
	return true;
}


bool 
AosDatasetStream::runByJql(
		AosRundata *rdata,
		const OmnString &objName, 
		const OmnString &jsonstr)
{
	//felicia, 2015/09/11
	JSONValue json;
	JSONReader reader;
	bool rslt = reader.parse(jsonstr, json);
	aos_assert_r(rslt, false);

	OmnString svr_name = json["service_name"].asString();
	
	OmnString svr_objid = AosObjid::getObjidByJQLDocName(JQLTypes::eServiceDoc, svr_name);
	AosXmlTagPtr doc = AosGetDocByObjid(svr_objid, rdata);
	aos_assert_r(doc, false);

	OmnString objname = doc->getAttrStr("zky_objname");
	aos_assert_r(objname != "", false);

	OmnString objid = AosObjid::getObjidByJQLDocName(JQLTypes::eServiceDoc, objname);
	doc = AosGetDocByObjid(objid, rdata);
	aos_assert_r(doc, false);

	AosXmlTagPtr datasets_node = doc->getFirstChild("datasets");
	aos_assert_r(datasets_node, false);

	AosXmlTagPtr ds_doc = datasets_node->getChildByAttr("zky_name", objName);
	aos_assert_r(ds_doc, false);
/*
	//get the service job configuration
	OmnString objid = AosObjid::getObjidByJQLDocName(JQLTypes::eDatasetDoc, objName);
	AosXmlTagPtr doc = AosGetDocByObjid(objid, rdata);
	aos_assert_r(doc, false);
*/
	//create original dataset
	AosDatasetObjPtr ds;
	AosRecordsetObjPtr rs;
	u64	tStart, tEnd, tStartT, tEndT;
	ds = AosCreateDataset(rdata, ds_doc);
	rslt = ds->config(rdata, ds_doc);
	aos_assert_r(rslt, false);

	rslt = ds->sendStart(rdata);   
	aos_assert_r(rslt, false);
	//AosRecordsetObjPtr streamrs = OmnNew AosStreamRecordset();
	while (true)
	{
		tStart = OmnGetTimestamp();
		aos_assert_r(ds->nextRecordset(rdata, rs), false);
		tEnd = OmnGetTimestamp();
		mDatasetStreamTime += tEnd - tStart;
		mDatasetStreamNum++;

		if (AosRecordsetObj::checkEmpty(rs))
			break;

		//if too many data buffered for dataproc to process,
		//this method will hang
		tStartT = OmnGetTimestamp();
		
		//AosRecordsetObjPtr streamrs = OmnNew AosStreamRecordset();
		
		// jimodb-1301, 2015.12.03
		// append record to stream recordset
		AosDataRecordObj* record = NULL;
		bool rslt = rs->nextRecord(rdata, record); 
		aos_assert_r(rslt,false);                  

		AosRecordsetObjPtr streamrs = AosRecordsetObj::createStreamRecordsetStatic(rdata,record);

		while(1)
		{
			bool rslt = rs->nextRecord(rdata, record);
			aos_assert_r(rslt,false);
			if(!record) break;

			AosDataRecordObjPtr recordptr = record;
			AosBuffDataPtr buff;
			streamrs->appendRecord(rdata,recordptr,buff);
		}

		//streamrs->setDataBuff(rdata, rs->getDataBuff());
		//AosService::dataArrived(rdata, objName, rs, "");
		//use stream data to distribute external data
		AosStreamDataPtr sData = AosStreamData::getStreamData(rdata, objName, svr_name);
		aos_assert_r(sData, false);
		sData->feedStreamData(rdata, streamrs);

		
		//q
		//print by levi
		//char *curData = rs->getData();
		//OmnScreen << "datasetstream curData : " << curData << endl;

		tEndT = OmnGetTimestamp();
		mServiceTime = tEndT - tStartT;
		mServiceNum++;

		outputCounters();
		//rs = 0;
	}
	rslt = ds->sendFinish(rdata);
	aos_assert_r(rslt, false);

	return true;
}


//////////////////////////////////////////////////////////////////
//     help methods
//////////////////////////////////////////////////////////////////
bool
AosDatasetStream::isDebug()
{
	return sgDebug;
}

bool
AosDatasetStream::addDebugData(
		OmnString rsName,
		AosRecordsetObjPtr rsPtr)
{
	aos_assert_r(rsPtr, false);

	//if debugging mode, append the recordset to the global 
	//recordset map
	vector<AosRecordsetObjPtr> *rsList;
	vector<AosRecordsetObjPtr> emptyList;
	map<OmnString, RecordsetList>::iterator rsItr;
	if (sgDebug)
	{
		rsItr = sgRecordsetMap.find(rsName);
		if (rsItr != sgRecordsetMap.end())
		{
			rsList = &(rsItr->second);
		}
		else
		{
			sgRecordsetMap[rsName] = emptyList;
			rsList = &(sgRecordsetMap[rsName]);
		}

		rsList->push_back(rsPtr);
	}

	return true;
}


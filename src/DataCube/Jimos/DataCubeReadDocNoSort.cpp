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
#include "DataCube/Jimos/DataCubeReadDocNoSort.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "API/AosApiG.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "DataCube/ReadDocUtil/Ptrs.h"
#include "DataCube/ReadDocUtil/DocOpr.h"
#include "DataCube/ReadDocUtil/ReadDocNoSortUnit.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosDataCubeReadDocNoSort_0(
		const AosRundataPtr &rdata,
		const u32 version) 
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosDataCubeReadDocNoSort(version);
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


AosDataCubeReadDocNoSort::AosDataCubeReadDocNoSort(const u32 version)
:
AosDataCubeCtnr(AOS_DATACUBETYPE_READDOC_NOSORT, version)
{
}


AosDataCubeReadDocNoSort::~AosDataCubeReadDocNoSort()
{
}


AosDataConnectorObjPtr 
AosDataCubeReadDocNoSort::cloneDataConnector()
{
	return OmnNew AosDataCubeReadDocNoSort(*this);
}


AosJimoPtr 
AosDataCubeReadDocNoSort::cloneJimo() const
{
	try
	{
		return OmnNew AosDataCubeReadDocNoSort(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating jimo" << enderr;
		return 0;
	}
}


bool
AosDataCubeReadDocNoSort::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc)
{
	//  worker_doc format 
	//  <data_cube jimo_objid="datacube_stat" >
	//		<docids num="xxx" ><![BDATA[xxxx]]>	
	//		</docids>
	//		<conds>
	//			....
	//		</conds>
	//	</data_cube>
	
	aos_assert_rr(worker_doc, rdata, false);
	
	bool rslt = splitDocids(rdata.getPtr(), worker_doc);
	aos_assert_r(rslt, false);

	return true;
}



bool
AosDataCubeReadDocNoSort::splitDocids(
		AosRundata *rdata,
		const AosXmlTagPtr &conf)
{
	AosBuffPtr docids_buff;
	u32 total;
	bool rslt = getDocidsByConf(conf, docids_buff, total);
	aos_assert_r(rslt, false);
	
	AosDocOprPtr opr = AosDocOpr::getDocOpr(rdata, conf);
	aos_assert_r(opr, false);
	
	map<u32, IdVector> docid_grp;
	rslt = opr->shufferDocids(rdata, docids_buff, total, docid_grp);
	aos_assert_r(rslt, false);

	AosReadDocNoSortUnitPtr reader;
	AosDataConnectorCallerObjPtr thisptr(this, false);
	map<u32, IdVector>::iterator itr;
	for(; itr != docid_grp.end(); itr++)
	{
		reader = OmnNew AosReadDocNoSortUnit(opr, itr->first, itr->second );
		reader->setCaller(thisptr);
		addDataConnector(reader);
	}
	
	return true;
}

bool
AosDataCubeReadDocNoSort::getDocidsByConf(
		const AosXmlTagPtr &conf,
		AosBuffPtr &docids_buff,
		u32 &total)
{
	AosXmlTagPtr docids_xml = conf->getFirstChild("docids");
	aos_assert_r(docids_xml, false);

	total = docids_xml->getAttrU32("num", 0);
	aos_assert_r(total, false);
	return true;
}



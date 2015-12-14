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
// 2014/07/26 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "LogEntry/LogEntry.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/JimoLogicObj.h"
#include "Thread/Mutex.h"

static hash_map<OmnString, AosLogParserObjPtr> sgJimoMap;
static OmnMutex sgLock;
static bool sgInited = false;

AosLogParser::AosLogParser()
:
AosLogParserObj(1)
{
}


AosLogParser::AosLogParser(const int version)
:
AosLogParserObj(version)
{
}


AosLogParser::~AosLogParser()
{
}


bool
AosLogParserObj::init()
{
	sgLock.lock();
	if (sgInited)
	{
		sgLock.unlock();
		return true;
	}
	
	AosRundataPtr rdata = OmnNew AosRundata();
	AosRundata *rdata_raw = rdata.getPtr();

	createJimoDoc(rdata_raw, "BinSin", 		"bin_sin");
	createJimoDoc(rdata_raw, "BinMul", 		"bin_mul");
	createJimoDoc(rdata_raw, "BinSinExt", 	"bin_sin_ext");
	createJimoDoc(rdata_raw, "BinMulExt", 	"bin_mul_ext");
	createJimoDoc(rdata_raw, "NameValue", 	"name_value");
	createJimoDoc(rdata_raw, "CSV", 		"csv");
	createJimoDoc(rdata_raw, "JSON", 		"json");
	createJimoDoc(rdata_raw, "BSON", 		"bson");
	createJimoDoc(rdata_raw, "XML", 		"xml");

	sgInited = true;
	sgLock.unlock();
	return true;
}


bool
AosLogParser::createJimoDoc(
		AosRundata *rdata, 
		const OmnString &classname, 
		const OmnString &key)
{
	// Create "binary_multi"
	OmnString jimostr;
	jimostr = "<jimo ";
	jimostr << AOSTAG_CURRENT_VERSION << "=\"1\" "
		<< "zky_classname=\"AosLogParser" << classname << "\" "
		<< "zky_otype=\"" << AOSOTYPE_JIMO << "\" "
		<< "zky_objid=\"jimo_logparser_" << key << "\">"
		<< "<versions>"
		<< 		"<version_1>libLogEntryJimos.so</version_1>"
		<< "</versions>"
		<< "</jimo>";
	AosXmlTagPtr xml = AosStr2Xml(rdata_raw, jimostr);
	sgJimoMap[key] = xml;
	return true;
}


AosLogParserObjPtr 
AosLogParserObj::createLogParser(
		const OmnString &type, 
		AosRundata *rdata)
{
	if (!sgInited) init();
	itr_t itr = sgJimoMap.find(type);
	if (itr == sgJimoMap.end())
	{
		AosSetErrorUser(rdata, "operation_error", "log_entry_not_found") << type << enderr;
		return 0;
	}

	return itr->second->clone();
}


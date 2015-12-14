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
// 01/01/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_SeLogClientObj_h
#define Aos_SEInterfaces_SeLogClientObj_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/Ptrs.h"

#define AOSSYSLOG_CREATE(isSuccess, logname, rdata) AosSeLogClientObj::getLogClient()->addSysLog(isSuccess, logname, rdata);

#ifndef endlog
#define endlog AosEndLogEntry()
#endif

class AosLogCaller : virtual public OmnRCObject
{
public:
	virtual bool entryRetrieved(
							const AosRundataPtr &rdata, 
							const OmnString &entry) = 0;

	virtual bool entriesRetrieved(
							const AosRundataPtr &rdata, 
							const vector<OmnString> &entry) = 0;
};
				

class AosSeLogClientObj : virtual public OmnRCObject
{
private:
	static AosSeLogClientObjPtr		smLogClient;

public:
	virtual bool createLogger(	
							const AosRundataPtr &rdata, 
	 						const OmnString &log_name) = 0;

	virtual bool removeLogger(	
							const AosRundataPtr &rdata, 
	 						const OmnString &log_name) = 0;

	virtual u64 addLog(	const AosRundataPtr &rdata, 
	 						const OmnString &log_objid,
	 						const OmnString &entry) = 0;

	virtual bool addLog(
							const AosRundataPtr &rdata,
							const OmnString &pctr_objid, 
							const OmnString &log_name,
							const OmnString &entry) = 0;

	virtual bool retrieveLog(
							const AosRundataPtr &rdata, 
							const OmnString &log_name,
	 						const u64 entry_id, 
							OmnString &entry,
							const AosLogCallerPtr &caller) = 0;

	virtual bool retrieveLogs(
							const AosRundataPtr &rdata, 
							const OmnString &log_name,
	 						const vector<u64> &entry_id, 
							vector<OmnString> &entries,
							const AosLogCallerPtr &caller) = 0;
	
	virtual bool addSysLog(
							const bool &isSuccess,
							const OmnString &logname,
							const AosRundataPtr &rdata) = 0;
    
	inline static AosSeLogClientObjPtr getLogClient() {return smLogClient;}
	inline static AosSeLogClientObjPtr getSelf() {return smLogClient;}
	static void setLogClient(const AosSeLogClientObjPtr &c) {smLogClient = c;}

	// Chen Ding, 2014/11/08
	virtual bool addLog(	AosRundata *rdata, 
	 						const OmnString &log_name,
	 						AosBuff *entry) = 0;
	
	// YaZong Ma, 2015/1/16
	virtual bool addLoginLog(
							const AosXmlTagPtr &logdoc, 
							const AosRundataPtr &rdata) = 0;

	virtual bool addInvalidReadEntry(
							const AosXmlTagPtr &doc, 
							const AosRundataPtr &rdata) = 0;

	virtual u64	addVersion(
							const AosXmlTagPtr &doc,
							const AosRundataPtr &rdata) = 0;

};
#endif

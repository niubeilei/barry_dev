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
// 03/20/2015 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_StreamEngine_RecordsetStream_h
#define AOS_StreamEngine_RecordsetStream_h

#include "DataRecord/Recordset.h"
#include "DataRecord/DataRecord.h"
#include "DataField/DataField.h"
#include "SEInterfaces/GenericObj.h"
#include "AosConf/DataRecordFixbin.h"
#include "TaskMgr/IDU.h"
#include "JSON/JSON.h"

typedef vector<AosRecordsetObjPtr> RecordsetList;

class AosRecordsetStream : public AosRecordset, public AosGenericObj
{
	OmnDefineRCObject

protected:
	OmnString			mDocidField;
	vector<OmnString>   mFields;
	OmnString			mDelimiter;

public:
	AosRecordsetStream(int idx);
	AosRecordsetStream(const AosRecordsetStream &stream);
	~AosRecordsetStream();

	virtual AosJimoPtr	clone(
					const AosXmlTagPtr &def,
					const AosRundataPtr &rdata);

	virtual AosJimoPtr	clone();
	
	AosJimoPtr	cloneJimo() const;

	//generic obj command
	bool createByJql(
				AosRundata *rdata,
				const OmnString &obj_name,
				const OmnString &jsonstr,
				const AosJimoProgObjPtr &prog);

	bool runByJql(AosRundata *rdata,
				const OmnString &objName,
				const OmnString &jsonstr);
	
	bool showByJql(AosRundata *rdata,
				   const OmnString &objName, 
				   const OmnString &jsonstr);
	
	bool feedByJql(AosRundata *rdata,
				const OmnString &objName,
				const OmnString &jsonstr);

	OmnString getObjType(AosRundata *rdata) {return "";}

	bool parseJQL(                        
			AosRundata *rdata, 
			AosJimoParserObj *jimo_parser,
			AosJimoProgObj *prog,
			bool &parsed, 
			bool dft = false) {return false;}



	static bool isDebug();

	static bool addDebugData(
		OmnString rsName,
		AosRecordsetObjPtr rsPtr);

	virtual bool setSchema(AosRundata*, AosSchemaObjPtr) {return false;}

private:
	bool		addDataField(                                        
					AosConf::DataRecordFixbin &dr,
					const OmnString &name,
					const OmnString &type,
					const OmnString &shortplc,
					const u32 offset,
					const u32 len);

	bool	defineRecord(AosRundata *rdata,
						const OmnString &schema_str,
						const OmnString &rcdName,
						vector<OmnString> &fields,
						AosDataRecordObjPtr &rcdContainer);

	bool		addDataField();

	AosDataFieldType::E convertToDataFieldType(
						AosRundata* rdata,
						const OmnString &str_type);

};
#endif

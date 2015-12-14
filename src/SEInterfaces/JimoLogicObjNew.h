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
// 2015/03/22 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEInterfaces_JimoLogicNewObj_h
#define AOS_SEInterfaces_JimoLogicNewObj_h

#include "Jimo/Jimo.h"
#include "Jimo/Ptrs.h"

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/JimoLogicType.h"

#include "Util/Ptrs.h"
#include "Util/ValueRslt.h"
#include "XmlUtil/Ptrs.h"
#include "JimoProg/JimoProg.h"

class AosJimoProg;

class AosJimoLogicObjNew : virtual public AosJimo
{
public:
	virtual bool compileJQL(AosRundata *rdata, AosJimoProgObj *prog) = 0; 

	virtual bool setUserVarValue(AosRundata *rdata, 
						const AosValueRslt &value) = 0;

	virtual AosValueRslt getUserVarValue(AosRundata *rdata) = 0;

	virtual bool setOutputName(
						AosRundata *rdata, 
						AosJimoProgObj *prog, 
						const OmnString &name) = 0;

	virtual bool setCache(
						AosRundata *rdata, 
						AosJimoProgObj *prog, 
						const OmnString &cache) = 0;

	virtual bool getOutputName(
						AosRundata *rdata,
						AosJimoProgObj *prog,
						OmnString &cache) = 0;

	virtual bool setUnionDatasetName( 
						AosRundata *rdata, 
						AosJimoProgObj *prog,
						const OmnString &name) = 0;

	virtual AosJimoLogicType::E	getJimoLogicType()const = 0;

	virtual vector<OmnString> getOutputNames() = 0;

	// 2015/06/03
	virtual bool setOrderFields(AosRundata* rdata, AosJimoProgObj *prog, const vector<OmnString>& name) = 0;
	virtual bool setOrderType(AosRundata* rdata, AosJimoProgObj *prog, const vector<OmnString>& name) = 0;

	// 2015/06/04
	virtual bool setFields (AosRundata* rdata, AosJimoProgObj* prog, const OmnString& fields_str) = 0;
	/*
	virtual bool setFields (AosRundata* rdata, 
							AosJimoProgObj* prog, 
							const vector<pair<OmnString, pair<OmnString,OmnString> > >&fieldsconf) = 0;
	*/
	virtual bool getFields(vector<AosExprObjPtr> &fields) = 0;
	virtual bool getDistinct(vector<AosExprObjPtr> &distinct) = 0;
	virtual OmnString getOutputByName ( AosRundata *rdata, const OmnString &name) = 0;
	// arvin 2015/06/19
	virtual bool getSchemaName(
						AosRundata *rdata, 
						AosJimoProgObj *prog,
						vector<OmnString> &schema_name) = 0;

	virtual OmnString getIILName() const  = 0;
	virtual OmnString getNewDataProcName() const  = 0;
	virtual vector<OmnString> getSubFields () const = 0;
	virtual OmnString getLogicName()const = 0;

	virtual bool setSubFields(
						AosRundata *rdata, 
						AosJimoProgObj* prog, 
						const vector<OmnString> &subfields) = 0;

	virtual bool getInputList(vector<OmnString> &inputs) = 0;
	virtual OmnString getInput() const = 0;
	virtual OmnString getTableName() const = 0;
	virtual bool isExist(const OmnString &name,const OmnString &parm) = 0;

	// jimodb-753
	//virtual bool getFieldStr(OmnString&) = 0;
	virtual bool getFieldStr(vector<OmnString> &) = 0;

	virtual bool getInputSchema(
			AosRundata *rdata,
			AosJimoProgObj *prog,
			vector<OmnString> &filed_strs) = 0;
};
#endif


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
//
// Modification History:
// 2014/10/26 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_GenericObj_h
#define Aos_SEInterfaces_GenericObj_h

#include "Jimo/Jimo.h"
#include "SEUtil/DocZTGs.h"
#include "Rundata/Ptrs.h"
#include "Util/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/JimoParserObj.h"
#include "SEInterfaces/JimoProgObj.h"
#include "SEInterfaces/Ptrs.h"
#include "JQLStatement/JqlStatement.h"
#include "JQLStatement/Ptrs.h"


#define AOSGENOBJ_METHOD_CREATE 	"create"
#define AOSGENOBJ_METHOD_START 		"start"
#define AOSGENOBJ_METHOD_FEED 		"feed"
#define AOSGENOBJ_METHOD_RUN 		"run"
#define AOSGENOBJ_METHOD_SHOW		"show"
#define AOSGENOBJ_METHOD_DROP 		"drop"
#define AOSGENOBJ_METHOD_STOP 		"stop"

#define AOSVERBNAME_CREATE			"create"
#define AOSVERBNAME_ADD				"add"
#define AOSVERBNAME_DESCRIBE		"describe"
#define AOSVERBNAME_REMOVE			"remove"


class AosGenericObj : virtual public AosJimo
{
public:
	AosGenericObj();
	AosGenericObj(const int version);
	AosGenericObj(const AosGenericObj &rhs);
	virtual ~AosGenericObj();

	virtual bool execute(
			AosRundata *rdata,
			const OmnString &verb_name,
			const OmnString &obj_name,
			const OmnString &jsonstr,
			const AosJimoProgObjPtr &prog);

	virtual bool createByJql(
			AosRundata *rdata,
			const OmnString &objname, 
			const OmnString &jsonstr,
			const AosJimoProgObjPtr &pog);

	virtual bool runByJql(
			AosRundata *rdata,
			const OmnString &objname, 
			const OmnString &jsonstr);

	virtual bool showByJql(
			AosRundata *rdata,
			const OmnString &obj_name, 
			const OmnString &jsonstr);

	virtual bool stopByJql(
			AosRundata *rdata,
			const OmnString &obj_name, 
			const OmnString &jsonstr);
	
	virtual bool dropByJql(
			AosRundata *rdata,
			const OmnString &obj_name, 
			const OmnString &jsonstr);

	virtual bool feedByJql(
			AosRundata *rdata,
			const OmnString &obj_name,
			const OmnString &jsonstr);

	virtual OmnString getObjType(AosRundata *rdata) = 0;
	
	virtual bool parseJQL(
					AosRundata *rdata, 
					AosJimoParserObj *jimo_parser, 
					AosJimoProgObj *prog,
					bool &parsed, 
					bool dft = false) = 0;
	
	virtual bool runJQL(
					AosRundata *rdata, 
					const AosJimoProgObjPtr &job,
					const OmnString &verb_name)
	{
		OmnNotImplementedYet;
		return false;
	}

	// virtual bool setJimoName(const OmnString &name) = 0;
	// virtual OmnString getJimoName() const = 0;
};
#endif


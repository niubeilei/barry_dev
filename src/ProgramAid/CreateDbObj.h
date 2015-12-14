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
// 3/19/2007: Created by cding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_ProgramAid_CreateDbObj_h
#define Aos_ProgramAid_CreateDbObj_h

#include "ProgramAid/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/DynArray.h"
#include "XmlParser/Ptrs.h"

class OmnFile;

class AosCreateDbObj : virtual public OmnRCObject
{
	OmnDefineRCObject;

private: 
	OmnString	mSrcDirName;
	OmnString	mProjPrefix;
	OmnString	mClassName;
	OmnString	mSrcFileExt;
	OmnString	mConfigFileName;
	OmnString	mTableName;
	bool		mForce;

	OmnDynArray<AosFieldPtr>	mMemData;

public:
	AosCreateDbObj();
	AosCreateDbObj(const OmnString &configFilename); 
	AosCreateDbObj(const OmnString &dirname, 
				   const OmnString &projPrefix,
				   const OmnString &srcFileExt,
				   const OmnString &configFilename, 
				   const bool mForce);
	~AosCreateDbObj();

	bool		generateCode();
	bool		config();
	OmnString	getClassName() const;

private:
	bool 		createOneObject(const OmnXmlItemPtr &def);
	OmnString 	getPrimaryKeyDecl() const;
	OmnString 	getPrimaryKeyParms() const;

	bool		addClassId() const;
	bool		generateSourceFile();
	bool		generateHeaderFile();
	bool		generatePtrFile();
	OmnString	getAdditionHeaderInHeader() const;
	OmnString	createWhereClause() const;
	bool 		createResetFunc(OmnFile &file) const;
	bool	   	createToStringFunc(OmnFile &file) const;
	bool	   	createCloneFunc(OmnFile &file) const;
	bool 		createSerializeFromDb(OmnFile &file) const;
	bool 		createUpdateFunc(OmnFile &file) const;
	bool 		createRemoveFunc(OmnFile &file) const;
	bool 		createRemoveAllFunc(OmnFile &file) const;
	bool 		createExistFunc(OmnFile &file) const;
	bool 		createInsertFunc(OmnFile &file) const;
	bool 		createSerializeFromRecordFunc(OmnFile&) const;
	bool 		createMemDataInitializer(OmnFile &file) const;
};
#endif


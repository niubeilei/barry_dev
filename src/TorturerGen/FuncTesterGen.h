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
// 02/18/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_TorturerGen_FuncTesterGen_h
#define Omn_TorturerGen_FuncTesterGen_h

#include "RVG/Ptrs.h"
#include "TorturerGen/TesterGen.h"
#include "TorturerGen/Ptrs.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/DynArray.h"
#include "XmlParser/Ptrs.h"

class OmnFile;

class AosFuncTesterGen : public AosTesterGen
{
	OmnDefineRCObject;

	struct CheckTableInfo
	{
		OmnString	datatype;
		OmnString	dataArg;
		OmnString	lenArg;

		OmnDynArray<OmnString>	fieldnames;
		OmnDynArray<OmnString>	structnames;
	};

private:
	OmnString						mCode;
	OmnString						mFuncName;
	OmnString						mHeaderFileName;
	OmnString						mModuleId;
	OmnDynArray<AosRVGPtr>			mParms;
	OmnString						mCppIncludes;
	OmnString						mHeaderIncludes;
	bool							mStatus;
	OmnString						mReturnType;
	OmnString						mPreCallProcess;
	OmnString						mPostCallProcess;
	OmnString						mPreAddToTableProc;
	CheckTableInfo					mCheckTableInfo;
	OmnString						mPreCheck;
	OmnString						mPostCheck;
	OmnString						mMemberDataDecl;
	OmnString						mPreActionFunc;
	OmnString						mPostActionFunc;

public:
	AosFuncTesterGen(
				 const OmnString &srcDirName, 
				 const OmnString &testDirName); 
	virtual ~AosFuncTesterGen();

	virtual OmnString getHeaderFileName() const {return mHeaderFileName;}

	static bool	isFuncNameAlreadyUsed(const OmnString &name);

	bool		config(const OmnXmlItemPtr &def); 
	bool		generateCode(const bool force);
	bool		getStatus() const {return mStatus;}
	OmnString	getIncludeStmt() const;

private:
	bool		checkDataType(const OmnString &dataType) const;
	bool		createCppFile(const bool force);
	bool		createHeaderFile(const bool force);
	bool 		createCheckTableFunc(OmnFile &file);
	bool 		parseActionFunc(const OmnXmlItemPtr &def);
	bool 		replaceSymbols(OmnString &str);
};

#endif


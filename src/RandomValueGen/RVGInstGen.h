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
// 02/06/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_RandomValueGen_ParmInstGen_h
#define Omn_RandomValueGen_ParmInstGen_h

#include "Parms/RVG.h"
#include "Parms/Util.h"
#include "aosUtil/Types.h"

class AosRVGInstGen : public AosRVG
{
	OmnDefineRCObject;

protected:
	OmnString								mClassName;
	static OmnDynArray<AosRVGInstGenPtr>	mRegistered;
	static OmnDynArray<AosRVGInstGenPtr>	mInstances;
	AosRVGInstGenPtr						mLastInstGen;
	OmnDynArray<AosRVGPtr>   				mParmGens;
	OmnString								mInstGenClassName;
	OmnString								mInstGenCode;
	OmnString								mCppIncludes;
	OmnString								mHeaderIncludes;
	OmnString								mInstanceGenName;
	bool									mStatus;
	OmnString								mFreeMemory;
	AosRVGInstGenPtr						mInstGen;
	bool									mIsPtrDataType;
	OmnString								mDataTypeWithoutPtr;

public:
	AosRVGInstGen(const AosCliCmdTorturerPtr &cmd, const OmnString &name);
	AosRVGInstGen(const AosRVGInstGen &rhs);
	virtual ~AosRVGInstGen();

	virtual AosRVGInstGenPtr clone() const;
	virtual AosRVGType::E getRVGType() const {return AosRVGType::eInstGen;}
	virtual bool getCrtValue(AosRVGInstGenPtr &value) const;
	virtual bool getCrtValueAsArg(OmnString &value, 
								  OmnString &decl, 
								  const u32 argIndex) const;
	virtual bool newRecordAdded(const AosGenRecordPtr &newRecord) const;
	virtual OmnString getNextValueFuncCall() const;
	virtual bool config(const AosCliCmdTorturerPtr &cmd, 
						const OmnXmlItemPtr &def, 
						const OmnString &cmdTag,
						OmnVList<AosGenTablePtr> &tables,
						const bool codegen);
	virtual bool getDeleteStmt(OmnString &stmt, const u32 argIndex) const;
	virtual OmnString	setMemberDataStmt(const OmnString &inst, 
							const OmnString &arg) const;

	virtual bool nextPtr(const AosGenTablePtr &data,
		                 const AosGenRecordPtr &record,
						 bool &isCorrect,
						 const bool correctOnly,
						 const bool selectFromRecord,
						 AosRVGReturnCode &rcode,
						 OmnString &errmsg); 
	
	virtual bool nextStr(
				OmnString &value,
				const AosGenTablePtr &data, 
				const AosGenRecordPtr &record, 
				bool &isCorrect, 
				const bool correctOnly,
				const bool selectFromRecord,
				AosRVGReturnCode &rcode,
				OmnString &errmsg);

	bool 	parseParm(const OmnXmlItemPtr &def, 
						const OmnString &cmdTag, 
						OmnVList<AosGenTablePtr> &tables,
						const bool codegen);

	bool 	genCode(const OmnString &srcDir, const bool force);
	bool 	createCppFile(const OmnString &srcDir, const bool force);
	bool 	createHeaderFile(const OmnString &srcDir, const bool force);
	bool	getStatus() const {return mStatus;}
	OmnString	getClassName() const {return mClassName;}
	OmnString	getInstanceGenName() const {return mInstanceGenName;}
	void	setInstGen(const AosRVGInstGenPtr &g) {mInstGen = g;}
	AosRVGInstGenPtr getInstGen() const {return mInstGen;}
	void 		copyData(AosRVGInstGen *ptr) const;

	static AosRVGInstGenPtr 	getInstance(const OmnString &name);
	static AosRVGInstGenPtr 	getRegistered(const OmnString &name);
	static bool					registerInstanceGen(
									const AosRVGInstGenPtr &instGen);
	static AosRVGInstGenPtr	createInstanceGen(
									const AosCliCmdTorturerPtr &cmd, 
									const OmnXmlItemPtr &def, 
									const OmnString &cmdTag,
									OmnVList<AosGenTablePtr> &tables, 
									const bool codegen);
	static bool 				genAllCode(const OmnString &dir, const bool force);
	static OmnString			genInstRegisterCode();
	static OmnString 			genInstGenIncludes( 
									const OmnString &srcDir,
						            const OmnString &testDir);

private:
	bool 		isPtrDataType(const OmnString &datatype) const;
//	void		setKeyTypeToParms(const AosRVGKeyType::E k);
};

#endif


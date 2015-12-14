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
#ifndef Omn_RandomValueGen_RVGMac_h
#define Omn_RandomValueGen_RVGMac_h

#include "aosUtil/Types.h"
#include "Parms/RVG.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "Util/IpAddr.h"
#include "Util/ValList.h"
#include "Random/Ptrs.h"

class AosRVGMac : public AosRVG
{
	OmnDefineRCObject;

	enum
	{
		eGenerateNotInTableMac,
		eGenerateInTableMac,
		eGenerateInvalidMac,
		
		eDefaultNotInTableWeight = 30,
		eDefaultInTableWeight = 30,
		eDefaultInvalidWeight = 40,
		
		eDefaultSelectFromTablePct = 50,
		eDefaultSelectFromRecordPct = 50
		
	};

private:
	OmnString	mBadValue;
	AosRandomIntegerPtr	mBadValueSelector;
	int			mSelectFromTablePct;

	int			mNotInTableWeight;
	int			mInTableWeight;
	int			mInvalidMacWeight;

public:
	AosRVGMac(const AosCliCmdTorturerPtr &cmd);
	AosRVGMac(const OmnString &name, 
			   const OmnString &fname, 
			   const AosRVGKeyType keyType,
			   const bool correctOnly, 
			   const OmnString &badValue, 
			   const AosGenTablePtr &table,
			   const OmnDynArray<AosGenTablePtr> &depTables,
			   const OmnDynArray<OmnString> &fieldNames);
	virtual ~AosRVGMac();

	virtual bool nextStr(OmnString &value, 
					const AosGenTablePtr &data, 
					const AosGenRecordPtr &record,
					bool &isGood,
					const bool correctOnly,
					const bool selectFromRecord,
					AosRVGReturnCode &rcode, 
					OmnString &errmsg);
	virtual bool nextInt(
					int &value, 
					const AosGenTablePtr &data, 
					const AosGenRecordPtr &record, 
					bool &isCorrect, 
					const bool correctOnly,
					const bool selectFromRecord,
					AosRVGReturnCode &rcode, 
					OmnString &errmsg);
	virtual bool nextInt(
					u32 &value, 
					const AosGenTablePtr &data, 
					const AosGenRecordPtr &record, 
					bool &isCorrect, 
					const bool correctOnly,
					const bool selectFromRecord,
					AosRVGReturnCode &rcode, 
					OmnString &errmsg);

	static AosRVGMacPtr	createInstance(
								const AosCliCmdTorturerPtr &cmd,
								const OmnXmlItemPtr &def, 
								const OmnString &cmdTag, 
								OmnVList<AosGenTablePtr> &tables);

	virtual bool config(const OmnXmlItemPtr &def, 
					const OmnString &cmdTag, 
					OmnVList<AosGenTablePtr> &tables);
	virtual bool 	newRecordAdded(const AosGenRecordPtr &newRecord) const;
	virtual bool 	recordDeleted(const AosGenRecordPtr &record) const;
	virtual bool 	setRecord(const AosGenRecordPtr &record) const;
	virtual OmnString toString(const u32 indent) const;
	virtual bool 	getCrtValue(int &value) const;
	virtual bool 	getCrtValue(u16 &value) const;
	virtual bool 	getCrtValue(u32 &value) const;
	virtual bool 	getCrtValue(char *&value) const;
	virtual bool 	getCrtValue(OmnString &value) const;
	virtual bool 	getCrtValue(OmnMacAddr &value) const;
	virtual bool 	getCrtValueAsArg(OmnString &value, 
									 OmnString &decl,
									 const u32 argIndex) const;
	virtual OmnString getNextValueFuncCall() const;
	virtual AosRVGType getRVGType() const {return eAosRVGType_Mac;}



	bool	nextExist(OmnString &value, 
					const AosGenTablePtr &data, 
				    const AosGenRecordPtr &record,
				    bool &isGood, 
					const bool correctOnly,
			 	    const bool selectFromRecord,
					AosRVGReturnCode &rcode,
				    OmnString &errmsg);
	bool	nextNew(OmnString &value, 
	                const AosGenTablePtr &data, 
				    const AosGenRecordPtr &record,
			 	    const bool selectFromRecord,
				    bool &isGood, 
					const bool correctOnly,
					AosRVGReturnCode &rcode,
				    OmnString &errmsg);
	bool	nextNotKey(OmnString &value, 
	                const AosGenTablePtr &data, 
				    const AosGenRecordPtr &record,
				    bool &isGood, 
					const bool correctOnly,
					const bool selectFromRecord,
					AosRVGReturnCode &rcode,
				    OmnString &errmsg);
	bool 	nextSubkeyNew(OmnString &value, 
	                const AosGenTablePtr &data, 
					const AosGenRecordPtr &record,
					bool &isGood, 
					const bool correctOnly,
					const bool selectFromRecord,
					AosRVGReturnCode &rcode,
					OmnString &errmsg);
	bool	generateBadMac(OmnString &value,
					const AosGenTablePtr &data, 
					const AosGenRecordPtr &record,
					AosRVGReturnCode &rcode,
					OmnString &errmsg);

private:
	bool			createSelectors();
	OmnString		genValidMac();
	OmnString		genInvalidMac();


};

#endif


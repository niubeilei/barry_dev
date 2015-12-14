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
#ifndef Omn_RandomValueGen_RVGIP_h
#define Omn_RandomValueGen_RVGIP_h

#include "aosUtil/Types.h"
#include "Parms/RVG.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "Util/IpAddr.h"
#include "Util/ValList.h"
#include "Random/Ptrs.h"



class AosRVGIP : public AosRVG
{
	OmnDefineRCObject;

	enum
	{
		eGenerateNotInTableIp,
		eGenerateInTableIp,
		eGenerateNotInRangeIp,
		eGenerateInvalidIp,
		
		eDefaultNotInTableWeight = 25,
		eDefaultInTableWeight = 25,
		eDefaultNotInRangeWeight = 25,
		eDefaultInvalidWeight = 25,
		
		eDefaultSelectFromTablePct = 50,
		eDefaultSelectFromRecordPct = 50
	};

private:
	OmnIpAddr	mAddr;
	OmnIpAddr	mNetmask;
	OmnString	mBadAddr;
	OmnString	mBadValue;
	AosRandomIntegerPtr	mBadValueSelector;
	int			mSelectFromTablePct;
	bool		mUsedAsInteger;
	
	int			mNotInTableWeight;
	int			mInTableWeight;
	int			mNotInRangeWeight;
	int			mInvalidIpWeight;

public:
	AosRVGIP(const AosCliCmdTorturerPtr &cmd);
	AosRVGIP(const OmnString &name, 
			  const OmnString &fieldName, 
			  const AosRVGKeyType keyType, 
			  const OmnIpAddr &addr, 
			  const OmnIpAddr &netmask,
			  const bool correctOnly,
			  const OmnIpAddr &badValue, 
			  const AosGenTablePtr &table,
			  const OmnDynArray<AosGenTablePtr> &depTables,
			  const OmnDynArray<OmnString> &fieldNames);
	virtual ~AosRVGIP();

	virtual bool nextStr(OmnString &value, 
					  const AosGenTablePtr &data, 
					  const AosGenRecordPtr &record,
					  bool &isGood,
					  const bool correctOnly,
					  const bool selectFromRecord,
					  AosRVGReturnCode &errocde, 
					  OmnString &errmsg);

	virtual bool nextInt(
				int &value, 
				const AosGenTablePtr &data, 
				const AosGenRecordPtr &record, 
				bool &isCorrect, 
				const bool correctOnly,
				const bool selectFromRecord,
				AosRVGReturnCode &errocde, 
				OmnString &errmsg);

	virtual bool nextInt(
				u32 &value, 
				const AosGenTablePtr &data, 
				const AosGenRecordPtr &record, 
				bool &isCorrect, 
				const bool correctOnly,
				const bool selectFromRecord,
				AosRVGReturnCode &errocde, 
				OmnString &errmsg);

	static AosRVGIPPtr    createInstance(
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
	virtual bool 	getCrtValue(char *&value) const;
	virtual bool 	getCrtValue(int &value) const;
	virtual bool 	getCrtValue(u16 &value) const;
	virtual bool 	getCrtValue(u32 &value) const;
	virtual bool 	getCrtValue(OmnString &value) const;
	virtual bool 	getCrtValue(OmnIpAddr &value) const;
	virtual bool 	getCrtValueAsArg(OmnString &arg, 
									 OmnString &decl, 
									 const u32) const;
	virtual AosRVGType getRVGType() const {return eAosRVGType_IP;}
	virtual OmnString getNextValueFuncCall() const;


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
				      bool &isGood, 
					  const bool correctOnly,
			 	      const bool selectFromRecord,
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
				      const AosGenRecordPtr &theRecord,
				      bool &isGood, 
					  const bool correctOnly,
					  const bool selectFromRecord, 
					  AosRVGReturnCode &rcode, 
				      OmnString &errmsg);
	bool	generateBadIp(OmnString &value,
					  const AosGenTablePtr &data, 
					  const AosGenRecordPtr &record,
					  AosRVGReturnCode &rcode, 
					  OmnString &errmsg);

private:
	bool			createSelectors();
};
	
#endif


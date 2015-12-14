////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CliConfigCmd.h
// Description:
//   
//
// Modification History:
// 11/01/2006: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_CliTorturer_CliCmdTorturer_h
#define Omn_CliTorturer_CliCmdTorturer_h

#include "aosUtil/Types.h"
#include "CliTorturer/Ptrs.h"
#include "CliTorturer/Util.h"
#include "Tester/TestPkg.h"
#include "Tester/TestMgr.h"
#include "Parms/Util.h"
#include "Parms/ReturnCode.h"
#include "Parms/Ptrs.h"
#include "TorturerConds/Ptrs.h"
#include "TorturerConds/CondData.h"
#include "Util/ValList.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/DynArray.h"
#include "XmlParser/Ptrs.h"

class AosCliCmdTorturer : virtual public OmnRCObject, 
						  public OmnTestPkg
{
public:
	enum
	{
		eMaxArgs = 100, 
		eDefaultCorrectPct = 85,
		eDefaultCmdWeight = 50,
		eDefaultSelectFromTablePct = 80,
		eMaxCmdsPerLogFile = 10000
	};

	enum CmdAction
	{
		eUnknownAction,
		eDoNothing,
		eAddToTable, 
		eDeleteFromTable,
		eDeleteFromSelfAndDepTables,
		eSetVar,
		eClearTable
	};

	struct DelInfo
	{
		AosGenTablePtr					mTable;
		OmnDynArray<OmnString>	mArgs;
	};

protected:
	static OmnFilePtr		mLogFile;
	AosCliTorturerLogType	mLogType;

	OmnDynArray<AosRVGPtr>		mArgs;
	OmnString					mCmdName;
	u32							mCorrectPct;
	CmdAction					mAction;
	bool						mTestOnly;
	AosCmdKeyType::E			mKeyType;
	u32							mWeight;
	bool						mCorrectPctSet;
	u32							mSelectFromTablePct;
	AosGenTablePtr				mTable;
	AosCliTorturerPtr			mCliTorturer;
	OmnDynArray<AosTortCondPtr>	mConds;
	OmnDynArray<OmnString> 		mFields;
	OmnDynArray<DelInfo>			mDelFromOtherTablesEntries;
	OmnDynArray<OmnString>		mVarNames;
	OmnDynArray<OmnString>		mVarValues;
	bool								mStatus;
	AosCondData							mCondData;
	u32									mId;
	bool								mOverrideFlag;
	bool								mCorrectOnly;

private:
	bool operator ==(const AosCliCmdTorturer& another) const;

public:
	AosCliCmdTorturer(const OmnString &name);
//	AosCliCmdTorturer(const OmnString &name, 
//					  const u32 correctPct, 
//					  const CmdAction action,
//					  const AosParmKeyType keytype, 
//					  const u32 weight, 
//					  const AosCliTorturerLogType logType, 
//					  const AosGenTablePtr &table, 
//					  const AosCliTorturerPtr &cliTorturer, 
//					  const OmnDynArray<AosTortCondPtr> &conds);
	AosCliCmdTorturer(const AosCliTorturerLogType logType, 
					  const AosCliTorturerPtr &cliTorturer);
	virtual ~AosCliCmdTorturer();

	virtual bool 	config(const OmnXmlItemPtr &def, 
						const AosCliTorturerPtr &tort); 
	static AosCliCmdTorturerPtr	createCmd(const OmnXmlItemPtr &def, 
					const AosCliTorturerLogType logType, 
					OmnVList<AosGenTablePtr> &tables, 
					AosVarListPtr &variables,
					const AosCliTorturerPtr &cliTorturer);
	virtual bool	doPreAction(bool &rslt, OmnString &errmsg) {return true;}
	virtual bool	doPostAction(bool &rslt, OmnString &errmsg) {return true;}
	virtual bool	run(bool &cmdGood, 
						const bool correctOnly,
						const OmnTestMgrPtr &tm, 
						bool &ignore) = 0;
	virtual bool 	check() = 0;
	virtual bool	preCheck(bool &cmdCorrect);
	virtual bool	postCheck(bool &cmdCorrect);
	virtual bool	addArg(const AosRVGPtr &arg);
	virtual bool	preAddToTableProc(const AosGenTablePtr &table, 
						const AosGenRecordPtr &record,
						bool &cmdCorrect, 
						OmnString &cmdErrmsg);

	static u32			calculateParmCrtPct(const u32 cmdGoodPct, const u32);
	static bool			setLogfile(const OmnFilePtr &file);
	static CmdAction	getAction(const OmnString &name);
	static bool 		cmdFailedStat(const u32 cmdId);
	static bool 		cmdExecutedStat(const u32 cmdId);

	virtual bool	checkCmdConditions(
							bool &cmdGood, 
							AosRVGReturnCode &rcode, 
							OmnString &errmsg, 
							const bool beforeExecution);

	OmnString	getName() const {return mCmdName;}	
	bool		deleteFromDepTables();
	bool		setCmdId();
	bool 		setParmCorrectPct();
	bool 		doesCommandIdentifyRecord(const AosGenTablePtr &table);
	bool		getStatus() const {return mStatus;}
	void		setStatus(const bool s) {mStatus= s;}
	u32			getWeight() const {return mWeight;}
	void		setCorrectOnly(const bool f) {mCorrectOnly = f;}
	void		setDepTables(const OmnDynArray<DelInfo> &tables);
	bool		getGlobalVar(const OmnString &varname, OmnString &varvalue);
	bool		checkVar(const OmnString &name, const OmnString &value);
	bool		checkVar(const OmnString &name, const int value);
	bool		setVar(const OmnString &name, const OmnString &value);
	bool		setVar(const OmnString &name, const int value);
	OmnString	getVar(const OmnString &name);
	// bool		setParm(const OmnString &name, 
	bool		setRVG(const OmnString &name, 
						const OmnString &value, 
						OmnString &errmsg);
	bool		setCrtValue(const OmnString &name, 
						const OmnString &value, 
						OmnString &errmsg);

	bool		logCmd(const OmnString &cmd, 
					const bool, 
					const OmnString &, 
					const u32 cmdId);
	bool		logApi(const OmnString &cmd, 
					const bool, 
					const OmnString &, 
					const u32 cmdId);
	bool		addToTable(const AosGenTablePtr &table,
				   	bool &cmdCorrect, 
				   	OmnString &cmdErrmsg);
	bool		deleteFromTable(const AosGenRecordPtr &record,
					OmnString &cmdErrmsg);
	bool 		getValue(const OmnString &parmName, OmnString &) const;
	OmnString	getParm(const OmnString &name) const;
	bool 		getValue(const OmnString &parmName, 
					OmnString &fieldName,
					OmnString &value, 
					AosRVGType &type) const;
	static bool	parseDepTables(const OmnXmlItemPtr &def, 
						OmnDynArray<DelInfo> &depTables,
						OmnVList<AosGenTablePtr> &tables);
	bool 		setVars(const OmnDynArray<OmnString> &names, 
						const OmnDynArray<OmnString> &values);

	static bool	parseSetvar(const OmnXmlItemPtr &def, 
							OmnDynArray<OmnString> &names, 
						    OmnDynArray<OmnString> &values, 
							AosVarListPtr &varaibles);
	static bool 		cmdStat(const u32 cmdId, const bool success);
	static bool 		init();
	static OmnString 	getStat();
};

#endif


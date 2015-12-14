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
// 2014/02/27 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataJoin_DataJoinUnicomIMEI_h
#define Aos_DataJoin_DataJoinUnicomIMEI_h 

#include "DataJoin/DataJoin.h"
#include "IILEntryMap/IILEntryMapMgr.h"


class AosDataJoinUnicomIMEI : public AosDataJoin
{
	OmnString					mTime;
	OmnString					mLastDay;
	OmnString					mMonth;
	bool						mIsDBB;

	AosIILScannerObjPtr			mIILScanner;

	AosIILEntryMapPtr			mStatusMap;
	AosIILEntryMapPtr			mAllCallMap;
	AosIILEntryMapPtr			mMatchCallMap;
	AosIILEntryMapPtr			m3GCallMap;

	struct USER
	{
		OmnString REGION_NAME;
		OmnString CITY_DESC;
		OmnString USER_ID;
		OmnString PHONE_NO;
		OmnString IMEI_NUMBER;
		OmnString PRODT_CDE;
		OmnString PRODT_DESC;
		OmnString TYPE;
		OmnString CHNL_CDE;
		OmnString CHNL_DESC;
		int		  STATUS_CODE;
		OmnString STATUS;
		OmnString MATCH_CALL;
		OmnString ALL_CALL;
		OmnString _3G_CALL;
	};

	struct SUM
	{
		OmnString REGION_NAME;
		OmnString CITY_DESC;
		OmnString TYPE;
		int		  CNT;
		int		  S1;
		float	  S1P;
		int		  S2;
		float	  S2P;
		int		  S3;
		float	  S3P;
		int		  S4;
		float	  S4P;
		int		  S2DIFF;
		float	  S2DIFFP;
		int		  CLASS;
		int		  MATCH1;
		float	  MATCH1P;

		SUM(){}
		SUM(USER user)
		:
		REGION_NAME(user.REGION_NAME),
		CITY_DESC(user.CITY_DESC),
		TYPE(user.TYPE),
		CNT(1),S1(0),S1P(0.00),S2(0),S2P(0.00),S3(0),S3P(0.00),S4(0),S4P(0.00),
		S2DIFF(0),S2DIFFP(0.00),CLASS(3),MATCH1(0),MATCH1P(0.00)
		{
		}

		SUM & operator += (const SUM &sum)
		{
			CNT += sum.CNT;
			S1 += sum.S1;
			S2 += sum.S2;
			S3 += sum.S3;
			S4 += sum.S4;
			S2DIFF += sum.S2DIFF;
			MATCH1 += sum.MATCH1;
			return *this;
		}
	};

public:
	AosDataJoinUnicomIMEI();
	AosDataJoinUnicomIMEI(
		const AosDataJoinCtlrPtr &ctlr,
		const AosRundataPtr &rdata);
	~AosDataJoinUnicomIMEI();

	// OmnThreadedShellProc Interface
	virtual	bool	run();
	
	int				getProgress();
	bool			setQueryContext(
						const AosQueryContextObjPtr &context,
						const AosRundataPtr &rdata);
private:
	bool			config(
						const AosXmlTagPtr &def,
						const AosRundataPtr &rdata);
	bool			runJoin(const AosRundataPtr &rdata);
	virtual AosDataJoinPtr	create(
						const AosXmlTagPtr &def,
						const AosDataJoinCtlrPtr &ctlr,
						const AosRundataPtr &rdata);
	virtual bool	checkConfig(
						const AosXmlTagPtr &def,
						const AosTaskObjPtr &task,
						const AosRundataPtr &rdata);
	
	bool			getNextKey(
						OmnString &phonenum,
						u64 &docid,
						bool &finish,
						const AosRundataPtr &rdata);
	void			clear();

	bool			getLastDatMap(map<OmnString, int> &last_map, const AosRundataPtr &rdata);
};

#endif


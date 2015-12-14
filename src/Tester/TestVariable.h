////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TestVariable.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef OMN_Tester_TestVariable_h
#define OMN_Tester_TestVariable_h

#include "Util/ValList.h"
#include "Tester/TestVariDataType.h"
#include "Tester/TestVariData.h"
#include "Util/String.h"

class OmnTestVariable :public OmnRCObject
{
public:
	enum ChangeMode				
	{
		eRandomChange,
		eStaticChange
	};

private:
	enum
	{
		eMaxStrLen  = 2000,
		eMaxInt     = 10000000,
		eMinInt		= 0
	};
	
	
	enum eAreaProperty{
		eAll,
		eBool,
		eOnlyMax_NotInclude,
		eOnlyMax_Include,
		eOnlyMin_NotInclude,
		eOnlyMin_Include,
		eMin_NotInclude_Max_NotInclude,
		eMin_Include_Max_NotInclude,
		eMin_NotInclude_Max_Include,
		eMin_Include_Max_Include,
		eIntSingleValue,
		eStrAscii,
		eStrAllZeroCode,
		eStrPrintableChar,
		eStrASCIIZeroTerminated,
		eStrAllMixedChar,
		eStrAllMixedZeroTerminated
	};

	struct DataArea
	{
		int					mMaxData;
		int					mMinData;
		int					mOccurPercent;
		eAreaProperty		mProperty;

		DataArea();
		DataArea(const int max,const int min,const int percent,const eAreaProperty perp);
		DataArea(const bool max,const bool min,const int percent,const eAreaProperty perp);
	};
	
private:
	OmnString				mName;
	eTester_DataType		mType;
	OmnVList<DataArea>		mAreaList;
	DataArea				mDefaultArea;
	
	int						mChangeRate;
	ChangeMode				mChangeMode;
	int						mTimeToChange;
	
public:
	OmnTestVariable(const OmnString &name,
					const eTester_DataType &dataType,
					const int changeRate);

	~OmnTestVariable();


	void				clearAreaList();
	bool				addArea(const int max,
								const int min,
								const int percent,
								const eAreaProperty perp);
	
	bool				addArea(const bool max,
								const bool min,
								const int percent,
								const eAreaProperty perp);
	
	bool				generateRandomData(OmnTestVariData	&data,bool &dataChanged);
	
	void				setChangeMode(const OmnTestVariable::ChangeMode mode){mChangeMode = mode;}
	void				setChangeRate(const int rate){mChangeRate = rate;}
	void				resetTimeToChange(){mTimeToChange = 0;}

private:
	bool				generateRandomInt(OmnTestVariData	&data);
	bool				generateRandomString(OmnTestVariData	&data);
	bool				generateRandomBool(OmnTestVariData	&data);

	bool				getRandomBool(const OmnTestVariable::DataArea &area,OmnTestVariData &data);
	bool				getRandomInt(const OmnTestVariable::DataArea &area,OmnTestVariData &data);
	bool				getRandomString(const OmnTestVariable::DataArea &area,OmnTestVariData &data);
	
};

#endif


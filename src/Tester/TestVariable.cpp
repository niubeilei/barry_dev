////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TestVariable.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Tester/TestVariable.h"

#include "Alarm/Alarm.h" 
#include "Random/RandomUtil.h"
#include "Tester/TestVariDataType.h"

OmnTestVariable::DataArea::DataArea(const int max,const int min,const int percent,const eAreaProperty perp)
:
mOccurPercent(percent),
mProperty(perp)
{
	mMaxData = max;
	mMinData = min;
}

OmnTestVariable::DataArea::DataArea()
:
mMaxData(0),
mMinData(0),
mOccurPercent(0),
mProperty(eAll)
{
}

OmnTestVariable::DataArea::DataArea(const bool max,const bool min,const int percent,const eAreaProperty perp)
:
mOccurPercent(percent),
mProperty(perp)
{
	mMaxData = (int)max;
	mMinData = (int)min;	
}


OmnTestVariable::OmnTestVariable(const OmnString &name,
								 const eTester_DataType &dataType,
								 const int changeRate)
:
mName(name),
mType(dataType),
mChangeRate(changeRate),
mChangeMode(eRandomChange),
mTimeToChange(0)
{
	switch(mType)
	{
		case eTester_DataType_Bool:
			mDefaultArea = DataArea(true,false,100,eAll);
			break;			
		case eTester_DataType_Int:
			mDefaultArea = DataArea(0,0,100,eAll);
			break;			
		case eTester_DataType_String:
			mDefaultArea = DataArea(0,eMaxStrLen,100,eStrAscii);
			break;			
		default:
			OmnAlarm << "No such type: " << mType << enderr;
	}	
	if(changeRate <= 0)
	{
		OmnAlarm << "Error in changeRate: " << changeRate << enderr;
	}
}


OmnTestVariable::~OmnTestVariable()
{
}

void				
OmnTestVariable::clearAreaList()
{
	mAreaList.clear();
}


bool
OmnTestVariable::addArea(const int max,
						 const int min,
						 const int percent,
						 const eAreaProperty perp)
{
	if(percent <= 0)
	{
		return false;
	}
	mAreaList.append(DataArea(max,min,percent,perp));
	return true;
}


bool
OmnTestVariable::addArea(const bool max,
						 const bool min,
						 const int percent,
						 const eAreaProperty perp)
{
	if(percent <= 0)
	{
		return false;
	}
	mAreaList.append(DataArea(max,min,percent,perp));
	return true;
}
							


bool
OmnTestVariable::generateRandomData(OmnTestVariData	&data,bool &dataChanged)
{
	if(eRandomChange == mChangeMode)
	{
		if(1 == OmnRandom::nextInt1(1,mChangeRate))
		{	
			dataChanged = true;		
		}
		else
		{
			dataChanged = false;
			return true;
		}		
	}

	if(eStaticChange == mChangeMode)
	{
		mTimeToChange ++;
		if(mTimeToChange >= mChangeRate)
		{
			mTimeToChange = 0;
			dataChanged = true;			
		}
		else
		{
			dataChanged = false;
			return true;
		}		
	}
	
	switch(mType)
	{
		case eTester_DataType_Bool:
			return generateRandomBool(data);
		case eTester_DataType_Int:
			return generateRandomInt(data);
		case eTester_DataType_String:
			return generateRandomString(data);
		default:
			OmnAlarm << "No such type: " << mType << enderr;
			return false;
	}
	// Should not be here
	OmnAlarm << "Should not be here" << mType << enderr;
	return 	false;
}

bool
OmnTestVariable::generateRandomInt(OmnTestVariData	&data)
{
	data.mType = eTester_DataType_Int;
	data.mChanged = true;
	
	if(!mAreaList.entries())
	{
		return getRandomInt(mDefaultArea,data);
	}

	bool finished = false;
	
	while(finished)
	{
		//1. generate random percent data
		int percentData = OmnRandom::nextInt1(1,100);
		//2. find the area
		int occerPercent = 0;
		
		mAreaList.reset();
		while (mAreaList.hasMore())
		{
			occerPercent += mAreaList.crtValue().mOccurPercent;
			if(occerPercent >= percentData)
			{
				//3. generate data
				finished = true;
				return getRandomInt(mAreaList.crtValue(),data); 
			}	
			mAreaList.next();
		}
	}
	return true;	
}

bool
OmnTestVariable::generateRandomString(OmnTestVariData	&data)
{
	data.mType = eTester_DataType_String;
	data.mChanged = true;
	
	if(!mAreaList.entries())
	{
		return getRandomString(mDefaultArea,data);
	}

	bool finished = false;
	
	while(finished)
	{
		//1. generate random percent data
		int percentData = OmnRandom::nextInt1(1,100);
		//2. find the area
		int occerPercent = 0;
		
		mAreaList.reset();
		while (mAreaList.hasMore())
		{
			occerPercent += mAreaList.crtValue().mOccurPercent;
			if(occerPercent >= percentData)
			{
				//3. generate data
				finished = true;
				return getRandomString(mAreaList.crtValue(),data); 
			}	
			mAreaList.next();
		}
	}
	return true;	
}

bool
OmnTestVariable::generateRandomBool(OmnTestVariData	&data)
{
	data.mType = eTester_DataType_Bool;
	data.mChanged = true;
	
	if(!mAreaList.entries())
	{
		return getRandomBool(mDefaultArea,data);
	}

	bool finished = false;
	
	while(finished)
	{
		//1. generate random percent data
		int percentData = OmnRandom::nextInt1(1,100);
		//2. find the area
		int occerPercent = 0;
		
		mAreaList.reset();
		while (mAreaList.hasMore())
		{
			occerPercent += mAreaList.crtValue().mOccurPercent;
			if(occerPercent >= percentData)
			{
				//3. generate data
				finished = true;
				return getRandomBool(mAreaList.crtValue(),data); 
			}	
			mAreaList.next();
		}
	}
	return true;	
}


bool
OmnTestVariable::getRandomBool(const OmnTestVariable::DataArea &area,OmnTestVariData &data)
{
	if((bool)area.mMaxData == (bool)area.mMinData)
	{
		data.mBoolValue = area.mMaxData;
	}
	else
	{
		data.mBoolValue = OmnRandom::nextBool();
	}
	return true;
}

bool
OmnTestVariable::getRandomInt(const OmnTestVariable::DataArea &area,OmnTestVariData &data)
{
	switch(area.mProperty)
	{
		case eOnlyMax_NotInclude:
			data.mIntValue = OmnRandom::nextInt1(area.mMaxData-1,eMinInt);
			return true;

		case eOnlyMax_Include:
			data.mIntValue = OmnRandom::nextInt1(area.mMaxData,eMinInt);
			return true;

		case eOnlyMin_NotInclude:
			data.mIntValue = OmnRandom::nextInt1(eMaxInt,area.mMinData+1);
			return true;

		case eOnlyMin_Include:
			data.mIntValue = OmnRandom::nextInt1(eMaxInt,area.mMinData);
			return true;

		case eMin_NotInclude_Max_NotInclude:
			data.mIntValue = OmnRandom::nextInt1(area.mMaxData-1,area.mMinData+1);
			return true;

		case eMin_Include_Max_NotInclude:
			data.mIntValue = OmnRandom::nextInt1(area.mMaxData-1,area.mMinData);
			return true;

		case eMin_NotInclude_Max_Include:
			data.mIntValue = OmnRandom::nextInt1(area.mMaxData,area.mMinData+1);
			return true;

		case eMin_Include_Max_Include:
			data.mIntValue = OmnRandom::nextInt1(area.mMaxData,area.mMinData);
			return true;

		case eIntSingleValue:
			data.mIntValue = area.mMaxData;
			return true;
		
		default:
			OmnAlarm << "No such int type:" << area.mProperty << enderr;		
			data.mIntValue = 0;
			return false;
	}
	OmnAlarm << "Should not be here: " << area.mProperty << enderr;		
	data.mIntValue = 0;
	return true;	
}

bool
OmnTestVariable::getRandomString(const OmnTestVariable::DataArea &area,OmnTestVariData &data)
{
	OmnAlarm << "to be done: " << area.mProperty << enderr;		
	return false;
	switch(area.mProperty)
	{
		case eStrAscii:
		case eStrAllZeroCode:
		case eStrPrintableChar:
		case eStrASCIIZeroTerminated:
		case eStrAllMixedChar:
		case eStrAllMixedZeroTerminated:
		default:
			OmnAlarm << "to be done: " << area.mProperty << enderr;		
			return false;
	}
	OmnAlarm << "Should not be here: " << area.mProperty << enderr;		
	return false;
}



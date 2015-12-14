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
// A RandomInteger is a random object that selects integers based 
// on a selection model, which is defined as:
//     [low1, high1], weight1
//     [low2, high2], weight2
//     [low3, high3], weight3
//     ...
//     [lown, highn], weightn
//
// The sum of all the weights should be the same as "base", which can
// be 100, 1000, 10000, or 0. If base is 0, the weights are relative. 
// which means that each weight's percentage is calculated as:
// 		weight[i]/total weights
//
// Otherwise, the object is not constructed correctly. 
//
// The object can be defined by an XML document:
// <Random>
//     <Name>
//     <Type>Selector</Type>
//     <Selector>
//         <Type>RandomInteger</Type>
//     	   <Base>base</Base>
//         <Elements>
//             <Element>value, weight</Element>
//             <Element>[low, high], weight</Element>
//             ...
//             <Element>[low, high], weight</Element>
//         </Elements>
//     </Selector>
// </Random>
//
// Modification History:
// 11/26/2006 Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "Random/RandomInteger.h"

#include "Alarm/Alarm.h"
#include "Random/RandomUtil.h"


AosRandomInteger::AosRandomInteger(const u32 base)
	:
mLows(0), 
mHighs(0), 
mWeights(0),
mNumElems(0)
{
	mBase = base;
	createArrays();
}


bool
AosRandomInteger::createArrays()
{
	if (mBase == 0)
	{
		// 
		// This is a relative base. We will create the arrays later.
		//
		return true;
	}

	if (mLows) OmnDelete [] mLows;
	if (mHighs) OmnDelete [] mHighs;
	if (mWeights) OmnDelete [] mWeights;

	mLows = OmnNew int64_t[mBase];
	mHighs = OmnNew int64_t[mBase];
	mWeights = OmnNew u16[mBase];

	memset(mLows, 0, sizeof(int) * mBase);
	memset(mHighs, 0, sizeof(int) * mBase);
	memset(mWeights, 0, sizeof(u16) * mBase);

	return true;
}


AosRandomInteger::AosRandomInteger(
		int64_t * low, 
		int64_t * high, 
		u16 * weights, 
		const u32 numElems, 
		const u32 base)
:
mLows(0), 
mHighs(0), 
mWeights(0),
mNumElems(0)
{
	mBase = base;
	createArrays();
	mIsGood = set(low, high, weights, numElems);
}


AosRandomInteger::AosRandomInteger(
		const OmnString &name, 
		const OmnString &conf, 
		const u32 base)
:
AosRandomSelector(name),
mLows(0), 
mHighs(0), 
mWeights(0),
mNumElems(0)
{
	mBase = base;
	createArrays();
	OmnXmlItemPtr item = OmnNew OmnXmlItem(conf);
	config(item);
}


AosRandomInteger::AosRandomInteger(
		const OmnString &name, 
		const OmnXmlItemPtr &conf, 
		const u32 base)
:
AosRandomSelector(name),
mLows(0), 
mHighs(0), 
mWeights(0),
mNumElems(0)
{
	// This constructor creates the instance based on an XML configuration:
	// <Random>
	//     <Name>
	//     <Type>Selector</Type>
	//     <Base>base</Base>
	//     <Selector>
	//         <Type>IntegerSelector</Type>
	//         <Base>base</Base>
	//         <Elements>
	//             <Element>value, weight</Element>
	//             <Element>[low, high], weight</Element>
	//             ...
	//             <Element>
	//         </Elements>
	//     </Selector>
	// </Random>
	// 
	// When this function is called, 'conf' is:
	//         <Elements>
	//             <Element>
	//             <Element>
	//             ...
	//             <Element>
	//         </Elements>
	//
	mBase = base;
	createArrays();	
	config(conf);
}


bool
AosRandomInteger::config(const OmnXmlItemPtr &conf)
{
	// When this function is called, 'conf' is:
	//         <Elements>
	//             <Element>
	//             <Element>
	//             ...
	//             <Element>
	//         </Elements>
	//
	
	mIsGood = false;
	int64_t lows[eMaxSelectors];
	int64_t highs[eMaxSelectors];
	u16 weights[eMaxSelectors];
	int64_t low, high, weight;

	int index = 0;
	int total = 0;
	conf->reset();
	while (conf->hasMore() && index < eMaxSelectors)
	{
		low=0;
		high=0;

		OmnXmlItemPtr elem = conf->next();
		if (!elem)
		{
			OmnAlarm << "Program error" << enderr;
			return true;
		}

		// 
		// Contents should be in the form: "[low, high], weight"
		//
		OmnString contents = elem->getStr();
		OmnStrParser1 parser(contents);
		if (parser.peekChar() == '[')
		{
			// 
			// It is a range: "[low, high], weight"
			//
			if (!expect(parser, '[', contents, elem)) return false;
			if (!getInt(parser, low, "low", contents, conf)) return false; 
			if (!expect(parser, ',', contents, elem)) return false;
			if (!getInt(parser, high, "high", contents, elem)) return false;
			if (!checkLowHigh(low, high, elem)) return false;
			if (!expect(parser, ']', contents, elem)) return false;
		}
		else
		{
			// 
			// It is an individual value "value, weight"
			//
			if (!getInt(parser, low, "low", contents, conf)) return false;
			high = low;
		}

		if (!expect(parser, ',', contents, conf)) return false;
		if (!getInt(parser, weight, "weight", contents, conf)) return false;

		lows[index] = low;
		highs[index] = high;
		weights[index] = weight;
		total += weight;
		index++;
	}

	if (!finalCheck(conf, total)) return false;

	mIsGood = set(lows, highs, weights, index);

	return true;
}


// 
// Make sure low <= high. Otherwise, it will raise an alarm
//
bool
AosRandomInteger::checkLowHigh(int low, int high, const OmnXmlItemPtr &conf)
{
	if (low > high)
	{
		OmnAlarm << "low > high. Conf: " << conf->toString() << enderr;
		return false;
	}

	return true;
}


bool
AosRandomInteger::set(
		int64_t *low, 
		int64_t *high, 
		u16 *weights, 
		const u32 numElems) 
{
	// 
	// In our implementation, percentages can be expressed to 1/100, 
	// 1/1000, or 1/10000. This is determined by 'mBase', which can 
	// be either 100, 1000, or 10000. 
	//
	// 'high' can be null. If that is the case, we assume all the 
	// ranges are defined as: (low[i], low[i]). Or in other word, 
	// if 'high' is null, it is assumed the same as 'low'.
	//
	// Make sure the array is not too big
	//
	
	mIsGood = false;
	if (numElems >= eMaxSelectors)
	{
		OmnAlarm << "Too many elements: " << (int)numElems << enderr;
		return false;
	}

	mNumElems = numElems;

	// The weights must add to mBase. If not, it is an error
	u16 sum = 0;
	for (u32 i=0; i<numElems; i++)
	{
		sum += weights[i];
	}

	if (sum > eMaxSelectors)
	{
		OmnAlarm << "Total weights too big: " << sum
			<< ". Maximum allowed: " << eMaxSelectors << enderr;
		return false;
	}

	if (mBase == 0)
	{
		// 
		// It is relative base. We will use the total as the
		// base. 
		//
		mBase = sum;
		createArrays();
		
		/*
		float ss = sum;
		for (u32 i=0; i<numElems; i++)
		{
			float sss = (weights[i] * 1.0) / ss * eWeightBase;
			weights[i] = (u16) (sss + 0.5);
		}

		mBase = eWeightBase;
		*/
	}
	else if (sum != mBase)
	{
		OmnWarn << "The total of the weights is not 100: " << sum 
			<< ". The last weight will be adjusted to make it 100!" << enderr;
		return false;
	}

	// 
	// We will distribute the ranges based on the weights to the 
	// array "mLow" and "mHigh"
	//
	u16 m = 0;
	for (u32 idx=0; idx<numElems; idx++)
	{
		if (m > mBase)
		{
			OmnAlarm << "Index out of bound: " 
				<< (int)m << ". Base: " << mBase << enderr;
			return false;
		}

		u32 w = weights[idx];
		for (u32 j=0; j<w; j++)
		{

			mLows[m] = low[idx]; 
			if (high)
			{
				mHighs[m++] = high[idx]; 
			}
			else
			{
				mHighs[m++] = low[idx];
			}
		}
	}

	// 
	// It is allowed for the last weight to be zero, 
	// which means that it will take whatever left. 
	// The following loop takes care of this case.
	//
	for (u32 i=m; i<mBase; i++)
	{
		mLows[i] = low[numElems-1];
		if (high)
		{
			mHighs[i] = high[numElems-1];
		}
		else
		{
			mHighs[i] = low[numElems-1];
		}
	}

	mIsGood = true;
	return true;
}


AosRandomInteger::~AosRandomInteger()
{
}


int
AosRandomInteger::nextInt(AosGenTable &data)
{
	int v = OmnRandom::nextU32(0, mBase-1);
	return OmnRandom::nextInt1(mLows[v], mHighs[v]);
}

int 		
AosRandomInteger::nextInt()
{
	if (mBase <= 0)
	{
		OmnAlarm << "mBase is invalid: " << mBase << enderr;
		return -1;
	}

	int v = OmnRandom::nextU32(0, mBase-1);
	return OmnRandom::nextInt1(mLows[v], mHighs[v]);
}


int 		
AosRandomInteger::nextInt(const OmnString &name)
{
	int v = OmnRandom::nextU32(0, mBase-1);
	return OmnRandom::nextInt1(mLows[v], mHighs[v]);
}


int 		
AosRandomInteger::nextInt(const OmnString &name, AosGenTable &data)
{
	int v = OmnRandom::nextU32(0, mBase-1);
	return OmnRandom::nextInt1(mLows[v], mHighs[v]);
}


OmnString
AosRandomInteger::nextStr(AosGenTable &data)
{
	OmnAlarm << "Cannot call AosRandomInteger::nextStr()" << enderr;
	return "";
}

OmnString
AosRandomInteger::nextStr()
{
	OmnAlarm << "Cannot call AosRandomInteger::nextStr()" << enderr;
	return "";
}

OmnString
AosRandomInteger::nextStr(const OmnString &name, AosGenTable &data)
{
	OmnAlarm << "Cannot call AosRandomInteger::nextStr()" << enderr;
	return "";
}

OmnString
AosRandomInteger::nextStr(const OmnString &name)
{
	OmnAlarm << "Cannot call AosRandomInteger::nextStr()" << enderr;
	return "";
}

bool
AosRandomInteger::checkWeight(int weight, const OmnXmlItemPtr &conf)
{
	if (weight != 100)
	{
        OmnWarn << "The total of the weights is not 100: " << weight
                << ". The last weight will be adjusted to make it 100!" << enderr;
        return false;
    }
	return true; 
}


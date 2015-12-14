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
//     [string1], weight1
//     [string2], weight2
//     [string3], weight3
//     ...
//     [stringn], weightn
//
// The sum of all the weights should be 100. Otherwise, the object is not
// constructed correctly. 
//
// The object can be defined by an XML document:
// <Random>
//     <Name>
//     <Type>Selector</Type>
//     <Selector>
//         <Type>RandomSelString</Type>
//         <Elements>
//             <Element>string1, weight</Element>
//             <Element>string2, weight</Element>
//             ...
//             <Element>stringn, weight</Element>
//         </Elements>
//     </Selector>
// </Random>
//
// If 'mSeparator' is not 0, this class will generate values in the following
// fashion:
//     <value> <separator> <value> <separator> ...
// where: 
// 		<value> is generated randomly by this class
// 		<separator> is the separator
//
// The number of values and separators are randomly determined. 
//
// Modification History:
// 11/30/2006 Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "Random/RandomSelStr.h"

#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "Random/RandomUtil.h"
#include "Random/RandomInteger.h"


AosRandomSelStr::AosRandomSelStr()
:
mViolateUniqueWeight(eDefaultViolateUniqueWeight),
mViolateOrderedWeight(eDefaultViolateOrderedWeight),
mViolateMinValuesWeight(eDefaultViolateMinValuesWeight),
mViolateMaxValuesWeight(eDefaultViolateMaxValuesWeight),
mIncorrectWeight(eDefaultIncorrectWeight),
mNullStrWeight(eDefaultNullStrWeight),
mNotInListWeight(eDefaultNotInListWeight),
mTotalWeight(0),
mSeparator(0), 
mMin(0),
mMax(0),
mUnique(false),
mOrdered(false),
mIncorrectStrMin(eDefaultIncorrectStrMin),
mIncorrectStrMax(eDefaultIncorrectStrMax),
mCorrectSepPct(eDefaultCorrectSepPct),
mWithTrailingSepPct(eDefaultWithTrailingSepPct),
mInvalidStrType(eAosStrType_VarName),
mBadValue("****BAD****")
{
}


AosRandomSelStr::AosRandomSelStr(const OmnString &values)
:
mViolateUniqueWeight(eDefaultViolateUniqueWeight),
mViolateOrderedWeight(eDefaultViolateOrderedWeight),
mViolateMinValuesWeight(eDefaultViolateMinValuesWeight),
mViolateMaxValuesWeight(eDefaultViolateMaxValuesWeight),
mIncorrectWeight(eDefaultIncorrectWeight),
mNullStrWeight(eDefaultNullStrWeight),
mNotInListWeight(eDefaultNotInListWeight),
mTotalWeight(0),
mSeparator(0),
mMin(0),
mMax(0),
mUnique(false),
mOrdered(false),
mIncorrectStrMin(eDefaultIncorrectStrMin),
mIncorrectStrMax(eDefaultIncorrectStrMax),
mCorrectSepPct(eDefaultCorrectSepPct),
mWithTrailingSepPct(eDefaultWithTrailingSepPct),
mInvalidStrType(eAosStrType_VarName),
mBadValue("****BAD****")
{
	mIsGood = parseValues(values);
}


bool
AosRandomSelStr::parseValues(const OmnString &values)
{
	// 
	// Create the enums
	// values: "[dddd, weight], [dddd, weight], ..., [dddd, weight]"
	//
	OmnStrParser1 parser(values);
	parser.reset();
	OmnString value, weight;
	while (parser.hasMore())
	{
		if (!parser.getValuePair(value, weight, '[', ','))
		{
			OmnAlarm << "Invalid values: " << values << enderr;
			return false;
		}

		if (!weight.isDigitStr())
		{
			OmnAlarm << "Weight is not a digit string: " << values << enderr;
			return false;
		}

		if (value == "")
		{
			OmnAlarm << "Value is null: " << values << enderr;
			return false;
		}

		if (weight == "")
		{
			OmnAlarm << "Weight is null: " << values << enderr;
			return false;
		}

		if (!weight.isDigitStr())
		{
			OmnAlarm << "Weight is not a digit string: " << weight << enderr;
			return false;
		}

		int w = atoi(weight.data());
		if (w < 0)
		{
			OmnAlarm << "Invalid weight: " << weight << enderr;
			return false;
		}

		addValue(value, (u32)w);
	}
	mIsGood = true;
	return true;
}


bool
AosRandomSelStr::addValue(const OmnString &value, u32 weight)
{
	if (weight > eMaxWeight)
	{
		OmnAlarm << "Weight too large: " << weight 
			<< ". Maximum allowed: " << eMaxWeight 
			<< ". Maximum weight is assumed." << enderr;
		weight = eMaxWeight;
	}
	mStrs.append(value);
	int index = mStrs.entries()-1;
	for (u32 i=0; i<weight; i++) mTags.append(index);
	mWeights.append(weight);
	mFlags.append(false);
	mTotalWeight += weight;
	return true;
}
	

AosRandomSelStr::AosRandomSelStr(
		const OmnString &name, 
		const OmnXmlItemPtr &conf)
:
AosRandomSelector(name),
mViolateUniqueWeight(eDefaultViolateUniqueWeight),
mViolateOrderedWeight(eDefaultViolateOrderedWeight),
mViolateMinValuesWeight(eDefaultViolateMinValuesWeight),
mViolateMaxValuesWeight(eDefaultViolateMaxValuesWeight),
mIncorrectWeight(eDefaultIncorrectWeight),
mNullStrWeight(eDefaultNullStrWeight),
mNotInListWeight(eDefaultNotInListWeight),
mTotalWeight(0),
mSeparator(0),
mMin(0),
mMax(0),
mUnique(false),
mOrdered(false),
mIncorrectStrMin(eDefaultIncorrectStrMin),
mIncorrectStrMax(eDefaultIncorrectStrMax),
mCorrectSepPct(eDefaultCorrectSepPct),
mWithTrailingSepPct(eDefaultWithTrailingSepPct),
mInvalidStrType(eAosStrType_VarName)
{
	mIsGood = config(conf);
}


bool
AosRandomSelStr::config(const OmnXmlItemPtr &conf)
{
	//
	//     <Selector>
	//         <Type>StringSelector</Type>
	//         <Values>[value, weight], [value, weight], ...</Values>
	//         <BadValue>
	//         <SeparatorDef>		(Optional)
	//         		<Separator>
	//         		<MinValues>
	//         		<MaxValues>
	//         		<Unique>
	//         		<Ordered>
	//         </SeparatorDef>
	//         <IncorrectGen>		(Optional)
	//         		<StrType>
	//         		<MinLen>
	//         		<MaxLen>
	//         		<BadValue>
	//         </IncorrectGen>
	//     </Selector>
	// 
	mIsGood = false;
	OmnString str;

	mTotalWeight = 0;
	OmnString values = conf->getStr("Values", "");
	if (!parseValues(values))
	{
		OmnAlarm << "Failed to parse values tag: " 
			<< conf->toString() << enderr;
		return false;
	}

	mBadValue = conf->getStr("BadValue", "****BAD****");

	OmnXmlItemPtr sepDef = conf->getItem("SeparatorDef");
	if (!parseSeparator(sepDef))
	{
		OmnAlarm << "Failed to parse SeparatorDef tag: " << conf->toString() << enderr;
		return false;
	}

	OmnXmlItemPtr incorrectGenDef = conf->getItem("IncorrectGen");
	if (!parseIncorrectGen(incorrectGenDef))
	{
		OmnAlarm << "Failed to parse SeparatorDef tag: " << conf->toString() << enderr;
		return false;
	}

	return true;
}


bool
AosRandomSelStr::set(OmnString *strs, u16 * weights, const u32 numElems)
{
	mTotalWeight = 0;
	for (u32 i=0; i<numElems; i++)
	{
		addValue(strs[i], weights[i]);
	}

	mIsGood = true;
	return true;
}


AosRandomSelStr::~AosRandomSelStr()
{
}


// 
// This function selects an index based on the weights
//
int 		
AosRandomSelStr::nextInt(AosGenTable &data)
{
	OmnAlarm << "Cannot call AosRandomSelStr::nextInt()" << enderr;
	return -1;
}

int 		
AosRandomSelStr::nextInt()
{
	OmnAlarm << "Cannot call AosRandomSelStr::nextInt()" << enderr;
	return -1;
}


int 		
AosRandomSelStr::nextInt(const OmnString &name)
{
	OmnAlarm << "Cannot call AosRandomSelStr::nextInt()" << enderr;
	return -1;
}


int 		
AosRandomSelStr::nextInt(const OmnString &name, AosGenTable &data)
{
	OmnAlarm << "Cannot call AosRandomSelStr::nextInt()" << enderr;
	return -1;
}


OmnString
AosRandomSelStr::nextStr(AosGenTable &data)
{
	OmnAlarm << "Not supported yet" << enderr;
	return mBadValue;
}


OmnString
AosRandomSelStr::nextStr()
{
	if (mSeparator)
	{
		return nextStrWithSeparator();
	}

	return nextStrPriv();
}


OmnString
AosRandomSelStr::nextStrPriv()
{
	int v = OmnRandom::nextU32(0, mTags.entries()-1);
	return mStrs[mTags[v]];
}


OmnString
AosRandomSelStr::nextStr(const OmnString &name, AosGenTable &data)
{
	return nextStr();
}

OmnString
AosRandomSelStr::nextStr(const OmnString &name)
{
	return nextStr();
}


bool
AosRandomSelStr::setSeparator(const char sep, const u32 min, const u32 max)
{
	mSeparator = sep;
	mMin = min;
	mMax = max;
	return true;
}


OmnString
AosRandomSelStr::nextStrWithSeparator()
{
	// 
	// This function will generate a string:
	// 	<value><separator><value><separator>...
	//
	int numRepeats = OmnRandom::nextInt1(mMin, mMax);
	OmnString str; 

	for (int i=0; i<mFlags.entries(); i++)
	{
		mFlags[i] = false;
	}

	if (mUnique || mOrdered)
	{
		// 
		// To ensure the uniqueness of the selection, 
		// we will construct the tag array for each 
		// iteration so that if a value has already
		// been used, its weight will be set to 0.
		//
		OmnDynArray<u32, 100, 100, 10000> weights;
		for (int i=0; i<mStrs.entries(); i++)
		{
			weights.append(mWeights[i]);
		}

		bool first = true;
		for (int i=0; i<numRepeats; i++)
		{
			//
			// Create the tag array now
			//
			OmnDynArray<u32, 100, 100, 10000> tags;
			for (int k=0; k<weights.entries(); k++)
			{
				for (u32 m=0; m<weights[k]; m++)
				{
					tags.append(k);
				}
			}

			if (tags.entries() == 0)
			{
				// 
				// Can't select any more element. 
				//
				return str;
			}

			int selection = OmnRandom::nextU32(0, tags.entries()?tags.entries()-1:0);
			int index = tags[selection];

			if (!first)
			{
				str << mSeparator;
			}
			else
			{
				first = false;
			}
			str << mStrs[index];

			weights[index] = 0;
			if (mOrdered)
			{
				for (int i=0; i<index; i++)
				{
					weights[i] = 0;
				}
			}
		}
	}
	else
	{
		str << nextStrPriv();
		for (int i=1; i<numRepeats; i++)
		{
			str << mSeparator << nextStrPriv();
		}
	}

	return str;
}


bool
AosRandomSelStr::createSelector()
{
	if (!mUnique) mViolateUniqueWeight = 0;
	if (!mOrdered || mStrs.entries() <= 1) mViolateOrderedWeight = 0;
	if (mMin <= 1) mViolateMinValuesWeight = 0;
	if (!mSeparator)
	{
		mViolateUniqueWeight = 0;
		mViolateOrderedWeight = 0;
		mViolateMinValuesWeight = 0;
		mViolateMaxValuesWeight = 0;
	}

	const int size = 7;
	int64_t index[size];
	u16 weights[size];
	int idx = 0;
	index[idx] = eViolateUnique;    weights[idx++] = mViolateUniqueWeight;
	index[idx] = eViolateOrdered;   weights[idx++] = mViolateOrderedWeight;
	index[idx] = eViolateMinValues; weights[idx++] = mViolateMinValuesWeight;
	index[idx] = eViolateMaxValues; weights[idx++] = mViolateMaxValuesWeight;
	index[idx] = eIncorrect;       	weights[idx++] = mIncorrectWeight;
	index[idx] = eNullStr;       	weights[idx++] = mNullStrWeight;
	index[idx] = eNotInList;       	weights[idx++] = mNotInListWeight;

    mBadValueSelector = OmnNew AosRandomInteger(
	        index,
			0,
			weights,
			idx,
			0);
	aos_assert_r(mBadValueSelector, false);

	return true;
}

		
OmnString 
AosRandomSelStr::nextIncorrectStr(AosRVGReturnCode &rcode, 
					OmnString &errmsg)
{
	// 
	// This function will randomly generate a string that is 
	// guaranteed incorrect.
	//
	// 1. If mUnique is true, it generate a string that
	//    repeats some of the values.
	// 2. If mOrdered is true, it generate a string that
	//    is not ordered. 
	// 3. If mMin > 1, it generate a string that contains
	//    less than mMin. 
	// 4. A string that contains more than mMax values.
	//
	// Note that the above are the strings whose values
	// are from the list but just violate other requirements.
	//
	// 5. Strings that contain incorrect values.
	// 6. Null string.
	//
	if (!mBadValueSelector) createSelector();

	int selection = mBadValueSelector->nextInt();
	OmnString str;
	switch (selection)
	{
	case eViolateUnique:
		 aos_assert_r(mUnique, mBadValue);
		 mUnique = false;
		 str = nextStr();
		 rcode = eAosRVGRc_NotUnique;
		 (errmsg = mName) << " values repeated. ";
		 makeNotUnique(str, rcode, errmsg);
		 mUnique = true;
		 return str;

	case eViolateOrdered:
	     aos_assert_r(mOrdered, mBadValue);
		 mOrdered = false;
		 str = nextStr();
		 rcode = eAosRVGRc_OutOfOrder;
		 (errmsg = mName) << " values out of order. ";
		 makeNotOrdered(str, rcode, errmsg);
		 mOrdered = true;
		 return str;

	case eViolateMinValues:
	     aos_assert_r(mMin > 1, mBadValue);
		 {
		 	u32 oldmin = mMin;
			u32 oldmax = mMax;
			mMin = 1;
			mMax = oldmin-1;
			str = nextStr();
			mMin = oldmin;
			mMax = oldmax;
			rcode = eAosRVGRc_TooFewValues;
			(errmsg = mName) << " too few values. ";
			return str;
		 }

	case eViolateMaxValues:
		 {
		 	u32 oldmin = mMin;
			u32 oldmax = mMax;
			mMin = oldmax+1;
			mMax = mMin * 3;
			str = nextStr();
			mMin = oldmin;
			mMax = oldmax;
			rcode = eAosRVGRc_TooManyValues;
			(errmsg = mName) << " too many values. ";

			// 
			// Note that it may or may not generate a string
			// with minimum mMax+1. Need to check
			//
			makeStrHaveEnoughSegs(str, mMax+1);
			return str;
		 }

	case eIncorrect:
		 str = OmnRandom::strByType(
				 mInvalidStrType, mIncorrectStrMin, mIncorrectStrMax);
		 rcode = eAosRVGRc_Incorrect;
		 (errmsg = mName) << " from IncorrectGen. ";
		 makeSureValueIsInvalid(str, rcode, errmsg);
		 return str;

	case eNullStr:
		 rcode = eAosRVGRc_NullValue;
		 (errmsg = mName) << " missing. ";
		 return "";

	case eNotInList:
		 rcode = eAosRVGRc_NotInList; 
		 (errmsg = mName) << " not in the enum list. ";
		 return notInList(rcode, errmsg);

	default:
		 OmnAlarm << "Unrecognized selection: " << selection << enderr;
		 rcode = eAosRVGRc_ProgramError;
		 (errmsg = mName) << " unrecognized error condition. ";
		 return mBadValue;
	}

	(errmsg = "Program Error: ") << __FILE__ << ":" << __LINE__;
	OmnAlarm << errmsg << enderr;
	return mBadValue;
}


OmnString
AosRandomSelStr::notInList(AosRVGReturnCode &rcode, OmnString &errmsg)
{
	// 
	// This function will generate a string that is guaranteed 
	// not one of the enum values defined in this class.
	//
	OmnString str = OmnRandom::strByType(mInvalidStrType, 
				mIncorrectStrMin, mIncorrectStrMax);

	u32 tries = 10;
	bool found = true;
	while (tries-- && found)
	{
		found = false;
		for (int i=0; i<mStrs.entries(); i++)
		{
			if (mStrs[i] == str)
			{
				found = true;
				break;
			}
		}

		if (!found) 
		{
			return str;
		}

		// 
		// The string is in the list. Try it again
		//
		str = OmnRandom::strByType(mInvalidStrType, 
				mIncorrectStrMin, mIncorrectStrMax);
	}

	// 
	// We have tried enough times but the value is still
	// in the list. This should rarely happen.
	//
	rcode = eAosRVGRc_Incorrect;
	(errmsg = mName) << " incorrect2. ";
	return mBadValue;
}


bool
AosRandomSelStr::parseSeparator(const OmnXmlItemPtr &sepDef)
{
	//
	//         <SeparatorDef>		(Optional)
	//         		<Separator>
	//         		<MinValues>
	//         		<MaxValues>
	//         		<Unique>
	//         		<Ordered>
	//         </SeparatorDef>
	//
	if (!sepDef) return true;

	mSeparator = sepDef->getChar("Separator", '|');
	if (!mSeparator)
	{
		OmnAlarm << "Missing separator tag: " << sepDef->toString() << enderr;
		return false;
	}

	mMin = sepDef->getInt("MinValues", eDefaultMinValues);
	mMax = sepDef->getInt("MaxValues", eDefaultMaxValues);
	mUnique = sepDef->getBool("Unique", false);
	mOrdered = sepDef->getBool("Ordered", false);

	if (mMax < mMin)
	{
		OmnAlarm << "Max < Min: " << mMin << ":" << mMax << enderr;
		return false;
	}

	if (mMax == 0)
	{
		OmnAlarm << "Max is zero" << enderr;
		return false;
	}

	// 
	// If the unique is set, the maximum should not be more than
	// the total values defined in mStrs.
	//
	if (mUnique && mMax > (u32)mStrs.entries()) mMax = mStrs.entries();

	return true;
}


bool
AosRandomSelStr::parseIncorrectGen(const OmnXmlItemPtr &def)
{
	//
	// <IncorrectGen>		(Optional)
	//     <StrType>
	//     <MinLen>
	//     <MaxLen>
	//     <BadValue>
	// </IncorrectGen>
	//
	if (!def) return true;

	mInvalidStrType = AosStrType_strToEnum(def->getStr("StrType", ""));
	if (mInvalidStrType == eAosStrType_Invalid)
	{
		OmnAlarm << "Invalid string type: " << def->toString() << enderr;
		return false;
	}

	mIncorrectStrMin = def->getInt("MinLen", eDefaultIncorrectStrMin);
	mIncorrectStrMax = def->getInt("MaxLen", eDefaultIncorrectStrMax);
	mBadValue = def->getStr("BadValue", "***BAD***");

	return true;
}


bool
AosRandomSelStr::makeNotUnique(OmnString &str, 
					AosRVGReturnCode &rcode, 
					OmnString &errmsg)
{
	// 
	// 'str' is a string that contains one or more value
	// values. We need to make sure 'str' contains at 
	// least one repeat value. 
	//
	OmnDynArray<OmnString, 100, 100, 10000> values;
	OmnString sep;
	sep << mSeparator;
	OmnStrParser1 parser(str, sep.data(), false, false);
	parser.reset();
	while (parser.hasMore())
	{
		OmnString word = parser.nextWord("");
		for (int i=0; i<values.entries(); i++)
		{
			if (values[i] == word)
			{
				return true;
			}
		}

		// 
		// this is a new value. Add it.
		//
		values.append(word);
	}

	// 
	// This means 'str' does not contain repeated values. 
	// Need to randomly pick one and add it.
	//
	if (values.entries() == 0)
	{
		// 
		// this is incorrect. There should be at least one value.
		//
		OmnAlarm << "Found no valid value: " << str << enderr;
		str = mBadValue;
		rcode = eAosRVGRc_ProgramError;
		errmsg << __FILE__ << ":" << __LINE__ 
			<< ": Found no valid value: " << str;
		return true;
	}
	
	// 
	// Add a randomly duplicated value to the string.
	//
	int index = OmnRandom::nextU32(0, values.entries()-1);
	str << mSeparator << values[index];
	return true;
}


bool
AosRandomSelStr::makeNotOrdered(
		OmnString &str, 
		AosRVGReturnCode &rcode,
		OmnString &errmsg)
{
	// 
	// 'str' contains a number of valid values, separated by
	// the mSeparator. This function makes sure that the
	// values are not ordered. 
	//
	OmnDynArray<OmnString, 100, 100, 10000> values;
	OmnString sep;
	sep << mSeparator;
	OmnStrParser1 parser(str, sep.data(), false, false);
	parser.reset();
	while (parser.hasMore())
	{
		OmnString word = parser.nextWord("");
		values.append(word);
	}

	if (values.entries() == 0)
	{
		// 
		// This is a program error. 
		//
		errmsg << __FILE__ << ":" << __LINE__
			<< ": Found no values: " << str;
		rcode = eAosRVGRc_ProgramError;
		OmnAlarm << "Found no values: " << str << enderr;
		str = mBadValue;
		return true;
	}

	int index = -1;
	for (int i=0; i<values.entries(); i++)
	{
		bool found = false;
		for (int k=0; k<mStrs.entries(); k++)
		{
			if (mStrs[k] == values[i])
			{
				found = true;
				if (index < 0)
				{
					index = i;
				}
				else
				{
					if (i < index)
					{
						// 
						// It is out of order. 
						//
						return true;
					}

					index = i;
				}
				break;
			}
		}

		if (!found)
		{
			// 
			// This is a program error. It means it did not find
			// the value from mStrs.
			//
			errmsg << __FILE__ << ":" << __LINE__ 
				<< ": Did not find the value: " << values[i] 
				<< " in " << str;
			OmnAlarm << "Did not find the value" << values[i]
				<< " in " << str << enderr;
			rcode = eAosRVGRc_ProgramError;
			str = mBadValue;
			return true;
		}
	}

	// 
	// This means the values in 'str' are in order. 
	//
	int index1 = OmnRandom::nextU32(mStrs.entries()/2, mStrs.entries()-1);
	int index2 = OmnRandom::nextU32(0, index1-1);
	str << mSeparator << mStrs[index1] << mSeparator << mStrs[index2];
	return true;
}


bool
AosRandomSelStr::makeStrHaveEnoughSegs(OmnString &str, const u32 min)
{
	// 
	// 'str' contains one or more values. This function makes
	// sure it contains at least 'min' number of values.
	//
	OmnDynArray<OmnString, 100, 100, 10000> values;
	OmnString sep;
	sep << mSeparator;
	OmnStrParser1 parser(str, sep.data(), false, false);
	parser.reset();
	while (parser.hasMore())
	{
		OmnString word = parser.nextWord("");
		values.append(word);
	}

	if (values.entries() >= (int)min) return true;

	for (u32 i=0; i<min-values.entries(); i++)
	{
		int index = OmnRandom::nextU32(0, mStrs.entries()-1);
		if (str != "")
		{
			str << mSeparator;
		}
		str << mStrs[index];
	}

	return true;
}


bool
AosRandomSelStr::makeSureValueIsInvalid(
					OmnString &str, 
					AosRVGReturnCode &rcode, 
					OmnString &errmsg)
{
	// 
	// 'str' may or may not be correct. This function is to 
	// make sure it is incorrect.
	//
	if (str == "")
	{
		return true;
	}

	OmnDynArray<OmnString, 100, 100, 10000> values;
	OmnString sep;
	sep << mSeparator;
	OmnStrParser1 parser(str, sep.data(), false, false);
	parser.reset();
	while (parser.hasMore())
	{
		OmnString word = parser.nextWord("");
		values.append(word);
	}

	for (int i=0; i<values.entries(); i++)
	{
		bool correct = false;
		for (int k=0; k<mStrs.entries(); k++)
		{	
			if (values[i] == mStrs[k])
			{
				correct = true;
				break;
			}
		}

		if (!correct) return true;
	}

	// 
	// This means it is a correct one. 
	//
	str << mSeparator << mBadValue;
	return true;
}

	

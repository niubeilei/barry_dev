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
// Modification History:
// Monday, December 03, 2007: Created by Allen Xu 
//
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_RVG_RIG_H
#define Aos_RVG_RIG_H

#include "RVG/RVG.h"

//
// T can be int8, int16, int32, int64, u8, u16, u32, u64
//
template <class T>
struct IntegerPair
{
	T	 mLow;
	T 	mHigh;
	u32 mWeight;	
};

template <class T>
class AosRIG : public AosRVG
{
	OmnDefineRCObject;
public:
	typedef struct IntegerPair<T>	AosIntPair;
	typedef std::vector<AosIntPair>	AosIntPairList;
	typedef std::vector<u32>		AosWeightHash;
	//
	// a functor to find IntPair, used by find/remove algorithm
	//
	typedef struct FunctorFindIntPair: public std::binary_function<AosIntPair, AosIntPair, bool>
	{
		bool operator()(const AosIntPair& intPair1, const AosIntPair& intPair2) const
		{
			return (intPair1.mLow == intPair2.mLow && intPair1.mHigh == intPair2.mHigh);
		}
		
	} FunctorFindIntPair;

protected:
	AosIntPairList	 	mIntPairList;
	u32					mTotalWeight;

	std::vector<u32>	mWeightHash;

public:
	AosRIG();
	virtual ~AosRIG();
	virtual bool nextValue(AosValue& value);
	virtual bool serialize(TiXmlNode& node);
	virtual bool deserialize(TiXmlNode* node);
	virtual bool createIncorrectRVGs(AosRVGPtrArray& rvgList);

	//
	// get all integer pair list
	//
	void getIntegerPair(AosIntPairList& pairList);

	//
	// add an integer pair, if exist, modify the weight
	//
	void setIntegerPair(T low, T high, u32 weight);

	bool removeIntegerPair(T low, T high);
	void clearIntegerPair();

protected:
	u32 getTotalWeight();
	void reconstructWeightHash();

};

typedef OmnSPtr< AosRIG<u8> > AosU8RIGPtr;
typedef OmnSPtr< AosRIG<u16> > AosU16RIGPtr;
typedef OmnSPtr< AosRIG<u32> > AosU32RIGPtr;
typedef OmnSPtr< AosRIG<u64> > AosU64RIGPtr;
typedef OmnSPtr< AosRIG<int8_t> > AosInt8RIGPtr;
typedef OmnSPtr< AosRIG<int16_t> > AosInt16RIGPtr;
typedef OmnSPtr< AosRIG<int32_t> > AosInt32RIGPtr;
typedef OmnSPtr< AosRIG<int64_t> > AosInt64RIGPtr;


//
// Now the compiler can not support seperating the declare and implementation of template class
// So put the implementation in the header file
//
template <class T>
AosRIG<T>::AosRIG()
:mTotalWeight(0)
{
	if (typeid(T) == typeid(u8))
	{
		mValueType = AosValueType::eU8;
		mRVGType = AosRVGType::eU8RIG;
	}
	if (typeid(T) == typeid(u16))
	{
		mValueType = AosValueType::eU16;
		mRVGType = AosRVGType::eU16RIG;
	}
	if (typeid(T) == typeid(u32))
	{
		mValueType = AosValueType::eU32;
		mRVGType = AosRVGType::eU32RIG;
	}
	if (typeid(T) == typeid(u64))
	{
		mValueType = AosValueType::eU64;
		mRVGType = AosRVGType::eU64RIG;
	}
	if (typeid(T) == typeid(int8_t))
	{
		mValueType = AosValueType::eInt8;
		mRVGType = AosRVGType::eInt8RIG;
	}
	if (typeid(T) == typeid(int16_t))
	{
		mValueType = AosValueType::eInt16;
		mRVGType = AosRVGType::eInt16RIG;
	}
	if (typeid(T) == typeid(int32_t))
	{
		mValueType = AosValueType::eInt32;
		mRVGType = AosRVGType::eInt32RIG;
	}
	if (typeid(T) == typeid(int64_t))
	{
		mValueType = AosValueType::eInt64;
		mRVGType = AosRVGType::eInt64RIG;
	}
}


template <class T>
AosRIG<T>::~AosRIG()
{
}


// 
// We may use a dynamic array to achieve this.
// Given an RIG:
//     [min1, max1, weight1], 
//     [min2, max2, weight2], 
//     ..., 
//     [minn, maxn, weightn]
//
// Construct an array A[total_weight] as:
//     A[0...weight1-1] = 0;
//     A[weight1...weight1+weight2-1] = 1;
//     A[weight1+weight2...wieght1+weight2+weight3-1] = 2;
//     ...
// the array data member is mWeightHash.
// the value in the array is the index of the integer pair in the pair list
// when we select one, we can get value between the integer pair [low, high]
// when the integerPairList changes, we will reconstruct the weight hash table.
//
template <class T>
bool 
AosRIG<T>::nextValue(AosValue& value)
{
	//
	// selected an integer pair, randomly get value between the pair
	//
	if (mTotalWeight <= 0)
	{
		OmnAlarm << "The total weight is 0" << enderr;
		return false;
	}
	u32 weight = aos_next_integer<u32>(0, mTotalWeight-1);
	u32 index = mWeightHash.at(weight);
	T intValue = aos_next_integer<T>(mIntPairList.at(index).mLow, mIntPairList.at(index).mHigh);
	value.setType(mValueType);
	value.setValue((void*)&intValue, sizeof(T));
	return true;
}


template <class T>
bool 
AosRIG<T>::createIncorrectRVGs(AosRVGPtrArray& rvgList)
{
	return true;
}


template <class T>
bool 
AosRIG<T>::serialize(TiXmlNode& node)
{
//
// serialize into a XML node 
// 	<U8RIG> // U16RIG/U32RIG/U64RIG/Int8RIG/Int16RIG/Int32RIG/Int64RIG
// 		<Name>a_global_unique_name</Name>
//		<ComposeWeight>100</ComposeWeight>
//		<ValueList>
//			<Value>
//				<Low>1</Low>
//				<High>10</High>
//				<Weight>100</Weight>
//			</Value>
//			<Value>
//				<Low>11</Low>
//				<High>20</High>
//				<Weight>100</Weight>
//			</Value>
//			...
//			<Value>
//				<Low>100</Low>
//				<High>200</High>
//				<Weight>100</Weight>
//			</Value>
//		</ValueList>
// 	</U8RIG>
//
	if (! AosRVG::serialize(node))
	{
		return false;
	}
	//
	// add ValueList
	//
	TiXmlElement valueList("ValueList");
	typename AosIntPairList::iterator iter;
	for (iter=mIntPairList.begin(); iter!=mIntPairList.end(); iter++)
	{
		TiXmlElement value("Value");
		value.addElement("Low", AosValue::itoa(iter->mLow));
		value.addElement("High", AosValue::itoa(iter->mHigh));
		value.addElement("Weight", AosValue::itoa(iter->mWeight));
		valueList.InsertEndChild(value);
	}
	node.InsertEndChild(valueList);
	return true;
}


template <class T>
bool 
AosRIG<T>::deserialize(TiXmlNode* node)
{
//
// parse a XML node 
// 	<U8RIG> // U16RIG/U32RIG/U64RIG/Int8RIG/Int16RIG/Int32RIG/Int64RIG
// 		<Name>a_global_unique_name</Name>
// 		<ValueType>u8/u16/u32/u64/int8_t/int16_t/int32_t/int64_t</ValueType>
//		<ComposeWeight>100</ComposeWeight>
//		<ValueList>
//			<Value>
//				<Low>1</Low>
//				<High>10</High>
//				<Weight>100</Weight>
//			</Value>
//			<Value>
//				<Low>11</Low>
//				<High>20</High>
//				<Weight>100</Weight>
//			</Value>
//			...
//			<Value>
//				<Low>100</Low>
//				<High>200</High>
//				<Weight>100</Weight>
//			</Value>
//		</ValueList>
// 	</U8RIG>
//
	if (! AosRVG::deserialize(node))
	{
		return false;
	}
	//
	// parse value list
	//
	TiXmlHandle docHandle(node);
	TiXmlElement* element = docHandle.FirstChild("ValueList").ToElement();
	if (element)
	{
		T low;
		T high;
		u32 weight;	
		TiXmlElement* child = element->FirstChildElement("Value");	
		for (; child!=NULL; child=child->NextSiblingElement("Value"))
		{
			TiXmlElement* tmp = child->FirstChildElement("Low");
			if (tmp)
			{
				low = atoi((tmp->GetText()) ? tmp->GetText() : "0");	
				tmp = child->FirstChildElement("High");
				if (tmp)
				{
					high = atoi((tmp->GetText()) ? tmp->GetText() : "0");	
					tmp = child->FirstChildElement("Weight");
					if (tmp)
					{
						weight = atoi(tmp->GetText() ? tmp->GetText() : "0");
						setIntegerPair(low, high, weight);
					}
				}
			}
		}
	}
	
	return true;
}


//
// get all integer pair list
//
template <class T>
void
AosRIG<T>::getIntegerPair(AosIntPairList& pairList)
{
	pairList = mIntPairList;
}


//
// add an integer pair, if exist, modify the weight
//
template <class T>
void 
AosRIG<T>::setIntegerPair(T low, T high, u32 weight)
{
	AosIntPair intPair;
	intPair.mLow = low;
	intPair.mHigh = high;
	intPair.mWeight = weight;

	typename AosIntPairList::iterator iter;
	iter = std::find_if(mIntPairList.begin(),
					    mIntPairList.end(),
					    bind2nd(FunctorFindIntPair(), intPair));
	if (iter != mIntPairList.end())
	{
		iter->mWeight = weight;
	}
	else
	{
		mIntPairList.push_back(intPair);
	}
	getTotalWeight();
	reconstructWeightHash();
}


template <class T>
bool 
AosRIG<T>::removeIntegerPair(T low, T high)
{
	AosIntPair intPair;
	intPair.mLow = low;
	intPair.mHigh = high;

	typename AosIntPairList::iterator iter;
	iter = std::find_if(mIntPairList.begin(),
						mIntPairList.end(),
					    bind2nd(FunctorFindIntPair(), intPair));
	if (iter != mIntPairList.end())
	{
		//mIntPairList.remove_if(bind2nd(FunctorFindIntPair(), intPair));
		mIntPairList.erase(iter);
		getTotalWeight();
		reconstructWeightHash();
		return true;
	}

	return false;
}


template <class T>
void 
AosRIG<T>::clearIntegerPair()
{
	mIntPairList.clear();
}


template <class T>
u32 
AosRIG<T>::getTotalWeight()
{
	mTotalWeight = 0;
	typename AosIntPairList::iterator iter;
	for (iter=mIntPairList.begin(); iter!=mIntPairList.end(); iter++)
	{
		mTotalWeight += iter->mWeight;
	}
	return mTotalWeight;
}


template <class T>
void 
AosRIG<T>::reconstructWeightHash()
{
	//
	// clear all the old value, and reserve the memory
	//
	mWeightHash.clear();
	mWeightHash.reserve(mTotalWeight);

	u32 pairNum = mIntPairList.size();
	for (u32 index=0; index<pairNum; index++)
	{
		for (u32 i=0; i<mIntPairList[index].mWeight; i++)	
		{
			mWeightHash.push_back(index);
		}
	}
}

#endif


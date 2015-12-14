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
// The base RVGs composed can be all kinds of RVGs, if the composed RVG is
// a RIG, it will change to integer string in the generated string. If
// the composed RVG is a RFG, it will be a float string in the generated
// string.
//   
//
// Modification History:
// 11/21/2007: Created by Allen Xu
//
////////////////////////////////////////////////////////////////////////////
#include "ComposeRSG.h"


// 
// Chen Ding
// From now on, try to add some comments at the beginning of each .cpp file
// to explain what the main ideas are about the class we are creating. This 
// forces you to write. Do not worry too much about the language. The more
// you write, the better the language will be. 
//
// Chen Ding
// We may need to consider changing the class relationship 
// a little bit:
//                 AosComposedRSG
//                       |
//          |-------------------------|
//      AosAndRSG                  AOSOrRSG
//
// where "AosComposedRSG" is derived from "AosRSG". 
//
// Chen Ding
// Another comment is about AosOrRSG. The question is how its
// members are "ORed". In your implementation, you randomly
// select one. This is one, but only one possible way. 
// We may need to consider other possible ways, too. In that
// case, we may need to consider deriving more classes under
// "AosOrRSG". 
//
// As an example, we may use RSG to generate SIP messages. SIP
// messages contain a number of optional headers. Each header
// may be implemented by an RSG. Some headers are independent of
// any others, while some headers may be conditional based on 
// some other headers. In order to achieve this, we need a more
// 'intelligent' mechanism. Don't worry too much about it now. 
// I will think over this. You may think over this, too.
//

AosComposeRSG::AosComposeRSG()
{
	
}


AosComposeRSG::~AosComposeRSG()
{
	//
	// delete base RSG Pointor list, since it is a smart pointer list
	// it will delete by itself
	//
}


bool 
AosComposeRSG::serialize(TiXmlNode& node)
{
//
//		<RVGList>...</RVGList>
//
	if (! AosRVG::serialize(node))
	{
		return false;
	}
	//
	// add RVGList
	//
	TiXmlElement rvgList("RVGList");
	for (AosRVGPtrList::iterator iter=mBRVGList.begin(); iter!=mBRVGList.end(); iter++)
	{
		TiXmlElement value("");
		(*iter)->serialize(value);
		rvgList.InsertEndChild(value);
	}
	node.InsertEndChild(rvgList);
	return true;
}


bool 
AosComposeRSG::deserialize(TiXmlNode* node)
{
//
//		<RVGList>...</RVGList>
//
	if (! AosRVG::deserialize(node))
	{
		return false;
	}
	//
	// parse RVGList
	//
	TiXmlHandle docHandle(node);
	TiXmlElement* element = docHandle.FirstChild("RVGList").ToElement();
	if (element)
	{
		TiXmlElement* child = element->FirstChildElement();
		for (; child!=NULL; child=child->NextSiblingElement())
		{
			AosRVGPtr ptr = AosRVG::RVGFactory(child);
			if (ptr)
			{
				mBRVGList.push_back(ptr);
			}
		}
	}
	return true;
}


void 
AosComposeRSG::getBRSGList(AosRVGPtrList &rvgList)
{
	rvgList = mBRVGList;
}


void 
AosComposeRSG::pushBRSG(const AosRVGPtr& rvg)
{
	mBRVGList.push_back(rvg);
}


AosRVGPtr 
AosComposeRSG::popBRSG()
{
	AosRVGPtr ptr = mBRVGList.back();
	mBRVGList.pop_back();
	return ptr;
}


void 
AosComposeRSG::clearBRSGList()
{
	mBRVGList.clear();
}


u32 
AosComposeRSG::getTotalComposeWeight(const AosRVGPtrList& rvgList)
{
	u32 weight = 0;
	for (AosRVGPtrList::const_iterator iter=rvgList.begin(); iter!=rvgList.end(); iter++)
	{
		if (!(*iter))
		{
			OmnAlarm << "The RSG pointer can not be empty" << enderr;
			return weight;
		}
		weight += (*iter)->getComposeWeight();
	}

	return weight;
}



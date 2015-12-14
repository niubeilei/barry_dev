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
// Saturday, December 01, 2007: Created by Allen Xu 
//
////////////////////////////////////////////////////////////////////////////

#include "RVG/RVG.h"
#include "RVG/RIG.h"
#include "RVG/CharsetRSG.h"
#include "RVG/EnumRSG.h"
#include "RVG/EnumSeqRSG.h"
#include "RVG/AndRSG.h"
#include "RVG/OrRSG.h"
#include "Util/OmnNew.h"
#include "RVG/ConfigMgr.h"
#include <unistd.h>
//#include "LogicExpr/LogicTerm.h"
//#include "CmdTorturer/CmdTorturer.h"

AosRVG::AosRVG():
mWeight(eRVGDefaultWeight),
mComposeWeight(eRVGDefaultComposeWeight)
{
}


AosRVG::~AosRVG()
{
}


bool 
AosRVG::serialize(TiXmlNode& node)
{
	// 
	// parse the commen atrributes
	// <"RVGTypeStr">
	// 		<Name>a_global_unique_name</Name>
	// 		<ProductName></ProductName>
	// 		<ValueType></ValueType>
	//		<Weight></Weight>
	//		<ComposeWeight>100</ComposeWeight>
	//		...
	// <"RVGTypeStr">
	//
	node.SetValue(AosRVGType::enum2Str(mRVGType));

	//
	// add Name node
	//
	node.addElement("Name", mName);

	//
	// add ProductName node
	//
	node.addElement("ProductName", mProductName);

	//
	// add ValueType node
	//
	node.addElement("ValueType", AosValueType::enum2Str(mValueType));

	//
	// add Weight node
	//
	node.addElement("Weight", AosValue::itoa(mWeight));

	//
	// add ComposeWeight node
	//
	node.addElement("ComposeWeight", AosValue::itoa(mComposeWeight));
	return true;
}


bool 
AosRVG::deserialize(TiXmlNode* node)
{
	// 
	// parse the commen atrributes
	// <"RVGTypeStr">
	// 		<Name>a_global_unique_name</Name>
	// 		<ProductName></ProductName>
	// 		<ValueType></ValueType>
	//		<ComposeWeight>100</ComposeWeight>
	// <"RVGTypeStr">
	//
	if (node == NULL)
	{
		return false;
	}

	if (node->ValueStr() != AosRVGType::enum2Str(mRVGType))
	{
		return false;
	}

	//
	// parse name, mandotary field
	//
	if (!node->getElementTextValue("Name", mName))
	{
		OmnAlarm << "XML parse: Name must exist in RVG" << enderr;
		return false;
	}

	//
	// parse ProductName, mandotary field
	//
	if (!node->getElementTextValue("ProductName", mProductName))
	{
		OmnAlarm << "XML parse: ProductName must exist in RVG" << enderr;
		return false;
	}

	//
	// parse value type, optional field
	//
	TiXmlHandle docHandle(node);
	TiXmlElement* element = docHandle.FirstChild("ValueType").ToElement();
	if (element)
	{
		const char* typeStr = element->GetText();
		if (typeStr && OmnString(typeStr) != AosValueType::enum2Str(mValueType))
		{
			return false;
		}
	}

	//
	// parse weight, mandatory field
	//
	if (!node->getElementTextValue("Weight", mWeight))
	{
		OmnAlarm << "XML parse: Weight must exist in RVG" << enderr;
		return false;
	}

	//
	// parse compose weight, mandatory field
	//
	if (!node->getElementTextValue("ComposeWeight", mComposeWeight))
	{
		OmnAlarm << "XML parse: ComposeWeight must exist in RVG" << enderr;
		return false;
	}
	return true;
}


u32 
AosRVG::getWeight() const
{
	return mWeight;
}


void 
AosRVG::setWeight(u32 weight)
{
	mWeight = weight;
}


u32 
AosRVG::getComposeWeight() const
{
	return mComposeWeight;
}


void 
AosRVG::setComposeWeight(u32 weight)
{
	mComposeWeight = weight;
}


OmnString
AosRVG::getName()
{
	return mName;
}


void 
AosRVG::setName(OmnString &name)
{
	mName = name;
}


OmnString
AosRVG::getProductName()
{
	return mProductName;
}


void 
AosRVG::setProductName(OmnString &name)
{
	mProductName = name;
}


AosRVGType::E
AosRVG::getRVGType()
{
	return mRVGType;
}


void 
AosRVG::setRVGType(AosRVGType::E type)
{
	mRVGType = type;
}


AosValueType::E
AosRVG::getValueType()
{
	return mValueType;
}


void 
AosRVG::setValueType(AosValueType::E type)
{
	mValueType = type;
}


AosRVGPtr 
AosRVG::RVGFactory(TiXmlNode* node)
{
	AosRVGPtr rvgPtr = NULL;
	if (node == NULL)
	{
		return rvgPtr;
	}
	OmnString xmlNodeStr = node->ValueStr();
	AosRVGType::E i = AosRVGType::str2Enum(xmlNodeStr);
	switch (i)
	{
		case AosRVGType::eCharsetRSG:
			rvgPtr = OmnNew AosCharsetRSG();
			break;
		case AosRVGType::eEnumRSG:
			rvgPtr = OmnNew AosEnumRSG();
			break;
		case AosRVGType::eEnumSeqRSG:
			rvgPtr = OmnNew AosEnumSeqRSG();
			break;
		case AosRVGType::eU8RIG:
			rvgPtr = OmnNew AosRIG<u8>();
			break;
		case AosRVGType::eU16RIG:
			rvgPtr = OmnNew AosRIG<u16>();
			break;
		case AosRVGType::eU32RIG:
			rvgPtr = OmnNew AosRIG<u32>();
			break;
		case AosRVGType::eU64RIG:
			rvgPtr = OmnNew AosRIG<u64>();
			break;
		case AosRVGType::eInt8RIG:
			rvgPtr = OmnNew AosRIG<int8_t>();
			break;
		case AosRVGType::eInt16RIG:
			rvgPtr = OmnNew AosRIG<int16_t>();
			break;
		case AosRVGType::eInt32RIG:
			rvgPtr = OmnNew AosRIG<int32_t>();
			break;
		case AosRVGType::eInt64RIG:
			rvgPtr = OmnNew AosRIG<int64_t>();
			break;
		case AosRVGType::eAndRSG:
			rvgPtr = OmnNew AosAndRSG();
			break;
		case AosRVGType::eOrRSG:
			rvgPtr = OmnNew AosOrRSG();
			break;
		default:
			OmnAlarm << "Unrecognized RVG type: " << xmlNodeStr << enderr;
			break;
	}
	if (rvgPtr)
	{
		rvgPtr->deserialize(node);
	}
	return rvgPtr;
}


AosRVGPtr 
AosRVG::RVGFactory(AosRVGType::E type)
{
	AosRVGPtr rvgPtr = NULL;
	switch (type)
	{
		case AosRVGType::eCharsetRSG:
			rvgPtr = OmnNew AosCharsetRSG();
			break;
		case AosRVGType::eEnumRSG:
			rvgPtr = OmnNew AosEnumRSG();
			break;
		case AosRVGType::eEnumSeqRSG:
			rvgPtr = OmnNew AosEnumSeqRSG();
			break;
		case AosRVGType::eU8RIG:
			rvgPtr = OmnNew AosRIG<u8>();
			break;
		case AosRVGType::eU16RIG:
			rvgPtr = OmnNew AosRIG<u16>();
			break;
		case AosRVGType::eU32RIG:
			rvgPtr = OmnNew AosRIG<u32>();
			break;
		case AosRVGType::eU64RIG:
			rvgPtr = OmnNew AosRIG<u64>();
			break;
		case AosRVGType::eInt8RIG:
			rvgPtr = OmnNew AosRIG<int8_t>();
			break;
		case AosRVGType::eInt16RIG:
			rvgPtr = OmnNew AosRIG<int16_t>();
			break;
		case AosRVGType::eInt32RIG:
			rvgPtr = OmnNew AosRIG<int32_t>();
			break;
		case AosRVGType::eInt64RIG:
			rvgPtr = OmnNew AosRIG<int64_t>();
			break;
		case AosRVGType::eAndRSG:
			rvgPtr = OmnNew AosAndRSG();
			break;
		case AosRVGType::eOrRSG:
			rvgPtr = OmnNew AosOrRSG();
			break;
		default:
			OmnAlarm << "Unrecognized RVG type: " << enderr;
			break;
	}
	return rvgPtr;
}


AosRVGPtr 
AosRVG::RVGFactory(const OmnString &configFile)
{
	TiXmlDocument doc(configFile);
	if (!doc.LoadFile())
	{
		return NULL;	
	}
	TiXmlElement* element = doc.FirstChildElement();
	return RVGFactory(element);
}


bool 
AosRVG::writeToFile()
{
	if (mName.empty())
	{
		OmnAlarm << "The RVG name is empty, please set it." << enderr;
		return false;
	}
	OmnString configFile;
	if (!AosConfigMgr::getConfigFileName(mProductName, AosConfigMgr::eRVG, mName, configFile))
	{
		return false;
	}
	return writeToFile(configFile);
}


bool 
AosRVG::writeToFile(const OmnString &configFile)
{
	TiXmlDocument doc(configFile);
	TiXmlElement element("");
	if (!serialize(element))
	{
		return false;
	}
	doc.InsertEndChild(element);
	doc.SaveFile();
	return true;
}


bool 
AosRVG::deleteConfigFile()
{
	if (mName.empty())
	{
		OmnAlarm << "The RVG name is empty, please set it." << enderr;
		return false;
	}
	OmnString configFile;
	if (!AosConfigMgr::getConfigFileName(mProductName, AosConfigMgr::eRVG, mName, configFile))
	{
		return false;
	}
	return (unlink(configFile.c_str()) == 0);
}


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
// Saturday, December 15, 2007: Created by Allen Xu 
//
////////////////////////////////////////////////////////////////////////////

#include "Alarm/Alarm.h"
#include "RVG/Parm.h"
#include "Util/OmnNew.h"
#include "Conditions/Condition.h"
#include "RVG/ConfigMgr.h"
#include "Random/RandomUtil.h"
#include "alarm/Alarm.h"


AosParm::AosParm()
{
	mLastValue = NULL;
	mCorrectRVG = NULL;
}


AosParm::~AosParm()
{
}


bool 
AosParm::nextValue(AosValue &value, bool &isCorrect, OmnString &errmsg)
{
	//
	// determine to generate correct or incorrect value
	//
	calculateCorrectFlag(isCorrect);
	//
	// Use the correct related RVGs to generate value
	//
	value.setType(mDataType);
	if (isCorrect)
	{
		if (!createCorrectValue(value,errmsg))	
		{
			return false;
		}
	}
	else
	{
		if (!createIncorrectValue(value,errmsg))	
		{
			return false;
		}
	}

	mLastValue = OmnNew AosValue(value);
	mLastValue->setType(mDataType);
	return true;
}


bool 
AosParm::createCorrectValue(AosValue &value, OmnString &errmsg)
{
	//
	// select a correct RVG and generate value
	//
	AosValue valueTmp;
	if (!mCorrectSelector.nextValue(valueTmp))
	{
		OmnAlarm << "select correct RVG error" << enderr;
		errmsg = "select correct RVG error";
		return false;
	}
	u32 index = valueTmp.toUint32();
	aos_assert_r(index<mAllCorrectRVGs.size(), false);
	if (!mAllCorrectRVGs[index]->nextValue(value))
	{
		OmnAlarm << "Generate correct value error" << enderr;
		errmsg = "Generate correct value error";
		return false;
	}
	//
	// check all the correct conditions
	//
	
	return true;
}


bool 
AosParm::createIncorrectValue(AosValue &value, OmnString &errmsg)
{
	//
	// select a incorrect RVG and generate value
	//
	AosValue valueTmp;
	if (!mIncorrectSelector.nextValue(valueTmp))
	{
		OmnAlarm << "Select incorrect RVG error" << enderr;
		errmsg = "Select incorrect RVG error";
		return false;
	}
	u32 index = valueTmp.toUint32();
	aos_assert_r(index<mAllIncorrectRVGs.size(), false);
	if (!mAllIncorrectRVGs[index]->nextValue(value))
	{
		OmnAlarm << "Generate incorrect value error" << enderr;
		errmsg = "Generate incorrect value error";
		return false;
	}
	//
	// check all the incorrect conditions
	//
	
	return true;
}


void 
AosParm::createCorrectSelector()
{
	mCorrectSelector.clearIntegerPair();
	int crtNum = mAllCorrectRVGs.size();
	for (int i=0; i<crtNum; i++)
	{
		mCorrectSelector.setIntegerPair(i, i, mAllCorrectRVGs[i]->getWeight());
	}
}


void 
AosParm::createIncorrectSelector()
{
	mIncorrectSelector.clearIntegerPair();
	int crtNum = mAllIncorrectRVGs.size();
	for (int i=0; i<crtNum; i++)
	{
		mIncorrectSelector.setIntegerPair(i, i, mAllIncorrectRVGs[i]->getWeight());
	}
}


bool
AosParm::calculateCorrectFlag(bool& isCorrect)
{
	isCorrect = true;
	switch (mCorrectnessControl)
	{
	case AosCorCtrl::eCorrectOnly:
	case AosCorCtrl::eCorrectIfPossible:
		 isCorrect = true;
		 return true;

	case AosCorCtrl::eIncorrectOnly:
	case AosCorCtrl::eIncorrectIfPossible:
		 isCorrect = false;
		 return true;

	case AosCorCtrl::eRandom:
		 aos_assert_r(mCorrectPct >= 0 && mCorrectPct <= 100, false);
		 isCorrect = aos_next_pct(mCorrectPct)?true:false;
		 return true;

	default:
		 OmnAlarm << "Unrecognized correctness: "
			 << mCorrectnessControl << enderr;
		 return false;
	}

	aos_should_never_come_here;
	return false;
}


//
// initialize the RVG selector
//
bool 
AosParm::initSelector()
{
	//
	// combine all correct RVGs
	//
	if (mCorrectRVG)
	{
		mAllCorrectRVGs.push_back(mCorrectRVG);
	}
	if (!mUserCorrectRVGs.empty())
	{
		mAllCorrectRVGs.insert(mAllCorrectRVGs.end(), 
							mUserCorrectRVGs.begin(), 
							mUserCorrectRVGs.end());
	}
	//
	// combine all incorrect RVGs
	//
	if (!mIncorrectRVGs.empty())
	{
		mAllIncorrectRVGs.insert(mAllIncorrectRVGs.end(), 
							mIncorrectRVGs.begin(), 
							mIncorrectRVGs.end());
	}
	if (!mUserIncorrectRVGs.empty())
	{
		mAllIncorrectRVGs.insert(mAllIncorrectRVGs.end(), 
							mUserIncorrectRVGs.begin(), 
							mUserIncorrectRVGs.end());
	}
	createCorrectSelector();
	createIncorrectSelector();
	return true;
}


bool 
AosParm::serialize(TiXmlNode& node)
{
//
// <Parm>
// 		<Name>Local_unique_name</Name>
// 		<ProductName></ProductName>
// 		<ArgName></ArgName>
// 		<UsageType></UsageType>
// 		<DataType></DataType>
// 		<DataTypeStr>char/int/struct StructName</DataTypeStr>
// 		<VariableDecl></VariableDecl>
// 		<CorrectPct></CorrectPct>
// 		<CorrectRVG>an RVG Name</CorrectRVG>
// 		<UserCorrectRVGs>
//			<RVGName></RVGName>
//			<RVGName></RVGName>
//			...
//			<RVGName></RVGName>
//		</UserCorrectRVGs>
// 		<UserInCorrectRVGs>
//			<RVGName></RVGName>
//			<RVGName></RVGName>
//			...
//			<RVGName></RVGName>
//		</UserInCorrectRVGs>
// 		<CorrectConditions>
//			<Condition></Condition>
//			<Condition></Condition>
//			...
//			<Condition></Condition>
//		</CorrectConditions>
// 		<InCorrectConditions>
//			<Condition></Condition>
//			<Condition></Condition>
//			...
//			<Condition></Condition>
//		</InCorrectConditions>
// </Parm>
//
	return true;
}


bool 
AosParm::deserialize(TiXmlNode* node)
{
//
// <Parm>
// 		<Name>Local_unique_name</Name>
// 		<ProductName></ProductName>
// 		<ArgName></ArgName>
// 		<UsageType></UsageType>
// 		<DataType></DataType>
// 		<DataTypeStr></DataTypeStr>
// 		<VariableDecl></VariableDecl>
// 		<CorrectPct></CorrectPct>
// 		<CorrectRVG>an RVG Name</CorrectRVG>
// 		<InitMemData></InitMemData>
// 		<UserCorrectRVGs>
//			<RVGName></RVGName>
//			<RVGName></RVGName>
//			...
//			<RVGName></RVGName>
//		</UserCorrectRVGs>
// 		<UserInCorrectRVGs>
//			<RVGName></RVGName>
//			<RVGName></RVGName>
//			...
//			<RVGName></RVGName>
//		</UserInCorrectRVGs>
// 		<CorrectConditions>
//			<Condition></Condition>
//			<Condition></Condition>
//			...
//			<Condition></Condition>
//		</CorrectConditions>
// 		<InCorrectConditions>
//			<Condition></Condition>
//			<Condition></Condition>
//			...
//			<Condition></Condition>
//		</InCorrectConditions>
// </Parm>
//
	if (node == NULL)
	{
		return false;
	}
	//
	// parse name, mandatory field
	//
	if (!node->getElementTextValue("Name", mName))
	{
		OmnAlarm << "XML parse: Name must exist in Parameter" << enderr;
		return false;
	}
	//
	// parse ProductName, mandatory field
	//
	if (!node->getElementTextValue("ProductName", mProductName))
	{
		OmnAlarm << "XML parse: ProductName must exist in Parameter" << enderr;
		return false;
	}
	//
	// parse ArgName, mandatory field
	//
	if (!node->getElementTextValue("ArgName", mArgName))
	{
		OmnAlarm << "XML parse: ArgName must exist in Parameter" << enderr;
		return false;
	}
	//
	// parse DataType, mandatory field
	//
	TiXmlHandle docHandle(node);
	TiXmlElement* element = docHandle.FirstChild("DataType").ToElement();
	if (element)
	{
		const char* typeStr = element->GetText();
		if (typeStr)
		{
			mDataType = AosValueType::str2Enum(typeStr);
		}
		else
		{
			OmnAlarm << "DataType tag value can not be empty" << enderr;
			return false;
		}
	}
	//
	// parse UsageType, mandatory field
	//
	element = docHandle.FirstChild("UsageType").ToElement();
	if (element)
	{
		const char* typeStr = element->GetText();
		if (typeStr)
		{
			mUsageType = AosRVGUsageType::str2Enum(typeStr);
		}
		else
		{
			OmnAlarm << "UsageType tag value can not be empty" << enderr;
			return false;
		}
	}
	//
	// parse DataTypeStr, mandatory field
	//
	if (!node->getElementTextValue("DataTypeStr", mDataTypeStr))
	{
		OmnAlarm << "XML parse: DataTypeStr must exist in Parameter" << enderr;
		return false;
	}
	//
	// parse VariableDecl, optional field
	//
	node->getElementTextValue("VariableDecl", mVarDecl);
	//
	// parse CorrectPct, mandatory field
	//
	if (!node->getElementTextValue("CorrectPct", mCorrectPct))
	{
		OmnAlarm << "XML parse: CorrectPct must exist in Parameter" << enderr;
		return false;
	}

	//
	// parse CorrectRVG, mandatory field
	//
	OmnString rvgName;
	if (!node->getElementTextValue("CorrectRVG", rvgName))
	{
		OmnAlarm << "XML parse: CorrectRVG must exist in Parameter" << enderr;
		return false;
	}
	OmnString configFile;
	if (!AosConfigMgr::getConfigFileName(mProductName, AosConfigMgr::eRVG, rvgName, configFile))
	{
		OmnAlarm << "XML parse: The config file is not exist:" 
				<< rvgName		
				<< enderr;
		return false;
	}
	mCorrectRVG = AosRVG::RVGFactory(configFile);
	if (!mCorrectRVG)
	{
		OmnAlarm << "XML parse: CorrectRVG xml tag is error" << enderr;
		return false;
	}
	//
	// get the incorrectRVGs
	//
	mCorrectRVG->createIncorrectRVGs(mIncorrectRVGs);

	//
	// parse InitMemData, optional field
	//
	node->getElementTextValue("InitMemData", mInitMemData);

	//
	// parse UserCorrectRVGs, option field
	//

	//
	// parse UserIncorrectRVGs, option field
	//
	element = docHandle.FirstChild("UserIncorrectRVGs").ToElement();
	if (element)
	{
		OmnString rvgName;
		OmnString configFile;
		AosRVGPtr rvg;
		TiXmlElement* child = element->FirstChildElement("RVGName");
		for (; child!=NULL; child=child->NextSiblingElement("RVGName"))
		{
			if (child->GetText())
			{
				rvgName = child->GetText();
				if (!AosConfigMgr::getConfigFileName(mProductName, AosConfigMgr::eRVG, rvgName, configFile))
				{
					OmnAlarm << "XML parse: The config file is not exist:" 
						<< rvgName		
						<< enderr;
					return false;
				}
				rvg = AosRVG::RVGFactory(configFile);
				if (rvg)
				{
					mUserIncorrectRVGs.push_back(rvg);
				}
			}
		}
	}

	//
	// parse CorrectConditions, option field
	//

	//
	// parse IncorrectConditions, option field
	//

	if (!initSelector())
	{
		OmnAlarm << "Init the RVG selector error" << enderr;
		return false;
	}
	return true;
}


void 
AosParm::setCorrectnessFlag(AosCorCtrl::E correctFlag)
{
	mCorrectnessControl = correctFlag;
}


bool 
AosParm::getCorrectnessFlag()
{
	return mCorrectnessControl;
}


void 
AosParm::setCorrectPct(u32 correctPct)
{
	mCorrectPct = correctPct;
}


u32 
AosParm::getCorrectPct()
{
	return mCorrectPct;
}


bool 
AosParm::getCrtValue(AosValue& value)
{
	if (!mLastValue)
	{
		return false;
	}
	value = *mLastValue;

	return true;
}


bool 
AosParm::getCrtValue(AosValuePtr& valuePtr)
{
	if (!mLastValue)
	{
		return false;
	}
	valuePtr = mLastValue;

	return true;
}


bool 
AosParm::setCrtValue(AosValuePtr& valuePtr)
{
	mLastValue = valuePtr;
	return true;
}


bool 
AosParm::setCrtValue(const OmnString &valueStr)
{
	if (!mLastValue)
	{
		mLastValue = OmnNew AosValue(mDataType);
	}
	mLastValue->fromString(valueStr);
	return true;
}


OmnString 
AosParm::getName()
{
	return mName;
}


OmnString 
AosParm::getInitMemData()
{
	return mInitMemData;
}


OmnString 
AosParm::getVarDecl()
{
	return mVarDecl;
}


AosRVGUsageType::E 
AosParm::getUsageType()
{
	return mUsageType;
}


AosValueType::E 
AosParm::getDataType()
{
	return mDataType;
}


OmnString 
AosParm::getDataTypeStr()
{
	return mDataTypeStr;
}


bool
AosParm::getCrtValueAsArg(OmnString &value, 
						OmnString &decl, 
						const u32 argIndex) const
{
	value = "";
	switch (mUsageType)
	{
		case AosRVGUsageType::eInputOnly:
			if (mVarDecl != "")
			{
				decl += mVarDecl;
				decl += ";\n";
			}
			if (mDataType == AosValueType::eString)
			{
				value += "\"";
				if (mLastValue)
					value += mLastValue->toString();
				value += "\"";
			}
			else
			{
				if (mLastValue)
					value += mLastValue->toString();
			}
			break;

		case AosRVGUsageType::eInputOutputThrRef:
		case AosRVGUsageType::eOutputThrRef:
			if (mVarDecl != "")
			{
				decl += mVarDecl;
				decl += ";\n";
			}
			else
			{
				decl += mDataTypeStr;
				decl += " _mArg_";
			   	decl += mArgName;
			   	decl +=	"(";
				if (mDataType == AosValueType::eString)
				{
			   		decl +=	"\"";
				}
				if (mLastValue)
					decl += mLastValue->toString();
				if (mDataType == AosValueType::eString)
				{
			   		decl +=	"\"";
				}
			   	decl += "); ";
			}
			value += "_mArg_";
		   	value += mArgName;
			break;

		case AosRVGUsageType::eOutputThrAddr:
			if (mVarDecl != "")
			{
				decl += mVarDecl;
				decl += ";\n";
			}
			else
			{
				if (mDataType == AosValueType::eString)
				{
					decl += "char _mArg_";
				   	decl += mArgName;
				   	decl += "[1024];";
					value += "_mArg_";
				   	value += mArgName;
				}
				else
				{
					decl += mDataTypeStr;
					decl += " _mArg_";
				   	decl += mArgName;
				   	decl += ";";
					value += "&_mArg_";
				   	value += mArgName;
				}
			}
			break;

		case AosRVGUsageType::eInputOutputThrAddr:
			if (mVarDecl != "")
			{
				decl += mVarDecl;
				decl += ";\n";
			}
			else
			{
				if (mDataType == AosValueType::eString && mDataTypeStr == "char")
				{
					decl += "char _mArg_";
				   	decl += mArgName;
				   	decl += "[1024];\n";
					decl += "strcpy(_mArg_";
					decl += mArgName;
					decl += ",\"";
					if (mLastValue)
						decl += mLastValue->toString();
					decl += "\");\n";
					value += "_mArg_";
				   	value += mArgName;
				}
				else
				{
					decl += mDataTypeStr;
					decl += " _mArg_";
				   	decl += mArgName;
				   	decl += "(";
					if (mLastValue)
						decl += mLastValue->toString();
				   	decl += ");";
					value += "&_mArg_";
				   	value += mArgName;
				}
			}
			break;

		default:
			OmnAlarm << "Unrecognized UsageType: " << mUsageType << enderr;
			return false;
	}

	return true;
}

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
// Modification History:
// 01/04/2008: Created by Allen Xu
//
////////////////////////////////////////////////////////////////////////////
#include "Torturer/FuncCmd.h"

#include "alarm_c/alarm.h"
#include "aosUtil/Alarm.h"
#include "Torturer/Torturer.h"
#include "RVG/RVG.h"
#include "ProgramAid/Util.h"
#include "Porting/GetTime.h"
#include "Tester/TestPkg.h"
#include "Tester/TestMgr.h"
#include "RVG/ConfigMgr.h"
#include "Util/OmnNew.h"
#include "Util/File.h"

static std::string sgCreationDate = "<$$CreationDate>";
static std::string sgSourceDirName = "<$$SourceDirectoryName>";
static std::string sgHeaderFileName = "<$$HeaderFileName>";
static std::string sgClassName = "<$$ClassName>";
//static std::string sgInstancePointer = "<$$InstancePointer>";
static std::string sgName = "<$$Name>";
static std::string sgProductName = "<$$ProductName>";
static std::string sgIncludes = "<$$Includes>";
static std::string sgPARMS = "<$$PARMS>";
static std::string sgFuncCall = "<$$FuncCall>";
static std::string sgPreCallProcess = "<$$PreCallProcess>";
static std::string sgPostCallProcess = "<$$PostCallProcess>";
//static std::string sgPreAddToTableProc = "<$$PreAddToTableProcSection>";
//static std::string sgCheckTableFuncDef = "<$$CheckTableFuncDef>";
//static std::string sgCheckTableFuncCall = "<$$CheckTableFuncCall>";
static std::string sgPrePostCheckFuncDecl = "<$$PrePostCheckFuncDecl>";
static std::string sgPreCheckCode = "<$$PreCheckCode>";
static std::string sgPostCheckCode = "<$$PostCheckCode>";
static std::string sgMemberDataDecl = "<$$MemberDataDecl>";
static std::string sgInitMemberData = "<$$InitMemberData>";
static std::string sgPreActionFuncDecl = "<$$PreActionFuncDecl>";
static std::string sgPreActionFuncBody = "<$$PreActionFuncBody>";
static std::string sgPostActionFuncDecl = "<$$PostActionFuncDecl>";
static std::string sgPostActionFuncBody = "<$$PostActionFuncBody>";



AosFuncCmd::AosFuncCmd()
{
}


AosFuncCmd::AosFuncCmd(
		const std::string &cmdName, 
		const std::string &productName)
:AosCommand(cmdName,productName)
{
}


AosFuncCmd::~AosFuncCmd()
{
}


bool 
AosFuncCmd::serialize(TiXmlNode& node)
{
	//
	// 	<Func>
	// 		<Name>
	// 		<ProductName>
	// 		<CorrectPct>
	// 		<Weight>
	// 		<Status>
	// 		<CorrectMultiFromTable>
	// 		<IncorrectMultiFromTable>
	// 		<CorrectConditions>
	// 		<IncorrectConditions>
	// 		<Parms>
	// 			<Parm>
	// 			</Parm>
	// 			...
	// 		</Parms>
	// 	<!-- the following fields are parsed in this class-->
	// 		<FuncName>
	//		<CppIncludes>
	//			<Include></Include>
	//			...
	//		</CppIncludes>
	//		<HeaderIncludes>
	//			<Include></Include>
	//			...
	//		</HeaderIncludes>
	//		<PreCallProcess>
	//		<PostCallProcess>
	//		<PreExeAction>
	//		<PostExeAction>
	//		<PreCheck>
	//		<PostCheck>
	// 	</FuncCmd>
	//
	if (!AosCommand::serialize(node))
	{
		OmnAlarm << "Failed to config the object" << enderr;
		return false;
	}
	node.SetValue("Func");

	return true;
}


bool 
AosFuncCmd::deserialize(TiXmlNode* node)
{
	//
	// 	<Func>
	// 		<Name>
	// 		<ProductName>
	// 		<CorrectPct>
	// 		<Weight>
	// 		<Status>
	// 		<CorrectMultiFromTable>
	// 		<IncorrectMultiFromTable>
	// 		<CorrectConditions>
	// 		<IncorrectConditions>
	// 		<Parms>
	// 			<Parm>
	// 			</Parm>
	// 			...
	// 		</Parms>
	// 	<!-- the following fields are parsed in this class-->
	// 		<FuncName>
	// 		<ReturnType>
	//		<CppIncludes>
	//			<Include></Include>
	//			...
	//		</CppIncludes>
	//		<HeaderIncludes>
	//			<Include></Include>
	//			...
	//		</HeaderIncludes>
	//		<PreCallProcess>
	//		<PostCallProcess>
	//		<PreExeAction>
	//		<PostExeAction>
	//		<PreCheck>
	//		<PostCheck>
	// 	</FuncCmd>
	//
	if (!AosCommand::deserialize(node))
	{
		OmnAlarm << "Failed to config the object" << enderr;
		return false;
	}
	//
	// parse FuncName, mandatory field
	//
	if (!node->getElementTextValue("FuncName", mFuncName))
	{
		OmnAlarm << "XML parse: FuncName must exist in Command" << enderr;
		return false;
	}
	if (mFuncName.empty())
	{
		OmnAlarm << "XML parse: FuncName is empty" << enderr;
		return false;
	}
	mClassName = "Aos" + mFuncName;
	mClassName += "Tester";
	//
	// parse ReturnType, mandatory field
	//
	if (!node->getElementTextValue("ReturnType", mReturnType))
	{
		OmnAlarm << "XML parse: ReturnType must exist in Command" << enderr;
		return false;
	}
	//
	// parse CppIncludes, optional field
	//
	TiXmlElement* element = node->FirstChildElement("CppIncludes");
	if (element)
	{
		std::string include;
		TiXmlElement* child = element->FirstChildElement("Include");
		for (; child!=NULL; child=child->NextSiblingElement("Include"))
		{
			if (child->GetText())
			{
				include = child->GetText();
				if (include[0] != '#')				
				{
					include = "#include \"";
					include += child->GetText();
					include += "\"";
				}
				include += "\n";
				mCppIncludes += include;
			}		
		}
	}
	//
	// parse HeaderIncludes, optional field
	//
	element = node->FirstChildElement("HeaderIncludes");
	if (element)
	{
		std::string include;
		TiXmlElement* child = element->FirstChildElement("Include");
		for (; child!=NULL; child=child->NextSiblingElement("Include"))
		{
			if (child->GetText())
			{
				include = child->GetText();
				if (include[0] != '#')				
				{
					include = "#include \"";
					include += child->GetText();
					include += "\"";
				}
				include += "\n";
				mHeaderIncludes += include;
			}		
		}
	}
	//
	// parse PreCallProcess, mandatory field
	//

	//
	// parse PostCallProcess, mandatory field
	//

	//
	// parse PreExeAction, mandatory field
	//

	//
	// parse PostExeAction, mandatory field
	//

	//
	// parse PreCheck, mandatory field
	//

	//
	// parse PostCheck, mandatory field
	//

	return true;
}


bool 	
AosFuncCmd::check()
{
	return true;
}


std::string
AosFuncCmd::toString()
{
	std::string str;
	str = "Function command: \n";
	str += "   	FuncName:            ";
	str += mFuncName;
   	str +=  "\n";
	str	+= "    Number of Arguments: ";
	str += mArgs.size();
	str += "\n";

	AosParmPtrArray::iterator itr;
	for (itr=mArgs.begin(); itr!=mArgs.end(); itr++)
	{
		//str << (*itr)->toString(4) << "\n";
	}

	return str;
}


std::string
AosFuncCmd::getCmdPrefix() const
{
	std::string str = mFuncName;
	str += "(";
	return str;
}


bool	
AosFuncCmd::runCommand(AosCorCtrl::E, 
							const AosTorturerPtr &,
							const OmnTestMgrPtr &,
							bool &cmdCorrect,
							std::string &cmdErrmsg, 
							bool &cmdExecuted)
{
	return true;
}


bool
AosFuncCmd::constructCmd(const u32 argIndex, 
							  std::string &decl, 
							  std::string &deleteStmt,
							  std::string &cmd)
{
	if (argIndex < 0 || argIndex >= mArgs.size())
	{
		return false;
	}
	//
	// Argument value was generated, we need now to generate 
	// a string for how to use the value in the function call. 
	// This is done by calling the function "getCrtValueAsArg".
	// If it needs to declare some local variables, the declarations
	// are returned through "decl".
	//
	std::string value;
	if (!mArgs[argIndex]->getCrtValueAsArg(value, decl, argIndex))
	{
		OmnAlarm << "Failed to get the argument: " 
			<< mArgs[argIndex]->getName() << enderr;
		return false;
	}

	// 
	// If local variables are declared and memory are allocated
	// for them, then we need to construct statements to delete
	// the memory after calling the funciton. This is what 
	// "getDeleteStmt(...)" does.
	//
	if (!mArgs[argIndex]->getDeleteStmt(deleteStmt, argIndex))
	{
		OmnAlarm << "Failed to get the delete statement: " 
			<< mArgs[argIndex]->getName() << enderr;
		return false;
	}

	if (argIndex == 0)
	{
		cmd += value;
	}
	else
	{
		cmd += ", ";
	   	cmd += value;
	}

	return true;
}


// 
// Description:
// All Parms have been instantiated and a command was generated.
// It is the time to finish up the function call. 
//
void
AosFuncCmd::finishUpCmd(const u32 cmdId, 
							 const std::string &decl, 
							 const std::string &deleteStmt,
							 std::string &cmd)
{
	// 
	// The function call has been constructed. Need to determine 
	// whether local variable declarations are needed. This is 
	// determined by "decl".
	//
	cmd += ")";
	if (decl != "")
	{
		std::string theCmd = "\n    {\n";
		theCmd += "        ";
	    theCmd += decl;
		theCmd += "\n        cout << \"CmdId ";
		theCmd += AosValue::itoa(cmdId);
	   	theCmd +=  ": \" << ";
	    theCmd += cmd;
	   	theCmd += " << endl;\n";
		if (deleteStmt != "")
		{
			theCmd += "        ";
		   	theCmd += deleteStmt;
		   	theCmd += "\n";
		}
		theCmd += "    }";
		cmd = theCmd;
	}
	else
	{
		std::string theCmd = "\n    cout << \"CmdId ";
		theCmd += AosValue::itoa(cmdId);
	   	theCmd += ": \" << ";
		theCmd += cmd;
	   	theCmd += " << endl;\n";
		cmd = theCmd;
	}
}


bool 
AosFuncCmd::generateCode(const std::string& srcDir, const bool force)
{
	return createCppFile(srcDir, force) && 
		   createHeaderFile(srcDir, force);
}


std::string 
AosFuncCmd::getClassName()
{
	return mClassName;
}


std::string 
AosFuncCmd::getIncludeStmt()
{
	std::string str = "#include \"";
	str += "./" + mFuncName + "_Tester.h\"";
	return str;
}


bool	
AosFuncCmd::createCppFile(const std::string& srcDir, const bool force)
{
	std::string platformDir;
	if (!AosConfigMgr::getPlatformDir(platformDir))
	{
		return false;
	}

	std::string filename = srcDir;
	filename  += "/";
   	filename +=	 mFuncName + "_Tester.cpp";
	if (force || !OmnFile::fileExist(filename.c_str()))
	{
		// 
		// Create a new file. This is to copy the file: 
		// CodeTemplate/FuncCmd.cpp to
		// srcDir/FuncName_Tester.cpp.
		//
		std::string cmd = "cp -f ";
		cmd += platformDir;
		cmd += "/CodeTemplate/FuncCmd.cpp ";
		cmd += filename;
		system(cmd.data());

		OmnFile file(filename.c_str(), OmnFile::eReadOnly);
		if (!file.isGood())
		{
			OmnAlarm << "Failed to open the file: " 
				<< filename
				<< enderr;
			return false;
		}

		std::string headername;
		headername = mFuncName + "_Tester.h";
		std::string instname = mClassName;
		instname += "Ptr";
		aos_assert_r(file.replace(sgCreationDate.c_str(), 
				OmnGetMDY(), false), false);
		aos_assert_r(file.replace(sgSourceDirName.c_str(), 
				".", false), false);
		aos_assert_r(file.replace(sgHeaderFileName.c_str(), 
				headername.c_str(), false), false);
		aos_assert_r(file.replace(sgClassName.c_str(), 
				mClassName.c_str(), true), false);
		aos_assert_r(file.replace(sgName.c_str(), 
				mCmdName.c_str(), false), false);
		aos_assert_r(file.replace(sgProductName.c_str(), 
				mProductName.c_str(), false), false);
		aos_assert_r(file.replace(sgPreCallProcess.c_str(), 
				mPreCallProcess.c_str(), false), false);
		aos_assert_r(file.replace(sgPostCallProcess.c_str(), 
				mPostCallProcess.c_str(), false), false);

		file.flushFileContents();
		file.closeFile();
	}

	OmnFile file(filename.c_str(), OmnFile::eReadOnly);
	// 
	// Add the include files for all the testers.
	// This includes the "Ptrs.h" file
	//
	aos_assert_r(file.replace(sgIncludes.c_str(), 
				mCppIncludes.c_str(), false), false);

	std::string parms;
	std::string funcCall;
	if (!mReturnType.empty())
	{
		funcCall += mReturnType + " __ret;\n";
	}

	funcCall += "    if (!TestOnly)\n";
	funcCall +=	"    {\n";
	funcCall += "        cmdExecuted = true;\n";
	funcCall += "         __ret = ";
	funcCall += mFuncName + "(";

	bool first = true;
	mMemberDataDecl = "";
	std::string memDataInit;
	int parmNum = mArgs.size();
	for (int i=0; i<parmNum; i++)
	{
		if (first)
		{
			first = false;
		}
		else
		{	
			funcCall += ", ";
		}

		if (!mArgs[i]->getInitMemData().empty())
		{
			memDataInit += mArgs[i]->getInitMemData() + ";\n";
		}
		std::string decl = mArgs[i]->getVarDecl();
		std::string dataType = mArgs[i]->getDataTypeStr();
		switch (mArgs[i]->getUsageType())
		{
		case AosRVGUsageType::eInputOnly:
			 // 
			 // The parameter is used as either input only or input/output
			 // and the output is through a reference.
			 //
			 if (!decl.empty())
			 {
			 	mMemberDataDecl += "    " + decl + ";\n";
			 }
			 else
			 {
				mMemberDataDecl += "    " + dataType;
			   	mMemberDataDecl += " _mArg_" + mArgs[i]->getName() + "; \n";
				std::string index = AosValue::itoa(i);
				std::string var = "_mArg_" + mArgs[i]->getName();
				parms += "	AosValue value";
			   	parms += index;
				parms += ";\n";
				parms += "    OmnTCTrue_1(mArgs[";
			   	parms += index;
				parms += "]->getCrtValue(value" + index;
				parms += "), false) << endtc;\n    ";
				parms += "	value" + index;
				if (dataType == "char*" || dataType == "char *")
				{
					parms += ".getValue(" + var + ");\n";
				}
				else
				{
					parms += ".getValue(&" + var + ");\n";
				}
			 }

			 funcCall += "_mArg_" + mArgs[i]->getName();
			 break;
		case AosRVGUsageType::eInputOutputThrRef:
			 // 
			 // The parameter is used as either input only or input/output
			 // and the output is through a reference.
			 //
			 if (!decl.empty())
			 {
			 	mMemberDataDecl += "    " + decl + ";\n";
			 }
			 else
			 {
				mMemberDataDecl += "    " + dataType;
			   	mMemberDataDecl += " _mArg_" + mArgs[i]->getName() + "; \n";
				std::string index = AosValue::itoa(i);
				std::string var = "_mArg_" + mArgs[i]->getName();
				parms += "	AosValue value";
			   	parms += index;
				parms += ";\n";
				parms += "    OmnTCTrue_1(mArgs[";
			   	parms += index;
				parms += "]->getCrtValue(value" + index;
				parms += "), false) << endtc;\n    ";
				parms += "	value" + index;
				parms += ".getValue(&" + var + ");\n";
			 }

			 funcCall += "_mArg_" + mArgs[i]->getName();
			 break;
		case AosRVGUsageType::eInputOutputThrAddr:
			 // 
			 // The parameter is used as either input only or input/output
			 // and the output is through a reference.
			 //
			 if (!decl.empty())
			 {
			 	mMemberDataDecl += "    " + decl + ";\n";
			 }
			/* else if (!mArgs[i]->getInstGenName().empty())
			 {
				 // 
				 // The parameter is a structure. We need to use the instance
				 // generator to generate an instance for it.
				 //
				 if (mArgs[i]->isSmartPtr())
				 {
					mMemberDataDecl << "    " << dataType << "Ptr _mArg_" << mArgs[i]->getName() << ";\n";
					parms << "_mArg_" << mArgs[i]->getName() << " = (" << dataType << "*)mArgs[" 
						<< i << "]->getData();\n"; 
				 }
				 else
				 {
					mMemberDataDecl << "    " << dataType << " _mArg_" << mArgs[i]->getName() << ";\n";
				   	parms << "_mArg_" << mArgs[i]->getName() << " = ("
				  		 << dataType << ")mArgs["
				  		 << i << "]->getData();\n";
				 }
			 }*/
			 else
			 {
				mMemberDataDecl += "    " + dataType;
			   	mMemberDataDecl += " _mArg_" + mArgs[i]->getName() + "; \n";
				std::string index = AosValue::itoa(i);
				std::string var = "_mArg_" + mArgs[i]->getName();
				parms += "	AosValue value";
			   	parms += index;
				parms += ";\n";
				parms += "    OmnTCTrue_1(mArgs[";
			   	parms += index;
				parms += "]->getCrtValue(value" + index;
				parms += "), false) << endtc;\n    ";
				parms += "	value" + index;
				parms += ".getValue(" + var + ");\n";
			 }

			 funcCall += "_mArg_" + mArgs[i]->getName();
			 break;

		case AosRVGUsageType::eOutputThrRef:
			 //
			 // The parameter is used as output only and it is through 
			 // reference.
			 //
			 if (!decl.empty())
			 {
			 	mMemberDataDecl += "    " + decl + ";\n";
			 }
			/* else if (mArgs[i]->isSmartPtr())
			 {
			 	mMemberDataDecl += "    " + dataType + " Ptr _mArg_" + mArgs[i]->getName() + ";\n";
			 }*/
			 else
			 {
			 	mMemberDataDecl += "    " + dataType + " _mArg_" + mArgs[i]->getName() + ";\n";
			 }
			 funcCall += "_mArg_" + mArgs[i]->getName();
			 break;

		case AosRVGUsageType::eOutputThrAddr:
			 // 
			 // The parameter is used as output only and it is through 
			 // addresses. 
			 //    datatype arg_i;
			 // 
			 if (!decl.empty())
			 {
			 	mMemberDataDecl += "    " + decl + ";\n";
			 }
			 else
			 {
			 	mMemberDataDecl += "    " + dataType + " _mArg_" + mArgs[i]->getName() + ";\n";
			 }
			 funcCall += "&_mArg_"  + mArgs[i]->getName();
			 break;

		default:
			 OmnAlarm << "Invalid Parm Usage Type: " 
			 	<< mArgs[i]->getUsageType()
				<< enderr;
			 return false;
		}
	}

	funcCall += ");\n    }\n";
	funcCall += "    else\n";
	funcCall += "    {\n";
	funcCall += "        cmdExecuted = false;\n";
	funcCall += "    }\n";

	if (!mReturnType.empty())
	{
		if (AosProgramAid_dataTypeSupported(mReturnType.c_str()))
		{
			funcCall += "    mFuncRet = \"\";\n";
			funcCall += "    mFuncRet +=  __ret;";
		}
	}

	aos_assert_r(file.replace(sgInitMemberData.c_str(), 
				memDataInit.c_str(), false), false);
	aos_assert_r(file.replace(sgPARMS.c_str(), 
				parms.c_str(), false), false);
	aos_assert_r(file.replace(sgFuncCall.c_str(), 
				funcCall.c_str(), false), false);
	/*
	// 
	// Create the function to check table, if needed.
	//
	if (!createCheckTableFunc(file))
	{
		OmnAlarm << "Failed to create CheckTable function" << enderr;
		return false;
	}
	*/

	aos_assert_r(file.replace(sgPreCheckCode.c_str(), 
			mPreCheck.c_str(), false), false);
	aos_assert_r(file.replace(sgPostCheckCode.c_str(), 
			mPostCheck.c_str(), false), false);

	std::string doAction;
	if (mPreActionFunc != "")
	{
		doAction = "bool\n";
		doAction += mClassName + 
				"::doPreAction(bool &rslt, std::string &errmsg)\n";
		doAction += "{\n";
		doAction += "    " + mPreActionFunc + "\n";
		doAction +=	"}\n";
	}

	aos_assert_r(file.replace(sgPreActionFuncBody.c_str(), 
			doAction.c_str(), false), false);
	
	doAction = "";
	if (mPostActionFunc != "")
	{
		doAction = "bool\n";
		doAction += mClassName + 
			"::doPostAction(bool &rslt, std::string &errmsg)\n";
		doAction += "{\n";
		doAction +=	"    " + mPostActionFunc + "\n";
		doAction +=	 "}\n";
	}

	aos_assert_r(file.replace(sgPostActionFuncBody.c_str(), 
			doAction.c_str(), false), false);

	file.flushFileContents();
	file.closeFile();

	return true;
}


bool	
AosFuncCmd::createHeaderFile(const std::string& srcDir, const bool force)const
{
	std::string platformDir;
	if (!AosConfigMgr::getPlatformDir(platformDir))
	{
		return false;
	}

	std::string filename = srcDir;
	filename  += "/";
   	filename +=	 mFuncName + "_Tester.h";
	if (force || !OmnFile::fileExist(filename.c_str()))
	{
		// 
		// Create a new file. This is to copy the file: 
		// CodeTemplate/FuncCmd.h to
		// srcDir/mFuncName_Tester.h.
		//
		std::string cmd = "cp -f ";
		cmd += platformDir;
		cmd += "/CodeTemplate/FuncCmd.h ";
		cmd += filename;
		system(cmd.data());

		OmnFile file(filename.c_str(), OmnFile::eReadOnly);
		if (!file.isGood())
		{
			OmnAlarm << "Failed to open the file: " 
				<< filename << enderr;
			return false;
		}

		std::string headername;
		headername = mFuncName + "_Tester";
		std::string instname = mClassName;
		instname += "Ptr";
		bool ret = file.replace(sgCreationDate.c_str(), OmnGetMDY(), false) &&
			  	   file.replace(sgSourceDirName.c_str(), mProductName.c_str(), true) &&
			  	   file.replace(sgHeaderFileName.c_str(), headername.c_str(), true) &&
			  	   file.replace(sgClassName.c_str(), mClassName.c_str(), true) &&
				   file.replace(sgIncludes.c_str(), mHeaderIncludes.c_str(), true);

		if (!ret)
		{
			OmnAlarm << "Failed to replace the contents: " 
				<< filename << enderr;
			return false;
		}
	
		if (!file.replace(sgMemberDataDecl.c_str(), mMemberDataDecl.c_str(), false))
		{
			OmnAlarm << "Failed to replace MemberDataDecl" << enderr;
			return false;
		}

		std::string actionFunc;
		if (mPreActionFunc != "")
		{
			actionFunc = "    bool doPreAction(bool &rslt, std::string &errmsg);\n";
		}
		if (!file.replace(sgPreActionFuncDecl.c_str(), actionFunc.c_str(), false))
		{
			OmnAlarm << "Failed to replace PreExecution ActionFunc" << enderr;
			return false;
		}
		
		actionFunc = "";
		if (mPostActionFunc != "")
		{
			actionFunc = "    bool doPostAction(bool &rslt, std::string &errmsg);\n";
		}
		if (!file.replace(sgPostActionFuncDecl.c_str(), actionFunc.c_str(), false))
		{
			OmnAlarm << "Failed to replace PostExecution ActionFunc" << enderr;
			return false;
		}

		file.flushFileContents();
		file.closeFile();
	}

	return true;
}


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
// A Function Test is a test in a Tester that tests a function. This test is 
// defined as:
// 
//
// This class member function: "genCode()" will generate the source code for
// the specified function test. 
//
// Modification History:
// 02/18/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "TorturerGen/FuncTesterGen.h"

#include "Alarm/Alarm.h"
#include "CmdTorturer/CmdTorturer.h"
#include "CliTorturer/CliTorturer.h"
#include "CliTorturer/CliCmdTorturer.h"
#include "Porting/GetTime.h"
#include "ProgramAid/Util.h"
#include "Tester/TestMgr.h"
#include "RVG/RVG.h"
#include "RVG/RVGContainer.h"
#include "TorturerGen/TesterGen.h"
#include "Random/RandomUtil.h"
#include "Util/OmnNew.h"
#include "Util/StrParser.h"
#include "Util/File.h"
#include "XmlParser/XmlItem.h"

static OmnString sgCreationDate = "<$$CreationDate>";
static OmnString sgSourceDirName = "<$$SourceDirectoryName>";
static OmnString sgHeaderFileName = "<$$HeaderFileName>";
static OmnString sgClassName = "<$$ClassName>";
static OmnString sgInstancePointer = "<$$InstancePointer>";
static OmnString sgName = "<$$Name>";
static OmnString sgIncludes = "<$$Includes>";
static OmnString sgPARMS = "<$$PARMS>";
static OmnString sgFuncCall = "<$$FuncCall>";
static OmnString sgPreCallProcess = "<$$PreCallProcess>";
static OmnString sgPostCallProcess = "<$$PostCallProcess>";
static OmnString sgPreAddToTableProc = "<$$PreAddToTableProcSection>";
static OmnString sgCheckTableFuncDef = "<$$CheckTableFuncDef>";
static OmnString sgCheckTableFuncCall = "<$$CheckTableFuncCall>";
static OmnString sgPrePostCheckFuncDecl = "<$$PrePostCheckFuncDecl>";
static OmnString sgPreCheckCode = "<$$PreCheckCode>";
static OmnString sgPostCheckCode = "<$$PostCheckCode>";
static OmnString sgMemberDataDecl = "<$$MemberDataDecl>";
static OmnString sgInitMemberData = "<$$InitMemberData>";
static OmnString sgPreActionFuncDecl = "<$$PreActionFuncDecl>";
static OmnString sgPreActionFuncBody = "<$$PreActionFuncBody>";
static OmnString sgPostActionFuncDecl = "<$$PostActionFuncDecl>";
static OmnString sgPostActionFuncBody = "<$$PostActionFuncBody>";

static OmnDynArray<OmnString> 	sgFuncNames;


AosFuncTesterGen::AosFuncTesterGen(
				 const OmnString &srcDirName, 
				 const OmnString &testDirName) 
:
AosTesterGen(srcDirName, testDirName)
{
}


AosFuncTesterGen::~AosFuncTesterGen()
{
}


bool
AosFuncTesterGen::config(const OmnXmlItemPtr &def) 
{
	// 
	//	<FuncTest>
	//		<Name>
	//		<FuncName>	
	//		<Type>FuncTest</Type>
	//		<CppIncludes>
	//		<HeaderIncludes>
	//		<ModuleId>  // Used as the file name for the function tester
	//		<Status>
	//		<PreCallProcess>
	//		<PostCallProcess>
	//		<PreExeAction>
	//		<PostExeAction>
	//		<PreCheck>
	//		<PostCheck>
	//		<ReturnType>
	//		<CheckTable>
	//		<Parms>
	//			<Parm>
	//			...
	//			</Parm>
	//			...
	//		</Parms>
	//	</FuncTest>
	//
	mName = def->getStr("Name", "");
	if (mName == "")
	{
		OmnAlarm << "Missing Name: " << def->toString() << enderr;
		return false;
	}

	mReturnType = def->getStr("ReturnType", "");
	mPostCallProcess = def->getStr("PostCallProcess", "");
	mPreCallProcess = def->getStr("PreCallProcess", "");
	mPreAddToTableProc = def->getStr("PreAddToTableProc", "");

	mPreCheck = def->getStr("PreCheck", "");
	mPreCheck.removeWhiteSpaces();
	replaceSymbols(mPreCheck);

	mPostCheck = def->getStr("PostCheck", "");
	mPostCheck.removeWhiteSpaces();
	replaceSymbols(mPostCheck);

	parseActionFunc(def);


	mFuncName = def->getStr("FuncName", "");
	if (mFuncName == "")
	{
		OmnAlarm << "Missing FuncName: " << def->toString() << enderr;
		return false;
	}

	if (isFuncNameAlreadyUsed(mFuncName))
	{
		OmnAlarm << "Function name has already been used: " 
			<< def->toString() << enderr;
		return false;
	}
	sgFuncNames.append(mFuncName);

	mClassName = "AosClass_";
	mClassName << mFuncName;

	mStatus = AosCliTorturer::getSelf()->isCommandOn(def);

	mCppIncludes = "";
	OmnString includes = def->getStr("CppIncludes", "");
	includes.removeLeadingWhiteSpace();
	includes.removeTailWhiteSpace();
	while (includes != "")
	{
		OmnString line = includes.getLine(true);
		line.removeWhiteSpaces();
		mCppIncludes << "#include \"" << line << "\"\n";
	}

	mHeaderIncludes = "";
	includes = def->getStr("HeaderIncludes", "");
	includes.removeLeadingWhiteSpace();
	includes.removeTailWhiteSpace();
	while (includes != "")
	{
		OmnString line = includes.getLine(true);
		mHeaderIncludes.removeLeadingWhiteSpace();
		mHeaderIncludes.removeTailWhiteSpace();
		mHeaderIncludes << "#include \"" << line << "\"\n";
	}

	mModuleId = def->getStr("ModuleId", "");
	if (mModuleId == "")
	{
		OmnAlarm << "Missing ModuleId tag: " << def->toString() << enderr;
		return false;
	}
	(mHeaderFileName = mModuleId) << "_Tester";

    OmnXmlItemPtr parms = def->getItem("Parms");
	if (!parms)
	{
		OmnAlarm << "Missing Parms tag: " << def->toString() << enderr;
		return false;
	}

    OmnVList<AosGenTablePtr> &tables = AosCliTorturer::getSelf()->getTables();
	parms->reset();
	while (parms->hasMore())
	{
		OmnXmlItemPtr parmDef = parms->next();
		AosRVGPtr parm = AosRVG::createRVG(0, parmDef, 
			def->getTag(), tables, AosCliTorturer::getSelf()->isCodeGen());
		if (!parm)
		{
			OmnAlarm << "Failed to create parm: "
				<< parmDef->toString() << enderr;
			return false;
		}

		mParms.append(parm);
	}

	OmnXmlItemPtr checkTableDef = def->getItem("CheckTable");
	if (checkTableDef)
	{
		// 
		// 	<CheckTable>
		// 		<DataType>
		// 		<DataArgIndex>
		// 		<LenArgIndex>
		// 		<FieldDef>
		//			fieldname, $.structname
		//			fieldname, $.structname
		//			...
		//		</FieldDef>
		//	</CheckTable>
		//
		mCheckTableInfo.datatype = checkTableDef->getStr("DataType", "");
		if (mCheckTableInfo.datatype == "")
		{
			OmnAlarm << "Missing data type in CheckTable tag: " 
				<< def->toString() << enderr;
			return false;
		}

		mCheckTableInfo.dataArg = checkTableDef->getStr("DataArg", "");
		mCheckTableInfo.lenArg = checkTableDef->getStr("LenArg", "");
		if (mCheckTableInfo.dataArg == "" || mCheckTableInfo.lenArg == "")
		{
			OmnAlarm << "Incorrect Arg Names in: " 
				<< checkTableDef->toString() 
				<< ". In the definition: " 
				<< def->toString() << enderr;
			return false;
		}

		OmnString str = checkTableDef->getStr("FieldDef", "");
		str.removeLeadingWhiteSpace();
		str.removeTailWhiteSpace();
		if (str == "")
		{
			OmnAlarm << "Invalid CheckTable tag: " 
				<< def->toString() << enderr;
			return false;
		}

		while (str != "")
		{
			OmnString line = str.getLine(true);
			OmnStrParser parser(line);
			OmnString fieldname = parser.nextEngWord("");
			OmnString structname = parser.nextWord("", ",");

			if (fieldname == "" || structname == "")
			{
				OmnAlarm << "FieldName/structname/index is null in the line: " 
					<< line << " in the CheckTable tag: " 
					<< checkTableDef->getStr()
					<< ". The function def: " << def->toString() << enderr;
				return false;
			}

			// 
			// Replace '$' with 'theData[index]'
			//
			OmnString str = "theData[index]";
			int idx = structname.find('$', false);
			if (idx < 0)
			{
				OmnAlarm << "The struct name does not contain '$'. "
					<< "The structname: " << structname 
					<< " in the CheckTable tag: " 
					<< checkTableDef->getStr()
					<< ". The function def: " << def->toString() << enderr;
				return false;
			}

			structname.replace(idx, 1, str);

			mCheckTableInfo.fieldnames.append(fieldname);
			mCheckTableInfo.structnames.append(structname);
		}
	}

	return true;
}


bool
AosFuncTesterGen::generateCode(const bool force)
{
	// 
	// The code will generate the code for the torturer.
	//
	if (!mStatus) return true;

	OmnString ptrFile = AosProgramAid_getAosHomeDir();
	ptrFile << "/src/" << mSrcDirName 
		<< "/" << mTestDirName << "/Ptrs.h";

	return createCppFile(force) && 
		   createHeaderFile(force) && 
		   AosAddSmartPtr("", mSrcDirName, getClassName(), ptrFile);
}


OmnString
AosFuncTesterGen::getIncludeStmt() const
{
	OmnString stmt = "#include \"";
	stmt << mSrcDirName;
	if (mTestDirName != "")
	{
		stmt << "/" << mTestDirName;
	}
	stmt << "/" << mModuleId << "_" << mFuncName << "_Tester.h\"\n";
	return stmt;
}


bool
AosFuncTesterGen::createCppFile(const bool force)
{
	OmnString filename = AosProgramAid_getAosHomeDir(); 
	filename << "/src/" << mSrcDirName;

	if (mTestDirName != "")
	{
    	filename <<	"/" << mTestDirName;
	}
	filename << "/" << mModuleId 
		<< "_" << mFuncName << "_Tester.cpp";
	if (force || !OmnFile::fileExist(filename))
	{
		// 
		// Create a new file. This is to copy the file: 
		// Data/ProgramGen/ApiTorturer.cpp to
		// mSrcDirName/Tester/mModuleId_Tester.cpp.
		//
		OmnString cmd = "cp -f ";
		cmd << AosProgramAid_getAosHomeDir();
		cmd << "/Data/ProgramGen/ApiTorturer.cpp "
			<< filename;
		// OmnTrace << "To run command: " << cmd << endl;
		system(cmd.data());

		OmnFile file(filename, OmnFile::eReadOnly);
		if (!file.isGood())
		{
			OmnAlarm << "Failed to open the file: " 
				<< AosProgramAid_getAosHomeDir() << "/" << mSrcDirName
				<< "/Tester/" << mModuleId << "_Tester.cpp" << enderr;
			return false;
		}

		OmnString headername;
		if (mTestDirName != "")
		{
			headername << mTestDirName << "/";
		}
		
		headername << mModuleId << "_" << mFuncName << "_Tester.h";
		OmnString instname = mClassName;
		instname << "Ptr";
		bool ret = file.replace(sgCreationDate, OmnGetMDY(), false) &&
			  	   file.replace(sgSourceDirName, mSrcDirName, false) &&
			  	   file.replace(sgHeaderFileName, headername, false) &&
			  	   file.replace(sgClassName, mClassName, true) &&
			  	   file.replace(sgName, mName, false) &&
			  	   file.replace(sgPreAddToTableProc, mPreAddToTableProc, false) &&
			  	   file.replace(sgPreCallProcess, mPreCallProcess, false) &&
			  	   file.replace(sgPostCallProcess, mPostCallProcess, false) &&
			  	   file.replace(sgInstancePointer, instname, true);


		if (!ret)
		{
			OmnAlarm << "Failed to replace the contents: " 
				<< filename << enderr;
			return false;
		}
		file.flushFileContents();
		file.closeFile();
	}

	OmnFile file(filename, OmnFile::eReadOnly);

	// 
	// Add the include files for all the testers.
	// This includes the "Ptrs.h" file
	//
	OmnString includes;
	includes << "#include \"" << mSrcDirName;
	if (mTestDirName != "")
	{
		includes << "/" << mTestDirName;
	}
	includes << "/Ptrs.h\"\n";

	includes << mCppIncludes;
	for (int i=0; i<mParms.entries(); i++)
	{
		if (mParms[i]->getInstGenName() != "")
		{
			includes << "\n#include \"InstGen_" 
				<< mParms[i]->getInstGenName() << ".h\"\n";
		}
	}
	
	bool ret = file.replace(sgIncludes, includes, false);
	if (!ret)
	{
	 	OmnAlarm << "Failed to insert includes: " << filename << enderr;
	 	return false;
	}

	OmnString parms;
	OmnString funcCall;
	if (mReturnType != "")
	{
		funcCall << mReturnType << " __ret;\n";
	}

	funcCall << "    if (!TestOnly)\n"
		<< "    {\n"
		<< "        cmdExecuted = true;\n"
		<< "         __ret = "
		<< mFuncName << "(";

	bool first = true;
	mMemberDataDecl = "";
	OmnString memDataInit;
	for (int i=0; i<mParms.entries(); i++)
	{
		if (first)
		{
			first = false;
		}
		else
		{	
			funcCall << ", ";
		}

		if (mParms[i]->getInitMemData() != "")
		{
			memDataInit << mParms[i]->getInitMemData() << ";\n";
		}
		OmnString decl = mParms[i]->getVarDecl();
		OmnString dataType = mParms[i]->getDataType();
		switch (mParms[i]->getUsageMethod())
		{
		case AosRVGUsageType::eInputOnly:
		case AosRVGUsageType::eInputOutputThrRef:
		case AosRVGUsageType::eInputOutputThrAddr:
			 // 
			 // The parameter is used as either input only or input/output
			 // and the output is through a reference.
			 //
			 if (decl != "")
			 {
			 	mMemberDataDecl << "    " << decl << ";\n";
			 }
			 else if (mParms[i]->getInstGenName() != "")
			 {
				 // 
				 // The parameter is a structure. We need to use the instance
				 // generator to generate an instance for it.
				 //
				 if (mParms[i]->isSmartPtr())
				 {
					mMemberDataDecl << "    " << dataType << "Ptr _mArg_" << mParms[i]->getName() << ";\n";
					parms << "_mArg_" << mParms[i]->getName() << " = (" << dataType << "*)mArgs[" 
						<< i << "]->getData();\n"; 
				 }
				 else
				 {
					mMemberDataDecl << "    " << dataType << " _mArg_" << mParms[i]->getName() << ";\n";
				   	parms << "_mArg_" << mParms[i]->getName() << " = ("
				  		 << dataType << ")mArgs["
				  		 << i << "]->getData();\n";
				 }
			 }
			 else
			 {
				mMemberDataDecl << "    " << dataType << " _mArg_" << mParms[i]->getName() << "; \n";
				parms << "    OmnTCTrue_1(mArgs[" << i
					<< "]->getCrtValue(_mArg_" << mParms[i]->getName()
					<< "), false) << endtc;\n    ";
			 }

			 if (mParms[i]->getUsageMethod() == 
			 		AosRVGUsageType::eInputOutputThrAddr)
			 {
			 	funcCall << "&_mArg_" << mParms[i]->getName();
			 }
			 else
			 {
			 	funcCall << "_mArg_" << mParms[i]->getName();
			 }
			 break;

		case AosRVGUsageType::eOutputThrRef:
			 //
			 // The parameter is used as output only and it is through 
			 // reference.
			 //
			 if (decl != "")
			 {
			 	mMemberDataDecl << "    " << decl << ";\n";
			 }
			 else if (mParms[i]->isSmartPtr())
			 {
			 	mMemberDataDecl << "    " << dataType << " Ptr _mArg_" << mParms[i]->getName() << ";\n";
			 }
			 else
			 {
			 	mMemberDataDecl << "    " << dataType << " _mArg_" << mParms[i]->getName() << ";\n";
			 }
			 funcCall << "_mArg_" << mParms[i]->getName();
			 break;

		case AosRVGUsageType::eOutputThrAddr:
			 // 
			 // The parameter is used as output only and it is through 
			 // addresses. 
			 //    datatype arg_i;
			 // 
			 if (decl != "")
			 {
			 	mMemberDataDecl << "    " << decl << ";\n";
			 }
			 else
			 {
			 	mMemberDataDecl << "    " << dataType << " _mArg_" << mParms[i]->getName() << ";\n";
			 }
			 funcCall << "&_mArg_" << mParms[i]->getName();
			 break;

		default:
			 OmnAlarm << "Invalid Parm Usage Method: " 
			 	<< mParms[i]->getUsageMethod()
				<< enderr;
			 return false;
		}
	}

	funcCall << ");\n    }\n"
		<< "    else\n"
		<< "    {\n"
		<< "        cmdExecuted = false;\n"
		<< "    }\n";

	if (mReturnType != "")
	{
		if (AosProgramAid_dataTypeSupported(mReturnType))
		{
			funcCall << "    mFuncRet = \"\";\n";
			funcCall << "    mFuncRet << __ret;";
		}
	}

	ret = file.replace(sgInitMemberData, memDataInit, false);
	if (!ret)
	{
		OmnAlarm << "Failed to replace InitMemData" << enderr;
		return false;
	}

	ret = file.replace(sgPARMS, parms, false);
	if (!ret)
	{
		OmnAlarm << "Failed to insert the argument declaration" << enderr;
		return false;
	}

	ret = file.replace(sgFuncCall, funcCall, false);
	if (!ret)
	{
		OmnAlarm << "Failed to insert the function call" << enderr;
		return false;
	}

	// 
	// Create the function to check table, if needed.
	//
	if (!createCheckTableFunc(file))
	{
		OmnAlarm << "Failed to create CheckTable function" << enderr;
		return false;
	}

	if (!file.replace(sgPreCheckCode, mPreCheck, false) ||
		!file.replace(sgPostCheckCode, mPostCheck, false))
	{
		OmnAlarm << "Failed to replace Pre/PostCheckCode" << enderr;
		return false;
	}

	OmnString doAction;
	if (mPreActionFunc != "")
	{
		doAction = "bool\n";
		doAction << mClassName << "::doPreAction(bool &rslt, OmnString &errmsg)\n"
			<< "{\n"
			<< "    " << mPreActionFunc << "\n"
			<< "}\n";
	}
	if (!file.replace(sgPreActionFuncBody, doAction, false))
	{
		OmnAlarm << "Failed to replace PreExecution Action Function Body" 
			<< enderr;
		return false;
	}
	
	doAction = "";
	if (mPostActionFunc != "")
	{
		doAction = "bool\n";
		doAction << mClassName << "::doPostAction(bool &rslt, OmnString &errmsg)\n"
			<< "{\n"
			<< "    " << mPostActionFunc << "\n"
			<< "}\n";
	}
	if (!file.replace(sgPostActionFuncBody, doAction, false))
	{
		OmnAlarm << "Failed to replace PostExecution Action Function Body" 
			<< enderr;
		return false;
	}

	file.flushFileContents();
	file.closeFile();

	return true;
}


bool
AosFuncTesterGen::createHeaderFile(const bool force)
{
	OmnString filename = AosProgramAid_getAosHomeDir(); 
	filename << "/src/" << mSrcDirName;
	if (mTestDirName != "")
	{
		filename << "/" << mTestDirName;
	}
	filename << "/" << mModuleId << "_" << mFuncName << "_Tester.h";
	if (force || !OmnFile::fileExist(filename))
	{
		// 
		// Create a new file. This is to copy the file: 
		// Data/ProgramGen/ApiTorturer.hto
		// mSrcDirName/Tester/mModuleId_Tester.h.
		//
		OmnString cmd = "cp -f ";
		cmd << AosProgramAid_getAosHomeDir();
		cmd << "/Data/ProgramGen/ApiTorturer.h "
			<< filename;
		// OmnTrace << "To run command: " << cmd << endl;
		system(cmd.data());

		OmnFile file(filename, OmnFile::eReadOnly);
		if (!file.isGood())
		{
			OmnAlarm << "Failed to open the file: " 
				<< filename << enderr;
			return false;
		}

		OmnString headername = mModuleId;
		headername << "_" << mFuncName << "_Tester";
		OmnString instname = mClassName;
		instname << "Ptr";
		OmnString srcdir = mSrcDirName;
		srcdir.replace('/', '_');
		bool ret = file.replace(sgCreationDate, OmnGetMDY(), false) &&
			  	   file.replace(sgSourceDirName, srcdir, true) &&
			  	   file.replace(sgHeaderFileName, headername, true) &&
			  	   file.replace(sgClassName, mClassName, true) &&
				   file.replace(sgIncludes, mHeaderIncludes, true);

		if (!ret)
		{
			OmnAlarm << "Failed to replace the contents: " 
				<< filename << enderr;
			return false;
		}
	
		if (!file.replace(sgMemberDataDecl, mMemberDataDecl, false))
		{
			OmnAlarm << "Failed to replace MemberDataDecl" << enderr;
			return false;
		}

		OmnString actionFunc;
		if (mPreActionFunc != "")
		{
			actionFunc = "    bool doPreAction(bool &rslt, OmnString &errmsg);\n";
		}
		if (!file.replace(sgPreActionFuncDecl, actionFunc, false))
		{
			OmnAlarm << "Failed to replace PreExecution ActionFunc" << enderr;
			return false;
		}
		
		actionFunc = "";
		if (mPostActionFunc != "")
		{
			actionFunc = "    bool doPostAction(bool &rslt, OmnString &errmsg);\n";
		}
		if (!file.replace(sgPostActionFuncDecl, actionFunc, false))
		{
			OmnAlarm << "Failed to replace PostExecution ActionFunc" << enderr;
			return false;
		}

		file.flushFileContents();
		file.closeFile();
	}

	return true;
}


bool
AosFuncTesterGen::checkDataType(const OmnString &dataType) const
{
	return true;
}


bool
AosFuncTesterGen::createCheckTableFunc(OmnFile &file)
{
	if (mCheckTableInfo.fieldnames.entries() <= 0)
	{
		bool ret = file.replace(sgCheckTableFuncDef, "", false) &&
				   file.replace(sgCheckTableFuncCall, "", false);
		if (!ret)
		{
			OmnAlarm << "Failed to replace CheckTableFunc" << enderr;
			return false;
		}

		return true;
	}

	OmnString str;
	str << "static bool\n"
		<< "aos_table_checker_set_record(const AosGenRecordPtr &record, \n"
		<< "                   void *data, \n"
		<< "                   const int index)\n"
		<< "{\n"
		<< "    aos_assert_r(data, false);\n"
		<< "    aos_assert_r(record, false);\n"
		<< "\n"
		<< "    " << mCheckTableInfo.datatype 
		<< "theData = (" << mCheckTableInfo.datatype << ")data;\n";
	
	for (int i=0; i<mCheckTableInfo.fieldnames.entries(); i++)
	{
		str << "    record->set(\"" << mCheckTableInfo.fieldnames[i]
			<< "\", " << mCheckTableInfo.structnames[i]
			<< ");\n";
	}

	str << "\n    return true;\n"
		<< "}\n";

	if (!file.replace(sgCheckTableFuncDef, str, false))
	{
		OmnAlarm << "Failed to replace FuncDef" << enderr;
		return false;
	}

	// 
	// Create the function call portion
	//
	str = "// Check Table \n"
		  "    AosTortCondCheckTable tableChecker(mTable,\n"
	      "                aos_table_checker_set_record, ";
	
	str << "_mArg_" << mCheckTableInfo.dataArg
		<< ", _mArg_" << mCheckTableInfo.lenArg
		<< ");\n";

	str << "    bool rslt;\n"
		<< "    OmnString theErrmsg;\n"
		<< "    AosCondData data;\n"
		<< "    bool tableCheckRet = tableChecker.check(data, rslt, theErrmsg);\n"
		<< "    OmnTCTrue(tableCheckRet) << \"Failed to check table. \"\n"
		<< "    << \" Error message: \" << theErrmsg << endtc;\n"
		<< "    if (rslt) return true;\n"
		<< "\n"
		<< "    OmnTCTrue(rslt) << \"The retrieved contents do not match "
		<< "with the database. \"\n"
		<< "    << \"Errmsg: \" << theErrmsg << endtc;\n";

	if (!file.replace(sgCheckTableFuncCall, str, false))
	{
		OmnAlarm << "Failed to replace the check table func call portion" 
			<< enderr;
		return false;
	}

	return true;
}


bool
AosFuncTesterGen::isFuncNameAlreadyUsed(const OmnString &name)
{
	for (int i=0; i<sgFuncNames.entries(); i++)
	{
		if (sgFuncNames[i] == name) return true;
	}

	return false;
}


bool
AosFuncTesterGen::parseActionFunc(const OmnXmlItemPtr &def)
{
	mPreActionFunc = def->getStr("PreExeAction", "");
	mPreActionFunc.removeWhiteSpaces();
	mPostActionFunc = def->getStr("PostExeAction", "");
	mPostActionFunc.removeWhiteSpaces();
	replaceSymbols(mPreActionFunc);
	replaceSymbols(mPostActionFunc);
	return true;
}


bool
AosFuncTesterGen::replaceSymbols(OmnString &str)
{
	if (str == "") return true;
	str.replace("$Var", "AosCliTorturer_getVar", true);
	str.replace("$Parm", "getParm", true);
	str.replace("$SetVar", "AosCliTorturer_setVar", true);
	str.replace("$Member", "getMember", true);
	return true;
}




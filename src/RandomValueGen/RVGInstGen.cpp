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
// 06/14/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "Parms/RVGInstGen.h"

#include "Debug/Debug.h"
#include "Porting/GetTime.h"
#include "ProgramAid/Util.h"
#include "Util/GenTable.h"
#include "Util/GenRecord.h"
#include "Util/StrParser.h"
#include "Util/File.h"
#include "XmlParser/XmlItem.h"
#include <math.h>

OmnDynArray<AosRVGInstGenPtr>	AosRVGInstGen::mRegistered;
OmnDynArray<AosRVGInstGenPtr>	AosRVGInstGen::mInstances;

AosRVGInstGen::AosRVGInstGen(
					const AosCliCmdTorturerPtr &cmd, 
					const OmnString &name)
:
AosRVG(cmd),
mStatus(true),
mIsPtrDataType(false)
{
	mArgName = name;
}


AosRVGInstGen::~AosRVGInstGen()
{
}


AosRVGInstGenPtr
AosRVGInstGen::getRegistered(const OmnString &name) 
{
	for (int i=0; i<mRegistered.entries(); i++)
	{
		if (mRegistered[i]->getName() == name)
		{
			return mRegistered[i];
		}
	}

	OmnAlarm << "InstanceGen not found: " << name << enderr;
	return 0;
}


// 
// Instances of ParmInstGen must be registered first before
// they can be used. All instances are registered in mRegistered.
//
bool
AosRVGInstGen::registerInstanceGen(const AosRVGInstGenPtr &instGen)
{
	// 
	// Make sure it is not in the list yet
	//
	for (int i=0; i<mRegistered.entries(); i++)
	{
		if (mRegistered[i]->getName() == instGen->getName())
		{
			OmnAlarm << "Instance Generator already registered: " 
				<< instGen->getName() << enderr;
			return false;
		}
	}

	mRegistered.append(instGen);
	return true;
}


// 
// mInstances holds a list of all the ParmInstgen instances
// that were created during the parsing of the configuration
// file (created by CliTorturer). This function retrieves
// the instance whose name is 'name'.
//
AosRVGInstGenPtr
AosRVGInstGen::getInstance(const OmnString &name)
{
	for (int i=0; i<mInstances.entries(); i++)
	{
		if (mInstances[i]->getName() == name)
		{
			return mInstances[i];
		}
	}

	OmnAlarm << "Failed to retrieve InstGen: " << name << enderr;
	return 0;
}


// 
// This function creates an instance of the ParmInstGen
// defined in 'def' and adds it to mInstances. 
// This function should be called by CliTorturer when 
// it parses the configuration file. 
//
AosRVGInstGenPtr
AosRVGInstGen::createInstanceGen(
					const AosCliCmdTorturerPtr &cmd, 
					const OmnXmlItemPtr &def, 
					const OmnString &cmdTag,
					OmnVList<AosGenTablePtr> &tables, 
					const bool codegen)
{
	// 
	// 	<InstanceGen>
	// 		<Name>
	// 		<ClassName>
	// 		<Status>
	// 		<Parms>
	// 			...
	// 		</Parms>
	// 	</InstanceGen>
	//
	OmnString name = def->getStr("Name", "");
	if (name == "")
	{
		OmnAlarm << "Missing Name tag: " << def->toString() << enderr;
		return 0;
	}

	for (int i=0; i<mInstances.entries(); i++)
	{
		if (mInstances[i]->getName() == name)
		{
			OmnAlarm << "Instance Generator: " << name
				<< " has already been defined: " 
				<< def->toString() << enderr;
			return 0;
		}
	}

	// 
	// It is not in the list yet. 
	//
	AosRVGInstGenPtr parm;
	if (codegen)
	{
		parm = OmnNew AosRVGInstGen(cmd, name);
	}
	else
	{
		parm = getRegistered(name);
	}

	if (!parm)
	{
		OmnAlarm << "Instance Generator: " << name
			<< " is not defined: " 
			<< def->toString() << enderr;
		return 0;
	}

	if (!parm->config(cmd, def, cmdTag, tables, codegen))
	{
		OmnAlarm << "Failed to parse the parm: " << def->toString() << enderr;
		return 0;
	}

	mInstances.append(parm);
	return parm;
}


bool 
AosRVGInstGen::newRecordAdded(const AosGenRecordPtr &newRecord) const
{
	if (mInstGen)
	{
		return mInstGen->newRecordAdded(newRecord);
	}

	for (int i=0; i<mParmGens.entries(); i++)
	{
		if (!mParmGens[i]->newRecordAdded(newRecord))
		{
			OmnAlarm << "Failed to add new record" 
				<< mParmGens[i]->getName() << enderr;
			return false;
		}
	}
	return true;
}


bool
AosRVGInstGen::config(
			 const AosCliCmdTorturerPtr &cmd, 
			 const OmnXmlItemPtr &item, 
			 const OmnString &cmdTag, 
			 OmnVList<AosGenTablePtr> &tables, 
			 const bool codegen)
{
	// 
	// There are two cases this function is called. One is to 
	// parse <InstanceGen> tag and the other is a parameter
	// whose type is "InstanceGen". 
	// 
	// 	<InstanceGen>
	// 		<Name>
	// 		<Status>
	// 		<ClassName>
	// 		<StructType>
	// 		<InstGenCode>
	// 		<CppIncludes>
	// 		<HeaderIncludes>
	// 		<DataType>
	// 		<Parms>
	// 			<Parm>
	// 				...
	// 			</Parm>
	// 			...
	// 		</Parms>
	// 	</InstanceGen>
	//
	// 	or 
	// 	<Parm>
	// 		<type>InstanceGen</type>
	// 		...
	// 	</Parm>
	//
	// if (cmdTag == "Parm")
	if (item->getTag() == "Parm")
	{
		return parseParm(item, cmdTag, tables, codegen);
	}

	// mUsageType = eAosRVGUsageType_OutputThrAddr;
	mArgName = item->getStr("Name", "");
	mIsGood = false;
	mDataType = item->getStr("DataType", "");
	mDataType.removeTailWhiteSpace();
	if (mDataType == "")
	{
		OmnAlarm << "DataType tag is null: " << item->toString() << enderr;
		return false;
	}

	mIsPtrDataType = isPtrDataType(mDataType);
	if (mIsPtrDataType)
	{
		// 
		// mDataTypeWithoutPtr is the same as mDataType except that it
		// removes the tailing '*'. 
		//
		mDataTypeWithoutPtr = mDataType;
		mDataTypeWithoutPtr.remove(mDataTypeWithoutPtr.length()-1, 1);
	}

	mFreeMemory = item->getStr("FreeMemory", "");
	mStatus = item->getBool("Status", true);
	if (mArgName == "")
	{
		OmnAlarm << "Missing Name tag: " << item->toString() << enderr;
		return false;
	}

	mClassName = item->getStr("ClassName", "");
	if (mClassName == "")
	{
		OmnAlarm << "Missing ClassName tag: " << item->toString() << enderr;
		return false;
	}

	(mInstGenClassName = mClassName) << "_InstGen";

	mStructType = item->getStr("StructType", "");
//	if (mStructType == "")
//	{
//		OmnAlarm << "Missing StructType tag: " << item->toString() << enderr;
//		return false;
//	}

	mCppIncludes = item->getStr("CppIncludes", "");
	mCppIncludes.removeLeadingWhiteSpace();
	mCppIncludes.removeTailWhiteSpace();
	mHeaderIncludes = item->getStr("HeaderIncludes", "");
	mHeaderIncludes.removeLeadingWhiteSpace();
	mHeaderIncludes.removeTailWhiteSpace();

	mInstGenCode = item->getStr("InstGenCode", "");
	mInstGenCode.removeLeadingWhiteSpace();
	mInstGenCode.removeTailWhiteSpace();

	OmnXmlItemPtr parms = item->getItem("Parms");
	if (!parms)
	{
		OmnAlarm << "Failed to retrieve ParmGens tag: " << item->toString()
			<< enderr;
		return false;
	}

	parms->reset();
	while (parms->hasMore())
	{
		OmnXmlItemPtr parmDef = parms->next();
		AosRVGPtr parm = AosRVG::createRVG(cmd, parmDef, cmdTag, 
							tables, codegen);
		if (!parm)
		{
			OmnAlarm << "Failed to create parm: "
			<< parmDef->toString() << enderr;
			return false;
		} 
		
		mParmGens.append(parm);
	}   

	if (mParmGens.entries() > 0)
	{
		float fff = pow(mCorrectPct / 100.0, 1.0/mParmGens.entries());
		u32 pct = (u32)((fff + 0.005) * 100);
		for (int i=0; i<mParmGens.entries(); i++)
		{
			mParmGens[i]->setCorrectPct(pct);
		}
	}

	mIsGood = true;
	return true;
}


bool
AosRVGInstGen::getCrtValue(AosRVGInstGenPtr &value) const
{
    value = mLastInstGen;
	return true;
}


bool
AosRVGInstGen::getCrtValueAsArg(OmnString &arg, 
								 OmnString &decl, 
								 const u32 argIndex) const
{
	arg = "";
	OmnString str;
	switch (mUsageType)
	{
	case eAosRVGUsageType_InputOnly:
		 if (mIsPtrDataType)
		 {
		 	arg << "&_mArg_" << mArgName;
			if (mVarDecl != "")
			{
				decl << mVarDecl << ";\n";
			}
			else
			{
		 		decl << mStructType << " "
		 	    	<< mDataTypeWithoutPtr
		 			<< " _mArg_" << mArgName << "; ";
			}
		 }
		 else
		 {
		 	if (mVarDecl != "")
			{
				decl << mVarDecl << ";\n";
			}
			else
			{
		 		arg << "_mArg_" << mArgName;
		 		decl << mStructType << " " << mDataType
		 			<< " _mArg_" << mArgName << "; ";
			}
		 }
		 str << "_mArg_" << mArgName;
		 decl << setMemberDataStmt(mInstGenCode, str);
		 break;

	case eAosRVGUsageType_InputOutputThrRef:
		 OmnNotImplementedYet;
		 break;

    case eAosRVGUsageType_OutputThrRef:
		 arg << "_mArg_" << mArgName;
		 if (mVarDecl != "")
		 {
		 	decl << mVarDecl << ";\n";
		 }
		 else
		 {
		 	decl << mStructType << " " << mDataType 
				<< " _mArg_" << mArgName << "; ";
		 }
		 break;

	case eAosRVGUsageType_OutputThrAddr:
		 if (mVarDecl != "")
		 {
		 	decl << mVarDecl << ";\n";
		 }
		 else
		 {
		 	decl << mDataType << " _mArg_" << mArgName << "; ";
		 }
		 arg << "&_mArg_" << mArgName;
		 break;

	case eAosRVGUsageType_InputOutputThrAddr:
		 OmnNotImplementedYet;
		 break;

	default:
		 OmnAlarm << "Unrecognized UsageType: " << mUsageType << enderr;
		 return false;
	}

	return true;
}


static OmnString sgCreationDate = "<$$CreationDate>";
static OmnString sgSourceDirName = "<$$SourceDirectoryName>";
static OmnString sgHeaderFileName = "<$$HeaderFileName>";
static OmnString sgClassName = "<$$ClassName>";
static OmnString sgInstancePointer = "<$$InstancePointer>";
static OmnString sgInstantiation = "<$$Instantiation>";
static OmnString sgInstGenCode = "<$$InstGenCode>";
static OmnString sgMemberDeclaration = "<$$MemberDeclaration>";
static OmnString sgOriginalClassDecl = "<$$OriginalClassDeclaration>";
static OmnString sgGenerateMemberData = "<$$GenerateMemberData>";
static OmnString sgIncludes = "<$$Includes>";
static OmnString sgCreateObject = "<$$CreateObject>";
static OmnString sgInstanceArg = "<$$InstanceArg>";
static OmnString sgSetMemberDataFuncBody = "<$$SetMemberDataFuncBody>";
static OmnString sgDeclNextPtrLocalVar = "<$$DeclNextPtrLocalVar>";


bool
AosRVGInstGen::genCode(const OmnString &srcDir, const bool force)
{
	// 
	// The code will generate the code for the torturer.
	//
	if (!mStatus) return true;

	if (mInstanceGenName != "")
	{
		// 
		// This instance is not <InstanceGen> but a <Parm>
		// that uses an InstanceGen. There is no need to 
		// generate code for <Parm>.
		//
		return true;
	}

	OmnString ptrFile = AosProgramAid_getAosHomeDir();
	ptrFile << "/src/" << srcDir << "/Ptrs.h";

	return createCppFile(srcDir, force) && 
		   createHeaderFile(srcDir, force) && 
		   AosAddSmartPtr("", srcDir, mInstGenClassName, ptrFile);
}


bool
AosRVGInstGen::createCppFile(const OmnString &srcDir, const bool force)
{
	OmnString filename = AosProgramAid_getAosHomeDir(); 
	filename << "/src/" << srcDir;

	filename << "/InstGen_" << mArgName << ".cpp";
	if (force || !OmnFile::fileExist(filename))
	{
		// 
		// Create a new file. This is to copy the file: 
		// Data/ProgramGen/ParmInstGen.cpp 
		//
		OmnString cmd = "cp -f ";
		cmd << AosProgramAid_getAosHomeDir();
		cmd << "/Data/ProgramGen/ParmInstGen.cpp "
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

		OmnString headername;
		headername << "InstGen_" << mArgName << ".h";
		OmnString instname = mInstGenClassName;
		instname << "Ptr";
		bool ret = file.replace(sgCreationDate, OmnGetMDY(), false) &&
			  	   file.replace(sgSourceDirName, srcDir, false) &&
			  	   file.replace(sgHeaderFileName, headername, false) &&
			  	   file.replace(sgClassName, mInstGenClassName, true);

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
	if (!file.isGood())
	{
		OmnAlarm << "Failed to open the file: "
			<< filename << enderr;
		return false;
	}

	// 
	// Insert the include files
	//
	OmnString str;
	OmnString includes = mCppIncludes;
	while (includes != "")
	{
		OmnString line = includes.getLine(true);
		line.removeWhiteSpaces();
		str << "#include \"" << line << "\"\n";
	}
	if (!file.replace(sgIncludes, str, false))
	{
		OmnAlarm << "Failed to replace the Includes section: " 
			<< filename << enderr;
		return false;
	}

	// 
	// Create the Instantiation portion
	//
	str = "";
	bool anyMemberInst = false;
	for (int i=0; i<mParmGens.entries(); i++)
	{
		if (mParmGens[i]->isInputType())
		{
			anyMemberInst = true;
			str << "\n    ";
			str << mParmGens[i]->getDataType() << " mMember" << i << ";\n";
			str << "    if (!mParmGens[" << i << "]->"
				<< mParmGens[i]->getNextValueFuncCall()
				<< "(mMember" << i
				<< ", data, record, __correct, correctOnly, selectFromRecord, rcode, errmsg))\n"
				<< "    {\n"
				<< "        OmnAlarm << \"Failed to generate member: \"\n"
				<< "            << mParmGens[" << i
				<< "]->getName() << enderr;\n"
				<< "        isCorrect = false;\n"
				<< "        return 0;\n"
				<< "    }\n"
				<< "    isCorrect = isCorrect && __correct;\n";
		}
	}

	if (!file.replace(sgInstantiation, str, false))
	{
		OmnAlarm << "Failed to replace the Instantiation section: " 
			<< filename << enderr;
		return false;
	}

	str = "";
	if (anyMemberInst)
	{
		str = "    bool __correct";
	}
	if (!file.replace(sgDeclNextPtrLocalVar, str, false))
	{
		OmnAlarm << "Failed to replace the DeclNextPtrLocalVar section: " 
			<< filename << enderr;
		return false;
	}
		
	OmnString code = mInstGenCode;
	if (code != "" && !code.replace("$Instance.", "inst->", true))
	{
		OmnAlarm << "Failed to replace '$Instance'. Most likely, "
			<< "the code is not entered correctly: " 
			<< mInstGenCode << enderr;
		return false;
	}

	if (!file.replace(sgInstGenCode, code, false))
	{
		OmnAlarm << "Failed to replace the InstGenCode section: " 
			<< filename << enderr;
		return false;
	}

	OmnString setMemStmt = code;
	for (int i=0; i<mParmGens.entries(); i++)
	{
		OmnString value;
		OmnString decl;
		if (!mParmGens[i]->getCrtValueAsArg(value, decl, i))
		{
			OmnAlarm << "Failed to get arg value: " << mParmGens[i]->getName() << enderr;
			return false;
		}

		OmnString name = "mMember";
		name << i;
		setMemStmt.replace(name, value, false);
	}

//	if (!file.replace(sgSetMemberDataFuncBody, setMemStmt, false))
//	{
//		OmnAlarm << "Failed to replace setMemStmt: " << setMemStmt << enderr;
//		return false;
//	}

	// 
	// Construct the GenerateMemberData section
	//
	str = "";
	for (int i=0; i<mParmGens.entries(); i++)
	{
		if (mParmGens[i]->isInputType())
		{
			str << "    if (!mParmGens[" << i
				<< "]->" << mParmGens[i]->getNextValueFuncCall()
				<< "(mMember" << i
				<< ", data, record, isCorrect, correctOnly, selectFromRecord, \n"
				<< "            rcode, errmsg)) \n"
				<< "    {\n"
				<< "        OmnAlarm << \"Failed to generate member: \"\n"
				<< "            << mParmGens[" << i 
				<< "]->getName() << enderr;\n"
				<< "        return 0;\n"
				<< "    }\n";
		}
	}

	if (!file.replace(sgGenerateMemberData, str, false))
	{
		OmnAlarm << "Failed to replace GenerateMemberData section: " 
			<< filename << enderr;
		return false;
	}

	// 
	// Create the "CreateObject" section
	//
	str = "    ";
	str << mStructType << " " 
		<< mDataType << "* inst = OmnNew " << mDataType << ";\n"
		<< "    if (mInstance) OmnDelete mInstance; \n"
		<< "    mInstance = inst;\n"
		<< "    " << mClassName << "_InstGenPtr ptr = (" 
		<< mClassName << "_InstGen *) mInstGen.getPtr();\n";
	if (!file.replace(sgCreateObject, str, false))
	{
		OmnAlarm << "Failed to replace CreateObject section: " 
			<< filename << enderr;
		return false;
	}

	// 
	// Create the "InstanceArg" line
	//
	str = mStructType;
	str << " " << mDataType << "* inst";
	if (!file.replace(sgInstanceArg, str, false))
	{
		OmnAlarm << "Failed to replace InstanceArg section: " 
			<< filename << enderr;
		return false;
	}

	file.flushFileContents();
	file.closeFile();
	return true;
}


bool
AosRVGInstGen::createHeaderFile(const OmnString &srcDir, const bool force)
{
	OmnString filename = AosProgramAid_getAosHomeDir(); 
	filename << "/src/" << srcDir;
	filename << "/" << "InstGen_" << mArgName << ".h";
	if (force || !OmnFile::fileExist(filename))
	{
		// 
		// Create a new file. This is to copy the file: 
		// Data/ProgramGen/ParmInstGen.h
		//
		OmnString cmd = "cp -f ";
		cmd << AosProgramAid_getAosHomeDir();
		cmd << "/Data/ProgramGen/ParmInstGen.h "
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

		OmnString headername = "InstGen_";
		headername << mArgName << "_h";
		OmnString instname = mInstGenClassName;
		instname << "Ptr";
		OmnString srcdir = srcDir;
		srcdir.replace('/', '_');
		bool ret = file.replace(sgCreationDate, OmnGetMDY(), false) &&
			  	   file.replace(sgSourceDirName, srcdir, true) &&
			  	   file.replace(sgHeaderFileName, headername, true) &&
			  	   file.replace(sgClassName, mInstGenClassName, true);

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
	if (!file.isGood())
	{
		OmnAlarm << "Failed to open the file: "
			<< filename << enderr;
		return false;
	}

	// 
	// Create the Original Class Declaration 
	//
	OmnString str;
	if (mStructType != "")
	{
		str = mStructType;
	 	str << " " << mClassName << ";\n";
	}
	if (!file.replace(sgOriginalClassDecl, str, false))
	{
	 	OmnAlarm << "Failed to replace Original Class Declaration: " 
		 	<< filename << enderr;
	 	return false;
	}

	// 
	// Insert the include files
	//
	str = "";
	OmnString includes = mHeaderIncludes;
	while (includes != "")
	{
		OmnString line = includes.getLine(true);
		str << "#include \"" << line << "\"\n";
	}
	if (!file.replace(sgIncludes, str, false))
	{
		OmnAlarm << "Failed to replace the Includes section: " 
			<< filename << enderr;
		return false;
	}

	// 
	// Create Member Data Declaration portion
	//
	str = "";
	for (int i=0; i<mParmGens.entries(); i++)
	{
		str << "    " << mParmGens[i]->getStructType()
			<< " " << mParmGens[i]->getDataType()
			<< " mMember" << i << ";\n";
	}

	if (!file.replace(sgMemberDeclaration, str, false))
	{
		OmnAlarm << "Failed to replace MemberDeclaration: " 
			<< filename << enderr;
		return false;
	}

	// 
	// Create the "InstanceArg" line
	//
	str = mStructType;
	str << " " << mDataType << "* inst";
	if (!file.replace(sgInstanceArg, str, false))
	{
		OmnAlarm << "Failed to replace InstanceArg section: " 
			<< filename << enderr;
		return false;
	}

	// 
	// Create the mInstance declaration
	//
	/*
	str = mClassName;
	str << " * mInstance;\n";
	if (!file.replace(sgInstanceDecl, str, false))
	{
		OmnAlarm << "Failed to replace Instance declaration: " 
			<< filename << enderr;
		return false;
	}
	*/

	file.flushFileContents();
	file.closeFile();
	return true;
}


OmnString 
AosRVGInstGen::getNextValueFuncCall() const
{
	return "nextInst";
}


bool
AosRVGInstGen::genAllCode(const OmnString &dir, const bool force)
{
	for (int i=0; i<mInstances.entries(); i++)
	{
		if (mInstances[i]->getStatus() &&
			mInstances[i]->getInstanceGenName() == "")
		{
			if (!mInstances[i]->genCode(dir, force))
			{
				OmnAlarm << "Failed to generate code" << enderr;
				return false;
			}
		}
	}

	return true;
}


OmnString
AosRVGInstGen::genInstRegisterCode()
{
	// 
	// These are the registration statements for each InstanceGen.
	// These statements are inserted into the torturer's main(...).
	// The format is:
	// 		classname::registerInstanceGen(InstanceGenName);
	//
	OmnString str;
	for (int i=0; i<mInstances.entries(); i++)
	{
		if (mInstances[i]->getStatus() &&
			mInstances[i]->getInstanceGenName() == "")
		{
			str << "    " << mInstances[i]->getClassName()
				<< "_InstGen::registerInstanceGen(\""
				<< mInstances[i]->getName()
				<< "\");\n";
		}
	}

	return str;
}


OmnString
AosRVGInstGen::genInstGenIncludes(
					const OmnString &srcDir, 
					const OmnString &testDir)
{
	// 
	// Each InstanceGen is defined as a class. The files are stored
	// mSrcDirName/mTestDirName. This function creates all
	// the include files. 
	//
	OmnString str;
	OmnString dirname = srcDir;
	if (testDir != "")
	{
		dirname << "/" << testDir;
	}
	dirname << "/";
	for (int i=0; i<mInstances.entries(); i++)
	{
		if (mInstances[i]->getStatus() &&
			mInstances[i]->getInstanceGenName() == "")
		{
			str << "#include \"" << dirname 
				<< "InstGen_"
				<< mInstances[i]->getName() 
				<< ".h\"\n";
		}
	}

	return str;
}


bool
AosRVGInstGen::parseParm(const OmnXmlItemPtr &item, 
			 const OmnString &cmdTag, 
			 OmnVList<AosGenTablePtr> &tables, 
			 const bool codegen)
{
	// 
	// 	<Parm>
	// 		<type>
	// 		<Name>
	// 		<InstanceGenName>
	// 		<SmartPtr>
	// 		<DataType>
	// 		<FieldName>
	// 		<FreeMemory>
	// 	</Parm>
	//
	if (!AosRVG::config(item, cmdTag, tables))
	{
		OmnAlarm << "Failed to parse InstanceGen: " 
			<< item->toString() << enderr;
		return false;
	}
	mFreeMemory = item->getStr("FreeMemory", "");
	if (mFreeMemory != "" && mFreeMemory != "free" && mFreeMemory != "delete")
	{
		OmnAlarm << "Invalid FreeMemory tag: " << item->toString()
			<< ". The allowed values are 'free' or 'delete'. " << enderr;
		return false;
	}

	mInstanceGenName = item->getStr("InstanceGenName", "");
	if (mInstanceGenName == "")
	{
		OmnAlarm << "Missing InstanceGenName tag: " 
			<< item->toString() << enderr;
		return false;
	}

	mIsPtrDataType = isPtrDataType(mDataType);
	if (mIsPtrDataType)
	{
		// 
		// mDataTypeWithoutPtr is the same as mDataType except that it
		// removes the tailing '*'. 
		//
		mDataTypeWithoutPtr = mDataType;
		mDataTypeWithoutPtr.remove(mDataTypeWithoutPtr.length()-1, 1);
	}

	mIsGood = true;
	return true;
}


bool 
AosRVGInstGen::getDeleteStmt(OmnString &stmt, const u32 argIndex) const
{
	if (mFreeMemory == "") return true;

	stmt << mFreeMemory << "(arg" << argIndex << "); ";
	return true;
}


bool
AosRVGInstGen::nextStr(
				OmnString &value,
				const AosGenTablePtr &table, 
				const AosGenRecordPtr &record, 
				bool &isCorrect, 
				const bool correctOnly,
				const bool selectFromRecord,
				AosRVGReturnCode &rcode,
				OmnString &errmsg)
{
	if (!mInstGen)
	{
		OmnAlarm << "mInstGen is null" << enderr;
		return false;
	}

	mInstGen->setKeyTypeToParms(mKeyType);
	return mInstGen->nextPtr(table, record, isCorrect, 
			correctOnly, selectFromRecord, rcode, errmsg);
}


bool 
AosRVGInstGen::nextPtr(const AosGenTablePtr &data,
		                 const AosGenRecordPtr &record,
						 bool &isCorrect,
						 const bool correctOnly,
						 const bool selectFromRecord,
						 AosRVGReturnCode &rcode,
						 OmnString &errmsg)
{
	OmnAlarm << "Function should not be called here" << enderr;
	return false;
}


AosRVGInstGenPtr
AosRVGInstGen::clone() const
{
	AosRVGInstGenPtr ptr = OmnNew AosRVGInstGen(mCommand, mArgName);
	copyData(ptr.getPtr());
	return ptr;
}


void
AosRVGInstGen::copyData(AosRVGInstGen *ptr) const
{
	ptr->mClassName = mClassName;
	ptr->mInstGenClassName = mInstGenClassName;
	ptr->mInstGenCode = mInstGenCode;
	ptr->mStructType = mStructType;
	ptr->mCppIncludes = mCppIncludes;
	ptr->mInstanceGenName = mInstanceGenName;
	ptr->mStatus = mStatus;
	ptr->mFreeMemory = mFreeMemory;
	ptr->mKeyType = mKeyType;
	ptr->mFieldName = mFieldName;
	ptr->mDataType = mDataType;
	ptr->mCorrectPct = mCorrectPct;
	ptr->mCorrectOnly = mCorrectOnly;
	ptr->mTable = mTable;
	ptr->mDependedTables = mDependedTables;
	ptr->mDependedFieldNames = mDependedFieldNames;
	ptr->mIsSmartPtr = mIsSmartPtr;
	ptr->mUsageType = mUsageType;
	ptr->mInstGenName = mInstGenName;
	ptr->mLastValue = mLastValue;
	ptr->mParmGens = mParmGens;
	ptr->mIsPtrDataType = mIsPtrDataType;
}


bool
AosRVGInstGen::isPtrDataType(const OmnString &datatype) const
{
	// 
	// It returns true if 'datatype' ends with '*'. Otherwise, it returns false.
	//
	if (datatype == "") return false;
	const char *data = datatype.data();
	return data[datatype.length()-1] == '*';
}


OmnString
AosRVGInstGen::setMemberDataStmt(const OmnString &s, const OmnString &arg) const
{
	// 
	// instance->memberData = value; 
	// ...
	//
	if (mInstGen)
	{
		return mInstGen->setMemberDataStmt(s, arg);
	}

	OmnString stmt = s;
	if (!stmt.replace("$Instance", arg, true))
	{
		OmnAlarm << "Failed to replace $Instance: " 
			<< stmt 
			<< ". Make sure the instance is referred by the name '$Instance'"
			<< enderr;
		return "";
	}
		
	for (int i=0; i<mParmGens.entries(); i++)
	{
        OmnString value;
		OmnString decl;
		if (!mParmGens[i]->getCrtValueAsArg(value, decl, i))
		{
			OmnAlarm << "Failed to get arg value: " << i << enderr;
			return false;
		}
		
		OmnString name = "mMember";
		name << i;
		if (!stmt.replace(name, value, false))
		{
			OmnAlarm << "Failed to replace: " << i
				<< "Name: " << name << ". Value: " << value
				<< "\n" << stmt << enderr;
			return false;
		}
	}

	return stmt;
}


void		
AosRVGInstGen::setKeyTypeToParms(const AosRVGKeyType k)
{
	for (int i=0; i<mParmGens.entries(); i++)
	{
		mParmGens[i]->setKeyType(k);
	}
}

	


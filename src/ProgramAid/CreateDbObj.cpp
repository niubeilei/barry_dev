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
// 3/19/2007: Created by cding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "ProgramAid/CreateDbObj.h"

#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Porting/GetTime.h"
#include "ProgramAid/Ptrs.h"
#include "ProgramAid/Field.h"
#include "ProgramAid/Util.h"
#include "Util/File.h"
#include "Util/StrParser.h"
#include "XmlParser/XmlParser.h"
#include "XmlParser/XmlItem.h"




AosCreateDbObj::AosCreateDbObj()
{
}


AosCreateDbObj::AosCreateDbObj(const OmnString &configFilename)
:
mConfigFileName(configFilename)
{
}


AosCreateDbObj::AosCreateDbObj(const OmnString &dirname, 
				   const OmnString &projPrefix,
				   const OmnString &srcFileExt,
				   const OmnString &configFilename, 
				   const bool force)
:
mSrcDirName(dirname),
mProjPrefix(projPrefix),
mSrcFileExt(srcFileExt),
mConfigFileName(configFilename),
mForce(force)
{
}


AosCreateDbObj::~AosCreateDbObj()
{
}


bool
AosCreateDbObj::config()
{
	// 
	// 	<DBObjectDef>
	// 		<ProjPrefix>
	// 		<SrcDirName>
	// 		<SrcFileExt>
	// 		<DBObjects>
	// 			<DBObject>
	// 				<TableName>
	// 				<ClassName>
	// 				<Fields>	
	// 					<Field>field_name type name default key</Field>
	// 					<Field>field_name type name default </Field>
	// 					...
	// 					<Field>field_name type name default </Field>
	//				</Fields>
	//			</DBObject>
	//			<DBObject>
	//			...
	//			</DBObject>
	//			...
	//		</DBObjects>
	//	</DBObjectDef>
	//
    OmnXmlParser parser;
	if (!parser.readFromFile(mConfigFileName))
	{
		OmnAlarm << "Failed to read the configure file: "
			<< mConfigFileName << enderr;
		return false;
	}

	//
	// Retrieve Source Code Directory Name
	//
	OmnXmlItemPtr item = parser.nextItem();
	if (!item)
	{
		OmnAlarm << "Failed to retrieve the contents: "
		    << mConfigFileName << enderr;
		return false;
	}
	
	if (mProjPrefix == "")
	{
		mProjPrefix = item->getStr("ProjPrefix", "Aos");
	}

	if (mSrcFileExt == "")
	{
		mSrcFileExt = item->getStr("SrcFileExt", "cpp");
	}

	if (mSrcDirName == "")
	{
		mSrcDirName = item->getStr("SrcDirName", "");
		if (mSrcDirName == "")
		{
			OmnAlarm << "Missing SrcDirName tag: " 
				<< mConfigFileName << enderr;
			return false;
		}
	}

	OmnXmlItemPtr objs = item->getItem("DBObjects");
	if (!objs)
	{
		OmnAlarm << "No objects defined" << enderr;
		return false;
	}

	objs->reset();
	while (objs->hasMore())
	{
		OmnXmlItemPtr obj = objs->next();
		if (!createOneObject(obj))
		{
			OmnAlarm << "Failed to create the object: "
				<< mClassName << enderr;
			return false;
		}

		mMemData.clear();
	}

	return true;
}


bool
AosCreateDbObj::createOneObject(const OmnXmlItemPtr &def)
{
	mClassName = def->getStr("ClassName", "");
	if (mClassName == "")
	{
		OmnAlarm << "Failed to retrieve table name: " 
			<< mConfigFileName << enderr;
		return false;
	}

	mTableName = def->getStr("TableName", "");
	if (mTableName == "")
	{
		OmnAlarm << "Failed to retrieve table name: " 
			<< mConfigFileName << enderr;
		return false;
	}

	OmnXmlItemPtr fields = def->getItem("Fields");
	if (!fields)
	{
		OmnAlarm << "Missing field definition: " << mConfigFileName << enderr;
		return false;
	}

	fields->reset();
	while (fields->hasMore())
	{
		OmnXmlItemPtr field = fields->next();
		OmnString def = field->getStr();
		OmnStrParser parser(def);
		OmnString fieldName  = parser.getValue("fn", "");
		OmnString typeName   = parser.getValue("type", "");
		OmnString memberName = parser.getValue("name", "");
		OmnString dflt       = parser.getValue("dflt", "");
		OmnString key        = parser.getValue("key", "no");

		if (fieldName == "" || typeName == "" || memberName == "") 
		{
			OmnAlarm << "Invalid field definition: " 
				<< fieldName << ":" << typeName << ":" << memberName << enderr;
			return false;
		}

		AosFieldPtr fptr= OmnNew AosField(fieldName, typeName, memberName, 
			key, dflt);
		mMemData.append(fptr);
	}

	return generateCode();
}


bool
AosCreateDbObj::generateCode()
{
	OmnString ptrFile = mSrcDirName; 
	ptrFile << "/Ptrs.h";
	return generateHeaderFile() && 
		   generateSourceFile() &&
		   AosAddSmartPtr(mProjPrefix, 
		   		mSrcDirName, mClassName, ptrFile);
}


const OmnString sgCreationDate = "<$$CreationDate>";
const OmnString sgCreatorName = "<$$CreatorName>";
const OmnString sgProjectPrefix= "<$$ProjectPrefix>";
const OmnString sgSrcDirName = "<$$SourceDirectoryName>";
const OmnString sgHeaderFileName = "<$$HeaderFileName>";
const OmnString sgClassName = "<$$ClassName>";

const OmnString sgMemberDataDeclaration = "<$$MemberDataDeclaration>";
const OmnString sgPrimaryKeyParmDecl = "<$$PrimaryKeyParmDecl>";
const OmnString sgGetMemDataPortion = "<$$GetMemberDataPortion>";
const OmnString sgSetMemDataPortion = "<$$SetMemberDataPortion>";
const OmnString sgAdditionalHeaderInHeader = "<$$AdditionalHeaderInHeader>";
const OmnString sgResetStatements = "<$$ResetStatements>";
const OmnString sgToStringStatements = "<$$ToStringStatements>";
const OmnString sgCloneStatements = "<$$CloneStatements>";
const OmnString sgSerializeFromDbSqlStatement = "<$$SerializeFromDbSqlStatement>";
const OmnString sgUpdateStatement = "<$$UpdateStatement>";
const OmnString sgRemoveStatement = "<$$RemoveStatement>";
const OmnString sgRemoveAllStatement = "<$$RemoveAllStatement>";
const OmnString sgExistStatement = "<$$ExistStatement>";
const OmnString sgInsertStatement = "<$$InsertStatement>";
const OmnString sgSerializeFromRecordStatements = "<$$SerializeFromRecordStatements>";
const OmnString sgPrimaryKeyParms = "<$$PrimaryKeyParms>";
const OmnString sgKeyMemDataInitializers = "<$$KeyMemDataInitializers>";
 


bool
AosCreateDbObj::generateHeaderFile()
{

	OmnString filename = "./";
	filename << mClassName << ".h";
	
	if (!mForce && OmnFile::fileExist(filename))
	{
		//
		// The file is there. Do nothing.
		// 
		return true;
	}

	// 
	// Copy the template file to the new location
	//
	OmnString cmd = "cp ";
	cmd << AosProgramAid_getAosHomeDir() 
		<< "/Data/ProgramGen/DbObj.h ./" << filename;

	OmnTrace << "To run command: " << cmd << endl;
	system(cmd.data());

	// 
	// Open the Makefile template
	//
	OmnFile file(filename, OmnFile::eReadOnly);
	if (!file.isGood())
	{
		OmnAlarm << "Failed to open the file: " 
			<< filename << enderr;
		return false;
	}

	OmnString username = getenv("USER");
	if (!file.replace(sgCreationDate, OmnGetMDY(), false))
	{
		OmnAlarm << "Failed to replace the creation date" << enderr;
		return false;
	}

	if (!file.replace(sgCreatorName, username, false))
	{
		OmnAlarm << "Failed to replace the creation date" << enderr;
		return false;
	}

	if (!file.replace(sgProjectPrefix, mProjPrefix, true))
	{
		OmnAlarm << "Failed to replace the creation date" << enderr;
		return false;
	}

	OmnString dir = mSrcDirName;
	dir.substitute('/', '_', true);
	if (!file.replace(sgSrcDirName, dir, true))
	{
		OmnAlarm << "Failed to replace the creation date" << enderr;
		return false;
	}

	if (!file.replace(sgHeaderFileName, mClassName, true))
	{
		OmnAlarm << "Failed to replace the creation date" << enderr;
		return false;
	}

	if (!file.replace(sgAdditionalHeaderInHeader, getAdditionHeaderInHeader(), true))
	{
		OmnAlarm << "Failed to replace the additionalHeaderInHeader" << enderr;
		return false;
	}

	OmnString classname = mProjPrefix;
	classname << mClassName;
	if (!file.replace(sgClassName, classname, true))
	{
		OmnAlarm << "Failed to replace the creation date" << enderr;
		return false;
	}

	// 
	// Create the member data declaration section
	//
	u32 length = 0;
	for (int i=0; i<mMemData.entries(); i++)
	{
		u32 len = mMemData[i]->getMemDataTypeName().length();
		if (len > length) length = len;
	}

	length = length / 4 * 4 + 8;

	OmnString memDataDecl;
	for (int i=0; i<mMemData.entries(); i++)
	{
		OmnString line(length, ' ', true);
		OmnString tt = mMemData[i]->getMemDataTypeName();
		line.replace(4, tt.length(), tt);
		line << "m" << mMemData[i]->getMemDataName(true) << ";\n";
		memDataDecl << line;
	}

	file.replace(sgMemberDataDeclaration, memDataDecl, false);

	// 
	// Create the PrimaryKeyParmDecl
	//
	/*
	OmnString keyParmDecl;
	for (int i=0; i<mMemData.entries(); i++)
	{
		if (mMemData[i]->isKey())
		{
			if (keyParmDecl != "") keyParmDecl << ", ";

			keyParmDecl << "const " 
				<< mMemData[i]->getMemDataTypeName() 
				<< " &" 
				<< mMemData[i]->getMemDataName(false);
		}
	}
	*/

	file.replace(sgPrimaryKeyParmDecl, getPrimaryKeyDecl(), true);

	// 
	// Create the Get's section. 
	//
	OmnString gets = "\n";
	for (int i=0; i<mMemData.entries(); i++)
	{
		gets << "    "
			 << mMemData[i]->getMemDataTypeName()
		  	 << "   get" 
			 << mMemData[i]->getMemDataName(true)
			 << "() const {return m"
			 << mMemData[i]->getMemDataName(true)
			 << ";}\n";
	}

	file.replace(sgGetMemDataPortion, gets, false);

	// 
	// Create the Set's section.
	//
	OmnString sets = "\n";
	for (int i=0; i<mMemData.entries(); i++)
	{
		sets << "    void    set"
			 << mMemData[i]->getMemDataName(true)
			 << "(const "
			 << mMemData[i]->getMemDataTypeName()
			 << " &" 
			 << mMemData[i]->getMemDataName(false)
			 << ") { m"
			 << mMemData[i]->getMemDataName(true)
			 << " = "
			 << mMemData[i]->getMemDataName(false)
			 << ";}\n";
	}
	file.replace(sgSetMemDataPortion, sets, false);

	file.flushFileContents();
	return true;
}


bool
AosCreateDbObj::generateSourceFile()
{
	OmnString filename = "./";
	filename << "/" << mClassName << "." << mSrcFileExt;
	
	if (!mForce && OmnFile::fileExist(filename))
	{
		//
		// The file is there. Do nothing.
		// 
		return true;
	}

	// 
	// Copy the template file to the new location
	//
	OmnString cmd = "cp ";
	cmd << AosProgramAid_getAosHomeDir() 
		<< "/Data/ProgramGen/DbObj.cpp " << filename;

	OmnTrace << "To run command: " << cmd << endl;
	system(cmd.data());

	// 
	// Open the Makefile template
	//
	OmnFile file(filename, OmnFile::eReadOnly);
	if (!file.isGood())
	{
		OmnAlarm << "Failed to open the file: " 
			<< filename << enderr;
		return false;
	}

	OmnString username = getenv("USER");
	OmnString headerFilename = mClassName;
	headerFilename << ".h";
	OmnString classname = mProjPrefix;
	classname << mClassName;
	bool ret = file.replace(sgCreationDate, OmnGetMDY(), false) &&
			   file.replace(sgCreatorName, username, false) &&
			   file.replace(sgSrcDirName, mSrcDirName, false) &&
			   file.replace(sgHeaderFileName, headerFilename, false) &&
			   file.replace(sgClassName, classname, true) &&
			   file.replace(sgPrimaryKeyParmDecl, getPrimaryKeyDecl(), true) &&
			   file.replace(sgPrimaryKeyParms, getPrimaryKeyParms(), true) &&
			   addClassId();

	if (!ret)
	{
		OmnAlarm << "Failed to create the file" << enderr;
		return false;
	}
	
	ret = createResetFunc(file) &&
		  createToStringFunc(file) &&
		  createCloneFunc(file) &&
		  createSerializeFromDb(file) &&
		  createUpdateFunc(file) &&
		  createRemoveFunc(file) &&
		  createRemoveAllFunc(file) &&
		  createExistFunc(file) &&
		  createInsertFunc(file) &&
		  createSerializeFromRecordFunc(file) &&
		  createMemDataInitializer(file);

	
	if (!ret)
	{
		OmnAlarm << "Failed to create member functions" << enderr;
		return false;
	}

	file.flushFileContents();
	return true;
}


bool
AosCreateDbObj::createResetFunc(OmnFile &file) const
{
	// 
	// Create the reset function
	//
	OmnString resets;
	for (int i=0; i<mMemData.entries(); i++)
	{
		resets << "    " << mMemData[i]->getMemDataName()
			<< " = " 
			<< mMemData[i]->getDefaultValue() 
			<< ";\n";
	}

	if (!file.replace(sgResetStatements, resets, false))
	{
		OmnAlarm << "Failed to create the reset member function" << enderr;
		return false;
	}

	return true;
}


bool
AosCreateDbObj::createToStringFunc(OmnFile &file) const
{
	// 
	// Create ::toString()
	//
	OmnString tostring;
	tostring << "    str << "
		<< "\"Class " << mProjPrefix << mClassName << ":\"";
	for (int i=0; i<mMemData.entries(); i++)
	{
		tostring << "\n        << \"    " 
			<< mMemData[i]->getMemDataName()
			<< ":   \" << " 
			<< mMemData[i]->getMemDataName();
	}
	tostring << ";\n";

	if (!file.replace(sgToStringStatements, tostring, false))
	{
		OmnAlarm << "Failed to create toString()" << enderr;
		return false;
	}

	return true;
}


bool
AosCreateDbObj::createCloneFunc(OmnFile &file) const
{
	// 
	// Create ::clone()
	//
	OmnString cloneStmts;
	cloneStmts << "    "
		<< getClassName() 
		<< "Ptr obj = OmnNew "
		<< getClassName()
		<< "();\n"
		<< "    obj.setDelFlag(false);\n";
	for (int i=0; i<mMemData.entries(); i++)
	{
		cloneStmts << "    obj->"
			<< mMemData[i]->getMemDataName()
			<< " = " 
			<< mMemData[i]->getMemDataName()
			<< ";\n";
	}

	if (!file.replace(sgCloneStatements, cloneStmts, false))
	{
		OmnAlarm << "Failed to create clone()" << enderr;
		return false;
	}
	
	return true;
}


bool
AosCreateDbObj::createSerializeFromDb(OmnFile &file) const
{
	OmnString stmt;
	stmt << "    OmnString stmt = \"select * from "
		<< mTableName 
		<< " where \";\n"
		<< "    stmt << ";
	
	bool first = true;
	OmnString prevSingleQuote;
	for (int i=0; i<mMemData.entries(); i++)
	{
		if (mMemData[i]->isKey())
		{
			if (first)
			{
				stmt << "\n        \"" << mMemData[i]->getFieldName();
				first = false;
			}
			else
			{
				stmt << "\n        << \"" << prevSingleQuote
					<< ", "
					<< mMemData[i]->getFieldName();
			}

			stmt	<< "=" 
				<< mMemData[i]->getSingleQuote()
				<< "\" << "
				<< mMemData[i]->getMemDataName();
			prevSingleQuote = mMemData[i]->getSingleQuote();
		}
	}

	stmt << ";\n";

	if (!file.replace(sgSerializeFromDbSqlStatement, stmt, false))
	{
		OmnAlarm << "Failed to create serializeFromDb()" << enderr;
		return false;
	}

	return true;
}

	
OmnString
AosCreateDbObj::getAdditionHeaderInHeader() const
{
	OmnString headers;

	headers << "#include \"" << mSrcDirName << "/Ptrs.h\"\n";

	for (int i=0; i<mMemData.entries(); i++)
	{
		OmnString header = mMemData[i]->getHeader();
		if (header != "")
		{
			if (headers.findSubString(header, 0) < 0)
			{
				headers << header << "\n";
			}
		}
	}

	return headers;
}



/*
bool
AosCreateDbObj::generatePtrFile()
{

	OmnString filename = "./Ptrs.h";
	
	if (!OmnFile::fileExist(filename))
	{
		if (!AosCreatePtrFile(
				 mProjPrefix,
				 mSrcDirName,
				 filename))
		{
			OmnAlarm << "Failed to create Ptrs.h" << enderr;
			return false;
		}
	}

	// 
	// Open the Makefile template
	//
	OmnFile file(filename, OmnFile::eReadOnly);
	if (!file.isGood())
	{
		OmnAlarm << "Failed to open the file: " 
			<< filename << enderr;
		return false;
	}

	OmnString decl = "OmnPtrDecl(";
	decl << mProjPrefix 
		 << mClassName
		 << ", "
		 << mProjPrefix
		 << mClassName
		 << "Ptr)\n";
	
	if (file.find(decl) < 0)
	{
		// 
		// It is not in the file. Need to insert it into the file
		//
		decl << "#endif";
		if (!file.replace("#endif", decl, false))
		{
			OmnAlarm << "Failed to insert the declaration: " << filename << enderr;
			return false;
		}

		file.flushFileContents();
	}

	return true;
}
*/


OmnString
AosCreateDbObj::getClassName() const
{
	OmnString cn = mProjPrefix;
	cn << mClassName;
	return cn;
}


bool
AosCreateDbObj::createUpdateFunc(OmnFile &file) const
{
	// 
	// Create the reset function
	//
	OmnString stmt;
	stmt << "    OmnString stmt = \"update "
		<< mTableName 
		<< " set \";\n"
		<< "    stmt ";

	OmnString preSingleQuote;
	bool first = true;
	for (int i=0; i<mMemData.entries(); i++)
	{
		stmt << "\n        << \"" 
			<< preSingleQuote;
		if (!first)
		{
			stmt << ", ";
		}
		
		stmt << mMemData[i]->getFieldName()
			<< " = " 
			<< mMemData[i]->getSingleQuote() 
			<< "\" << "
			<< mMemData[i]->getMemDataName();

		preSingleQuote = mMemData[i]->getSingleQuote();
	}
	stmt << ";\n";

	if (!file.replace(sgUpdateStatement, stmt, false))
	{
		OmnAlarm << "Failed to create the update member function" << enderr;
		return false;
	}

	return true;
}


bool
AosCreateDbObj::createRemoveFunc(OmnFile &file) const
{
	OmnString stmt;
	stmt << "    OmnString stmt = \"delete from "
		<< mTableName;
	stmt << createWhereClause();

	if (!file.replace(sgRemoveStatement, stmt, false))
	{
		OmnAlarm << "Failed to create the remove member function" << enderr;
		return false;
	}

	return true;
}


OmnString
AosCreateDbObj::createWhereClause() const
{
	OmnString stmt;
	stmt << " where \";\n"
		<< "    stmt ";

	OmnString preSingleQuote;
	bool first = true;
	for (int i=0; i<mMemData.entries(); i++)
	{
		if (!mMemData[i]->isKey())
		{
			continue;
		}

		stmt << "\n        << \"" 
			<< preSingleQuote;
		if (!first)
		{
			stmt << " and ";
		}
		
		stmt << mMemData[i]->getFieldName()
			<< " = " 
			<< mMemData[i]->getSingleQuote() 
			<< "\" << "
			<< mMemData[i]->getMemDataName();

		preSingleQuote = mMemData[i]->getSingleQuote();
	}
	stmt << ";\n";

	return stmt;
}



bool
AosCreateDbObj::createRemoveAllFunc(OmnFile &file) const
{
	OmnString stmt;
	stmt << "    OmnString stmt = \"delete from "
		<< mTableName 
		<< "\";\n";

	if (!file.replace(sgRemoveAllStatement, stmt, false))
	{
		OmnAlarm << "Failed to create the remove member function" << enderr;
		return false;
	}

	return true;
}


bool
AosCreateDbObj::createExistFunc(OmnFile &file) const
{
	OmnString stmt;
	stmt << "    OmnString stmt = \"select * from "
		<< mTableName 
		<< createWhereClause();

	if (!file.replace(sgExistStatement, stmt, false))
	{
		OmnAlarm << "Failed to create the exist member function" << enderr;
		return false;
	}

	return true;
}


bool
AosCreateDbObj::createInsertFunc(OmnFile &file) const
{
	// 
	// Create the reset function
	//
	OmnString stmt;
	stmt << "    OmnString stmt = \"insert into "
		<< mTableName 
		<< " (\"";

	for (int i=0; i<mMemData.entries(); i++)
	{
		stmt << "\n        \""
		 	 << mMemData[i]->getFieldName();
		if (i == mMemData.entries()-1)
		{
		//	stmt << "\"";
		}
		else
		{
			stmt << ", \"";
		}
	}

	stmt << ") values (\";\n\n" << "    stmt ";

	for (int i=0; i<mMemData.entries()-1; i++)
	{
		stmt << "        << " 
		    << mMemData[i]->getMemDataName()
			<< " << \"" 
			<< mMemData[i]->getSingleQuote() 
			<< ", "
			<< mMemData[i+1]->getSingleQuote()
			<< "\"\n";
	}

	stmt << "        << " 
		<< mMemData[mMemData.entries()-1]->getMemDataName()
		<< "  << \""
		<< mMemData[mMemData.entries()-1]->getSingleQuote()
		<< ")\";\n";

	if (!file.replace(sgInsertStatement, stmt, false))
	{
		OmnAlarm << "Failed to create the insert member function" << enderr;
		return false;
	}
	return true;
}


bool
AosCreateDbObj::createSerializeFromRecordFunc(OmnFile &file) const
{
	// 
	// Create the reset function
	//
	OmnString stmt;

	int idx = 0;
	for (int i=0; i<mMemData.entries(); i++)
	{
		stmt << "    " << mMemData[i]->getMemDataName()
			<< " = record->get"
			<< mMemData[i]->getRecordTypeName()
			<< "(" << idx
			<< ", "
			<< mMemData[i]->getDefaultValue()
			<< ", rslt);\n";
	}

	if (!file.replace(sgSerializeFromRecordStatements, stmt, false))
	{
		OmnAlarm << "Failed to create the SerializeFromRecord function" << enderr;
		return false;
	}

	return true;
}


/*
bool
AosCreateDbObj::createRetrieveFromDbFunc(OmnFile &file) const
{
	// 
	// Create the reset function
	//
	OmnString stmt;
	stmt << "    " 
		<< mProjPrefix 
		<< mClassName 
		<< " user = OmnNew "
		<< mProjPrefix 
		<< mClassName
		<< "("
		<< getPrimaryKeyDecl()
		<< ");\n";

	if (!file.replace(sgSerializeFromRecordStatements, stmt, false))
	{
		OmnAlarm << "Failed to create the SerializeFromRecord function" << enderr;
		return false;
	}

	return true;
}
*/


OmnString
AosCreateDbObj::getPrimaryKeyDecl() const
{
	OmnString keyParmDecl;
	for (int i=0; i<mMemData.entries(); i++)
	{
		if (mMemData[i]->isKey())
		{
			if (keyParmDecl != "") keyParmDecl << ", ";

			keyParmDecl << "const " 
				<< mMemData[i]->getMemDataTypeName() 
				<< " &" 
				<< mMemData[i]->getMemDataName(false);
		}
	}

	return keyParmDecl;
}


OmnString
AosCreateDbObj::getPrimaryKeyParms() const
{
	OmnString keyParms;
	for (int i=0; i<mMemData.entries(); i++)
	{
		if (mMemData[i]->isKey())
		{
			if (keyParms != "") keyParms << ", ";

			keyParms << mMemData[i]->getMemDataName(false);
		}
	}

	return keyParms;
}
	

bool
AosCreateDbObj::createMemDataInitializer(OmnFile &file) const
{
	OmnString stmt;
	int prev = -1;
	for (int i=0; i<mMemData.entries(); i++)
	{
		if (mMemData[i]->isKey())
		{
			if (prev >= 0)
			{
				stmt << "m" << mMemData[prev]->getMemDataName(true)
					<< "("
					<< mMemData[prev]->getMemDataName(false)
					<< "),\n";
			}
			prev = i;
		}
	}

	if (prev >= 0)
	{
		stmt << "m" << mMemData[prev]->getMemDataName(true)
			<< "("
			<< mMemData[prev]->getMemDataName(false)
			<< ")\n";
	}

	if (!file.replace(sgKeyMemDataInitializers, stmt, false))
	{
		OmnAlarm << "Failed to create the KeyMemInitialize" << enderr;
		return false;
	}

	return true;
}


bool
AosCreateDbObj::addClassId() const
{
	OmnString classId;


	OmnString filename = AosProgramAid_getAosHomeDir();
	filename << "/src/Obj/ClassId.h";

	// 
	// Open the template
	//
	OmnFile file(filename, OmnFile::eReadOnly);
	if (!file.isGood())
	{
		OmnAlarm << "Failed to open the file: " 
			<< filename << enderr;
		return false;
	}

	classId << "e" << mProjPrefix << mClassName;
	if (file.find(classId) < 0)
	{
		// 
		// It is not in the file. Need to insert it into the file
		//
		classId = "        e";
		classId << mProjPrefix << mClassName
			    << ",\n\n        eLastValidEntry";
		if (!file.replace("\n        eLastValidEntry", classId, false))
		{
			OmnAlarm << "Failed to insert the class ID: " << filename << enderr;
			return false;
		}

		file.flushFileContents();
	}

	return true;
}
#endif



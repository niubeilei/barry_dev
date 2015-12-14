////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: main.cpp
// Description:
//   
//
// Modification History:
// 03/15/2007 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////

#include "Alarm/Alarm.h"
#include "aosUtil/Tracer.h"
#include "AppMgr/App.h"
#include "Debug/Debug.h"
#include "Porting/ThreadDef.h"
#include "Porting/Sleep.h"
#include "ProgramAid/CreateClassFiles.h"
#include "SingletonClass/SingletonMgr.h"
#include "Util/OmnNew.h"
#include "Util/IpAddr.h"

#include <stdio.h>
// #include <readline/readline.h>
// #include <readline/history.h>
#include <stdlib.h>
#include <stdlib.h>

void printusage(void)
{
	cout << "Usage: ProgAid -dir <SrcDirName> -classname <ClassName> [-homedir <ProjHomeDir>] [-sptr] [-force] [-ext <FileExt>]" << endl;
	cout << "where: " << endl;
	cout << "    -dir:       The source code directory name in which the files are created" << endl;
	cout << "    -classname: The name of the class to be crated" << endl;
	cout << "    -homedir:   The project home directory name, such as \"/home/cding/AOS\"" << endl;
	cout << "                This is optional. If you defined the environment " << endl;
	cout << "                variable: AOS_HOME, it will use it." << endl;
	cout << "    -sptr:      Whether to use the smart pointer for the class. Default: no." << endl;
	cout << "    -force:     If specified, it will override the source files, if they exist. " << endl;
	cout << "    -ext:       The source code file extension. The default is \".cpp\"" << endl;
	cout << "    -help:      To print this menu." << endl;
	return;
}


void printCreateDbObjectUsage(void)
{
	cout << "Usage: ProgAid -dir <SrcDirName> -fname <FileName> [-homedir <ProjHomeDir>] [-force]" << endl;
	cout << "where: " << endl;
	cout << "    -dir:       The source code directory name in which the class is created" << endl;
	cout << "    -fname:     The configuration file name." << endl;
	cout << "    -classname: The class name." << endl;
	cout << "    -homedir:   The project home directory name, such as \"/home/cding/AOS\"" << endl;
	cout << "                This is optional. If you defined the environment " << endl;
	cout << "                variable: AOS_HOME, it will use it." << endl;
	cout << "    -force:     If specified, it will override the source files, if they exist. " << endl;
	cout << "    -help:      To print this menu." << endl;
	return;
}


void printCreateTorturerUsage(void)
{
	cout << "Usage: ProgAid -dir <SrcDirName> -fname <FileName> [-tname <TorturerDirName>] [-homedir <ProjHomeDir>] [-cli] [-force]" << endl;
	cout << "where: " << endl;
	cout << "    -dir:       The source code directory name in which the torturer is created" << endl;
	cout << "    -fname:     The torturer file name." << endl;
	cout << "    -tname:     The torturer directory name (default: \"Tester\"" << endl;
	cout << "    -homedir:   The project home directory name, such as \"/home/cding/AOS\"" << endl;
	cout << "                This is optional. If you defined the environment " << endl;
	cout << "                variable: AOS_HOME, it will use it." << endl;
	cout << "    -cli:       If specified, torturer will support the CLI client. Default: no" << endl;
	cout << "    -force:     If specified, it will override the source files, if they exist. " << endl;
	cout << "    -help:      To print this menu." << endl;
	return;
}


void changeExt(OmnString &srcFileExt)
{
	cout << "Please Enter Source File Extension: " << endl;
	char buff[100];
	cin >> buff;
	srcFileExt = buff;
}


void changeForceFlag(bool &force)
{
	cout << "Do you want to override the files if they exist (y/n)?" << endl;
	char c;
	cin >> c;
	if (c == 'y')
	{
		force = true;
	}
	else
	{
		force = false;
	}
}


void changeFileName(OmnString &filename)
{
	cout << "Please Enter Tester File Name: " << endl;
	char buff[100];
	cin >> buff;
	filename = buff;
}

void changeConfigFileName(OmnString &configFileName)
{
	cout << "Please Enter Configuration File Name: " << endl;
	char buff[100];
	cin >> buff;
	configFileName = buff;
}

void changeProjPrefix(OmnString &projPrefix)
{
	cout << "Please Enter Project Prefix: " << endl;
	char buff[100];
	cin >> buff;
	projPrefix = buff;
}


void changeTesterName(OmnString &testerName)
{
	cout << "Please Enter Tester Directory Name: " << endl;
	char buff[100];
	cin >> buff;
	testerName = buff;
}

void changeProjHomeDir(OmnString &projHomeDir)
{
	cout << "Please Enter Project Home Directory Name: " << endl;
	char buff[100];
	cin >> buff;
	projHomeDir = buff;
}


void changeSrcDir(OmnString &dirname)
{
	cout << "Please Enter the Source Code Directory Name: " << endl;
	char buff[100];
	cin >> buff;
	dirname = buff;
}


void changeTorturerName(OmnString &torturerName)
{
	cout << "Please Enter the Torturer Name: " << endl;
	char buff[100];
	cin >> buff;
	torturerName = buff;
}


void getValue(const OmnString &message, OmnString &value)
{
	cout << "Please Enter the " << message << ":" << endl;
	char buff[100];
	cin >> buff;
	value = buff;
}


void changeCliSupport(bool &withCliSupport)
{
	cout << "Do you want the torturer to implement the CLI Client: (y/n)?" << endl;
	char c;
	cin >> c;
	if (c == 'y')
	{
		withCliSupport = true;
	}
	else
	{
		withCliSupport = false;
	}
}


void changeClassname(OmnString &className)
{
	cout << "Please Enter the Class Name: " << endl;
	char buff[100];
	cin >> buff;
	className = buff;
}

void createClass(int argc, char **argv);
void createTorturer(int argc, char **argv);
void createDbObject(int argc, char **argv);

void changeSptr(bool &withSmartPointerSupport)
{
	cout << "Do you want to use the smart pointer for this class: (y/n)?" << endl;
	cout << "If you do not know, answer 'y'" << endl;
	char c;
	cin >> c;
	if (c == 'y')
	{
		withSmartPointerSupport = true;
	}
	else
	{
		withSmartPointerSupport = false;
	}
}

int 
main(int argc, char **argv)
{	

	OmnApp theApp(argc, argv);
	try
	{
		// theApp.startSingleton(OmnNew OmnKernelInterfaceSingleton());
	}

	catch (const OmnExcept &e)
	{
		cout << "****** Failed to start the application: " << e.toString() << endl;
		OmnAlarm << "Failed to start the application: " << e.toString() << enderr;
		theApp.exitApp();
		return 0;
	}

	int idx = 0;
	OmnString type;
	OmnString configFilename;
	while (idx < argc)
	{
		if (strcmp(argv[idx], "-help") == 0)
		{
			printusage();
			theApp.exitApp();
			return 0;
		}

		if (strcmp(argv[idx], "-torturer") == 0)
		{
			type = "Torturer";
			idx++;
			continue;
		}

		if (strcmp(argv[idx], "-class") == 0)
		{
			type = "Class";
			idx++;
			continue;
		}

		if (strcmp(argv[idx], "-dbobj") == 0)
		{
			type = "db";
			idx++;
			continue;
		}

		if (strcmp(argv[idx], "-c") == 0)
		{
			configFilename = argv[idx+1];
			idx += 2;
			continue;
		}

		idx++;
	}

	/*
	if (type == "db")
	{
		if (configFilename == "")
		{
			cout << "To create a DB Object but missing the config file" << endl;
			theApp.exitApp();
		}

		// AosCreateDbObj gen(configFilename);

		if (!gen.config())
		{
			OmnAlarm << "Failed to create database object" << enderr;
		}

		if (!gen.generateCode())
		{
			OmnAlarm << "Failed to generate code" << enderr;
		}

		theApp.exitApp();
		return 0;
	}
	*/


	bool finished = false;
	while (!finished)
	{
		cout << "Please select one: " << endl;
		cout << "0: Exit" << endl;
		cout << "1: Create Files for a Class" << endl;
		cout << "2: Create a Torturer" << endl;
		// cout << "3: Create a Database Object" << endl;

		char selection;
		cin >> selection;
		switch (selection)
		{
		case '0':
			 finished = true;
			 break;

		case '1':
			 createClass(argc, argv);
		 	 finished = true;
			 break;

		case '2':
			 createTorturer(argc, argv);
		 	 finished = true;
			 break;

		// case '3':
		// 	 createDbObject(argc, argv);
		// 	 finished = true;
		//	 break;

		default:
			 cout << "Invalid selection" << endl;
			 break;
		}
	}

	theApp.exitApp();
	return 0;
}


void createClass(int argc, char **argv)
{
    OmnString projHomeDir = getenv("AOS_HOME");
	OmnString dirname;
	OmnString className;
	OmnString projPrefix = "Aos";
	bool withSmartPointerSupport = true;
	bool force = false;
	OmnString srcFileExt = "cpp";
	int idx = 0;

	while (idx < argc)
	{
		if (strcmp(argv[idx], "-help") == 0)
		{
			printusage();
			return;
		}

		if (strcmp(argv[idx], "-ext") == 0)
		{
			srcFileExt = argv[idx+1];
			idx += 2;
			continue;
		}

		if (strcmp(argv[idx], "-force") == 0)
		{
			force = true;
			idx += 1;
			continue;
		}

		if (strcmp(argv[idx], "-sptr") == 0)
		{
			withSmartPointerSupport = true;
			idx += 1;
			continue;
		}

		if (strcmp(argv[idx], "-classname") == 0)
		{
			className= argv[idx+1];
			idx += 2;
			continue;
		}

		if (strcmp(argv[idx], "-dir") == 0)
		{
			dirname = argv[idx+1];
			idx += 2;
			continue;
		}

		if (strcmp(argv[idx], "-homedir") == 0)
		{
			projHomeDir = argv[idx+1];
			idx += 2;
			continue;
		}

		idx++;
	}

	do 
	{
		if (projHomeDir == "")
		{
			changeProjHomeDir(projHomeDir);
		}

		if (dirname == "")
		{
			changeSrcDir(dirname);
		}

		if (className == "")
		{
			changeClassname(className);
		}

		cout << "You want to create a class: " << endl;
		cout << "  Project Home Directory: " << projHomeDir << endl;
		cout << "  Source Code Directory:  " << dirname << endl;
		cout << "  Class Name:             " << className << endl;
		cout << "  Project Prefix:         " << projPrefix << endl;

		if (withSmartPointerSupport)
		{
			cout << "  Support Smart Pointer:  Yes" << endl;
		}
		else
		{
			cout << "  Support Smart Pointer:  No" << endl;
		}

		if (force)
		{
			cout << "  Force Flag:             Set" << endl;
		}
		else
		{
			cout << "  Force Flag:             Not Set" << endl;
		}

		cout << "  Source File Extension:  " << srcFileExt << endl;

		cout << "Are these settings correct (y/n)?" << endl;
		char answer;
		cin >> answer;
		if (answer == 'y')
		{
			break;
		}

		cout << "Please select an item: " << endl;
		cout << "0: Generate the Code" << endl;
		cout << "1: Set Project Home Directory" << endl;
		cout << "2: Set Source Code Directory" << endl;
		cout << "3: Set Class Name" << endl;
		cout << "4: Set Project Prefix" << endl;
		cout << "5: Set Smart Pointer Support" << endl;
		cout << "6: Set Force Flag" << endl;
		cout << "7: Set Source File Extension" << endl;

		char select;
		cin >> select;
		switch (select)
		{
		case '0':
			 break;

		case '1':
			 changeProjHomeDir(projHomeDir);
			 break;

		case '2':
			 changeSrcDir(dirname);
			 break;

		case '3':
			 changeClassname(className);
			 break;

		case '4':
			 changeProjPrefix(projPrefix);
			 break;

		case '5':
			 changeSptr(withSmartPointerSupport);
			 break;

		case '6':
			 changeForceFlag(force);
			 break;

		case '7':
			 changeExt(srcFileExt);
			 break;

		default:
			 cout << "Invalid selection. Please select one of the items listed below" << endl;
		}
	} while (1);

	AosCreateClassFiles gen(projHomeDir, dirname, className, 
			projPrefix, withSmartPointerSupport, force, srcFileExt);

} 


void createTorturer(int argc, char **argv)
{
	OmnString dirname;
	OmnString torturerName;
    OmnString projHomeDir = getenv("AOS_HOME");
	OmnString testerDirName = "Tester";
	OmnString filename;
	bool force = false;
	bool withCliSupport = false;
	int idx = 0;

	while (idx < argc)
	{
		if (strcmp(argv[idx], "-help") == 0)
		{
			printCreateTorturerUsage();
			return;
		}

		if (strcmp(argv[idx], "-force") == 0)
		{
			force = true;
			idx += 1;
			continue;
		}

		if (strcmp(argv[idx], "-cli") == 0)
		{
			withCliSupport = true;
			idx += 1;
			continue;
		}

		if (strcmp(argv[idx], "-dir") == 0)
		{
			dirname = argv[idx+1];
			idx += 2;
			continue;
		}

		if (strcmp(argv[idx], "-homedir") == 0)
		{
			projHomeDir = argv[idx+1];
			idx += 2;
			continue;
		}

		if (strcmp(argv[idx], "-fname") == 0)
		{
			torturerName = argv[idx+1];
			idx += 2;
			continue;
		}

		if (strcmp(argv[idx], "-tname") == 0)
		{
			testerDirName = argv[idx+1];
			idx += 2;
			continue;
		}

		idx++;
	}

	do 
	{
		if (projHomeDir == "")
		{
			changeProjHomeDir(projHomeDir);
		}

		if (dirname == "")
		{
			changeSrcDir(dirname);
		}

		if (torturerName == "")
		{
			changeTorturerName(torturerName);
		}

		if (testerDirName == "")
		{
			changeTesterName(testerDirName);
		}

		if (filename == "")
		{
			changeFileName(filename);
		}

		cout << "You want to create a torturer: " << endl;
		cout << "  Project Home Directory: " << projHomeDir << endl;
		cout << "  Source Code Directory:  " << dirname << endl;
		cout << "  Tester Directory:       " << testerDirName << endl;
		cout << "  Torturer Name:          " << torturerName << endl;
		cout << "  Tester File Name:       " << filename << endl;

		if (withCliSupport)
		{
			cout << "  Support CLI Client:     Yes" << endl;
		}
		else
		{
			cout << "  Support CLI Client:     No" << endl;
		}

		if (force)
		{
			cout << "  Force Flag:             Set" << endl;
		}
		else
		{
			cout << "  Force Flag:             Not Set" << endl;
		}

		cout << "Are these settings correct (y/n)?" << endl;
		char answer;
		cin >> answer;
		if (answer == 'y')
		{
			break;
		}

		cout << "Please select an item: " << endl;
		cout << "0: Generate the Code" << endl;
		cout << "1: Change Project Home Directory" << endl;
		cout << "2: Change Source Code Directory" << endl;
		cout << "3: Change Tester Directory" << endl;
		cout << "4: Change Torturer Name" << endl;
		cout << "5: Change Tester File Name" << endl;
		cout << "6: Change CLI Client Support" << endl;
		cout << "7: Change Force Flag" << endl;

		char select;
		cin >> select;
		switch (select)
		{
		case '0':
			 break;

		case '1':
			 changeProjHomeDir(projHomeDir);
			 break;

		case '2':
			 changeSrcDir(dirname);
			 break;

		case '3':
			 changeTesterName(testerDirName);
			 break;

		case '4':
			 changeTorturerName(torturerName);
			 break;

		case '5':
			 changeFileName(filename);
			 break;

		case '6':
			 changeCliSupport(withCliSupport);
			 break;

		case '7':
			 changeForceFlag(force);
			 break;

		default:
			 cout << "Invalid selection. Please select one of the items listed below" << endl;
		}
	} while (1);

	// AosTorturerGen gen(projHomeDir, dirname, testerDirName, 
	// 		torturerName, filename, withCliSupport, force, true);
	// gen.genCode();
} 


void createDbObject(int argc, char **argv)
{
	OmnString dirname;
    OmnString projHomeDir = getenv("AOS_HOME");
	OmnString filename;
	OmnString projPrefix = "Aos";
	bool force = false;
	int idx = 0;
	OmnString srcFileExt = "cpp";

	while (idx < argc)
	{
		if (strcmp(argv[idx], "-help") == 0)
		{
			printCreateDbObjectUsage();
			return;
		}

		if (strcmp(argv[idx], "-force") == 0)
		{
			force = true;
			idx += 1;
			continue;
		}

		if (strcmp(argv[idx], "-dir") == 0)
		{
			dirname = argv[idx+1];
			idx += 2;
			continue;
		}

		if (strcmp(argv[idx], "-homedir") == 0)
		{
			projHomeDir = argv[idx+1];
			idx += 2;
			continue;
		}

		if (strcmp(argv[idx], "-fname") == 0)
		{
			filename = argv[idx+1];
			idx += 2;
			continue;
		}

		idx++;
	}

	do 
	{
		if (projHomeDir == "")
		{
			changeProjHomeDir(projHomeDir);
		}

		if (dirname == "")
		{
			changeSrcDir(dirname);
		}

		if (filename == "")
		{
			getValue("Config File Name", filename);
		}

		cout << "You want to create a torturer: " << endl;
		cout << "  Project Home Directory: " << projHomeDir << endl;
		cout << "  Source Code Directory:  " << dirname << endl;
		cout << "  Config File Name:       " << filename << endl;
		cout << "  Project Prefix:         " << projPrefix << endl;
		cout << "  Source File Extension:  " << srcFileExt << endl;

		if (force)
		{
			cout << "  Force Flag:             Set" << endl;
		}
		else
		{
			cout << "  Force Flag:             Not Set" << endl;
		}

		cout << "Are these settings correct (y/n)?" << endl;
		char answer;
		cin >> answer;
		if (answer == 'y')
		{
			break;
		}

		cout << "Please select an item: " << endl;
		cout << "0: Generate the Code" << endl;
		cout << "1: Change Project Home Directory" << endl;
		cout << "2: Change Source Code Directory" << endl;
		cout << "3: Change Config File Name" << endl;
		cout << "4: Change Project Prefix Name" << endl;
		cout << "5: Change Force Flag" << endl;
		cout << "6: Change Source File Extension" << endl;

		char select;
		cin >> select;
		switch (select)
		{
		case '0':
			 break;

		case '1':
			 changeProjHomeDir(projHomeDir);
			 break;

		case '2':
			 changeSrcDir(dirname);
			 break;

		case '3':
			 changeConfigFileName(filename);
			 break;

		case '4':
			 changeProjPrefix(projPrefix);
			 break;

		case '5':
			 changeForceFlag(force);
			 break;

		case '6':
			 changeExt(srcFileExt);
			 break;

		default:
			 cout << "Invalid selection. Please select one of the items listed below" << endl;
		}
	} while (1);

	// AosCreateDbObj gen(dirname, projPrefix, 
	//		projHomeDir, srcFileExt, filename, force);

	// if (!gen.config())
	// {
	// 	OmnAlarm << "Failed to create database object" << enderr;
	// }

	// if (!gen.generateCode())
	// {
	// 	OmnAlarm << "Failed to generate code" << enderr;
	// }

	return; 
} 



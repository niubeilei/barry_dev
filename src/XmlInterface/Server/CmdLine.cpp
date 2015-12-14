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
// 11/15/2009: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "XmlInterface/Server/CmdLine.h"

#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "Book/Tag.h"
#include "DataStore/StoreMgr.h"
#include "Debug/Debug.h"
#include "MySQL/DataStoreMySQL.h"
#include "Proggie/ReqDistr/ReqDistr.h"
#include "TorturerWrappers/ObjectWrapper.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "util2/global_data.h"
#include "VpdParser/VpdParser.h"
#include "XmlInterface/Server/WebReqProc.h"
#include "XmlInterface/Server/Ptrs.h"
#include "XmlInterface/Server/DbConvert.h"


bool
AosWSProcCmdLine(int argc, char **argv)
{
	int index = 1;
	while (index < argc)
	{
		if (strcmp(argv[index], "-dumpxml") == 0)
		{
			if (index + 3 >= argc)
			{
				cout << "Usage WebServer -dumpxml <tablename>"
					<< " <idfieldname> <dataid>" << endl;
				exit(0);
			}

			OmnString tname = argv[index+1];
			OmnString idname = argv[index+2];
			OmnString dataid = argv[index+3];
			AosConvertDb conv;
			conv.dumpXml(tname, idname, dataid);
			exit(0);
		}

		if (strcmp(argv[index], "-mdb") == 0)
		{
			if (index + 7 >= argc)
			{
				cout << "Usage: WebServer -mdb <tname> <query> "
					"<xmlfname> <idfname> <aname> <newname> <sep>" << endl;
				exit(0);
			}

			AosConvertDb conv;
			OmnString tname 	= argv[index+1];
			OmnString query 	= argv[index+2];
			OmnString xmlfname 	= argv[index+3];
			OmnString idfname 	= argv[index+4];
			OmnString aname 	= argv[index+5];
			OmnString newname 	= argv[index+6];
			OmnString sep 		= argv[index+7];
			OmnString errmsg;
			aos_assert_r(conv.changeAllAttrName(tname, query, xmlfname, 
				idfname, aname, newname, sep, errmsg), -1);
			exit(0);
		}


		if (strcmp(argv[index], "-convertimg") == 0)
		{
			AosObjectWrapper wrap;
			wrap.fromImgToImages();
			exit(0);
		}

		if (strcmp(argv[index], "-delattr") == 0)
		{
			if (index + 4 >= argc)
			{
				cout << "Usage: WebServer -delattr <tname> <query> "
					"<aname> <avalue>" << endl;
				exit(0);
			}
			OmnString tname = argv[index+1];
			OmnString query = argv[index+2];
			OmnString aname = argv[index+3];
			OmnString avalue = argv[index+3];
			OmnString errmsg;
			AosConvertDb conv;
			conv.removeAttrs(tname, query, aname, avalue, errmsg);
			exit(0);
		}

		if (strcmp(argv[index], "-modattr") == 0)
		{
			if (index + 5 >= argc)
			{
				cout << "Usage: WebServer -modattr <tname> <query> "
					"<aname> <avalue> <newvalue>" << endl;
				exit(0);
			}

			AosConvertDb conv;
			OmnString tname 	= argv[index+1];
			OmnString query 	= argv[index+2];
			OmnString aname 	= argv[index+3];
			OmnString avalue	= argv[index+4];
			OmnString newvalue 	= argv[index+5];
			OmnString errmsg;
			aos_assert_r(conv.changeAllAttrValues(tname, query, 
				aname, avalue, newvalue, errmsg), -1);
			exit(0);
		}

		if (strcmp(argv[index], "-cleantagtable") == 0)
		{
			AosTag::cleanTagTable();
			exit(0);
		}

		if (strcmp(argv[index], "-removedupeattr") == 0)
		{
			if (index + 2 >= argc)
			{
				cout << "Usage: WebServer -removedupeattr <tname> <query> "
					<< endl;
				exit(0);
			}

			AosConvertDb conv;
			OmnString tname 	= argv[index+1];
			OmnString query 	= argv[index+2];
			OmnString errmsg;
			aos_assert_r(conv.removeDupeAttrs(tname, query, errmsg), -1);
			exit(0);
		}

		if (strcmp(argv[index], "-convertoimages") == 0)
		{
			if (index + 5 >= argc)
			{
				cout << "Usage: WebServer -converttoimages <tname> <query> "
					<< "<container> <creator> <imgtype>"
					<< endl;
				exit(0);
			}

			AosObjectWrapper wrap;
			OmnString tname 	= argv[index+1];
			OmnString query 	= argv[index+2];
			OmnString container = argv[index+3];
			OmnString creator	= argv[index+4];
			OmnString imgtype	= argv[index+5];
			aos_assert_r(wrap.toImages(tname, query, 
				container, creator, imgtype), -1);
			exit(0);
		}

		index++;
	}

	return true;
}

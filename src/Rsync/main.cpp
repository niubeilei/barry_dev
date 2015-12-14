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
// 2015/01/28 Created by Koala Ren
////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include <cstring>
#include "AOS_Rsync.h"
#include <unistd.h>
using namespace std;

int main()
{
    AOS_Rsync *rsync = new AOS_Rsync(20);
    char target_filename[50],reference_filename[50];

//    cout<<"please input the filename you want to syncronize:";
//    cin>>target_filename;
//    cout<<"please input the reference filename :";
//    cin>>reference_filename;

    if(rsync->compare("hello2","hello") == false)
    {
        exit(0);
    }
    rsync->printResult();
    //pause();
    return 0;
}

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
// 2015/02/03 Created by Koala Ren
////////////////////////////////////////////////////////////////////////////
#include "AOS_Rsync.h"
#include "stdio.h"
#include "stdlib.h"
#include <fstream>
#include <cstring>
#include <vector>
#include <sys/types.h>
#include "AOS_Match.h"
#include <iostream>

//////////////////////////////////////////////////////////////
// This class is used to compare two strings (files) to
// list the matched blocks and the differences.
//
// 1. If two strings are identical, it outputs a null struct.
// 2. Otherwise, it should generate:
//      delta[pos1, length1, pos2, length2]
//      match[block1, block2]
//      delta[pos1, length1, pos2, length2]
//      match[block1, block2]
//      ...
//////////////////////////////////////////////////////////////

using namespace std;
AOS_Rsync::AOS_Rsync(uint BlcokSize)
{
    match = new AOS_Match();
    blocksize = BlcokSize;
    s = new Sum_Struct();
    blockid = 0;
    is_equal = true;
}

AOS_Rsync::~AOS_Rsync()
{
    delete[] s->sums;
    delete[] reference_buf;
    delete[] target_buf;
    delete match;
}
Sum_Struct*
AOS_Rsync::generate_sums(char *buf,off_t filelen)//divide file2 and calclulate the block checksum
{
    // This function creates the MD5 checksums for the data 'buf'.
    // The results are stored in 'Sum_Struct'. It creates one
    // checksum for each block. The block size is determined
    // by member data 'blocksize'.
    off_t offset = 0;
    int count = (filelen+(blocksize-1))/blocksize;

    s->count = count;
    s->n = blocksize;
    s->flength = filelen;

    if (count == 0)
    {
        s->sums = NULL;
        cout<<"block acount = 0 in AOS_Rsync::generate_sums!"<<endl;
        return s;
    }
    s->sums = (Sum_Buf *)malloc(sizeof(s->sums[0])*s->count);
    if(NULL == s->sums)
    {
        cout<<"Fail to allocate enough memory for Sum_Buf in AOS_Rsync::generate_sums !"<<endl;
        exit(0);
    }
    int len = filelen;
    for (int i=0 ; i< count; i++)
    {
        int n1 = len>blocksize?blocksize:len;
        s->sums[i].sum1 = AOS_Checksum::get_checksum1(buf,n1);
        bzero(s->sums[i].sum2,33);
        AOS_Checksum::get_checksum2(buf,n1,s->sums[i].sum2);

        s->sums[i].offset = offset;
        s->sums[i].len = n1;
        s->sums[i].i = i;
        len -= n1;
        buf += n1;
        offset += n1;
    }
    return s;
}
bool
AOS_Rsync::compare(char* filename1, char* filename2)
{
    // This function read two file name and compare it's content

    off_t file2len,file1len;
    if(readFile(filename2 , reference_buf, file2len) == false)
    {
        return false;
    }
    if(readFile(filename1 , target_buf, file1len) == false)
    {
        return false;
    }

    s = generate_sums(reference_buf,file2len);
//    for(int i = 0; i< s->count; i++ )
//        cout<<"block"<<i<<" sum1 = "<<s->sums[i].sum1<<"    MD5 =  "<<s->sums[i].sum2<<endl;
    match->build_hash_table(s);

    uint last_hit_block = -1;
    off_t target_offset = 0;//current block position
    off_t flag_offset = 0;//last matched end position
    int id;


    off_t remainder_len;
    off_t dif_len;
    do
    {
        char tem_buf[blocksize+1];
        bzero(tem_buf, blocksize+1);
        dif_len = target_offset - flag_offset;
        remainder_len = file1len - dif_len;
        int actuallen = remainder_len < blocksize?remainder_len:blocksize;

        bcopy(target_buf + target_offset ,tem_buf,actuallen);
        int target_sum1 = AOS_Checksum::get_checksum1(tem_buf,actuallen);
        id = match->match_sums(s,tem_buf,actuallen,target_sum1, last_hit_block+1);
        if(id >= 0)
        {
            Result tem_result;
            if(dif_len > 0)//if there are some char before this block
            {
                addDifStr(flag_offset,dif_len, (last_hit_block+1)*blocksize ,(id- last_hit_block - 1)*blocksize);
                is_equal = false;
            }
            last_hit_block = id;
            tem_result.flag = 1;
            tem_result.blockid1 = blockid;
            tem_result.blockid2 = id;
            result.push_back(tem_result);
            blockid++;

            target_offset += actuallen;
            flag_offset = target_offset;
            file1len -= (actuallen + dif_len);
        }
        else
        {
            target_offset++;
        }
    }while(remainder_len > 0);
    if(dif_len > 0)
        addDifStr(flag_offset,dif_len, (last_hit_block+1)*blocksize ,s->flength - (last_hit_block+1)*blocksize);
    return true;
}
bool
AOS_Rsync::printResult()
{
    // This function print the final result
    // 1. If two strings are identical, it outputs a null struct.
    // 2. Otherwise, it should generate:
    //      delta[pos1, length1, pos2, length2]
    //      match[block1, block2]
    //      delta[pos1, length1, pos2, length2]
    //      match[block1, block2]
    if(true == is_equal)
    {
        cout<<"they are completely equal!"<<endl;
        return true;
    }

    for(int i = 0; i< result.size(); i++)
    {
        if(result[i].flag == 1)
        {
            cout<<"match[ "<<result[i].blockid1<<" , "<<result[i].blockid2<<" ]"<<endl;
            //cout<<"block "<<result[i].blockid1<<" match blcok "<<result[i].blockid2<<endl;
        }
        else if(result[i].flag == 0)
        {
            cout<<"delta[ "<<result[i].pos1<<" , "<<result[i].len1<<" , "<<result[i].pos2<<" , "<<result[i].len2<<" ]"<<endl;
        }
    }
    return true;
}
bool
AOS_Rsync::addDifStr(off_t pos1,off_t len1, off_t pos2, off_t len2)
{
    // This function save the different block
    // format as follows:
    // if flag == 0 it means that this is a different block
    // delta[pos1, length1, pos2, length2]
    if(pos1 < 0 || pos2 <0 || len1 <0 || len2 < 0)
    {
        return false;
    }
    Result tem_result;
    tem_result.flag = 0;
    tem_result.blockid1 = blockid;
    tem_result.pos1 = pos1;
    tem_result.len1 = len1;
    tem_result.pos2 = pos2;
    tem_result.len2 = len2;
    result.push_back(tem_result);
    blockid++;
    return true;
}
bool
AOS_Rsync::readFile(char *filename , char* &buf, off_t &len)
{
    // This function read file and return it's content as buf
    //and it's length as len

    ifstream fin;
    fin.open(filename);
    if(!fin)
    {
        cout<<"fail to open "<<filename<< " in AOS_Rsync::ReadFile"<<endl;
        return false;
    }
    fin.seekg(0,ios::end);
    len = fin.tellg();
    cout<<"filename = " <<filename <<" length = "<<len<<endl;
    if(0 > len)
    {
        cout<<"file length < 0"<<endl;
        return false;
    }
    fin.seekg(ios::beg);
    buf = new char[len];
    if(!buf)
    {
        cout<<"Fail to allocate enough memory for "<<filename<<" in AOS_Rsync::generate_sums !"<<endl;
    }
    fin.read(buf,len);
    fin.close();
    return true;
}












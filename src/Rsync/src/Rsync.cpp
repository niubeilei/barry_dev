#include "../include/Rsync.h"
#include "stdio.h"
#include "stdlib.h"
#include <fstream>
#include <cstring>
#include <vector>
#include <sys/types.h>
#include "../include/Match.h"
#include <iostream>

using namespace std;
Rsync::Rsync(uint BlcokSize)
{
    match = new Match();
    blocksize = BlcokSize;
    s = new Sum_Struct();
}

Rsync::~Rsync()
{
    for(unsigned int i =0; i< result.size(); i++)
    {
        if(0 == result[i].flag)
            delete[] result[i].str;
    }
    delete[] s->sums;
    delete[] reference_buf;
    delete[] target_buf;
    delete match;
}
Sum_Struct* Rsync::generate_sums(char *buf,off_t filelen)//divide file2 and calclulate the block checksum
{
    off_t offset = 0;
    int count = (filelen+(blocksize-1))/blocksize;

    s->count = count;
    s->remainder = filelen%blocksize;
    s->n = blocksize;
    s->flength = filelen;

    if (count == 0)
    {
        s->sums = NULL;
        cout<<"block acount = 0 in Rsync::generate_sums!"<<endl;
        return s;
    }
    s->sums = (Sum_Buf *)malloc(sizeof(s->sums[0])*s->count);
    if(NULL == s->sums)
    {
        cout<<"Fail to allocate enough memory for Sum_Buf in Rsync::generate_sums !"<<endl;
        exit(0);
    }
    int len = filelen;
    for (int i=0 ; i< count; i++)
    {
        int n1 = len>blocksize?blocksize:len;
        s->sums[i].sum1 = Checksum::get_checksum1(buf,n1);
        bzero(s->sums[i].sum2,33);
        Checksum::get_checksum2(buf,n1,s->sums[i].sum2);

        s->sums[i].offset = offset;
        s->sums[i].len = n1;
        s->sums[i].i = i;
        len -= n1;
        buf += n1;
        offset += n1;
    }
    return s;
}
bool Rsync::Compare(char* filename1, char* filename2)
{
    off_t file2len,file1len;
    if(ReadFile(filename2 , reference_buf, file2len) == false)
    {
        return false;
    }
    if(ReadFile(filename1 , target_buf, file1len) == false)
    {
        return false;
    }

    s = generate_sums(reference_buf,file2len);
    for(int i = 0; i< s->count; i++ )
        cout<<"block"<<i<<" sum1 = "<<s->sums[i].sum1<<"    MD5 =  "<<s->sums[i].sum2<<endl;
    match->build_hash_table(s);

    off_t target_offset = 0;//current block position
    off_t flag_offset = 0;//last matched end position
    off_t remainder_len;
    off_t dif_len;
    do
    {
        char tem_buf[blocksize+1];
        bzero(tem_buf, blocksize+1);
        dif_len = target_offset - flag_offset;
        remainder_len = file1len - dif_len;
        int actuallen = remainder_len < blocksize?remainder_len:blocksize;

        if(flag_offset + actuallen > file1len)
        {
            cout<< "bcopy over maxsize file in Rsync::Compare!"<<endl;
            return NULL;
        }
        bcopy(target_buf + target_offset ,tem_buf,actuallen);
        int target_sum1 = Checksum::get_checksum1(tem_buf,actuallen);
        int id = match->match_sums(s,tem_buf,actuallen,target_sum1);
        if(id >= 0)
        {
            struct Result tem_result;
            if(dif_len > 0)//if there are some char before this block
            {
                AddDifStr(target_buf + flag_offset ,dif_len);
            }
            tem_result.flag = 1;
            tem_result.blockid = id;
            result.push_back(tem_result);

            target_offset += blocksize;
            flag_offset = target_offset;
            file1len -= (actuallen + dif_len);
        }
        else
        {
            target_offset++;
        }

    }while(remainder_len > 0);
    if(dif_len > 0)
        AddDifStr(target_buf + flag_offset,dif_len);
    return true;
}
bool Rsync::PrintResult()
{
    for(int i = 0; i< result.size(); i++)
    {
        if(result[i].flag == 1)
        {
            cout<<"block "<<i<<" match reference file blcok id = "<<result[i].blockid<<endl;
        }
        else if(result[i].flag == 0)
        {
            cout<<"block "<<i<<" can't find match,string = "<<result[i].str<<endl;
        }
    }
    return true;
}
bool Rsync::AddDifStr(char *srcbuf,int len)
{
    if(NULL == srcbuf || 0 == len)
    {
        cout<< "string can't be NULL or length of string can't be 0 in Rsync::AddDifStr"<<endl;
        return false;
    }
    struct Result tem_result;
    tem_result.str = new char[len+1];
    if(!tem_result.str)
    {
        cout<<"Fail to allocate enough memory wen in Rsync::AddDifStr !"<<endl;
    }
    tem_result.flag = 0;
    bzero(tem_result.str,len+1);
    bcopy(srcbuf,tem_result.str,len);
    result.push_back(tem_result);
    return true;
}
bool Rsync::ReadFile(char *filename , char* &buf, off_t &len)
{
    ifstream fin;
    fin.open(filename);
    if(!fin)
    {
        cout<<"fail to open "<<filename<< " in Rsync::ReadFile"<<endl;
        return false;
    }
    fin.seekg(0,ios::end);
    len = fin.tellg();
    if(0 > len)
    {
        cout<<"file length < 0"<<endl;
        return false;
    }
    fin.seekg(ios::beg);
    buf = new char[len];
    if(!buf)
    {
        cout<<"Fail to allocate enough memory for "<<filename<<" in Rsync::generate_sums !"<<endl;
    }
    fin.read(buf,len);
    fin.close();
    return true;
}












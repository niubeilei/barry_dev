#include "../include/Checksum.h"
#include <string.h>
#include <iostream>

using namespace std;
unsigned int Checksum::get_checksum1(char *buf,int len)
{
    int i;
    uint32 s1, s2;

    s1 = s2 = 0;
    for (i = 0; i < len; i++)
    {
        s1 += buf[i];
        s2 += s1;
    }
    return (s1 & 0xffff) + (s2 << 16);
}
void Checksum::get_checksum2(char *buf,int len,char *MD5_result)//get MD5
{
    char tem_buf[len+1];
    bzero(tem_buf,len+1);
    if(flag_offset + actuallen > file1len)
    {
        cout<< "bcopy over maxsize file in Rsync::Compare!"<<endl;
        return NULL;
    }
    bcopy(buf,tem_buf,len);
//    cout<<"\n************MD5 start line******************\n";
//    cout<<"MD5 buf length = "<<strlen(tem_buf)<<" \n"<<tem_buf <<endl;
//    cout<<"\n************MD5 end line******************\n";
    MD5::TransTo32ByteMd5(tem_buf,MD5_result);
}

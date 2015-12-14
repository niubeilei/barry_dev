#ifndef CHECKSUM_H
#define CHECKSUM_H

#include "MD5.h"

typedef unsigned int uint32;
class Checksum
{
    public:
        Checksum();
        virtual ~Checksum();
        static unsigned int get_checksum1(char *buf,int len);
        static void get_checksum2(char *buf,int len,char *MD5_result);
    protected:
    private:
};

#endif // CHECKSUM_H

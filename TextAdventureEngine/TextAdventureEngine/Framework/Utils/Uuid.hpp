#pragma once
#ifndef UUID_H
#define UUID_H

//=====================================================================================
struct Uuid
{
	friend Uuid GenUuid();

private:
    unsigned long  Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char  Data4[ 8 ];
};

//=====================================================================================
Uuid GenUuid();

//=====================================================================================
uint32_t GenUniqueRND();

#endif//UUID_H
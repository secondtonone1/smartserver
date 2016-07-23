#ifndef _STRUCTTYPE_H_
#define _STRUCTTYPE_H_



#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>

#ifndef WIN32
#include <netinet/in.h>
# ifdef _XOPEN_SOURCE_EXTENDED
#  include <arpa/inet.h>
# endif
#include <sys/socket.h>
#endif

#include "event2/bufferevent.h"
#include "event2/buffer.h"
#include "event2/listener.h"
#include "event2/util.h"
#include "event2/event.h"

#include <WinSock2.h>
#include <string.h>
#include <iostream>
using namespace std;
#define PACKETHEADLEN  8

struct PacketHead
{
	int packetID;
	int packetLen;
};

typedef long long int  Int64;
typedef int Int32;
typedef unsigned long long int UInt64;
typedef unsigned int UInt32;

#endif
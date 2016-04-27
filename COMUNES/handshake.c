/*
 * handshake.c
 *
 *  Created on: 26/4/2016
 *      Author: Franco Albornoz
 */

#ifndef HANDSHAKE_H_
#include "handshake.h"
#endif

int handshakeOut(char yo,char el,int socket)
{
	char buffer;
	send(socket,yo,sizeof(yo),0);
	recv(socket,&buffer,sizeof(buffer),0);
	if (buffer == el)
		return 0;
	else
		return 1;
}

handshakeIn(char yo,char el1,char el2,int socket)
{
	char buffer;
	recv(socket,&buffer,sizeof(buffer),0);
	send(socket,yo,sizeof(yo),0);
	if (buffer == el1 || buffer == el2)
		return 0;
	else
		return 1;
}

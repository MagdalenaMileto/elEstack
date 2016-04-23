/*
 * swap.c
 *
 *  Created on: 21/4/2016
 *      Author: Ivan Bober
 */

#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<sys/types.h>
#include <netinet/in.h>
#include<netinet/ip.h>
#include<unistd.h>

int main(int argc,char **argv) {

	struct sockaddr_in umcAddress;
		umcAddress.sin_family = AF_INET;
		umcAddress.sin_addr.s_addr = inet_addr(argv[1]);
		umcAddress.sin_port = htons(atoi(argv[2]));


		int umc = socket(AF_INET,SOCK_STREAM,0);
			if(connect(umc,(void*)&umcAddress,sizeof(umcAddress)) != 0) {
				perror("No se pudo conectar al nucleo.");
				return EXIT_FAILURE;
			}
		close(umc);
		return EXIT_SUCCESS;
}


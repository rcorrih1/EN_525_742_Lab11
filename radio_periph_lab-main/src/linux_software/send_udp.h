#ifndef SEND_UDP_H
#define SEND_UDP_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define PACKET_SIZE 1028
#define DATA_COUNT 256

void send_udp(const char* ip_address, const uint32_t* data, uint16_t port);

#endif
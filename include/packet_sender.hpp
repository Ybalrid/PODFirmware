#pragma once

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>

#include "packets.h"
#include <string>

class packet_sender
{
    struct sockaddr_in si_server;
    int s, rs; //socket kernel file descriptor
    struct POD_WalkVector walkVector;
    std::string server_address;

    public:
        void setServerAddress(const std::string& addr)
        {
            server_address = addr;
            inet_aton(server_address.c_str(), &si_server.sin_addr);
        }

        packet_sender()
        {
            //Open socket for upd datagram
            s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

            //Give socket rights to boadcast on the network
            const int broadcast = 1;
            setsockopt(s, SOL_SOCKET, SO_BROADCAST, (const void*)&broadcast, sizeof broadcast);

            //Configure some server feilds
            memset((char*)&si_server, 0, sizeof(si_server));
            si_server.sin_family = AF_INET;
            si_server.sin_port = htons(POD_PROTOCOL_PORT);

            //TODO bind socket to be able to receive data into port POD_PROTOCOL_PORT via too
        }

        ~packet_sender()
        {
            close(s);
        }


        void sendWalkVector(long long timepoint, float x, float y)
        {
            walkVector.packetType = PODAPI_WALK_VEC;
            walkVector.timepoint = timepoint;
            walkVector.x = x;
            walkVector.y = y;

            sendto(s,
                    (POD_Byte*)&walkVector,
                    sizeof walkVector,
                    0,
                    (struct sockaddr*)&si_server,
                    sizeof si_server);
        }
};

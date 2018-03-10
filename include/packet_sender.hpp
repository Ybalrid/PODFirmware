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
    int s; //socket kernel file descriptor
    struct POD_WalkVector walkVector;
    char* server_address

    public:
        void setServerAddress(const std::string& addr)
        {
            server_address = addr.c_str();
            inet_aton(server_address, &si_server.sin_addr);
        }

        packet_sender()
        {
            s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
            memset((char*)&si_sever, 0, sizeof(si_server));
            si_server.sin_family = AF_INET;
            si_sever.sin_port = htons(POD_PROTOCOL_PORT);
        }

        ~packet_sender()
        {
            close(socket);
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
                    (struct sockaddr*)&si_other,
                    sizeof si_other);
        }
};

#include "net_server.h"
#include "net_message.h"
#include <stdio.h>

typedef struct ClientData ClientData;
struct ClientData
{
    u32 connectID;
};

static void ServerSend(ENetPeer *peer, NetMessage msg)
{
    ENetPacket *packet = enet_packet_create(&msg, sizeof(NetMessage), 0);

    if (enet_peer_send(peer, 0, packet) != 0)
    {
        printf("[Server] failed to send packet with type %d to client\n", msg.type);
    }
}

void ServerHandleConnect(ENetHost *host, ENetEvent event)
{
    printf("[Server] A new client connected from %x:%u.\n", event.peer->address.host,
           event.peer->address.port);
    event.peer->data = malloc(sizeof(ClientData));
    ClientData *data = event.peer->data;
    data->connectID = event.peer->connectID;

    for (size_t i = 0; i < host->peerCount; ++i)
    {
        if (host->peers[i].connectID != event.peer->connectID && host->peers[i].data)
        {
            ServerSend(host->peers + i, (NetMessage){
                                            .type = TO_CLIENT_PLAYER_JOIN,
                                            .id = event.peer->connectID,
                                        });
        }
    }
}

void ServerHandleReceive(ENetHost *host, ENetEvent event)
{
    NetMessage *msg = (NetMessage *)event.packet->data;

    switch (msg->type)
    {
    case TO_SERVER_PLAYER_ID: {
        ServerSend(event.peer, (NetMessage){
                                   .type = TO_CLIENT_PLAYER_ID,
                                   .id = event.peer->connectID,
                               });
        break;
    }
    case TO_SERVER_PLAYER_LIST: {
        NetMessage res = {
            .type = TO_CLIENT_PLAYER_LIST,
            .playerList.count = (i32)host->peerCount,
        };

        for (size_t i = 0; i < host->peerCount; ++i)
        {
            res.playerList.ids[i] = host->peers[i].connectID;
        }

        ServerSend(event.peer, res);
        break;
    }
    case TO_SERVER_PLAYER_INFO: {
        for (size_t i = 0; i < host->peerCount; ++i)
        {
            if (host->peers[i].connectID != event.peer->connectID && host->peers[i].data)
            {
                ServerSend(host->peers + i, (NetMessage){
                                                .type = TO_CLIENT_PLAYER_INFO,
                                                .id = event.peer->connectID,
                                                .playerInfo = msg->playerInfo,
                                            });
            }
        }
        break;
    }
    default:
        printf("[Server] Ignoring packet with type: %d\n", msg->type);
        break;
    }

    enet_packet_destroy(event.packet);
}

void ServerHandleDisconnect(ENetHost *host, ENetEvent event)
{
    printf("[Server] client with id %d disconnected.\n", event.peer->connectID);

    for (size_t i = 0; i < host->peerCount; ++i)
    {
        if (host->peers[i].connectID != event.peer->connectID && host->peers[i].data)
        {
            ServerSend(host->peers + i, (NetMessage){
                                            .type = TO_CLIENT_PLAYER_LEAVE,
                                            .id = event.peer->connectID,
                                        });
        }
    }

    free(event.peer->data);
    event.peer->data = NULL;
}

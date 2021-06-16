#include "net_server.h"
#include <stdio.h>

void ServerHandleConnect(ENetEvent event)
{
    printf("[Server] A new client connected from %x:%u.\n", event.peer->address.host,
           event.peer->address.port);
    event.peer->data = "Client";
}

void ServerHandleRecieve(ENetEvent event)
{
    printf("[Server] Received: %s\n", event.packet->data);
    enet_packet_destroy(event.packet);

    ENetPacket *packet = enet_packet_create("pong", strlen("pong") + 1, 0);
    enet_peer_send(event.peer, 0, packet);
}

void ServerHandleDisconnect(ENetEvent event)
{
    printf("[Server] %s disconnected.\n", (char *)event.peer->data);
    event.peer->data = NULL;
}

#include "net.h"
#include <stdio.h>

void ServerPollEvents(ENetHost *server)
{
    ENetEvent event;
    while (enet_host_service(server, &event, 0) > 0)
    {
        switch (event.type)
        {
        case ENET_EVENT_TYPE_CONNECT:
            printf("A new client connected from %x:%u.\n", event.peer->address.host,
                   event.peer->address.port);
            event.peer->data = "Client information";
            break;
        case ENET_EVENT_TYPE_RECEIVE:
            printf("A packet of length %u containing %s was received from %s on "
                   "channel %u.\n",
                   event.packet->dataLength, event.packet->data, event.peer->data,
                   event.channelID);
            enet_packet_destroy(event.packet);
            break;
        case ENET_EVENT_TYPE_DISCONNECT:
            printf("%s disconnected.\n", event.peer->data);
            event.peer->data = NULL;
        }
    }
}

void ClientPollEvents(ENetHost *client)
{
    ENetEvent event;
    while (enet_host_service(client, &event, 0) > 0)
    {
        switch (event.type)
        {
        case ENET_EVENT_TYPE_CONNECT:
            printf("connected to host %x:%u.\n", event.peer->address.host,
                   event.peer->address.port);
            event.peer->data = "Host information";
            break;
        case ENET_EVENT_TYPE_DISCONNECT:
            printf("%s disconnected.\n", event.peer->data);
            event.peer->data = NULL;
        }
    }
}

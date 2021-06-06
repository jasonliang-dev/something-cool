#include "net.h"
#include <stdio.h>
#include <string.h>

static struct
{
    ENetHost *server;
    ENetHost *client;
    ENetPeer *peer;
} g_Net;

void InitNet(void)
{
    if (enet_initialize() != 0)
    {
        Fatal("Cannot initialize ENet.");
    }

    memset(&g_Net, 0, sizeof(g_Net));
}

void InitServer(u16 port)
{
    if (g_Net.server)
    {
        return;
    }

    ENetAddress address;
    address.host = ENET_HOST_ANY;
    address.port = port;

    // 32 connections, 2 channels, 0 and 1
    g_Net.server = enet_host_create(&address, 32, 2, 0, 0);

    if (!g_Net.server)
    {
        Fatal("Can't create server host");
    }
    else
    {
        printf("Created server\n");
    }
}

void ServerPollEvents(void)
{
    if (!g_Net.server)
    {
        return;
    }

    ENetEvent event;
    while (enet_host_service(g_Net.server, &event, 0) > 0)
    {
        switch (event.type)
        {
        case ENET_EVENT_TYPE_CONNECT:
            printf("A new client connected from %x:%u.\n", event.peer->address.host,
                   event.peer->address.port);
            event.peer->data = "Client information";
            break;
        case ENET_EVENT_TYPE_RECEIVE:
            printf("[Server] Received: %s\n", event.packet->data);
            enet_packet_destroy(event.packet);

            ENetPacket *packet = enet_packet_create("pong", strlen("pong") + 1, 0);
            enet_peer_send(event.peer, 0, packet);
            break;
        case ENET_EVENT_TYPE_DISCONNECT:
            printf("%s disconnected.\n", (char *)event.peer->data);
            event.peer->data = NULL;
        default:
            break;
        }
    }
}

void InitClient(void)
{
    if (g_Net.client)
    {
        return;
    }

    g_Net.client = enet_host_create(NULL, 1, 2, 0, 0);

    if (!g_Net.client)
    {
        Fatal("Can't create client host");
    }
    else
    {
        printf("Created client\n");
    }
}

NetError ClientConnect(const char *host, u16 port)
{
    if (g_Net.peer)
    {
        return "Already connected to another peer";
    }

    ENetAddress address;
    if (enet_address_set_host(&address, host) != 0)
    {
        return "Could not resolve host";
    }
    address.port = port;

    g_Net.peer = enet_host_connect(g_Net.client, &address, 2, 0);
    if (!g_Net.peer)
    {
        return "Could not connect to peer";
    }

    return NULL;
}

NetError ClientSend(const char *message)
{
    ENetPacket *packet = enet_packet_create(message, strlen(message) + 1, 0);

    if (enet_peer_send(g_Net.peer, 0, packet) != 0)
    {
        return "Cannot queue the packet to be sent";
    }

    return NULL;
}

void ClientPollEvents(void)
{
    ENetEvent event;
    while (enet_host_service(g_Net.client, &event, 0) > 0)
    {
        switch (event.type)
        {
        case ENET_EVENT_TYPE_CONNECT:
            printf("connected to host %x:%u.\n", event.peer->address.host,
                   event.peer->address.port);
            event.peer->data = "Host information";
            break;
        case ENET_EVENT_TYPE_RECEIVE:
            printf("[Client] Received: %s\n", event.packet->data);
            enet_packet_destroy(event.packet);
            break;
        case ENET_EVENT_TYPE_DISCONNECT:
            printf("%s disconnected.\n", (char *)event.peer->data);
            event.peer->data = NULL;
        default:
            break;
        }
    }
}

#include "net.h"
#include <stdio.h>
#include <string.h>

static struct
{
    struct
    {
        ENetHost *host;
    } server;

    struct
    {
        ENetHost *host;
        ENetPeer *peer;
    } client;
} g_Net;

void InitNet(void)
{
    if (enet_initialize() != 0)
    {
        Fatal("Cannot initialize ENet.");
    }

    memset(&g_Net, 0, sizeof(g_Net));

    if (g_Net.client.host)
    {
        return;
    }

    enum
    {
        NUM_PEERS = 1,
        NUM_CHANNELS = 2,
        DOWNSTREAM_BANDWIDTH = 0,
        UPSTREAM_BANDWIDTH = 0,
    };

    g_Net.client.host = enet_host_create(NULL, NUM_PEERS, NUM_CHANNELS,
                                         DOWNSTREAM_BANDWIDTH, UPSTREAM_BANDWIDTH);

    if (!g_Net.client.host)
    {
        Fatal("Can't create client host");
    }
    else
    {
        printf("Created net client\n");
    }
}

NetError InitServer(u16 port)
{
    if (g_Net.server.host)
    {
        return "Server already created";
    }

    ENetAddress address;
    address.host = ENET_HOST_ANY;
    address.port = port;

    enum
    {
        NUM_CLIENTS = 32,
        NUM_CHANNELS = 2,
        DOWNSTREAM_BANDWIDTH = 0,
        UPSTREAM_BANDWIDTH = 0,
    };

    g_Net.server.host = enet_host_create(&address, NUM_CLIENTS, NUM_CHANNELS,
                                         DOWNSTREAM_BANDWIDTH, UPSTREAM_BANDWIDTH);

    if (!g_Net.server.host)
    {
        return "Can't create server host";
    }

    printf("Created server\n");
    return NULL;
}

void CloseServer(void)
{
    if (!g_Net.server.host)
    {
        return;
    }

    for (i32 i = 0; i < g_Net.server.host->peerCount; ++i)
    {
        ENetPeer *peer = g_Net.server.host->peers + i;
        enet_peer_disconnect(peer, 0);
    }

    enet_host_flush(g_Net.server.host);
    enet_host_destroy(g_Net.server.host);
    g_Net.server.host = NULL;
    printf("Closed server\n");
}

void ServerPollEvents(void)
{
    if (!g_Net.server.host)
    {
        return;
    }

    ENetEvent event;
    while (enet_host_service(g_Net.server.host, &event, 0) > 0)
    {
        switch (event.type)
        {
        case ENET_EVENT_TYPE_CONNECT:
            printf("[Server] A new client connected from %x:%u.\n",
                   event.peer->address.host, event.peer->address.port);
            event.peer->data = "Client";
            break;
        case ENET_EVENT_TYPE_RECEIVE:
            printf("[Server] Received: %s\n", event.packet->data);
            enet_packet_destroy(event.packet);

            ENetPacket *packet = enet_packet_create("pong", strlen("pong") + 1, 0);
            enet_peer_send(event.peer, 0, packet);
            break;
        case ENET_EVENT_TYPE_DISCONNECT:
            printf("[Server] %s disconnected.\n", (char *)event.peer->data);
            event.peer->data = NULL;
        default:
            break;
        }
    }
}

NetError ClientConnect(const char *hostAddress, u16 port)
{
    if (g_Net.client.peer)
    {
        return "Already connected to another peer";
    }

    ENetAddress address;
    if (enet_address_set_host(&address, hostAddress) != 0)
    {
        return "Could not resolve host";
    }
    address.port = port;

    g_Net.client.peer = enet_host_connect(g_Net.client.host, &address, 2, 0);
    if (!g_Net.client.peer)
    {
        return "Could not connect to peer";
    }

    return NULL;
}

void ClientDisconnect(void)
{
    enet_peer_disconnect(g_Net.client.peer, 0);
    g_Net.client.peer = NULL;
    printf("Disconnected from peer\n");
}

NetError ClientSend(const char *message)
{
    if (!g_Net.client.peer)
    {
        return "Not connected to a peer";
    }

    ENetPacket *packet = enet_packet_create(message, strlen(message) + 1, 0);

    if (enet_peer_send(g_Net.client.peer, 0, packet) != 0)
    {
        return "Cannot queue the packet to be sent";
    }

    return NULL;
}

void ClientPollEvents(void)
{
    ENetEvent event;
    while (enet_host_service(g_Net.client.host, &event, 0) > 0)
    {
        switch (event.type)
        {
        case ENET_EVENT_TYPE_CONNECT:
            printf("[Client] connected to host %x:%u.\n", event.peer->address.host,
                   event.peer->address.port);
            event.peer->data = "Host";
            break;
        case ENET_EVENT_TYPE_RECEIVE:
            printf("[Client] Received: %s\n", event.packet->data);
            enet_packet_destroy(event.packet);
            break;
        case ENET_EVENT_TYPE_DISCONNECT:
            printf("[Client] %s disconnected.\n", (char *)event.peer->data);
            event.peer->data = NULL;
            g_Net.client.peer = NULL;
        default:
            break;
        }
    }
}

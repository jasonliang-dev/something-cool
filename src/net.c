#include "net.h"
#include "net_server.h"
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
        void (*OnConnect)(NetError);
        void (*OnReceive)(ENetEvent);
        void (*OnDisconnect)(void);
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
        printf("[Client] Created net client\n");
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

    printf("[Server] Created server\n");
    return NULL;
}

void CloseServer(void)
{
    if (!g_Net.server.host)
    {
        return;
    }

    for (size_t i = 0; i < g_Net.server.host->peerCount; ++i)
    {
        ENetPeer *peer = g_Net.server.host->peers + i;
        if (peer->data)
        {
            free(peer->data);
        }
        enet_peer_disconnect(peer, 0);
    }

    enet_host_flush(g_Net.server.host);
    enet_host_destroy(g_Net.server.host);
    g_Net.server.host = NULL;
    printf("[Server] Closed server\n");
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
            ServerHandleConnect(g_Net.server.host, event);
            break;
        case ENET_EVENT_TYPE_RECEIVE:
            ServerHandleReceive(g_Net.server.host, event);
            break;
        case ENET_EVENT_TYPE_DISCONNECT:
            ServerHandleDisconnect(g_Net.server.host, event);
        default:
            break;
        }
    }
}

void ClientConnect(const char *hostAddress, u16 port, void (*OnConnect)(NetError))
{
    g_Net.client.OnConnect = NULL;

    if (g_Net.client.peer)
    {
        OnConnect("Already connected/connecting to another peer");
    }

    ENetAddress address;
    if (enet_address_set_host(&address, hostAddress) != 0)
    {
        OnConnect("Could not resolve host");
    }
    address.port = port;

    g_Net.client.peer = enet_host_connect(g_Net.client.host, &address, 2, 0);
    if (!g_Net.client.peer)
    {
        OnConnect("Could not connect to peer");
    }

    g_Net.client.OnConnect = OnConnect;
}

void ClientDisconnect(void)
{
    if (!g_Net.client.peer)
    {
        printf("[Client] Cannot disconnect to peer. Not connected.");
        return;
    }

    enet_peer_disconnect(g_Net.client.peer, 0);
    g_Net.client.peer = NULL;
    printf("[Client] Disconnected from peer\n");
}

void ClientForceDisconnect(void)
{
    if (!g_Net.client.peer)
    {
        printf("[Client] Cannot reset peer. Not connected.");
        return;
    }

    enet_peer_reset(g_Net.client.peer);
    g_Net.client.peer = NULL;
    printf("[Client] Forceful disconnect from peer\n");
}

NetError ClientSend(NetMessage msg)
{
    if (!g_Net.client.peer)
    {
        return "Not connected to a peer";
    }

    ENetPacket *packet = enet_packet_create(&msg, sizeof(NetMessage), 0);

    if (enet_peer_send(g_Net.client.peer, 0, packet) != 0)
    {
        return "Cannot queue the packet to be sent";
    }

    return NULL;
}

void ClientReceiveCallback(void (*OnReceive)(ENetEvent))
{
    g_Net.client.OnReceive = OnReceive;
}

void ClientOnDisconnect(void (*OnDisconnect)(void))
{
    g_Net.client.OnDisconnect = OnDisconnect;
}

void ClientPollEvents(void)
{
    ENetEvent event;
    while (enet_host_service(g_Net.client.host, &event, 0) > 0)
    {
        switch (event.type)
        {
        case ENET_EVENT_TYPE_CONNECT: {
            printf("[Client] connected to host %x:%u.\n", event.peer->address.host,
                   event.peer->address.port);
            event.peer->data = "Host";
            if (g_Net.client.OnConnect)
            {
                g_Net.client.OnConnect(NULL);
                g_Net.client.OnConnect = NULL;
            }
            else
            {
                printf("[Client] Warning: expected an OnConnect callback");
            }
            break;
        }
        case ENET_EVENT_TYPE_RECEIVE: {
            if (g_Net.client.OnReceive)
            {
                g_Net.client.OnReceive(event);
            }
            else
            {
                printf("[Client] Warning: expected an OnRecieve callback");
            }

            enet_packet_destroy(event.packet);
            break;
        }
        case ENET_EVENT_TYPE_DISCONNECT: {
            printf("[Client] %s disconnected.\n", (char *)event.peer->data);
            if (g_Net.client.OnDisconnect)
            {
                g_Net.client.OnDisconnect();
                g_Net.client.OnDisconnect = NULL;
            }
            event.peer->data = NULL;
            g_Net.client.peer = NULL;
            break;
        }
        default:
            break;
        }
    }
}

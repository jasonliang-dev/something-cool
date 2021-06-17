#pragma once

#include <enet/enet.h>

void ServerHandleConnect(ENetHost *host, ENetEvent event);
void ServerHandleReceive(ENetHost *host, ENetEvent event);
void ServerHandleDisconnect(ENetHost *host, ENetEvent event);

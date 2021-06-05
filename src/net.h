#pragma once

#include <enet/enet.h>

void ServerPollEvents(ENetHost *server);
void ClientPollEvents(ENetHost *client);

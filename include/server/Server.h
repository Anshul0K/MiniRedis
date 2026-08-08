#pragma once

#include <netinet/in.h>
#include "storage/Database.h"
#include "persistence/AOFManager.h"
#include <thread>
#include <atomic>

class Server
{
public:
    Server();
    void start();

private:
    void handleClient(int clientSocket);
    void snapshotLoop();

    std::atomic<bool> running;
    std::thread snapshotThread;

    int serverSocket;
    Database database;
    sockaddr_in serverAddress;
    AOFManager aofManager;
};
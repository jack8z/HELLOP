#pragma once

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include <functional>
#include <string>
#include <fstream>
#include <windows.h>

// gdi+ ++++++++++
#define NOMINMAX
#include <algorithm>
namespace Gdiplus
{
  using std::min;
  using std::max;
};
#ifndef ULONG_PTR   
#define ULONG_PTR unsigned long* 
#endif
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment(lib, "gdiplus.lib")
// gdi+ ----------

#include "easylogging++.h"

typedef websocketpp::server<websocketpp::config::asio> WebSocketServer;

class TodopServer
{
public:
    TodopServer();
    ~TodopServer();

    void OnMessage(websocketpp::connection_hdl hdl, WebSocketServer::message_ptr msg);

    void DoRun();

    int DoPrint();

private:
    int m_port; // WebSocket服务的端口号，默认：30303.
    int m_status = -1; // WebSocket服务的状态：-1:未启动;0:运行中;

    WebSocketServer m_server;
};

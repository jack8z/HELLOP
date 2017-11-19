#pragma once

#include "afxTodop.h"

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

namespace hellop {
    typedef websocketpp::server<websocketpp::config::asio> WebSocketServer;

    class TodopServer
    {
    public:
        TodopServer();
        ~TodopServer();

        void OnMessage(websocketpp::connection_hdl hdl, WebSocketServer::message_ptr msg);

        void DoRun();

        // int DoPrint();

    private:
        int m_port; // WebSocket服务的端口号，默认：30303.
        int m_status = -1; // WebSocket服务的状态：-1:未启动;0:运行中;

        WebSocketServer m_server;
    };
}

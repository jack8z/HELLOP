#include "TodopServer.h"
#include "strsafe.h"
#include "GdiplusPrintEngine.h"

#include "easylogging++.h"

hellop::TodopServer::TodopServer() {
    m_port = 30303;

    // m_server.get_alog().set_ostream(&m_logFileStream);
   
    // Set logging settings
    m_server.set_error_channels(websocketpp::log::elevel::all);
    m_server.set_access_channels(websocketpp::log::alevel::all ^ websocketpp::log::alevel::frame_payload);

    // Initialize Asio
    m_server.init_asio();

    // Set the default message handler to the echo handler
    m_server.set_message_handler(std::bind(
        &TodopServer::OnMessage, this,
        std::placeholders::_1, std::placeholders::_2
    ));
}

hellop::TodopServer::~TodopServer() {
    // do nothing
}

void hellop::TodopServer::OnMessage(websocketpp::connection_hdl hdl, websocketpp::server<websocketpp::config::asio>::message_ptr msg) {
    LOG(DEBUG) << "on_message called with hdl: " << hdl.lock().get()
        << " and message: " << msg->get_payload()
        << std::endl;

    // 返回的消息
    std::string returnMsg = msg->get_payload();

    // check for a special command to instruct the server to stop listening so
    // it can be cleanly exited.
    if (msg->get_payload() == "stop-listening") {
        m_server.stop_listening();
        return;
    }

    if (msg->get_payload() == "get-fonts") {
        GdiplusPrintEngine printEngine;
        std::list<TString> fontList = printEngine.getSystemFontFamilys();

        TString fontsBuff;
        for (std::list<TString>::iterator it=fontList.begin(); it!=fontList.end(); ++it) {
            fontsBuff += *it;
            if (it!=fontList.end()) {
                fontsBuff += L",";
            }
        }

        LOG(DEBUG) << fontsBuff << endl;

        returnMsg = todop_to_string(fontsBuff);
    }

    if (msg->get_payload() == "get-printers") {
        GdiplusPrintEngine printEngine;
        std::list<TString> printerList = printEngine.getLocalPrinters();

        TString printersBuff;
        for (std::list<TString>::iterator it=printerList.begin(); it!=printerList.end(); ++it) {
            printersBuff += *it;
            if (it!=printerList.end()) {
                printersBuff += L",";
            }
        }

        LOG(DEBUG) << printersBuff << endl;

        returnMsg = todop_to_string(printersBuff);
    }

    if (msg->get_payload() == "do-print") {
        GdiplusPrintEngine printEngine;
        int printResult = printEngine.doPrint();

        if (printResult>=0) {
			returnMsg = todop_to_string(L"打印成功");
		} else {
			returnMsg = todop_to_string(L"打印失败！");
		}
    }

    try {
        // write a new message
        m_server.send(hdl, returnMsg, msg->get_opcode());
    } catch (websocketpp::lib::error_code const & e) {
        LOG(ERROR) << "Echo failed because: " << e << "(" << e.message() << ")" << std::endl;
    }
}

void hellop::TodopServer::DoRun() {
    // Listen on port 30303
    m_server.listen(m_port);

    // Queues a connection accept operation
    m_server.start_accept();

    LOG(DEBUG) << "TodopServer start:" << m_port << std::endl;

    // Start the Asio io_service run loop
    m_server.run();
}

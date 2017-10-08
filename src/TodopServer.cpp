#include "TodopServer.h"

INITIALIZE_EASYLOGGINGPP

TodopServer::TodopServer() {
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

TodopServer::~TodopServer() {
    // do nothing
}

void TodopServer::OnMessage(websocketpp::connection_hdl hdl, websocketpp::server<websocketpp::config::asio>::message_ptr msg) {
    LOG(DEBUG) << "on_message called with hdl: " << hdl.lock().get()
        << " and message: " << msg->get_payload()
        << std::endl;

    // check for a special command to instruct the server to stop listening so
    // it can be cleanly exited.
    if (msg->get_payload() == "stop-listening") {
        m_server.stop_listening();
        return;
    }

    if (msg->get_payload() == "do-print") {
        DoPrint();
    }

    try {
        // write a new message
        m_server.send(hdl, msg->get_payload(), msg->get_opcode());
    } catch (websocketpp::lib::error_code const & e) {
        LOG(ERROR) << "Echo failed because: " << e << "(" << e.message() << ")" << std::endl;
    }
}

void TodopServer::DoRun() {
    // Listen on port 30303
    m_server.listen(m_port);

    // Queues a connection accept operation
    m_server.start_accept();

    // Start the Asio io_service run loop
    m_server.run();
}

int TodopServer::DoPrint(){
    // Initialize GDI+.
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    DWORD size;
    HDC hdcPrint;

    DOCINFO docInfo;
    ZeroMemory(&docInfo, sizeof(docInfo));
    docInfo.cbSize = sizeof(docInfo);
    docInfo.lpszDocName = "GdiplusPrint";

    // Get the size of the default printer name.
    GetDefaultPrinter(NULL, &size);

    // Allocate a buffer large enough to hold the printer name.
    TCHAR* buffer = new TCHAR[size];

    // Get the printer name.
    if(!GetDefaultPrinter(buffer, &size))
    {
        LOG(DEBUG) << "GetDefaultPrinter Failure" << std::endl;
    }
    else
    {
        // Get a device context for the printer.
        hdcPrint = CreateDC(NULL, buffer, NULL, NULL);

        StartDoc(hdcPrint, &docInfo);
        Graphics* graphics;
        Pen* pen = new Pen(Color(255, 0, 0, 0));

        StartPage(hdcPrint);
        graphics = new Graphics(hdcPrint);
        graphics->DrawRectangle(pen, 50, 50, 200, 300);
        // graphics->DrawString(_T("您好，World!"));//http://blog.csdn.net/LorenLiu/article/details/2894170
        delete graphics;
        EndPage(hdcPrint);

        StartPage(hdcPrint);
        graphics = new Graphics(hdcPrint);
        graphics->DrawEllipse(pen, 50, 50, 200, 300);
        delete graphics;
        EndPage(hdcPrint);

        delete pen;
        EndDoc(hdcPrint);

        DeleteDC(hdcPrint);
    }

    delete buffer;

    GdiplusShutdown(gdiplusToken);

    return 0;
}

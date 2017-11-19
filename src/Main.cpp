#pragma once

#include "afxTodop.h"
#include "MainFrameWnd.h"
#include "TodopServer.h"

#include "easylogging++.h"
INITIALIZE_EASYLOGGINGPP

// WebSocket服务的线程主体
unsigned __stdcall TodopServerRun(void* pParam){
    {
        // MessageBox(NULL, _T("TodopServerRun begin..."), NULL, MB_OK);

        hellop::TodopServer todopServer;
        todopServer.DoRun();
        
        // MessageBox(NULL, _T("TodopServerRun end."), NULL, MB_OK);
    }

    _endthread(); // 防止内存泄露，函数主体代码要包在大括号内
    return 0;
}

// 创建运行WebSocket服务的线程
unsigned CreateWebSocketThread(LPVOID lpParam){
    std::printf("CreateWebSocketThread...");

    unsigned threadID;

    HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, &TodopServerRun, NULL, 0, &threadID);
    
    CloseHandle(hThread);

    return 0;
}

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    CreateWebSocketThread(NULL);// 创建运行WebSocket服务的线程

    CPaintManagerUI::SetInstance(hInstance);

    CMainFrameWnd mainFrame;
    mainFrame.Create(NULL, _T("测试"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
    mainFrame.CenterWindow();
    mainFrame.ShowModal();
    return 0;
}
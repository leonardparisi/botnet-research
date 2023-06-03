#pragma once
#include "abstract_bot.h"

class RatBot : public ABot {
public:
    RatBot(const char* str_host, const char* str_port)
        : ABot(str_host, str_port)
    {
    }

    RatBot()
        : ABot()
    {
    }

    virtual void on_client_connect()
    {
        ABot::on_client_connect();
        std::cout << "Client connected!" << std::endl;
    }

    virtual void on_client_rec_data(char* data)
    {
        std::cout << "Client received data:" << std::endl;
        std::cout << data << std::endl;
    }
};
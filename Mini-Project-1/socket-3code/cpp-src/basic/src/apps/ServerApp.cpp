#include<iostream>
#include "socket/server.hpp"
#include "IO/input.hpp"
/**
 * @brief basic starting point 
 *
 *      Author: gash
 */
int main(int argc, char **argv) {

    std::string input, default_ip="127.0.0.1", default_name="CPP_SERVER";
    unsigned int default_port=2000;

    configure::input config;
    config.getInput(default_ip, default_port, default_name);
    std::cout<<"This is the client data : "<<config.name<<" "<<config.ip<<" "<<config.port<<std::endl;  
    
    basic::BasicServer svr(config.ip, config.port);
    svr.start();
}
#include <iostream>
#include <iomanip>
#include <sstream>
#include <thread>
#include <string>

#include "socket/client.hpp"
#include "IO/input.hpp"

/**
 * @brief basic starting point 
 *
 *      Author: gash
 */
int main(int argc, char **argv) {
    std::stringstream msg;
    std::string input, default_ip="127.0.0.1", default_name="CPP_CLIENT";
    unsigned int default_port=2000;


    configure::input config;
    config.getInput(default_ip, default_port, default_name);
    std::cout<<"This is the client data : "<<config.name<<" "<<config.ip<<" "<<config.port<<std::endl;  
    
    basic::BasicClient clt(config.name, config.ip, config.port);
    clt.connect();
    
    
    while(input!="exit"){        
        std::cout<< "Enter a message (to end type exit): ";
        std::getline(std::cin, input);
        if(input!=""){
            msg << input << std::ends;
            clt.sendMessage(msg.str());
            msg.str("");
        }
    }
     
    std::cout << "sleeping a bit before exiting..." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
}

#include <iostream>
#include <iomanip>
#include <sstream>
#include <thread>
#include <string>

#include "socket/client.hpp"

/**
 * @brief basic starting point 
 *
 *      Author: gash
 */
int main(int argc, char **argv) {
    basic::BasicClient clt;
    clt.connect();

    
    std::stringstream msg;
    std::string input;
    while(input!="exit"){        
        std::cout<< "Enter a message (to end type exit): ";
        std::getline(std::cin, input);
        if(input!=""){
            msg << input << std::ends;
            clt.sendMessage(msg.str());
        }
    }
     
    std::cout << "sleeping a bit before exiting..." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
}

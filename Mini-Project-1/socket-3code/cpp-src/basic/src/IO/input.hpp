#include<iostream>
#include<vector>
#include<string>
#include <stdbool.h>
#include <limits.h>

//input client data
namespace configure{

class input{
  public:
    std::string ip, name, port_string;
    unsigned int port;

    void getInput(std::string default_ip="127.0.0.1", unsigned int default_port=2000, std::string default_name="CPP_CLIENT" ){
      std::cout<<"Please Enter Client Name  (DEFAULT:"<<default_name<<" )";
      getline(std::cin, name);
      std::cout<<"Please Enter the Server Address you want to connect (DEFAULT:"<<default_ip<<" )";
      getline(std::cin, ip);
      std::cout<<"Please Enter the Server Port Number (DEFAULT:"<<default_port<<" )";
      getline(std::cin, port_string);
      if(port_string!=""){
        unsigned long ul = std::stoul (port_string,nullptr,0);
        this->port = static_cast<unsigned int>(ul);
      }

      this->name = name=="" ? default_name : name; 
      this->ip = ip=="" ? default_ip : ip; 
      this->port = port_string=="" ? default_port : port; 
    }

};
} // configure


#include <thread>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sstream>
#include <iostream>

#include "socket/client.hpp"
#include "payload/basicbuilder.hpp"

basic::BasicClient::BasicClient(std::string name, std::string ipaddr, unsigned int port) {
      this->name = name;
      this->group = "public";
      this->ipaddr =ipaddr;
      this->portN = port;
      this->good = false;
      this->clt = -1;

      if (this->portN <= 1024)
         throw std::out_of_range("port must be greater than 1024");
}

void basic::BasicClient::stop() {
   std::cerr << "--> closing client connection" << std::endl;
   this->good = false;

   if (this->clt > -1) {
      ::close(this->clt);
      this->clt = -1;
   }
} 

void basic::BasicClient::join(std::string group){
   this->group = group;
}

bool basic::BasicClient::isServerAlive() {
    // Using a single null byte as the probe message
    char probe[6] = "0000,";
    // Attempt to send the probe message
    ssize_t sent = ::send(this->clt, probe, sizeof(probe), MSG_NOSIGNAL); // MSG_NOSIGNAL to prevent SIGPIPE on errors
    sent = ::send(this->clt, probe, sizeof(probe), MSG_NOSIGNAL); // MSG_NOSIGNAL to prevent SIGPIPE on errors
    if (sent == -1) {
        // If send returns -1, an error occurred, and we assume the server is not alive
        // You might also want to check for specific errors like EPIPE (broken pipe), ECONNRESET, etc.
      //   std::cerr << "Server check failed, errno = " << errno << std::endl;
        return false;
    }

    // If send does not return -1, we assume the server is alive
    // Note: This does not guarantee the server application is responsive, just that the TCP connection is intact
    return true;
}

void basic::BasicClient::sendMessage(std::string m) {
   if (!this->good) return;

   if (this->isServerAlive()) {
      basic::Message msg(this->name,this->group,m);
      basic::BasicBuilder bldr;
      auto payload = bldr.encode(msg); 
      auto plen = payload.length();

      while (this->good) {
         auto n = ::write(this->clt, payload.c_str(), plen);
         //auto n = ::send(this->clt, payload.c_str(), plen);
         
         if (n == -1) {
            std::cerr << "--> send() error for " << m << ", n = " << n << ", errno = " << errno << std::endl;
         } else if ( errno == ETIMEDOUT) { 
            // @todo send portion not sent!
            continue;
         } else if (payload.length() != (std::size_t)n) {
            // @todo hmmmm, houston we may have a problem
            std::stringstream err;
            err << "failed to fully send(), err = " << errno << std::endl;
            throw std::runtime_error(err.str());
         } else 
            std::cerr << "sent: " << payload << ", size: " << plen << ", errno: " << errno << std::endl;
      
         break;
      }
   } else {
      std::cerr << "Server is Down" << std::endl;
      // Add code here to handle the server being down, similar to the Python code
      this->stop();
      std::string inp;
      std::cout << "\nDo you want to reconnect? ('Y' or 'N'): ";
      std::cin >> inp;

      if (inp == "Y") {
         while (true) {
               try {
                  this->connect();
                  break;
               } catch (const std::runtime_error& e) {
                  std::cerr << "\nNot able to connect! Do you want to retry? ('Y' or 'N'): ";
                  std::cin >> inp;

                  if (inp != "Y") {
                     throw std::runtime_error("\nNo connection to server exists");
                  }
               }
         }
      } else {
         throw std::runtime_error("\nNo connection to server exists");
      }
    }
}

void basic::BasicClient::connect() {
   if (this->good) return;

   std::cerr << "connecting..." << std::endl;

   this->clt = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
   if (this->clt < 0) {
      std::stringstream err;
      err << "failed to create socket, err = " << errno << std::endl;
      throw std::runtime_error(err.str());
   }

   struct sockaddr_in serv_addr;
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = inet_addr(this->ipaddr.c_str());
   serv_addr.sin_port = htons(this->portN);

   auto stat = inet_pton(AF_INET, this->ipaddr.c_str(), &serv_addr.sin_addr);
   if (stat < 0) {
      throw std::runtime_error("invalid IP address");
   }
   
   stat = ::connect(this->clt, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
   // std::cout<<"This is the status: "<<stat<<std::endl;
   if (stat < 0) {
      std::stringstream err;
      err << "failed to connect() to server, err = " << errno << std::endl;
      throw std::runtime_error(err.str());
   }

   this->good = true;
}

#define WIN32_LEAN_AND_MEAN 
#include "pop3templ.hpp"
#include <iostream>
#include <boost/thread.hpp>
#include <boost/bind.hpp>

int main() {
  boost::asio::io_service  io_service;
  pop3_server<pop3_template_session>  pop3_serv( io_service, 110 );

  boost::thread  thr( boost::bind( &pop3_server<pop3_template_session>::run, &pop3_serv ) );
  getchar();
  io_service.stop();
  thr.join();
  return 0;
}



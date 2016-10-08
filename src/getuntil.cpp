//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Copyright (c) 2007-2009 OKI Miyuki (oki.miyuki at gmail dot com)
//
//---------------------------------------------------------------------------
// sample of getuntil.hpp
//---------------------------------------------------------------------------
#include "getuntil.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>

#include <boost/asio.hpp>

int main() {
  std::ifstream ifs( "src/getuntil.cpp", std::ios::binary );

  int i  =  0;
  std::string delm( "\r\n" );
  std::string line;
  while( ifs.good() ) {
    getuntil( ifs, line, delm );
    std::cout << std::setw(3) << ++i << ": " << line; 
  }

  std::cout << std::endl;
  std::cout << "Enter stringstream test" << std::endl;

  std::stringstream  ss;
  ss << "hi-ho, hi-ho!";
  ss << "It's home from work we go! hi-ho, hi-ho!...";

  while( ss.good() ) {
    getuntil( ss, line, "ho!" );
    std::cout << line << std::endl;
  }

  std::cout << std::endl;
  std::cout << "Enter boost::asio::streambuf test" << std::endl;

  boost::asio::streambuf  sb;

  std::ostream  ostrm( &sb );
  ostrm << "This is data from client\r\n";
  ostrm << "And flying send\r\n";

  std::istream  istrm( &sb );

  while( istrm.good() ) {
    getuntil( istrm, line, "\r\n" );
    std::cout << line;
  }
}


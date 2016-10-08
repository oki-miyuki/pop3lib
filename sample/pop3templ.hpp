#ifndef POP3TEMPLATE_HPP
#define POP3TEMPLATE_HPP
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//
// Copyright (c) 2006-2009 OKI Miyuki (oki.miyuki at gmail dot com)
//

#include <pop3parser.hpp>
#include <pop3server.hpp>

using namespace rfc::pop3;

class pop3_template_session : 
  public pop3_session< pop3_template_session >
{
public:
  //! constructor
  pop3_template_session(
    boost::asio::io_service& io_service
  ) : 
    pop3_session< pop3_template_session >( io_service )
  {
  }

  //! do PASS command
  void response_pass(
    const std::string& user,
    const std::string& pass
  ) {
    // check user and password
    // if invalid user or password then call state_.invalid_user() and response -ERR
    state_.into_transaction();
    send_single_response( true, "welcome! now in transaction" );
  }

  //! do STAT command
  void response_stat() {
    //send_single_response( true, "invalid response test\r\n" );
    send_single_response( true, "0 0 (0) messages (0) bytes" );
  }

  //! do LIST command
  void response_list(
    unsigned number
  ) {
    send_single_response( false, "invalid message number" );
  }

  //! do LIST command
  void response_list() {
    send_single_response( true, "0 [no messages]\r\n." );
  }

  //! do RETR command
  void response_retr( 
    unsigned rnum 
  ) {
    send_single_response( false, "invalid message number" );
  }

  //! do DELE command
  void response_dele( 
    unsigned dnum 
  ) {
    send_single_response( false, "invalid message number" );
  }

  //! do RSET command
  void response_rset() {
    //deleted_  =  false;
    send_single_response( true, "rollback and new transaction." );
  }

  //! do TOP command
  void response_top( 
    unsigned tnum, 
    unsigned wnum 
  ) {
    send_single_response( false, "invalid message number" );
  }

  //! do UIDL command
  void response_uidl( 
    unsigned unum 
  ) {
    send_single_response( false, "invalid message number" );
  }

  //! do UIDL command
  void response_uidl() {
    send_single_response( true, "0 no messages\r\n." );
  }

};

#endif // POP3TEMPLATE_HPP

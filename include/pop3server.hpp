#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)
#ifndef POP3SERVER_HPP
#define POP3SERVER_HPP
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//
// Copyright (c) 2006-2009 OKI Miyuki (oki.miyuki at gmail dot com)
//
#define DATE_TIME_INLINE
#include <string>
#include <iostream>
#include <algorithm>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>
#include <pop3parser.hpp>

namespace rfc {
namespace pop3 {

template <typename Pop3Session>
class pop3_session : 
  public boost::enable_shared_from_this< pop3_session<Pop3Session> >
{
public:
  typedef typename Pop3Session    session_type;

  //@{
  /*!
    Pop3Session must have these member functions.
  */
  //! do PASS command
  void response_pass( const std::string&, const std::string& );
  //! do LIST command
  void response_list( unsigned );
  //! do LIST command
  void response_list();
  //! do RETR command
  void response_retr( unsigned );
  //! do DELE command
  void response_dele( unsigned );
  //! do STAT command
  void response_stat();
  //! do RSET command
  void response_rset();
  //! do TOP command
  void response_top( unsigned, unsigned );
  //! do UIDL command
  void response_uidl( unsigned );
  //! do UIDL command
  void response_uidl();
  //@}
  //@{
  /*!
    Pop3Session may have these member functions.
  */
  //! return pop3 server connection string
  const std::string get_connection_string();
  //! do USER command
  void response_user( const std::string& );
  //! do APOP command
  void response_apop( const std::string&, const std::string& );
  //! do NOOP command
  void response_noop();
  //! do QUIT command
  void response_quit();
  //@}

public:
  //! constructor
  pop3_session( boost::asio::io_service& );

  //! start handler
  void start();
  //! get boost::asio::ip::tcp::socket
  boost::asio::ip::tcp::socket& socket();

protected:
  //! handling after "QUIT" command. (do quit)
  void handle_quit( const boost::system::error_code& );
  //! handling read completion.
  void handle_read( const boost::system::error_code& );
  //! send single line response.
  void send_single_response( bool, const std::string& );
  //! write quit response line. next do is disconnect.
  void send_quit_response( const std::string& );
  //! check session timeout and do.
  void handle_expire( const boost::system::error_code& );
  //! read client input. and reroute handle_command.
  void handle_wrote( const boost::system::error_code& );
  //! reset timer count. and reserve timeout check (= handle_expire).
  void reset_timer();

private:
  //! parse "pop3 command" and call command.
  void parse_command( const std::string& );

protected:
  pop3_state                        state_;
  boost::asio::io_service&          io_service_;
  boost::asio::ip::tcp::socket      socket_;
  boost::asio::deadline_timer       timer_;
  boost::posix_time::time_duration  session_time_duration_;
  std::string                       user_account_;
  boost::asio::streambuf            request_;
  boost::asio::streambuf            response_;

};

//typedef typename boost::shared_ptr< pop3_session<Pop3Session> >  session_ptr;

#include <pop3session.ipp>

//! pop3 server class
template <typename Pop3Session>
class pop3_server {
private:
  //! final pop3 session class type
  typedef typename Pop3Session pop3_session_type;
  typedef typename boost::shared_ptr<Pop3Session>  pop3_session_type_ptr;
  //! bind client connection 
  void bind_accept( pop3_session_type_ptr );
  //! handle first contact from pop3 client mailer.
  void handle_accept( pop3_session_type_ptr, const boost::system::error_code& );

public:
  //! constructor
  pop3_server( boost::asio::io_service&, short );

  //! start thread
  void start();
  //! run thread
  void run();
  //! stop thread
  void stop();

private:
  std::auto_ptr<boost::thread>    runner_;
  boost::asio::io_service&        io_service_;
  boost::asio::ip::tcp::acceptor  acceptor_;
};

#include <pop3server.ipp>

}  // namespace pop3
}  // namespace rfc

#endif  // POP3SERVER_HPP

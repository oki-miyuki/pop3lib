//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//
// Copyright (c) 2006-2009 OKI Miyuki (oki.miyuki at gmail dot com)
//

//namespace rfc {
//namespace pop3 {

//! bind to async accept handler
template <typename Pop3Session>
void pop3_server<Pop3Session>::bind_accept( 
  pop3_session_type_ptr session 
) {
  acceptor_.async_accept(
    session->socket(),
    boost::bind( 
      &pop3_server::handle_accept,
      this,
      session,
      boost::asio::placeholders::error
    )
  );
}

//! constructor
template <typename Pop3Session>
pop3_server<Pop3Session>::pop3_server( 
  boost::asio::io_service& io_service,
  short port
) :
  io_service_(io_service),
  acceptor_(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
{
  pop3_session_type_ptr new_session( new pop3_session_type( io_service ) );
  bind_accept( new_session );
}

//! handle first contact from pop3 client mailer.
template <typename Pop3Session>
void pop3_server<Pop3Session>::handle_accept( 
  pop3_session_type_ptr    new_session,
  const boost::system::error_code&  error
) {
  if( !error ) {
  std::cout << "hi" << std::endl;
    new_session->start();
    new_session.reset( new pop3_session_type( io_service_ ) );
    bind_accept( new_session );
  }
}

//! run thread
template <typename Pop3Session>
void pop3_server<Pop3Session>::run() {
  io_service_.run();
}

//! start thread
template <typename Pop3Session>
void pop3_server<Pop3Session>::start() {
  BOOST_ASSERT( !runner_.get() );
  runner_.reset( 
    new boost::thread(
      boost::bind( &pop3_server<Pop3Session>::run, this )
    )
  );
}

//! stop thread
template <typename Pop3Session>
void pop3_server<Pop3Session>::stop() {
  BOOST_ASSERT( runner_.get() );
  io_service_.interrupt();
  runner_.join();
  runner_.reset();
}

//}  // namespace pop3
//}  // namespace rfc

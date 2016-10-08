//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//
// Copyright (c) 2006-2007 OKI Miyuki (oki.miyuki at gmail dot com)
//

#include <getuntil.hpp>

//namespace rfc {
//namespace pop3 {

//! constructor
template <typename Pop3Session>
pop3_session<Pop3Session>::pop3_session(
	boost::asio::io_service&	io_service
) : 
	io_service_(io_service),
	socket_(io_service),
	timer_(io_service),
	//session_time_duration_(boost::posix_time::seconds(600)),
	session_time_duration_(boost::posix_time::seconds(15)),
	state_(false)
{
}

//! start handler
template <typename Pop3Session>
void pop3_session<Pop3Session>::start() {
	session_type* parent	=	static_cast<session_type*>(this);
	send_single_response( true, parent->get_connection_string() );
}

//! handling after "QUIT" command. (do quit)
template <typename Pop3Session>
void pop3_session<Pop3Session>::handle_quit( 
	const boost::system::error_code& error
) {
	socket_.close();
}

//! handling read completion.
template <typename Pop3Session>
void pop3_session<Pop3Session>::handle_read( 
	const boost::system::error_code& error
) {
	if( !error ) {
		std::istream	request_stream( &request_ );
		std::string command;
		getuntil( request_stream, command, "\r\n" );
		parse_command( command );
	}
}

//! parse "pop3 command" and call command.
template <typename Pop3Session>
void pop3_session<Pop3Session>::parse_command( 
	const std::string& command	//!< [in] command line input
) {
	//std::cout << command;
	pop3_command_grammar<char const*>	pcg( state_ );
	boost::spirit::classic::parse_info<> info	=	boost::spirit::classic::parse( command.c_str(), pcg );
	session_type* parent	=	static_cast<session_type*>(this);
	// parse command
	switch( state_.cmd_type_ ) {
	case pop3_cmd_user:	parent->response_user( state_.arg<std::string>() );	break;
	case pop3_cmd_pass:	parent->response_pass( user_account_, state_.arg<std::string>() );	break;
	case pop3_cmd_noop:	parent->response_noop();	break;
	case pop3_cmd_rset:	parent->response_rset();	break;
	case pop3_cmd_stat:	parent->response_stat();	break;
	case pop3_cmd_list: {
		if( state_.is_arg_empty() )	parent->response_list();
		else												parent->response_list( state_.arg<unsigned>() );
		break;
											}
	case pop3_cmd_uidl: {
		if( state_.is_arg_empty() )	parent->response_uidl();
		else												parent->response_uidl( state_.arg<unsigned>() );
		break;
											}
	case pop3_cmd_apop:	parent->response_apop( state_.arg<std::string>(), state_.arg<std::string>() );	break;
	case pop3_cmd_dele:	parent->response_dele( state_.arg<unsigned>() );	break;
	case pop3_cmd_top:	parent->response_top( state_.arg<unsigned>(), state_.arg<unsigned>() );		break;
	case pop3_cmd_retr:	parent->response_retr( state_.arg<unsigned>() );	break;
	case pop3_cmd_quit:	parent->response_quit();	break;
	default: {
		if( !state_.in_transaction() )	state_.invalidate_user();
		send_single_response( false, state_.result_ );
					}
		break;
	}
}

//! return pop3 server connection string
template <typename Pop3Session>
const std::string pop3_session<Pop3Session>::get_connection_string() {
	return std::string( "Hello Pop3 Client" );
}

//! do USER command
template <typename Pop3Session>
void pop3_session<Pop3Session>::response_user(
	const std::string& user	//!< [in] mail accout (user) name
) {
	// later check user and password. now validate user input by temporary
	user_account_	=	user;		state_.validate_user();
	send_single_response( true, "input PASS command" );
}

//! do PASS command
template <typename Pop3Session>
void pop3_session<Pop3Session>::response_pass(
	const std::string& user,	//!< [in] mail account (user) name
	const std::string& pass		//!< [in] mail account (user) password
) {
	BOOST_ASSERT( !"no implementation in Pop3Session" );
}

//! do APOP command
template <typename Pop3Session>
void pop3_session<Pop3Session>::response_apop(
	const std::string& user, 
	const std::string& digest
) {
	BOOST_ASSERT( !"no implementation in Pop3Session" );
}

//! do STAT command
template <typename Pop3Session>
void pop3_session<Pop3Session>::response_stat() {
	BOOST_ASSERT( !"no implementation in Pop3Session" );
}

//! do LIST command
template <typename Pop3Session>
void pop3_session<Pop3Session>::response_list(
	unsigned number
) {
	BOOST_ASSERT( !"no implementation in Pop3Session" );
}

//! do LIST command
template <typename Pop3Session>
void pop3_session<Pop3Session>::response_list() {
	BOOST_ASSERT( !"no implementation in Pop3Session" );
}

//! do QUIT command
template <typename Pop3Session>
void pop3_session<Pop3Session>::response_quit() {
	std::ostream	response_stream( &response_ );
	response_stream << "+OK good bye\r\n";
	boost::asio::async_write(
		socket_, response_,
		boost::bind( &session_type::handle_quit, shared_from_this(), boost::asio::placeholders::error )
	);
}

//! do RETR command
template <typename Pop3Session>
void pop3_session<Pop3Session>::response_retr( 
	unsigned rnum 
) {
	BOOST_ASSERT( !"no implementation in Pop3Session" );
}

//! do DELE command
template <typename Pop3Session>
void pop3_session<Pop3Session>::response_dele( 
	unsigned dnum 
) {
	BOOST_ASSERT( !"no implementation in Pop3Session" );
}

//! do NOOP command
template <typename Pop3Session>
void pop3_session<Pop3Session>::response_noop() {
	// automatically resets for each coomand
	send_single_response( true, "reset auto disconnect timer" );
}

//! do RSET command
template <typename Pop3Session>
void pop3_session<Pop3Session>::response_rset() {
	BOOST_ASSERT( !"no implementation in Pop3Session" );
}

//! do TOP command
template <typename Pop3Session>
void pop3_session<Pop3Session>::response_top( 
	unsigned tnum, 
	unsigned wnum 
) {
	BOOST_ASSERT( !"no implementation in Pop3Session" );
}

//! do UIDL command
template <typename Pop3Session>
void pop3_session<Pop3Session>::response_uidl( 
	unsigned unum 
) {
	BOOST_ASSERT( !"no implementation in Pop3Session" );
}

//! do UIDL command
template <typename Pop3Session>
void pop3_session<Pop3Session>::response_uidl() {
	BOOST_ASSERT( !"no implementation in Pop3Session" );
}

//! get boost::asio::stream_socket
/*!
	@retval this session socket
*/
template <typename Pop3Session>
boost::asio::ip::tcp::socket& pop3_session<Pop3Session>::socket() { 
	return socket_; 
}

//! send single line response.
template <typename Pop3Session>
void pop3_session<Pop3Session>::send_single_response( 
	bool success,	//!< [in] command result status. true: +OK, false: -ERR
	const std::string& msg	//!< [in] additional information
) {
	std::ostream	response_stream( &response_ );
	if( success )	response_stream << "+OK";
	else					response_stream << "-ERR";
	response_stream << " " << msg << "\r\n";
	boost::asio::async_write( 
		socket_, response_,
		boost::bind( &session_type::handle_wrote, shared_from_this(), boost::asio::placeholders::error )
	);
	reset_timer();
}

//! write quit response line. next do is disconnect.
/*!
	@attention this is final response of quit. next do is disconnect.
*/
template <typename Pop3Session>
void pop3_session<Pop3Session>::send_quit_response( 
	const std::string& response	//!< [in] command response
) {
	boost::asio::async_write(
		socket_, response_,
		boost::bind( &session_type::handle_quit, shared_from_this(), boost::asio::placeholders::error )
	);
	reset_timer();
}

//! check session timeout and do.
template <typename Pop3Session>
void pop3_session<Pop3Session>::handle_expire(
	const boost::system::error_code& error
) {
	if( error != boost::asio::error::operation_aborted ) {
		socket_.close();
	}
}

//! read client input. and reroute handle_command.
template <typename Pop3Session>
void pop3_session<Pop3Session>::handle_wrote(
	const boost::system::error_code&	error
) {
	if( !error ) {
		boost::asio::async_read_until( 
			socket_, request_, "\r\n",
			boost::bind(
				&session_type::handle_read, shared_from_this(),
				boost::asio::placeholders::error
			)
		);
		reset_timer();
	}
}

//! reset timer count. and reserve timeout check (= handle_expire).
template <typename Pop3Session>
void pop3_session<Pop3Session>::reset_timer() {
	timer_.cancel();
	timer_.expires_from_now(session_time_duration_);
	timer_.async_wait( 
		boost::bind( &session_type::handle_expire, shared_from_this(), boost::asio::placeholders::error )
	);
}

//}	// namespace pop3
//}	// namespace rfc

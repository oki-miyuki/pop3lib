#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)
#ifndef POP3_PARSER_HPP
#define POP3_PARSER_HPP
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//
// Copyright (c) 2006-2009 OKI Miyuki (oki.miyuki at gmail dot com)
//
#include <iostream>
#include <string>
#include <deque>
#include <boost/bind.hpp>
#include <boost/any.hpp>
#include <boost/spirit/include/classic.hpp>
#include <boost/logic/tribool.hpp>
#include <boost/assert.hpp>


// RFC 1939

namespace rfc {
namespace pop3 {

//using namespace boost;
//using namespace boost::spirit::classic;

//! parse POP3 connection string
boost::tribool accept_connection( const std::string&, std::string&, std::string& );  

//! parse STAT command response
boost::tribool accept_stat( const std::string&, unsigned&, unsigned&, std::string& );

//! parse simple. Judge command response is '+OK' or '-ERR' or others.
boost::tribool accept_simple( const std::string&, std::string& );

//! parse period. Judge (UIDL, LIST, TOP, RETR) command response lines after '+OK' is period.
bool is_period( const std::string& );

//! parse UIDL contigous response line
boost::tribool accept_contigous_uidl_line( const std::string&, unsigned&, std::string& );

//! parse LIST contigous response line
boost::tribool accept_contigous_list_line( const std::string&, unsigned&, unsigned& );

//! parse TOP | RETR contigous response line
bool accept_contigous_mail_line( const std::string&, std::string& );

//! pop3 command types
enum pop3_command_type {
  pop3_detect_error  =  0, pop3_cmd_user, pop3_cmd_pass, pop3_cmd_noop, pop3_cmd_rset,
  pop3_cmd_stat, pop3_cmd_list, pop3_cmd_uidl, pop3_cmd_apop, pop3_cmd_dele,
  pop3_cmd_top, pop3_cmd_retr, pop3_cmd_quit,
};

//! pop3 process types
enum pop3_process_type {
  pop3_process_connect, pop3_process_user, pop3_process_pass, pop3_process_apop,
  pop3_process_stat, pop3_process_list, pop3_process_uidl, pop3_process_top,
  pop3_process_retr, pop3_process_dele, pop3_process_rset, pop3_process_noop,
  pop3_process_quit,
};

//! pop3 state
/*!
  @note treat POP3 server's state and parser result.
*/
struct pop3_state {
  // for state
  bool                      apop_support_;    //!< true: APOP support, false: APOP unsupport
  bool                      in_transaction_;  //!< true: in transaction, false: out of transaction
  bool                      uname_active_;    //!< true: now USER command was accepted, false: other
  // results
  pop3_command_type         cmd_type_;
  std::string               result_;
  std::deque<boost::any>    argv_;

  //! constructor
  explicit pop3_state( bool apop_support );

  //! Take argument
  /*!
    @note Arguments are follow
      @li LIST: [a message-number (optional)]
      @li UIDL: [a message-number (optional)]
      @li RETR: [a message-number (required)]
      @li DELE: [a message-number (required)]
      @li TOP: [a message-number (required)] [lines (required)]
      @li USER: [a string identifying a mailbox (required)]
      @li PASS: [a server/mailbox-specific password (required)]
      @li APOP: [a string identifying a mailbox(required)] [a MD5 digest string(required)]
    @attention Optional argument is set to empty value. 
      You have to inspect the argument is empty or not with is_arg_empty, 
      on 'LIST' and 'UIDL' command.
      Arguments are queued FIFO.
    @retval any argument.
  */
  template <class T>
  T arg() {
    T result  =  boost::any_cast<T>( argv_.front() );
    argv_.pop_front();
    return result;
  }

  //! check argument is empty
  bool is_arg_empty();

  //! clear parser result
  void clear_result();
  //! change state to 'transaction mode.'
  void into_transaction();
  //! inspect state is 'transaction mode?'
  bool pop3_state::in_transaction() const;
  //! change state to 'accept PASS command.'
  void validate_user();
  //! change state to 'accept USER command.'
  void invalidate_user();
};

//! pop3 command symbols
struct pop3_command_symbols : 
  public boost::spirit::classic::symbols<pop3_command_type>
{
  //! constructor
  pop3_command_symbols();
};

//! pop3 command grammar class for pop3server
template <typename IterT>
class pop3_command_grammar :
  public boost::spirit::classic::grammar< pop3_command_grammar<IterT> >
{
public:
  pop3_state&            state_;
  pop3_command_symbols  pop3_command_p;

  pop3_command_grammar( pop3_state& );

  void set_command( pop3_command_type ct ) const;
  void set_uparam(unsigned uparam) const;
  void set_sparam( IterT const& begin, IterT const& end ) const;
  void set_default( IterT const&, IterT const& ) const;
  void set_invalid_command_msg( IterT const&, IterT const& ) const;
  void set_invalid_param_msg( IterT const&, IterT const& ) const;
  void set_apop_unsupport_msg( IterT const&, IterT const& ) const;
  void set_in_transaction_msg( IterT const&, IterT const& ) const;
  void set_out_transaction_msg( IterT const&, IterT const& ) const;
  void set_non_accept_user_msg( IterT const&, IterT const& ) const;
  void set_non_accept_pass_msg( IterT const&, IterT const& ) const;

  //! pop3 command scanner class for pop3server
  template <typename ScannerT>
  struct definition {
    boost::spirit::classic::rule<ScannerT>
      pop3_default_p, //!< for optional default unsigned argument rule
      pop3_uinput_p,  //!< unsigned argument rule
      pop3_string_p,  //!< string argument rule
      set_command_p,  //!< for set pop3_command_type
      error_user_p,   //!< for set user command error
      error_pass_p,   //!< for set pass command error
      error_apop_p,   //!< for set apop command error
      error_cmd_p,    //!< for set invalid command error
      error_param_p,  //!< for set invalid parameter error
      error_in_p,     //!< for set in transaction error
      error_out_p,    //!< for set out of transaction error
      pop3_user_p,    //!< USER 'NAME' command rule
      pop3_pass_p,    //!< PASS 'PASSWORD' command rule
      pop3_noop_p,    //!< NOOP command rule
      pop3_rset_p,    //!< RSET command rule
      pop3_stat_p,    //!< STAT command rule
      pop3_list_p,    //!< LIST ('MESSAGE') command rule
      pop3_uidl_p,    //!< UIDL ('MESSAGE') command rule
      pop3_apop_p,    //!< APOP 'NAME' 'DIGEST' command rule
      pop3_dele_p,    //!< DELE 'MESSAGE' command rule
      pop3_quit_p,    //!< QUIT command rule
      pop3_top_p,     //!< TOP 'MESSAGE' 'LINES' command rule
      pop3_retr_p,    //!< RETR 'MESSAGE' command rule
      expression;

    //! constructor
    definition( pop3_command_grammar<IterT> const& );
    //! start
    boost::spirit::classic::rule<ScannerT> const& start() const;
  };
};

#include <pop3parser.ipp>

} // namespace pop3
}  // namespace rfc

#endif  // POP3_PARSER_HPP

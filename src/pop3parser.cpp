//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//
// Copyright (c) 2006-2009 OKI Miyuki (oki.miyuki at gmail dot com)
//

#include <pop3parser.hpp>
#include <boost/regex.hpp>

// RFC 1939

namespace rfc {
namespace pop3 {

typedef std::pair<unsigned,std::string>  uidl_element_type;
typedef std::vector<uidl_element_type>  uidl_vector_type;

using namespace boost;
using namespace boost::spirit::classic;

namespace {
  const char ok_str[]    =  "+OK";
  const char err_str[]  =  "-ERR";
}

/*!
  @defgroup pop3client_parser POP3 parser for client
*/


//! parse POP3 connection string
/*!
  @note if POP3 server implements APOP, apop_word is set.
    otherwise apop_word is empty.
  @ingroup pop3client_parser
  @retval true SUCCEEDED(+OK)
  @retval false FAILED(-ERR)
  @retval boost::indeterminate Unformal response
*/
boost::tribool accept_connection(
  const std::string&  response,    //!< [in] connection string
  std::string&        apop_word,  //!< [out,ref] APOP private keyword
  std::string&        comment      //!< [out,ref] comment
) {
  rule<> expression  =  (str_p(ok_str) | str_p(err_str)) >> space_p 
    >> (*print_p)[assign(comment)] >> eol_p >> end_p;
  apop_word.clear();
  parse_info<>  hit  =  parse( response.c_str(), expression );
  if( hit.full ) {
    hit  =  parse( response.c_str(), str_p(ok_str) );
    if( hit.hit ) {
      boost::regex  r( "<[0-9]+\\.[0-9]+@([a-z0-9\\+\\-]+\\.)*[a-z0-9\\+\\-]+>" );
      boost::smatch  m;
      if( boost::regex_search( response, m, r ) ) {
        apop_word  =  m.str();
      } else {
        apop_word.clear();
      }
      return true;
    }
    return false;
  }
  return boost::indeterminate;
}

//! parse STAT command response
/*!
  @attention messages and total_bytes are sets only '+OK' condition.
    and comment is set only '-ERR' condition.
  @ingroup pop3client_parser
  @retval true SUCCEEDED(+OK)
  @retval false FAILED(-ERR)
  @retval boost::indeterminate Unformal response string
*/
boost::tribool accept_stat(
  const std::string&  response,      //!< [in] response string
  unsigned&            messages,      //!< [out,ref] avarable message count
  unsigned&            total_bytes,  //!< [out,ref] total message bytes
  std::string&        comment        //!< [out,ref] comment part
) {
  rule<> ok_expression  =  str_p(ok_str) >> space_p >> uint_p[assign(messages)] 
      >> space_p >> uint_p[assign(total_bytes)] >> eol_p >> end_p;
  rule<> err_expression  =  str_p(err_str) >> space_p >> (*print_p)[assign(comment)] >> eol_p >> end_p;
  parse_info<>  hit;
  hit  =  parse( response.c_str(), ok_expression );
  if( hit.full )  return true;
  hit  =  parse( response.c_str(), err_expression );
  if( hit.full )  return false;
  return boost::indeterminate;
}

//! parse simple. Judge command response is '+OK' or '-ERR' or others.
/*!
  @retval boost::indeterminate Unformal response string.
  @ingroup pop3client_parser
  @retval ture SUCCESS(+OK)
  @retval false FAILED(-ERR)
*/
boost::tribool accept_simple( 
  const std::string&  response,  //!< [in] response string
  std::string&  comment          //!< [out,ref] comment part of response
) {
  rule<> expression  =  (str_p(ok_str) | str_p(err_str)) 
    >> (anychar_p - eol_p)[assign(comment)] >> eol_p >> end_p;
  parse_info<>  hit;
  hit  =  parse( response.c_str(), str_p(ok_str) );
  if( hit.hit ) {
    parse( response.c_str(), expression );  // assign comment
    return true;
  }
  hit  =  parse( response.c_str(), expression );
  if( hit.full )  return false;
  return boost::indeterminate;
}

//! parse period. Judge (UIDL, LIST, TOP, RETR) command response lines after '+OK' is period.
/*!
  @note this function is not needed. because accept_contigous_* function return false
    if reached end of line.
  @ingroup pop3client_parser
  @retval true Just period!
  @retval false Contiguous line.
*/
bool is_period( 
  const std::string&  response  //!< [in] response string
) {
  parse_info<> hit  =  parse( response.c_str(), ('.' >> eol_p >> end_p) );
  return hit.full;
}

//! parse UIDL contigous response line
/*!
  @ingroup pop3client_parser
  @retval true SUCCEEDED. read more uidls.
  @retval false END OF LINE. no more uidls.
  @retval boost::indeterminate ILLIGAL! untrusted response.
*/
boost::tribool accept_contigous_uidl_line( 
  const std::string&  response,  //!< [in] response string
  unsigned&            number,    //!< [out,ref] uidl number
  std::string&        uidl      //!< [out,ref] uidl string
) {
  if( is_period( response ) )  return false;
  rule<> expression = uint_p[assign(number)] >> space_p
                >> repeat_p(1,70)[range_p<char>(0x21,0x7e)][assign(uidl)] >> eol_p >> end_p;
  parse_info<> hit  =  parse( response.c_str(), expression );
  if( hit.full )  return true;
  return boost::indeterminate;
}

//! parse LIST contigous response line
/*!
  @ingroup pop3client_parser
  @retval true SUCCEEDED. read more lists.
  @retval false END OF LINE. no more lists.
  @retval boost::indeterminate ILLIGAL! untrusted response.
*/
boost::tribool accept_contigous_list_line( 
  const std::string&  response,  //!< [in] response string
  unsigned&            number,    //!< [out,ref] list number
  unsigned&            bytes      //!< [out,ref] bytes
) {
  if( is_period( response ) )  return false;
  rule<> expression  =  uint_p[assign(number)] >> space_p >> uint_p[assign(bytes)] >> eol_p >> end_p;
  parse_info<> hit  =  parse( response.c_str(), expression );
  if( hit.full )  return true;
  return boost::indeterminate;
}

//! parse TOP | RETR contigous response line
/*!
  @ingroup pop3client_parser
  @retval true SUCCEEDED. read more lines.
  @retval false END OF LINE. no more lines.
*/
bool accept_contigous_mail_line( 
  const std::string&  response,  //!< [in] response string
  std::string&        line      //!< [out,ref] line
) {
  if( is_period( response ) )  return false;
  line  =  ".";
  rule<> expression = (str_p("..") >> eol_p >> end_p)
    | ((*(anychar_p - eol_p))[assign(line)] >> eol_p >> end_p);
  parse_info<> hit  =  parse( response.c_str(), expression );
  BOOST_ASSERT( hit.full );
  return true;
}

//! constructor
pop3_state::pop3_state( 
  bool  apop_support  //!< [in] T: support APOP, F: not support APOP
) :
  apop_support_( apop_support ),
  in_transaction_(false),
  uname_active_(false),
  cmd_type_(pop3_detect_error),
  result_(),
  argv_()
{
}

//! clear parser result
void pop3_state::clear_result() { 
  result_.clear(); 
  argv_.clear();
}

//! check argument is empty
/*!
  @attention If the argument is empty, the argument is poped.
  @retval true empty
  @retval false not empty
*/
bool pop3_state::is_arg_empty() {
  bool result  =  argv_.front().empty();
  if( result )  argv_.pop_front();
  return result;
}

//! change state to 'transaction mode.'
void pop3_state::into_transaction() { in_transaction_ = true; }
//! inspect state is 'transaction mode?'
/*!
  @retval true now in transaction
  @retval false now out of transaction
*/
bool pop3_state::in_transaction() const { return in_transaction_; } 
//! change state to 'accept PASS command.'
void pop3_state::validate_user() { uname_active_ = true; }
//! change state to 'accept USER command.'
void pop3_state::invalidate_user() { uname_active_ = false; }

//! constructor
pop3_command_symbols::pop3_command_symbols() {
  add
    ("user", pop3_cmd_user)
    ("pass", pop3_cmd_pass)
    ("noop", pop3_cmd_noop)
    ("rset", pop3_cmd_rset)
    ("stat", pop3_cmd_stat)
    ("list", pop3_cmd_list)
    ("uidl", pop3_cmd_uidl)
    ("apop", pop3_cmd_apop)
    ("dele", pop3_cmd_dele)
    ("top" , pop3_cmd_top )
    ("retr", pop3_cmd_retr)
    ("quit", pop3_cmd_quit)
  ;
}

} // namespace pop3
}  // namespace rfc


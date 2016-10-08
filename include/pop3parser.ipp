//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//
// Copyright (c) 2006-2009 OKI Miyuki (oki.miyuki at gmail dot com)
//

//namespace rfc {
//namespace pop3 {

//! constructor
template <typename IterT>
pop3_command_grammar<IterT>::pop3_command_grammar( 
  pop3_state&  current
) : 
  state_(current),
  pop3_command_p()
{
  current.clear_result();
}

//! set 'pop3 command type' or 'detect error' to pop3_state structure.
template <typename IterT>
void pop3_command_grammar<IterT>::set_command( 
  pop3_command_type ct  //!< [in] pop3 command type
) const {
  state_.cmd_type_  =  ct;
}

//! append unsigned param to pop3_state structure.
template <typename IterT>
void pop3_command_grammar<IterT>::set_uparam(
  unsigned uparam  //!< [in] unsigned param
) const {
  state_.argv_.push_back( boost::any( uparam ) );
}

//! append string param to pop3_state structure.
template <typename IterT>
void pop3_command_grammar<IterT>::set_sparam( 
  IterT const& begin,  //!< [in] start position of string param
  IterT const& end     //!< [in] end position of string param
) const {
  std::string  sparam;
  sparam.assign( begin, end );
  state_.argv_.push_back( boost::any( sparam ) );
}

//! append default param (= empty) to pop3_state structure.
template <typename IterT>
void pop3_command_grammar<IterT>::set_default( 
  IterT const&,  //!< [in] dummy (not used)
  IterT const&  //!< [in] dummy (not used)
) const {
  state_.argv_.push_back( boost::any() );
}

//! set invalid_command_msg to pop3_state structure.
template <typename IterT>
void pop3_command_grammar<IterT>::set_invalid_command_msg( 
  IterT const&,  //!< [in] dummy (not used)
  IterT const&  //!< [in] dummy (not used)
) const {
  state_.cmd_type_  =  pop3_detect_error;
  state_.result_    =  "invalid command";
}

//! set invalid_param_msg to pop3_state structure.
template <typename IterT>
void pop3_command_grammar<IterT>::set_invalid_param_msg( 
  IterT const&,  //!< [in] dummy (not used)
  IterT const&  //!< [in] dummy (not used)
) const {
  state_.cmd_type_  =  pop3_detect_error;
  state_.result_    =  "invalid parameter";
}

//! set unsupport_apop_msg to pop3_state structure.
template <typename IterT>
void pop3_command_grammar<IterT>::set_apop_unsupport_msg( 
  IterT const&,  //!< [in] dummy (not used)
  IterT const&  //!< [in] dummy (not used)
) const {
  state_.cmd_type_  =  pop3_detect_error;
  state_.result_    =  "APOP is not supported";
}

//! set in_transaction_msg to pop3_state structure.
template <typename IterT>
void pop3_command_grammar<IterT>::set_in_transaction_msg( 
  IterT const&,  //!< [in] dummy (not used)
  IterT const&  //!< [in] dummy (not used)
) const {
  state_.cmd_type_  =  pop3_detect_error;
  state_.result_    =  "in transaction now... command not valid here.";
}

//! set out_transaction_msg to pop3_state structure.
template <typename IterT>
void pop3_command_grammar<IterT>::set_out_transaction_msg( 
  IterT const&,  //!< [in] dummy (not used)
  IterT const&  //!< [in] dummy (not used)
) const {
  state_.cmd_type_  =  pop3_detect_error;
  state_.result_    =  "out of transaction now... command not valid here.";
}

//! set non_accept_user_msg to pop3_state structure.
template <typename IterT>
void pop3_command_grammar<IterT>::set_non_accept_user_msg( 
  IterT const&,  //!< [in] dummy (not used)
  IterT const&  //!< [in] dummy (not used)
) const {
  state_.cmd_type_  =  pop3_detect_error;
  state_.result_    =  "USER command is not accepted... command not valid here.";
}

//! set non_accept_pass_msg to pop3_state structure.
template <typename IterT>
void pop3_command_grammar<IterT>::set_non_accept_pass_msg( 
  IterT const&,  //!< [in] dummy (not used)
  IterT const&  //!< [in] dummy (not used)
) const {
  state_.cmd_type_  =  pop3_detect_error;
  state_.result_    =  "PASS command is not accepted... command not valid here.";
}

//! constructor
template <typename IterT>
template <typename ScannerT>
pop3_command_grammar<IterT>::definition<ScannerT>::definition(
  pop3_command_grammar<IterT> const& self
) {
  using namespace boost::spirit::classic;
  // set error
  error_user_p   =  eps_p[ boost::bind( &pop3_command_grammar::set_non_accept_user_msg, self, _1, _2 ) ] >> *(anychar_p - eol_p) >> eol_p;
  error_pass_p   =  eps_p[ boost::bind( &pop3_command_grammar::set_non_accept_pass_msg, self, _1, _2 ) ] >> *(anychar_p - eol_p) >> eol_p;
  error_apop_p   =  eps_p[ boost::bind( &pop3_command_grammar::set_apop_unsupport_msg, self, _1, _2 ) ] >> *(anychar_p - eol_p) >> eol_p;
  error_cmd_p    =  eps_p[ boost::bind( &pop3_command_grammar::set_invalid_command_msg, self, _1, _2 ) ] >> *(anychar_p - eol_p) >> eol_p;
  error_param_p  =  eps_p[ boost::bind( &pop3_command_grammar::set_invalid_param_msg, self, _1, _2 ) ] >> *(anychar_p - eol_p) >> eol_p;
  error_in_p     =  eps_p[ boost::bind( &pop3_command_grammar::set_in_transaction_msg, self, _1, _2 ) ] >> *(anychar_p - eol_p) >> eol_p;
  error_out_p    =  eps_p[ boost::bind( &pop3_command_grammar::set_out_transaction_msg, self, _1, _2 ) ] >> *(anychar_p - eol_p) >> eol_p;
  // set rules
  // for param
  pop3_uinput_p  =  eps_p(space_p >> uint_p) >> space_p >> uint_p[
    boost::bind( &pop3_command_grammar::set_uparam, self, _1 )
  ];
  pop3_default_p =  eps_p[ boost::bind( &pop3_command_grammar::set_default, self, _1, _2 ) ];
  pop3_string_p  =  (+print_p)[ boost::bind( &pop3_command_grammar::set_sparam, self, _1, _2 ) ];  // may much almost
  // for command
  set_command_p  =  as_lower_d[ self.pop3_command_p[ boost::bind( &pop3_command_grammar::set_command, self, _1 ) ] ];
  // each command
  if( !self.state_.in_transaction_ && !self.state_.uname_active_ ) {
    pop3_user_p  =  eps_p(as_lower_d["user"]) >> set_command_p >> (( space_p >> pop3_string_p >> eol_p) | error_param_p);
  } else {
    pop3_user_p  =  eps_p(as_lower_d["user"]) >> error_user_p;
  }
  if( self.state_.uname_active_ && !self.state_.in_transaction_ ) {
    pop3_pass_p  =  eps_p(as_lower_d["pass"]) >> set_command_p >> (( space_p >> pop3_string_p >> eol_p) | error_param_p);
  } else if( self.state_.in_transaction_ ) {
    pop3_pass_p  =  eps_p(as_lower_d["pass"]) >> error_in_p;
  } else {
    pop3_pass_p  =  eps_p(as_lower_d["pass"]) >> error_pass_p;
  }
  if( self.state_.in_transaction_ ) {
    pop3_rset_p  =  eps_p(as_lower_d["rset"]) >> set_command_p >> (eol_p | error_param_p);
    pop3_stat_p  =  eps_p(as_lower_d["stat"]) >> set_command_p >> (eol_p | error_param_p);
    pop3_list_p  =  eps_p(as_lower_d["list"]) >> set_command_p >> (((pop3_uinput_p | pop3_default_p) >> eol_p) | error_param_p);
    pop3_uidl_p  =  eps_p(as_lower_d["uidl"]) >> set_command_p >> (((pop3_uinput_p | pop3_default_p) >> eol_p) | error_param_p);
    pop3_dele_p  =  eps_p(as_lower_d["dele"]) >> set_command_p >> ((pop3_uinput_p >> eol_p) | error_param_p);
    pop3_top_p   =  eps_p(as_lower_d["top" ]) >> set_command_p >> ((pop3_uinput_p >> pop3_uinput_p >> eol_p) | error_param_p);
    pop3_retr_p  =  eps_p(as_lower_d["retr"]) >> set_command_p >> ((pop3_uinput_p >> eol_p) | error_param_p);
  } else {
    pop3_rset_p  =  eps_p(as_lower_d["rset"]) >> error_out_p;
    pop3_stat_p  =  eps_p(as_lower_d["stat"]) >> error_out_p;
    pop3_list_p  =  eps_p(as_lower_d["list"]) >> error_out_p;
    pop3_uidl_p  =  eps_p(as_lower_d["uidl"]) >> error_out_p;
    pop3_dele_p  =  eps_p(as_lower_d["dele"]) >> error_out_p;
    pop3_top_p   =  eps_p(as_lower_d["top" ]) >> error_out_p;
    pop3_retr_p  =  eps_p(as_lower_d["retr"]) >> error_out_p;
  }
  if( self.state_.in_transaction_ ) {
    pop3_apop_p  =  eps_p(as_lower_d["apop"]) >> error_in_p;
  } else if( self.state_.apop_support_ ) {
    pop3_apop_p  =  eps_p(as_lower_d["apop"]) >> set_command_p
      >> ((space_p >> +(print_p - blank_p) >> space_p >> repeat_p(32)[ hex_p ] >> eol_p) | error_param_p);
  } else {
    pop3_apop_p  =  eps_p(as_lower_d["apop"]) >> error_apop_p;
  }
  pop3_noop_p    =  eps_p(as_lower_d["noop"]) >> set_command_p >> (eol_p | error_param_p);
  pop3_quit_p    =  eps_p(as_lower_d["quit"]) >> set_command_p >> (eol_p | error_param_p);
  expression  =  (
    pop3_user_p | pop3_pass_p | pop3_noop_p | pop3_rset_p | pop3_stat_p |
    pop3_list_p | pop3_uidl_p | pop3_apop_p | pop3_dele_p | pop3_quit_p |
    pop3_top_p  | pop3_retr_p | error_cmd_p
  ) >> end_p;

  BOOST_SPIRIT_DEBUG_RULE(pop3_default_p);
  BOOST_SPIRIT_DEBUG_RULE(pop3_uinput_p);
  BOOST_SPIRIT_DEBUG_RULE(pop3_string_p);
  BOOST_SPIRIT_DEBUG_RULE(set_command_p);
  BOOST_SPIRIT_DEBUG_RULE(error_user_p);
  BOOST_SPIRIT_DEBUG_RULE(error_pass_p);
  BOOST_SPIRIT_DEBUG_RULE(error_apop_p);
  BOOST_SPIRIT_DEBUG_RULE(error_cmd_p);
  BOOST_SPIRIT_DEBUG_RULE(error_param_p);
  BOOST_SPIRIT_DEBUG_RULE(error_in_p);
  BOOST_SPIRIT_DEBUG_RULE(error_out_p);
  BOOST_SPIRIT_DEBUG_RULE(pop3_user_p);
  BOOST_SPIRIT_DEBUG_RULE(pop3_pass_p);
  BOOST_SPIRIT_DEBUG_RULE(pop3_noop_p);
  BOOST_SPIRIT_DEBUG_RULE(pop3_rset_p);
  BOOST_SPIRIT_DEBUG_RULE(pop3_stat_p);
  BOOST_SPIRIT_DEBUG_RULE(pop3_list_p);
  BOOST_SPIRIT_DEBUG_RULE(pop3_uidl_p);
  BOOST_SPIRIT_DEBUG_RULE(pop3_apop_p);
  BOOST_SPIRIT_DEBUG_RULE(pop3_dele_p);
  BOOST_SPIRIT_DEBUG_RULE(pop3_quit_p);
  BOOST_SPIRIT_DEBUG_RULE(pop3_top_p);
  BOOST_SPIRIT_DEBUG_RULE(pop3_retr_p);
}

//! start
/*!
  @retval start expression
*/
template <typename IterT>
template <typename ScannerT>
boost::spirit::classic::rule<ScannerT> const& 
pop3_command_grammar<IterT>::definition<ScannerT>::start() const {
  return expression; 
}

//} // namespace pop3
//}  // namespace rfc


#ifndef GETUNTIL_HPP
#define GETUNTIL_HPP
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Copyright (c) 2007-2009 OKI Miyuki (oki.miyuki at gmail dot com)
//
#include <iostream>
#include <string>

template<typename CharT, typename CharTraits, typename Allocator>
std::basic_istream<CharT, CharTraits>& getuntil(
  std::basic_istream<CharT, CharTraits>&  istrm,
  std::basic_string<CharT, CharTraits, Allocator>& str,
  const std::basic_string<CharT, CharTraits, Allocator>& delm
) {
  typedef std::basic_istream<CharT, CharTraits> istrm_type;
  std::ios_base::iostate cur_state  =  std::ios_base::goodbit;
  bool read_once = false;
  const typename istrm_type::sentry sync( istrm, true );
  if( sync ) {
    try {
      str.clear();
      typename CharTraits::int_type row_xor  =  0;
      typename CharTraits::int_type ins_xor  =  0;
      size_t delm_len  =  delm.size();
      typename CharTraits::int_type val = istrm.rdbuf()->sgetc();
      for( size_t i = 0; i < delm_len - 1; ++i, val = istrm.rdbuf()->snextc() ) {
          if( CharTraits::eq_int_type( CharTraits::eof(), val ) )  break;
          read_once  =  true;
          ins_xor   ^=  val;
          row_xor   ^=  CharTraits::to_int_type( delm[ i ] );
          str       +=  CharTraits::to_char_type( val );
      }
      row_xor  ^=  CharTraits::to_int_type( *delm.rbegin() );
      for( size_t ipos = 0; ; val = istrm.rdbuf()->snextc(), ++ipos ) {
        if( CharTraits::eq_int_type( CharTraits::eof(), val ) ) {
          cur_state  |=  std::ios_base::eofbit;
          break;
        }
        read_once =  true;
        ins_xor  ^=  val;
        str      +=  CharTraits::to_char_type( val );
        if( 
          ins_xor == row_xor 
          && val == CharTraits::to_int_type( *delm.rbegin() ) 
          && str.substr( ipos ) == delm 
        ) {
          istrm.rdbuf()->sbumpc();
          break;
        }
        if( str.max_size() <= str.size() ) {
          cur_state  |=  std::ios_base::failbit;
          break;
        }
        ins_xor  ^=  CharTraits::to_int_type( str[ipos] );
      }
    } catch(...) {
      istrm.setstate( std::ios_base::badbit, true ); 
    }
  }
  if( !read_once ) {
    cur_state  |=  std::ios_base::failbit;
  }
  istrm.setstate( cur_state );

  return istrm;
}

template<typename CharT, typename CharTraits, typename Allocator>
std::basic_istream<CharT, CharTraits>& getuntil(
  std::basic_istream<CharT, CharTraits>&  istrm,
  std::basic_string<CharT, CharTraits, Allocator>& str,
  const CharT* delm
) {
  std::basic_string<CharT,CharTraits,Allocator> delimiter( delm );
  return getuntil( istrm, str, delimiter );
}

#endif // GETUNTIL_HPP

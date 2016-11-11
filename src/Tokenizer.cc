//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: Tokenizer.cc,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////
#include <amis/Tokenizer.h>
#include <amis/Profile.h>

AMIS_NAMESPACE_BEGIN

/*
bool Tokenizer::nextToken( std::string& token, const char* delimiters ) {
  AMIS_PROF2( "Tokenizer::nextToken(string)" );
  if ( end_of_stream ) return false;
  int c = s.get();
  while ( true ) {
    current_delimiter = c;
    if ( findChar( line_delimiters, c ) ) {
      ++line_number;
      return false;
    } else if ( c == EOF ) {
      end_of_stream = true;
      return false;
    } else if ( findChar( symbols, c ) ) {
      token = c;
      return true;
    } else if ( findChar( comment_start, c ) ) {
      while ( c != EOF && ! findChar( comment_end, c ) ) c = s.get();
      current_delimiter = c;
      if ( c == EOF ) end_of_stream = true; else ++line_number;
      return false;
    } else if ( findChar( word_delimiters, c ) ||
		findChar( delimiters, c ) ) {
      c = s.get();
      continue;
    } else {
      break;
    }
  }
  token.resize( 0 );
  while ( true ) {
    if ( findChar( escape, c ) ) {
      c = s.get();
      if ( c == EOF )
	throw TokenError( "Escape character found at the end of the stream" );
    }
    token.push_back( c );
    c = s.get();
    current_delimiter = c;
    if ( findChar( word_delimiters, c ) ||
	 findChar( delimiters, c ) ) {
      break;
    } else if ( findChar( line_delimiters, c ) ||
		findChar( comment_start, c ) ||
		findChar( symbols, c ) ) {
      s.putback( c );
      break;
    } else if ( c == EOF ) {
      end_of_stream = true;
      break;
    }
  }
  return ! token.empty();
}
*/

bool Tokenizer::nextToken( std::string& token, const char* delimiters )
{
  static std::vector< char > char_list;
  if ( endOfStream() ) return false;
  int c = get();
  if ( prev_line_delim != EOF && char_class[ c ] == LINE_DELIM && c != prev_line_delim ) c = get();  // to handle Windows' format
  prev_line_delim = EOF;
  while ( true ) {
    // read delimiters until a proper character appears
    current_delimiter = c;
    if ( c == EOF ) {
      end_of_stream = true;
      return false;
    }
    if ( char_class[ c ] == OTHER || char_class[ c ] == ESCAPE ) {
      if ( ! findChar( delimiters, c ) ) break;
      c = get();
      continue;
    }
    if ( char_class[ c ] == SYMBOL ) {
      token = c;
      //std::cerr << "token = " << token << std::endl;
      return true;
    }
    if ( char_class[ c ] == WORD_DELIM ) {
      c = get();
      continue;
    }
    if ( char_class[ c ] == LINE_DELIM ) {
      prev_line_delim = c;
      ++line_number;
      return false;
    }
    if ( char_class[ c ] == COM_START ) {
      while ( c != EOF && char_class[ c ] != LINE_DELIM && char_class[ c ] != COM_END ) c = get();
      current_delimiter = c;
      if ( char_class[ c ] == COM_END ) {
	c = get();
	continue;
      }
      if ( c == EOF ) {
        end_of_stream = true;
      } else {
        prev_line_delim = c;
        ++line_number;
      }
      return false;
    }
    throw TokenError( "Unbalanced Comment end token" );
  }
  char_list.resize( 0 );
  while ( true ) {
    if ( char_class[ c ] == ESCAPE ) {
      // retains the escape character but reads the next character as-is
      char_list.push_back( c );
      c = get();
      if ( c == EOF ) {
	end_of_stream = true;
	throw TokenError( "Escape character found at the end of the stream" );
      }
    }
    char_list.push_back( c );
    c = get();
    current_delimiter = c;
    if ( c == EOF ) {
      end_of_stream = true;
      break;
    }
    CharClass cl = char_class[ c ];
    if ( cl == OTHER || cl == ESCAPE ) {
      if ( findChar( delimiters, c ) ) break;
      continue;
    }
    if ( cl == WORD_DELIM ) {
      break;
    }
    if ( cl == LINE_DELIM ||
	 cl == COM_START ||
	 cl == SYMBOL ) {
      putback( c );
      break;
    }
    throw TokenError( "Unbalanced Comment end token" );
  }
  token = std::string( char_list.begin(), char_list.end() );
  //std::cerr << "token = " << token << std::endl;
  return ! token.empty();
}

AMIS_NAMESPACE_END

// end of Tokenizer.cc

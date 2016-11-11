//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: Tokenizer.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_Tokenizer_h_

#define Amis_Tokenizer_h_

#include <amis/configure.h>
#include <amis/Real.h>
#include <amis/ErrorBase.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif // HAVE_UNISTD_H
#include <iostream>
#include <vector>
#include <cstdio>

AMIS_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////

/// <classdef>
/// <name>TokenError</name>
/// <overview>Exception for invalid tokens</overview>
/// <desc>
/// This class signals invalid tokens in the Tokenizer class.
/// </desc>
/// <body>

class TokenError : public ErrorBase {
 public:
  TokenError( const std::string& m ) : ErrorBase( m ) {}
  /// Initialize with an error message
  TokenError( const char* m ) : ErrorBase( m ) {}
  /// Initialize with an error message
};

/// </body>
/// </classdef>

//////////////////////////////////////////////////////////////////////

/// <classdef>
/// <name>Tokenizer</name>
/// <overview>String tokenizer</overview>
/// <desc>
/// This class splits an input string into tokens
/// </desc>
/// <body>

class Tokenizer {
public:
  enum CharClass {
    OTHER = 0,
    WORD_DELIM = 1,
    LINE_DELIM = 2,
    ESCAPE = 3,
    SYMBOL = 4,
    COM_START = 5,
    COM_END = 6
  };

  static const size_t MAX_BUFFER_SIZE = 4096;

private:
  std::istream& stream_ref;
  bool end_of_stream;
  int current_delimiter;
  int line_number;
  char* buffer;
  size_t pointer;
  size_t buffer_size;
  size_t max_buffer_size;
  int prev_line_delim;
  std::string temp;  // temporal storage

  const char* word_delimiters;
  const char* line_delimiters;
  const char* escape;
  const char* symbols;
  const char* comment_start;
  const char* comment_end;

  std::vector< CharClass > char_class;

protected:
  /*
  int get() {
    if ( endOfStream() ) return EOF;
    if ( pointer == buffer_size ) {
      if ( ! stream_ref ) {
        end_of_stream = true;
        return EOF;
      }
      pointer = 0;
      stream_ref.read( buffer, MAX_BUFFER_SIZE );
      buffer_size = stream_ref.gcount();
      if ( buffer_size == 0 ) {
        end_of_stream = true;
        return EOF;
      }
    }
    return buffer[ pointer++ ];
  }
  void putback( int c ) {
    buffer[ --pointer ] = c;
  }
  */
// above is problematic because it blocks for std::cin. Please re-implement and ensure that it never block and "--analyzer" works fine with "--event-file stdin" (J.K. 2003/9/8)

/*
  int get() {
    return stream_ref.get();
  }
  void putback( int c ) { stream_ref.putback( c ); }
*/

  int get() {
    if ( endOfStream() ) return EOF;
    if ( pointer == buffer_size ) {
      if ( ! stream_ref ) {
        end_of_stream = true;
        return EOF;
      }
      pointer = 0;
      //stream_ref.getline( buffer, max_buffer_size, *line_delimiters );
      stream_ref.read( buffer, max_buffer_size );
      buffer_size = stream_ref.gcount();
      if ( buffer_size == 0 ) {
        // end of file
        //std::cerr << "end of file" << std::endl;
        end_of_stream = true;
        return EOF;
      }
      //std::cerr << "buffer=" << buffer << std::endl;
      //std::cerr << "count=" << buffer_size << std::endl;
//       if ( stream_ref.fail() ) {
//         // buffer overflow
//         stream_ref.clear();
//       } else {
//         buffer[ buffer_size - 1 ] = *line_delimiters;
//       }
    }
    return static_cast< unsigned char >( buffer[ pointer++ ] );
  }
  void putback( int c ) {
    buffer[ --pointer ] = c;
  }

  void initCharClass( const char* char_it, CharClass c ) {
    while ( *char_it != '\0' ) {
      char_class[ *char_it ] = c;
      ++char_it;
    }
  }

  static bool findChar( const char* str, char c ) {
    while ( *str != '\0' ) {
      if ( *str == c ) return true;
      ++str;
    }
    return false;
  }

public:
  Tokenizer( std::istream& s_,
             const char* word_delim = " \t",
             const char* line_delim = "\r\n",
             const char* esc = "\\",
             const char* sym = "",
             const char* com_start = "#",
             const char* com_end = "",
             size_t max_buf_size = MAX_BUFFER_SIZE )
    : stream_ref( s_ ), temp(),
      word_delimiters( word_delim ), line_delimiters( line_delim ),
      escape( esc ), symbols( sym ),
      comment_start( com_start ), comment_end( com_end ),
      char_class( 256, OTHER ) {
    end_of_stream = false;
    current_delimiter = '\0';
    line_number = 0;
    max_buffer_size = ( stream_ref == std::cin && isatty( 0 ) ? 1 : max_buf_size );  // to avoid dead lock
    buffer = new char[ max_buffer_size ];
    pointer = 0;
    buffer_size = 0;
    prev_line_delim = EOF;
    initCharClass( word_delimiters, WORD_DELIM );
    initCharClass( line_delimiters, LINE_DELIM );
    initCharClass( escape, ESCAPE );
    initCharClass( symbols, SYMBOL );
    initCharClass( comment_start, COM_START );
    initCharClass( comment_end, COM_END );
  }
  /// Initialize with an input stream
  virtual ~Tokenizer() {
    delete [] buffer;
  }

  bool endOfStream() const {
    return end_of_stream;
  }
  /// Whether we've reached to the end of the stream
  int currentDelimiter() const {
    return current_delimiter;
  }
  /// Get the most recent delimiter
  int lineNumber() const {
    return line_number;
  }
  /// Get the current line number

  static int str2int( const std::string& str ) {
    char* end_ptr;
    int num = static_cast< int >( strtol( str.c_str(), &end_ptr, 0 ) );
    if ( *end_ptr != '\0' ) {
      throw TokenError( "Illegal format for integer: " + str );
    }
    return num;
  }
  /// Translate a string into an integer

  static Real str2Real( const std::string& str ) {
    char* end_ptr;
    Real num = static_cast< Real >( strtod( str.c_str(), &end_ptr ) );
    if ( *end_ptr != '\0' ) {
      throw TokenError( "Illegal format for real: " + str );
    }
    return num;
  }
  /// Translate a string into a real number

  bool gotoNewLine() {
    int c = get();
    if ( prev_line_delim != EOF && char_class[ c ] == LINE_DELIM && c != prev_line_delim ) c = get();  // to handle Windows' format
    prev_line_delim = EOF;
    while ( true ) {
      if ( c == EOF ) return false;
      if ( char_class[ c ] == LINE_DELIM ) {
        prev_line_delim = c;
        ++line_number;
        return true;
      }
      c = get();
    }
  }

  bool nextToken( std::string& token, const char* delimiters = "" );
  /// Get a next token from an input line

  bool nextToken( int& i ) {
    //std::string temp;
    if ( nextToken( temp ) ) {
      i = str2int( temp );
      return true;
    } else {
      return false;
    }
  }
  /// Get a next integer token from an input line
  bool nextToken( Real& r ) {
    //std::string temp;
    if ( nextToken( temp ) ) {
      //AMIS_DEBUG_MESSAGE( 3, "IN:" <<  __PRETTY_FUNCTION__ << ", temp=" << temp << '\n' );
      r = str2Real( temp );
      return true;
    } else {
      return false;
    }
  }
  /// Get a next real number token from an input line

  static bool convert( std::string& in, std::string& str ) {
    str = in;
    return true;
  }
  static bool convert( std::string& in, int& i ) {
    try {
      i = str2int( in );
    } catch ( TokenError& ) {
      return false;
    }
    return true;
  }
  static bool convert( std::string& in, Real& r ) {
    try {
      r = str2Real( in );
    } catch ( TokenError& ) {
      return false;
    }
    return true;
  }
};

/// </body>
/// </classdef>

AMIS_NAMESPACE_END

#endif // Tokenizer_h_
// end of Tokenizer.h

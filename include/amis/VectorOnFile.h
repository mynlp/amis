//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: VectorOnFile.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_VectorOnFile_h_
#define Amis_VectorOnFile_h_

#include <amis/configure.h>
#include <amis/ErrorBase.h>
#include <amis/objstream.h>
#include <amis/StringStream.h>

#include <vector>
#include <cstdio>

AMIS_NAMESPACE_BEGIN
//////////////////////////////////////////////////////////////////////

/// <classdef>
/// <name>VectorOnFileError</name>
/// <overview>A vector class on a file</overview>
/// <desc>
/// Only one element is accessible at the same time
/// </desc>
/// <body>

class VectorOnFileError : public ErrorBase {
 public:
  VectorOnFileError( const std::string& s ) : ErrorBase( s ) {}
  /// Initialize with an error message
  VectorOnFileError( const char* s ) : ErrorBase( s ) {}
  /// Initialize with an error message
};

/// </body>
/// </classdef>

//////////////////////////////////////////////////////////////////////

/// <classdef>
/// <name>VectorOnFile</name>
/// <overview>A vector class on a file</overview>
/// Only one element is accessible at the same time
/// <body>

template < class T >
class VectorOnFile {
public:
  static const int DEFAULT_CHUNK_SIZE = 512 * 1024 * 1024;  // 512 MB

private:
  std::string stream_base_name;

  std::vector<objstream*> stream_v;
  std::vector<objstream*> event2stream_v;
  std::vector<std::streampos>  event2pos_v;
  int                last_stream_no;
  std::streampos          last_pos;
  

  mutable T* tmp_object;

protected:
  void readObject( int i ) const
  {
    /*
    if ( stream == NULL ) {
      throw VectorOnFileError( "Stream is not set to `VectorOnFile'" );
    }
    */

    // support multiple stream 
    objstream* stream = event2stream_v[i];

    stream->seekg( event2pos_v[i] );

    /*
    if ( ! *stream ) {
      throw VectorOnFileError( "Cannot seek a read position in a stream" );
    }
    */

    if ( tmp_object == NULL ) {
      throw VectorOnFileError( "Invalid Vector On Memory (should not happen)" );
    }
    tmp_object->readObject( *stream );
    
    // Delete: since "!" predicts the success of the next action, 
    // not the previous action. 
    // The following code does not work on platform such as Mac OS X.
    // (misrports the fail of the read)
    /*
    if ( ! *stream ) {
      throw VectorOnFileError( "Cannot read an object from a stream" );
    }
    */

  }
  void pushObject( const T& t )
  {
    /*
    if ( stream == NULL ) {
      throw VectorOnFileError( "Stream is not set to `VectorOnFile'" );
    }
    */
    objstream* stream = stream_v[last_stream_no];
    stream->seekg( last_pos );

    event2stream_v.push_back(stream);
    event2pos_v.push_back(last_pos);
    
    if ( ! *stream ) {
      throw VectorOnFileError( "Cannot seek a write position in a stream" );
    }

    t.writeObject( *stream );
    last_pos = stream->tellp();

    if( last_pos > DEFAULT_CHUNK_SIZE ){
      last_stream_no++;
      OStringStream os;
      os << stream_base_name << '.' << last_stream_no;
      std::string fname = os.str();
      AMIS_PROF_MESSAGE( "Creating a new temporary event file on " << fname << '\n' );
      stream_v.push_back(new objstream(fname));
      last_pos = 0;
    }
  
    if ( ! *stream ) {
      throw VectorOnFileError( "Cannot write an object to a stream" );
    }
  }

public:
  VectorOnFile( void )
  {
    stream_base_name = "amis.vector_on_file.tmp";
    stream_v.push_back(new objstream(stream_base_name));
    tmp_object = NULL;
  
    last_stream_no = 0;
    last_pos       = 0;
  }
  
  VectorOnFile( objstream* s ) {
    stream_base_name = s->fileName();
    stream_v.push_back(s);
    tmp_object = NULL;
    last_stream_no = 0;
    last_pos       = 0;
  }
  virtual ~VectorOnFile() {    
    delete tmp_object;
  }
  
  virtual void clear() {
    delete tmp_object;
    tmp_object = NULL;
    last_stream_no = 0;
    last_pos       = 0;
    event2stream_v.clear();
    event2pos_v.clear();
  }

  void validate(T& tmp)
  {
    delete tmp_object;
    tmp_object = &tmp;
  }

  void setStream( objstream* s ) {
    stream_v[0] = s; // ?? is this useful
  }

  int size( void ) const {
    return event2pos_v.size();
  }

  const T& operator[]( int i ) const {
    readObject( i );
    return *tmp_object;
  }

  void push_back( const T& t ) {
    pushObject( t );
  }
};

AMIS_NAMESPACE_END

#endif // VectorOnFile_h_
// end of VectorOnFile.h


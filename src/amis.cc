//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: amis.cc,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#include <amis/AmisDriver.h>
#include <string>

//////////////////////////////////////////////////////////////////////

// Main routine

using namespace std;
using namespace amis;

int main( int argc, const char** argv ) {
  AMIS_PROFILE_START;

  int status = 0;
  try {
    AmisDriver amis_driver;
    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
      args.push_back(std::string(argv[i]));
    }
    status = amis_driver.run(args);
  }
  catch ( ErrorBase& err ) {
    cerr << "ERROR: " << err.message() << endl;
    return 1;
  }
  catch ( ... ) {
    cerr << "Unknown error" << endl;
    return 100;
  }

  AMIS_PROFILE_FINISH;

  return status;
}

// end of amis.cc

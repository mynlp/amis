//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: forestcheck.cc,v 1.2 2008-07-29 06:06:11 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#include <amis/configure.h>
#include <amis/NameTable.h>
#include <amis/EventReaderInst.h>
#include <amis/AmisDriver.h>
#include <string>

//////////////////////////////////////////////////////////////////////

AMIS_NAMESPACE_BEGIN

// NameTable with automatic registeration of new features
class NameTableTol : public NameTable {
public:
  FeatureID featureID( const std::string& name, bool unk_on_not_found = false )
  {
    if ( unk_on_not_found ) return NameTable::featureID( name, unk_on_not_found );
    FeatureID id = NameTable::featureID( name, true );
    if ( id == UNKNOWN_FEATURE_ID ) {
      id = this->numFeatures();
      NameTable::registerNewFeature( name, id );
    }
    return id;
  }
};

//////////////////////////////////////////////////////////////////////

class ForestCheckerError : public ErrorBase {
public:
  ForestCheckerError( const std::string& s ) : ErrorBase( s ) {
  }
  ForestCheckerError( const char* s ) : ErrorBase( s ) {
  }
};

static const size_t LIMIT_ERRORS = 0;
static const size_t LIMIT_NODE_CHECK = 1024*1024*32;
static const int DEFAULT_PRECISION = 6;

template < class Feature >
class ForestChecker {
public:
  typedef typename Feature::FeatureFreq FeatureFreq;

  enum CheckResult {
    SUCCESS, ROOT_NODE_FAIL, REMAINING_FEATURES
  };

private:
  NameTableTol* name_table;
  EventTree< Feature, false > event_tree;
  std::vector< bool > observed_feature;
  std::vector< FeatureFreq > observed_freq;
  std::vector< bool > node_check;
  std::vector< EventTreeNodeID > node_stack;
  std::vector< std::pair< FeatureID, FeatureFreq > > remaining_features;
  size_t num_events;
  size_t num_errors;
  size_t limit_errors;
  size_t num_node_check;
  size_t limit_node_check;
  Real machine_epsilon;

  std::string current_event_name;

public:
  ForestChecker( NameTableTol* nt,
		 size_t limit_errors_ = LIMIT_ERRORS,
                 size_t limit_node_check_ = LIMIT_NODE_CHECK,
                 int precision = DEFAULT_PRECISION ) {
    name_table = nt;
    num_events = 0;
    num_errors = 0;
    limit_errors = limit_errors_;
    num_node_check = 0;
    limit_node_check = limit_node_check_;
    machine_epsilon = std::pow( 0.1, precision + 1 );
  }
  virtual ~ForestChecker() {}

protected:
  virtual void initialize(size_t num_features, size_t num_nodes ) {
    observed_feature.resize( num_features );
    observed_freq.resize( num_features );
    node_check.resize( num_nodes );
    node_stack.clear();
    remaining_features.clear();
    num_node_check = 0;
    typename std::vector< bool >::iterator it1 = observed_feature.begin();
    typename std::vector< FeatureFreq >::iterator it2 = observed_freq.begin();
    for ( ; it1 != observed_feature.end(); ++it1, ++it2 ) {
      *it1 = false;
      *it2 = static_cast< FeatureFreq >( 0 );
    }
    for ( typename FeatureList< Feature >::const_iterator it = event_tree.observedEvent().begin();
          it != event_tree.observedEvent().end();
          ++it ) {
      if ( it->freq() != static_cast< FeatureFreq >( 0 ) ) {
        observed_feature[ it->id() ] = true;
        observed_freq[ it->id() ] += it->freq();
      }
    }
    for ( EventTreeNodeID id = 0; id < num_nodes; ++id ) {
      if ( event_tree[ id ].isDisjunctiveNode() ) {
        // disjunctive node
        node_check[ id ] = false;
        for ( EventTreeNodeIDList::const_iterator it = event_tree[ id ].daughterList().begin();
              it != event_tree[ id ].daughterList().end();
              ++it ) {
          if ( node_check[ *it ] ) {
            node_check[ id ] = true;
            break;
          }
        }
      } else {
        // conjunctive node
        node_check[ id ] = true;
        for ( typename FeatureList< Feature >::const_iterator fit = event_tree[ id ].featureList().begin();
              fit != event_tree[ id ].featureList().end();
              ++fit ) {
          if ( fit->freq() != static_cast< FeatureFreq >( 0 ) && ! observed_feature[ fit->id() ] ) {
            node_check[ id ] = false;
            break;
          }
        }
        if ( node_check[ id ] && ! event_tree[ id ].daughterList().empty() ) {
          for ( EventTreeNodeIDList::const_iterator it = event_tree[ id ].daughterList().begin();
                it != event_tree[ id ].daughterList().end();
                ++it ) {
            if ( ! node_check[ *it ] ) {
              node_check[ id ] = false;
              break;
            }
          }
        }
      }
    }
//     for ( EventTreeNodeID id = 0; id < num_nodes; ++id ) {
//       std::cerr << id << " " << node_check[ id ] << std::endl;
//     }
//     for ( typename std::vector< bool >::iterator it = observed_feature.begin();
//           it != observed_feature.end();
//           ++it ) {
//       if ( *it ) {
//         throw ForestCheckerError( "Feature " + name_table.featureNameString( it - observed_feature.begin() ) + " not found in a feature forest in " + tree_format->getCurrentEventName() );
//       }
//     }
    for ( typename FeatureList< Feature >::const_iterator it = event_tree.observedEvent().begin();
          it != event_tree.observedEvent().end();
          ++it ) {
      observed_feature[ it->id() ] = true;
    }
  }

  virtual bool checkNode( EventTreeNodeID node_id ) {
    //std::cerr << node_id << std::endl;
    if ( ! node_check[ node_id ] ) return false;
    if ( ++num_node_check > limit_node_check ) throw ForestCheckerError( "limit of node checks exceeded" );
    EventTreeNode< Feature >& node = event_tree[ node_id ];
    if ( node.isDisjunctiveNode() ) {
      // disjunctive node
      std::vector< EventTreeNodeID > node_stack_copy( node_stack );
      for ( EventTreeNodeIDList::const_iterator it = node.daughterList().begin();
            it != node.daughterList().end();
            ++it ) {
        if ( checkNode( *it ) ) return true;
        node_stack = node_stack_copy;
      }
      return false;
    } else {
      // conjunctive node
      for ( typename FeatureList< Feature >::const_iterator fit = node.featureList().begin();
            fit != node.featureList().end();
            ++fit ) {
        observed_freq[ fit->id() ] -= fit->freq();
      }
      if ( node.daughterList().empty() ) {
        // terminal node
        if ( node_stack.empty() ) {
          std::vector< std::pair< FeatureID, FeatureFreq > > remain;
          for ( typename FeatureList< Feature >::const_iterator fit = event_tree.observedEvent().begin();
                fit != event_tree.observedEvent().end();
                ++fit ) {
            if ( std::abs( observed_freq[ fit->id() ] - static_cast< FeatureFreq >( 0 ) ) > machine_epsilon ) {
              remain.push_back( std::make_pair( fit->id(), observed_freq[ fit->id() ] ) );
            }
          }
          if ( ! remain.empty() ) {
            if ( remaining_features.empty() || remain.size() < remaining_features.size() ) {
              remaining_features.swap( remain );
            }
            goto FAIL;
          }
        } else {
          EventTreeNodeID next_id = node_stack.back();
          node_stack.pop_back();
          if ( ! checkNode( next_id ) ) {
            goto FAIL;
          }
        }
        return true;
      } else {
        for ( EventTreeNodeIDList::size_type i = 0; i < node.daughterList().size(); ++i ) {
          node_stack.push_back( node.daughterList()[ node.daughterList().size() - i - 1 ] );
        }
        EventTreeNodeID next_id = node_stack.back();
        node_stack.pop_back();
        if ( ! checkNode( next_id ) ) {
          goto FAIL;
        }
        return true;
      }
FAIL:
      // backtracking
      for ( typename FeatureList< Feature >::const_iterator fit = node.featureList().begin();
            fit != node.featureList().end();
            ++fit ) {
        observed_freq[ fit->id() ] += fit->freq();
      }
      return false;
    }
  }

public:
  size_t numEvents() const { return num_events; }
  size_t numErrors() const { return num_errors; }

  virtual bool inputEvent( Tokenizer& tokenizer, AmisTreeEventReader< Feature, AlphaValue, false >& tree_format ) {
    typename AmisTreeEventReader< Feature, AlphaValue, false >::EventType* ret =
      tree_format.inputEvent( current_event_name, tokenizer );
    if ( ret == 0 ) return false;
    else {
      event_tree = *ret;
      delete ret;
      return true;
    }
  }
  virtual CheckResult checkForest() {
    initialize( name_table->numFeatures(), event_tree.numEventTreeNodes() );
    EventTreeNodeID root_id = event_tree.numEventTreeNodes() - 1;
    if ( ! node_check[ root_id ] ) return ROOT_NODE_FAIL;
    if ( ! checkNode( root_id ) ) return REMAINING_FEATURES;
    return SUCCESS;
  }
  virtual bool checkForest( Tokenizer& tokenizer, AmisTreeEventReader< Feature, AlphaValue, false >& tree_format ) {
    while ( ! tokenizer.endOfStream() ) {
      if ( inputEvent( tokenizer, tree_format ) ) {
        ++num_events;
        try {
          CheckResult result = checkForest();
          switch ( result ) {
          case ROOT_NODE_FAIL:
            AMIS_PROF_MESSAGE( "Event " << current_event_name << ": observed features are not included in feature forest\n" );
            ++num_errors;
            break;
          case REMAINING_FEATURES:
            AMIS_PROF_MESSAGE( "Event " << current_event_name << ": following features are not included in feature forest\n" );
            for ( typename std::vector< std::pair< FeatureID, FeatureFreq > >::const_iterator it = remaining_features.begin();
                  it != remaining_features.end();
                  ++it ) {
              AMIS_PROF_MESSAGE( "  " << name_table->featureNameString( it->first ) << "\t" << it->second << '\n' );
            }
            ++num_errors;
            break;
          default:
            ;
          }
        } catch ( ForestCheckerError& e ) {
          AMIS_PROF_MESSAGE( "Event " << current_event_name << ": " << e.message() << '\n' );
        }
        if ( limit_errors > 0 && num_errors >= limit_errors ) return false;
        if ( num_events % 100 == 0 ) {
          AMIS_PROF_MESSAGE( num_events << " events processed.\n" );
        }
        event_tree.clear();
        name_table->empty();
      }
    }
    return true;
  }
};

//////////////////////////////////////////////////////////////////////

static PropertyItem< int > limit_errors( "LIMIT_ERRORS", "--limit-errors", "", LIMIT_ERRORS, "Limit number of errors (property for forestcheck)" );
static PropertyItem< int > limit_node_check( "LIMIT_NODE_CHECK", "--limit-node-check", "", LIMIT_NODE_CHECK, "Limit number of node checks (property for forestcheck)" );

AMIS_NAMESPACE_END

//////////////////////////////////////////////////////////////////////

using namespace std;
using namespace amis;

template < class Feature >
bool forest_check_main( AmisDriver& amis_driver ) {
  EventReaderPtr event_format = amis_driver.newEventReader();
  NameTableTol name_table;
  event_format->setFeatureNameTable(&name_table);
  AmisTreeEventReader< Feature, AlphaValue, false >* tree_format = dynamic_cast< AmisTreeEventReader< Feature, AlphaValue, false >* >( event_format.get() );
  if ( tree_format == NULL ) {
    AMIS_PROF_MESSAGE( "Data format must be AmisTree\n" );
    return false;
  }

  bool ret = true;

  std::vector<std::string> event_files = amis_driver.getProperty().getEventFileList();
  for (std::vector<std::string>::iterator event_it = event_files.begin();
       event_it != event_files.end();
       ++event_it) {
    auto_ptr< istream > input_stream =
      Property::openInputStream( *event_it, amis_driver.getProperty().getEventFileCompression() );
    if ( ! *input_stream ) {
      AMIS_PROF_MESSAGE( "Cannot open input event file\n" );
      return false;
    }

    // Main program
    Tokenizer t( *input_stream );
    ForestChecker< Feature > checker( &name_table, limit_errors.getValue(), limit_node_check.getValue(), amis_driver.getProperty().getPrecision() );
    if (! checker.checkForest( t, *tree_format ) ) {
      ret = false;
    }

    if ( ! ret ) {
      AMIS_PROF_MESSAGE( "Limit number of errors exceeded\n" );
    }
    AMIS_PROF_MESSAGE( checker.numEvents() << " events processed.\n" );
    AMIS_PROF_MESSAGE( checker.numErrors() << " errors found.\n" );
  }

  return ret;
};

//////////////////////////////////////////////////////////////////////

// help message

const char* forestcheck_name() {
  return "forestcheck (" AMIS_PACKAGE " " AMIS_VERSION ") - A program for checking consistency of observed events and feature forests\n";
}

void help_message() {
  std::cerr << "Usage: forestcheck [list of options] [property file]" << std::endl;
  std::cerr << "Options:" << std::endl;
  std::cerr << limit_errors.getFullDescription();
  std::cerr << limit_node_check.getFullDescription();
  std::cerr << "\nYou can specify other \"amis\" options listed below." << std::endl;
}

//////////////////////////////////////////////////////////////////////

// Main routine

int main( int argc, const char** argv ) {
  AMIS_PROFILE_START;

  int status = 0;
  try {
    AmisDriver amis_driver;

    std::vector<std::string> args;
    for ( int i = 1; i < argc; ++i ) {
      args.push_back( std::string( argv[i] ) );
    }

    // Read options
    AMIS_DEBUG_MESSAGE( 1, "Input a property file... " );
    amis_driver.importProperty(args);
    AMIS_DEBUG_MESSAGE( 1, "done.\n" );

    // Initialization
    amis_driver.initialize();

    // Print startup messages
    AMIS_PROF_MESSAGE( forestcheck_name() );
    if ( amis_driver.getProperty().isHelp() || amis_driver.getProperty().isHeavyHelp() ) {
      help_message();
      amis_driver.showHelp();
      return 0;
    }

    // Input event space
    AMIS_PROF_MESSAGE( "Start checking feature forests...\n" );
    bool success = false;
    if ( amis_driver.getProperty().getFeatureType() == "binary" ) {
      success = forest_check_main< BinaryFeature >( amis_driver );
    } else if ( amis_driver.getProperty().getFeatureType() == "integer" ) {
      success = forest_check_main< IntegerFeature >( amis_driver );
    } else if ( amis_driver.getProperty().getFeatureType() == "real" ) {
      success = forest_check_main< RealFeature >( amis_driver );
    }
    if ( success ) status = 0; else status = 1;
    AMIS_PROF_MESSAGE( "done.\n" );

  } catch ( ErrorBase& err ) {
    AMIS_PROF_MESSAGE( "ERROR: " << err.message() << '\n' );
    return 1;
  } catch ( ... ) {
    AMIS_PROF_MESSAGE( "Unknown error\n" );
    return 100;
  }

  AMIS_PROFILE_FINISH;

  return status;
}

// end of amis.cc

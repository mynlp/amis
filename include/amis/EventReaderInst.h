//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: EventReaderInst.h,v 1.2 2008-07-29 06:05:57 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_EventReaderInst_h_

#define Amis_EventReaderInst_h_

#include <amis/EventReader.h>
#include <amis/Property.h>

#include <amis/Event.h>
#include <amis/AmisWeightAccumulator.h>

#include <amis/EventTree.h>
#include <amis/TreeWeightAccumulator.h>

#include <amis/EventFix.h>
#include <amis/FixWeightAccumulator.h>

#include <amis/EventCRF.h>
#include <amis/CRFWeightAccumulator.h>


AMIS_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////

/**
 * The class imports event data from Amis-style data files.
 * The Amis format enables non-binary features and packed events.
 * For details, see README.
 * @see Event, EventSpace, EventReader
 */

template<class Feature, class ParameterValue, bool ReferenceProbability >
class AmisEventReader : public EventReader {
  FeatureParser< Feature > feature_parser;
public:
  typedef Event<Feature, ReferenceProbability> EventType;
  AmisEventReader(Property* property)
    : EventReader(property->getFeatureWeightType())
  { }

  EventBase* allocate() const
  {
    return new EventType();
  }

  WeightAccumulator* makeAccumulator() const
  {
    return new AmisWeightAccumulator<Feature, ParameterValue, ReferenceProbability>();
  }

  EventType* inputEvent( std::string& event_name, Tokenizer& t) const
  {
    EventType* ret = new EventType();
    NameTable& name_table = *this->name_table;
    std::string tmp;
    if ( ! t.nextToken( event_name ) ) {
      //throw IllegalEventFormatError( "Event name expected" + event_name, t.lineNumber() );
      // !!!!!!!!!!!!!!!!!!
      delete ret;
      return 0; // empty line
    }
    Real prob = 0.0;
    if ( t.nextToken( tmp ) ) {
      prob = t.str2Real( tmp );
      if ( t.nextToken( tmp ) ) {
	throw IllegalEventFormatError( "Too many tokens found in Event " + event_name, t.lineNumber() );
      }
    }
    else {
      prob = 1.0;
    }
    std::vector< Feature > fl;
    while ( true ) {
      int freq;
      if ( ! t.nextToken( freq ) ) break;
      if ( freq < 0 ) {
	throw IllegalEventFormatError( "Feature list frequency must be positive or zero in Event " + event_name,
				       t.lineNumber() );
      }
      //cerr << "freq = " << freq << std::endl;
      try {
	fl.resize( 0 );
	feature_parser.inputFeatureList( name_table, t, fl );
      }
      catch ( IllegalFeatureError& e ) {
	throw IllegalEventFormatError( e.message() + " in Event " + event_name,
				       t.lineNumber() );
      }
      
      ret->addFeatureList( fl, freq );
    }
    return ret;
  }

  bool inputProb( std::string& event_name,
		  Tokenizer& t,
                  EventType* event ) const
  {
    if ( ! t.nextToken( event_name ) ) {
      // !!!!!!!!!!!!!!!!!!
      return false;
    }
    std::string dummy;
    if ( t.nextToken( dummy ) ) {
      throw IllegalEventFormatError( "More than one probability values found in Event " + event_name, t.lineNumber() );
    }
    for ( typename EventType::iterator fl = event->begin();
	  fl != event->end();
	  ++fl ) {
      Real p = 0.0;
      if ( t.nextToken( p ) ) {
	fl->setReferenceProbability( this->inputLambda(p) );
      }
      else {
        throw IllegalEventFormatError( "Cannot read probability value in Event " + event_name, t.lineNumber() );
      }
      if ( t.nextToken( dummy ) ) {
	throw IllegalEventFormatError( "More than one probability values found in Event " + event_name, t.lineNumber() );
      }
    }
    if ( t.nextToken( dummy ) ) {
      throw IllegalEventFormatError( "Too many lines for probability values found in Event " + event_name, t.lineNumber() );
    }
    return true;
  }

  EventBase* input( std::string& event_name, Tokenizer& t) const
  {
    return inputEvent( event_name, t);
  }

  EventBase* inputWithProb( std::string& event_name,
			    Tokenizer& t1,
			    Tokenizer& t2) const
  {
    EventType* ret = this->inputEvent( event_name, t1);
    if ( ret == 0 ) return 0;

    std::string prob_event_name;
    while ( ! inputProb( prob_event_name, t2, ret ) ) {
      if ( t2.endOfStream() ) {
	throw IllegalEventFormatError( "Probability file is shorter than event file", t1.lineNumber() );
      }
    }
    if ( prob_event_name != event_name ) {
      throw IllegalEventFormatError( "Event names are inconsistent: " + event_name + " vs. " + prob_event_name, t1.lineNumber() );
    }
    return ret;
  }

};

template<class Feature, class ParameterValue, bool ReferenceProbability >
class AmisTreeEventReader : public EventReader {
  mutable StringHash< EventTreeNodeID > node_hash;
  FeatureParser< Feature > feature_parser;
public:
  typedef EventTree<Feature, ReferenceProbability> EventType;
  AmisTreeEventReader(Property* property)
    : EventReader(property->getFeatureWeightType())
  { }
  EventBase* allocate() const
  {
    return new EventType();
  }

  WeightAccumulator* makeAccumulator() const
  {
    return new TreeWeightAccumulator<Feature, ParameterValue, ReferenceProbability>();
  }


  EventTreeNodeID inputEventDisjNode(std::string& event_name, 
				     Tokenizer& t,
				     EventType& et ) const {
    std::string token;
    std::vector< EventTreeNodeID > disj_list;
    if ( ! t.nextToken( token ) ) {
      // name of the node
      throw IllegalEventFormatError( "Cannot find the name of a disjunctive node in Event " + event_name, t.lineNumber() );
    }
    std::string top_node_name = token;
    std::pair< StringHash< EventTreeNodeID >::iterator, bool > nodehash_it;
    if ( token == "_" ) {
      // anonymous node
      nodehash_it.second = false;
    }
    else {
      nodehash_it = node_hash.insert( std::make_pair( token, 0 ) );
      if ( ! nodehash_it.second ) {
	// the same name node found
	throw IllegalEventFormatError( "You cannot use the same name of a node in Event " + event_name + " : " + token, t.lineNumber() );
      }
    }
    while ( true ) {
      if ( ! t.nextToken( token ) ) {
        // end of line
        throw IllegalEventFormatError( "Truncated line found in a disjunctive node in Event " + event_name, t.lineNumber() );
      }
      else if ( token.length() == 1 ) {
        switch ( token[ 0 ] ) {
        case '{':
          // disjunction starts in a disjunction
          throw IllegalEventFormatError( "Nested disjunction found in Event " + event_name, t.lineNumber() );
        case ')':
          // end of EventTreeNode
          throw IllegalEventFormatError( "Too many ')'s in Event " + event_name, t.lineNumber() );
        case '(':
          {
            // start of EventTreeNode
            EventTreeNodeID id = inputEventConjNode( event_name, t, et );
            disj_list.push_back( id );
            continue;
          }
        case '}':
          // end of disjunction
          EventTreeNodeID id = et.newDisjunctiveNode( disj_list );
	  if ( nodehash_it.second ) {  // node name was specified
	    nodehash_it.first->second = id;
	  }
	  return id;
        }
      }
      else if ( token[ 0 ] == '$' ) {
	// reference to a conjunctive node
	std::string node_name( token.substr( 1 ) );
	StringHash< EventTreeNodeID >::const_iterator it = node_hash.find( node_name );
	if ( it == node_hash.end() ) {
	  throw IllegalEventFormatError( "Unknown conjunctive node in Event " + event_name + " : " + node_name, t.lineNumber() );
	}
	if ( et[ it->second ].isDisjunctiveNode() ) {
	  throw IllegalEventFormatError( "Daughter of a disjunctive node must be conjunctive in Event " + event_name + " : " + node_name, t.lineNumber() );
	}
	disj_list.push_back( it->second );
      }
      else {
	throw IllegalEventFormatError( "Feature cannot be specified in a disjunctive node in Event " + event_name + " : " + top_node_name, t.lineNumber() );
      }
    }
    AMIS_ABORT( "AmisTreeFormat::InputEventDisjNode: never comes here" );
    return 0;  // dummy
  }

  /// Input conjunctive node
  EventTreeNodeID inputEventConjNode(std::string& event_name, 
				     Tokenizer& t,
				     EventType& et ) const {
    std::string token;
    std::vector< Feature > feature_list;
    std::vector< EventTreeNodeID > daughter_list;
    if ( ! t.nextToken( token ) ) {
      // name of the node
      throw IllegalEventFormatError( "Cannot find the name of an event tree node in Event " + event_name, t.lineNumber() );
    }
    std::pair< StringHash< EventTreeNodeID >::iterator, bool > nodehash_it;
    if ( token == "_" ) {
      // anonymous node
      nodehash_it.second = false;
    }
    else {
      nodehash_it = node_hash.insert( std::make_pair( token, 0 ) );
      if ( ! nodehash_it.second ) {
	// the same name node found
	throw IllegalEventFormatError( "You cannot use the same name of a node in Event " + event_name + " : " + token, t.lineNumber() );
      }
    }
    while ( true ) {
      if ( ! t.nextToken( token , ":" ) ) {
        // end of line
        throw IllegalEventFormatError( "Truncated line found in a conjunctive node in Event " + event_name, t.lineNumber() );
      }
      else if ( token.length() == 1 ) {
	if ( t.currentDelimiter() == ':' ) {
	  throw IllegalEventFormatError( "Symbols cannot be followed by \":\" in Event " + event_name + " : " + token, t.lineNumber() );
	}
        switch ( token[ 0 ] ) {
        case '(':
          // event tree node starts in an event tree node
          throw IllegalEventFormatError( "Nested event tree node found in Event " + event_name, t.lineNumber() );
        case '}':
          // end of disjunction
          throw IllegalEventFormatError( "Too many '}'s in Event " + event_name, t.lineNumber() );
        case ')':
          {
            // end of an event tree node
            EventTreeNodeID id = et.newConjunctiveNode( feature_list, daughter_list );
            if ( nodehash_it.second ) {
	      nodehash_it.first->second = id;
            }
            return id;
          }
        case '{':
          // a new disjunction found
          daughter_list.push_back( inputEventDisjNode( event_name, t, et ) );
          continue;
        }
      }
      else if ( token[ 0 ] == '$' ) {
	// reference to a disjunctive node
	if ( t.currentDelimiter() == ':' ) {
	  throw IllegalEventFormatError( "Node name cannot be followed by \":\" in Event " + event_name + " : " + token, t.lineNumber() );
	}
	std::string node_name( token.substr( 1 ) );
	StringHash< EventTreeNodeID >::const_iterator it = node_hash.find( node_name );
	if ( it == node_hash.end() ) {
	  throw IllegalEventFormatError( "Unknown disjunctive node in Event " + event_name + " : " + node_name, t.lineNumber() );
	}
	if ( ! et[ it->second ].isDisjunctiveNode() ) {
	  throw IllegalEventFormatError( "Daughter of a conjunctive node must be disjunctive in Event " + event_name + " : " + token, t.lineNumber() );
	}
	daughter_list.push_back( it->second );
      }
      else {
	// a feature found
	typename Feature::FeatureFreq freq = 1;
	if ( t.currentDelimiter() == ':' ) {
	  if ( ! t.nextToken( freq ) ) {
	    throw ( "Cannot read feature frequency in Event " + event_name, t.lineNumber() );
	  }
	}
	std::string feature;
	Tokenizer::convert( token, feature );
        try {
          feature_list.push_back( Feature( name_table->featureID( feature ), freq ) );
        }
	catch ( IllegalFeatureError& e ) {
          throw IllegalEventFormatError( e.message() + " in Event " + event_name, t.lineNumber() );
        }
      }
    }
    AMIS_ABORT( "AmisTreeFormat::InputEventConjNode: never comes here" );
    return 0;  // dummy
  }

public:
  /// Input probability from stream
  bool inputProb(std::string& event_name,
		 Tokenizer& t,
		 EventType& event_tree ) const {
    if ( ! t.nextToken( event_name ) ) return false; // empty line
    std::string token;
    if ( t.nextToken( token ) ) {
      throw IllegalEventFormatError( "Too many tokens found in probability file in Event " + event_name, t.lineNumber() );
    }
    Real prob = 0.0;
    if ( ! t.nextToken( prob ) ) {
      throw IllegalEventFormatError( "Probability value of observed event not found in Event " + event_name, t.lineNumber() );
    }
    event_tree.observedEvent().setReferenceProbability( this->inputLambda(prob) );
    if ( t.nextToken( token ) ) {
      throw IllegalEventFormatError( "Too many tokens found in probability file in Event " + event_name, t.lineNumber() );
    }
    while ( t.nextToken( token ) ) {
      StringHash< EventTreeNodeID >::const_iterator event_node = node_hash.find( token );
      if ( event_node == node_hash.end() ) {
	throw IllegalEventFormatError( "Unknown event node " + token + " found in probability file in Event " + event_name, t.lineNumber() );
      }
      if ( event_tree[ event_node->second ].isDisjunctiveNode() ) {
	throw IllegalEventFormatError( "Reference probability cannot be assigned to disjunctive node " + token + " in Event " + event_name, t.lineNumber() );
      }
      if ( ! t.nextToken( prob ) ) {
	throw IllegalEventFormatError( "Probability value not found in probability file in Event " + event_name, t.lineNumber() );
      }
      event_tree[ event_node->second ].featureList().setReferenceProbability( this->inputLambda(prob) );
      if ( t.nextToken( token ) ) {
        throw IllegalEventFormatError( "Too many tokens found in probability file in Event " + event_name, t.lineNumber() );
      }
    }
    return true;
  }

  /// Input an event from tokenizer
  EventType* inputEvent( std::string& event_name, Tokenizer& t ) const
  {
    EventType* ret = new EventType();
    NameTable& name_table = *this->name_table;
    std::string token;
    if ( ! t.nextToken( event_name ) ) {
      delete ret;
      return 0; // empty line
    }
    Real prob = 0.0;
    if ( t.nextToken( token ) ) {
      prob = t.str2Real( token );
      if ( t.nextToken( token ) ) {
	throw IllegalEventFormatError( "Too many tokens found in Event " + event_name, t.lineNumber() );
      }
    }
    else {
      prob = 1.0;
    }
    int freq = 0;
    if ( ! t.nextToken( freq ) ) {
      throw IllegalEventFormatError( "Event freqeuency not found in Event " + event_name,
				     t.lineNumber() );
    }
    if ( freq <= 0 ) {
      throw IllegalEventFormatError( "Event frequency must be positive in Event " + event_name,
				     t.lineNumber() );
    }
    //std::cerr << event_name << ", freq = "<< freq<< std::endl;
    std::vector< Feature > fl;
    try {
      feature_parser.inputFeatureList( name_table, t, fl );
    }
    catch ( IllegalFeatureError& e ) {
      throw IllegalEventFormatError( e.message() + " in Event " + event_name, t.lineNumber() );
    }
    if ( ! t.nextToken( token ) || token.compare( "{" ) != 0 ) {
      throw IllegalEventFormatError( "Event tree not found in Event " + event_name,
				     t.lineNumber() );
    }
    //node_hash.clear();
    node_hash = StringHash< EventTreeNodeID >();
    ret->clear();
    ret->addObservedEvent( freq, fl );
    //std::cerr << "start reading tree" << std::endl;
    inputEventDisjNode( event_name, t, *ret );
    //std::cerr << "finish reading tree" << std::endl;
    if ( t.nextToken( token ) || t.nextToken(token) ) {
      throw IllegalEventFormatError( "A token found after the event tree in Event " + event_name,
				     t.lineNumber() );
    }
    return ret;
  }

  EventBase* input( std::string& event_name, Tokenizer& t) const
  {
    return inputEvent(event_name, t);
  }

  EventBase* inputWithProb( std::string& event_name,
			    Tokenizer& t1,
			    Tokenizer& t2) const
  {
    EventType* ret = this->inputEvent( event_name, t1);
    if ( ret == 0 ) return 0;

    std::string prob_event_name;
    while ( ! inputProb( prob_event_name, t2, *ret ) ) {
      if ( t2.endOfStream() ) {
	throw IllegalEventFormatError( "Probability file is shorter than event file", t1.lineNumber() );
      }
    }
    if ( prob_event_name != event_name ) {
      throw IllegalEventFormatError( "Event names are inconsistent: " + event_name + " vs. " + prob_event_name, t1.lineNumber() );
    }
    return ret;
  }

};


template<class Feature, class ParameterValue, bool ReferenceProbability >
class AmisFixEventReader : public EventReader {
  typedef typename Feature::FeatureFreq FeatureFreq;
  std::vector<std::string> fixmap_files;
  std::string fixmap_compression;

  FixedTarget fixed_target;

  FeatureParser< Feature > feature_parser;

public:
  typedef EventFix<Feature, ReferenceProbability> EventType;
  AmisFixEventReader(Property* property)
    : EventReader(property->getFeatureWeightType())
  {
    fixmap_files = property->getFixMapFileList();
    fixmap_compression = property->getFixMapFileCompression();
  }

  void initialize()
  {
    for (std::vector<std::string>::iterator file_it = fixmap_files.begin();
	 file_it != fixmap_files.end();
	 ++file_it) {
      std::auto_ptr< std::istream > fixmap_file =
	Property::openInputStream( *file_it, fixmap_compression );
      this->initFixMap(*fixmap_file);
    }
  }

  EventBase* allocate() const
  {
    return new EventType();
  }

  WeightAccumulator* makeAccumulator() const
  {
    return new FixWeightAccumulator<Feature, ParameterValue, ReferenceProbability>(fixed_target);
  }

  void initFixMap( std::istream& s )
  {
    Tokenizer t(s);
    this->inputFixedTarget(t);
  }

  void readTargetDef( Tokenizer& t )
  {
    // target definition
    std::string target;
    while ( t.nextToken( target ) ) {
      fixed_target.registerNewTarget( target );
    }
  }

  void inputFixedTarget( Tokenizer& t)
  {
    readTargetDef(t);
    while ( ! t.endOfStream() ) {
      std::string history;
      if ( ! t.nextToken( history ) ) continue;
      FeatureID hid = fixed_target.registerNewHistory( history );
      std::string target;
      while( t.nextToken( target ) ) {
        FeatureID tid = fixed_target.targetID( target );
        AMIS_DEBUG_MESSAGE( 2, "target=" << target << " id=" << tid << '\n' );

	std::string feature;
        if ( ! t.nextToken( feature ) ) {
          throw IllegalEventFormatError( "Cannot read feature name", t.lineNumber() );
        }
	FeatureID fid = name_table->featureID(feature);
	fixed_target.addFeatureMap( hid, tid, fid );
        AMIS_DEBUG_MESSAGE( 2, "feature=" << feature << " id=" << fid << " lambda=" << lambda << '\n' );
      }
    }
  }

  bool inputProb( Tokenizer& t, EventType& event ) const {
    AMIS_ABORT( "not implemented yet: AmisFixEventReader::inputProb(...)" );
    return false;
  }

  /// Input a history list from tokenizer
  void inputHistoryList( Tokenizer& t, std::vector< Feature >& hv ) const {
    std::vector< std::pair<std::string, FeatureFreq> > org_vec;
    std::pair< std::string, FeatureFreq>              tmp_pair;
    
    std::string history_feat;
    FeatureFreq val;
    while ( feature_parser.inputFeature( t, history_feat, val ) ) {
      if ( val <= 0 ) {
        throw IllegalModelFormatError( "History value must be positive in Event " , t.lineNumber() );
      }
      FeatureID hid = fixed_target.historyID( history_feat );
      
      hv.push_back( Feature( hid, val ) );
    }
  }

  EventType* inputEvent( std::string& event_name, Tokenizer& t ) const
  {
    EventType* ret = new EventType();

    int freq = 0;
    if ( !t.nextToken( freq ) ) {
      //std::cerr << "finishing, " << freq << std::endl;
      delete ret;
      return 0;
    }
    //std::cerr << "freq = " << freq << std::endl;
    if( freq < 0 ){
      throw IllegalEventFormatError( "AmisFixFormat: invalid frequency value. line=", t.lineNumber() );
    }
		
    std::string target;
    if ( !t.nextToken( target ) ) {
      throw IllegalEventFormatError( "AmisFixFormat: No target ", t.lineNumber() );
    }
    FeatureID tid = fixed_target.targetID( target );

    std::vector< Feature > hv;
    inputHistoryList( t, hv ); // without registering
    //FeatureList<Feature> hl( hv, 1 );
    FeatureList<Feature> hl( hv, freq );
    
    ret->add( fixed_target, freq, tid, hl );
    
    return ret;
  }
  
  EventBase* input( std::string& event_name, Tokenizer& t) const
  {
    return inputEvent(event_name, t);
  }

  EventBase* inputWithProb( std::string& event_name,
			    Tokenizer& t1,
			    Tokenizer& t2) const
  {
    AMIS_ABORT( "not implemented yet: AmisFixEventReader::inputProb(...)" );
    return 0;
  }
};



template<class Feature, class ParameterValue, bool ReferenceProbability >
class AmisCRFEventReader : public EventReader {
  typedef typename Feature::FeatureFreq FeatureFreq;

  std::vector<std::string> fixmap_files;
  std::string fixmap_compression;
  std::vector<std::string> transition_files;

  std::string transition_compression;
  FixedTarget fixed_target;
  CRFTransition crf_transition;
  FeatureParser< Feature > feature_parser;
public:
  typedef EventCRF<Feature, ReferenceProbability> EventType;
  AmisCRFEventReader(Property* property)
    : EventReader(property->getFeatureWeightType()), crf_transition(fixed_target)
  {
    fixmap_files = property->getFixMapFileList();
    fixmap_compression = property->getFixMapFileCompression();
    transition_files = property->getTransitionFileList();
    transition_compression = property->getTransitionFileCompression();
  }
  void initialize()
  {
    for (std::vector<std::string>::iterator file_it = fixmap_files.begin();
	 file_it != fixmap_files.end();
	 ++file_it) {
      std::auto_ptr< std::istream > fixmap_file =
	Property::openInputStream( *file_it, fixmap_compression );
      this->initFixMap(*fixmap_file);
    }

    for (std::vector<std::string>::iterator file_it = transition_files.begin();
	 file_it != transition_files.end();
	 ++file_it) {
      std::auto_ptr< std::istream > transition_file = Property::openInputStream( *file_it, transition_compression );
      this->initTransition(*transition_file);
    }
  }

  void initFixMap( std::istream& s)
  {
    Tokenizer t(s);
    this->inputFixedTarget(t);
  }

  void readTargetDef( Tokenizer& t)
  {
    // target definition
    std::string target;
    while ( t.nextToken( target ) ) {
      fixed_target.registerNewTarget( target );
    }
  }

  void inputFixedTarget( Tokenizer& t)
  {
    readTargetDef(t);
    while ( ! t.endOfStream() ) {
      std::string history;
      if ( ! t.nextToken( history ) ) continue;
      FeatureID hid = fixed_target.registerNewHistory( history );
      std::string target;
      while( t.nextToken( target ) ) {
        FeatureID tid = fixed_target.targetID( target );
        AMIS_DEBUG_MESSAGE( 2, "target=" << target << " id=" << tid << '\n' );

	std::string feature;
        if ( ! t.nextToken( feature ) ) {
          throw IllegalEventFormatError( "Cannot read feature name", t.lineNumber() );
        }
	FeatureID fid = name_table->featureID(feature);
	fixed_target.addFeatureMap( hid, tid, fid );
        AMIS_DEBUG_MESSAGE( 2, "feature=" << feature << " id=" << fid << " lambda=" << lambda << '\n' );
      }
    }
  }


  void initTransition( std::istream& s )
  {
    Tokenizer t(s);
    inputTransition( t );
  }


  void inputTransition( Tokenizer& t )
  {
    crf_transition.initTransition();

    std::string state;
    while ( t.nextToken( state ) ) {
      crf_transition.registerNewState(state);
    }
    crf_transition.finalize();

    while ( true ) {
      std::string cur_state;
      if ( ! t.nextToken( cur_state ) ) break;
      FeatureID cur_id = crf_transition.stateID(cur_state);

      std::string next_state;
      while ( t.nextToken( next_state ) ) {
	FeatureID next_id = crf_transition.stateID(next_state);
	crf_transition.addPath(cur_id, next_id);
      }
    }

    while ( true ) {
      if ( ! t.nextToken( state ) ) break;
      FeatureID state_id = crf_transition.stateID(state);

      std::string target;
      while ( t.nextToken( target ) ) {
	FeatureID target_id = fixed_target.targetID(target);
	crf_transition.addTarget(state_id, target_id);
      }
    }
  }

  EventBase* allocate() const
  {
    return new EventType();
  }

  WeightAccumulator* makeAccumulator() const
  {
    return new CRFWeightAccumulator<Feature, ParameterValue, ReferenceProbability>(fixed_target, crf_transition);
  }


  /// Input a history list from tokenizer
  void inputHistoryList( Tokenizer& t, const FixedTarget& fixed_target, std::vector< Feature >& hv ) const {
    std::vector< std::pair<std::string, FeatureFreq> > org_vec;
    std::pair< std::string, FeatureFreq >              tmp_pair;
    
    std::string history_feat;
    FeatureFreq val;
    while ( feature_parser.inputFeature( t, history_feat, val ) ) {
      if ( val <= 0 ) {
        throw IllegalModelFormatError( "History value must be positive in Event " , t.lineNumber() );
      }
      FeatureID hid = fixed_target.historyID( history_feat );
      
      hv.push_back( Feature( hid, val ) );
    }
  }

  EventType* inputEvent(std::string& event_name,
		       Tokenizer& t) const
  {
    EventType* ret = new EventType();

    int freq;
    if( !t.nextToken( freq ) || freq < 0 ){
      delete ret;
      return 0;
    }
    std::string dummy;
    if ( t.nextToken( dummy ) ) {
      throw IllegalEventFormatError( "Too many tokens found in event file ", t.lineNumber() );
    }

    std::vector<FeatureID> state_list;
    std::vector<std::vector<Feature> > history_list;
    while ( true ) {

      std::string state;
      if ( !t.nextToken( state ) ) {
	break;
      }
      FeatureID sid = crf_transition.stateID(state);

      std::vector< Feature > hv;
      inputHistoryList( t, fixed_target, hv ); // without registering
      //FeatureList<Feature> hl( hv, 1 );

      if ( !state_list.empty() ) {
	if ( !crf_transition.transTable(state_list.back(), sid) ) {
	  std::ostringstream os;
	  os << "Illegal transition from " << crf_transition.stateName(state_list.back())
	     << " to " << crf_transition.stateName(sid);
	  throw IllegalEventFormatError(os.str(), 0 );
	}
      }

      state_list.push_back(sid);
      history_list.push_back(hv);
    }
    ret->add(fixed_target, crf_transition, freq, state_list, history_list);
    return ret;
  }


  EventBase* input( std::string& event_name, Tokenizer& t) const
  {
    return inputEvent(event_name, t);
  }

  EventBase* inputWithProb( std::string& event_name,
			    Tokenizer& t1,
			    Tokenizer& t2) const
  {
    AMIS_ABORT( "not implemented yet: AmisFixEventReader::inputProb(...)" );
    return 0;
  }
};



AMIS_NAMESPACE_END

#endif // Amis_EventReaderInst_h_
// end of EventReader.h

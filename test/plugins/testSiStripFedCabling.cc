#include "CondFormats/SiStripObjects/test/plugins/testSiStripFedCabling.h"
#include "CalibTracker/SiStripESProducers/interface/SiStripFedCablingESProducer.h"
#include "CondFormats/DataRecord/interface/SiStripFedCablingRcd.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include <iostream>
#include <sstream>

// -----------------------------------------------------------------------------
// 
void testSiStripFedCabling::beginRun( const edm::Run& run, 
				      const edm::EventSetup& setup ) {
  
//   edm::ESHandle<SiStripFedCabling> cabling;
//   setup.get<SiStripFedCablingRcd>().get( cabling ); 

//   if ( cabling.isValid() ) {
//     edm::LogError("testSiStripFedCabling") 
//       << "[testSiStripFedCabling::" << __func__ << "]"
//       << " ESHandle is invalid!";
//     return;
//   }

#ifdef SISTRIPCABLING_USING_NEW_STRUCTURE
  SiStripFedCabling::Conns conns;
#else
  std::vector<FedChannelConnection> conns;
#endif
  connections(conns);
  SiStripFedCabling cabling(conns);
  
  {
    std::stringstream ss;
    ss << "[testSiStripFedCabling::" << __func__ << "]"
       << " VERBOSE DEBUG" << std::endl;
    cabling.print( ss );
    ss << std::endl;
    edm::LogVerbatim("testSiStripFedCabling") << ss.str();
  }
  
  {
    std::stringstream ss;
    ss << "[testSiStripFedCabling::" << __func__ << "]"
       << " TERSE DEBUG" << std::endl;
    cabling.terse( ss );
    ss << std::endl;
    edm::LogVerbatim("testSiStripFedCabling") << ss.str();
  }
  
  {
    std::stringstream ss;
    ss << "[testSiStripFedCabling::" << __func__ << "]"
       << " SUMMARY DEBUG" << std::endl;
    cabling.summary( ss );
    ss << std::endl;
    edm::LogVerbatim("testSiStripFedCabling") << ss.str();
  }
  
}

// -----------------------------------------------------------------------------
// 
#ifdef SISTRIPCABLING_USING_NEW_STRUCTURE
void testSiStripFedCabling::connections( SiStripFedCabling::Conns& conns ) {
#else
void testSiStripFedCabling::connections( std::vector<FedChannelConnection>& conns ) {
#endif
  
  conns.clear();

  uint16_t min_id = static_cast<uint16_t>( FEDNumbering::getSiStripFEDIds().first );
  uint16_t max_id = static_cast<uint16_t>( FEDNumbering::getSiStripFEDIds().second );
  
  for ( uint16_t fed_id = min_id; fed_id <= max_id; fed_id++ ) {
    uint16_t cntr = fed_id - min_id;
    for ( uint16_t fed_ch = 0; fed_ch < 96; fed_ch++ ) {
      FedChannelConnection conn;
      if ( fed_ch != 0 ) {
	conn = FedChannelConnection( SiStripFedCablingESProducer::fecCrate(cntr),
				     SiStripFedCablingESProducer::fecSlot(cntr),
				     SiStripFedCablingESProducer::fecRing(cntr),
				     SiStripFedCablingESProducer::ccuAddr(cntr),
				     SiStripFedCablingESProducer::ccuChan(cntr),
				     32 + 2*(fed_ch%3), // apv0
				     33 + 2*(fed_ch%3), // apv1
				     1 + cntr, // dcu
				     1 + cntr, // det
				     3,    // npairs
				     fed_id,
				     fed_ch );
	uint16_t fed_crate = ( fed_id - min_id ) / 20 + 1;
	uint16_t fed_slot  = ( fed_id - min_id ) % 20 + 2;
	conn.fedCrate( fed_crate );
	conn.fedSlot( fed_slot );
      } else { conn = FedChannelConnection(); }
      LogTrace("testSiStripFedCabling") << conn;
      conns.push_back(conn);
    }
  }
  
}

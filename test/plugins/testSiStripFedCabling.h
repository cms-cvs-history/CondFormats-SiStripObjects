#ifndef OnlineDB_SiStripESSources_testSiStripFedCabling_H
#define OnlineDB_SiStripESSources_testSiStripFedCabling_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "CondFormats/SiStripObjects/interface/SiStripFedCabling.h"

/**
   @class testSiStripFedCabling 
   @brief 
*/
class testSiStripFedCabling : public edm::EDAnalyzer {

 public:
  
  testSiStripFedCabling( const edm::ParameterSet& ) {;}
  
  virtual ~testSiStripFedCabling() {;}
  
  void beginRun( const edm::Run&, const edm::EventSetup& );
  
  void analyze( const edm::Event&, const edm::EventSetup& ) {;}
  
 private:

#ifdef SISTRIPCABLING_USING_NEW_STRUCTURE
  void connections( SiStripFedCabling::Conns& );
#else
  void connections( std::vector<FedChannelConnection>& );
#endif
  
};

#endif // OnlineDB_SiStripESSources_testSiStripFedCabling_H

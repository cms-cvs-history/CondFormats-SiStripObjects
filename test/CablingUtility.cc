// -*- C++ -*-
//
/**\class CablingUtility CablingUtility.cc CondFormats/SiStripObjects/test/CablingUtility.cc


 Description: Simple utility to inspect tracker cabling maps.

 Implementation:
     Reads from stdin. One can give as input DET, FED or FEC views.
     The job ends by pressing ctrl-d.
*/
//
// Original Author:  Christophe DELAERE
//         Created:  Wed Mar 28 18:01:43 CEST 2007
// $Id: CablingUtility.cc,v 1.2 2007/03/29 10:25:25 delaer Exp $
//
//


// system include files
#include <memory>
#include <iostream>
#include <sstream>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include <CondFormats/DataRecord/interface/SiStripFedCablingRcd.h>
#include <CondFormats/SiStripObjects/interface/FedChannelConnection.h>
#include <CalibFormats/SiStripObjects/interface/SiStripFecCabling.h>
#include <CondFormats/SiStripObjects/interface/SiStripFedCabling.h>
#include <DataFormats/SiStripDetId/interface/TOBDetId.h>
#include <DataFormats/SiStripDetId/interface/TIBDetId.h>
#include <DataFormats/SiStripDetId/interface/TIDDetId.h>
#include <DataFormats/SiStripDetId/interface/TECDetId.h>
//
// class decleration
//

class CablingUtility : public edm::EDAnalyzer {
   public:
      explicit CablingUtility(const edm::ParameterSet&);
      ~CablingUtility();


   private:
      virtual void beginJob(const edm::EventSetup&) ;
      virtual void analyze(const edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;

      // ----------member data ---------------------------
      SiStripFedCabling* fedCabling_; 
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
CablingUtility::CablingUtility(const edm::ParameterSet& iConfig)
{
   //now do what ever initialization is needed

}


CablingUtility::~CablingUtility()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called to for each event  ------------
void
CablingUtility::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;
}


// ------------ method called once each job just before starting event loop  ------------
void 
CablingUtility::beginJob(const edm::EventSetup& setup)
{
 // ---------- FED cabling ----------

 edm::ESHandle<SiStripFedCabling> fed_cabling;
 setup.get<SiStripFedCablingRcd>().get( fed_cabling );
 fedCabling_ = const_cast<SiStripFedCabling*>( fed_cabling.product() );

 std::cout << "=================================================" << std::endl;
 std::cout << "*       Informations about cabling map          *" << std::endl;
 std::cout << "=================================================" << std::endl << std::endl;
 std::cout << " input: " << std::endl;
 std::cout << " DET followed by DetId to see connections for a given module."<< std::endl;
 std::cout << " FED followed by FedId/Ch to see connections for a given FED channel."<< std::endl;
 std::cout << " FEC followed by Crate/Fec/Ring/CCUaddr/CCUchan/LLDchan to see connections for a given FEC channel."<< std::endl;
 std::cout << " e.g.: DET 369213701" << std::endl << "       FED 92 33" << std::endl << "       FEC 0 8 4 63 27 2" << std::endl;
 std::cout << " Other possibilities are TOB, TIB, TID, TEC." << std::endl;
 std::cout << " Please refer to the SiStripDetId documentation." << std::endl;
 std::cout << std::endl << std::endl;

 while (!std::cin.eof()) {
  int32_t detid = 0; 
  int32_t fedid = 0;
  int32_t fedch = 0;
  int32_t feccrate = 0;
  int32_t fecslot = 0;
  int32_t fecring = 0;
  int32_t ccuaddr = 0;
  int32_t ccuchan = 0;
  int32_t lldchan = 0;
  char buffer[256];
  char mode[256];
  std::cin.getline(buffer,256);
  if(std::cin.eof()) continue;
  std::stringstream ss(buffer); 
  ss >> mode;
  std::cout << "Dumping connection for " << mode << "." << std::endl;
  int theMode = -1;
  if(strncmp(mode,"DET",3)==0) { theMode = 1; ss >> detid; }
  else if(strncmp(mode,"TOB",3)==0) { 
    theMode = 1; 
    int32_t layer, rod_fw_bw, rod, module, ster;
    ss >> layer >> rod_fw_bw >> rod >> module >> ster;
    TOBDetId id(layer, rod_fw_bw, rod, module, ster);
    detid = id.rawId();
  }
  else if(strncmp(mode,"TID",3)==0) { 
    theMode = 1; 
    int32_t side, wheel, ring, module_fw_bw, module, ster;
    ss >> side >> wheel >> ring >> module_fw_bw >> module >> ster;
    TIDDetId id(side, wheel, ring, module_fw_bw, module, ster);
    detid = id.rawId();
  }
  else if(strncmp(mode,"TIB",3)==0) { 
    theMode = 1;
    int32_t layer, str_fw_bw, str_int_ext, str, module, ster;
    ss >> layer >> str_fw_bw >> str_int_ext >> str >> module >> ster;
    TIBDetId id(layer, str_fw_bw, str_int_ext, str, module, ster);
    detid = id.rawId();
  }
  else if(strncmp(mode,"TEC",3)==0) { 
    theMode = 1;
    int32_t side, wheel, petal_fw_bw, petal, ring, module, ster;
    ss >> side >> wheel >> petal_fw_bw >> petal >> ring >> module >> ster;
    TECDetId id(side, wheel, petal_fw_bw, petal, ring, module, ster);
    detid = id.rawId();
  }
  else if(strncmp(mode,"FED",3)==0) { theMode = 2; ss >> fedid >> fedch; }
  else if(strncmp(mode,"FEC",3)==0) { theMode = 3; ss >> feccrate >> fecslot >> fecring >> ccuaddr >> ccuchan >> lldchan; }
  std::vector<uint16_t>::const_iterator ifed = fedCabling_->feds().begin();
  for ( ; ifed != fedCabling_->feds().end(); ifed++ ) {
    const std::vector<FedChannelConnection>& conns = fedCabling_->connections(*ifed);
    std::vector<FedChannelConnection>::const_iterator iconn = conns.begin();
    for ( ; iconn != conns.end(); iconn++ ) {
       if((theMode == 1 && iconn->detId()==detid) ||
          (theMode == 2 && (iconn->fedId()==fedid || fedid<0) && (iconn->fedCh()==fedch || fedch<0) ) ||
	  (theMode == 3 && (iconn->fecCrate()==feccrate || feccrate<0) && (iconn->fecSlot()==fecslot || fecslot<0) && (iconn->fecRing()==fecring || fecring<0) && (iconn->ccuAddr()==ccuaddr || ccuaddr<0) && (iconn->ccuChan()==ccuchan || ccuchan<0) && (iconn->lldChannel()==lldchan || lldchan<0))  ){
	 std::stringstream ss;
	 iconn->print(ss);
	 std::cout << ss.str() << std::endl;
       }
    }
  }
  std::cout << "=================================================" << std::endl << std::endl;
 }
}

// ------------ method called once each job just after ending the event loop  ------------
void 
CablingUtility::endJob() {
}

//define this as a plug-in
DEFINE_FWK_MODULE(CablingUtility);

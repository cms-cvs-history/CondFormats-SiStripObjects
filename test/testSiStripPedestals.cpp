#include "CondCore/DBCommon/interface/DBWriter.h"
#include "CondCore/DBCommon/interface/DBSession.h"
#include "CondCore/DBCommon/interface/Exception.h"
#include "CondCore/DBCommon/interface/ServiceLoader.h"
#include "CondCore/DBCommon/interface/ConnectMode.h"
#include "CondCore/DBCommon/interface/MessageLevel.h"
#include "CondCore/IOVService/interface/IOV.h"
#include "CondCore/MetaDataService/interface/MetaData.h"
#include "FWCore/Framework/interface/IOVSyncValue.h"

#include "CondFormats/SiStripObjects/interface/SiStripPedestals.h"

#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGauss.h"
#include<iostream>
#include<string>

#include<boost/cstdint.hpp>



static const  int FedId_Mask_   = 0x3FF;
static const  int feFPGA_Mask_  = 0x7;
static const  int channel_Mask_ = 0xF;
static const  int ApvPair_Mask_ = 0x1;

static const  int FedId_StartBit_   = 8;
static const  int feFPGA_StartBit_  = 5;
static const  int channel_StartBit_ = 1;
static const  int ApvPair_StartBit_ = 0;

static const  int FedId_Max   =  2;
static const  int feFPGA_Max  =  8;
static const  int channel_Max = 12;
static const  int ApvPair_Max =  2;

const float MeanPed   = 100;
const float RmsPed    = 5;
const float MeanNoise = 5;
const float RmsNoise  = 1;
const double badStripProb = .002;


unsigned int EncodeApvIdValues(int FedId_, int feFPGA_, int channel_, int ApvPair_);
std::vector<int> DecodeApvIdValues( int value);

uint32_t EncodeStripData(float ped_, float noise_, float lowTh_, float highTh_, bool disable_);



int main(int csize, char** cline ) {
  cond::ServiceLoader* loader=new cond::ServiceLoader;
  ::putenv("CORAL_AUTH_USER=me");
  ::putenv("CORAL_AUTH_PASSWORD=mypass");
  loader->loadAuthenticationService( cond::Env );
  loader->loadMessageService( cond::Error );

  std::string dbConnection( "sqlite_file:test_SiStripPedestals.db");
  std::string dbCatalog("file:test_catalog.xml");

  std::cout<<"DB connection to "<<dbConnection<<std::endl;
  std::cout<<"catalog..."<<dbCatalog<<std::endl;
  
  try{
    cond::DBSession* session=new cond::DBSession(dbConnection);
    session->setCatalog(dbCatalog);

    cond::MetaData metadata_svc(dbConnection,*loader);
    metadata_svc.connect();

    cond::DBWriter pwriter(*session, "SiStripPedestals");
    cond::DBWriter iovwriter(*session, "IOV");
    cond::IOV* pedIov=new cond::IOV;

    if (csize==1){ //write      
      std::cout << "Writing mode" << std::endl;
      session->connect(cond::ReadWriteCreate);
      std::cout<<"connected in ReadWriteCreate mode to "<<dbConnection<<std::endl;
      session->startUpdateTransaction();

      //int totmodules=16000;
      int iovn=0;
      int evtn=10;
      for (int j=0; j<evtn; ++j){
	iovn=j%5;//till time
	if(iovn==0){
	  SiStripPedestals* ped=new SiStripPedestals();
	  for(int fedid=1; fedid<=FedId_Max; ++fedid)
	    for(int fefpga=1; fefpga<=feFPGA_Max; ++fefpga)
	      for(int ch=1; ch<=channel_Max; ++ch)
		for(int apvpair=1;apvpair<=ApvPair_Max;apvpair++){	   
		  unsigned int numschema = EncodeApvIdValues(fedid,fefpga,ch,apvpair);
		  //std::cout << "numschema " << numschema <<  std::endl;
		  
		  SiStripPedestalsVector theSiStripVector;
		  for(int strip=0; strip<128; ++strip){
		    SiStripPedestals::SiStripData theSiStripData;
		    
		    bool printstrip= (strip==-1) ? true : false;
		  
		    if (printstrip)
		      std::cout << " strip " << strip << " =\t";
		    theSiStripData.Data = ped->EncodeStripData(
							       RandGauss::shoot(MeanPed,RmsPed),
							       RandGauss::shoot(MeanNoise,RmsNoise),
							       2,
							       5,
							       (RandFlat::shoot(1.) < badStripProb ? true:false),
							       printstrip);
		    
		    theSiStripVector.push_back(theSiStripData);
		  }
		  ped->m_pedestals.insert(std::pair<unsigned int, SiStripPedestalsVector > (numschema,theSiStripVector));
		}
	  std::string pedtoken=pwriter.markWrite<SiStripPedestals>(ped);
	  unsigned long iov_idx=j+5;
	  std::cout << "iov idx " << iov_idx << " pedtoken " << pedtoken << std::endl; 
	  pedIov->iov.insert(std::make_pair(iov_idx,pedtoken)); 
	}	
      }
      std::cout << "iov size " << pedIov->iov.size() << std::endl;
      std::string pediovToken=iovwriter.markWrite<cond::IOV>(pedIov);
      session->commit();//commit all in one
      std::cout << "iov size " << pedIov->iov.size() << std::endl;
      session->disconnect();
      std::cout << "committed" << std::endl;
      std::cout << "iov tokens "<<pediovToken<< std::endl;   
      delete session;
      if(metadata_svc.addMapping("TrackerCalibration", pediovToken)){
	std::cout<<"new mapping inserted "<<std::endl;
      }else{
	std::cout<<"mapping exists, do nothing "<<std::endl;
      }
      metadata_svc.disconnect();
    }else {
      std::cout<<"Reading Mode"<<std::endl;
      std::string iovAName(cline[1]);
      std::string iovAToken= metadata_svc.getToken(iovAName);      
      std::cout << "iovAName " << iovAName << std::endl;
      std::cout << "iovAToken " << iovAToken << std::endl;
      
      session->connect(cond::ReadOnly);
      std::cout<<"connected in ReadOnly mode to "<<dbConnection<<std::endl;
      session->startReadOnlyTransaction();

      //FIXME: Understand how to read db with new interface
      /*
      pool::Ref<cond::IOV> iovA(svc,iovAToken);

      cond::IOV* pedIov=new cond::IOV;
      std::map<unsigned long,std::string>::iterator iov_iter=pedIov->iov.begin();
      for(;iov_iter!=pedIov->iov.end();iov_iter++){

	std::cout<<"iov idx "<<iov_iter->first<<std::endl;
	std::string pedToken =iov_iter->second;
	std::cout<<"reading Ped back by token "<<pedToken<<std::endl;
     
	SiStripPedestals* ped=new SiStripPedestals();
	
	std::cout << "Pedestals for " << std::endl;
      
	SiStripPedestalsMapIterator mapit = ped->m_pedestals.begin();
	for (;mapit!=ped->m_pedestals.end();mapit++){
      	  unsigned int numschema = (*mapit).first;
	  std::cout << "mappit " <<  numschema << std::endl;
	  
      	  std::vector<int> values = DecodeApvIdValues(numschema);
      	  SiStripPedestalsVector theSiStripVector =  (*mapit).second;
      	  //const SiStripPedestalsVector theSiStripVector =  (*ped).getSiStripPedestalsVector(numschema);
	  
      	  std::cout << "SiStripData for " 
      		    << " FedId_   \t" << values[0]
      		    << " feFPGA_  \t" << values[1]
      		    << " strip_   \t" << values[2]
      		    << " ApvPair_ \t" << values[3]
      		    << " numschema \t"<< numschema << std::endl;
	  
      	  int strip=0;
      	  for(SiStripPedestalsVectorIterator iter=theSiStripVector.begin(); iter!=theSiStripVector.end(); iter++){
	    std::cout << " strip " << strip++ << " =\t"
      		      << iter->getPed()     << " \t" 
      		      << iter->getNoise()   << " \t" 
      		      << iter->getLowTh()   << " \t" 
      		      << iter->getHighTh()  << " \t" 
      		      << iter->getDisable()<< " \t" 
      		      << std::endl; 	    
      	  } 
      	}
	}*/
    }//end else   
  }catch(const cond::Exception& er){
    std::cout<<er.what()<<std::endl;
    delete loader;
    exit(-1);
  }catch(const seal::Exception& er){
    std::cout<<er.what()<<std::endl;
    delete loader;
    exit(-1);
  }catch(...){
    std::cout<<"Funny error"<<std::endl;
    delete loader;
    exit(-1);
  }
  delete loader;
  std::cout << "finished" << std::endl;  
  return 0;
};



unsigned int EncodeApvIdValues(int FedId_, int feFPGA_, int channel_, int ApvPair_)
{
unsigned int apvpairid=0;
apvpairid = 
( (( FedId_  - 1) & FedId_Mask_   ) << FedId_StartBit_   ) | 
( (( feFPGA_ - 1) & feFPGA_Mask_  ) << feFPGA_StartBit_  ) | 
( (( channel_- 1) & channel_Mask_ ) << channel_StartBit_ ) | 
( (( ApvPair_- 1) & ApvPair_Mask_ ) << ApvPair_StartBit_ );
return apvpairid;
};

// uint32_t EncodeStripData(float ped_, float noise_, float lowTh_, float highTh_, bool disable_)
// {
//   // Encoding Algorithm from Fed9UUtils/src/Fed9UDescriptionToXml.cc
  
//   uint32_t low   = (static_cast<uint32_t>(lowTh_*5.0 + 0.5)  ) & 0x3F; 
//   uint32_t high  = (static_cast<uint32_t>(highTh_*5.0 + 0.5) ) & 0x3F;
//   uint32_t noise =  static_cast<uint32_t>(noise_*10.0 + 0.5)   & 0x01FF;
//   uint32_t ped   =  static_cast<uint32_t>(ped_)                & 0x03FF;
  
//   uint32_t stripData = (ped << 22) | (noise << 13) | (high << 7) | (low << 1) | ( disable_ ? 0x1 : 0x0 );

//   std::cout 
//     << std::fixed << ped_       << " \t" 
//     << std::fixed << noise_     << " \t" 
//     << lowTh_     << " \t" 
//     << highTh_    << " \t" 
//     << disable_  << " \t" 
//     << stripData << " \t" 
//     << std::endl;
  
//   return stripData;
// };

std::vector<int> DecodeApvIdValues(int apvpairid)
{
  std::vector<int> values;
  values.push_back(1 + (apvpairid >> FedId_StartBit_   ) & FedId_Mask_   ); 
  values.push_back(1 + (apvpairid >> feFPGA_StartBit_  ) & feFPGA_Mask_  );  
  values.push_back(1 + (apvpairid >> channel_StartBit_ ) & channel_Mask_ );   
  values.push_back(1 + (apvpairid >> ApvPair_StartBit_ ) & ApvPair_Mask_ );    
  return values;
};












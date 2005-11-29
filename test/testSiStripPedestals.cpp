#include "Reflex/Type.h"
#include "PluginManager/PluginManager.h"
#include "POOLCore/Token.h"
#include "FileCatalog/URIParser.h"
#include "FileCatalog/FCSystemTools.h"
#include "FileCatalog/IFileCatalog.h"
#include "StorageSvc/DbType.h"
#include "PersistencySvc/DatabaseConnectionPolicy.h"
#include "PersistencySvc/ISession.h"
#include "PersistencySvc/ITransaction.h"
#include "PersistencySvc/IDatabase.h"
#include "PersistencySvc/Placement.h"
#include "DataSvc/DataSvcFactory.h"
#include "DataSvc/IDataSvc.h"
#include "DataSvc/ICacheSvc.h"
#include "DataSvc/Ref.h"

//#include "SealUtil/SealTimer.h"
//#include "SealUtil/TimingReport.h"
//#include "SealUtil/SealHRRTChrono.h"
//#include "SealUtil/SealHRChrono.h"

#include "CondFormats/SiStripObjects/interface/SiStripPedestals.h"

#include "CondCore/IOVService/interface/IOV.h"
#include "CondCore/MetaDataService/interface/MetaData.h"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGauss.h"
#include<iostream>
#include<string>

#include "POOLCore/POOLContext.h"
#include "SealKernel/Exception.h"

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


unsigned int EncodeValues(int FedId_, int feFPGA_, int channel_, int ApvPair_);
std::vector<int> DecodeValues( int value);


int main(int csize, char** cline ) {
  const std::string userNameEnv = "POOL_AUTH_USER=cms_xiezhen_dev";
  ::putenv( const_cast<char*>( userNameEnv.c_str() ) );
  const std::string passwdEnv = "POOL_AUTH_PASSWORD=xiezhen123";
  ::putenv( const_cast<char*>( passwdEnv.c_str() ) );
  //std::string m_dbConnection( "oracle://devdb10/cms_xiezhen_dev");
  std::string m_dbConnection( "sqlite_file:test_SiStripPedestals.db");
  std::cout<<"connecting..."<<m_dbConnection<<std::endl;
  seal::PluginManager::get()->initialise();
  //seal::TimingReport timRep;



  
  try {
    // Loads the seal message stream
    pool::POOLContext::loadComponent( "SEAL/Services/MessageService" );
    pool::POOLContext::loadComponent( "POOL/Services/EnvironmentAuthenticationService" );


    pool::URIParser p;
    p.parse();
    
    pool::IFileCatalog lcat;
    pool::IFileCatalog * cat = &lcat;
    cat->setWriteCatalog(p.contactstring());
    cat->connect();
    cat->start();
    
    pool::IDataSvc *svc = pool::DataSvcFactory::instance(cat);
    // Define the policy for the implicit file handling
    pool::DatabaseConnectionPolicy policy;
    policy.setWriteModeForNonExisting(pool::DatabaseConnectionPolicy::CREATE);
    // policy.setWriteModeForExisting(pool::DatabaseConnectionPolicy::OVERWRITE);
    policy.setWriteModeForExisting(pool::DatabaseConnectionPolicy::UPDATE);
    svc->session().setDefaultConnectionPolicy(policy);
    cond::MetaData meta(m_dbConnection);


    if (csize==1) { //write
      long int tech(pool::POOL_RDBMS_StorageType.type()); 
      svc->transaction().start(pool::ITransaction::UPDATE);
      pool::Ref<cond::IOV> pedIov(svc, new cond::IOV);
      pool::Placement iovPlace(m_dbConnection, pool::DatabaseSpecification::PFN, "IOV", seal::reflex::Type(), tech); 
      pedIov.markWrite(iovPlace);
      std::string iovtoken=pedIov.toString();
      
      std::cout << "iovtoken " << iovtoken << std::endl;

      pool::Placement pedPlace(m_dbConnection, pool::DatabaseSpecification::PFN, "Pedestals", seal::reflex::Type(), tech); 
      
      //int totmodules=16000;
      int iovn=0;
      int evtn=10;
      for (int j=0; j<evtn; ++j) {
	iovn=j%5;//till time
	if(iovn==0){
	  pool::Ref<SiStripPedestals> ped(svc,new SiStripPedestals);
	  ped.markWrite(pedPlace);

	  std::string pedtoken=ped.toString();
	  std::cout << "pedtoken " << pedtoken << std::endl;

	  pedIov->iov[j+5]=pedtoken;
	  for(int fedid=1; fedid<=FedId_Max; ++fedid)
	    for(int fefpga=1; fefpga<=feFPGA_Max; ++fefpga)
	      for(int ch=1; ch<=channel_Max; ++ch)
		for(int apvpair=1;apvpair<=ApvPair_Max;apvpair++)
		  {	   
		    unsigned int numschema = EncodeValues(fedid,fefpga,ch,apvpair);
		    std::cout << "numschema " << numschema <<  std::endl;

		    SiStripPedestals::SiStripPedestalsVector theApvStripVector;
 		    for(int strip=0; strip<128; ++strip){
		      SiStripPedestals::Item theItem;
		      theItem.ped      = (int) RandGauss::shoot(MeanPed,RmsPed);
		      theItem.noise    = (int) RandGauss::shoot(MeanNoise,RmsNoise);
		      theItem.disabled = (RandFlat::shoot(1.) < badStripProb ? true:false) ;
		      //theItem.highTh   = 5;
		      //theItem.lowTh    = 2;
		     

		      std::cout << " strip " << strip << " =\t"
				<< theItem.ped      << " \t" 
				<< theItem.noise    << " \t" 
				<< theItem.disabled << " \t" 
			//<< theItem.lowTh    << " \t" 
			//<< theItem.highTh   << " \t" 
				<< std::endl; 

		      theApvStripVector.push_back(theItem);
		    }
		    
		    ped->m_pedestals.insert(std::pair<unsigned int, SiStripPedestals::SiStripPedestalsVector > (numschema,theApvStripVector));
		  }	
	}	
      }
    
      svc->transaction().commit();
      std::cout << "iov size " << pedIov->iov.size() << std::endl;
      svc->session().disconnectAll();
      std::cout << "committed" << std::endl;
      std::cout << "iov tokens "<<iovtoken<< std::endl;
      if( meta.addMapping("TrackerCalibration",iovtoken) ){
	std::cout<<"new mapping inserted "<<std::endl;
      }else{
	std::cout<<"mapping exists, do nothing "<<std::endl;
      }
    }else {
      std::cout<<"Reading "<<std::endl;
      std::string iovAName(cline[1]);
      std::string iovAToken= meta.getToken(iovAName);
      
      std::cout << "iovAName " << iovAName << std::endl;
      std::cout << "iovAToken " << iovAToken << std::endl;
      
      svc->transaction().start(pool::ITransaction::READ);
      
      pool::Ref<cond::IOV> iovA(svc,iovAToken);
      //      std::pair<unsigned long,std::string> iovpair=*iovA->iov.lower_bound(0);
      std::pair<unsigned long,std::string> iovpair=*iovA->iov.lower_bound(7);
      std::cout<<"iov idx "<<iovpair.first<<std::endl;
      std::string pedCid =iovpair.second;
      std::cout << "ped Cid " << pedCid << std::endl;
     
      pool::Ref<SiStripPedestals> ped(svc,pedCid);

      std::cout << "Pedestals for " << std::endl;
	
      SiStripPedestals::SiStripPedestalsMapIterator mapit = (*ped).m_pedestals.begin();
      for (;mapit!=(*ped).m_pedestals.end();mapit++)
	{
	  unsigned int numschema = (*mapit).first;
	    std::cout << "mappit " <<  numschema << std::endl;
	  
	  std::vector<int> values = DecodeValues(numschema);
	  SiStripPedestals::SiStripPedestalsVector theApvStripVector =  (*mapit).second;
	  std::cout << "Item for " 
		    << " FedId_   \t" << values[0]
		    << " feFPGA_  \t" << values[1]
		    << " strip_   \t" << values[2]
		    << " ApvPair_ \t" << values[3]
		    << " numschema \t"<< numschema << std::endl;
	  
	  for(int strip=0; strip<128; ++strip){
	    SiStripPedestals::Item theItem = theApvStripVector[strip];
		      
	    std::cout << " strip " << strip << " =\t"
		      << theItem.ped      << " \t" 
		      << theItem.noise    << " \t" 
		      << theItem.disabled << " \t" 
	      //<< theItem.lowTh    << " \t" 
	      //<< theItem.highTh   << " \t" 
		      << std::endl; 
	    
	  } 
	}
    }//end else
    std::cout << "commit catalog" << std::endl;
    cat->commit();
    delete svc;
  }   
  catch ( seal::Exception& e ) {
    std::cout << e.what() << std::endl;
    return 1;
  }
  catch ( std::exception& e ) {
    std::cout << e.what() << std::endl;
    return 1;
  }
  catch ( ... ) {
    std::cout << "Funny error" << std::endl;
    return 1;
  }
  
  // timRep.dump();
  
  std::cout << "finished" << std::endl;
  
  return 0;
};



unsigned int EncodeValues(int FedId_, int feFPGA_, int channel_, int ApvPair_)
{
  unsigned int apvpairid=0;
  apvpairid = 
    ( (( FedId_  - 1) & FedId_Mask_   ) << FedId_StartBit_   ) | 
    ( (( feFPGA_ - 1) & feFPGA_Mask_  ) << feFPGA_StartBit_  ) | 
    ( (( channel_- 1) & channel_Mask_ ) << channel_StartBit_ ) | 
    ( (( ApvPair_- 1) & ApvPair_Mask_ ) << ApvPair_StartBit_ );
  return apvpairid;
};



std::vector<int> DecodeValues(int apvpairid)
{
  std::vector<int> values;
  values.push_back(1 + (apvpairid >> FedId_StartBit_   ) & FedId_Mask_   ); 
  values.push_back(1 + (apvpairid >> feFPGA_StartBit_  ) & feFPGA_Mask_  );  
  values.push_back(1 + (apvpairid >> channel_StartBit_ ) & channel_Mask_ );   
  values.push_back(1 + (apvpairid >> ApvPair_StartBit_ ) & ApvPair_Mask_ );    
  return values;
};












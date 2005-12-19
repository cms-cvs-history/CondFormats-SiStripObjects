//Check if using Xerces version 1 
#if XERCES < 2
#warning "Using old version of XERCES, upgrade to 2 if you want to use test_Fed9UXMLDescription."
int main(void){}
#else

//Check Xerces version
#include "xercesc/util/XercesVersion.hpp"
#if _XERCES_VERSION >= 20300

#include"Fed9UDeviceFactory.hh"
#include"Fed9UXMLDescriptionFromFile.hh"

#include "Fed9UAddress.hh"
#include "Fed9UDevice.hh"
#include "Fed9UDescription.hh"
#include "Fed9UDescriptionToXml.hh"
#include "ICExDecl.hh"

#include <stdlib.h>
#include<TRandom.h>
#include <iostream>
#include <fstream>

using namespace std;
using namespace Fed9U;

void printFed9UStripDescription(const Fed9UStripDescription &strip);
void CreateFedDescription(int iFed, Fed9UDescription &fed);

//&&&&&&&&&&&&&&&&&&&&&&&&&&
// Rob Values
const float MeanPed   = 100;
const float RmsPed    = 5;
const float MeanNoise = 5;
const float RmsNoise  = 1;
//&&&&&&&&&&&&&&&&&&&&&&&&&&

const double badStripProb = .002;
int TotFedNumber;

int main(int argc, char *argv[])
{

  //#ifdef DATABASE
  //Create a Fed9UDescription (this would be the default description).
  Fed9UDescription theFed9UDescription;
  //cout << "1" << endl;
  //Fed9UDescription * bob = new Fed9UDescription();
  //cout << "2" << endl;
  //Fed9UDescription * rob = new Fed9UDescription(theFed9UDescription);
  //return 0;

  // vector<Fed9UDescription*>* descriptionList;
  u16 versionMajor = 0;
  u16 versionMinor = 0;
  cout << "Program started..." << endl;

  bool wasError(false);

  int startFed=0;

  try
    {

      bool firstFed = false;      
      TotFedNumber = 1;
      if (argc>1)
	TotFedNumber = atoi(argv[1]);
      if (argc>2)  
	firstFed = atoi(argv[2]);
      if (argc>3) 
	startFed = atoi(argv[3]);
      

      cout << "TotFedNumber " << TotFedNumber << endl;
      cout << "firstFed     " << firstFed  << endl;

      //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&


      cout << "Ready to go..." << endl;
      string login="nil", passwd="nil", path="nil" ;
      DbFedAccess::getDbConfiguration (login, passwd, path) ;
      cout << "got the configuration from dbFedAccess..." << endl;
      cout << "The login is now: " << login << " " << passwd << " " << path << endl;
      if (login != "nil") {
      
	//Create a Fed9UDeviceFactory
	cout << "Enter the instance Id for the factory" << endl;
	u16 instanceId;
	//cin >> instanceId;
	instanceId=0;
	cout << "instance Id = " << instanceId << endl;
	//Fed9UDeviceFactory theDeviceFactory;
	Fed9UDeviceFactory theDeviceFactory;
	
	theDeviceFactory.setInstance(instanceId);
	cout << "Constructed FedDeviceFactory.." << endl;
	theDeviceFactory.setDebugOutput(true);

 	
	//&&&&&&&&&&&&&&&&&&&&&&&&&&&&
	//char choice;	
	// 	cout << "do you want to include the strips in your download or upload y / n?" << endl;
	// 	//cin >> choice;
	// 	choice='y';
	// 	cout << "Choice  " << choice << endl;
	// 	if (choice == 'n')
	// 	  theDeviceFactory.setUsingStrips(false);
 	//&&&&&&&&&&&&&&&&&&&&&&&&&&&&

	//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
	// 	cout << "do you want to set the partition for up or download?  y / n?" << endl;
	// 	//cin >> choice;
	// 	choice='y';
	// 	cout << "Choice  " << choice << endl;

 	string partitionName;
	partitionName="test";

	// 	if (choice == 'y') {
	// 	  cout << " enter the partition name! " << endl;
	// 	  //cin >> partitionName;
	// 	  partitionName="testNov05";
	
	// 	} else partitionName = "null";
	//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
	cout << "Partition Name " << partitionName << endl;

	char choice;
	
	cout << "do you want to upload to the database or to file? d for databse, f for file" << endl;
	cin >> choice;
	bool newmajorversion = false;
	 
	//string theXMLFileName = "output.xml";	
	//string theXMLFileName = argc>1 ? argv[1] :  "output.xml";
	//cout << ">>>> theXMLFileName : " << theXMLFileName << endl;
	//vector<Fed9UDescription*>* tempdescriptionlist = new vector<Fed9UDescription*>; 
	//Fed9UXMLDescriptionFromFile xml(theXMLFileName, theFed9UDescription, tempdescriptionlist);
	//xml.makeNewFed9UDescription();    

	// FIXME
	//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
	for (int iFed=startFed;iFed<TotFedNumber;iFed++)
	  {

	    CreateFedDescription(iFed,theFed9UDescription);

	      
	    if ( choice == 'f') {
	      // now instruct the DeviceFactory that you are using file
	      theDeviceFactory.setUsingFile();
	      // set the output filename
	      char outputfilename[128];
	      sprintf(outputfilename,"factoryoutput_%d.xml",iFed);
	      theDeviceFactory.setOutputFileName(outputfilename);
	    }
	    // else {
	    // 		cout << "Do you wish to upload new major version? y/n" << endl;
	    // 		//cin >> choice;
	    // 		//newmajorversion = choice=='y'?1:0; 
	    // 	      }


	    cout << "\ndbConnected = " << (theDeviceFactory.getDbConnected()?"true":"false") << "\n" << endl ;
	      
	    theDeviceFactory.setFed9UDescription(theFed9UDescription,partitionName, &versionMajor, &versionMinor, firstFed?1:2);
	      
	    cout << std::dec << "Uploaded to version = " << versionMajor << "." << versionMinor << " first Fed?" << (firstFed?"true":"false")<<  endl;
	      
	    cout << "The version uploaded to " << versionMajor << "." <<versionMinor <<endl;
	    cout << "firstFed " << firstFed << "\n" << endl;
	    firstFed=false;	      
	  }
      } else {
	wasError=true;
	cout << " Error!!!! could not get database login!" << endl;
      }
    }
  catch (ICUtils::ICException &e) {
    cout << e.what();
    wasError = true;
  }
  catch (exception &e) {
    cout << e.what();
    wasError = true;
  }
  catch (...) { //Catch all types of exceptions
    cout << "ERROR: Some kind of exception caught in " << __FILE__ << endl;
    wasError = true;
  }
  catch (oracle::occi::SQLException e) {
    std::cerr << e.getMessage() << std::endl;
    cout << e.what();
    wasError = true;
  } 
  
  
  
//   try {
//     //Test the new Fed9UDescription (read back the delay settings for fpga=0 & channel=2). 
//     if (!wasError) {
//       Fed9UAddress addr;
//       addr.setExternalFedFeUnit(1).setFeUnitChannel(3);
//       cout << "This FED description is called: " << theFed9UDescription.getName() << endl;
//       cout << "\nWe have set the FED up in...." << endl;
//       cout << "DAQ mode: " <<  theFed9UDescription.getDaqMode() << endl;
//       cout << "Scope length: " << theFed9UDescription.getScopeLength() << endl;
//       cout << "Clock source: " << theFed9UDescription.getClock() << endl;
//       cout << "Trigger source: " << theFed9UDescription.getTriggerSource() << endl;
//       cout << "Read route: " << theFed9UDescription.getBeFpgaReadRoute() << endl;
//       cout << "Test Register: " << theFed9UDescription.getTestRegister() << endl;
//       cout << "The base address is: " << theFed9UDescription.getBaseAddress() << endl;
//       cout << "The FED ID is: " << theFed9UDescription.getFedId() << endl;
//       if (theFed9UDescription.getFedBeFpgaDisable()) cout << "This FED will be disabled." << endl;
//       else cout << "This FED will be enabled." << endl;
//       cout << "\nWhat are the settings for fpga=0 & channel=3 in the new Fed9UDescription...." << endl;
//       cout << "FeFpgaDisable: " << theFed9UDescription.getFedFeUnitDisable(addr) << endl;
//       cout << "CoarseDelay: " << theFed9UDescription.getCoarseDelay(addr) << endl;
//       cout << "FineDelay: " << theFed9UDescription.getFineDelay(addr) << endl;
//       cout << "FrameThreshold: " << theFed9UDescription.getFrameThreshold(addr) << endl; 
//       cout << "TrimDacOffset: " << theFed9UDescription.getTrimDacOffset(addr) << endl;
      
//       cout << "Getting the ADC controls: " << endl;
//       struct Fed9UAdcControls theFed9UAdcControls;
//       theFed9UAdcControls = theFed9UDescription.getAdcControls(addr);
//       cout << "dfsen: " << theFed9UAdcControls._dfsen << endl;
//       cout << "dfsval: " << theFed9UAdcControls._dfsval << endl;
//       cout << "s1: " << theFed9UAdcControls._s1 << endl;
//       cout << "s2: " << theFed9UAdcControls._s2 << endl;
      
//       cout << "Getting the temperature controls: " << endl;
//       Fed9UTempControl theFed9UTempControl;
//       theFed9UTempControl = theFed9UDescription.getTempControl(addr);
//       cout << "maxLm82Temp: " << theFed9UTempControl.getLm82High() << endl;
//       cout << "maxFpgaTemp: " << theFed9UTempControl.getFpgaHigh() << endl;
//       cout << "maxCriticalTemp: " << theFed9UTempControl.getCritical() << endl;
//       cout << "Getting the OptoRx settings: " << endl;
//       //   cout << "Offset: " << theFed9UDescription.getOptoRxOffset(addr) << endl;
//       cout << "Input Offset: " << theFed9UDescription.getOptoRxInputOffset(addr) << endl;
//       cout << "Output Offset: " << theFed9UDescription.getOptoRxOutputOffset(addr) << endl;
//       cout << "Capacitor: " << theFed9UDescription.getOptoRxCapacitor(addr) << endl;
      
//       cout << "What are the APV settings for this channel...." << endl;
//       addr.setChannelApv(0);
//       cout << "ApvDisable: " << theFed9UDescription.getApvDisable(addr) << endl;
//       cout << "MedianOverride: " << theFed9UDescription.getMedianOverride(addr) << endl;
      
//       cout << "   What are the 1st and 2nd strip parameters...." << endl;
//       addr.setApvStrip(0);
//       Fed9UStripDescription theFed9UStripDescription = theFed9UDescription.getFedStrips().getStrip(addr);
//       printFed9UStripDescription(theFed9UStripDescription);
//       addr.setApvStrip(1);
//       theFed9UStripDescription = theFed9UDescription.getFedStrips().getStrip(addr);
//       printFed9UStripDescription(theFed9UStripDescription);
      
//       addr.setChannelApv(1);
//       cout << "ApvDisable: " << theFed9UDescription.getApvDisable(addr) << endl;
//       cout << "MedianOverride: " << theFed9UDescription.getMedianOverride(addr) << endl;
      
//       cout << "   What are the 127th strip parameters...." << endl;
//       addr.setApvStrip(127);
//       theFed9UStripDescription = theFed9UDescription.getFedStrips().getStrip(addr);
//       printFed9UStripDescription(theFed9UStripDescription);
      
//       if (!theFed9UDescription.getMedianOverrideDisable(addr)) {
// 	cout << "The median values have been set, therefore the median calculation is disabled." << endl; 
//       }
      
//       cout << "And the temperatutre controls for the BE FPGA are: " << endl;
//       addr.setFedFeUnit(Fed9UAddress::BACKEND);
//       theFed9UTempControl = theFed9UDescription.getTempControl(addr);
//       cout << "maxLm82TempBeFpga: " << theFed9UTempControl.getLm82High() << endl;
//       cout << "maxFpgaTempBeFpga: " << theFed9UTempControl.getFpgaHigh() << endl;
//       cout << "maxCriticalTempBeFpga: " << theFed9UTempControl.getCritical() << endl;
      
//       cout << "\nTesting the input/output opto rx set/get in Fed9UDescription..." << endl;
//       cout << " Opto RX input setting is now " << theFed9UDescription.getOptoRxInputOffset(Fed9UAddress(1)) << endl;
//       cout << " Opto RX output setting is now " << theFed9UDescription.getOptoRxOutputOffset(Fed9UAddress(1)) << endl;
//       theFed9UDescription.setOptoRxInputOffset(Fed9UAddress(1),1);
//       cout << " Opto RX input setting is now " << theFed9UDescription.getOptoRxInputOffset(Fed9UAddress(1)) << endl;
//       cout << " Opto RX output setting is now " << theFed9UDescription.getOptoRxOutputOffset(Fed9UAddress(1)) << endl;
//       theFed9UDescription.setOptoRxOutputOffset(Fed9UAddress(1),2);
//       cout << " Opto RX input setting is now " << theFed9UDescription.getOptoRxInputOffset(Fed9UAddress(1)) << endl;
//       cout << " Opto RX output setting is now " << theFed9UDescription.getOptoRxOutputOffset(Fed9UAddress(1)) << endl; 
//       //cout << " Opto RX full setting is now " << theFed9UDescription.getOptoRxOffset(Fed9UAddress(1)) << endl;
      
//       //Read the TTC parameters
//       cout << "TTC Parameters: " << endl;
//       Fed9UTtcrxDescription theFed9UTtcrxDescription;
//       theFed9UTtcrxDescription = theFed9UDescription.getTtcrx();
//       cout << "The L1AcceptCourseDelay: " << theFed9UTtcrxDescription.getL1AcceptCoarseDelay() << endl;
//       cout << "The L1AcceptFineDelay: " << theFed9UTtcrxDescription.getClockDesOneFineDelay() << endl;
//       cout << "The BrcstStrTwoCoarseDelay: " << theFed9UTtcrxDescription.getBrcstStrTwoCoarseDelay() << endl;
//       int theBrcstStrTwoFineDelay = 0;
//       if (true == theFed9UTtcrxDescription.getDeskewedClock2Selected()) {
// 	theBrcstStrTwoFineDelay = theFed9UTtcrxDescription.getClockDesTwoFineDelay();
//       } 
//       else {
// 	theBrcstStrTwoFineDelay = theFed9UTtcrxDescription.getClockDesOneFineDelay();
//       }
//       cout << "The BrcstStrTwoFineDelay: " << theBrcstStrTwoFineDelay << endl;
//       cout << "The DLLPumpCurrent: " << theFed9UTtcrxDescription.getDllPumpCurrent() << endl;
//       cout << "The PLLPumpCurrent: " << theFed9UTtcrxDescription.getPllPumpCurrent() << endl;
//       cout << "The IacId: " << theFed9UTtcrxDescription.getIacId() << endl;
//       cout << "The I2cId: " << theFed9UTtcrxDescription.getI2cId() << endl;
//       cout << "The HammingChecking: " << theFed9UTtcrxDescription.getHammingCheckingDisable() << endl;
//       cout << "The BunchCounter: " << (theFed9UTtcrxDescription.getCounterOperation() & FED9U_TTCRX_ENABLE_BUNCH_CTR) << endl;
//       cout << "The EventCounter: " << (theFed9UTtcrxDescription.getCounterOperation() & FED9U_TTCRX_ENABLE_EVENT_CTR) << endl;
//       cout << "The Clock40DeskwedTwo: " << theFed9UTtcrxDescription.getDeskewedClock2Selected() << endl;
//       cout << "The Clock40DeskwedTwoOutput: " << theFed9UTtcrxDescription.getClockL1AcceptDisable() << endl;
//       cout << "The ClockL1AcceptOutput: " << theFed9UTtcrxDescription.getClockL1AcceptDisable() << endl;
//       cout << "The ParallelOutputBus: " << theFed9UTtcrxDescription.getParrallelOutputDisable() << endl;
//       cout << "The SerialBOutput: " << theFed9UTtcrxDescription.getSerialBDisable() << endl;
//       cout << "The NonDeskwedClock40Output: " << theFed9UTtcrxDescription.getNonDeskewedClockDisable() << endl;
      
//       //Read the voltage monitor parameters
//       cout << "\nVOLTAGE MONITOR parameters: " << endl;
//       Fed9UVoltageControl theFed9UVoltageControl = theFed9UDescription.getVoltageMonitor();
//       cout << "The standby flag: " << theFed9UVoltageControl.getStandBy() << endl;
//       cout << "The ResetStatusRegister flag: " << theFed9UVoltageControl.getResetStatusRegister() << endl;
//       cout << "The 2.5V min: " << theFed9UVoltageControl.get2Point5VoltMin() << endl;
//       cout << "The 2.5V max: " << theFed9UVoltageControl.get2Point5VoltMax() << endl;
//       cout << "The 3.3V min: " << theFed9UVoltageControl.get3Point3VoltMin() << endl;
//       cout << "The 3.3V max: " << theFed9UVoltageControl.get3Point3VoltMax() << endl;
//       cout << "The 5V min: " << theFed9UVoltageControl.get5VoltMin() << endl;
//       cout << "The 5V max: " << theFed9UVoltageControl.get5VoltMax() << endl;
//       cout << "The 12V min: " << theFed9UVoltageControl.get12VoltMin() << endl;
//       cout << "The 12V max: " << theFed9UVoltageControl.get12VoltMax() << endl;
//       cout << "The core V min: " << theFed9UVoltageControl.getCoreVoltageMin() << endl;
//       cout << "The core V max: " << theFed9UVoltageControl.getCoreVoltageMax() << endl;
//       cout << "The supply V min: " << theFed9UVoltageControl.getSupplyVoltageMin() << endl;
//       cout << "The supply V max: " << theFed9UVoltageControl.getSupplyVoltageMax() << endl;
//       cout << "The external T min: " << theFed9UVoltageControl.getExternalTempMin() << endl;
//       cout << "The external T max: " << theFed9UVoltageControl.getExternalTempMax() << endl;
//       cout << "The internal T min: " << theFed9UVoltageControl.getInternalTempMin() << endl;
//       cout << "The internal T max: " << theFed9UVoltageControl.getInternalTempMax() << endl;
//       cout << "The offset T: " << theFed9UVoltageControl.getTempOffset() << endl;
//       cout << "The offset T selection: " << theFed9UVoltageControl.getOffsetTempSelect() << endl;
      
//       //Read the EPROM parameters
//       cout << "\nEPROM Parameters: " << endl;
//       Fed9UEpromDescription theFed9UEpromDescription = theFed9UDescription.getEprom();
//       cout << "The serial number: " << theFed9UEpromDescription.getFedSerialNumber() << endl;
      
//       cout << "\nGLOBAL FE SKEWS: " << endl;
//       cout << "Coarse: " << "***NOT_IMPLEMENTED_YET***" << endl;
//       cout << "Fine: " << theFed9UDescription.getGlobalFineSkew() << endl;
//     }
//   }catch (ICUtils::ICException & exc) {
//     cout << exc.what() << endl;
//   } 



  cout << endl << "Program finished." << endl;
  
  return 0;
  
}


void CreateFedDescription(int iFed, Fed9UDescription &f)
{

  TRandom theRandomGen(iFed*123456);

  try{
    Fed9UAddress addr;
    //Fed9UDescription f;
    
    f.loadDefaultDescription();
    
    //FIXME
    // Address = 0x(slot)0000
    f.setBaseAddress(0x40000+iFed);
    //

    f.setHalAddressTable("../Fed9UStandAlone/Fed9UAddressTable.dat");
    f.setFedBeFpgaDisable(0);
    f.setClock(FED9U_CLOCK_TTC);
    f.setDaqMode(FED9U_MODE_SCOPE);
    f.setScopeLength(280);
    f.setTriggerSource(FED9U_TRIG_TTC);
    f.setTestRegister(0);

    // FIXME
    // FED address
    cout << "FedID " <<   50  + iFed << endl;	  
    //f.setFedId(0x4);
    f.setFedId(50+iFed);
    
    //FIXME
    f.setFedHardwareId(iFed);
    //  templateDescription.setFedHardwareId(fe.getFed9UABC()->getFedHardwareId());
    //  templateDescription.setFedVersion(fe.getFed9UABC()->getFedVersion());


    f.setBeFpgaReadRoute(FED9U_ROUTE_VME);

    for(u32 i = 0; i < FEUNITS_PER_FED; i++)
      {
	addr.setFedFeUnit(i);
	f.setFedFeUnitDisable(addr, true);
	f.setOptoRxInputOffset(addr, 0x05);
	f.setOptoRxOutputOffset(addr, 0);
	f.setOptoRxCapacitor(addr, 0);

	for(u32 i = 0; i < CHANNELS_PER_FEUNIT; i++)
	  {
	    addr.setFeUnitChannel(i).setChannelApv(0);
	    f.setApvDisable(addr,0);
	    addr.setChannelApv(1);
	    f.setApvDisable(addr,0);
	  }
      }

    //        f.setApvDisable(addr.setFedFeUnit(0).setFeUnitChannel(0).setChannelApv(0),0);
    //        f.setApvDisable(addr.setFedFeUnit(0).setFeUnitChannel(0).setChannelApv(1),0);
    //        f.setApvDisable(addr.setFedFeUnit(0).setFeUnitChannel(1).setChannelApv(0),0);
    //        f.setApvDisable(addr.setFedFeUnit(0).setFeUnitChannel(1).setChannelApv(1),0);
    //       int a;
    //       cin>>a;
    f.setFedFeUnitDisable(addr.setFedFeUnit(0), false);
    f.setFedFeUnitDisable(addr.setFedFeUnit(1), false);

    for (u32 i = 0; i < CHANNELS_PER_FED; i++)
      {
	addr.setFedChannel(i);
	f.setFineDelay(addr, 0);
	f.setCoarseDelay(addr, 0);
	f.setTrimDacOffset(addr, 0x7f);
	f.setFrameThreshold(addr, 480);
	f.setAdcControls(addr, true, false, true, false);
      }

    for (u32 i = 0; i < APVS_PER_FED; i++)
      {
	addr.setFedApv(i);
	vector<u32> pedestals(128,0);
	vector<float> noise(128,0);
	vector<bool> disableStrips(128,0);
	vector<u32> highThresholds(128,0);
	vector<u32> lowThresholds(128,0);
	vector<Fed9UStripDescription> apvStripDescription(128);

	for (u32 j = 0; j < STRIPS_PER_APV; j++)
	  {
	    pedestals[j] = (u32) theRandomGen.Gaus(MeanPed,RmsPed);
	    noise[j]     = theRandomGen.Gaus(MeanNoise,RmsNoise);
	    
	    disableStrips[j] = (theRandomGen.Uniform() < badStripProb ? true:false) ;
	    highThresholds[j] = 5;
	    lowThresholds[j] = 2;

	    apvStripDescription[j].setPedestal(pedestals[j]);
	    apvStripDescription[j].setDisable(disableStrips[j]);
	    apvStripDescription[j].setLowThreshold(lowThresholds[j]);
	    apvStripDescription[j].setHighThreshold(highThresholds[j]);
	    apvStripDescription[j].setNoise(noise[j]);
	  
	    //cout << "ped " << pedestals[j] << " noise " << noise[j] << " GetNoise " << apvStripDescription[j].getNoise() << endl; 
	  }
	f.getFedStrips().setApvStrips (addr, apvStripDescription);
      }

    char filename[128];
    sprintf(filename,"xml/Description_Fed_%d.xml",iFed);
    cout << "............ Creating file " << filename <<endl;

    Fed9UDescriptionToXml theFed9UDescriptionToXml(filename, f);
    theFed9UDescriptionToXml.writeXmlFile();
    cout << " ------------------------------------------" << endl;
  }
  catch(exception& e)
    {
      cout << "Caught exception:\n" << e.what() << endl;
    }
  catch( ... )
    {
      cout << "Caught an unknown exception." << endl;
    }
}

void printFed9UStripDescription(const Fed9UStripDescription &strip)
{
if (strip.getDisable()) cout << "   This strip is disabled." << endl;
if (!strip.getDisable()) cout << "   This strip is enabled." << endl;
cout << "   The pedestal is: " << strip.getPedestal() << endl;
cout << "   The low is: " << strip.getLowThreshold() << endl;
  cout << "   The high is: " << strip.getHighThreshold() << endl;
}


#endif
#endif

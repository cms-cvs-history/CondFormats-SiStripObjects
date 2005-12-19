/* @author Saima Iqbal
 * o2o transformation code for Tracker to generate pedestal object
 */
 
#include <iostream>
#include <sstream>
using namespace std;
#include <stdio.h>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <vector>
#define OTL_ORA9I // Compile OTL 4.0/OCI9i
#include "otlv4.h" // include the OTL 4.0 header file
otl_connect db; // connect object
#include<time.h>

int Number_of_apvs_to_upload_in_a_bunch;
int Apvs_uploaded;
int Apvs_uploaded_tot;

time_t start_time, stop_time, sleep_time;


unsigned int EncodeValues(int FedId_, int feFPGA_, int channel_, int ApvPair_);
int recordnum();
int getlastiov();
vector<int> getversionmajorid();
void select(int &recnum);
//void select_IOV(int iov, int &versionmajorid);
void select_pedestals_values(int iov, int &versionmajorid);

bool already_in_table(char * query);

void insert_in_table_sistrippedestals(int iov_value_id);
void insert_in_table_sistrippedestals_map(int iov_value_id, int map_pos, int apvid);
void insert_in_table_sistrippedestals_value(int iov_value_id, int map_pos, int vec_pos, int value);


void sleep()
{
  cout << "Sleeping for " << sleep_time << " s"<< endl; 
  time_t time_1, time_2;
  
  time(&time_1);
  time(&time_2);

  while (difftime(time_2,time_1)<sleep_time)
    {      
      time(&time_2);
    }

  cout << "End sleep: Current Date " << ctime(&time_2) << endl;
}

unsigned int EncodeValues(int FedId_, int feFPGA_, int channel_, int ApvPair_)
{
  static const  int FedId_Mask_   = 0x3FF;
  static const  int feFPGA_Mask_  = 0x7;
  static const  int channel_Mask_ = 0xF;
  static const  int ApvPair_Mask_ = 0x1;
  
  static const  int FedId_StartBit_   = 8;
  static const  int feFPGA_StartBit_  = 5;
  static const  int channel_StartBit_ = 1;
  static const  int ApvPair_StartBit_ = 0;
  
  unsigned int apvpairid=0;
  apvpairid = 
    ( (( FedId_  - 1) & FedId_Mask_   ) << FedId_StartBit_   ) | 
    ( (( feFPGA_ - 1) & feFPGA_Mask_  ) << feFPGA_StartBit_  ) | 
    ( (( channel_- 1) & channel_Mask_ ) << channel_StartBit_ ) | 
    ( (( ApvPair_- 1) & ApvPair_Mask_ ) << ApvPair_StartBit_ );
  return apvpairid;
};


// int recordnum()
// {
//   otl_stream r(50, // buffer size
// 	       "select count(*) from strip", // select statement
// 	       db //connect object
// 	       );		
//   int rowcount; 
//   while(!r.eof()){ // while not end-of-data 
//     r>>rowcount;
//     return rowcount;
//   } //while
//   return 0;
// } //recordnum()

vector<int> getversionmajorid()
{
  otl_stream r(50, // buffer size
	       "select strip.versionmajorid from strip where (strip.versionmajorid>18) group by strip.versionmajorid", // select statement
	       db //connect object
	       );		
  vector<int> vmjid; 
  while(!r.eof()){ // while not end-of-data 
    int mjid;
    r>> mjid;
    vmjid.push_back(mjid);
  } //while
  return vmjid;
} 

int getlastiov()
{
  otl_stream r(50, // buffer size
	       "select max(cms_streamuser.sitrackerpedestals.iov_value_id) from cms_streamuser.sitrackerpedestals group by cms_streamuser.sitrackerpedestals.iov_value_id", // select statement
	       db //connect object
	       );		
  int iov_value_id; 
  while(!r.eof()){ // while not end-of-data 
    r>> iov_value_id;
    return iov_value_id;
  } //while
  return iov_value_id;
} 

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

bool already_in_table(char * query)
{
  //cout << "\nquery :\t " << query << "\n" << endl;
  
  otl_stream i(50, // buffer size
	       query,// SELECT statement
	       db // connect object
	       );// create select stream
  int count;
  while(!i.eof()){ // while not end-of-data
    //get data from online db    
    i >> count;
    //cout << "count " << count << endl;
    return count;
  }
  return false;
}

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&


void select(int &recnum)
{ 

  int iov_value_id=1;//getlastiov()+1;
  //  cout << "last iov = " << getlastiov() << endl;
  
  vector<int> vmjid=getversionmajorid();


  time(&start_time);
  cout << "start_time: Current Date " << ctime(&start_time) << endl;
  
  for (vector<int>::iterator iter=vmjid.begin();iter!=vmjid.end();iter++)
    {
      cout << "\n--------------------------------\nmjid = " << (*iter) << "\t iov_value_id \t "<< iov_value_id << "\n " << endl;

      //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      // FIXME
      // at the moment pedestals table is filled 
      //without read the correspoinding tables
      
      //select_pedestals(iov_value_id,(*iter));
      int time=0;
      //load_pedestals(iov_value_id,time);
      //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

      insert_in_table_sistrippedestals(iov_value_id);
      select_pedestals_values(iov_value_id,(*iter));
      
      iov_value_id++;
    }
}

// void select_pedestals(int iov_value_id, int &versionmajorid)
// {
// //   // selecting data and transformed to generate PEDESTALS object 
// //   // creating input stream for pedestals table
   
//   char query[1024];
//   sprintf(query,
// 	  ""
// 	  );  
//   otl_stream p(50, // buffer size
// 	       query,// SELECT statement
// 	       db // connect object
// 	       );// create select stream
//   int time=0;

//   while(!p.eof()){ // while not end-of-data
//     /* loading data into PEDESTALS table */
//     p>>time;
//     cout<<time<<endl;  

//     iov_value_id++;
//     load_pedestals(iov_value_id,time);
//   } //while 
// }


void select_pedestals_values(int iov_value_id, int &versionmajorid)
{
  // creating input stream for pedestals_value

  cout << "SELECT_PEDESTALS_VALUES from ONLINEDB" << endl;

  time_t start_query_time, stop_query_time;

  time(&start_query_time);
  
  char query[1024];
  sprintf(query,
	  "select "
	  "strip.versionmajorid,"
	  "fed.id,"
	  "fefpga.id,"
	  "channel.id,"
	  "apvfed.id, "
	  "strip.apvid, "
	  "strip.value "
	  "from fed,fefpga,channel,apvfed,strip "
	  "where ("
	  "fed.fedid=fefpga.fedid and "
	  "fefpga.fefpgaid=channel.fefpgaid and "
	  "channel.channelid=apvfed.channelid and "
	  "apvfed.deviceid=strip.apvid and "
	  "strip.versionmajorid=%d "
	  //"and fed.id=50 "
	  ")"
	  " order by fed.id,strip.apvid",
	  versionmajorid
	  );

  cout << "start_query_time: Current Date " << ctime(&start_query_time) << endl;
  cout << "\nquery :\t " << query << "\n" << endl;
  
  otl_stream i(2048, // buffer size
	       query,// SELECT statement
	       db // connect object
	       );// create select stream


  time(&stop_query_time);

  cout << "stop_query_time: Current Date " << ctime(&stop_query_time) << endl;


  int fedid, fefpgaid, channelid, apvfedid,apvid;
  int versionmajid; 
  char value[1024];
    
  int map_pos=0;
  while(!i.eof()){ // while not end-of-data

    //get data from online db    
    i >> versionmajid>> fedid >>  fefpgaid >> channelid >> apvfedid >> apvid >> value ;
    
    int myapvid=EncodeValues(fedid,fefpgaid,channelid,apvfedid);

   //  cout 
//       << versionmajid << "\t " 
//       << fedid        << "\t "
//       << fefpgaid     << "\t "
//       << channelid    << "\t "
//       << apvfedid     << "\t "
//       << apvid        << "\t "
//       << myapvid      << "\t "
//       << value        << "\t "
//       <<endl;

    //load_pedestals_values(iov_value_id,myapvid,value);
    

    insert_in_table_sistrippedestals_map(iov_value_id,map_pos,myapvid);

    //&&&&&&&&&&&&&&&&&&&&&&
    // Unpacking
    //&&&&&&&&&&&&&&&&&&&&&& 
    string s=value;
    int stripData;
    stringstream ss,ssNodeValue;
    
    ssNodeValue<< hex << s.c_str();
        
    int vec_pos=0;
    while (vec_pos<128){
      ss<< hex << ssNodeValue.str().substr(vec_pos*8,8);
      ss>>stripData;
      ss.clear();
      insert_in_table_sistrippedestals_value(iov_value_id,map_pos,vec_pos,stripData);
      vec_pos++;
    }
    //&&&&&&&&&&&&&&&&&&&&&&
    map_pos++;
    Apvs_uploaded++;

    
    if (Apvs_uploaded>=Number_of_apvs_to_upload_in_a_bunch)
      {
	time(&stop_time);
	Apvs_uploaded_tot+=Apvs_uploaded;

	cout << "------------------------------------" << endl;
	cout << "stop_time: Current Date: " << ctime(&stop_time) << endl;
	
	cout << "N apvs uploaded (in the bunch, tot ) = ( " << Apvs_uploaded << " \t,\t " << Apvs_uploaded_tot << " )" << endl;

	Apvs_uploaded=0;
	Number_of_apvs_to_upload_in_a_bunch=2*Number_of_apvs_to_upload_in_a_bunch;
      
	sleep();
	
	time(&start_time);
	cout << "start_time: Current Date: " << ctime(&start_time) << endl;		
	cout << "------------------------------------" << endl;

      }
       
    //cout << "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&" << endl;
  }
}

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

void insert_in_table_sistrippedestals(int iov_value_id)
{
  cout << " loading data into the SISTRIPPEDESTALS tables" << endl; 
 
  char query[1024];
  sprintf(query,
	  "select count(*) from cms_streamuser.sistrippedestals where (cms_streamuser.sistrippedestals.iov_value_id=%d)",
	  iov_value_id
	  );
 
  if ( already_in_table(query) ) return;

  otl_stream o(50, // buffer size
	       "insert into cms_streamuser.sistrippedestals values(:iov_value_id<int>)", // SQL statement
	       db // connect object
	       );// create insert stream
	  
  cout << "iov_value_id " << iov_value_id << endl;
  o << iov_value_id ;
}

void insert_in_table_sistrippedestals_map(int iov_value_id, int map_pos, int apvid)
{
  //  cout << "\n loading data into the SISTRIPPEDESTALS_MAP tables " << endl;

  char query[1024];
  sprintf(query,
	  "select count(*) from cms_streamuser.sistrippedestals_map where (cms_streamuser.sistrippedestals_map.iov_value_id=%d and cms_streamuser.sistrippedestals_map.map_pos=%d)",
	  iov_value_id,map_pos
	  );
  
  //if ( already_in_table(query) ) return;

  otl_stream o(50, // buffer size
	       "insert into cms_streamuser.sistrippedestals_map values(:map_pos<int>,:iov_value_id<int>,:apvid<int>)", 
	       // SQL statement
	       db // connect object
	       );// create insert stream
  
  if (map_pos%1000==0) 
    cout << "map_pos iov_value_id apvid " << map_pos << " " << iov_value_id << " " << apvid << endl;
  o << map_pos << iov_value_id << apvid;
}


void insert_in_table_sistrippedestals_value(int iov_value_id, int map_pos, int vec_pos, int StripData)
{
  //cout << "\n loading data into the SISTRIPPEDESTALS_VALUE tables " << endl;

  char query[1024];
  sprintf(query,
	  "select count(*) from cms_streamuser.sistrippedestals_value where (cms_streamuser.sistrippedestals_value.iov_value_id=%d and cms_streamuser.sistrippedestals_value.map_pos=%d and cms_streamuser.sistrippedestals_value.vec_pos=%d)",
	  iov_value_id,map_pos,vec_pos
	  );
  
  //if ( already_in_table(query) ) return;

  otl_stream o(50, // buffer size
	       "insert into cms_streamuser.sistrippedestals_value values(:map_pos<int>,:vec_pos<int>,:iov_value_id<int>,:stripvalue<int>)",// SQL statement
	       db // connect object
	       );// create insert stream
  
  //cout << "map_pos vec_pos iov_value_id StripData " << map_pos << " " << vec_pos << " " << iov_value_id << " " << StripData << endl;
  o << map_pos << vec_pos << iov_value_id << StripData;

  //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
  // ONLY TO VERIFY
  //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

//   int low = static_cast<float>((StripData >> 1) &0x0000003F)/5.0;
//   int high = static_cast<float>((StripData >> 7) &0x0000003F)/5.0;
//   float noise = static_cast<float>((StripData >> 13) &0x000001FF)/10.0;
//   int ped = static_cast<int>((StripData >> 22) &0x000003FF);
//   bool disable = static_cast<bool>(StripData &0x00000001); 

  //  cout << "low " << low << " high " << high << " noise " << noise << " ped " << ped << " disabled " << disable << endl;

}

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&


// void load_pedestals_values(int iov_value_id, unsigned int myapvid,char * value)
// {
//     cout << "\n loading data into the PEDESTALS_VALUES tables " << endl;

//     cout << "\n----\n"<< myapvid << "\t " << value << endl;
    
//     // otl_stream o(50, // buffer size
//     //	 "insert into cms_streamuser.sitrackerpedestals_value values(:apvid<int>,:iov_value_id<int>,:value<char[1025]>)", // SQL statement
//     //	 db // connect object
//     //	 );// create insert stream


//     //&&&&&&&&&&&&&&&&&&&&&&
//     // Unpacking
//     //&&&&&&&&&&&&&&&&&&&&&&
 
//     string s=value;
//     int stripData;
//     int c=0;
//     stringstream ss,ssNodeValue;
    
//     //    cout << "s.c_str " << s.c_str() << endl;
//     ssNodeValue<< hex << s.c_str();
//     //cout << " ssNodeValue " << ssNodeValue << endl;
    
//     while (c<128){

//       //cout << " ssNodeValue.str().substr " << ssNodeValue.str().substr(c*8,8) << endl;
      
//       ss<< hex << ssNodeValue.str().substr(c*8,8);
//       //cout << " ss " << ss << endl;
//       ss>>stripData;
//       ss.clear();
//       int low = static_cast<float>((stripData >> 1) &0x0000003F)/5.0;
//       int high = static_cast<float>((stripData >> 7) &0x0000003F)/5.0;
//       float noise = static_cast<float>((stripData >> 13) &0x000001FF)/10.0;
//       int ped = static_cast<int>((stripData >> 22) &0x000003FF);
//       bool disable = static_cast<bool>(stripData &0x00000001); 

//       cout << "low " << low << " high " << high << " noise " << noise << " ped " << ped << " disabled " << disable << endl;
//       c++;
//     }
//     //&&&&&&&&&&&&&&&&&&&&&&


//     //    o<< (int)  myapvid << iov_value_id << value;  
// }
  
int main(int argc, char * argv[])
{
  int recnum;
  int versionmajorid;

  sleep_time=60;

  Apvs_uploaded=0;
  Apvs_uploaded_tot=0;
  Number_of_apvs_to_upload_in_a_bunch=1000;
  if (argc==2) 
    Number_of_apvs_to_upload_in_a_bunch=atoi(argv[1]);
  cout << "Starting Number of apvs to upload in a bunch  " << Number_of_apvs_to_upload_in_a_bunch << endl;
  
  otl_connect::otl_initialize(); // initialize OCI environment
  try{
    //  db.rlogon("cmsdbtr/fred@devdb"); // connect to Oracle
    db.rlogon("cms_sitracker/sitracker@cmsomds"); // connect to Oracle
   
    //FIXME : recnum is usefull?
    //recnum=recordnum();
    recnum=0;
    select(recnum); // select records from table 
    /////////////////////////
  }
  catch(otl_exception& p){ // intercept OTL exceptions
    cerr<<p.msg<<endl; // print out error message
    cerr<<p.stm_text<<endl; // print out SQL that caused the error
    cerr<<p.var_info<<endl; // print out the variable that caused the error
  }
  db.logoff(); // disconnect from Oracle
  return 0;
}

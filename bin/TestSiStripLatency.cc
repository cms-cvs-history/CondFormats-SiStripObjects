#include "CondFormats/SiStripObjects/interface/SiStripLatency.h"

#include <algorithm>
#include <iostream>

using namespace std;

struct DetIdAndApvs
{
  uint32_t detId;
  vector<uint16_t> apvs;
};

void test( const vector<DetIdAndApvs> & detIdAndApvs, const vector<int> & indexes, vector<float> & latencies, SiStripLatency & latency )
{
  int i = 0;
  int k = 0;
  int flip = 1;
  vector<DetIdAndApvs>::const_iterator detIdAndApv = detIdAndApvs.begin();
  for( ; detIdAndApv != detIdAndApvs.end(); ++detIdAndApv ) {
    vector<uint16_t>::const_iterator apv = detIdAndApv->apvs.begin();
    for( ; apv != detIdAndApv->apvs.end(); ++apv, ++i ) {

      // cout << "detId = " << detIdAndApv->detId << ", apv = " << *apv << ", detIdAndApv = " << compactValue << endl;

      if( find(indexes.begin(), indexes.end(), i) != indexes.end() ){
        if( flip == 1 ) {
          k += 1;
        }
        else {
          k -= 1;
        }
        flip *= -1;
      }
      latency.put(detIdAndApv->detId, *apv, 1.3+k);

//       cout << "latency stored is = " << latency.get(detIdAndApv->detId, *apv) << endl;
      latencies.push_back(latency.get(detIdAndApv->detId, *apv));
//       cout << endl;
    }
  }
  // Finished filling, now compress the ranges
  latency.compress();
}

void check( const vector<float> & latencies, const vector<DetIdAndApvs> & detIdAndApvs, SiStripLatency & latency )
{
  vector<DetIdAndApvs>::const_iterator detIdAndApv = detIdAndApvs.begin();
  vector<float>::const_iterator it = latencies.begin();
  detIdAndApv = detIdAndApvs.begin();
  int errorCount = 0;
  for( ; detIdAndApv != detIdAndApvs.end(); ++detIdAndApv ) {
    vector<uint16_t>::const_iterator apv = detIdAndApv->apvs.begin();
    for( ; apv != detIdAndApv->apvs.end(); ++apv, ++it ) {
      uint32_t detId = detIdAndApv->detId;
      uint32_t detIdAndApvValue = (detId<<2)|(*apv);
      cout << "detId = " << detIdAndApv->detId << ", apv = " << *apv << ", detIdAndApv = " << detIdAndApvValue << endl;
      cout << "latency passed = " << *it << ", latency saved = " << latency.get(detIdAndApv->detId, *apv) << endl;
      if( *it != latency.get(detIdAndApv->detId, *apv) ) {
        cout << "ERROR: the values are different" << endl;
        ++errorCount;
      }
    }
  }
  cout << "error count = " << errorCount << endl;
}

int main()
{
  vector<DetIdAndApvs> detIdAndApvs;
  DetIdAndApvs element1;
  element1.detId = 100000;
  element1.apvs.push_back(0);
  element1.apvs.push_back(1);
  detIdAndApvs.push_back(element1);

  DetIdAndApvs element2;
  element2.detId = 100001;
  element2.apvs.push_back(0);
  element2.apvs.push_back(1);
  element2.apvs.push_back(2);
  detIdAndApvs.push_back(element2);

  DetIdAndApvs element3;
  element3.detId = 9998;
  element3.apvs.push_back(0);
  element3.apvs.push_back(1);
  element3.apvs.push_back(2);
  detIdAndApvs.push_back(element3);

  DetIdAndApvs element4;
  element4.detId = 9999;
  element4.apvs.push_back(0);
  element4.apvs.push_back(1);
  detIdAndApvs.push_back(element4);

  DetIdAndApvs element5;
  element5.detId = 100002;
  element5.apvs.push_back(0);
  element5.apvs.push_back(1);
  element5.apvs.push_back(2);
  detIdAndApvs.push_back(element5);

  cout << "Testing the SiStripLatency object" << endl << endl;

  // Testing with all the same values. Expected final size of internal ranges and latencies = 1
  vector<int> indexes;
  vector<float> latencies;
  SiStripLatency latency1;
//   test(detIdAndApvs, indexes, latencies, latency1);
//   cout << endl;
//   cout << "Filling complete, starting check" << endl;
//   cout << endl;
//   check(latencies, detIdAndApvs, latency1);

  SiStripLatency latency2;
  indexes.push_back(3);
  indexes.push_back(5);
  indexes.push_back(10);
  indexes.push_back(11);
  latencies.clear();
  test(detIdAndApvs, indexes, latencies, latency2);
  cout << endl;
  cout << "Filling complete, starting check" << endl;
  cout << endl;
  check(latencies, detIdAndApvs, latency2);


  return 0;
}

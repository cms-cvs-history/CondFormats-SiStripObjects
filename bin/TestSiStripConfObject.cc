#include "CondFormats/SiStripObjects/interface/SiStripConfObject.h"

#include <iostream>

using namespace std;

int main()
{
  cout << "Testing SiStripConfObject" << endl;

  SiStripConfObject conf;
  cout << "putting par1 with value 1" << endl;
  conf.put("par1", 1);
  cout << "putting par2 with value 2" << endl;
  conf.put("par2", 2);

  cout << "Reading back parameters" << endl;
  cout << "getting par1 = " << conf.get("par1") << endl;
  cout << "getting par2 = " << conf.get("par2") << endl;

  cout << "Trying to read back a non-existent parameter" << endl;
  cout << "getting par3 (non-existent) " << conf.get("par3") << endl; 
}

#include <iostream>

using namespace std;

int main() {

  float rh, rm, rs, rd;
  float dh, dm, ds, dd;

  cout << "Enter RA in format hh mm ss (with spaces):  ";
  cin >> rh >> rm >> rs;

  cout << endl << "Enter Dec in format dd mm ss (with spaces):  ";
  cin >> dh >> dm >> ds;

  rd = 15*(rh + rm/60 + rs/3600);
  dd = dh + dm/60 + ds/3600;

  cout << endl << "RA =  " << rd << endl;
  cout << "Dec = " << dd << endl;

  return 0;

}

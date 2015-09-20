#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include "Eigen/Dense"
#include "Eigen/Geometry"
#include <iomanip> //control the precision
#include <cmath>
#include "Molecule.h"
using namespace std;
int main()
{
	Molecule a;
	string filename;
	int molenum;
	cout << "Please enter file name:"<<endl;
	cin >> filename;
	cout << "Please enter atom numbers of this molecule" << endl;
	cin >> molenum;
	MepMoldenToXYZfiles(filename,a,molenum);
	system("PAUSE");
	return 0;
}
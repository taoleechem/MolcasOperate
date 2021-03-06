#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include "Eigen/Dense"
#include "Eigen/Geometry"
#include <iomanip> //control the precision
#include <cmath>
using namespace std;
const int MaxAtom = 150;
const int MaxDim = 3;
class Molecule
{
private:
	vector<string> name;
	string energy;
	double corr[MaxAtom][MaxDim];
public:
        double MaxRadius()
        {
           double N=name.size();
           double dis=0;
           for(int i=0;i!=N;i++)
             for(int j=i+1;j!=N;j++)
                {
                    if(sqrt((corr[i][1]-corr[j][1])*(corr[i][1]-corr[j][1])+(corr[i][2]-corr[j][2])*(corr[i][2]-corr[j][2])+(corr[i][3]-corr[j][3])*(corr[i][3]-corr[j][3]))>dis)
                     dis=sqrt((corr[i][1]-corr[j][1])*(corr[i][1]-corr[j][1])+(corr[i][2]-corr[j][2])*(corr[i][2]-corr[j][2])+(corr[i][3]-corr[j][3])*(corr[i][3]-corr[j][3]));
                }
               return dis;
        }
        friend double MaxRadius2(Molecule &a, Molecule &b,double dis)
            {
                return a.MaxRadius()+b.MaxRadius()+dis;
            }
         
	void clear()
	{
		name.clear();
		energy = "NULL";
		for (int i = 0; i != MaxAtom; i++)
			for (int j = 0; j != MaxDim; j++)
				corr[i][j] = -1024;
	}
	void operator=(Molecule a)
	{
		vector<string>::iterator aiter;
		int i = 0;
		if (name.size() != 0)
			clear();
		energy = a.energy;
		for (aiter = a.name.begin(), i = 0; aiter != a.name.end(); i++, aiter++)
		{
			name.push_back(*aiter);
			for (int j = 0; j != MaxDim; j++)
				corr[i][j] = a.corr[i][j];
		}
	}

	Molecule ()
	{
	    name.clear();
		energy.clear();
		energy = "NULL";
		for (int i = 0; i != MaxAtom; i++)
			for (int j = 0; j != MaxDim; j++)
				corr[i][j] = -1024;
	}
	//File operate
	void ReadFromXYZfile(string filename)
	{
		ifstream infile(filename.c_str());
		if (!cout)
		{
			cerr << "Error to open " << filename <<" to get the geometry info"<< endl;
			exit(1);
		}
		int atomNum;
		infile >> atomNum;
		//Clear and initialize the info
		if (name.size() != 0)
			clear();
		string temp;
		getline(infile, temp);
		getline(infile, energy);

		for (int i = 0; i != atomNum; i++)
		{
			string iname;
			infile >> iname;
			name.push_back(iname);
			for (int j = 0; j != MaxDim; j++)
			{
				double icorr;
				infile >> icorr;
				corr[i][j] = icorr;
			}
		}
		infile.close();
	}
	friend void MepMoldenToXYZfiles(string MepFileName, Molecule NowMolecule,int AtomNum)
	{

		ifstream fromfile(MepFileName.c_str());
		if (!fromfile)
		{
			cerr << "Can't open the file: " << MepFileName << endl;
			exit(1);
		}
		string LabelString;
		string JudgeNum;//Judge if is [FORCES]
		string EnergyInFile;
		Molecule TempMolecule;
		while (fromfile >> LabelString&&!fromfile.eof())
		{
			if (LabelString == "[GEOMETRIES]")
			{
				cout << "Find XYZ corrdinates" << endl;
				//Scan molecule and save to some files
				getline(fromfile, JudgeNum);
				fromfile >> JudgeNum;
				int CountNum = 0;
				while (JudgeNum != "[FORCES]")
				{
					CountNum += 1;
					cout << "Begin to scan No." << CountNum << " molecule corrdinate" << endl;
					fromfile >> EnergyInFile;
					NowMolecule.clear();
					NowMolecule.energy = EnergyInFile;
					for (int i = 0; i != AtomNum; i++)
					{
						string iname;
						fromfile >> iname;
						NowMolecule.name.push_back(iname);
						for (int j = 0; j != MaxDim; j++)
						{
							double icorr;
							fromfile >> icorr;
							NowMolecule.corr[i][j] = icorr;
						}
					}
					stringstream stringoperate;
					string tempcountnum;
					stringoperate << CountNum;
					stringoperate >> tempcountnum;
					NowMolecule.ToXYZfile("mepmolecule-" + tempcountnum + ".xyz");
					fromfile >> JudgeNum;
				}
			}
			
		}
		fromfile.close();


	}
	void ToXYZfile(string filename)
	{
		ofstream tofile(filename.c_str(), ios::out);
		if (!tofile)
		{
			cerr << "Error to write " << filename << endl;
			exit(1);
		}
		int atomNum = name.size();
		if (atomNum == 0)
			cout << "Empty molecule and no info is written to " << filename << endl;
		else
		{
			tofile << atomNum << endl;
			tofile << energy<< endl;
			for (int i = 0; i != atomNum; i++)
			{
				tofile << name[i];
				for (int j = 0; j != MaxDim; j++)
				{
					tofile <<"\t"<< corr[i][j];
				}
				tofile << endl;
			}
		}
		tofile.close();
	}
	friend void ToXYZfile(Molecule a, Molecule b, string filename,string energyS="NULL",string CallTimes="0")
	{
		ofstream tofile(filename.c_str(), ios::out);
		if (!tofile)
		{
			cerr << "Error to write " << filename << endl;
			exit(1);
		}
		int atomNum = a.name.size()+b.name.size();
		if (atomNum == 0)
			cout << "Empty molecule and no info is written to " << filename << endl;
		else
		{
			tofile << atomNum << endl;
			tofile <<"  This energy is "<<energyS<<" and Call times are: "<<CallTimes<<endl;
			for (int i = 0; i != a.name.size(); i++)
			{
				tofile << a.name[i];
				for (int j = 0; j != MaxDim; j++)
				{
					tofile << "\t" << a.corr[i][j];
				}
				tofile << endl;
			}
			for (int i = 0; i != b.name.size(); i++)
			{
				tofile << b.name[i];
				for (int j = 0; j != MaxDim; j++)
				{
					tofile << "\t" << b.corr[i][j];
				}
				tofile << endl;
			}

		}
		tofile.close();
	}
	void appGeomTofile(string filename)
	{
		ofstream out(filename.c_str(), ios::app);
		vector<string>::iterator iter;
		int i = 0;
		for (iter = name.begin(), i = 0; iter != name.end(); i++, iter++)
		{
			out << " " << *iter;
			for (int j = 0; j != MaxDim; j++)
			{
				out << " " << setprecision(6) << corr[i][j];
			}
			out << endl;
		}
		out.close();
	}
	void ToNWchemFileHF(string filename,string basis="6-31G")
	{
		ofstream out(filename.c_str(), ios::out);
		out << "# ================================================================" << endl;
		out << "# # NWChem input file made by LiTao for Molecule Recognization" << endl;
		out << "# ================================================================" << endl << endl;
		out << "charge 0" << endl << endl;
		out << "geometry" << endl;
		vector<string>::iterator iter;
		int i = 0;
		for (iter = name.begin(), i = 0; iter != name.end(); i++, iter++)
		{
			out << " " << *iter;
			for (int j = 0; j != MaxDim; j++)
			{
				out << " " << setprecision(6) << corr[i][j];
			}
			out << endl;
		}
		out << "end" << endl << endl;
		out << "basis  \"ao basis\" spherical" << endl;
		out << " * library "<<basis<< endl;
		out << "end" << endl << "scf" << endl << " Singlet" << endl << "end" << endl << endl;
		out << "task SCF" << endl;
		out.close();
	}
	void ToG09FileDFT(string filename,string basis="6-31g")
	{
	        ofstream out((filename+".gjf").c_str(), ios::out);
		out<<"\%nprocshared=7"<<endl;
                out<<"\%mem=8GB"<<endl;
                out<<"\%chk=system.chk"<<endl;
		out<<"#p b3lyp/"<<basis<<endl;
		out<<endl;
		out<<"Title Card Required"<<endl<<endl;
		out<<0<<" "<<1<<endl;
		vector<string>::iterator iter;
		int i = 0;
		for (iter = name.begin(), i = 0; iter != name.end(); i++, iter++)
		{
			out << " " << *iter;
			for (int j = 0; j != MaxDim; j++)
			{
				out << " " << setprecision(6) << corr[i][j];
			}
			out << endl;
		}
		out<<endl;
		out<<endl<<endl<<endl<<endl;
		out.close();
	}
	friend void ToG09FileDFT(Molecule a, Molecule b, string filename,string basis="6-31g")
	{
        ofstream out((filename+".gjf").c_str(), ios::out);
		out<<"\%nprocshared=7"<<endl;
        out<<"\%mem=8GB"<<endl;
        out<<"\%chk=system.chk"<<endl;
		out<<"#p b3lyp/"<<basis<<endl;
		out<<endl;
		out<<"Title Card Required"<<endl<<endl;
		out<<0<<" "<<1<<endl;
        vector<string>::iterator iter;
		int i = 0;
		for (iter = a.name.begin(), i = 0; iter != a.name.end(); i++, iter++)
		{
			out << " " << *iter;
			for (int j = 0; j != MaxDim; j++)
			{
				out << " " << setprecision(6) << a.corr[i][j];
			}
			out << endl;
		}
			for (iter = b.name.begin(), i = 0; iter != b.name.end(); i++, iter++)
		{
			out << " " << *iter;
			for (int j = 0; j != MaxDim; j++)
			{
				out << " " << setprecision(6) << b.corr[i][j];
			}
			out << endl;
		}
		out<<endl;
		out<<endl<<endl<<endl<<endl;
		out.close();
	}
	void ReadFromG09DFT(string filename,int atomNum)
	{
		ifstream infile(filename.c_str());
		if (!cout)
		{
			cerr << "Error to open " << filename <<" to get the geometry info"<< endl;
			exit(1);
		}
		//Clear and initialize the info
		if (name.size() != 0)
			clear();
		string temp;
		getline(infile, temp);
		getline(infile, temp);
		getline(infile, temp);
		getline(infile, temp);
		getline(infile, temp);
		getline(infile, temp);
		getline(infile, temp);
		getline(infile, temp);
		for (int i = 0; i != atomNum; i++)
		{
			string iname;
			infile >> iname;
			name.push_back(iname);
			for (int j = 0; j != MaxDim; j++)
			{
				double icorr;
				infile >> icorr;
				corr[i][j] = icorr;
			}
		}
		infile.close();
	}
	friend void ToNWchemFileHF(Molecule a, Molecule b, string filename,string basis="6-31G")
	{
		ofstream out(filename.c_str(), ios::out);
		out << "# ================================================================" << endl;
		out << "# # NWChem input file made by LiTao for Molecule Recognization" << endl;
		out << "# ================================================================" << endl << endl;
		out << "charge 0" << endl << endl;
		out << "geometry" << endl;
        vector<string>::iterator iter;
		int i = 0;
		for (iter = a.name.begin(), i = 0; iter != a.name.end(); i++, iter++)
		{
			out << " " << *iter;
			for (int j = 0; j != MaxDim; j++)
			{
				out << " " << setprecision(6) << a.corr[i][j];
			}
			out << endl;
		}
			for (iter = b.name.begin(), i = 0; iter != b.name.end(); i++, iter++)
		{
			out << " " << *iter;
			for (int j = 0; j != MaxDim; j++)
			{
				out << " " << setprecision(6) << b.corr[i][j];
			}
			out << endl;
		}
		out << "end" << endl << endl;
		out << "basis  \"ao basis\" spherical" << endl;
		out << " * library " << basis << endl;
		out << "end" << endl << "scf" << endl << " Singlet" << endl << "end" << endl << endl;
		out << "task SCF" << endl;
		out.close();
	}
	friend void ToTinkerFile(Molecule a,Molecule b,string filename,string forceField)
	{
		ofstream tofile(filename.c_str(), ios::out);
		if (!tofile)
		{
			cerr << "Error to save to " << filename << endl;
			exit(1);
		}
		tofile << "Hello Tinker!" << endl;
		tofile.close();
		cout << "Test to Tinker file" << endl;
	}
	//common info of molecule
	double massOfName(string iname)
	{
		double mass;
		if (iname == "H")
			mass = 1.007825;
		else if (iname == "C")
			mass = 12;
		else if (iname == "N")
			mass = 14.003070;
		else if (iname == "O")
			mass = 15.994910;
		else if (iname == "S")
			mass = 31.972070;
		else if (iname == "Cl")
			mass = 34.968850;
		else mass = 0;
		return mass;
	}
	int atomNumber()
	{
		return name.size();
	}
	double moleculeMass()
	{
		double totalMass = 0;
		vector<string>::iterator iter;
		for (iter = name.begin(); iter != name.end(); iter++)
			totalMass += massOfName(*iter);
		return totalMass;
	}
	Eigen::Vector3d MassCenter()
	{
		double totalMass = 0;
		Eigen::Vector3d massCenter;
		double x[3];
		vector<string>::iterator iter;
		for (iter = name.begin(); iter != name.end(); iter++)
			totalMass += massOfName(*iter);
		int i = 0;
		double multi = 0;
		for (iter = name.begin(), i = 0; iter != name.end(); i++, iter++)
			multi += massOfName(*iter)*corr[i][0];
		x[0] = multi / totalMass;
		multi = 0;
		for (iter = name.begin(), i = 0; iter != name.end(); i++, iter++)
			multi += massOfName(*iter)*corr[i][1];
		x[1] = multi / totalMass;
		multi = 0;
		for (iter = name.begin(), i = 0; iter != name.end(); i++, iter++)
			multi += massOfName(*iter)*corr[i][2];
		x[2] = multi / totalMass;
		massCenter << x[0], x[1], x[2];
		return massCenter;
	}
	double DistanceOfMassCenter(Molecule &a)
	{
		Eigen::Vector3d x1 = MassCenter();
		Eigen::Vector3d x2 = a.MassCenter();
		x1 = x1 - x2;
		return sqrt(x1(0)*x1(0) + x1(1)*x1(1) + x1(2)*x1(2));
	}
	//output to console
	void outputMoleculeName()
	{
		vector<string>::iterator iter1, iter2;
		vector<string> newname(name);
		for (iter1 = newname.begin(); iter1 != newname.end(); iter1++)
			for (iter2 = iter1 + 1; iter2 != newname.end(); iter2++)
			{
				if (massOfName(*iter1) > massOfName(*iter2))
				{
					string temp;
					temp = *iter1; *iter1 = *iter2; *iter2 = temp;
				}
			}
		int i = 1;
		iter1 = newname.begin();
		cout << *iter1;
		for (iter1 = newname.begin() + 1; iter1 != newname.end(); iter1++)
		{
			if (*iter1 == *(iter1 - 1))
				++i;
			else
			{
				if (i != 1)
					cout << i;
				cout << *iter1;
				i = 1;
			}
			if (iter1 == newname.end() - 1 && i != 1)
				cout << i;
		}
		cout << endl;
	}
	void output()
	{
		outputMoleculeName();
		cout << energy << endl;
		cout << "Atom" << "\t" << "x" << "\t" << "y" << "\t" << "z" << endl;
		vector<string>::iterator iter;
		int i = 0;
		for (iter = name.begin(), i = 0; iter != name.end(); i++, iter++)
		{
			cout << *iter << "\t";
			for (int j = 0; j != MaxDim; j++)
				cout << corr[i][j] << "\t";
			cout << endl;
		}
		cout <<endl<< "Mass Center" << endl;
		cout << MassCenter()<<endl<<endl;
	}

	//Simple operate on molecule
	void PerformRot(Eigen::Matrix3d rot)
	{
		Eigen::Vector3d singleAtom;
		int atomNum = name.size();
		for (int i = 0; i != atomNum; i++)
		{
			singleAtom << corr[i][0], corr[i][1], corr[i][2];
			singleAtom = rot*singleAtom;
			corr[i][0] = singleAtom(0);
			corr[i][1] = singleAtom(1);
			corr[i][2] = singleAtom(2);
		}
	}
	void PerformTrans(Eigen::Vector3d trans)
	{
		Eigen::Vector3d singleAtom;
		int atomNum = name.size();
		for (int i = 0; i != atomNum; i++)
		{
			singleAtom << corr[i][0], corr[i][1], corr[i][2];
			singleAtom = trans+singleAtom;
			corr[i][0] = singleAtom(0);
			corr[i][1] = singleAtom(1);
			corr[i][2] = singleAtom(2);
		}
	}
	void PerformXTrans(double deltaX)
	{
		int atomNum = name.size();
		for (int i = 0; i != atomNum; i++)
			corr[i][0] +=deltaX;
	}
	void MCtoOrigin()
	{
		Eigen::Vector3d mc;
		mc = MassCenter();
		int atomNum = name.size();
		for (int i = 0; i != atomNum; i++)
		{
			corr[i][0] -= mc(0);
			corr[i][1] -= mc(1);
			corr[i][2] -= mc(2);
		}
	}
	void MCtoVector(Eigen::Vector3d x)
	{
		Eigen::Vector3d mc;
		mc = x - MassCenter();
		int atomNum = name.size();
		for (int i = 0; i != atomNum; i++)
		{
			corr[i][0] += mc(0);
			corr[i][1] += mc(1);
			corr[i][2] += mc(2);
		}
	}
	//Complex operate on molecule, using simple operation
	friend void InitiConfig(Molecule &a, Molecule &b, Eigen::Vector3d bposition)
	{
		//bposition is the Mass Center of Molecule b.
		a.MCtoOrigin();
		b.MCtoVector(bposition);
	}
	Eigen::Matrix3d EulerRot(double a1, double a2, double a3)
	{
		Eigen::Matrix3d m;
		m = Eigen::AngleAxisd(a1, Eigen::Vector3d::UnitX())*Eigen::AngleAxisd(a2, Eigen::Vector3d::UnitY())* Eigen::AngleAxisd(a3, Eigen::Vector3d::UnitZ());
		return m;
	}
void PerformRandomRotEuler(double precision)
	{
		double x[3];
		const int nums = (int)(360 / precision);
		clock_t now = clock();
		srand(now);
		for (int i = 0; i != 3; i++)
			x[i] = (rand()%nums+1)*precision;


		//std::default_random_engine generator(now);
	/*	std::default_random_engine generator(now);
		std::uniform_int_distribution<int> dis(0, nums);
		for(int i=0;i!=3;i++)
		{
		x[i] = dis(generator)*precision;
		}
*/

		Eigen::Matrix3d randomRotation;
		randomRotation = EulerRot(x[0], x[1], x[2]);
		Eigen::Vector3d point = MassCenter();
		PerformTrans(-1 * point);
		PerformRot(randomRotation);
		PerformTrans(point);
	}
friend void PerformRandomRotEuler2(Molecule &a, Molecule &b, double RoTPrecision)
	{
        double x[6];
		const int nums = (int)(360 / RoTPrecision);
		clock_t now = clock();
		srand(now);
		for (int i = 0; i != 6; i++)
			x[i] = (rand() % nums + 1)*RoTPrecision;

		/*std::default_random_engine generator(now);
		std::uniform_int_distribution<int> dis(0, nums);
		for(int i=0;i!=6;i++)
		{
		x[i] = dis(generator)*RoTPrecision;
		}*/

		Eigen::Matrix3d randomRotation;
		randomRotation = a.EulerRot(x[0], x[1], x[2]);
		Eigen::Vector3d point = a.MassCenter();
		a.PerformTrans(-1 * point);
		a.PerformRot(randomRotation);
		a.PerformTrans(point);

		randomRotation = a.EulerRot(x[3], x[4], x[5]);
        point = b.MassCenter();
		b.PerformTrans(-1 * point);
		b.PerformRot(randomRotation);
		b.PerformTrans(point);


	}
	/*
	void PerformRandomRotAxis(double precision)
	{
		const int nums = (int)(360 / precision);
		clock_t now = clock();
		std::default_random_engine generator(now);
		std::uniform_int_distribution<int> dis(0, nums);
		double x= dis(generator)*precision;
		Eigen::Matrix3d randomRot;
		randomRot << 1, 0, 0,
			0, 1, 0,
			0, 0, 1;
		clock_t now2 = clock();
		std::default_random_engine gene(now2);
		std::uniform_int_distribution<int> dist(0, 2);
		char axis = 'x';
		if (dist(gene) == 1)
			axis = 'y';
		else if (dist(gene) == 2)
			axis = 'z';
		if (axis == 'x' || axis == 'X')
			randomRot = EulerRot(x, 0, 0);
		else if (axis == 'y' || axis == 'Y')
			randomRot = EulerRot(0, x, 0);
		else if (axis == 'z' || axis == 'Z')
			randomRot = EulerRot(0, 0, x);
		PerformRot(randomRot);
	}
	friend void PerformRandomRotAxis2(Molecule &a, Molecule &b, double precision)
	{
		clock_t now = clock();
		std::default_random_engine generator(now);
		std::uniform_int_distribution<int> dis(0, 1);
		dis(generator) == 1 ? (a.PerformRandomRotAxis(precision)) : (b.PerformRandomRotAxis(precision));
	}*/

};

class DoubleMolecule
{
private:
	Molecule a, b;
	double energy;
public:
	DoubleMolecule(Molecule ia, Molecule ib, double ienergy)
	{
		a = ia;
		b = ib;
		energy = ienergy;
	}
	DoubleMolecule(DoubleMolecule &i)
	{
		a = i.a;
		b = i.b;
		energy = i.energy;
	}
	DoubleMolecule()
	{
		Molecule ia, ib;
		a = ia;
		b = ib;
		energy = 0;
	}
	double DistanceOfMC()
	{
		return a.DistanceOfMassCenter(b);
	}
	void operator=(DoubleMolecule id)
	{
		a = id.a;
		b = id.b;
		energy = id.energy;
	}
	void Set(Molecule ia, Molecule ib, double ienergy)
	{
		a = ia;
		b = ib;
		energy = ienergy;
	}
	void GetInfo(Molecule &ia, Molecule &ib, double &ienergy)
	{
		ia = a;
		ib = b;
		ienergy = energy;
	}
	void ToXYZ(string filename,int calltimes)
	{
		string energyS;
		stringstream is;
		is << energy;
		is >> energyS;
		string CallTimes;
		stringstream is2;
		is2 << calltimes;
		is2 >> CallTimes;
		ToXYZfile(a,b,filename,energyS,CallTimes);
	}
	void output()
	{
	    cout<<"Energy: "<<energy<<endl;
	    a.output();
	    b.output();
	}

};


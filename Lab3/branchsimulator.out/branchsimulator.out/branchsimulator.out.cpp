// branchsimulator.out.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"



#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <stdlib.h>
#include <cmath>
#include <bitset>

using namespace std;
//Prediction state
#define T 1 // Branch Pre-Taken
#define NT 0 // Branch Pre-Not Taken


class  predictor
{
public:

	struct counter
	{
		int Sbits;//2 bit saturating counter,value should betwen 0 and 3(00-11).

	};

	vector<counter> Spredictor;
	predictor(int LSB)
	{
		Spredictor.resize(pow(2, LSB));
		for (int i = 0; i < pow(2, LSB);i++)
		{
			Spredictor[i].Sbits = 3;
		}
	}


};




int main(int argc, char* argv[]) {



	int LSB;
	double wrongnum=0;
	double total=0;

	ifstream branchbits_params;

	branchbits_params.open(argv[1]);
	while (!branchbits_params.eof())  // read config file
	{
		branchbits_params >> LSB;
	}


	predictor Predictor(LSB);



	int Prediction = 0; // Branch Prediction variable，can be one of T,NT;


	ifstream traces;
	ofstream tracesout;
	string outname;
	outname = string(argv[2]) + ".out";

	traces.open(argv[2]);
	tracesout.open(outname.c_str());

	string line;
	string branchresult;  // Taken or Not Taken
	string xaddr;       // the address from the memory trace store in hex;
	unsigned int addr;  // the address from the memory trace store in unsigned int;   
	unsigned int BranchResult;
	bitset<32> accessaddr; // the address from the memory trace store in the bitset;

	if (traces.is_open() && tracesout.is_open()) {
		while (getline(traces, line)) {   // read mem access file and access Branch
			total++;
			istringstream iss(line);
			if (!(iss  >> xaddr >> branchresult)) { break; }
			stringstream saddr(xaddr);
			saddr >> std::hex >> addr;
			stringstream sBR(branchresult);
			sBR >> std::hex >> BranchResult;
			accessaddr = bitset<32>(addr);

//			cout << xaddr;
//			system("pause");

			int index = addr << (32-LSB) >> (32-LSB);

			if (Predictor.Spredictor[index].Sbits == 0 || Predictor.Spredictor[index].Sbits == 1)
				Prediction = NT;
			else if (Predictor.Spredictor[index].Sbits == 2 || Predictor.Spredictor[index].Sbits == 3)
				Prediction = T;
			else
				system("pause");

			if (Prediction != BranchResult)
				wrongnum++;

			tracesout << Prediction << endl;

			if (BranchResult == 0)
			{
				Predictor.Spredictor[index].Sbits--;
				if (Predictor.Spredictor[index].Sbits == 1)Predictor.Spredictor[index].Sbits--;
				if (Predictor.Spredictor[index].Sbits < 0)
					Predictor.Spredictor[index].Sbits = 0;
			}
			else if (BranchResult == 1)
			{
				Predictor.Spredictor[index].Sbits++;
				if (Predictor.Spredictor[index].Sbits == 2)Predictor.Spredictor[index].Sbits++;
				if (Predictor.Spredictor[index].Sbits > 3)
					Predictor.Spredictor[index].Sbits = 3;
			}
			else
				system("pause");
/*			if (total == 333) {
				cout << total<<" "<<index;
				system("pause");
			}


			if (index == 1009) {
				cout << total<<" "<<index<<BranchResult;
				system("pause");
			}

			*/
		}
		traces.close();
		tracesout.close();
	}
	else cout << "Unable to open trace or traceout file ";


	double missrate;
	missrate = wrongnum / total;




	return 0;
}

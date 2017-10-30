// LAB2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


/*
Cache Simulator
Level one L1 and level two L2 cache parameters are read from file (block size, line per set and set per cache).
The 32 bit address is divided into tag bits (t), set index bits (s) and block offset bits (b)
s = log2(#sets)   b = log2(block size)  t=32-s-b
*/


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
//access state:
#define NA 0 // no action
#define RH 1 // read hit
#define RM 2 // read miss
#define WH 3 // Write hit
#define WM 4 // write miss



void writeL2();








struct config {
	int L1blocksize;
	int L1setsize;
	int L1size;
	int L2blocksize;
	int L2setsize;
	int L2size;
};

// you can define the cache class here, or design your own data structure for L1 and L2 cache
class cache
{
public:

	struct cachetype
	{
		vector <unsigned char> Darry;//Data arrry
		vector <int> tag;
		vector <bool> Dbit;//dirty bit.
		vector <bool> Vbit;//valid bit.
		int evnum=0;//evicted-way number
		int elnum=0;//elected-way number
	};

	vector<cachetype> Dcache;
	cache(int blocksize, int setsize,int size)
	{

		Blocksize = blocksize; Setsize = setsize; Size = size;
		if (setsize == 0)
		{
			Setnumber = 1;
			Setsize = Size * 1024 / Blocksize;
		}
		else
		Setnumber = Size*1024/Setsize/Blocksize;			//set per cache
		Dcache.resize(Setnumber);
		for (int i = 0; i < Setnumber; i++)
		{
			Dcache[i].elnum = 0;
			Dcache[i].evnum = 0;
			Dcache[i].Darry.resize(Blocksize*Setsize);
			Dcache[i].tag.resize(Setsize);
			Dcache[i].Dbit.resize(Setsize);
			Dcache[i].Vbit.resize(Setsize);
		}

	}



	int Blocksize,Setsize,Size;
	int Setnumber;


};


int main(int argc, char* argv[]) {



	config cacheconfig;
	ifstream cache_params;
	string dummyLine;
	cache_params.open(argv[1]);
	while (!cache_params.eof())  // read config file
	{
		cache_params >> dummyLine;
		cache_params >> cacheconfig.L1blocksize;
		cache_params >> cacheconfig.L1setsize;
		cache_params >> cacheconfig.L1size;
		cache_params >> dummyLine;
		cache_params >> cacheconfig.L2blocksize;
		cache_params >> cacheconfig.L2setsize;
		cache_params >> cacheconfig.L2size;
	}



	// Implement by you: 
	// initialize the hirearch cache system with those configs
	// probably you may define a Cache class for L1 and L2, or any data structure you like

	cache L1(cacheconfig.L1blocksize, cacheconfig.L1setsize, cacheconfig.L1size);
	cache L2(cacheconfig.L2blocksize, cacheconfig.L2setsize, cacheconfig.L2size);


	L1.Dcache[0].Darry[0] =0xff;//Dcache[i].Darry[j*Blocksize+offset]-------i=set index num, j=n-way.	0xFF---8bits----Byte----unsighed char.

	int L1AcceState = 0; // L1 access state variable, can be one of NA, RH, RM, WH, WM;
	int L2AcceState = 0; // L2 access state variable, can be one of NA, RH, RM, WH, WM;


 //	The 32 bit address is divided into tag bits(t), set index bits(s) and block offset bits(b)
 //		s = log2(#sets)   b = log2(block size)  t = 32 - s - b

	int s1 = log2(L1.Setnumber), s2 = log2(L2.Setnumber), b1 = log2(L1.Blocksize), b2 = log2(L2.Blocksize);
	int t1 = 32 - s1 - b1, t2 = 32 - s2 - b2;






	ifstream traces;
	ofstream tracesout;
	string outname;
	outname = string(argv[2]) + ".out";

	traces.open(argv[2]);
	tracesout.open(outname.c_str());

	string line;
	string accesstype;  // the Read/Write access type from the memory trace;
	string xaddr;       // the address from the memory trace store in hex;
	unsigned int addr;  // the address from the memory trace store in unsigned int;        
	bitset<32> accessaddr; // the address from the memory trace store in the bitset;


	int bug = 0;

	if (traces.is_open() && tracesout.is_open()) {
		while (getline(traces, line)) {   // read mem access file and access Cache
			bug++;
			istringstream iss(line);
			if (!(iss >> accesstype >> xaddr)) { break; }
			stringstream saddr(xaddr);
			saddr >> std::hex >> addr;
			accessaddr = bitset<32>(addr);

			int trace_tag1,trace_tag2,trace_offset1,trace_offset2;// to be used in tag comparision;come from accessaddr.
			trace_offset1 = addr << (t1 + s1) >> (t1 + s1);
			trace_offset2 = addr << (t2 + s2) >> (t2 + s2);
			trace_tag1 = addr >> (b1 + s1);
			trace_tag2 = addr >> (b2 + s2);

			int L1_index, L2_index;
			if (!(addr >> b1))
				L1_index = 0;
			else
			{
				L1_index = (addr >> b1) % (L1.Setnumber);
			}
			if (!(addr >> b2))
				L2_index = 0;
			else
			{
				L2_index = (addr >> b2) % (L2.Setnumber);
			}

			// access the L1 and L2 Cache according to the trace;
			if (accesstype.compare("R") == 0)

			{
				//Implement by you:
				// read access to the L1 Cache, 
				//  and then L2 (if required), 
				//  update the L1 and L2 access state variable;
				int sign1 = 0,sign2=0;

				for(int i=0;i<L1.Setsize;i++)
					if (L1.Dcache[L1_index].Vbit[i] == 1 && L1.Dcache[L1_index].tag[i] == trace_tag1)	//L1 Read Hit
					{
						L1AcceState = RH;
						L2AcceState = NA;
						sign1 = 1;
					}

/*				if (bug == 487) {
					
					
				system("pause")	;
				
				}
				*/
//************************************************L1 miss*****************************************************************************
			
				
				if (sign1 == 0)				//L1 miss
				{
					for (int i = 0; i<L2.Setsize; i++)
						if (L2.Dcache[L2_index].Vbit[i] == 1 && L2.Dcache[L2_index].tag[i] == trace_tag2)	//L1 Read Hit
						{
							L1AcceState = RM;
							L2AcceState = RH;
							sign2 = 1;
						}

					if (sign2 == 1)				//what should we operate when L2 Hit L1 Miss
					{
						if (L1.Dcache[L1_index].Vbit[L1.Dcache[L1_index].elnum] == 0)
						{
							L1.Dcache[L1_index].Vbit[L1.Dcache[L1_index].elnum] = 1;
							L1.Dcache[L1_index].tag[L1.Dcache[L1_index].elnum] = trace_tag1;

							if (L1.Dcache[L1_index].Vbit[L1.Setsize-1] == 1)
								L1.Dcache[L1_index].elnum = L1.Dcache[L1_index].evnum;
							else
								L1.Dcache[L1_index].elnum += 1;
							
						}

						else 
						{
							L1.Dcache[L1_index].tag[L1.Dcache[L1_index].elnum] = trace_tag1;
							L1.Dcache[L1_index].Dbit[L1.Dcache[L1_index].elnum] = 1;
							L1.Dcache[L1_index].elnum += 1;
							L1.Dcache[L1_index].evnum += 1;
							if (L1.Dcache[L1_index].evnum > L1.Setsize - 1)
							{
								L1.Dcache[L1_index].evnum = 0;
								L1.Dcache[L1_index].elnum = 0;
							}

	/*						if (L1.Dcache[L1_index].Dbit[L1.Dcache[L1_index].elnum] == 1)
							{
								writeL2();
								int sign2 = 0;
								for (int i = 0; i<L2.Setsize; i++)
									if (L2.Dcache[L2_index].Vbit[i] == 1 && L2.Dcache[L2_index].tag[i] == trace_tag2)	//L2 Write Hit
									{

										if (accesstype.compare("W") == 0)
											L2AcceState = WH;
										sign2 = 1;
									}
								if (sign2 == 0)
									if (accesstype.compare("W") == 0)
										L2AcceState = WM;
							}
							*/

						}

					}


					else//L1 Miss L2 Miss
					{
						L1AcceState = RM;
						L2AcceState = RM;
						if (L2.Dcache[L2_index].Vbit[L2.Dcache[L2_index].elnum] == 0)
						{
							L2.Dcache[L2_index].Vbit[L2.Dcache[L2_index].elnum] = 1;
							L2.Dcache[L2_index].tag[L2.Dcache[L2_index].elnum] = trace_tag2;

							if (L2.Dcache[L2_index].Vbit[L2.Setsize - 1] == 1)
								L2.Dcache[L2_index].elnum = L2.Dcache[L2_index].evnum;
							else
								L2.Dcache[L2_index].elnum += 1;

						}

						else
						{
							L2.Dcache[L2_index].tag[L2.Dcache[L2_index].elnum] = trace_tag2;
							L2.Dcache[L2_index].Dbit[L2.Dcache[L2_index].elnum] = 1;
							L2.Dcache[L2_index].elnum += 1;
							L2.Dcache[L2_index].evnum += 1;
							if (L2.Dcache[L2_index].evnum > L2.Setsize - 1)
							{
								L2.Dcache[L2_index].evnum = 0;
								L2.Dcache[L2_index].elnum = 0;
							}
						}





						if (L1.Dcache[L1_index].Vbit[L1.Dcache[L1_index].elnum] == 0)
						{
							L1.Dcache[L1_index].Vbit[L1.Dcache[L1_index].elnum] = 1;
							L1.Dcache[L1_index].tag[L1.Dcache[L1_index].elnum] = trace_tag1;

							if (L1.Dcache[L1_index].Vbit[L1.Setsize - 1] == 1)
								L1.Dcache[L1_index].elnum = L1.Dcache[L1_index].evnum;
							else
								L1.Dcache[L1_index].elnum += 1;

						}

						else
						{
							L1.Dcache[L1_index].tag[L1.Dcache[L1_index].elnum] = trace_tag1;
							L1.Dcache[L1_index].Dbit[L1.Dcache[L1_index].elnum] = 1;
							L1.Dcache[L1_index].elnum += 1;
							L1.Dcache[L1_index].evnum += 1;
							if (L1.Dcache[L1_index].evnum > L1.Setsize - 1)
							{
								L1.Dcache[L1_index].evnum = 0;
								L1.Dcache[L1_index].elnum = 0;
							}
						}

					}

					
				}



			}




/********************************************************Write**********************************************************/

			else
			{
				//Implement by you:
				// write access to the L1 Cache, 
				//and then L2 (if required), 
				//update the L1 and L2 access state variable;		addr	3214427468	unsigned int
				int sign1 = 0,sign2=0;

				for (int i = 0; i<L1.Setsize; i++)
					if (L1.Dcache[L1_index].Vbit[i] == 1 && L1.Dcache[L1_index].tag[i] == trace_tag1)	//L1 Write Hit
					{
						L1AcceState = WH;
						L2AcceState = NA;
						sign1 = 1;

						if (L1.Dcache[L1_index].Dbit[i] == 0)
							L1.Dcache[L1_index].Dbit[i] == 1;
					/*	else
						{
							int sign2 = 0;
							for (int i = 0; i < L2.Setsize; i++)
								if (L2.Dcache[L2_index].Vbit[i] == 1 && L2.Dcache[L2_index].tag[i] == trace_tag2)	//L2 Write Hit
								{

									if (accesstype.compare("W") == 0)
										L2AcceState = WH;
									sign2 = 1;
								}
							if (sign2 == 0)
								if (accesstype.compare("W") == 0)
									L2AcceState = WM;
						}*/
					}
		

				if (sign1 == 0)
				{
					L1AcceState = WM;
	
					for (int i = 0; i<L2.Setsize; i++)
						if (L2.Dcache[L2_index].Vbit[i] == 1 && L2.Dcache[L2_index].tag[i] == trace_tag2)	//L2 Write Hit
						{

							if (accesstype.compare("W") == 0)
								L2AcceState = WH;
							sign2 = 1;
						}
					if (sign2 == 0)
						if (accesstype.compare("W") == 0)
							L2AcceState = WM;
				}



			}



			tracesout << L1AcceState << " " << L2AcceState << endl;  // Output hit/miss results for L1 and L2 to the output file;


		}
		traces.close();
		tracesout.close();
	}
	else cout << "Unable to open trace or traceout file ";




	







	return 0;
}



	void writeL2()
	{

		
	}










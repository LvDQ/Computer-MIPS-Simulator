// ConsoleApplication1.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"


#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>
using namespace std;
#define ADDU 1
#define SUBU 3
#define AND 4
#define OR  5
#define NOR 7
#define MemSize 65536 // memory size, in reality, the memory size should be 2^32, but for this lab, for the space resaon, we keep it as this large number, but the memory is still 32-bit addressable.



class RF
{
public:
	bitset<32> ReadData1, ReadData2;
	RF()
	{
		Registers.resize(32);
		Registers[0] = bitset<32>(0);
	}

	void ReadWrite(bitset<5> RdReg1, bitset<5> RdReg2, bitset<5> WrtReg, bitset<32> WrtData, bitset<1> WrtEnable)
	{
		if (WrtEnable==0)
		{
			ReadData1=Registers[RdReg1.to_ulong()];
			ReadData2=Registers[RdReg2.to_ulong()];
		}


		else 
		{
			Registers[WrtReg.to_ulong()] = WrtData;

		}
		// implement the funciton by you.                
	}

	void OutputRF()
	{
		ofstream rfout;
		rfout.open("RFresult.txt", std::ios_base::app);
		if (rfout.is_open())
		{
			rfout << "A state of RF:" << endl;
			for (int j = 0; j<32; j++)
			{
				rfout << Registers[j] << endl;
			}

		}
		else cout << "Unable to open file";
		rfout.close();

	}
private:
	vector<bitset<32> >Registers;

};

class ALU
{
public:
	bitset<32> ALUresult;
	bitset<32> ALUOperation(bitset<3> ALUOP, bitset<32> oprand1, bitset<32> oprand2)
	{
		bitset<32> sum; bitset<32> carry;
/*		if (ALUOP == ADDU) { do { sum = oprand1 ^ oprand2; carry = (oprand1 & oprand2) << 1; oprand1 = sum; oprand2 = carry; } while (oprand2 != 0); }
		if (ALUOP == SUBU) { oprand2 = ~oprand2; carry = 1; while (carry != 0) { oprand2 = carry^oprand2; if ((carry & oprand2) == carry)carry = carry << 1; else carry = 0; }do { sum = oprand1 ^ oprand2; carry = (oprand1 & oprand2) << 1; oprand1 = sum; oprand2 = carry; } while (oprand2 != 0);
		}
		*/
		if (ALUOP == ADDU) { ALUresult = oprand1.to_ulong() + oprand2.to_ulong(); }
		if (ALUOP == SUBU) { ALUresult = oprand1.to_ulong() - oprand2.to_ulong();  }
		if (ALUOP == AND)ALUresult = oprand1&oprand2;
		if (ALUOP == OR)ALUresult = oprand1|oprand2;
		if (ALUOP == NOR)  ALUresult = ~(oprand1|oprand2);
		// implement the ALU operations by you. 
		return ALUresult;
	}
};

class INSMem
{
public:
	bitset<32> Instruction;
	INSMem()
	{
		IMem.resize(MemSize);
		ifstream imem;
		string line;
		int i = 0;
		imem.open("imem.txt");
		if (imem.is_open())
		{
			while (getline(imem, line))
			{
				IMem[i] = bitset<8>(line);
				i++;
			}

		}
		else cout << "Unable to open file";
		imem.close();

	}

	bitset<32> ReadMemory(bitset<32> ReadAddress)
	{
		int a = 31;
		for (int i = 0; i < 4; i++)
		{
		
			for (int j = 0; j < 8; j++)
			{
			
				Instruction[a--] = IMem[ReadAddress.to_ulong() + i][7-j];

			}
		}
		// implement by you. (Read the byte at the ReadAddress and the following three byte).
		return Instruction;
	}

private:
	vector<bitset<8> > IMem;

};

class DataMem
{
public:
	bitset<32> readdata;
	DataMem()
	{
		DMem.resize(MemSize);
		ifstream dmem;
		string line;
		int i = 0;
		dmem.open("dmem.txt");
		if (dmem.is_open())
		{
			while (getline(dmem, line))
			{
				DMem[i] = bitset<8>(line);
				i++;
			}
		}
		else cout << "Unable to open file";
		dmem.close();

	}
	bitset<32> MemoryAccess(bitset<32> Address, bitset<32> WriteData, bitset<1> readmem, bitset<1> writemem)
	{
		bitset<8> line;
		if (readmem == 1) {
			int a = 31;
			for (int i = 0; i < 4; i++)
			{

				for (int j = 0; j < 8; j++)
				{
					readdata[a--] = DMem[Address.to_ulong() + i][7-j];

				}
			}
		}
		if (writemem == 1)
		{
			for (int i = 0, j = 31; i < 4; i++)
			{
				for (int k = 0; k < 8; j--, k++)
					line[7-k] = WriteData[j];
				DMem[Address.to_ulong() + i] = line;
			}
		}
		// implement by you.
		return readdata;
	}

	void OutputDataMem()
	{
		ofstream dmemout;
		dmemout.open("dmemresult.txt");
		if (dmemout.is_open())
		{
			for (int j = 0; j< 1000; j++)
			{
				dmemout << DMem[j] << endl;
			}

		}
		else cout << "Unable to open file";
		dmemout.close();

	}

private:
	vector<bitset<8> > DMem;

};



int main()
{
	
	RF myRF;
	ALU myALU;
	INSMem myInsMem;
	DataMem myDataMem;
	int PC = 0;

	while (1)
	{
		// Fetch

		myInsMem.ReadMemory(PC);
		PC += 4;//PC GO

/***************************************************************************************************************************************************************/
		// If current insturciton is "11111111111111111111111111111111", then break;
		if (myInsMem.Instruction == 0xFFFFFFFF)
			break;


/***************************************************************************************************************************************************************/
		// decode(Read RF)
	


		bitset<5> RdReg1; bitset<5> RdReg2; bitset<5> WrtReg; bitset<32> WrtData; bitset<1> WrtEnable; bitset<3> Func;
	
		if ((myInsMem.Instruction >> 26) == 0)				//R-Type
		{
			for (int i = 0, j = 0; j < 3; i++, j++)
			{
				Func[j] = myInsMem.Instruction[i];
			}
			WrtEnable = 1;
			for (int i = 21, j = 0; j < 5; i++, j++)
			{
				 RdReg1[j]= myInsMem.Instruction[i];
			}

			for (int i = 16, j = 0; j < 5; i++, j++)
			{
				RdReg2[j] = myInsMem.Instruction[i];
			}

			for (int i = 11, j = 0; j < 5; i++, j++)
			{
				WrtReg[j] = myInsMem.Instruction[i];
			}

			
			myRF.ReadWrite(RdReg1, RdReg2, WrtReg, WrtData, 0);


				WrtData=myALU.ALUOperation(Func, myRF.ReadData1, myRF.ReadData2);
				WrtEnable = 1;
	
				myRF.ReadWrite(RdReg1, RdReg2, WrtReg, WrtData, WrtEnable);
			
		}




		/***************************************************************************************************************************************************************/

		else if ((myInsMem.Instruction >> 26) == 2 || (myInsMem.Instruction >> 26) == 3)	//J-Type	//j address		//PC={PC+4[31:28],address,00}
		{
			WrtEnable = 0;
			PC = (PC&0xF0000000)|(myInsMem.Instruction.to_ulong()<<6>>4);

		}




/***************************************************************************************************************************************************************/



		else              //I-Type
		{	
			for (int i = 21, j = 0; j < 5; i++, j++)
			{
				RdReg1[j] = myInsMem.Instruction[i];
			}

			for (int i = 16, j = 0; j < 5; i++, j++)
			{
				RdReg2[j] = myInsMem.Instruction[i];
			}

			for (int i = 16, j = 0; j < 5; i++, j++)// WrtData Address= 20:16
			{
				WrtReg[j] = myInsMem.Instruction[i];
			}

			bitset <32> imm = myInsMem.Instruction << 16 >> 16;//Extend Imm value



			if ((myInsMem.Instruction >> 26) == 0x09)	//addi rs, rt,imm    //R[rt]=R[rs]+{SignExtend,imm}
			{
				myRF.ReadWrite(0,0,WrtReg,myALU.ALUOperation(ADDU, myRF.ReadData1, imm),1);
			}

			if ((myInsMem.Instruction >> 26) == 0x04)	//beq if(R[rs]==R[rt]) PC = PC + 4 + BranchAddr(imm)
			{
				WrtEnable = 0;
				myRF.ReadWrite(RdReg1, RdReg2, WrtReg, WrtData, WrtEnable);
				if (myRF.ReadData1 == myRF.ReadData2)
					PC = PC + (imm.to_ulong()<<2);	//befor this, PC has already excuted  PC=PC+4 
			}

			if ((myInsMem.Instruction >> 26) == 0x23)// lw rs, rt, imm	//R[rt]=Mem[{SignExtendimm}+R[rs]]
			{
				WrtEnable = 0;
				myRF.ReadWrite(RdReg1, RdReg2, WrtReg, WrtData, WrtEnable);
				WrtEnable = 1;
				WrtData=myDataMem.MemoryAccess(myALU.ALUOperation(ADDU, myRF.ReadData1, imm), 0, 1, 0);
				myRF.ReadWrite(RdReg1, RdReg2, WrtReg, WrtData, WrtEnable);
			}

			if ((myInsMem.Instruction >> 26) == 0x2B)	//sw Mem[R[rs]+SignExtImm] = R[rt] 
			{
				WrtEnable = 0;
				myRF.ReadWrite(RdReg1, RdReg2, WrtReg, WrtData, WrtEnable);
				
				myDataMem.MemoryAccess(myALU.ALUOperation(ADDU, myRF.ReadData1, imm),myRF.ReadData2,0,1);
	
			}
		}


		myRF.ReadWrite(RdReg1, RdReg2, WrtReg,  WrtData, WrtEnable);



/***************************************************************************************************************************************************************/
		// Execute






		// Read/Write Mem

		// Write back to RF

		myRF.OutputRF(); // dump RF;    



	}
	myDataMem.OutputDataMem(); // dump data mem

	return 0;

}
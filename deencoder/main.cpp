#include <iostream>
#include <fstream>
#include <bitset>
#include <sstream>
#include <math.h>

using namespace std;

int changeIndex(&index, int* tempNLS)
{
	'''After having enough bits for a number, changes the index value
	and returns 1 if a back reference was complete, 2 if an encoded raw was complete,
	and 0 otherwise to tell the main which process to continue'''
	if(index == 0)
	{
		index = 1;
		return 1;
	}
	if(index == 1)
	{
		if(tempNLSC[index] == 0)
			index = 2;
		else
			index = 0;
		return 0;
	}
	if(index == 2)
	{
		index = 1;
		return 2;
	}
}

int main(int argc, char** argv)
{
	ifstream inFile;
	char c;
	int index = 1, wIndex = 0;
	int bitLength = 0; //Index of NLSC
	int currentType = 0; //Checks whether we are looking for the bits of length, offset, string length, or char
	bitset<16> bitC, currentBits;
	int NLS3[3], tempNLS[3];

	if(argc != 2)
	{
		cerr << "Incorrect number of inputs\n";
		exit(0);
	}
	inFile.open(argv[1]);
	if(!inFile)
	{
		cerr << "Unable to open file datafile.txt";
        	exit(1);   // call system to stop
	}

	inFile.get(c);
	currentC = bitset<16>(c);
	NLS[0] = (currentC.to_ulong() >> 4);
	NLS[1] = (currentC.to_ulong() << 12 >> 12);

	inFile.get(c);
	currentC = bitset<16>(c);
	NLS[2] = (currentC.to_ulong() >> 4);
	char window[pow(2,NLSC[0])];
	currentBits |= (currentC << 12 >> 16-NLSC[1]);
	bitLength = 4;

	while(inFile.get(c))
	{
		currentC = bitset<16>(c);
		if(NLSC[index]-bitLength > 8)
		{
			currentBits |= (currentC << NLSC[index]-bitLength-8);
			bitLength += 8;
		}
		else
		{
			currentBits |= (currentC >> 8 - (NLS[index] - bitLength));
			int leftover = NLS[index] - bitLength;
			tempNLS[index] = currentBits.to_ulong();
			currentBits = bitset<16>(0);
			value = changeIndex(index, tempNLS);
			if (value == 1)
			{
				for(int i = 0; i < tempNLSC[1]; i++)
				{
					window[wIndex+i] = window[wIndex+i-tempNLS[0]];
					cout << window[wIndex+1-tempNLS[0]];
				}
				tempNLS[0] = 0;
				tempNLS[1] = 0;
			}
			else if (value == 2)
			{
				for(int i = 0; i < tempNLS[2]; i++)
				{
					currentBits |= (currentC << 16 - leftover >> 8);
					if(!inFile.get(c))
					{
						inFile.close();
						return 0;
					}
					currentC = bitset<16>(c);
					currentBits |= (currentC >> (8 - leftover));
					window[wIndex+i] = char(currentBits.to_ulong());
					currentBits = bitset<16>(0);
				}
				tempNLS[2] = 0;
				tempNLS[1] = 0;
			}
			currentBits |= (currentC << 16 - leftover >> 16-NLS[index]);
			bitLength = leftover;
		}
	}
	
	inFile.close();

	return 0;
}
#include <iostream>
#include <fstream>
#include <bitset>
#include <sstream>
#include <math.h>

using namespace std;

int changeIndex(int &index, int* tempNLS)
{
	//After having enough bits for a number, changes the index value
	//and returns 1 if a back reference was complete, 2 if an encoded raw was complete,
	//and 0 otherwise to tell the main which process to continue
	if(index == 0)
	{
		index = 1;
		return 1;
	}
	if(index == 1)
	{
		if(tempNLS[index] == 0)
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
	int index = 1, wIndex = 0, leftover = 0;
	int bitLength = 0; //Index of NLS
	int currentType = 0; //Checks whether we are looking for the bits of length, offset, string length, or char
	bitset<16> currentC, currentBits;
	int NLS[3], tempNLS[3];

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

	for (int i = 0; i < 3; i++)
	{
		inFile.get(c);
		currentC = bitset<16>(c);
		NLS[i] = currentC.to_ulong();
	}
	char window[(int)pow(2,NLS[0])];
	bitLength = 0;
	

	while(inFile.get(c))
	{
		currentC = bitset<16>(c);
		int leftover = 8;
		if(NLS[index]-bitLength > 8)
		{
			currentBits |= (currentC << NLS[index]-bitLength-8);
			bitLength += 8;
		}
		else
		{
			do
			{
				currentBits |= (currentC << 16 - leftover >> 16 - (NLS[index] - bitLength));
				leftover -= NLS[index] - bitLength;
				tempNLS[index] = currentBits.to_ulong();
				currentBits = bitset<16>(0);
				int value = changeIndex(index, tempNLS);
				if (value == 1)
				{
					for(int i = 0; i <= tempNLS[1]; i++)
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
			} while(leftover >= NLS[index]);
			currentBits |= (currentC << 16 - leftover >> 16-NLS[index]);
			bitLength = leftover;
		}
	}
	
	inFile.close();

	return 0;
}

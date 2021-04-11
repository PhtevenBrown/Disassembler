#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// declare variables
// Registers and main memory
int mainm[4096][16] = {0};
int ac[16] = {0};
int ir[16] = {0};
int mbr[16] = {0};
int pc[12] = {0};
int mar[12] = {0};
char inREG[16] = {0};
int outREG[16] = {0};

// Constants
const int OPERAND_SIZE = 12;
const int INSTRUCTION_SIZE = 16;
const int MAIN_MEM_SIZE = 4096;

// Global variables used throughout program
int binary[16] = {0};
int decimal = 0;
int twoComp = 0;
int bitLength = 11;
char instructions[] = "/tmp/instructions.txt";	// /tmp/ is being used because it means we can run the code on any machine

// declare functions
void start(int pc[]);
void getData();
void getAddress();
void decodeOp();
int loadX();
void storeX();
void subtX();
void addX();
void input();
void output();
void halt();
void skipcond();
void jumpX();
void flipAC();
void jumpZero();
void shiftLeft();
void shiftRight();

// Aux functions
int decToBin(int inputD);
int binToDec(int mar[]);
void displayMemory();
void printAssembly(char* code, int operand);
void consoleInput();
void fileInput();

void main(int argc, char *argv[])
{
	if (argc !=2)
	{
		printf("No argument entered.\n");
		exit(1);
	}

	if (!strcmp(argv[1], "-d"))
	{
		printf("Default input loaded...\n\n");
		strcpy(instructions,"/tmp/default.txt");
		fileInput();
	}
	
	else if (!strcmp(argv[1], "-c"))
	{
		printf("Console input loaded...\n\n");
		consoleInput();
	}
	
	else if (!strcmp(argv[1], "-f"))
	{
		printf("File input loaded...\n\n");
		fileInput();
	}
	
	else
	{
		printf("Invalid argument entered.\n");
		exit(1);
	}
	
	displayMemory();
	printf("\n");
	printf("OPCODE|OPERAND     \n");
	// Fetch/execute cycle
	while(1)
	{
		start(pc);
	}
}

void start(int pc[])
{
	// Store PC in MAR
	for (int i = 0; i < OPERAND_SIZE; i++)
	{
		mar[i] = pc[i];
	}
	
	// Update IR
	for (int i = 0; i < INSTRUCTION_SIZE; i++)
	{
		ir[i] = mainm[binToDec(mar)][i];
	}
	
	// Update PC
	int i;
	for(i = OPERAND_SIZE-1; i >= 0; i--)
	{
		if(pc[i] == 1)
		{
			pc[i] = 0;
		}
		
		else
		{
			break;
		}
	}
	pc[i] = 1;
    
	decodeOp();
	
	for (int i = 0; i < INSTRUCTION_SIZE; i++)
	{
		mar[i] = ir[i];
	}
}

void getData()
{
	// Setting MBR to contents of current place in memory
	for (int i = 0; i < INSTRUCTION_SIZE; i++)
	{
		mbr[i] = mainm[binToDec(mar)][i];
	}
}

void getAddress()
{
	// gets the address we want to go to (last 12 bits of mbr)
	for (int i = 4; i < INSTRUCTION_SIZE; i++)
	{
		mar[i-4] = mbr[i];
	}
}

void decodeOp()
{
	// Decoding the opcode of instruction
	
	// Opcode = first four bits of current instruction
	int opcode[4] = {0};
	for (int i = 0; i < 4; i++)
	{
		opcode[i] = mainm[binToDec(mar)][i];
	}
	
	// Convert opcode to decimal and chooses which function to do accordingly
	char code[3];
	bitLength = 3;
	switch(binToDec(opcode))
	{
	case 0: //0000
		strcpy(code, "HLT");
		printAssembly(code, 0);
		halt();
		break;
		
	case 1: //0001
		strcpy(code, "LDA");
		printAssembly(code, 1);
		loadX();
		break;
		
	case 2: //0010
		strcpy(code, "STA");
		printAssembly(code, 1);
		storeX();
		break;
		
	case 3: //0011
		strcpy(code, "SUB");
		printAssembly(code, 1);
		subtX();
		break;
		
	case 4: //0100
		strcpy(code, "ADD");
		printAssembly(code, 1);
		addX();
		break;
		
	case 5: //0101
		strcpy(code, "PUT");
		printAssembly(code, 0);
		input();
		break;
		
	case 6: //0110
		strcpy(code, "OUT");
		printAssembly(code, 0);
		output();
		break;
		
	case 7: //0111
		strcpy(code, "SKP");
		printAssembly(code, 1);
		skipcond();
		break;
		
	case 8: //1000
		strcpy(code, "JMP");
		printAssembly(code, 1);
		jumpX();
		break;
		
	case 9: //1001
		strcpy(code, "JZE");
		printAssembly(code, 1);
		jumpZero();
		break;
		
	case 10: //1010
		strcpy(code, "SHL");
		printAssembly(code, 0);
		shiftLeft();
		break;
	case 11: //1011
		strcpy(code, "SHR");
		printAssembly(code, 0);
		shiftRight();
		break;
	}
}

int loadX()
{
	// Load the contents of the specified address into AC

	getData();
	getAddress();
	for (int i = 0; i < INSTRUCTION_SIZE; i++)
	{
		ac[i] = mainm[binToDec(mar)][i];
	}
	return 0;
}

void storeX()
{
	// Store the contents of AC into specified address
	
	getData();
	getAddress();
	
	for (int i = 0; i < INSTRUCTION_SIZE; i++)
	{
		mainm[binToDec(mar)][i] = ac[i];
	}
}

void subtX()
{
	//Subtract the contents of the address to AC and store the result back in AC

	getData();
	getAddress();
	
	// Convert AC to decimal
	bitLength = INSTRUCTION_SIZE-1;
	int a = binToDec(ac);
	
	// get the contents of the address we want in main memory
	int contents[16] = {0};
	for (int i = 0; i < INSTRUCTION_SIZE; i++)
	{
		contents[i] = mainm[binToDec(mar)][i];
	}

	// Convert contents of the address we want into decimal
	bitLength = INSTRUCTION_SIZE-1;
	int b = binToDec(contents);
	
	// AC minus contents of address X
	int temp = a-b;

	// Converts above answer to binary
	if (temp < 0)
	{
		twoComp = 1;
	}
	decToBin(temp);

	// AC = above answer
	for (int i = 0; i < INSTRUCTION_SIZE; i++)
	{
		ac[i] = binary[i];
	}
}

void addX()
{
	// Add the contents of the address to AC and store the result back in AC

	getData();
	getAddress();
	
	// Convert AC to decimal
	bitLength = INSTRUCTION_SIZE-1;
	int a = binToDec(ac);
	
	// get the contents of the address we want in main memory
	int contents[16] = {0};
	for (int i = 0; i < INSTRUCTION_SIZE; i++)
	{
		contents[i] = mainm[binToDec(mar)][i];
	}

	// Convert contents of the address we want into decimal
	bitLength = INSTRUCTION_SIZE-1;
	int b = binToDec(contents);
	
	// AC + contents of address X
	int temp = a+b;

	// Converts above answer to binary
	twoComp = 0;
	decToBin(temp);

	// AC = above answer
	for (int i = 0; i < INSTRUCTION_SIZE; i++)
	{
		ac[i] = binary[i];
	}
}

void input()
{
	// Input value from user console into AC
	
	int i = 0;
	inREG[0] = '\0';
	
	// Getting the value from user
	while(strlen(inREG) != INSTRUCTION_SIZE)
	{
		printf("\nInput a value (16 bits) into the AC:\n");
		scanf("%s",inREG);
	}
	
	// add the contents of user input into AC
	for (int i = 0; i < INSTRUCTION_SIZE; i++)
	{
		ac[i] = inREG[i] - '0';
	}
	printf("\n");
}

void output()
{
	// Print AC to the display
	
	// Put AC into outREG
	for (int i = 0; i < INSTRUCTION_SIZE; i++)
	{
		outREG[i] = ac[i];
	}
	
	// Print contents of outREG
	printf("[AC: ");
	for (int i = 0; i < INSTRUCTION_SIZE; i++)
	{
		printf("%d", outREG[i]);
	}
	printf("]\n");
}

void halt()
{
	// Quit the program
	
	// Display memory then quit
	printf("\n");
	displayMemory();
	printf("\nPROGRAM FINISHED.\n");
	exit(0);
}

void skipcond()
{
	// Skip if condition is met
	
	getData();
	
	int operand[12] = {0};
	for (int i = 4; i <= INSTRUCTION_SIZE-1; i++)
	{
		operand[i-4] = mbr[i];
	}
	
	// Condition: if AC < operand)
	bitLength = INSTRUCTION_SIZE-1;
	if(binToDec(ac) < binToDec(operand))
	{
		decToBin(binToDec(pc) + 1);
		for (int i = 0; i < OPERAND_SIZE; i++)
		{
			pc[i] = binary[i+4];
		}
	}
}


void jumpX()
{
	// Load the specified value into PC
	
	getData();
	getAddress();
	for(int i = 4; i <= INSTRUCTION_SIZE-1; i++){
		pc[i-4] = mbr[i];
		
	}
}


// Our three instructions

void jumpZero()
{
	// Jump to the specified address if AC = 0
	
	getData();
	getAddress();
	
	bitLength = INSTRUCTION_SIZE-1;
	if(binToDec(ac) == 0)
	{
		for(int i = 4; i <= INSTRUCTION_SIZE-1; i++){
			pc[i-4] = mbr[i];
		}
	}
}

void shiftLeft()
{
	// Shift left logical on AC
	
	int i;
	for (i = 0; i < INSTRUCTION_SIZE-1; i++)
	{
		ac[i] = ac[i+1];
	}
	ac[i] = 0;
}

void shiftRight()
{
	// Shift right logical on AC
	int i;
	for (i = INSTRUCTION_SIZE-1; i > 0; i--)
	{
		ac[i] = ac[i-1];
	}
	ac[i] = 0;
}


// Auxiliary/Other C functions
int decToBin(int inputD)
{
	//Decimal to binary converter
	
	int i;
	
	// If number requires two's complement, do this
	if(twoComp == 0)
	{
		for (i = INSTRUCTION_SIZE-1; i >=0; i--)
		{
		if (inputD == 0){
			for (i = i; i >=0; i--)
			{
				binary[i] = 0;
			}
			break;
		}
		binary[i] = inputD%2;
		inputD = inputD/2;
		}
		return *binary;
	}
	
	// if number doesn't require two's complement, do this
	else
	{
		int neg = 0;
		if(inputD < 0){
			neg = 1;
			inputD = 0 - inputD;	
		}
		//convert positive
		for (i = INSTRUCTION_SIZE-1; i >=1; i--)
		{
			if (inputD == 0)
			{
				for (i = i; i >=0; i--)
				{
					binary[i] = 0;
				}
				break;
			}
			binary[i] = inputD%2;
			inputD = inputD/2;
		}
		
		//check if negative
		if(neg == 1)
		{
			//flip the bits
			for(int i = 0; i <=INSTRUCTION_SIZE-1; i++)
			{
				if(binary[i] == 0)
				{
					binary[i] = 1;
				}
				else if(binary[i] == 1)
				{
					binary[i] = 0;
				}
			}
			
			//add one to binary
			int i;
			for(i = INSTRUCTION_SIZE-1; i >= 0; i--)
			{
				if(binary[i] == 1)
				{
					binary[i] = 0;
				}
				else
				{
					break;
				}
			}
			binary[i] = 1;
		}
		return *binary;
	}
	
	// Reset two's complement to 0
	twoComp = 0;
}

int binToDec(int mar[])
{
	// Binary to decimal converter
	
	int i;
	int j = 1;
	int decimal = 0;

	for (i = bitLength; i >=0; i--)
	{
		decimal = decimal + (mar[i] * j);
		j = j * 2;
	}
	bitLength = OPERAND_SIZE-1;
	return decimal;
}

void displayMemory()
{
	//Display main memory
	
	int display = 0;
	inREG[0] = '\0';
	
	// User decides how many lines they'd like to see
	printf("How many lines of memory would you like to view (0 - 4095)?\n");
	scanf("%s",inREG);
	
	for (int i = 0; i < strlen(inREG); i++)
	{
		display = display*10 + (inREG[i] - '0');
	}
	
	// Prints out specified nubmer of lines
	printf("MAIN MEMORY:\n¯¯¯¯¯¯¯¯¯¯¯\n");
	for (int i = 0; i < display; i++)
	{
		printf("%d:	", i);
		for (int j = 0; j < INSTRUCTION_SIZE; j++)
		{
			printf("%d", mainm[i][j]);
		}
		printf("\n");
	}
}

void printAssembly(char* code, int operand)
{
	// Displays the operand and opcode (if applicable) for each instruction as they're executed
	
	printf("%s   |", code);
	if(operand == 1)
	{
		for (int i = 4; i < INSTRUCTION_SIZE; i++)
		{
			printf("%d", ir[i]);
		}
	}
	else
	{
		printf(" NO OPERAND");
	}
	printf("\n");
}


void consoleInput()
{
	// User can enter instructions via the console
	
	inREG[0] = '\0';
	
	// loop until the user types stop or max size of memory is reached
	for (int i = 0; i < MAIN_MEM_SIZE; i++)
	{
		printf("Enter a valid instruction (16 bits):\n");
		scanf("%s",inREG);
		if (strcmp(inREG, "stop") == 0)
		{
			printf("\n");
			break;
		}
		
		// Add the instruction the user typed into main memory
		else
		{
			for (int j = 0; j < INSTRUCTION_SIZE; j++)
			{
				mainm[i][j] = inREG[j] - '0';
			}
		}
	}
}

void fileInput()
{
	// Load instructions via a text file

	// Load text file with instructions
	FILE *fp;
	fp = fopen(instructions, "r");
	char textFile[MAIN_MEM_SIZE+1];
	
	// textFile now contains the contents of instructions.txt
	fread(textFile, MAIN_MEM_SIZE+1, 1, (FILE*)fp);
	fclose(fp);
	
	int intFile[MAIN_MEM_SIZE];
	int j = 0;
	
	// Convert the instructions from char to int
	for (int i = 0; i < MAIN_MEM_SIZE; i++)
	{
		if (textFile[i] == '1' || textFile[i] == '0')
		{
			intFile[j] = textFile[i] - '0';
			j++;
		}
		
		// Quit converting instructions when null character is found
		if (textFile[i] == '\0')
		{
			break;
		}
	}

	// Load intFile into memory
	int k = 0;
	for (int i = 0; i < MAIN_MEM_SIZE; i++)
	{
		
		for (j = 0; j < INSTRUCTION_SIZE; j++)
		{
			if (k < MAIN_MEM_SIZE)
			{
				mainm[i][j] = intFile[k];
				k++;
			}
		}
	}
}

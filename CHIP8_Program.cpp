#include "CHIP8_Program.h"

int main(int argc, char* argv[])
{
	//std::cout << "Hello World!\nPrinting 0xF1 = " << std::hex << 0xF1 << std::endl;
	// By pushing std::hex to the string buffer, it'll know to interpret any integer literals as Hex (albeit lowercase).

	// Insert Interface Display Code here, using SDL or something

	// Push the Keyboard Key Sprites into the Memory at the correct location.

	// Before the Program can even begin we need to setup things, namely:
	// 1. load the Fonts into memory
	// 2. read a file containing a binary CHIP-8 program
	// 3. parse the whole thing into Memory too

	// Here the fonts are looped into memory at the location defined by a constant.
	PC = FONT_START; // Position the Program to where the Font should be loaded.
	for (int keyNum{ 0 }; keyNum < 0x10; keyNum++) {
		for (int row{ 0 }; row < 5; row++) {
			memory[PC] = keySprites[keyNum][row];
			PC++;
		}
	}
	if (PC == (FONT_START + sizeof(keySprites))) {
		printf("Font Loaded Successfully\n");
	} // Check if the Program Counter is at the end of the Font Section. Error otherwise.
	else {
		printf("Font Loading Error\n");
	}
	PC = 0; // Reset Program Counter to Zero


	// Here is where the File is going to be loaded into memory.
	// There needs to be some sort of error if the file ends up being too big to fit in the memory.
	// I also should add some way of communicating whether the file needs to be put in the standard memory location of 0x200
		// or the ETI660 specific location at 0x600. This will likely be done as a command-line tag or something.

	// I ended up not needing an error, since I guess trying to load a program that is too big will break everything and that seems funny.

	std::string filename {"IBM Logo.ch8"};
	bool ETI660_flag {false};

	if (argc != 0) {
		for (int i{ 0 }; i < argc; i++) {
			if (strstr( argv[i] , R"(.ch8)") != nullptr) {
				filename = argv[i];
				break;
			}
		}
		for (int i{ 0 }; i < argc; i++) {
			if (strstr(argv[i], R"(-660)") != nullptr) {
				ETI660_flag = true;
				break;
			}
		}
	}

	std::fstream workingFile;
	std::cout << filename << " is loading..." << std::endl;

	workingFile.open(filename, std::ios::in | std::ios::binary);

	char temp[0xFFF] = {};
	workingFile.read(temp, 0xFFF);
	//std::cout << workingFile.gcount() << std::endl;

	workingFile.close();

	// I've read the program into a temp array, and need to put the data into the appropriate location in memory, based on the flag I set earlier. 

	int programStart{ 0x200 };
	if (ETI660_flag) {
		programStart = 0x600;
	}

	PC = programStart;
	for (int i{ 0 }; i < workingFile.gcount(); i++) {
		if ((PC + i) > 0xFFF) {
			break;
		} // This little code will stop pushing the program to memory if they're at the end of the CHIP-8 memory.
		memory[PC+i] = temp[i];
	} // This code puts the program into the appropriate memory location, as well as priming the Program Counter to the proper place to start the Execution Loop.

	std::cout << "Program has been loaded into memory." << std::endl;


	// The core execution loop should be as follows:
		// 1. Interpret Instruction at the Program Counter's location in memory
		// 2. Increase Program Counter by One
	// Which is actually quite simple, all things considered.
	unsigned char currentInstruction[2]{};
	unsigned char nybl1{}, nybl2{}, nybl3{}, nybl4{};

	while (PC <= 0xFFF) {
		// The current instruction is interpreted as 2 consecutive bytes, pulled from memory at the program counter.
		currentInstruction[0] = memory[PC++];
		currentInstruction[1] = memory[PC++];

		// I think converting the instruction to 4 nybls makes the interpretation way easier on my end.
		nybl1 = (currentInstruction[0] & 0xF0) >> 4;
		nybl2 = (currentInstruction[0] & 0x0F);
		nybl3 = (currentInstruction[1] & 0xF0) >> 4;
		nybl4 = (currentInstruction[1] & 0x0F);

			//std::cout << "PC: " << std::hex << (unsigned short)PC-2 << std::endl << (unsigned short)nybl1 << "." << (unsigned short)nybl2 << "." << (unsigned short)nybl3 << "." << (unsigned short)nybl4 << " : " << std::hex << (unsigned short)currentInstruction[0] << "." << (unsigned short)currentInstruction[1] << std::endl;
		// This very long test piece worked great for testing that I'm correctly distinguishing the different nybls.
		// It also displays the entirety of the Program Memory space.
		
	}





	return 0;
}

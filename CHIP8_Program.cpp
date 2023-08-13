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

	std::string filename {"Trip8 Demo (2008) [Revival Studios].ch8"};
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
	unsigned short currentInstruction{}, targetAddress{};
	unsigned char nybl[4]{}, byte[2]{};

	while (PC <= 0xFFF) {
		// The current instruction is interpreted as 2 consecutive bytes, pulled from memory at the program counter.
		currentInstruction = memory[PC++];
		currentInstruction <<= 8;
		currentInstruction += memory[PC++];

		// I think converting the instruction to 4 nybls makes the interpretation way easier on my end.
		nybl[0] = (currentInstruction & 0xF000) >> 12;
		nybl[1] = (currentInstruction & 0x0F00) >> 8;
		nybl[2] = (currentInstruction & 0x00F0) >> 4;
		nybl[3] = (currentInstruction & 0x000F);
		// Why not also get the bytes themselves for convienience.
		byte[0] = (currentInstruction & 0xFF00) >> 8;
		byte[1] = (currentInstruction & 0x00FF);

		// Some codes use the last 3 nybls, so lets make that a thing too.
		targetAddress = (currentInstruction & 0x0FFF);

			std::cout << "PC: " << std::hex << (unsigned short)PC-2 << std::endl << (unsigned short)nybl[0] << "." << (unsigned short)nybl[1] << "." << (unsigned short)nybl[2] << "." << (unsigned short)nybl[3] << " : " << (unsigned short)byte[0] << "." << (unsigned short)byte[1] << " : " << std::hex << currentInstruction << std::endl;
		// This very long test piece worked great for testing that I'm correctly distinguishing the different nybls.
		// It also displays the entirety of the Program Memory space.
		
		// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-


		// This is the section of the code that governs all of the OPCodes and their various implimentations.
		// The Switch Statement checks all OP Codes in numeric order. Artithmatic is easy to impliment, drawing sprites is not.


		
		switch (nybl[0]) {
		case 0x0:
			if (currentInstruction == 0x0000) { // Empty Command is treated as NOP
				break;
			}
			switch (byte[1]) {
			case 0xE0: // 00E0 : Clear Screen
				break;
			case 0xEE: // 00EE : Return Subroutine
				PC = theStack.at(SP);
				SP--;
				break;
			default:
				// 0NNN : reference to computer outside of interpreter, uncompatable in modern computers
				std::cout << "Machine Jump" << std::endl;
				break;
			}
			break;
		case 0x1: // 1NNN : Jump
			PC = targetAddress;
			break;
		case 0x2: // 2NNN : Start Subroutine
			SP++;
			theStack.push_back(PC);
			PC = targetAddress;
			break;
		case 0x3: // 3XKK : Skip Instruction if VX == KK 
			if (v[nybl[1]] == byte[1]) {
				PC += 2;
			}
			break;
		case 0x4: // 4XKK : Skip Instruction if VX != KK
			if (v[nybl[1]] != byte[1]) {
				PC += 2;
			}
			break;
		case 0x5: // 5XY0 : Skip Instruction if VX == VY
			if (nybl[3] == 0) {
				if (v[nybl[2]] == v[nybl[3]]) {
					PC += 2;
				}
			}
			break;
		case 0x6: // 6XKK : Load KK into VX
			v[nybl[1]] = byte[1];
			break;
		case 0x7: // 7XKK : Add KK to VX
			v[nybl[1]] += byte[1];
			break;
		case 0x8:
			switch (nybl[3]) {
			case 0x0: // 8XY0 : Load VY into VX
				break;
			case 0x1: // 8XY1 : Bitwise OR VY into VX
				break;
			case 0x2: // 8XY2 : Bitwise AND VY into VX
				break;
			case 0x3: // 8XY3 : Bitwise XOR VY into VX
				break;
			case 0x4: // 8XY4 : Bitwise ADD VY into VX
				break;
			case 0x5: // 8XY5 : Bitwise SUB VY into VX
				break;
			case 0x6: // 8XY6 : Least Significant Bit VY into VX
				break;
			case 0x7: // 8XY7 : Bitwise SUB VX into VY
				break;
			case 0xE: // 8XYE : Most Significant Bit VY into VX
				break;
			default:
				break;
			}
			std::cout << "Bitwise" << std::endl;
			break;
		case 0x9: // 9XY0 : Skip Instruction if VX != VY
			if (nybl[3] == 0) {
				if (v[nybl[1]] != v[nybl[2]]) {
					PC += 2;
				}
			}
			break;
		case 0xA: // ANNN : Set I to Addr
			I_register = targetAddress;
			break;
		case 0xB: // 8NNN : Jump to Addr + V0
			PC = ( targetAddress + v[0] );
			break;
		case 0xC: // CXKK : Load into VX a Random Byte that's been Bitwise AND with KK
			//random number generation
			//v[nybl[2]] = (byte[2] & randomByte)
			std::cout << "Random Number" << std::endl;
			break;
		case 0xD: // DXYN : Push a sprite N-bytes long at I_register to Display[x][y]
			std::cout << "Sprite Loading" << std::endl;
			break;
		case 0xE:
			/*
			if (byte[2] == 0x9E && keyPressed[nybl[2]]) { // EX9E : Skip Instruction if Key X IS pressed
				
			}
			else if (byte[2] == 0xA1 && !keyPressed[nybl[2]]) { // EXA1 : Skip Instruction if Key X IS NOT pressed

			}
			*/
			std::cout << "Key Inquiry" << std::endl;
			break;
		case 0xF:
			switch (byte[1]) {
			case 0x07: // FX07 : Set VX to DT
				v[nybl[1]] = DT;
				break;
			case 0x0A: // FX0A : Sleep until Key is pressed. Store Key value in VX
				break;
			case 0x15: // FX15 : Set DT to VX
				DT = v[nybl[1]];
				break;
			case 0x18: // FX18 : Set ST to VX
				ST = v[nybl[1]];
				break;
			case 0x1E: // FX1E : Add VX to I
				I_register += v[nybl[1]];
				break;
			case 0x29: // FX29 : I is set to fontSprite coresponding to the Key in VX
				I_register = FONT_START + (nybl[1] * 5);
				break;
			case 0x33: // FX33 : Store BCD representation of Vx in memory locations I, I+1, and I+2
				break;
			case 0x55: // FX55 : Store registers V0 - VX in memory location starting at I
				for (int j{ 0 }; j <= nybl[1]; j++) {
					memory[I_register + j] = v[j];
				}
				break;
			case 0x65: // FX65 : Load registers V0 - VX from memory location starting at I
				for (int j{ 0 }; j <= nybl[1]; j++) {
					v[j] = memory[I_register + j];
				}
				break;
			default:
				break;
			}
			std::cout << "Math" << std::endl;
			break;
		default:
			break;
		}
		

	}





	return 0;
}

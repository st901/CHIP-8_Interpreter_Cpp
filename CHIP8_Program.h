#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

// Since I am not familiar with the CHIP-8, I am referencing "Cowgod's Chip-8 Technical Reference",
	// which is very detailed despite it's age.
// I've made all of the data structures representing the imagined machine static so they don't get killed off accidentally.

// Memory in the Chip-8 is 4096 bytes of RAM, though not all of it is meant to be used by programs
// I am going to try implimenting the Memory as a really big Array, since the amount of RAM does not change
// Each Byte is 8 Bits, represented by two Hex digits
// Each Nybl is 4 Bits, represented by a single Hex digit
// (Nybl? Nibble? Nybble? who cares, it's half a byte)
// Also of note, C++ can interpret HEX natively by using 0x# notation, so 0xF is 15
// 4096 Bytes of RAM is 4KB, but can also be interpreted as 0xFFF bytes, which is in Hex notation,
	// and thus I will try to use if for consistency
// The Primitive Data Type "Unsigned Characters" use the least amount of computer memory to store, and only take 1 byte to store
	// each memory location is 1 byte each, so this works out great
	// the Range of an Unsigned Character is from 0-255, or 0x0 to 0xFF, which is incidentally 8 bits, or 1 byte, as above
	// Technically this represents an ASCII value, but I will just use it exclusevly as I would an integer.

static unsigned char memory[0x1000] = {};

// The Chip-8 has 16 8-bit all-purpose registers, and one 16-bit instruction register (which is functionally 12-bit)
// Coincidentally the 8-bit registers are sequentially named, so I can similarly use an array to store the registers
// Just as an Unsigned Char gives me a 8-bit range, an Unsigned Short Integer gives me a 16-bit range, perfect for the "I" register.

static unsigned char v[0x10] = {};
static unsigned short I_register = {};

// There are also two 8-bit registers for creating delay. Their values are both are reduced by one, 60 times a second, until they are Zero.
	// The first is a Sound Timer. If non-zero, it will play a tone, typically of the interpreter's choice (so i get to pick).
	// The other is the Delay Timer. This can be used by programs to create timing events, like Sleeping for 5 seconds or something.

static unsigned char ST = {};
static unsigned char DT = {};

// The variable names are for my own reference, and are based on their names in the Cowgod CHIP-8 Reference.

// Like many computers, there's also a Program Counter and Stack Pointer, 16-bit and 8-bit respectively.
	// These are not used by programs, and technically shouldn't be referenced by them, but I'm not too worried, more power to you.

static unsigned char SP = {};
static unsigned short PC = {};


// When there is a Stack Pointer, there must also be a stack. I'll probably get by using a Vector, since they can be pushed and pulled from, and scale infinitely.
static std::vector<unsigned short> theStack = {16, 0x000};
// Not that it matters too much, I made the Stack Pointer a Unsigned Char, so the Stack will only ever be as big as 256 entries
// There's no risk of Stack Overflow in a compiler sense, since the Stack Pointer will loop back to zero. This will break the execution though...
// The original Stack of the CHIP-8 is only ever supposed to go as high as 16, so technically the Stack Pointer could be as small as a Nybl.

// I need to also define a Sprite for each Keyboard Key.
// Sprites are 8-pixels wide, simply because they are defined by Bytes. Each 1 bit is an enabled pixel, and each 0 bit is a disabled pixel.
// The Keyboard Sprites are specifically 5-pixels tall, because they must all be made up of 5 Bytes.
// I'll probably define them here, and then add them to the memory later.

// I think if you use 0b# notation you can represent Binary Literals. That makes it easier for me to "Draw" the sprites
// Cowgod's Reference has some rather basic sprites, but I'm gonna make it a little spicy and make mine custom. Use some of that Digital Art Minor I dropped.
// Incidentally, all of his sprites are left-aligned, and do not use the lower 4-bits of the sprite. I guess that's a built in space character or something.
// I'll try to copy that as well I guess.

// I'm also going to use a 2D array to keep all of the sprite data in one variable, keep myself from making a variable for each sprite.
// First Dimension is the Character it represents, Second Dimension is which of the 5 rows of pixels it represents.
	// This also means the index of the array is the value it literally represents.
static unsigned char keySprites[0x10][5] = { // 16 Sprites, Count Starts at Zero, 
	{	 // 0 ; I like Zeros with the slash through them, makes them different from an O
		0b01100000,
		0b10110000,
		0b10010000,
		0b11010000,
		0b01110000
	},	{// 1 ; This one is the same as Cowgod's, it's already perfect.
		0b00100000,
		0b01100000,
		0b00100000,
		0b00100000,
		0b01110000
	},	{// 2 ; Why make a blocky 2 when I can make a curvy one?
		0b01100000,
		0b10010000,
		0b00100000,
		0b01000000,
		0b11110000
	},	{// 3 ; I wanted this to look different from the B and unlike a backwards E. Bonus! It's only 3 pixels wide
		0b11000000,
		0b00100000,
		0b11000000,
		0b00100000,
		0b11000000
	},	{// 4 ; I think the closed 4 looks cooler here. Also get that overlap in the lower right of the sprite too.
		0b01100000,
		0b10100000,
		0b10100000,
		0b11110000,
		0b00100000
	},	{// 5 ; This looks much more like a 5 than an S. Hard angle on the top left, imply curves elsewhere.
		0b11110000,
		0b10000000,
		0b01110000,
		0b00010000,
		0b11100000
	},	{// 6 ; Different from the 9, but still kinda bottom heavy.
		0b01110000,
		0b10000000,
		0b11110000,
		0b10010000,
		0b11100000
	},	{// 7 ; I could have done a full slanted 7, but this one looks smart, even if it's basic.
		0b11110000,
		0b00010000,
		0b00100000,
		0b01000000,
		0b01000000
	},	{// 8 ; Rounding the corners shouldn't end poorly, I hope
		0b01100000,
		0b10010000,
		0b01100000,
		0b10010000,
		0b01100000
	},	{// 9 ; This 9 looks like it's squinting at you
		0b01100000,
		0b10010000,
		0b01110000,
		0b00010000,
		0b01100000
	},	{// A ; Downslanted A is very cool looking, reminisant of the Triangle A, while in a rectangle
		0b01000000,
		0b10100000,
		0b10010000,
		0b11110000,
		0b10010000
	},	{// B ; Making unique letters is tricky with so little space. At least this one isn't blocky.
		0b11100000,
		0b10010000,
		0b11100000,
		0b10010000,
		0b11100000
	},	{// C ; C is for Cookie
		0b01110000,
		0b10001000,
		0b10000000,
		0b10001000,
		0b01110000
	},	{// D ; This slanted D is going to make me look really dumb is a game is trying to use these to draw something.
		0b11000000,
		0b10100000,
		0b10010000,
		0b10010000,
		0b11100000
	},	{// E ; Short middle section supremacy
		0b11110000,
		0b10000000,
		0b11100000,
		0b10000000,
		0b11110000
	},	{// F ; By rounding the corner like this, it looks different than the E also remind you of a lowercase F a bit
		0b01110000,
		0b10000000,
		0b11100000,
		0b10000000,
		0b10000000
	}
};
const unsigned short FONT_START = {0x050}; // Apparently this is where most people tend to put it, so by all means

// The basic, simple, original CHIP-8 uses a Pixel Grid of 64 pixel width, 32 pixel height
// I will use a 2D array of Booleans to represent these pixels, as well as whether they are enabled or disabled.
// Just like an Array, the coordinates used by CHIP-8 programs start at 0,0 and continue to 63,31.

static bool Display[64][32] = {};
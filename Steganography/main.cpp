#include <iostream>
#include <array>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

using byte = uint8_t;
//using pixel = std::array<byte, 4>;
struct channels {
	byte a;
	byte b;
	byte g;
	byte r;
};

struct pixel {
	channels channels;
};

//Maps a 2D index to a 1D index
int index(int x, int y, int width) {
	return y * width + x;
}

pixel readPixel(int x, int y, int width, byte* memory) {
	pixel pixel;
	//uint32_t channels = memory[index(x, y, width)];
	pixel.channels.a = (memory[index(x + 0, y, width)] & 0xff000000) >> 24;
	pixel.channels.b = (memory[index(x + 1, y, width)] & 0x00ff0000) >> 16;
	pixel.channels.g = (memory[index(x + 2, y, width)] & 0x0000ff00) >> 8;
	pixel.channels.r = (memory[index(x + 3, y, width)] & 0x000000ff);
	return pixel;
}

uint32_t readPixel(int index, byte* memory) {
	uint32_t value = 0;
	memcpy(&value, memory + index, sizeof(uint32_t));
	return value;
}

void printPixel(byte r, byte g, byte b, byte a) {
	printf("r: %u g: %u b: %u a: %u\n", r, g, b, a);
}

void printPixel(uint32_t rgba) {
	printf("%X\n", rgba);
}

int main() {
	const int32_t rgba = 4;
	int32_t width = 0, height = 0;

	//Channels are a, b, g, r (left to right, most to least significant bit) in memory!
	byte* memory = stbi_load("res/test_input.png", &width, &height, nullptr, rgba);
	const int32_t row_size = width * rgba;

	//Memory of the first 5 pixels for the first two rows:
	//0        //1      //2      //3      //4
	//red      //green  //blue   //white  //black
	//ff0000ff ff00ff00 ffff0000 ffffffff ff000000

	//5        //6      //7      //8      //9
	//black    //white  //blue   //green  //red
	//ff000000 ffffffff ffff0000 ff00ff00 ff0000ff

	byte b0 = memory[0];
	byte b1 = memory[1];
	byte b2 = memory[2];
	byte b3 = memory[3];
	uint32_t ba = readPixel(0, memory);

	byte b4 = memory[4];
	byte b5 = memory[5];
	byte b6 = memory[6];
	byte b7 = memory[7];
	uint32_t bb = readPixel(4, memory);
	
	byte b8 = memory[8];
	byte b9 = memory[9];
	byte b10 = memory[10];
	byte b11 = memory[11];
	uint32_t bc = readPixel(8, memory);

	printf("Red\n");
	printPixel(b0, b1, b2, b3);
	printPixel(ba);

	printf("\nGreen\n");
	printPixel(b4, b5, b6, b7);
	printPixel(bb);

	printf("\nBlue\n");
	printPixel(b8, b9, b10, b11);
	printPixel(bc);

	//Layout is ABGR most to least significant.
	printf("Break!\n");

	//1st row:
	//pixel p00 = readPixel(0, 0, width, memory);	//Should be red.
	//pixel p10 = readPixel(1, 0, width, memory);
	//pixel p20 = readPixel(2, 0, width, memory);
	//pixel p30 = readPixel(3, 0, width, memory);
	//pixel p40 = readPixel(4, 0, width, memory);
	//
	////2nd row:
	//pixel p01 = readPixel(0, 1, width, memory);	//Should be black.
	//pixel p11 = readPixel(1, 1, width, memory);
	//pixel p21 = readPixel(2, 1, width, memory);
	//pixel p31 = readPixel(3, 1, width, memory);
	//pixel p41 = readPixel(4, 1, width, memory);

	//Makes the first 2.5 rows white:
	//for (int i = 0; i < 5; i++) {
	//	byte* row = memory + i * row_size;
	//	memset(row, 255, row_size);
	//}

	//Makes the first 2.5 rows white:
	//for (int i = 0; i < 5; i++) {
	//	byte* row = memory + i * row_size;
	//	memset(row, 255, row_size / 2);
	//}
	
	//Make the blue channel of each pixel fully blue:
	//for (int y = 0; y < height; y++) {
	//	for (int x = 0; x < width; x++) {
	//		int i = index(x, y, width);
	//		byte a = memory[i];
	//		byte b = memory[i + 1];
	//		byte g = memory[i + 2];
	//		byte r = memory[i + 3];
	//		printf("R: %i G: %i B: %i A: %i\n", r, g, b, a);
	//	}
	//}

	//Works
	//stbi_write_png("res/test_output.png", width, height, rgba, memory, width * rgba);

	stbi_image_free(memory);
	return EXIT_SUCCESS;
}
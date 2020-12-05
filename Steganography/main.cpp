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

int main() {
	const int32_t rgba = 4;
	int32_t width = 0, height = 0;

	//Channels are a, b, g, r (left to right, most to least significant bit) in memory!
	byte* memory = stbi_load("res/test_input.png", &width, &height, nullptr, rgba);
	const int32_t row_size = width * rgba;

	//Memory of the first 5 pixels for the first two rows:
	//0        //1      //2      //3      //4
	//ff0000ff ff00ff00 ffff0000 ffffffff ff000000
	//5        //6      //7      //8      //9
	//ff000000 ffffffff ffff0000 ff00ff00 ff0000ff

	//1st row:
	pixel p00 = readPixel(0, 0, width, memory);	//Should be red.
	pixel p10;
	pixel p20;
	pixel p30;
	pixel p40;

	//2nd row:
	pixel p01 = readPixel(0, 1, width, memory);	//Should be black.
	pixel p11;
	pixel p21;
	pixel p31;
	pixel p41;

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
	stbi_write_png("res/test_output.png", width, height, rgba, memory, width * rgba);

	stbi_image_free(memory);
	return EXIT_SUCCESS;
}
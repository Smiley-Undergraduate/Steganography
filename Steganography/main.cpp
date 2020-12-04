#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

using byte = unsigned char;

int main() {
	const int32_t rgba = 4;
	int32_t width = 0, height = 0;

	//red      //green  //blue   //white  //black
	//ff0000ff ff00ff00 ffff0000 ffffffff ff000000
	byte* memory = stbi_load("res/random.png", &width, &height, nullptr, rgba);

	//black    //white  //blue   //green  //red
	//ff000000 ffffffff ffff0000 ff00ff00 ff0000ff
	byte* next_row = memory + uintptr_t(width * rgba);

	//Channels are a, b, g, r (left to right, most to least significant bit) in memory!

	//Having fun using the memory viewer to define the width and height as hex addresses :D
	int32_t w = 0x00000280;
	int32_t h = 0x000001e0;
	printf("Width: %i, height: %i.\n", w, h);

	stbi_image_free(memory);
	return EXIT_SUCCESS;
}
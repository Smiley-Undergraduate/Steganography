#pragma region includes
#include <cstdio>
#include <cstdint>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>
#pragma endregion

#pragma region types
using channel= uint8_t;
using pixel = uint32_t;
#pragma endregion

#pragma region packing
inline void pack_r(pixel& rgba, channel r) {
	rgba |= static_cast<pixel>(r);
}

inline void pack_g(pixel& rgba, channel g) {
	rgba |= static_cast<pixel>(g) << 8;
}

inline void pack_b(pixel& rgba, channel b) {
	rgba |= static_cast<pixel>(b) << 16;
}

inline void pack_a(pixel& rgba, channel a) {
	rgba |= static_cast<pixel>(a) << 24;
}

inline pixel pack_pixel(channel r, channel g, channel b, channel a) {
	pixel rgba = 0;
	pack_r(rgba, r);
	pack_g(rgba, g);
	pack_b(rgba, b);
	pack_a(rgba, a);
	return rgba;
}
#pragma endregion

#pragma region unpacking
inline void unpack_r(pixel rgba, channel& r) {
	r = (rgba & 0x000000FF);
}

inline void unpack_g(pixel rgba, channel& g) {
	g = (rgba & 0x0000FF00) >> 8;
}

inline void unpack_b(pixel rgba, channel& b) {
	b = (rgba & 0x00FF0000) >> 16;
}

inline void unpack_a(pixel rgba, channel& a) {
	a = (rgba & 0xFF000000) >> 24;
}

inline void unpack_pixel(pixel rgba, channel& r, channel& g, channel& b, channel& a) {
	unpack_r(rgba, r);
	unpack_g(rgba, g);
	unpack_b(rgba, b);
	unpack_a(rgba, a);
}
#pragma endregion

int index(int x, int y, int width) {
	return y * width + x;
}

inline pixel read_pixel(int x, int y, int width, uint32_t* memory) {
	return memory[index(x, y, width)];
}

inline void write_pixel(pixel value, int x, int y, int width, uint32_t* memory) {
	memory[index(x, y, width)] = value;
}

inline void print_pixel(pixel rgba) {
	printf("%X\n", rgba);
}

inline void print_channel(channel c) {
	printf("%u\n", c);
}

inline pixel set_bit(pixel rgba, int index) {
	return rgba | (1 << index);
}

bool get_bit(pixel rgba, int index) {
	return rgba & (1 << index);
}

struct test {
	char a;
	float b;
	char c;
	long d;
};

void serialize(const test& t) {
	//1. Fit pixels to T:
	//Pixels are 4 bytes, so if T is 5 bytes we need two pixels.
	const int pixelBase = sizeof(t) / sizeof(pixel);
	const int pixelAdditional = sizeof(t) % sizeof(pixel);
	const int pixelTotal = pixelBase + pixelAdditional;

	//2. Fit bits to pixels:
	//1 pixel is 4 bytes or 32 bits.
	//Hence, bits needed = pixels needed * 8;
	const int bitTotal = pixelTotal * 8;

	//3. Save by writing to last bit of each channel!
}

int main() {
	int six = 6;						//0110
	int seven = set_bit(six, 0);		//0111
	int fourteen = set_bit(six, 3);		//1110

	const int components = 4;
	int width = 0, height = 0;
	pixel* memory = reinterpret_cast<pixel*>(stbi_load("res/test_input.png", &width, &height, nullptr, components));
	const int row_bytes = width * components;
	const int col_bytes = height * components;
	const int all_bytes = width * height * components;

	pixel before = 0xFCFDFEFF;
	channel r, g, b, a;
	unpack_pixel(before, r, g, b, a);
	pixel after = pack_pixel(r, g, b, a);

	//Channels are a, b, g, r (left to right, most to least significant bit) in memory!
	//Memory of the first 5 pixels for the first two rows:

	//0        //1      //2      //3      //4
	//red      //green  //blue   //white  //black
	//ff0000ff ff00ff00 ffff0000 ffffffff ff000000

	//5        //6      //7      //8      //9
	//black    //white  //blue   //green  //red
	//ff000000 ffffffff ffff0000 ff00ff00 ff0000ff

	pixel p00 = read_pixel(0, 0, width, memory);
	printf("P00 (red)\n");
	print_pixel(p00);

	pixel p10 = read_pixel(1, 0, width, memory);
	printf("\nP10 (green)\n");
	print_pixel(p10);

	pixel p20 = read_pixel(2, 0, width, memory);
	printf("\nP20 (blue)\n");
	print_pixel(p20);

	pixel p30 = read_pixel(3, 0, width, memory);
	printf("\nP30 (white)\n");
	print_pixel(p30);

	pixel p40 = read_pixel(4, 0, width, memory);
	printf("\nP40 (black)\n");
	print_pixel(p40);

	printf("\nSecond row:\n");
	pixel p01 = read_pixel(0, 1, width, memory);
	printf("P01 (black)\n");
	print_pixel(p01);

	pixel p11 = read_pixel(1, 1, width, memory);
	printf("\nP11 (white)\n");
	print_pixel(p11);

	pixel p21 = read_pixel(2, 1, width, memory);
	printf("\nP21 (blue)\n");
	print_pixel(p21);

	pixel p31 = read_pixel(3, 1, width, memory);
	printf("\nP31 (green)\n");
	print_pixel(p31);

	pixel p41 = read_pixel(4, 1, width, memory);
	printf("\nP41 (red)\n");
	print_pixel(p41);

	//Make a copy of the original image memory so we can restore it after overwriting.
	pixel* memory_copy = reinterpret_cast<pixel*>(malloc(all_bytes));
	memcpy(memory_copy, memory, all_bytes);

	//Red tint:
	memcpy(memory, memory_copy, all_bytes);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			pixel rgba = read_pixel(x, y, width, memory);
			pack_r(rgba, 0xFF);
			write_pixel(rgba, x, y, width, memory);
		}
	}
	stbi_write_png("res/test_output_red.png", width, height, components, memory, row_bytes);

	//Green tint:
	memcpy(memory, memory_copy, all_bytes);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			pixel rgba = read_pixel(x, y, width, memory);
			pack_g(rgba, 0xFF);
			write_pixel(rgba, x, y, width, memory);
		}
	}
	stbi_write_png("res/test_output_green.png", width, height, components, memory, row_bytes);
	
	//Blue tint:
	memcpy(memory, memory_copy, all_bytes);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			pixel rgba = read_pixel(x, y, width, memory);
			pack_b(rgba, 0xFF);
			write_pixel(rgba, x, y, width, memory);
		}
	}
	stbi_write_png("res/test_output_blue.png", width, height, components, memory, row_bytes);

	stbi_image_free(memory);
	free(memory_copy);
	return getchar();
}
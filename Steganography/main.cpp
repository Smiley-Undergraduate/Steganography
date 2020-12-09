#include <cstdio>
#include <cstdint>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

using channel= uint8_t;
using pixel = uint32_t;

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

inline void unpack_pixel(pixel rgba, channel& r, channel& g, channel& b, channel& a) {
	a = (rgba & 0xFF000000) >> 24;
	b = (rgba & 0x00FF0000) >> 16;
	g = (rgba & 0x0000FF00) >> 8;
	r = (rgba & 0x000000FF);
}

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

int main() {
	const int components = 4;
	int width = 0, height = 0;
	uint32_t* memory = reinterpret_cast<uint32_t*>(stbi_load("res/test_input.png", &width, &height, nullptr, components));
	const int row_bytes = width * components;

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
	
	//Make the blue channel of each pixel fully blue:
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			const int i = index(x, y, width);
		}
	}

	stbi_write_png("res/test_output.png", width, height, components, memory, row_bytes);
	stbi_image_free(memory);
	
	return getchar();
}
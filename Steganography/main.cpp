#pragma region includes
#include <cstdio>
#include <cstdint>

#include <iostream>
#include <bitset>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>
#pragma endregion

#pragma region types
using byte = uint8_t;
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

inline byte set_bit_on(byte b, int i) {
	return b | (1 << i);
}

inline byte set_bit_off(byte b, int i)
{
	return b & ~(1 << i);
}

inline bool get_bit(byte b, int i) {
	return b & (1 << i);
}

struct test {
	char arr[3];
};

void serialize(const test& t, int xStart, int yStart, int arrayWidth, pixel* memory) {
	const int bitCount = sizeof(t) * 8;
	const int byteCount = sizeof(t);
	const int pixelCount = byteCount / sizeof pixel + (byteCount % sizeof(pixel) == 0 ? 0 : 1);

	const uint8_t* byteAddress = reinterpret_cast<const uint8_t*>(&t);
	
	//for all pixels
	for (int i = 0; i < pixelCount; i++) {
		//for all bytes in a pixel (4)
		for (int i = 0; i < sizeof(pixel); i++) {
			uint8_t byte = *byteAddress;
			//for all bytes in a bit (8, highest-to-lowest)
			for (int i = 7; i >= 0; i--) {
				//Windows is little endian which is apparently least to most significant bit (right to left ascending rather than left to right).
				//TLDR I need testing before I write this!
			}
			byteAddress++;
		}
	}
}

template<typename T>
std::string to_binary(T t) {
	std::bitset<sizeof(t) * 8> bits(t);
	return bits.to_string();
}

template<typename T>
void print_binary(T t) {
	std::cout << to_binary(t) << std::endl;
}

int main() {
	//Big endian means the most significant byte is at the lowest address (towards the end of the data).
	//ie 0x0F01 is 0000 1111 0000 0001  

	//Little endian means the least significant byte is at the lowest address (towards the start of the data).
	//ie 0x0F01 is 0000 0001 0000 1111

	uint16_t u16 = 0;
	byte& rl = *reinterpret_cast<byte*>(&u16);			//low (least significant)
	byte& rh = *(reinterpret_cast<byte*>(&u16) + 1);	//high (most significant)
	byte* p_rl = &rl;
	byte* p_rh = &rh;

	rl = 0x01;
	rh = 0x0F;

	uint16_t* pu16 = &u16;
	print_binary(u16);

	//Windows is little endian.
	//Our pixel memory layout is ABGR where A is most significant and R is least significant.
	//If we set A to 255 and R to 1, our memory should look like:
	//0000 0001 0000 0000 0000 0000 1111 1111 in little endian!

	pixel rgba = 0;
	pack_a(rgba, 255);
	pack_r(rgba, 1);
	pixel* p_rgba = &rgba;
	print_binary(rgba);

	pixel hex = 0xFF000001;
	print_binary(hex);

	//The displays look like big endian. The most significant bits are printed first... No entiendo!

	const int components = 4;
	int width = 0, height = 0;

	//First five pixels are red, green, blue, black white
	//Red --> FF0000FF Green --> FF00FF00 Blue --> FFFF0000 Black --> FFFFFFFF White --> FF000000
	pixel* memory = reinterpret_cast<pixel*>(stbi_load("res/test_input.png", &width, &height, nullptr, components));

	const int row_bytes = width * components;
	const int col_bytes = height * components;
	const int all_bytes = width * height * components;

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
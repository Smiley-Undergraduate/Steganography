#pragma region includes
#include <cstdio>
#include <cstdint>
#include <cassert>

#include <iostream>
#include <fstream>
#include <vector>
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
#pragma optimize("", off)

enum class CHANNEL : int {
	R,
	G,
	B,
	A
};

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

inline void pack(pixel& rgba, channel value, CHANNEL type) {
	switch (type)
	{
	case CHANNEL::R:
		pack_r(rgba, value);
		break;
	case CHANNEL::G:
		pack_g(rgba, value);
		break;
	case CHANNEL::B:
		pack_b(rgba, value);
		break;
	case CHANNEL::A:
		pack_a(rgba, value);
		break;
	default:
		break;
	}
}

inline pixel pack(channel r, channel g, channel b, channel a) {
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

inline void unpack(pixel rgba, channel& value, CHANNEL type) {
	switch (type)
	{
	case CHANNEL::R:
		unpack_r(rgba, value);
		break;
	case CHANNEL::G:
		unpack_g(rgba, value);
		break;
	case CHANNEL::B:
		unpack_b(rgba, value);
		break;
	case CHANNEL::A:
		unpack_a(rgba, value);
		break;
	default:
		break;
	}
}

inline void unpack(pixel rgba, channel& r, channel& g, channel& b, channel& a) {
	unpack_r(rgba, r);
	unpack_g(rgba, g);
	unpack_b(rgba, b);
	unpack_a(rgba, a);
}
#pragma endregion

int index(int x, int y, int width, int height) {
	int result = y * width + x;
	assert(result < width* height);
	return result;
}

inline pixel read_pixel(int x, int y, int width, int height, uint32_t* memory) {
	return memory[index(x, y, width, height)];
}

inline void write_pixel(pixel value, int x, int y, int width, int height, uint32_t* memory) {
	memory[index(x, y, width, height)] = value;
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

template<typename T>
std::string to_binary(T t) {
	std::bitset<sizeof(t) * 8> bits(t);
	return bits.to_string();
}

template<typename T>
void print_binary(T t) {
	std::cout << to_binary(t) << std::endl;
}

struct test {
	long memes;
	//char a = 1;
	//short b = 2;
	//int c = 3;
	//long long d = 0xF1;
};

void serialize(const test& t, int xStart, int yStart, int arrayWidth, int arrayHeight, pixel* memory) {
	//Data is not guaranteed to fit perfectly into pixels.
	//We ensure we have enough pixels to store our data by analyzing the remainder of bytes divided by bytes per pixel.
	const int bitCount = sizeof(t) * 8;
	const int byteCount = bitCount / 8;
	const int pixelCount = byteCount / sizeof pixel + byteCount % sizeof pixel == 0 ? 0 : 1;
	const uint8_t* byteAddress = reinterpret_cast<const uint8_t*>(&t);
}

void test_serialize(std::ofstream& output, const test& input) {
	const int byteCount = sizeof(input);
	const int bitCount = byteCount * 8;
	std::vector<bool> bits(bitCount);
	//std::bitset<bitCount> bits;

	byte* address = reinterpret_cast<byte*>(const_cast<test*>(&input));
	for (int i = 0; i < byteCount; i++) {
		for (int j = 7; j >= 0; j--) {
			bool bit = get_bit(*address, j);
			bits.push_back(bit);
		}
		address++;
	}

	for (bool i : bits) {
		int representation = i;
		output << representation;
	}
}

void test_deserialize(std::ifstream& input, test& output) {

}

//Overwrites channel values if pointers are non-null.
void write(const channel* r, const channel* g, const channel* b, const channel* a, int width, int height, pixel* memory) {
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			pixel rgba = read_pixel(x, y, width, height, memory);
			if (r != nullptr) pack_r(rgba, *r);
			if (g != nullptr) pack_g(rgba, *g);
			if (b != nullptr) pack_b(rgba, *b);
			if (a != nullptr) pack_a(rgba, *a);
			write_pixel(rgba, x, y, width, height, memory);
		}
	}
}

int main() {
	test t;
	serialize(t, 0, 0, 0, 0, nullptr);
	
	//1. Serialize test
	//std::ofstream output("test.txt");
	//test_serialize(output, t);
	//output.close();

	//2. Deserialize test
	//std::ifstream input("test.txt");


	//Big endian puts the most significant bit (largest value) first.
	//Litte endian puts the least significant bit (smallest value) first.
	uint16_t memes = 0x0301;
	uint16_t u16 = 0;
	byte& rl = *reinterpret_cast<byte*>(&u16);
	byte& rh = *(reinterpret_cast<byte*>(&u16) + 1);
	rl = 0x01;
	rh = 0x03;
	print_binary(u16);
	print_binary(memes);

	//Windows is big endian and most significant bits are being displayed first. Everything checks out!
	pixel rgba = 0;
	pack_a(rgba, 255);
	pack_r(rgba, 1);
	print_binary(rgba);
	pixel hex = 0xFF000001;
	print_binary(hex);

	/*const int components = 4;
	int width = 0, height = 0;

	//First five pixels are red, green, blue, black white
	//Red --> FF0000FF Green --> FF00FF00 Blue --> FFFF0000 Black --> FFFFFFFF White --> FF000000
	pixel* memory = reinterpret_cast<pixel*>(stbi_load("res/test_input.png", &width, &height, nullptr, components));

	const int row_bytes = width * components;
	const int col_bytes = height * components;
	const int all_bytes = width * height * components;
	const channel full = 0xFF;

	//Make a copy of the original image memory so we can restore it after overwriting.
	pixel* memory_copy = static_cast<pixel*>(malloc(all_bytes));
	memcpy(memory_copy, memory, all_bytes);

	memcpy(memory, memory_copy, all_bytes);
	write(&full, nullptr, nullptr, nullptr, width, height, memory);
	stbi_write_png("res/test_output_red.png", width, height, components, memory, row_bytes);

	memcpy(memory, memory_copy, all_bytes);
	write(nullptr, &full, nullptr, nullptr, width, height, memory);
	stbi_write_png("res/test_output_green.png", width, height, components, memory, row_bytes);
	
	memcpy(memory, memory_copy, all_bytes);
	write(nullptr, nullptr, &full, nullptr, width, height, memory);
	stbi_write_png("res/test_output_blue.png", width, height, components, memory, row_bytes);

	stbi_image_free(memory);
	free(memory_copy);*/
	printf("Done!\n");
	return getchar();
}
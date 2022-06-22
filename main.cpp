#include <openssl/sha.h>

#include <algorithm>
#include <array>
#include <bitset>
#include <iomanip>
#include <iostream>
#include <span>
#include <vector>

#define LOG(x) std::cout << x << '\n';

/*hex string to binary string stream*/
std::string convert(const std::string &hexa) {
	size_t i = 0;
	std::stringstream ss;
	while (hexa[i]) {
		switch (hexa[i]) {
			case '0':
				ss << "0000";
				break;
			case '1':
				ss << "0001";
				break;
			case '2':
				ss << "0010";
				break;
			case '3':
				ss << "0011";
				break;
			case '4':
				ss << "0100";
				break;
			case '5':
				ss << "0101";
				break;
			case '6':
				ss << "0110";
				break;
			case '7':
				ss << "0111";
				break;
			case '8':
				ss << "1000";
				break;
			case '9':
				ss << "1001";
				break;
			case 'A':
			case 'a':
				ss << "1010";
				break;
			case 'B':
			case 'b':
				ss << "1011";
				break;
			case 'C':
			case 'c':
				ss << "1100";
				break;
			case 'D':
			case 'd':
				ss << "1101";
				break;
			case 'E':
			case 'e':
				ss << "1110";
				break;
			case 'F':
			case 'f':
				ss << "1111";
				break;
			default:
				std::cout << "\nlease enter valid hexadecimal digit " << hexa[i];
		}
		i++;
	}
	return ss.str();
}

/*hex char to int*/
inline unsigned int value(char c) {
	if (c >= '0' && c <= '9') {
		return c - '0';
	}
	if (c >= 'a' && c <= 'f') {
		return c - 'a' + 10;
	}
	if (c >= 'A' && c <= 'F') {
		return c - 'A' + 10;
	}
	return -1;
}

/*function to xor hex strings*/
std::string strXor(const std::string &s1, const std::string &s2) {
	const char alphabet[] = "0123456789abcdef";

	std::string result;
	result.reserve(s1.length());

	for (std::size_t i = 0; i != s1.length(); ++i) {
		unsigned int v = value(s1[i]) ^ value(s2[i]);

		result.push_back(alphabet[v]);
	}

	return result;
}

std::string sha256(std::span<uint8_t> data) {
	unsigned char hash[SHA256_DIGEST_LENGTH];
	SHA256_CTX sha256;
	SHA256_Init(&sha256);
	SHA256_Update(&sha256, data.data(), data.size());
	SHA256_Final(hash, &sha256);
	std::stringstream ss;
	for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
		ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
	}
	return ss.str();
}

/*generate the dataset*/
template <const size_t BLen, const size_t ALen>
auto generateData() {
	std::array<std::bitset<BLen>, ALen> data;
	std::bitset<BLen> b{};

	data[0] = b;

	b = b.to_ullong() + 1;

	for (size_t i = 1; i < ALen; i++) {
		data[i] = b;
		b = (b << 1);
	}

	return data;
}

/*function to convert bitsets into an array of uint8_t*/
template <const size_t ULen>
auto toByteArray(std::bitset<ULen> &data) {
	std::array<uint8_t, ULen / 8> res;
	const std::bitset<ULen> mask{0xFF};
	size_t index = ULen / 8;

	for (int i = 0; i < ULen / 8; i++) {
		uint8_t byte = (data & mask).to_ulong();
		res[--index] = byte;
		data = (data >> 8);
	}

	return res;
}

int main() {
	const size_t n = 50;
	const size_t A = 104; /*must be divisible by 8*/

	static_assert(A >= n - 1);

	std::array<std::bitset<A>, n> data = generateData<A, n>();

	for (auto x : data) {
		std::cout << x << std::endl;
	}

	std::array<std::array<uint8_t, A / 8>, n> res;

	size_t index = 0;
	for (auto i = data.begin(); i != data.end(); i++, index++) {
		res[index] = (toByteArray(*i));
	}

	std::string parent = sha256(res[0]);
	std::string xored;
	std::string bin_xored;

	unsigned int countK = 0;
	for (auto x = res.begin() + 1; x != res.end(); x++) {
		xored = strXor(parent, sha256(*x));
		// std::cout << sha256(*x) << std::endl;
		bin_xored = convert(xored);
		countK += std::count(bin_xored.begin(), bin_xored.end(), '1');
	}

	std::cout << "result is: " << countK / (float)((n - 1) * 256) << '\n';

	/*
	 //binary representation of string "0001" for validation, expected hash is 888b19a43b151683c87895f6211d9f8640f97bdc8ef32f03dbe057c8f5e56d32
	std::array<uint8_t, 4> m{0b00110000, 0b00110000, 0b00110000, 0b00110001};
	LOG(sha256(m));
	std::bitset<32> bit{0b00110000001100000011000000110001};
	auto f = toByteArray<32>(bit);
	std::cout << sha256(f) << std::endl;
	*/
}

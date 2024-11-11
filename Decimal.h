#pragma once
#include <iostream>
#include <vector>
#include <string>

class Decimal {
private:
	char* _num;
	int64_t _size = 0;

	inline constexpr uint8_t CharToDigit(const char& ch) noexcept;
	inline constexpr char DigitToChar(const uint8_t& digit) noexcept;
	inline constexpr int64_t find(const char& ch) noexcept;
	friend std::ostream& operator << (std::ostream& ostream, const Decimal& num) noexcept;
	friend std::istream& operator >> (std::istream& istream, Decimal& num) noexcept;
	Decimal(const char* num, int64_t size) noexcept;
	
public:
	Decimal(const char* num);
	Decimal(const Decimal& other);
	Decimal();
	~Decimal();

	Decimal operator + (Decimal& other) noexcept;
	void operator += (Decimal& other) noexcept;
	Decimal operator - (Decimal& other) noexcept;
	Decimal operator * (Decimal& other) noexcept;
	const Decimal& operator = (const Decimal& num) noexcept;
	bool operator < (Decimal& other) noexcept;
	bool operator > (Decimal& other) noexcept;
	bool operator == (Decimal& other) noexcept;
	static Decimal abs(const Decimal& other) noexcept;
	Decimal pow(const uint16_t& n) noexcept;
};

class InvalidValue : public std::exception {
public:
	const char* what() const noexcept override { return "Entered invalid value"; }
};
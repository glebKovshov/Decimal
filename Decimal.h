#pragma once
#include <iostream>
#include <vector>
#include <string>

class Decimal {
private:
	char* _num = nullptr;
	int64_t _size = 0;
	bool _is_negative = false;
	uint16_t precision = 50;

	inline constexpr int8_t CharToDigit(const char& ch) noexcept;
	inline constexpr char DigitToChar(const uint8_t& digit) noexcept;
	inline constexpr int64_t find(const char& ch) const  noexcept;
	friend std::ostream& operator << (std::ostream& ostream, const Decimal& num) noexcept;
	friend std::istream& operator >> (std::istream& istream, Decimal& num) noexcept;
	inline void AddDigit(const char& digit) noexcept;
	Decimal(const char* num, const int64_t size, bool is_negative = false) noexcept;
	Decimal addition(Decimal other) noexcept;
	Decimal subtraction(Decimal other) noexcept;
	
public:
	Decimal(const char* num);
	Decimal(const Decimal& other, bool is_negative);
	Decimal(const Decimal& other);
	Decimal();
	~Decimal();

	Decimal operator + (Decimal other) noexcept;
	void operator += (Decimal& other) noexcept;
	void operator -= (Decimal& other) noexcept;
	Decimal operator - (Decimal other) noexcept;
	Decimal operator * (Decimal& other) noexcept;
	Decimal operator / (Decimal& other);
	const Decimal& operator = (const Decimal& num) noexcept;
	bool operator < (Decimal& other) noexcept;
	bool operator < (int64_t num) noexcept;
	bool operator > (Decimal& other) noexcept;
	bool operator > (int64_t num) noexcept;
	bool operator == (Decimal& other) noexcept;
	bool operator == (int64_t num) noexcept;
	bool operator >= (Decimal& other) noexcept;
	Decimal pow(const uint32_t& n) noexcept;
	Decimal IntToDecimal(int64_t num) noexcept;
};

class InvalidValue : public std::exception {
public:
	const char* what() const noexcept override { return "Entered invalid value"; }
};

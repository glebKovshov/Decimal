#pragma once
#include <iostream>
#include <vector>
#include <string>
typedef unsigned long long UInt64;
typedef long long Int64;
typedef unsigned short UInt16;
typedef short Int16;

class Decimal {
private:
	char* _num;
	UInt64 _size = 0;

	inline constexpr Int16 CharToDigit(const char& ch) noexcept;
	inline constexpr char DigitToChar(const UInt16& digit) noexcept;
	inline constexpr Int64 find(const char& ch) noexcept;
	friend std::ostream& operator << (std::ostream& ostream, const Decimal& num) noexcept;
	friend std::istream& operator >> (std::istream& istream, Decimal& num) noexcept;
	Decimal(const char* num, UInt64 size) noexcept;
	
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
	static Decimal abs(Decimal& other) noexcept;
	Decimal pow(uint16_t n) noexcept;
};

class InvalidValue : public std::exception {
public:
	const char* what() const noexcept override { return "Entered invalid value"; }
};
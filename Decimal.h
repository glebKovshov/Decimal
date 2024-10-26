#pragma once
#include <iostream>
#include <vector>
typedef unsigned long long UInt64;
typedef long long Int64;
typedef unsigned short UInt16;
typedef short Int16;

class Decimal {
private:
	char* _num;
	UInt64* _size = new UInt64(0);
	UInt64* _prec = new UInt64(0);

	inline const Int16 CharToDigit(const char& ch);
	inline const char DigitToChar(const UInt16& digit);
	inline const Int64 find(const char& ch);
	friend std::ostream& operator << (std::ostream& ostream, const Decimal& num);
	
public:
	Decimal(const char* num);
	Decimal(const Decimal& other);
	Decimal();
	~Decimal();

	Decimal operator + (Decimal& other);
	const Decimal& operator = (const Decimal& num);
	bool operator < (Decimal& other);
	bool operator > (Decimal& other);
};

class InvalidValue : public std::exception {
public:
	const char* what() const noexcept override { return "Entered invalid value"; }
};
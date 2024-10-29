﻿#include "Decimal.h"

Decimal::Decimal(const char* num) {

	if (num == nullptr) {
		_num = new char[1];
		_num[0] = '\0';
		return;
	}
	if (num[0] == '-') (*_size)++;
	else if (Decimal::CharToDigit(num[0]) == -1) throw InvalidValue();

	UInt16 dotcount = 0;
	UInt64 start_insertion = 0;

	while (num[start_insertion] == '0') start_insertion++;

	while (num[*_size + start_insertion] != '\0') {
		if (num[*_size + start_insertion] == '.') {
			dotcount++;
			if (dotcount > 1) throw InvalidValue();
		}
		else if (Decimal::CharToDigit(num[*_size + start_insertion]) == -1) throw InvalidValue();
		(*_size)++;
	}

	if (num[*_size + start_insertion - 1] == '0' && dotcount == 1) {
		for (UInt64 i = *_size + start_insertion - 1; num[i] == '0'; i--) {
			(*_size)--;
		}
	}

	if (num[*_size - 1] == '.') (*_size)--;
	
	_num = new char[*_size + 1];
	_num[*_size] = '\0';

	for (UInt64 i = 0; i < *_size; i++) {
		_num[i] = num[i+start_insertion];
	}
}

Decimal::Decimal(const Decimal& other) {
	this->_size = new UInt64(*other._size);
	this->_num = new char[*this->_size];
	for (UInt64 i = 0; i < *this->_size; i++) this->_num[i] = other._num[i];
}

Decimal::Decimal() {
	_num = new char[1];
	_num[0] = '\0';
}

Decimal::~Decimal() {
	delete[] _num;
	delete _size;
	delete _prec;
}

std::ostream& operator << (std::ostream& ostream, const Decimal& num) noexcept {
	for (UInt64 i = 0; i < *num._size; i++) ostream << num._num[i];
	return ostream;
}

const Decimal& Decimal::operator=(const Decimal& other) noexcept {
	if (this != nullptr) {
		delete[] this->_num, this->_size;
	}

	this->_size = new UInt64(*other._size);
	this->_num = new char[*this->_size];
	
	for (UInt64 i = 0; i < *this->_size; i++) this->_num[i] = other._num[i];
	return *this;
}

Decimal Decimal::abs(Decimal& other) noexcept {
	if (other._num[0] != '-') return Decimal(other);
	char* result = new char[*other._size - 1];
	for (UInt64 i = 0; i < *other._size - 1; i++) result[i] = other._num[i + 1];
	result[*other._size - 1] = '\0';
	return Decimal(result);
}

Decimal Decimal::operator+(Decimal& other) noexcept {

	std::vector<char> fracpart;
	std::vector<char> intpart;
	UInt16 carryover = 0;
	UInt64 fraclen1 = 0;
	UInt64 fraclen2 = 0;
	UInt64 start1 = 0;
	UInt64 start2 = 0;
	Int64 end1 = 0;
	Int64 end2 = 0;
	UInt16 result = 0;
	UInt64 index = 0;
	Int64 dotpos1 = this->find('.');
	Int64 dotpos2 = other.find('.');
	Int64 i = 0;

	if (dotpos1 != -1) fraclen1 = *this->_size - dotpos1 - 1; //Checking float numbers for fraction part length
	if (dotpos2 != -1) fraclen2 = *other._size - dotpos2 - 1;

	if (fraclen1 > fraclen2) {
		for (i = *this->_size - 1; i > dotpos1 + fraclen2; i--) fracpart.push_back(this->_num[i]);
		start1 = *this->_size - (fraclen1 - fraclen2) - 1;
		end1 = dotpos1;
		if (dotpos2 == -1) {
			end2 = *other._size - 1;
		}
		else {
			start2 = *other._size - 1;
			end2 = dotpos2;
		}
	}

	else if (fraclen1 < fraclen2) {
		for (i = *other._size - 1; i > dotpos2 + fraclen1; i--) fracpart.push_back(other._num[i]);
		start2 = *other._size - (fraclen2 - fraclen1) - 1;
		end2 = dotpos2;
		if (dotpos1 == -1) {
			end1 = *this->_size - 1;
		}
		else {
			start1 = *this->_size - 1;
			end1 = dotpos1;
		}
	}

	else if (fraclen1 == fraclen2 && fraclen1 != 0) {
		start1 = *this->_size - 1;
		end1 = dotpos1;
		start2 = *other._size - 1;
		end2 = dotpos2;
	}

	else {
		end1 = *this->_size - 1;
		end2 = *other._size - 1;
	}

	while (start1 > end1 && start1 > 0 && start2 > end2 && start2 > 0) {
		result = CharToDigit(this->_num[start1]) + CharToDigit(other._num[start2]) + carryover;
		carryover = result / 10;
		fracpart.push_back(DigitToChar(result % 10));
		start1--;
		start2--;
	}

	if (start1 > 0) end1--;  //Pass dot char
	if (start2 > 0) end2--;

	while (end1 > -1 || end2 > -1 || carryover > 0) { // Add operation
		result = 0;
		if (end1 > -1) {
			result += CharToDigit(this->_num[end1]);
			end1--;
		}
		if (end2 > -1) {
			result += CharToDigit(other._num[end2]);
			end2--;
		}
		if (carryover > 0) result += carryover;

		carryover = result / 10;
		intpart.push_back(DigitToChar(result % 10));
	}

	UInt64 size = intpart.size() + fracpart.size() + 1; // 1 for '\0'

	if (fracpart.size() > 0) size++; // for '.'
	char* num = new char[size];

	for (i = intpart.size() - 1; i > -1; i--, index++) num[index] = intpart[i];

	if (fracpart.size() > 0) {
		num[index] = '.';
		index++;
		for (i = fracpart.size() - 1; i > -1; i--, index++) num[index] = fracpart[i];
	}

	num[index] = '\0';

	return Decimal(num);
}

Decimal Decimal::operator - (Decimal& other) noexcept {
	Decimal absthis = Decimal::abs(*this);
	Decimal absother = Decimal::abs(other);

	if (other._num[0] == '-' && this->_num[0] != '-') { // a - (-b) = a + b
		return this->Decimal::operator+(absother);
	}

	else if (this->_num[0] == '-' && other._num[0] != '-') { // -a - b = -(a+b)
		Decimal add_res = absthis + absother;
		
		char* num = new char[*add_res._size + 1];
		num[0] = '-';

		for (UInt64 i = 0; i < *add_res._size; i++) num[i + 1] = add_res._num[i];

		num[*add_res._size + 1] = '\0';

		return Decimal(num);
	}
	
	else if (this->_num[0] == '-' && other._num[0] == '-') // -a - (-b) = b - a
		return absother - absthis;

	else if (absother > absthis) { // a - b = -(b-a) : b > a
		Decimal sub_res = absother - absthis;
		char* num = new char[*sub_res._size + 1];
		num[0] = '-';

		for (UInt64 i = 0; i <= *sub_res._size; i++) num[i + 1] = sub_res._num[i];

		return Decimal(num);
	}

	else {											  // a - b: a > b
		std::vector<char> fracpart;
		std::vector<char> intpart;
		UInt16 borrow = 0;
		UInt64 fraclen1 = 0;
		UInt64 fraclen2 = 0;
		UInt64 start1 = 0;
		UInt64 start2 = 0;
		Int64 end1 = 0;
		Int64 end2 = 0;
		UInt16 result = 0;
		UInt64 index = 0;
		Int64 dotpos1 = this->find('.');
		Int64 dotpos2 = other.find('.');
		Int64 i = 0;

		if (dotpos1 != -1) fraclen1 = *this->_size - dotpos1 - 1; // Checking float numbers for fraction part length
		if (dotpos2 != -1) fraclen2 = *other._size - dotpos2 - 1;

		if (fraclen1 > fraclen2) {								  // pushing firts num extra digits and define start and end position for each nums
			for (i = *this->_size - 1; i > dotpos1 + fraclen2; i--) fracpart.push_back(this->_num[i]);
			start1 = *this->_size - (fraclen1 - fraclen2) - 1;
			end1 = dotpos1;
			if (dotpos2 == -1) {
				end2 = *other._size - 1;
			}
			else {
				start2 = *other._size - 1;
				end2 = dotpos2;
			}
		}

		else if (fraclen1 < fraclen2) {
			fracpart.push_back(10 - other._num[(*other._size - 1)]); // first time always will be (10 - second num last digit. Then always will be (9 - second num digit) while fraclen1 < fraclen2
			borrow = 1;
			for (i = *other._size - 2; i > dotpos2 + fraclen1; i--) fracpart.push_back(10 - borrow - other._num[i]);
			start2 = *other._size - (fraclen2 - fraclen1) - 1;
			end2 = dotpos2;
			if (dotpos1 == -1) {
				end1 = *this->_size - 1;
			}
			else {
				start1 = *this->_size - 1;
				end1 = dotpos1;
			}
		}

		else if (fraclen1 == fraclen2 && fraclen1 != 0) {
			start1 = *this->_size - 1;
			end1 = dotpos1;
			start2 = *other._size - 1;
			end2 = dotpos2;
		}

		else {
			end1 = *this->_size - 1;
			end2 = *other._size - 1;
		}

		while (start1 > 0 && start2 > 0 && start1 > end1 && start2 > end2) {
			result = Decimal::CharToDigit(this->_num[start1]) - Decimal::CharToDigit(other._num[start2]) - borrow;
			if (result < 0) {
				result += 10;
				borrow = 1;
			}
			else borrow = 0;
			fracpart.push_back(Decimal::DigitToChar(result));
			start1--;
			start2--;
		}

		if (start1 > 0) end1--;
		if (start2 > 0) end2--;

		while (end1 > -1 || end2 > -1 || borrow > 1) {
			result = -borrow;
			if (end1 > -1) {
				result += Decimal::CharToDigit(this->_num[end1]); // diminutive
				end1--;
			}
			if (end2 > -1) {
				result -= Decimal::CharToDigit(other._num[end2]); // deductible
				end2--;
			}
			if (result < 0) {
				result += 10;
				borrow = 1;
			}
			else borrow = 0;
			intpart.push_back(Decimal::DigitToChar(result));
			
		}

		UInt64 size = intpart.size() + fracpart.size() + 1; // 1 for '\0'

		if (fracpart.size() > 0) size++; // for '.'
		char* num = new char[size];

		for (i = intpart.size() - 1; i > -1; i--, index++) num[index] = intpart[i];

		if (fracpart.size() > 0) {
			num[index] = '.';
			index++;
			for (i = fracpart.size() - 1; i > -1; i--, index++) num[index] = fracpart[i];
		}

		num[index] = '\0';

		return Decimal(num);
	}
}

bool Decimal::operator < (Decimal& other) noexcept {

	if (this->_num[0] == '-' && other._num[0] != '-') return true;			// first - negative, second - non negative
	else if (this->_num[0] != '-' && other._num[0] == '-') return false;	// first - non negative, second - negative
	else if (this->_num[0] != '-' && other._num[0] != '-') {				// both non negative
		UInt64 fraclen1 = 0;
		UInt64 fraclen2 = 0;
		Int64 dotpos1 = this->find('.');
		Int64 dotpos2 = other.find('.');

		if (dotpos1 != -1) fraclen1 = *this->_size - dotpos1 - 1;           //Checking float numbers for fraction part length
		if (dotpos2 != -1) fraclen2 = *other._size - dotpos2 - 1;

		if (*this->_size - fraclen1 > *other._size - fraclen2) return false;	// first number integer part bigger
		else if (*this->_size - fraclen1 < *other._size - fraclen2) return true;// second number integer part bigger
		else {																	// equals
			UInt64 start1 = 0;
			UInt64 start2 = 0;
			Int64 end1 = 0;
			Int64 end2 = 0;

			if (fraclen1 > fraclen2) {
				start1 = *this->_size - (fraclen1 - fraclen2) - 1;
				end1 = dotpos1;
				if (dotpos2 == -1) {
					end2 = *other._size - 1;
				}
				else {
					start2 = *other._size - 1;
					end2 = dotpos2;
				}
			}
			else if (fraclen1 < fraclen2) {
				start2 = *other._size - (fraclen2 - fraclen1) - 1;
				end2 = dotpos2;
				if (dotpos1 == -1) {
					end1 = *this->_size - 1;
				}
				else {
					start1 = *this->_size - 1;
					end1 = dotpos1;
				}
			}
			else if (fraclen1 == fraclen2 && fraclen1 != 0) {
				start1 = *this->_size - 1;
				end1 = dotpos1;
				start2 = *other._size - 1;
				end2 = dotpos2;
			}
			else {
				end1 = *this->_size - 1;
				end2 = *other._size - 1;
			}

			bool result = false;

			while (start1 > end1 && start1 > 0 && start2 > end2 && start2 > 0) {
				if (Decimal::CharToDigit(this->_num[start1]) < Decimal::CharToDigit(other._num[start2])) result = true;
				else if (Decimal::CharToDigit(this->_num[start1]) > Decimal::CharToDigit(other._num[start2])) result = false;
				start1--;
				start2--;
			}

			if (start1 > 0) end1--;
			if (start2 > 0) end2--;

			while (end1 > -1) {
				if (Decimal::CharToDigit(this->_num[end1]) < Decimal::CharToDigit(other._num[end2])) result = true;
				else if (Decimal::CharToDigit(this->_num[end1]) > Decimal::CharToDigit(other._num[end2])) result = false;
				end1--;
				end2--;
			}

			return result;
		}
	}
	else {			//both negative
		Decimal* absthis = new Decimal(Decimal::abs(*this));
		Decimal* absother = new Decimal(Decimal::abs(other));

		if (*absthis < *absother) {
			delete absthis;
			delete absother;
			return false;
		}
		delete absthis;
		delete absother;
		return true;
	}
}

bool Decimal::operator > (Decimal& other) noexcept {

	if (this->_num[0] == '-' && other._num[0] != '-') return false;			// first - negative, second - non negative
	else if (this->_num[0] != '-' && other._num[0] == '-') return true;		// first - non negative, second - negative
	else if (this->_num[0] != '-' && other._num[0] != '-') {				// both non negative
		UInt64 fraclen1 = 0;
		UInt64 fraclen2 = 0;
		Int64 dotpos1 = this->find('.');
		Int64 dotpos2 = other.find('.');
		UInt64 start1 = 0;
		UInt64 start2 = 0;
		Int64 end1 = 0;
		Int64 end2 = 0;
		if (dotpos1 != -1) fraclen1 = *this->_size - dotpos1 - 1; //Checking float numbers for fraction part length
		if (dotpos2 != -1) fraclen2 = *other._size - dotpos2 - 1;

		if (*this->_size - fraclen1 > *other._size - fraclen2) return true;	      // first number integer part bigger
		else if (*this->_size - fraclen1 < *other._size - fraclen2) return false; // second number integer part bigger
		else {																	  // equals

			if (fraclen1 > fraclen2) {
				start1 = *this->_size - (fraclen1 - fraclen2) - 1;
				end1 = dotpos1;
				if (dotpos2 == -1) {
					end2 = *other._size - 1;
				}
				else {
					start2 = *other._size - 1;
					end2 = dotpos2;
				}
			}
			else if (fraclen1 < fraclen2) {
				start2 = *other._size - (fraclen2 - fraclen1) - 1;
				end2 = dotpos2;
				if (dotpos1 == -1) {
					end1 = *this->_size - 1;
				}
				else {
					start1 = *this->_size - 1;
					end1 = dotpos1;
				}
			}
			else if (fraclen1 == fraclen2 && fraclen1 != 0) {
				start1 = *this->_size - 1;
				end1 = dotpos1;
				start2 = *other._size - 1;
				end2 = dotpos2;
			}
			else {
				end1 = *this->_size - 1;
				end2 = *other._size - 1;
			}

			bool result = false;

			while (start1 > end1 && start1 > 0 && start2 > end2 && start2 > 0) {
				if (Decimal::CharToDigit(this->_num[start1]) < Decimal::CharToDigit(other._num[start2])) result = false;
				else if (Decimal::CharToDigit(this->_num[start1]) > Decimal::CharToDigit(other._num[start2])) result = true;
				start1--;
				start2--;
			}

			if (start1 > 0) end1--;
			if (start2 > 0) end2--;

			while (end1 > -1) {
				if (Decimal::CharToDigit(this->_num[end1]) < Decimal::CharToDigit(other._num[end2])) result = false;
				else if (Decimal::CharToDigit(this->_num[end1]) > Decimal::CharToDigit(other._num[end2])) result = true;
				end1--;
				end2--;
			}

			return result;
		}
	}
	else {			//both negative
		Decimal* absthis = new Decimal(Decimal::abs(*this));
		Decimal* absother = new Decimal(Decimal::abs(other));

		if (*absthis > *absother) {
			delete absthis;
			delete absother;
			return false;
		}
		delete absthis;
		delete absother;
		return true;
	}
}

bool Decimal::operator == (Decimal& other) noexcept {
	if (*this->_size != *other._size) return false;
	for (UInt64 right = *this->_size - 1, left = 0; right >= left; left++, right--) {
		if (this->_num[left] != other._num[left] || this->_num[right] != other._num[right]) return false;
	}
	return true;
}

inline const Int16 Decimal::CharToDigit(const char& ch) noexcept {
	switch (ch)
	{
	case '0': return 0;
	case '1': return 1;
	case '2': return 2;
	case '3': return 3;
	case '4': return 4;
	case '5': return 5;
	case '6': return 6;
	case '7': return 7;
	case '8': return 8;
	case '9': return 9;
	default:
		return -1;
	}
}

inline const char Decimal::DigitToChar(const UInt16& digit) noexcept {
	switch (digit) {
	case 0: return '0';
	case 1: return '1';
	case 2: return '2';
	case 3: return '3';
	case 4: return '4';
	case 5: return '5';
	case 6: return '6';
	case 7: return '7';
	case 8: return '8';
	case 9: return '9';
	default:
		return ' ';
	}
}

inline const Int64 Decimal::find(const char& ch) noexcept {
	for (UInt64 left = 0, right = *this->_size - 1; right >= left; left++, right--) {
		if (this->_num[right] == ch) return right;
		else if (this->_num[left] == ch) return left;
	}
	return -1;
}
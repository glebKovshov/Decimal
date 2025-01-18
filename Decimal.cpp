#include "Decimal.h"

Decimal::Decimal(const char* num) {
	
	int64_t start_insertion = 0;

	if (num[0] == '-' && num[1] != '\0') {
		start_insertion++;
		_is_negative = true;
	}
	else if (Decimal::CharToDigit(num[0]) == -1) throw InvalidValue();

	while (num[start_insertion] == '0'  && num[start_insertion + 1] != '.' && num[start_insertion + 1] != '\0')
		start_insertion++;
	
	uint8_t dotcount = 0;

	while (num[_size + start_insertion] != '\0') {
		if (num[_size + start_insertion] == '.') {
			dotcount++;
			if (dotcount > 1) throw InvalidValue();
		}
		else if (Decimal::CharToDigit(num[_size + start_insertion]) == -1) throw InvalidValue();
		(_size)++;
	}

	if (dotcount > 0 && num[_size + start_insertion - 1] == '0') {
		for (int64_t i = _size + start_insertion - 1; num[i] == '0'; i--) {
			(_size)--;
		}
	}

	if (num[_size + start_insertion - 1] == '.') (_size)--;
	
	_num = new char[_size + 1];
	_num[_size] = '\0';

	for (int64_t i = 0; i < _size; i++) {
		_num[i] = num[i+start_insertion];
	}
}

Decimal::Decimal(const Decimal& other) {
	if (this->_num != nullptr) delete[] _num;

	this->_size = other._size;
	this->_num = new char[this->_size+1];
	if (other._is_negative) this->_is_negative = true;
	for (int64_t i = 0; i <= this->_size; i++) this->_num[i] = other._num[i];
}

Decimal::Decimal() {
	_num = new char[1];
	_num[0] = '\0';
}

Decimal::Decimal(const char* num, const int64_t size, bool _is_negative) noexcept {
	this->_size = size;
	if (this->_num != nullptr) delete[] this->_num;

	this->_num = new char[size + 1];
	this->_num[size] = '\0';
	if (_is_negative) this->_is_negative = true;

	for (int64_t i = 0; i < size; i++) _num[i] = num[i];
}

Decimal::~Decimal() {
	delete[] _num;
}

std::ostream& operator << (std::ostream& ostream, const Decimal& num) noexcept {
	if (num._is_negative) ostream << '-';
	ostream << num._num;
	return ostream;
}

std::istream& operator >> (std::istream& istream, Decimal& num) noexcept {
	std::vector<char> chars;
	for (char ch; istream.get(ch) && ch != '\n'; chars.push_back(ch));
	
	int size = chars.size();

	if (chars[0] == '-') {
		size--;
		num._is_negative = true;
	}

	delete[] num._num;
	num._num = new char[size + 1];

	num._size = size;

	for (int64_t i = 0; i < chars.size(); i++) num._num[i] = chars[i];
	num._num[size] = '\0';

	return istream;
}

const Decimal& Decimal::operator = (const Decimal& other) noexcept {
	if (this->_num != nullptr) delete[] this->_num;

	this->_size = other._size;
	this->_num = new char[this->_size+1];
	this->_num[this->_size] = '\0';
	this->_is_negative = other._is_negative;
	
	for (int64_t i = 0; i < this->_size; i++) this->_num[i] = other._num[i];
	return *this;
}

Decimal Decimal::abs(const Decimal& other) noexcept {
	if (!other._is_negative) return Decimal(other);
	char* num = new char[other._size - 1];
	for (int64_t i = 0; i <= other._size - 1; i++) num[i] = other._num[i + 1];
	return Decimal(num);
}

inline void int_pow(Decimal& result, Decimal& multipier, const uint32_t& n) {
	#pragma omp parallel for
	for (uint32_t i = 0; i < n; i++) {
		result = result * multipier;
	}
}

Decimal Decimal::pow(const uint32_t& n) noexcept
{
	Decimal result = Decimal("1", 1);
	if (n == 0) return result;

	
	if (n % 2 == 0) {
		int_pow(result, *this, n / 2);
		result = result * result;
	}
	else {
		int_pow(result, *this, n - 1);
		result = result * *this;
	}
	return result;
}

Decimal Decimal::IntToDecimal(int64_t num) noexcept
{
	Decimal temp;

	if (num == 0) {
		temp.AddDigit('0');
		return temp;
	}

	if (num < 0) {
		num = std::abs(num);
		temp._is_negative = true;
	}

	while (num > 0) {
		temp.AddDigit(Decimal::DigitToChar(num % 10));
		num /= 10;
	}

	return temp;
}

Decimal Decimal::addition(Decimal& other) noexcept {
	std::vector<char> fracpart;
	std::vector<char> intpart;
	uint8_t carryover = 0;
	int64_t fraclen1 = 0;
	int64_t fraclen2 = 0;
	int64_t start1 = 0;
	int64_t start2 = 0;
	int64_t end1 = 0;
	int64_t end2 = 0;
	uint8_t result = 0;
	int64_t index = 0;
	int64_t dotpos1 = this->find('.');
	int64_t dotpos2 = other.find('.');
	int64_t i = 0;

	if (dotpos1 != -1) fraclen1 = this->_size - dotpos1 - 1; //Checking float numbers for fraction part length
	if (dotpos2 != -1) fraclen2 = other._size - dotpos2 - 1;

	if (fraclen1 > fraclen2) {
		for (i = this->_size - 1; i > dotpos1 + fraclen2; i--) fracpart.push_back(this->_num[i]);
		start1 = this->_size - (fraclen1 - fraclen2) - 1;
		end1 = dotpos1;
		if (dotpos2 == -1) {
			end2 = other._size - 1;
		}
		else {
			start2 = other._size - 1;
			end2 = dotpos2;
		}
	}

	else if (fraclen1 < fraclen2) {
		for (i = other._size - 1; i > dotpos2 + fraclen1; i--) fracpart.push_back(other._num[i]);
		start2 = other._size - (fraclen2 - fraclen1) - 1;
		end2 = dotpos2;
		if (dotpos1 == -1) {
			end1 = this->_size - 1;
		}
		else {
			start1 = this->_size - 1;
			end1 = dotpos1;
		}
	}

	else if (fraclen1 == fraclen2 && fraclen1 != 0) {
		start1 = this->_size - 1;
		end1 = dotpos1;
		start2 = other._size - 1;
		end2 = dotpos2;
	}

	else {
		end1 = this->_size - 1;
		end2 = other._size - 1;
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
			result += Decimal::CharToDigit(this->_num[end1]);
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

	int64_t size = intpart.size() + fracpart.size(); // for '\0'

	if (fracpart.size() > 0) size++; // for '.'
	char* num = new char[size + 1];

	for (i = intpart.size() - 1; i > -1; i--, index++) num[index] = intpart[i];

	if (fracpart.size() > 0) {
		num[index] = '.';
		index++;
		for (i = fracpart.size() - 1; i > -1; i--, index++) num[index] = fracpart[i];
	}

	num[size] = '\0';

	return Decimal(num, size);
}

Decimal Decimal::operator +(Decimal other) noexcept {
	if (this->_is_negative && !other._is_negative) {      // -a + b = b - a;
		this->_is_negative = false;
		return other - *this;
	}
	else if (!this->_is_negative && other._is_negative) { // a + (-b) = a - b
		other._is_negative = false;
		return (*this).subtraction(other);
	}
	else if (this->_num[0] == '-' && other._num[0] == '-') { // -a + (-b) = -a - b = -(a+b)
		Decimal result = this->Decimal::addition(other);
		result._is_negative = true;
		
		return result;
	}
	else return this->Decimal::addition(other);
}

void Decimal::operator += (Decimal& other) noexcept
{
	*this = this->Decimal::addition(other);
}

void Decimal::operator-=(Decimal& other) noexcept
{
	*this = this->Decimal::subtraction(other);
}

Decimal Decimal::subtraction(Decimal& other) noexcept {
	std::vector<char> fracpart;
	std::vector<char> intpart;
	uint8_t borrow = 0;
	int64_t fraclen1 = 0;
	int64_t fraclen2 = 0;
	int64_t start1 = 0;
	int64_t start2 = 0;
	int64_t end1 = 0;
	int64_t end2 = 0;
	int8_t result = 0;
	int64_t index = 0;
	int64_t dotpos1 = this->find('.');
	int64_t dotpos2 = other.find('.');
	int64_t i = 0;

	if (dotpos1 != -1) fraclen1 = this->_size - dotpos1 - 1; // Checking float numbers for fraction part length
	if (dotpos2 != -1) fraclen2 = other._size - dotpos2 - 1;

	if (fraclen1 > fraclen2) {								  // pushing firts num extra digits and define start and end position for each nums
		for (i = this->_size - 1; i > dotpos1 + fraclen2; i--) fracpart.push_back(this->_num[i]);
		start1 = this->_size - (fraclen1 - fraclen2) - 1;
		end1 = dotpos1;
		if (dotpos2 == -1) {
			end2 = other._size - 1;
		}
		else {
			start2 = other._size - 1;
			end2 = dotpos2;
		}
	}

	else if (fraclen1 < fraclen2) {
		fracpart.push_back(DigitToChar(10 - CharToDigit(other._num[(other._size - 1)]))); // first time always will be 10 - second num last digit. Then always will be (9 - second num digit) while fraclen1 < fraclen2
		borrow = 1;
		for (i = other._size - 2; i > dotpos2 + fraclen1; i--) fracpart.push_back(DigitToChar(10 - borrow - CharToDigit(other._num[i])));
		start2 = other._size - (fraclen2 - fraclen1) - 1;
		end2 = dotpos2;
		if (dotpos1 == -1) {
			end1 = this->_size - 1;
		}
		else {
			start1 = this->_size - 1;
			end1 = dotpos1;
		}
	}

	else if (fraclen1 == fraclen2 && fraclen1 != 0) {
		start1 = this->_size - 1;
		end1 = dotpos1;
		start2 = other._size - 1;
		end2 = dotpos2;
	}

	else {
		end1 = this->_size - 1;
		end2 = other._size - 1;
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

	int64_t size = intpart.size() + fracpart.size() + 1; // 1 for '\0'

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

Decimal Decimal::operator - (Decimal other) noexcept {

	if (other._is_negative && !this->_is_negative) { // a - (-b) = a + b
		other._is_negative = false;
		return this->addition(other);
	}

	else if (this->_is_negative && other._is_negative) { // -a - b = -(a+b)
		this->_is_negative = false;
		other._is_negative = false;
		Decimal result = this->addition(other);
		result._is_negative = true;

		return result;
	}

	else if (this->_is_negative && other._is_negative) { // -a - (-b) = b - a
		other._is_negative = false;
		return other.subtraction(*this);
	}
	else if (other > *this) {						   // a - b = -(b-a) : b > a
		Decimal result = other.subtraction(*this);
		result._is_negative = true;

		return result;
	}

	else {											  // a - b: a > b
		return this->subtraction(other);
	}
}

Decimal Decimal::operator * (Decimal& other) noexcept
{
	if (other._num[0] == '0' && other._size == 1 || this->_num[0] == '0' && this->_size == 1) return Decimal("0", 1); // a * 0 = 0
	else if (other._num[0] == '1' && other._size == 1) return Decimal(*this);										  // a * 1 = a
	else if (this->_num[0] == '1' && this->_size == 1) return Decimal(other);										  // 1 * b = b

	Decimal sum("0", 1);
	Decimal zero("0", 1);
	int64_t second_digit_capacity = other._size - 1;
	uint8_t carryover = 0;
	std::vector<char> line_result;
	uint8_t result;
	Decimal temp;
	int64_t dotpos1 = this->find('.');
	int64_t dotpos2 = other.find('.');
	int64_t decimal_place = 0; // signs after '.'
	int64_t intpart = 0;
	int64_t fracpart = 0;

	if (dotpos1 != -1) decimal_place += this->_size - dotpos1 - 1;
	if (dotpos2 != -1) {
		second_digit_capacity--;
		decimal_place += other._size - dotpos2 - 1;
	}

	for (int64_t index_other = 0, other_size = other._size; index_other < other_size; index_other++, second_digit_capacity--) {
		if (other._num[index_other] == '.') {
			second_digit_capacity++; // avoid unwanted decrement
			continue;
		}

		if (other._num[index_other] == '0' && sum > zero) continue;

		for (int64_t i = 0; i < second_digit_capacity; i++) line_result.push_back('0');
		
		for (int64_t index_this = this->_size - 1; index_this > -1; index_this--){
			if (this->_num[index_this] == '.') continue;

			result = CharToDigit(this->_num[index_this]) * CharToDigit(other._num[index_other]) + carryover;
			carryover = result / 10;
			line_result.push_back(DigitToChar(result%10));

		}
		if (carryover > 0) {
			line_result.push_back(DigitToChar(carryover));
			carryover = 0;
		}
		temp._size = line_result.size();
		temp._num = new char[temp._size + 1];

		for (int64_t index_vec = line_result.size() - 1, index_ch = 0; index_vec > -1; index_vec--, index_ch++) temp._num[index_ch] = line_result[index_vec];
		temp._num[temp._size] = '\0';

		line_result = {};
		sum += temp;
	}
	int64_t size = sum._size;
	intpart = size - decimal_place;

	if (decimal_place > 0) {
		fracpart = decimal_place;
		// Checking zeros after '.'
		for (int64_t i = 0; i < decimal_place; i++) {
			if (sum._num[sum._size - i - 1] == '0') {
				fracpart--;
				size--;
			}
			else break;
		}
	}
	char* num = new char[size + 1];

	if (fracpart > 0) {
		size++; // for '.'
		for (int64_t i = 0; i < intpart; i++) num[i] = sum._num[i];
		num[size - fracpart - 1] = '.';
		for (int64_t i = size - fracpart; i < size; i++) num[i] = sum._num[i - 1];
	}

	else for (int64_t i = 0; i < size; i++) num[i] = sum._num[i];
	num[size] = '\0';

	return Decimal(num);
}

inline void Decimal::AddDigit(const char& digit) noexcept {
	this->_size++;

	char* result = new char[this->_size + 1];
	result[this->_size - 1] = digit;
	result[this->_size] = '\0';

	for (int64_t i = 0; i < this->_size-1; i++) result[i] = this->_num[i];

	delete[] this->_num;
	this->_num = new char[this->_size + 1];
	for (int64_t i = 0; i <= this->_size; i++) this->_num[i] = result[i];
	delete[] result;
}

Decimal Decimal::operator / (Decimal& other)
{
	if (other == 0) throw InvalidValue();
	
	Decimal ten("10", 2);
	Decimal divisible = *this;
	Decimal divisor = other;

	while (divisor.find('.') != -1) {
		divisible = divisible * ten;
		divisor = divisor * ten;
	}

	if (divisor == 1) return Decimal(divisible);	 // a / 1 = a
	else if (divisible == 0) return Decimal("0", 1); // 0 / a = 0
	
	bool contain_dot = false;
	uint8_t quotient = 0;
	int64_t start = 1;
	Decimal divpart = Decimal(divisible._num, start);
	std::vector<char> result;

	while (divpart < divisor && start < divisible._size) {
		if (divisible._num[start] != '.') divpart.AddDigit(divisible._num[start]);
		else {
			result.push_back('0');
			result.push_back('.');
			contain_dot = true;
		}
		start++;
	}
	
	if (divisible < divisor && !contain_dot) {
		result.push_back('0');
		result.push_back('.');
		contain_dot = true;
		divpart.AddDigit('0');
		while (divpart < divisor) {
			divpart.AddDigit('0');
			result.push_back('0');
		}
	}
	
	Decimal mod;
	
	for (int64_t i = start, size = divisible._size + precision; i < size;) {
		quotient = 0;
		mod = divpart;

		while (mod >= divisor) {
			mod -= divisor;
			quotient++;
		}

		divpart = mod;

		result.push_back(Decimal::DigitToChar(quotient));

		if (divpart._num[0] == '0' && divpart._size == 1) {
			if (i >= divisible._size) break;
			delete[] divpart._num;
			divpart._num = new char[1];
			divpart._num[0] = '\0';
			divpart._size = 0;
		}

		if (divpart < divisor && i < divisible._size) {
			if (divisible._num[i] == '.' && !contain_dot) {
				result.push_back('.');
				contain_dot = true;
				i++;
			}
			divpart.AddDigit(divisible._num[i]);
			i++;
			while (divpart < divisor && i < divisible._size) {
				if (divisible._num[i] != '.') divpart.AddDigit(divisible._num[i]);
				else if (!contain_dot) {
					result.push_back('.');
					contain_dot = true;
				}
				result.push_back('0');
				i++;
			}
		}

		if (i >= divisible._size && divpart < divisor) {
			if (!contain_dot) {
				result.push_back('.');
				contain_dot = true;
			}
			divpart.AddDigit('0');
			i++;
			while (divpart < divisor && i < size) {
				divpart.AddDigit('0');
				result.push_back('0');
				i++;
			}
		}

	}
	
	char* temp = new char[result.size() + 1];
	temp[result.size()] = '\0';

	for (int64_t i = 0; i < result.size(); i++) temp[i] = result[i];

	return Decimal(temp, result.size());
	
}

bool Decimal::operator < (Decimal& other) noexcept {

	if (this->_num[0] == '-' && other._num[0] != '-') return true;			// first - negative, second - non negative
	else if (this->_num[0] != '-' && other._num[0] == '-') return false;	// first - non negative, second - negative
	else if (this->_num[0] != '-' && other._num[0] == '-') {			    //both negative
		Decimal absthis = Decimal::abs(*this);
		Decimal absother = Decimal::abs(other);

		if (absthis < absother) return false;
		return true;
	}
	else {				                                                    // both non negative
		int64_t fraclen1 = 0;
		int64_t fraclen2 = 0;
		int64_t dotpos1 = this->find('.');
		int64_t dotpos2 = other.find('.');
		int64_t start1 = 0;
		int64_t start2 = 0;
		int64_t end1 = 0;
		int64_t end2 = 0;
		bool result = false;
		if (dotpos1 != -1) fraclen1 = this->_size - dotpos1; 
		if (dotpos2 != -1) fraclen2 = other._size - dotpos2;

		if (this->_size - fraclen1 < other._size - fraclen2) return true;		// int part of the second num bigger than the first
		else if (this->_size - fraclen1 > other._size - fraclen2) return false;	// int part of the first num bigger than the second
																				// equals by int part size
		if (fraclen1 > fraclen2) {
			start1 = this->_size - (fraclen1 - fraclen2) - 2;
			end1 = dotpos1;
			if (dotpos2 == -1) {
				end2 = other._size - 1;
			}
			else {
				start2 = other._size - 1;
				end2 = dotpos2;
			}
		}
		else if (fraclen1 < fraclen2) {
			result = true;
			start2 = other._size - (fraclen2 - fraclen1) - 2;
			end2 = dotpos2;
			if (dotpos1 == -1) {
				end1 = this->_size - 1;
			}
			else {
				start1 = this->_size - 1;
				end1 = dotpos1;
			}
		}
		else if (fraclen1 == fraclen2 && fraclen1 != 0) {
			start1 = this->_size - 1;
			end1 = dotpos1;
			start2 = other._size - 1;
			end2 = dotpos2;
		}
		else {
			end1 = this->_size - 1;
			end2 = other._size - 1;
		}

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

bool Decimal::operator < (int64_t num) noexcept
{
	Decimal temp = Decimal::IntToDecimal(num);
	if (*this < temp) return true;
	return false;
}

bool Decimal::operator > (Decimal& other) noexcept {

	if (this->_is_negative && !other._is_negative) return false;			// first - negative, second - non negative
	else if (!this->_is_negative && other._is_negative) return true;		// first - non negative, second - negative
	else if (this->_is_negative && other._is_negative) {					// both negative
		if (*this > other) return false;
		return true;
	}
	else {				                                                    // both non negative
		int64_t fraclen1 = 0;
		int64_t fraclen2 = 0;
		int64_t dotpos1 = this->find('.');
		int64_t dotpos2 = other.find('.');
		int64_t start1 = 0;
		int64_t start2 = 0;
		int64_t end1 = 0;
		int64_t end2 = 0;
		bool result = false;
		if (dotpos1 != -1) fraclen1 = this->_size - dotpos1;
		if (dotpos2 != -1) fraclen2 = other._size - dotpos2;

		if (this->_size - fraclen1 > other._size - fraclen2) return true;		// int part of first num bigger than second
		else if (this->_size - fraclen1 < other._size - fraclen2) return false; // int part of second num bigger than first
																				// equals by int part size
		if (fraclen1 > 0) fraclen1--;
		if (fraclen2 > 0) fraclen2--;

 		if (fraclen1 > fraclen2) {
			result = true;
			start1 = this->_size - (fraclen1 - fraclen2) - 1;
			end1 = dotpos1;
			if (dotpos2 == -1) {
				end2 = other._size - 1;
			}
			else {
				start2 = other._size - 1;
				end2 = dotpos2;
			}
		}
		else if (fraclen1 < fraclen2) {
			start2 = other._size - (fraclen2 - fraclen1) - 1;
			end2 = dotpos2;
			if (dotpos1 == -1) {
				end1 = this->_size - 1;
			}
			else {
				start1 = this->_size - 1;
				end1 = dotpos1;
			}
		}
		else if (fraclen1 == fraclen2 && fraclen1 != 0) {
			start1 = this->_size - 1;
			end1 = dotpos1;
			start2 = other._size - 1;
			end2 = dotpos2;
		}
		else {
			end1 = this->_size - 1;
			end2 = other._size - 1;
		}

		while (start1 > end1 && start1 > 0 && start2 > end2 && start2 > 0) {
			if (Decimal::CharToDigit(this->_num[start1]) > Decimal::CharToDigit(other._num[start2])) result = true;
			else if ((Decimal::CharToDigit(this->_num[start1]) < Decimal::CharToDigit(other._num[start2]))) result = false;
			start1--;
			start2--;
		}

		if (start1 > 0) end1--;
		if (start2 > 0) end2--;

		while (end1 > -1 || end2 > -1) {
			if (Decimal::CharToDigit(this->_num[end1]) > Decimal::CharToDigit(other._num[end2])) result = true;
			else if ((Decimal::CharToDigit(this->_num[end1]) < Decimal::CharToDigit(other._num[end2]))) result = false;
			end1--;
			end2--;
		}

		return result;
	}
	
}

bool Decimal::operator>(int64_t num) noexcept
{
	Decimal temp = Decimal::IntToDecimal(num);
	if (*this > temp) return true;
	return false;
}

bool Decimal::operator == (Decimal& other) noexcept {
	if (this->_size != other._size) return false;
	for (int64_t right = this->_size - 1, left = 0; right >= left; left++, right--) {
		if (this->_num[left] != other._num[left] || this->_num[right] != other._num[right]) return false;
	}
	return true;
}

inline bool Decimal::operator == (int64_t num) noexcept
{
	Decimal temp = Decimal::IntToDecimal(num);
	if (*this == temp) return true;
	return false;
}

inline bool Decimal::operator >= (Decimal& other) noexcept
{
	if (*this > other || *this == other) return true;
	return false;
}

inline constexpr int8_t Decimal::CharToDigit(const char& ch) noexcept {
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

inline constexpr char Decimal::DigitToChar(const uint8_t& digit) noexcept {
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

inline constexpr int64_t Decimal::find(const char& ch) const noexcept {
	int64_t left = 0, right = this->_size - 1;
	while (right >= left ) {
		if (this->_num[right] == ch) return right;
		else if (this->_num[left] == ch) return left;
		left++; 
		if (right > 0) right--;
		else break;
	}
	return -1;
}
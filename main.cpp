#include "Decimal.h"
#include <chrono>

int main() {
	auto start = std::chrono::high_resolution_clock::now();

	Decimal num1 = "-11011.63";
	Decimal num2 = "-11211.9831";

	std::cout << num1 <<" > " << num2 << " " << std::boolalpha << (num1 > num2) << std::endl;

	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> duration = end - start;
	std::cout << "Executing time: " << duration.count() << " sec" << std::endl;
	
}
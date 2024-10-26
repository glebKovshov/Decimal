#include "Decimal.h"
#include <chrono>

int main() {
	auto start = std::chrono::high_resolution_clock::now();

	Decimal num1 = "108746234632.3981723", num2 = "102131.9317293713";

	Decimal num3 = num1 + num2;
	std::cout << num1 << " + " << num2 << " = " << num3 << std::endl;

	std::cout << num1 << " > " << num2 << " = " << std::boolalpha << (num1 > num2) << std::endl;

	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> duration = end - start;
	std::cout << "Executing time: " << duration.count() << " sec" << std::endl;
}
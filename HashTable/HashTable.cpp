#include <HashTable.h>

int CharArrayToASCIICode(const char* num) {
	int result = 0;
	for (int i = 0; num[i] != '\0'; i++) {
		result += num[i];
	}
	return result;
}

void HashTable::add(const char* num1, const char* num2, const char* result) {
	int hashValue;
	if (amount == buckets) {
		buckets *= 2;
		std::list<std::pair<std::pair<const char*, const char*>, const char*>>* temp = new std::list<std::pair<std::pair<const char*, const char*>, const char*>>[buckets];
		for (int i = 0; i < buckets/2; i++) {
			for (auto& pair : table[i]) {
				hashValue = (CharArrayToASCIICode(pair.first.first) + CharArrayToASCIICode(pair.first.second)) % buckets;
				temp[hashValue].push_back(pair);
			}
		}
		delete[] table;
		table = temp;
	}
	hashValue = hashFunction(num1, num2);
	std::pair<const char*, const char*> key = std::make_pair(num1, num2);
	table[hashValue].push_back(std::make_pair(key, result));
	amount++;
}

bool HashTable::contains(const char* num1, const char* num2) const {
	int hashValue = hashFunction(num1, num2);
	auto& cell = table[hashValue];
	auto iter = begin(cell);
	while (iter != end(cell)) {
		if (iter->first == std::make_pair(num1, num2)) return true;
		iter++;
	}
	return false;
}

int HashTable::hashFunction(const char* num1, const char* num2) const {
	return (CharArrayToASCIICode(num1) + CharArrayToASCIICode(num2)) % buckets;
}

const char* HashTable::get(const char* num1, const char* num2) const {

	int hashValue = hashFunction(num1, num2);
	auto key = std::make_pair(num1, num2);
	for (auto& pair : table[hashValue]) {
		if (pair.first == key) return pair.second;
	}
	return nullptr;
}
#include <iostream>
#include <utility>
#include <list>

class HashTable {
private:
	int buckets = 5;
	int amount = 0;
	std::list<std::pair<std::pair<const char*, const char*>, const char*>>* table = new std::list<std::pair<std::pair<const char*, const char*>, const char*>>[buckets];
	int hashFunction(const char* num1, const char* num2) const;

public:
	bool contains(const char* num1, const char* num2) const;
	void add(const char* num1, const char* num2, const char* result);
	const char* get(const char* num1, const char* num2) const;
};
#include <iostream>
#include "Allocator.h"

using namespace std;

void fillBlock(void* start, int size, int filler) {
	for (int i = 0; i < size; i++) {
		*((int*)start + 1) = filler;
	}
}

void test() {
	cout << "Тест запущен \n";
	const int n = 2000;
	const int calls = 5;
	const int bSize = 50;
	Allocator al = Allocator(n);
	void* curBlock;
	void* mas[calls];
	for (int i = 0; i < calls; i++) {
		curBlock = al.mem_alloc(bSize);
		mas[i] = curBlock;
		fillBlock(curBlock, 50, 170);
	}
	cout << "Аллокация 5 блоков \n";
	al.mem_dump();
	cout << "Перевыделение памяти для 3 блока (20 байт)\n";
	al.mem_realloc(mas[3], 20);
	al.mem_dump();
	cout << "Перевыделение памяти для 3 блока (30 байт)\n";
	al.mem_realloc(mas[3], 30);
	al.mem_dump();
	cout << "Перевыделение памяти для 3 блока (50 байт)\n";
	al.mem_realloc(mas[3], 50);
	al.mem_dump();
	cout << "Высвобождение памяти 2 и 4 элемента \n";
	al.mem_free(mas[2]);
	al.mem_free(mas[4]);
	al.mem_dump();
	cout << "Тест успешно завершен \n";
}

int main() {
	setlocale(LC_ALL, "Russian");
	test();
	getchar();
	return 0;
}

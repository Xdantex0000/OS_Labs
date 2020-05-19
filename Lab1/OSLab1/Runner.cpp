#include <iostream>
#include "Allocator.h"

using namespace std;

void fillBlock(void* start, int size, int filler) {
	for (int i = 0; i < size; i++) {
		*((int*)start + 1) = filler;
	}
}

void test() {
	cout << "���� ������� \n";
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
	cout << "��������� 5 ������ \n";
	al.mem_dump();
	cout << "������������� ������ ��� 3 ����� (20 ����)\n";
	al.mem_realloc(mas[3], 20);
	al.mem_dump();
	cout << "������������� ������ ��� 3 ����� (30 ����)\n";
	al.mem_realloc(mas[3], 30);
	al.mem_dump();
	cout << "������������� ������ ��� 3 ����� (50 ����)\n";
	al.mem_realloc(mas[3], 50);
	al.mem_dump();
	cout << "������������� ������ 2 � 4 �������� \n";
	al.mem_free(mas[2]);
	al.mem_free(mas[4]);
	al.mem_dump();
	cout << "���� ������� �������� \n";
}

int main() {
	setlocale(LC_ALL, "Russian");
	test();
	getchar();
	return 0;
}

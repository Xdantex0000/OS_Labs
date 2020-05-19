#include "stdafx.h"
#include "Allocator.h"
#include "Test.h"
#include <iostream>
#include <stdlib.h>
#include <ctime>

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	setlocale(LC_ALL, "Russian");
	const size_t pageCount = 128;
	Allocator allocator(pageCount);
	Test Test(&allocator);
	void* ptrs[1000];
	size_t ptrsCount = 0;
	int a = -1;
	while (a != 0)
	{
		cout << "�������� ��������:\n";
		cout << "1 - ���� ����������\n";
		cout << "2 - �������� ������\n";
		if (ptrsCount != 0)
		{
			cout << "3 - ����������� ������\n";
			cout << "4 - �������������� ������\n";
		}
		cout << "0 - �����\n";
		cout << "> ";
		cin >> a;
		if (a == 1) {
			Test.test(ptrs, &ptrsCount);
		}
		if (a == 2) {
			size_t size;
			cout << "������� ���������� ������.\n";
			cout << "> ";
			cin >> size;
			clock_t start = clock();
			void* ptr = allocator.mem_alloc(size);
			clock_t end = clock();
			cout << "Time: " << end - start;
			if (ptr != NULL)
			{
				ptrs[ptrsCount++] = ptr;
			}
			cout << "ptr = ";
			cout << ptr;
			cout << "\n";
		}
		if (a == 3) {
			cout << "�������� ���� ������ ��� �������������:\n";
			for (size_t i = 0; i < ptrsCount; i++)
			{
				cout << i;
				cout << " - ";
				cout << ptrs[i];
				cout << "\n";
			}
			cout << "> ";
			size_t n;
			cin >> n;
			if (n < ptrsCount)
			{
				allocator.mem_free(ptrs[n]);
				for (size_t i = n + 1; i < ptrsCount; i++)
				{
					ptrs[i - 1] = ptrs[i];
				}
				ptrsCount--;
			}
		}
		if (a == 4) {
			cout << "�������� ���� ������ ��� ���������������:\n";
			for (size_t i = 0; i < ptrsCount; i++)
			{
				cout << i;
				cout << " - ";
				cout << ptrs[i];
				cout << "\n";
			}
			cout << "> ";
			size_t n;
			cin >> n;
			if (n < ptrsCount)
			{
				size_t size;
				cout << "������� ���-�� ������ ��� ���������������.\n";
				cout << "> ";
				cin >> size;
				void* ptr = allocator.mem_realloc(ptrs[n], size);
				if (ptr != NULL)
				{
					ptrs[n] = ptr;
				}
				cout << "ptr = ";
				cout << ptr;
				cout << "\n";
			}
		}
		cin.get();
	}
	return 0;
}

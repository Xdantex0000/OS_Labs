#include <Windows.h>
#include <iostream>
#include "Allocator.h"

using namespace std;
Allocator::Allocator(const int n) {
	int* mas = new int[n + 1];
	N = n;
	bSize = sizeof(BlockHeader) / sizeof(int);
	begin = (BlockHeader*)(&mas[0]);

	begin->prevsize = NULL;
	begin->size = &mas[n] - &mas[0] - bSize;
	begin->state = false;

	endOfMemory = &mas[n];
}

void* Allocator::mem_alloc(size_t size) {
	BlockHeader* current = begin;
	while (current->size < size || current->state) {
		if (isLast(current))
			return NULL;
		current = nextBlockHeader(current);
	}
	if ((current->size == size) || ((current->size - size) < bSize)) {
		current->state = 1;
		return getBlock(current);
	}
	else {
		return separateOnUseAndFree(current, size);
	}
}

void* Allocator::mem_realloc(void* addr, size_t size) {
	if (addr == NULL) {
		return mem_alloc(size);
	}

	BlockHeader* current = (BlockHeader*)addr - 1;
	size_t deltaSize = current->size - size;

	if (deltaSize == 0)
		return addr;

	BlockHeader* previous = previousBlockHeader(current);
	BlockHeader* next = nextBlockHeader(current);

	if (previous == NULL)
		if (next == NULL) {
			return searchNewBlock(addr, size);
		}
		else
			if (next->state)
				return searchNewBlock(addr, size);
			else
				return expandRight(addr, size);
	else
		if (previous->state)
			if (next == NULL)
				return searchNewBlock(addr, size);
			else
				if (next->state)
					return searchNewBlock(addr, size);
				else
					return expandRight(addr, size);
		else
			if (next == NULL)
				return expandLeft(addr, size);
			else
				if (next->state)
					return expandLeft(addr, size);
				else
					return expandBoth(addr, size);

	return NULL;
}

void Allocator::mem_free(void* addr) {
	BlockHeader* current = (BlockHeader*)addr - 1;
	BlockHeader* next = nextBlockHeader(current);
	BlockHeader* previous = previousBlockHeader(current);


	if (previous == NULL)
		if (next == NULL)
			current->state = false;
		else
			if (next->state)
				current->state = false;
			else
				mergeWithNext(current, next);
	else
		if (previous->state)
			if (next == NULL)
				current->state = false;
			else
				if (next->state)
					current->state = false;
				else
					mergeWithNext(current, next);
		else
			if (next == NULL)
				mergeWithPrevious(previous, current);
			else
				if (next->state)
					mergeWithPrevious(previous, current, next);
				else
					mergeBoth(previous, current, next);
}

BlockHeader* Allocator::nextBlockHeader(BlockHeader* current) {
	if (isLast(current))
		return NULL;

	return (BlockHeader*)((int*)((char*)current + sizeof(BlockHeader)) + current->size);
}

BlockHeader* Allocator::previousBlockHeader(BlockHeader* current) {
	if (current->prevsize == NULL)
		return NULL;
	return (BlockHeader*)((int*)((char*)current - sizeof(BlockHeader)) - current->prevsize);
}

bool Allocator::isLast(BlockHeader* h) {
	if (((int*)((char*)h + sizeof(BlockHeader)) + h->size) == endOfMemory)
		return true;
	else
		return false;
}

void Allocator::mergeWithNext(BlockHeader* current, BlockHeader* next) {
	current->size += next->size + sizeof(BlockHeader) / sizeof(int);
	current->state = false;
	BlockHeader* next2 = nextBlockHeader(next);
	if (next2 != NULL)
		next2->prevsize = current->size;
}

void Allocator::mergeWithPrevious(BlockHeader* previous, BlockHeader* current) {
	previous->size += current->size + sizeof(BlockHeader) / sizeof(int);
}

void Allocator::mergeWithPrevious(BlockHeader* previous, BlockHeader* current, BlockHeader* next) {
	previous->size += current->size + sizeof(BlockHeader) / sizeof(int);
	next->prevsize = previous->size;
}

void Allocator::mergeBoth(BlockHeader* previous, BlockHeader* current, BlockHeader* next) {
	previous->size += current->size + next->size + 2 * sizeof(BlockHeader) / sizeof(int);
	BlockHeader* next2 = nextBlockHeader(next);
	if (next2 != NULL)
		next2->prevsize = previous->size;
}

void Allocator::copyData(void* from, void* to, size_t quantity) {
	int* f = (int*)from;
	int* t = (int*)to;
	for (unsigned int i = 0; i < quantity; i++)
		t[i] = f[i];
}

void* Allocator::searchNewBlock(void* addr, size_t size) {

	BlockHeader* current = (BlockHeader*)addr - 1;
	size_t deltaSize = current->size - size;

	if ((deltaSize > 0) && (deltaSize < 3)) {
		return addr;
	}

	if (deltaSize > 0) {
		return separateOnUseAndFree((BlockHeader*)addr - 1, size);
	}
	else {
		void* nBlock = mem_alloc(size);
		if (nBlock != NULL) {
			copyData(addr, nBlock, current->size);
			mem_free(addr);
			return nBlock;
		}
		return NULL;
	}
}


void* Allocator::expandLeft(void* addr, size_t size) {

	BlockHeader* current = (BlockHeader*)addr - 1;
	BlockHeader* previous = previousBlockHeader(current);
	size_t area = current->size + current->prevsize + sizeof(BlockHeader) / sizeof(int);
	size_t deltaSize = current->size - size;

	if (deltaSize > 0) {
		int* first = (int*)addr;
		for (int i = size - 1; i >= 0; i--) {
			first[i + deltaSize] = first[i];
		}
		previous->size += deltaSize;

		current = nextBlockHeader(previous);
		initBlockHeader(current, true, previous->size, size, 7);

		BlockHeader* next = nextBlockHeader(current);
		if (next != NULL)
			next->prevsize = current->size;

		return getBlock(current);
	}
	else {
		if (area >= size)
			if ((area == size) || ((area - size) < 3)) {
				previous->size = area;
				BlockHeader* next = nextBlockHeader(current);
				if (next != NULL)
					next->prevsize = previous->size;
				previous->state = true;
				copyData(addr, getBlock(previous), current->size);
				return getBlock(previous);
			}
			else {
				previous->size = size;
				previous->state = true;
				copyData(addr, getBlock(previous), current->size);

				current = nextBlockHeader(previous);
				current->size = area - size - sizeof(BlockHeader) / sizeof(int);
				current->state = false;
				current->prevsize = previous->size;
				BlockHeader* next = nextBlockHeader(current);
				if (next != NULL)
					next->prevsize = current->size;
				return getBlock(previous);
			}
		else {
			void* p = mem_alloc(size);
			if (p != NULL) {
				copyData(addr, p, current->size);
				BlockHeader* next = nextBlockHeader(current);
				if (next == NULL)
					mergeWithPrevious(previous, current);
				else
					mergeWithPrevious(previous, current, next);
				return p;
			}
		}
	}
	return NULL;
}
void* Allocator::expandRight(void* addr, size_t size) {
	BlockHeader* current = (BlockHeader*)addr - 1;
	BlockHeader* next = nextBlockHeader(current);
	size_t area = current->size + next->size + sizeof(BlockHeader) / sizeof(int);
	int deltaSize = (int)(current->size) - (int)(size);

	if (deltaSize > 0) { 	
		current->size = size;
		next = nextBlockHeader(current);
		initBlockHeader(next, false, current->size, area - size - sizeof(BlockHeader) / sizeof(int), 7);
		BlockHeader* next2 = nextBlockHeader(next);
		if (next2 != NULL)
			next2->prevsize = next->size;
		return getBlock(current);
	}
	else {
		if (area >= size)
			if ((area - size) < 3) {
				current->size = area;
				next = nextBlockHeader(current);
				if (next != NULL)
					next->prevsize = current->size;
				return getBlock(current);
			}
			else {
				current->size = size;

				next = nextBlockHeader(current);
				next->size = area - size - sizeof(BlockHeader) / sizeof(int);
				next->prevsize = size;
				next->state = false;
				BlockHeader* next2 = nextBlockHeader(next);
				if (next2 != NULL)
					next2->prevsize = next->size;
				return getBlock(current);
			}
		else {
			void* p = mem_alloc(size);
			if (p != NULL) {
				copyData(addr, p, current->size);
				next = nextBlockHeader(current);
				mergeWithNext(current, next);
				return p;
			}
		}
	}
	return NULL;
}
void* Allocator::expandBoth(void* addr, size_t size) {

	BlockHeader* current = (BlockHeader*)addr - 1;
	BlockHeader* previous = previousBlockHeader(current);
	BlockHeader* next = nextBlockHeader(current);
	size_t area = current->size + previous->size + next->size + 2 * sizeof(BlockHeader) / sizeof(int);
	int deltaSize = (int)(current->size) - (int)(size);

	if (deltaSize > 0) {
		mergeBoth(previous, current, next);
		copyData(addr, getBlock(previous), size);
		return separateOnUseAndFree(previous, size);
	}
	else {
		if (area >= size)
			if ((area == size) || ((area - size) < 3)) {
				previous->size = area;
				BlockHeader* next2 = nextBlockHeader(next);
				if (next2 != NULL)
					next2->prevsize = previous->size;
				previous->state = true;
				copyData(addr, getBlock(previous), current->size);
				return getBlock(previous);
			}
			else {
				mergeBoth(previous, current, next);
				copyData(addr, getBlock(previous), current->size);
				return separateOnUseAndFree(previous, size);
			}
		else {
			void* p = mem_alloc(size);
			if (p != NULL) {
				copyData(addr, p, current->size);
				mergeBoth(previous, current, next);
				return p;
			}
		}
	}
	return NULL;
}

void Allocator::initBlockHeader(BlockHeader* bh, bool state, size_t previous, size_t size, int mask) {
	if (bh == NULL)
		return;
	if (mask & 4) {
		bh->state = state;
	}
	if (mask & 2) {
		bh->prevsize = previous;
	}
	if (mask & 1)
		bh->size = size;
}

void* Allocator::separateOnUseAndFree(BlockHeader* current, size_t size) {
	size_t curSize = current->size;
	BlockHeader* next = nextBlockHeader(current);
	if (next == NULL) {
		initBlockHeader(current, true, NULL, size, 5);
		BlockHeader* next1 = nextBlockHeader(current);
		initBlockHeader(next1, false, size, (curSize - size - bSize), 7);
		return getBlock(current);
	}
	else {
		initBlockHeader(current, true, NULL, size, 5);
		BlockHeader* next1 = nextBlockHeader(current);
		initBlockHeader(next1, false, size, (curSize - size - bSize), 7);
		next->prevsize = next1->size;
		return getBlock(current);
	}
}

void* Allocator::getBlock(BlockHeader* h) {
	return (void*)(h + 1);
}

void Allocator::mem_dump() {
	BlockHeader* current = begin;
	int i = 0;
	while (current != NULL) {
		cout << i << ". " << current << endl;
		i++;
		current = nextBlockHeader(current);
	}
}


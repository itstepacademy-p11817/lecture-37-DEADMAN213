#pragma once

#include <inttypes.h>

typedef struct {
	char* title;
	char* author;
} TBook;

typedef struct {
	uint8_t books;
	TBook* bookList;
} TVisitor;

typedef struct {
	TBook* book;
	bool taken;
	char* holder;
} TAdmin;

void pass();
char* createLine(FILE* fl);

TAdmin** readCatalogFile(FILE* fl, uint8_t* num);
void writeToFile(FILE* fl, TAdmin** bookList, uint8_t num);
TAdmin** clearList(TAdmin** list, uint8_t* len);

uint8_t adminAction();
uint8_t userAction();
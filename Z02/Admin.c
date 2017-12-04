#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <locale.h>
#include <ctype.h>
#include <stdbool.h>
#include "Header.h"

#define MENU 6

enum {
	ADD_BOOK = 1,
	SAVE_BOOKS_FILE,
	READ_BOOKS_FILE,
	PRINT_BOOKS,
	SEARCH_USER,
	EXIT
};

static const char* menuPoints[] = {
	"����:",
	"�������� ����� � �������.",
	"��������� �������.",
	"�������� �������.",
	"����������� �������.",
	"������ ������������.",
	"�����.",
};

static uint8_t menu() {
	uint8_t ch = 0;
	system("cls");

	printf("%s\n", menuPoints[0]);
	for (uint8_t i = 1; i <= MENU; i++) {
		printf("%hhu - %s\n", i, menuPoints[i]);
	}
	printf("��� �����: ");
	scanf("%hhu", &ch);

	return ch;
}
//-------------------------------------------------------------------------------------------------------------------

TBook* createBook() {
	TBook* book = NULL;
	char* line = NULL;

	printf("������� �������� ����� ��� ������� \"����\" ��� ������: ");
	if (line = createLine(stdin)) {
		book = (TBook*)calloc(1, sizeof(TBook));
		book->title = line;

		printf("������� ������� ������� � ��� ������ ����� ������: ");
		line = createLine(stdin);
		book->author = line;
	}

	return book;
}

TAdmin* createBookRecord() {
	TAdmin* bookRecord = NULL;
	TBook* temp = NULL;

	if (temp = createBook()) {
		bookRecord = (TAdmin*)calloc(1, sizeof(TAdmin));
		bookRecord->book = temp;

		bookRecord->taken = false;
		bookRecord->holder = NULL;
	}

	return bookRecord;
}

void printRecord(TAdmin* rec) {
	printf("\n��������: %s.\n", rec->book->title);
	printf("�����: %s.\n", rec->book->author);
	printf((rec->taken) ? "�� �����: " : "� �������.\n");

	if (rec->taken) {
		printf("%s.\n", rec->holder);
	}
}

void deleteRecord(TAdmin* rec) {
	free(rec->book->title);
	free(rec->book->author);

	if (rec->taken) {
		free(rec->holder);
	}

	free(rec);
}
//-------------------------------------------------------------------------------------------------------------------

bool loginAdmin() {
	system("cls");

	FILE* fl = NULL;

	if ((fl = fopen("catalog.txt", "rb")) != NULL) {
		char* pass = createLine(fl);

		printf("������� ������: ");
		char* passConfirm = createLine(stdin);

		if (strcmp(pass, passConfirm)) {
			printf("�������� ������.\n");
			printf("������� ����� �������.\n");
			_getch();

			free(pass);
			free(passConfirm);

			return false;
		}
	}
	else if ((fl = fopen("admin.txt", "wb")) != NULL) {
		printf("������� ������ ��������������: ");
		char* pass = createLine(stdin);

		fwrite(pass, strlen(pass) + 1, 1, fl);
	}
	else {
		printf("������ ��� �������� �����.\n");
	}

	if (fl) {
		fclose(fl);
	}
	
	return true;
}

uint8_t adminAction() {
	TAdmin** bookList = NULL;
	uint8_t numList = 0;

	uint8_t ch = 0;

	while (!loginAdmin());

	while ((ch = menu()) != EXIT) {
		switch (ch) {
		case ADD_BOOK: {
			pass();

			TAdmin* rec = NULL;
			while (rec = createBookRecord()) {
				bookList = (TAdmin**)realloc(bookList, (numList + 1) * sizeof(TAdmin));
				*(bookList + numList++) = rec;
			}
		}
			break;

		case SAVE_BOOKS_FILE: {
			pass();
			FILE* fl = NULL;

			if ((fl = fopen("catalog.txt", "wb")) == NULL) {
				printf("������ ��� �������� �����.\n");
			}
			else {
				printf("������� ����� ������ ��� ����������� ������: ");
				char* ps = createLine(stdin);
				fwrite(ps, strlen(ps) + 1, 1, fl);

				writeToFile(fl, bookList, numList);

				free(ps);
				fclose(fl);
			}
		}
			break;

		case READ_BOOKS_FILE: {
			pass();

			FILE* fl = NULL;
			if ((fl = fopen("catalog.txt", "rb")) == NULL) {
				printf("������ ��� �������� �����.\n");
			}
			else {
				bookList = readCatalogFile(fl, &numList);
				fclose(fl);
			}
		}
			break;

		case PRINT_BOOKS:
			for (uint8_t i = 0; i < numList; i++) {
				printRecord(*(bookList + i));
			}
			break;

		case SEARCH_USER: {
			pass();
			printf("������� ��� ������������: ");
			
			char* uname = createLine(stdin);
			size_t len = strlen(uname);
			uname = (char*)realloc(uname, len + 5);
			strcat(uname, ".txt");

			FILE* fl = NULL;
			if ((fl = fopen(uname, "rb")) == NULL) {
				printf("������������ � ����� ������� �� ������.\n");
			}
			else {
				uname = (char*)realloc(uname, len + 1);
				uname[len] = '\0';

				printf("������������ %s:\n", uname);
				free(uname);
				uname = createLine(fl);

				uint8_t num = 0;
				fread(&num, sizeof(num), 1, fl);
				for (uint8_t i = 0; i < num; i++) {
					free(uname);

					uname = createLine(fl);
					printf("\n��������: %s.\n", uname);
					free(uname);

					uname = createLine(fl);
					printf("�����: %s.\n", uname);
				}

				fclose(fl);
			}

			free(uname);
		}
			break;

		default:
			pass();
			printf("�������� ����.\n");
			break;
		}

		printf("������� ����� �������.\n");
		_getch();
	}

	clearList(bookList, &numList);

	return 1;
}
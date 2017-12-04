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

#define MENU 4

enum {
	TAKE_BOOK = 1,
	RETURN_BOOK,
	VIEW_BOOKS,
	EXIT
};

static const char* menuPoints[] = {
	"Меню:",
	"Взять книгу.",
	"Вернуть книгу.",
	"Просмотреть список взятых книг.",
	"Выход."
};

static uint8_t menu() {
	uint8_t ch = 0;
	system("cls");

	printf("%s\n", menuPoints[0]);
	for (uint8_t i = 1; i <= MENU; i++) {
		printf("%hhu - %s\n", i, menuPoints[i]);
	}
	printf("Ваш выбор: ");
	scanf("%hhu", &ch);

	return ch;
}

void printBooks(TBook* books, uint8_t num) {
	for (uint8_t i = 0; i < num; i++) {
		printf("\nКнига №%hhu:\n", i + 1);

		printf("Название: ");
		printf("%s.\n", (*(books + i)).title);

		printf("Автор: ");
		printf("%s.\n", (*(books + i)).author);
	}
}

bool loginUser(char** fname) {
	system("cls");

	FILE* fl = NULL;

	printf("Введите имя: ");
	char* uname = createLine(stdin);
	uname = (char*)realloc(uname, strlen(uname) + 5);

	*fname = (char*)calloc(1, strlen(uname) + 1);
	strcpy(*fname, uname);

	strcat(uname, ".txt");

	if ((fl = fopen(uname, "rb")) != NULL) {
		char* pass = createLine(fl);

		printf("Введите пароль: ");
		char* passConfirm = createLine(stdin);

		if (strcmp(pass, passConfirm)) {
			printf("Неверный пароль.\n");
			printf("Нажмите любую клавишу.\n");
			_getch();

			free(pass);
			free(passConfirm);

			return false;
		}
	}
	else if ((fl = fopen(uname, "wb")) != NULL) {
		printf("Введите пароль пользователя: ");
		char* pass = createLine(stdin);

		fwrite(pass, strlen(pass) + 1, 1, fl);

		uint8_t temp = 0;
		fwrite(&temp, sizeof(temp), 1, fl);
	}
	else {
		printf("Ошибка при открытии файла.\n");
	}

	if (fl) {
		fclose(fl);
	}

	free(uname);

	return true;
}

uint8_t userAction() {
	FILE* fl = NULL;
	uint8_t ch = 0;
	uint8_t numBooks = 0;
	TAdmin** bookList = NULL;

	uint8_t numMyBooks = 0;
	TBook* myBooks = NULL;
	char* login = NULL;

	while (!loginUser(&login));

	if ((fl = fopen("catalog.txt", "rb")) == NULL) {
		printf("Ошибка при открытии каталога.\n");
		printf("Нажмите любую клавишу.\n");
		_getch();
		return 1;
	}
	else {
		bookList = readCatalogFile(fl, &numBooks);
		fclose(fl);
	}

	char* fname = (char*)calloc(strlen(login) + 5, sizeof(char));
	strcpy(fname, login);
	strcat(fname, ".txt");

	if ((fl = fopen(fname, "rb")) == NULL) {
		printf("Ошибка при открытии файла пользователя.\n");
		printf("Нажмите любую клавишу.\n");
		_getch();
		return 1;
	}
	else {
		char* line = createLine(fl);
		fread(&numMyBooks, sizeof(numMyBooks), 1, fl);

		for (uint8_t i = 0; i < numMyBooks; i++) {
			myBooks = (TBook*)realloc(myBooks, (numMyBooks + 1) * sizeof(TBook));
			(*(myBooks + i)).title = createLine(fl);
			(*(myBooks + i)).author = createLine(fl);
		}
		fclose(fl);
	}

	while ((ch = menu()) != EXIT) {
		switch (ch) {
		case TAKE_BOOK: {
			uint8_t num = 0;

			for (uint8_t i = 0; i < numBooks; i++) {
				printf("\nКнига №%hhu", i + 1);
				printf("\nНазвание: %s.\n", (*(bookList + i))->book->title);
				printf("Автор: %s.\n", (*(bookList + i))->book->author);
				
				if ((*(bookList + i))->taken) {
					printf("Отсутствует.\n");
				}
				else {
					printf("В наличии.\n");
				}
			}

			printf("\nВведите номер книги или \"q\" для выхода: ");

			if (scanf("%hhu", &num) == 1 && num <= numBooks && !(*(bookList + num - 1))->taken && numMyBooks < 5) {
				myBooks = (TBook*)realloc(myBooks, (numMyBooks + 1) * sizeof(TBook));

				(*(myBooks + numMyBooks)).title = (char*)calloc(strlen((*(bookList + num - 1))->book->title) + 1, sizeof(char));
				strcpy((*(myBooks + numMyBooks)).title, (*(bookList + num - 1))->book->title);

				(*(myBooks + numMyBooks)).author = (char*)calloc(strlen((*(bookList + num - 1))->book->author) + 1, sizeof(char));
				strcpy((*(myBooks + numMyBooks)).author, (*(bookList + num - 1))->book->author);

				(*(bookList + num - 1))->taken = true;
				(*(bookList + num - 1))->holder = (char*)calloc(strlen(login) + 1, sizeof(char));
				strcpy((*(bookList + num - 1))->holder, login);

				numMyBooks++;
			}
			pass();
		}
			break;

		case RETURN_BOOK: {
			uint8_t num = 0;

			printBooks(myBooks, numMyBooks);

			printf("Ведите номер книги: ");
			if (scanf("%hhu", &num) == 1 && num <= numMyBooks) {
				for (uint8_t i = 0; i < numBooks; i++) {
					if (!strcmp((*(myBooks + num - 1)).title, (*(bookList + i))->book->title)) {
						free((*(bookList + 1))->holder);
						(*(bookList + i))->holder = NULL;
						(*(bookList + i))->taken = false;

						for (uint8_t i = num - 1; i < numMyBooks - 1; i++) {
							(*(myBooks + i)) = (*(myBooks + i + 1));
						}
						myBooks = (TBook*)realloc(myBooks, (numMyBooks - 1) * sizeof(TBook));
						numMyBooks--;

						break;
					}
				}
			}
		}
			break;

		case VIEW_BOOKS: 
			printBooks(myBooks, numMyBooks);
			break;

		default:
			pass();
			printf("Неверный ввод.\n");
			break;
		}

		printf("Нажмите любую клавишу.\n");
		_getch();
	}

	if ((fl = fopen("catalog.txt", "r+b")) == NULL) {
		printf("Ошибка при открытии каталога.\n");
		printf("Нажмите любую клавишу.\n");
		_getch();
		return 1;
	}
	else {
		char* ps = createLine(fl);
		_chsize(_fileno(fl), strlen(ps) + 1);
		fseek(fl, 0, SEEK_END);
		free(ps);

		writeToFile(fl, bookList, numBooks);
		fclose(fl);
	}

	if ((fl = fopen(fname, "r+b")) == NULL) {
		printf("Ошибка при открытии файла пользователя.\n");
		printf("Нажмите любую клавишу.\n");
		_getch();
		return 1;
	}
	else {
		char* ps = createLine(fl);
		_chsize(_fileno(fl), strlen(ps) + 1);
		fseek(fl, 0, SEEK_END);
		free(ps);

		fwrite(&numMyBooks, sizeof(numMyBooks), 1, fl);
		for (uint8_t i = 0; i < numMyBooks; i++) {
			fwrite((*(myBooks + i)).title, strlen((*(myBooks + i)).title) + 1, 1, fl);
			fwrite((*(myBooks + i)).author, strlen((*(myBooks + i)).author) + 1, 1, fl);
		}
		fclose(fl);
	}

	free(fname);
	free(login);

	return 1;
}
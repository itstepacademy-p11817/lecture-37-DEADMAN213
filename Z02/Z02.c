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

#define MENU 3

enum {
	ADMIN = 1,
	USER,
	EXIT
};

static const char* menuPoints[] = {
	"Меню:",
	"Администратор.",
	"Пользователь.",
	"Выход.",
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

void pass() {
	while (getchar() != '\n');
}

char* createLine(FILE* fl) {
	char* line = NULL;
	char ch = 0;
	uint16_t ln = 0;
	uint16_t ind = 0;

	while (!feof(fl)) {
		ch = getc(fl);

		if (ln == ind) {
			ln++;
			line = (char*)realloc(line, (ln *= 2) * sizeof(char));
		}

		if (ch == '\n' || ch == EOF || ch == '\0') {
			if (ind == 0) {
				free(line);
				line = NULL;
				break;
			}

			*(line + ind) = '\0';
			line = (char*)realloc(line, (ind + 1) * sizeof(char));
			break;
		}
		else {
			*(line + ind++) = ch;
		}
	}

	return line;
}

TAdmin** readCatalogFile(FILE* fl, uint8_t* num) {
	TAdmin** bList = NULL;

	char* ps = createLine(fl);
	free(ps);

	fread(num, sizeof(*num), 1, fl);
	bList = (TAdmin**)calloc(1, *num * sizeof(TAdmin*));
	for (uint8_t i = 0; i < *num; i++) {
		*(bList + i) = (TAdmin*)calloc(1, sizeof(TAdmin));
		(*(bList + i))->book = (TBook*)calloc(1, sizeof(TBook));
		(*(bList + i))->book->title = createLine(fl);
		(*(bList + i))->book->author = createLine(fl);

		fread(&((*(bList + i))->taken), sizeof(bool), 1, fl);
		if ((*(bList + i))->taken) {
			(*(bList + i))->holder = createLine(fl);
		}
	}

	return bList;
}

void writeToFile(FILE* fl, TAdmin** bookList, uint8_t num) {
	fwrite(&num, sizeof(num), 1, fl);
	for (uint8_t i = 0; i < num; i++) {
		fwrite((*(bookList + i))->book->title, strlen((*(bookList + i))->book->title) + 1, 1, fl);
		fwrite((*(bookList + i))->book->author, strlen((*(bookList + i))->book->author) + 1, 1, fl);

		fwrite(&((*(bookList + i))->taken), sizeof(bool), 1, fl);

		if ((*(bookList + i))->taken) {
			fwrite((*(bookList + i))->holder, strlen((*(bookList + i))->holder) + 1, 1, fl);
		}
	}
}

TAdmin** clearList(TAdmin** list, uint8_t* len) {
	if (list) {
		for (uint8_t i = 0; i < *len; i++) {
			free((*(list + i))->book->title);
			free((*(list + i))->book->author);
			free((*(list + i))->book);
			free((*(list + i))->holder);
			free(*(list + i));
		}
		free(list);
	}

	*len = 0;
	return NULL;
}

int main(void) {
	setlocale(LC_ALL, "Russian");
	uint8_t ch = 0;

	extern TAdmin** bookList;
	extern uint8_t numList;

	while ((ch = menu()) != EXIT) {
		switch (ch) {
		case ADMIN:
			pass();
			adminAction();
			break;

		case USER:
			pass();
			userAction();
			break;

		default:
			pass();
			printf("Неверный ввод.\n");
			break;
		}

		/*printf("Нажмите любую клавишу.\n");
		_getch();*/
	}
	
	return 0;
}
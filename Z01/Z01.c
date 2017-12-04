#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <locale.h>
#include <ctype.h>
#include <stdbool.h>

#define MENU 7

enum {
	ADD_TO_DICTIONARY = 1,
	PRINT_DICTIONARY,
	CLEAR_DICTIONARY,
	CENSOR_FILE,
	PRINT_SOURCE_FILE,
	PRINT_CENSOR_FILE,
	EXIT
};

const char* menuPoints[] = {
	"Меню:\n",
	"Добавить слово в словарь.\n",
	"Печать словаря.\n",
	"Очистить словарь.\n",
	"Цензурировать исходный файл.\n",
	"Печатать исходного файл.\n",
	"Печатать отцензурированного файл.\n",
	"Выход.\n",
};

uint8_t menu() {
	uint8_t ch = 0;

	system("cls");

	printf(menuPoints[0]);
	for (uint8_t i = 1; i <= MENU; i++) {
		printf("%hhu - %s", i, menuPoints[i]);
	}
	printf("Ваш выбор: ");
	scanf("%hhu", &ch);

	return ch;
}

void pass() {
	while (getchar() != '\n');
}

void passChar(FILE* fl) {
	char ch = 0;
	while ((ch = getc(fl)) != EOF && !isalpha(ch));
	if (!feof(fl)) {
		fseek(fl, -1, SEEK_CUR);
	}
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

char* createWord(FILE* fl) {
	char* line = NULL;
	char ch = 0;
	uint16_t ln = 0;
	uint16_t ind = 0;

	passChar(fl);
	while (!feof(fl)) {
		ch = getc(fl);
		if (ln == ind) {
			ln++;
			line = (char*)realloc(line, (ln *= 2) * sizeof(char));
		}

		if (!isalpha(ch)) {
			*(line + ind) = '\0';
			line = (char*)realloc(line, (ind + 1) * sizeof(char));
			break;
		}
		else {
			*(line + ind++) = ch;
		}
	}

	if (!feof(fl)) {
		fseek(fl, -1, SEEK_CUR);
	}

	return line;
}

uint8_t printFile(FILE* fl) {
	if (!fl) {
		printf("Файла не существует.\n");
		return 0;
	}

	fseek(fl, 0, SEEK_SET);
	char ch = 0;

	while (!feof(fl)) {
		if ((ch = getc(fl)) != EOF) {
			printf("%c", ch);
		}
	}

	return 1;
}

bool searchLine(const char* line, FILE* fl) {
	fseek(fl, 0, SEEK_SET);
	char* word = NULL;

	while (word = createLine(fl)) {
		if (strcmp(line, word) == 0) {
			return true;
		}
	}

	return false;
}

int main(void) {
	setlocale(LC_ALL, "Russian");

	uint8_t ch = 0;

	FILE* fdict = NULL;
	FILE* fsource = NULL;
	FILE* fcensor = NULL;

	const char* dictName = "dictionary.txt";

	if ((fdict = fopen(dictName, "r+")) == NULL) {
		printf("Ошибка при открытии словаря.\n");
		return 1;
	}

	while ((ch = menu()) != EXIT) {
		switch (ch) {
		case ADD_TO_DICTIONARY:
			if (fdict) {
				pass();
				fseek(fdict, 0, SEEK_END);

				printf("Введите слово: ");
				char* line = createLine(stdin);

				fputs(line, fdict);
				putc('\n', fdict);
				fflush(fdict);
			}
			break;

		case PRINT_DICTIONARY:
			if (fdict) {
				fseek(fdict, 0, SEEK_SET);

				char* line = NULL;
				while (line = createLine(fdict)) {
					fputs(line, stdout);
					putc('\n', stdout);
				}
			}
			break;

		case CLEAR_DICTIONARY:
			_chsize(_fileno(fdict), 0);
			break;

		case CENSOR_FILE: {
			char* line = NULL;

			pass();
			printf("Введите имя исходного файла: ");
			const char* fsourcename = createLine(stdin);

			printf("Введите имя отцензурированного файла: ");
			const char* fcensorename = createLine(stdin);

			if (fsourcename && fcensorename && (fsource = fopen(fsourcename, "r")) != NULL && (fcensor = fopen(fcensorename, "w")) != NULL) {
				fseek(fsource, 0, SEEK_SET);
				while (line = createWord(fsource)) {
					if (searchLine(line, fdict)) {
						fputs("[censored]", fcensor);
					}
					else {
						fputs(line, fcensor);
					}

					free(line);
					uint8_t ch = getc(fsource);
					while (!isalpha(ch)) {
						putc(ch, fcensor);
						ch = getc(fsource);
					}
					fseek(fsource, -1, SEEK_CUR);
				}

				fclose(fsource);
				fclose(fcensor);
			}
			else {
				printf("Ошибка открытия файла.\n");
			}
		}
			break;

		case PRINT_SOURCE_FILE: {
			pass();
			printf("Введите имя исходного файла: ");
			const char* fname = createLine(stdin);

			if (fname != NULL && (fsource = fopen(fname, "r")) != NULL) {
				printFile(fsource);
				putc('\n', stdout);
			}
			else {
				printf("Ошибка открытия файла.\n");
			}
		}
			break;

		case PRINT_CENSOR_FILE:
			pass();
			printf("Введите имя исходного файла: ");
			const char* fname = createLine(stdin);

			if (fname != NULL && (fcensor = fopen(fname, "r")) != NULL) {
				printFile(fcensor);
				putc('\n', stdout);
			}
			else {
				printf("Ошибка открытия файла.\n");
			}
			break;

		default:
			pass();
			printf("Неверный ввод.\n");
			break;
		}
		printf("Нажмите любую клавишу.\n");
		_getch();
	}



	return 0;
}
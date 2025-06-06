#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <ncurses.h>

/* STRUCTS */
typedef struct inode {
	int value;
	struct inode *prev;
	struct inode *next;
} inode;

typedef struct snode {
	short value;
	struct snode *prev;
	struct snode *next;
} snode;

typedef struct pair {
	int x;
	int y;
} pair;

/* FUNCTION DECLARATIONS */
inode* ilist_init();
snode* slist_init();
void slist_set(snode* node, char val);
void slist_insert(snode* node, char val);
void slist_append(snode* node, char val);
snode* slist_backspace(snode* node);
snode* slist_delete(snode* node);
snode* slist_recede(snode* node, int steps);
snode* slist_advance(snode* node, int steps);
int slist_xpos(snode* node);
snode* slist_previous_line(snode* node, int xpos);
snode* slist_next_line(snode* node, int xpos);
void open_file(snode* buffer);
void slist_destroy(snode* head);

/* VARIABLES */
char running;

snode *buffer_start_point;

snode *cursor_point;
pair cursor_position;
int xpos;

snode *first_printed_point;
int first_printed_line;
int line, column;
int number_of_lines;
int number_of_printed_lines;
pair screen_size;
int buffer_area_height;

enum Modes {
	NORMAL,
	INSERTION
} mode = NORMAL;

char filename[100] = {};
FILE *fptr;
int filesize;

short ch;
snode *cp; // Current point

/* FUNCTION DEFINITIONS */
inode* ilist_init() {
	inode *head = malloc(sizeof(inode));
	head->value = -1;
	head->prev = NULL;
	head->next = NULL;
	return head;
}

snode* slist_init() {
	snode *head = malloc(sizeof(snode));
	head->value = 10; // Newline character
	head->prev = NULL;
	head->next = NULL;
	return head;
}

void slist_destroy(snode* head) {
	snode* next;
	while (head != NULL) {
		next = head->next;
		free(head);
		head = next;
	}
}

void slist_set(snode* node, char val) {
	node->value = val;
}

void slist_insert(snode* node, char val) {
	snode *prev = malloc(sizeof(snode));
	prev->value = val;
	prev->prev = node->prev;
	prev->next = node;
	if (node->prev != NULL) node->prev->next = prev;
	node->prev = prev;
}

void slist_append(snode* node, char val) {
	snode *next = malloc(sizeof(snode));
	next->value = val;
	next->prev = node;
	next->next = node->next;
	if (node->next != NULL) node->next->prev = next;
	node->next = next;
}

snode* slist_backspace(snode* node) {
	snode* prev = node->prev;
	if (prev != NULL) {
		if (prev->prev != NULL) prev->prev->next = node;
		node->prev = prev->prev;
		free(prev);
	}
	return node;
}

snode* slist_delete(snode* node) {
	snode* next = node->next;
	if (next != NULL) {
		if (node->prev != NULL) node->prev->next = node->next;
		node->next->prev = node->prev;
		free(node);
		return next;
	}
	return node;
}

snode* slist_recede(snode* node, int steps) {
	for (int i = 0; i < steps && node->prev != NULL; i++) {
		node = node->prev;
	}
	return node;
}

snode* slist_advance(snode* node, int steps) {
	for (int i = 0; i < steps && node->next != NULL; i++) {
		node = node->next;
	}
	return node;
}

int slist_xpos(snode* node) {
	int xpos = 0;
	while (node->prev != NULL && node->prev->value != '\n') {
		node = node->prev;
		xpos++;
	}
	return xpos;
}

snode* slist_previous_line(snode* node, int xpos) {
	if (node->prev != NULL) node = node->prev;
	while (node->value != '\n' && node->prev != NULL) node = node->prev;
	while (node->prev != NULL && node->prev->value != '\n') node = node->prev;
	for (int i = 0; i < xpos && node->next != NULL && node->value != '\n'; i++) node = node->next;
	return node;
}

snode* slist_next_line(snode* node, int xpos) {
	snode* initial_position = node;
	while (node->value != '\n' && node->next != NULL) node = node->next;
	if (node->next != NULL) {
		node = node->next;
		for (int i = 0; i < xpos && node->next != NULL && node->value != '\n'; i++) node = node->next;
	} else {
		node = initial_position;
	}
	return node;
}

void open_file(snode* buffer) {
	buffer_start_point = slist_init();

	cursor_point = buffer_start_point;
	cursor_position.x = 0;
	cursor_position.y = 0;
	xpos = 0;

	first_printed_point = buffer_start_point;
	first_printed_line = 0;
	line = 0;
	column = 0;
	number_of_lines = 0;
	number_of_printed_lines = 0;


	cursor_position.x = 0;
	cursor_position.y = 0;
	xpos = 0;

	first_printed_line = 0;
	line = 0;
	column = 0;
	number_of_lines = 0;

	filesize = 0;

	slist_destroy(buffer_start_point);
	buffer_start_point = slist_init();
	clear();
	printw("Open file: ");
	refresh();
	echo();
	getstr(filename);
	noecho();

	fptr = fopen(filename, "r");
	if (fptr != NULL) {
		slist_set(buffer_start_point, fgetc(fptr));
		filesize++;
		cp = buffer_start_point;
		while ((ch = fgetc(fptr)) != EOF) {
			//if (ch == '\n') number_of_lines++;
			slist_append(cp, ch);
			cp = cp->next;
			filesize++;
		}
		fclose(fptr);
	}
	cursor_point = buffer_start_point;
	first_printed_point = buffer_start_point;
}

/* MAIN */
int main() {
	setlocale(LC_ALL, ""); // Get locale from user's environment

	running = 1;

	buffer_start_point = slist_init();

	cursor_point = buffer_start_point;
	cursor_position.x = 0;
	cursor_position.y = 0;
	xpos = 0;

	first_printed_point = buffer_start_point;
	first_printed_line = 0;
	line = 0;
	column = 0;
	number_of_lines = 0;
	number_of_printed_lines = 0;

	initscr();

	cbreak();
	keypad(stdscr, TRUE);
	noecho();

	// Main loop
	do {
		erase();
		// Buffer content
		number_of_printed_lines = 0;
		screen_size.y = getmaxy(stdscr);
		buffer_area_height = screen_size.y - 2;
		cp = first_printed_point;
		while (cp != NULL && number_of_printed_lines < buffer_area_height) {
			if (cp == cursor_point) {
				getyx(stdscr, cursor_position.y, cursor_position.x);
			}
			if (cp->value == '\n') {
				number_of_printed_lines++;
			}
			printw("%c", cp->value);
			cp = cp->next;
		}
		// Statusline
		move(buffer_area_height, 0);
		printw("\n");
		printw("%s ", mode == NORMAL ? "NORMAL" : "INSERTION");
		if (filename[0] == '\0')
			printw("*New file* ");
		else
			printw("%s ", filename);
		printw("%d:%d", line+1, column+1);
		move(cursor_position.y, cursor_position.x);
		refresh();

		ch = getch();
		if (mode == NORMAL) {
			if (ch == 'q') {
				running = 0;
			} else if (ch == 'i') {
				mode = INSERTION;
			} else if (ch == 'o') {
				open_file(buffer_start_point);
			} else if (ch == 's') {
				if (filename[0] != '\0') {
					fptr = fopen(filename, "w");
					cp = buffer_start_point;
					while (cp != NULL) {
						putc(cp->value, fptr);
						cp = cp->next;
					}
					fclose(fptr);
				} else {
					while (filename[0] == '\0') {
						clear();
						printw("Where to save the file: ");
						refresh();
						echo();
						getstr(filename);
						noecho();
					}
					fptr = fopen(filename, "w");
					cp = buffer_start_point;
					while (cp != NULL) {
						putc(cp->value, fptr);
						cp = cp->next;
					}
				}
				fclose(fptr);
			} else if (ch == 'l') {
				if (cursor_point->next != NULL) {
					cursor_point = cursor_point->next;
					xpos = slist_xpos(cursor_point);
					if (cursor_point->prev != NULL && cursor_point->prev->value == '\n') line++;
					column = slist_xpos(cursor_point);
				}
			} else if (ch == 'h') {
				if (cursor_point->prev != NULL) {
					cursor_point = cursor_point->prev;
					xpos = slist_xpos(cursor_point);
					if (cursor_point->next != NULL && cursor_point->value == '\n') line--;
					column = slist_xpos(cursor_point);
				}
			} else if (ch == 'j') {
				if (cursor_point != slist_next_line(cursor_point, xpos)) line++;
				cursor_point = slist_next_line(cursor_point, xpos);
				column = slist_xpos(cursor_point);
			} else if (ch == 'k') {
				if (cursor_point != slist_previous_line(cursor_point, xpos)) line--;
				cursor_point = slist_previous_line(cursor_point, xpos);
				column = slist_xpos(cursor_point);
			}
		} else if (mode == INSERTION) {
			if (ch == 27) { // Esc key code
				mode = NORMAL;
			} else if (ch == 263) { // Backspace key code
				if (cursor_point->prev != NULL && cursor_point->prev->value == '\n') {
					line--;
				}
				cursor_point = slist_backspace(cursor_point);
				// TODO: I have to plan a better way of doing this. I'm repeating myself too much.
				//if (cursor_point->prev == NULL) buffer_start_point = cursor_point;
				xpos = slist_xpos(cursor_point);
				column = slist_xpos(cursor_point);
			} else if (ch == 330) { // Delete key code
				cursor_point = slist_delete(cursor_point);
				xpos = slist_xpos(cursor_point);
				column = slist_xpos(cursor_point);
			} else if (!((ch <= 31 && ch != 10) || ch == 94 || ch == 96 || ch == 126 || ch > 128)) {
				slist_insert(cursor_point, ch);
				xpos = slist_xpos(cursor_point);
				if (cursor_point == buffer_start_point) {
					buffer_start_point = cursor_point->prev;
					first_printed_point = cursor_point->prev;
				}
				if (ch == 10) { // Enter key code
					line++;
				}
				column = slist_xpos(cursor_point);
			}
		}
		while ((first_printed_line + buffer_area_height - 1) < line) {
			first_printed_line++;
			first_printed_point = slist_next_line(first_printed_point, 0);
		}
		while (line < first_printed_line) {
			first_printed_line--;
			first_printed_point = slist_previous_line(first_printed_point, 0);
		}
	} while (running);
	erase();
	endwin();
	return 0;
}

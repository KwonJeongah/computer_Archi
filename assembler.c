#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct Inst {
	char *name;
	char *op;
	char type;
	char *funct;
};

struct Inst inst[22] = {
	{"addiu", "001001", 'I',""},
	{"addu", "000000", 'R',"100001"},
	{"and", "000000", 'R',"100100"},
	{"andi", "001100", 'I', ""},
	{"beq","000100", 'I', ""},
	{"bne", "000101", 'I', ""},
	{"j", "000010", 'J', ""},
	{"jal", "000011", 'J', ""},
	{"jr", "000000", 'J',"001000"},
	{"lui", "001111", 'I', ""},
	{"lw", "100011", 'I', ""},
	{"nor", "000000", 'R' , "100111"},
	{"or", "000000", 'R', "100101"},
	{"ori", "001101", 'I', ""},
	{"sltiu", "001011",'I' ,""},
	{"sltu", "000000", 'R', "101011"},
	{"sll", "000000", 'S' ,"000000"},
	{"srl", "000000", 'S', "000010"},
	{"sw", "101011", 'I', ""},
	{"subu", "000000", 'R', "100011"}
};

struct Data {
	int value;
	struct Data *next;
};

struct Text {
	int idx;
	char *d;
	char *s;
	char *t;
	unsigned int address;
	struct Text *next;
};

struct Sym {
	char *name;
	int size;
	unsigned int address;
	struct Data *first;
	struct Sym *next;
	struct Data *last;
};

struct Data* createData()
{
	struct Data* newData = (struct Data*)malloc(sizeof(struct Data));
	
	return newData;
}


struct Text* createText()
{
	struct Text* newText = (struct Text*)malloc(sizeof(struct Text));
	newText->d = NULL;
    newText->s = NULL;
    newText->t = NULL;
    newText->next = NULL;
    newText->address = -1;

	return newText;
}


struct Sym* createSym()
{
	struct Sym* newSym = (struct Sym*)malloc(sizeof(struct Sym));
	

	return newSym;
}

void appendText(struct Text* head, struct Text* newText)
{
	while (head->next != NULL)
	{
		head = head->next;
	}
	head->next = newText;
}

void appendData(struct Data* head, struct Data* newData)
{
	while (head->next != NULL)
	{
		head = head->next;
	}
	head->next = newData;
}

void appendSym(struct Sym* head, struct Sym* newSym)
{
	while (head->next != NULL)
	{
		head = head->next;
	}
	head->next = newSym;
}


struct Sym *Symbols;
struct Text *Texts;
int datasize, textsize;

/*
 * You may need the following function
 */
char* NumToBits(unsigned int num, int len) {
	char* bits = (char *)malloc(len + 1);
	int idx = len - 1, i;

	while (num > 0 && idx >= 0) {
		if (num % 2 == 1) {
			bits[idx--] = '1';
		}
		else {
			bits[idx--] = '0';
		}
		num /= 2;
	}

	for (i = idx; i >= 0; i--) {
		bits[i] = '0';
	}

	return bits;
}

unsigned int findSym(struct Sym *Symbols, char* name)
{
	struct Sym* sym;

	for (sym = Symbols->next; sym != NULL; sym = sym->next) {
		/* blank */
		if (strcmp(sym->name, name))
		{
			return sym->address;
		}
	}

	return -1;
}

void Eliminate(char *str, char ch)
{
	for (; *str != '\0'; str++)
	{
		if (*str == ch)
		{
			strcpy(str, str + 1);
			str--;
		}
	}
}


/*
 * read the assmebly code
 */
void read_asm() {
	int tmp = 0;
    int i;
	unsigned int address;
	char temp[0x1000] = { 0 };
	struct Sym *temp_sym = NULL;
	struct Data *temp_data = NULL;
	struct Text *temp_text = NULL;

	//Read .data region
	address = 0x10000000;
	for (i = 0; scanf("%s", temp) == 1;) {
		if (strcmp(temp, ".text") == 0) {
			/* blank */
			break; //text나오면 data부분 끝
		}
		else if (temp[strlen(temp) - 1] == ':') {
			Eliminate(temp, ':');

			if (tmp != 0)
			{
				temp_sym->size = tmp * 4;
				tmp = 0;
				temp_sym->last = temp_data;
			}

			temp_sym = createSym();
            temp_sym->address = address;
            strcpy(temp_sym->name, temp);

			appendSym(Symbols->next, temp_sym);
		}
		else if (strcmp(temp, ".word") == 0) {
			if (scanf("%s", temp) == 1)
			{
				temp_data = createData();
                temp_data->value = (unsigned int)atoi(temp);
				address += 0x00000004;
				tmp++;
			}
			appendData(temp_sym->first, temp_data);
		}
	}

	datasize = address - 0x10000000;

	tmp = 0;
	temp_sym = NULL;

	//Read .text region
	address = 0x400000;
	for (i = 0; scanf("%s", temp) == 1;) {
		if (temp[strlen(temp) - 1] == ':') {
			temp_sym = createSym();
            temp_sym->address = address;
            
            strcpy(temp_sym->name, temp);
			appendSym(Symbols, temp_sym);
			address += 0x00000004;

		}
		else {
			/* blank */

			appendText(Texts, temp_text);
			temp_text = createText();

			if (strcmp(temp, "la"))// temp가 la인 경우
			{
				address += 0x000004;
				temp_text->idx = 9;//lui

				if (scanf("%s", temp) == 1)
				{
					//s
					strcpy(temp_text->s, temp);
				}

				if (scanf("%s", temp) == 1)//data
				{
                    strcpy(temp_text->d, temp);
                    temp_text->address =address;
                    appendText(Texts->next, temp_text);

                    temp_text = createText();

                    for (struct Text* text = Texts->next; text != NULL; text = text->next) {
							strcpy(temp_text->s, text->s);
							strcpy(temp_text->t, text->s);
						}

                    strcpy(temp_text->d, temp);
                    temp_text->address = address;
                    /*
					if (findSym(Symbols->next, temp) == 0x10000000)
					{
						temp_text->address = 0x1000;
					}
					else
					{
						temp_text->address = 0x1000;
						appendText(Texts->next, temp_text);

						temp_text = createText();

						for (struct Text* text = Texts->next; text != NULL; text = text->next) {
							strcpy(temp_text->s, text->s);
							strcpy(temp_text->t, text->s);
						}

						temp_text->idx = 13;
						temp_text->address = findSym(Symbols->next, temp) & 0x00001111;

					}*/
				}
			}
			else //la가 아닌 경우
			{
				//idx찾기, 여기서 찾아지면 명령어고 아니면 데이터임
				for (i = 0; i < 22; i++)
				{
					if (strcmp(inst[i].name, temp) == 1)
					{
						temp_text->idx = i;
						break;
					}
				}

				if (i < 22) //명령이다
				{
					temp_text->address = address;
					address += 0x000004;

				}
				else //아니다
				{
					if (inst[temp_text->idx].op == "R" || inst[temp_text->idx].op == "S")
					{
                        scanf("%s", temp);
						strcpy(temp_text->d, temp);
                        scanf("%s", temp);
                        strcpy(temp_text->s, temp);
                        scanf("%s", temp);
                        strcpy(temp_text->t, temp);
                        temp_text->address = address;
					}
					else if (inst[temp_text->idx].op == "I")
					{
						scanf("%s", temp);
                        strcpy(temp_text->s, temp);
                        scanf("%s", temp);
                        strcpy(temp_text->t, temp);
                        scanf("%s", temp);    //숫자                    
                        strcpy(temp_text->d, temp);
                        temp_text->address = address;
					}
					else if (inst[temp_text->idx].op == "J")
					{
                        scanf("%s", temp);
						strcpy(temp_text->d,temp); //lab3이나 레지스터 이름 같은 거 들어감
                        temp_text->address = address;
					}
				}
			}
		}
	}

	textsize = address - 0x400000;
}

/*
 * convert the assembly code to num
 */
void subst_asm_to_num() {
	struct Text *text;
	struct Sym *sym;

	for (text = Texts->next; text != NULL; text = text->next) {
            if(text->d != NULL)	Eliminate(text->d, '$');	
			if(text->s != NULL) Eliminate(text->s, '$');		
			if(text->t != NULL) Eliminate(text->t, '$');
            
           
	}

    for (sym = Symbols->next; sym != NULL; sym = sym->next)
    {
        Eliminate(sym->name, ':');
    }
}


/*
 * print the results of assemble
 */
void print_bits() {
	struct Text* text;
	struct Sym* sym;
	struct Data* data;

	// print the header
	printf("%s", NumToBits(textsize, 32));
	printf("%s", NumToBits(datasize, 32));

	// print the body
	for (text = Texts->next; text != NULL; text = text->next) {
		printf("%s", inst[text->idx].op);

		if (inst[text->idx].type == 'R') {
			/* blank */
			printf("%s", NumToBits((unsigned int)atoi(text->s), 5));
			printf("%s", NumToBits((unsigned int)atoi(text->t), 5));
			printf("%s", NumToBits((unsigned int)atoi(text->d), 5));
			printf("%s", "00000");
			printf("%s", inst[text->idx].funct);

		}
		else if (inst[text->idx].type == 'I') {
			/* blank */
			printf("%s", NumToBits((unsigned int)atoi(text->s), 5));

			if (text->t == NULL)
			{
				printf("%s", "00000");
			}
			else
			{
				printf("%s", NumToBits((unsigned int)atoi(text->t), 5));
			}

            if(text->idx == 9)//lui
            {
            	printf("%s", NumToBits(0x1000, 16));					
            } 
            else if(text->idx == 13)//ori
            {
                printf("%s", NumToBits(findSym(Symbols, text->d) & 0x1111, 16));
            }
            else
            {
                if(findSym(Symbols, text->d) != -1)
              {
                printf("%s", NumToBits(findSym(Symbols, text->d) - text->address,16));
              }			
			    else
			  {
				printf("%s", "0000000000000000");
		      }
            }       


		}
		else if (inst[text->idx].type == 'S') {
			printf("%s", "00000");
			printf("%s", NumToBits((unsigned int)atoi(text->s), 5));
			printf("%s", NumToBits((unsigned int)atoi(text->d), 5));
			printf("%s", NumToBits((unsigned int)atoi(text->t), 5));
			printf("%s", inst[text->idx].funct);

		}
		else { //jump        
		printf("%s", NumToBits(findSym(Symbols->next, text->d) >> 2, 26));
		}
	}

	for (sym = Symbols->next; sym != NULL; sym = sym->next) {
		for (data = sym->first; data != NULL; data = data->next) {
			printf("%s", NumToBits(data->value, 32));
		}
	}
	printf("\n");
}

/*
 * main function
 */
int main(int argc, char* argv[]) {

	if (argc != 2) {

		printf("Usage: ./assembler <assembly file>\n");
		exit(0);

	}
	else {

		// reading the input file
		char *filename = (char *)malloc(strlen(argv[1]) + 3);
		strncpy(filename, argv[1], strlen(argv[1]));

		if (freopen(filename, "r", stdin) == 0) {
			printf("File open Error\n");
			exit(1);
		}

		Symbols = (struct Sym *)malloc(sizeof(struct Sym));
		Texts = (struct Text *)malloc(sizeof(struct Text));

        Symbols->next = NULL;
        Texts->next = NULL;


		// creating the output file (*.o)
		filename[strlen(filename) - 1] = 'o';
		freopen(filename, "w", stdout);

		// Let's complete the below functions!
		read_asm();
		subst_asm_to_num();
		print_bits();

		// freeing the memory
		free(filename);
		free(Symbols);
		free(Texts);
	}
	return 0;
}

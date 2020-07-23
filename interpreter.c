#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOKENS "+-<>,.[]"
#define INS_SIZE 500
#define TAPE_SIZE 500
#define BSTK_SIZE 500

struct Instruction {
   int type;
   size_t jmp;
};

static struct Instruction* instructs;
static size_t ins_size = 0;
static size_t mx_ins_size = INS_SIZE;
static size_t* b_stk;
static size_t b_stk_top = 0;

int push_instruction(int ins) {

   if (ins_size >= mx_ins_size) 
      instructs = realloc(instructs, (mx_ins_size *= 2) * sizeof(struct Instruction));

   struct Instruction insert;
   insert.type = ins;

   if (ins == '[') b_stk[b_stk_top++] = ins_size;

   if (ins == ']') {
      if (b_stk_top == 0) return 1;
      size_t idx = b_stk[--b_stk_top];
      insert.jmp = idx;
      instructs[idx].jmp = ins_size;
   }

   instructs[ins_size++] = insert;

   return 0;
}

void cleanup(FILE* fp) {
   fclose(fp);
   free(b_stk);
   free(instructs);
}

int main(int argc, char** argv) {

   if (argc != 2) {
      printf("Usage: %s <filename>\n", *argv);
      return 1;
   }

   FILE* file = fopen(argv[1], "r");

   if (file == NULL) {
      printf("Unable to open file \"%s\".\n", argv[1]);
      return 2;
   }

   instructs = malloc(INS_SIZE * sizeof(struct Instruction));
   b_stk = malloc(BSTK_SIZE * sizeof(size_t));

   char c;
   while ((c = fgetc(file)) != EOF) {
      if (strchr(TOKENS, c) && push_instruction(c) == 1) {
         printf("ERROR: Closing bracket without opening bracket.\n");
         cleanup(file);
         return 3;
      }
   }

   if (b_stk_top != 0) {
      printf("ERROR: Opening bracket without closing bracket.\n");
      cleanup(file);
      return 4;
   }

   unsigned char tape[TAPE_SIZE] = {0};
   unsigned char* ptr = tape;

   for (size_t i = 0; i < ins_size; i++) {
      switch(instructs[i].type) {
         case '+': *ptr += 1; break;
         case '-': *ptr -= 1; break;
         case '>': ptr++; break;
         case '<': ptr--; break;
         case '.': putchar(*ptr); break;
         case ',': *ptr = getchar(); break;
         case '[': if (*ptr == 0) i = instructs[i].jmp; break;
         case ']': if (*ptr > 0) i = instructs[i].jmp; break;
      }
   }

   cleanup(file);

   return 0;
}
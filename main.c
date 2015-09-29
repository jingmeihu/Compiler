#include "def.h"

extern int yydebug;

static int err_status = 0;
int error_type;

inline void set_error_status()
{
	err_status = 1;
}

void print_error(int errtype, int lineno, char* msg)
{
	error_type = errtype;
	printf("Error at line %d: %s\n", lineno, msg);
	set_error_status(); // error found
}

int main(int argc, char** argv)
{
	FILE *fin;
	FILE *fout_tree;
	FILE *fout_ir;
	FILE *fout_mc;
	if (argc < 4)
	{
		printf("please give enough file to read test file, write grammer tree, ir code and machine code");
		return -1;
	}

	if (!(fin = fopen(argv[1], "r")))
		{printf("cannot open the test file"); return -1;}
	fout_tree = fopen(argv[2], "w");
	fout_ir = fopen(argv[3], "w");
	fout_mc = fopen(argv[4], "w");
	// initialization
	init_symboltable();
	printf("\nsymbol table initializes complete.\n");
	// grammer parse tree
	yyrestart(fin);
	yyparse();

	printf("\nparse complete.\n");
	if (treeroot == NULL)
	{
		printf("Syntax Error: No Grammer Tree has built.\n");
		return -1;
	}
	fprintf(fout_tree, "grammer tree begin!\n");
	tree_display(treeroot, 0, fout_tree);
	fprintf(fout_tree, "grammer tree end!\n");
	printf("\ngrammer tree complete.\n");
	/* semantic analysis */
	semantic_parse(treeroot);	
	printf("\nsemantic complete.\n");
	if (err_status) /* Well, semantic error! */
		return -1;
	
	/* generate intermediate code! */
	ir_init();	
	printf("\nintermediate code initializes complete.\n");

	intermediatecode_parse(treeroot);
	printf("\nintermediate code parses complete.\n");

	ir_display(fout_ir);
	printf("\nintermediate code generates complete.\n");

	mc_init(fout_mc);
	printf("\nmachine code initializes complete.\n");
	mc_generate(fout_mc);
	printf("\nmachine code generates complete.\n");

	fclose(fout_tree);
	fclose(fout_ir);
	fclose(fout_mc);

	return 0;
}

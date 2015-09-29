#ifndef _DEF_H_
#define _DEF_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

// grammar tree
enum NodeType
{
	NT_UNKNOWN,
	NT_INT, NT_ID, NT_SEMI, NT_COMMA, NT_ASSIGN, NT_RELOP,
	NT_TYPE, NT_LP, NT_RP, NT_LB, NT_RB, NT_LC, NT_RC,
	NT_IF, NT_ELSE, NT_BREAK, NT_CONT, NT_FOR, NT_STRUCT, NT_RETURN, NT_WHILE,
	NT_INCR, NT_DECR, 
	NT_LSHIFTASSIGN, NT_RSHIFTASSIGN, NT_LSHIFT, NT_RSHIFT, 
	NT_LOGIAND, NT_LOGIOR, 
	NT_ADDASSIGN, NT_MINUSASSIGN, NT_MULTASSIGN, NT_DIVASSIGN, 
	NT_BITANDASSIGN, NT_BITXORASSIGN, NT_BITORASSIGN, 
	NT_MULT, NT_DIV, NT_MOD, NT_ADD, NT_MINUS, 
	NT_BITOR, NT_BITXOR, NT_BITAND, 
	NT_LOGINOT, NT_BITNOT, NT_DOT,
	NT_Program, NT_ExtDefS, NT_ExtDef, NT_ExtVarS, NT_Spec, NT_StSpec, NT_OptTag, NT_Tag, NT_Var, NT_Func, NT_ParaS, NT_Para, NT_StmtBlock, NT_StmtS, NT_Stmt, NT_DefS, NT_Def, NT_DecS, NT_Dec, NT_Exp, NT_Args	
};
enum ReduceType
{
	RT_TERMINAL,
	RT_Program_ExtDefS, 
	RT_ExtDefS_ExtDef_ExtDefS, RT_ExtDefS_NULL, 
	RT_ExtDef_Spec_ExtVarS_SEMI, RT_ExtDef_Spec_SEMI, RT_ExtDef_Spec_Func_StmtBlock, RT_ExtDef_Spec_Func_SEMI,
	RT_ExtVarS_Dec, RT_ExtVarS_Dec_COMMA_ExtVarS, 
	RT_Spec_TYPE, RT_Spec_StSpec, 
	RT_StSpec_STRUCT_OptTag_LC_DefS_RC, RT_StSpec_STRUCT_Tag, 
	RT_OptTag_ID, RT_OptTag_NULL, 
	RT_Tag_ID, 
	RT_Var_ID, RT_Var_Var_LB_INT_RB, 
	RT_Func_ID_LP_ParaS_RP, RT_Func_ID_LP_RP, 
	RT_ParaS_Para_COMMA_ParaS, RT_ParaS_Para, 
	RT_Para_Spec_Var, 
	RT_StmtBlock_LC_DefS_StmtS_RC, 
	RT_StmtS_Stmt_StmtS, RT_StmtS_NULL, 
	RT_Stmt_Exp_SEMI, RT_Stmt_StmtBlock, RT_Stmt_RETURN_Exp_SEMI, RT_Stmt_IF_LP_Exp_RP_Stmt, 
	RT_Stmt_IF_LP_Exp_RP_Stmt_ELSE_Stmt, RT_Stmt_FOR_LP_Exp_SEMI_Exp_SEMI_Exp_RP_Stmt, RT_Stmt_FOR_LP_SEMI_Exp_SEMI_Exp_RP_Stmt, RT_Stmt_WHILE_LP_Exp_RP_Stmt,
	 RT_Stmt_CONT_SEMI, RT_Stmt_BREAK_SEMI, 
	RT_DefS_Def_DefS, RT_DefS_NULL, 
	RT_Def_Spec_DecS_SEMI, 
	RT_DecS_Dec, RT_DecS_Dec_COMMA_DecS, 
	RT_Dec_Var, RT_Dec_Var_ASSIGN_Init, RT_Dec_Var_ASSIGN_Exp, RT_Dec_Var_ASSIGN_LC_Args_RC, RT_Dec_ID_LB_INT_RB_ASSIGN_LC_Args_RC, RT_Dec_ID_LB_INT_RB_LB_INT_RB_ASSIGN_LC_Args_RC,
	RT_Init_Exp, RT_Init_LC_Args_RC, 
	RT_Exp_Exp_ASSIGN_Exp, 
	RT_Exp_Exp_MULT_Exp,RT_Exp_Exp_DIV_Exp, RT_Exp_Exp_MOD_Exp, RT_Exp_Exp_ADD_Exp,RT_Exp_Exp_MINUS_Exp, RT_Exp_Exp_MULTASSIGN_Exp,RT_Exp_Exp_DIVASSIGN_Exp, RT_Exp_Exp_ADDASSIGN_Exp,RT_Exp_Exp_MINUSASSIGN_Exp,
	RT_Exp_Exp_RELOP_Exp, 	
	RT_Exp_Exp_LSHIFT_Exp, RT_Exp_Exp_RSHIFT_Exp, RT_Exp_Exp_LSHIFTASSIGN_Exp, RT_Exp_Exp_RSHIFTASSIGN_Exp,
	RT_Exp_Exp_LOGIAND_Exp,RT_Exp_Exp_LOGIOR_Exp, RT_Exp_Exp_BITAND_Exp, RT_Exp_Exp_BITOR_Exp, RT_Exp_Exp_BITXOR_Exp, RT_Exp_Exp_BITANDASSIGN_Exp, RT_Exp_Exp_BITORASSIGN_Exp, RT_Exp_Exp_BITXORASSIGN_Exp, 
	RT_Exp_LOGINOT_Exp, RT_Exp_BITNOT_Exp, RT_Exp_INCR_Exp, RT_Exp_DECR_Exp, RT_Exp_MINUS_Exp, RT_Exp_ID_LP_Args_RP, RT_Exp_LP_Exp_RP, RT_Exp_ID_LP_RP, RT_Exp_Exp_LB_Exp_RB, RT_Exp_Exp_DOT_ID, 
	RT_Exp_ID, RT_Exp_INT,  RT_Exp_NULL, 
	RT_Args_Exp_COMMA_Args, RT_Args_Exp
};
enum Relop {R_LESS, R_GREATER, R_NOTEQUAL, R_EQUAL, R_NOGREATERTHAN, R_NOLESSTHAN};
#define reverse_relop(r) (5 - (r))

enum ExprType {ET_NUM, ET_BOOL}; 

enum VarComplexity {VC_BASIC, VC_ADDR, VC_REL};

#define NT_FIRST_TOKEN NT_INT;
#define NT_LAST_TOKEN NT_RELOP
#define NT_FIRST_NON_TERMINAL NT_Program

//for lvalue
#define LV_UNKNOWN 0
#define LV_LVALUE 1
#define LV_RVALUE 2
#define LV_BIVALUE 3

union TreeNode_val 
{
	int	val_int;
	char*	val_str;
	enum Relop val_rel;
};

struct TreeNode 
{
	union TreeNode_val val;//node value
	enum NodeType type;//node type
	char* type_name;//node type name
	struct TreeNode *child, *sibling;
	int lineno;//number of code line

	enum ReduceType reduce_type;//reduced type
	/* syn: synchronized property; inh: inherited property */
	int lvalue;
	int arraydec;
	union 
	{
		struct TypeNode* vtype;//value 
		struct TypeStructList* field;//field-list type
		struct TypeNode* return_type;//used for Fun->CompSt
	};
	char* name;//name if has an ID name 
	unsigned char struct_defined;//struct defined or not

	enum ExprType exp_type;//whether a value expr
	struct irNode *ir_prev;// prev irNode
	struct irNode *label_true, *label_false;//inh/syn: T/F label
	union 
	{
		struct 
		{
			struct irOperand *base_addr;//current offset variable id 
			int size;//array size
			int *arraylist; //array list
		} complex_type;//complex type [array struct]

		struct 
		{
			union 
			{
				struct irOperand* rvop;//result var op
				int ival;//actual value
			};
			int numeric;//if literal numeric, 0 for no
		} int_type;
		struct 
		{
			struct irNode* rel_node;// ifgoto stmt
		} condition_stmt;//condition stmt
	};
	enum VarComplexity var_cmplx;
};

// for new node
struct TreeNode * tree_new_node(char * type_name, int lineno, union TreeNode_val val, enum NodeType type, enum ReduceType reduce_type);

// append as sibling 
struct TreeNode * tree_append_sibling(struct TreeNode * prev_sibling, struct TreeNode * cur_node);

// append as child
struct TreeNode * tree_append_child(struct TreeNode * parent, struct TreeNode * cur_node);

// Grammer Tree Root
struct TreeNode* treeroot;

void tree_display(struct TreeNode* p, int level, FILE * fout);

/****************************************************************************************************************/

//symbol table
//type kind
enum TypeKind {
	TK_INT, TK_ARRAY, TK_STRUCT, TK_FUNC, TK_ERROR
};

enum SymbolKind {
	SK_UNKNOWN, 
	SK_DEF, //definition
	SK_VAR,	//variable
	SK_LINKHEADER, // link list header 
	SK_SSTACKHEADER // stack header
};

enum VarType {
	VT_PTR, //pointer
	VT_VARIABLE, //variable itself
	VT_CONSTANT	//constant
};

struct TypeNode;

#define LAST_BASIC_TYPE TK_INT

//array
struct TypeArray {
	struct TypeNode* type; 
	int size;
	int width;
	int *list;
};

//struct
struct TypeStructList {
	char* name;
	struct TypeNode* type;
	struct TypeStructList* next;
	int width;
	int offset;
};

//function
#define FUNC_DEFINED 1
#define FUNC_UNDEFINED 0
struct func {
	struct TypeNode* return_type; //return type
	struct TypeStructList* param; //param list
	int defined; //non-zero: Defined, 0: only declarated
};

//type node
struct TypeNode {
	enum TypeKind kind; //variable, function, link-header
	int width; //width of the type
	union {
		// function
		struct func* function;
		// struct
		struct TypeStructList* field_list;
		// array
		struct TypeArray array;
	};
};

// symbol table

struct SymbolNode {
	struct SymbolNode *up, *down; //link of SCOPE-STACK 
	struct SymbolNode *left, *right; //link of HASH TABLE

	enum SymbolKind kind;

	char* name; // name of variable, function
	struct {
		int vid;// tmp vid 
		enum VarType vtype;
	} cur_variable; // indicates the tmp variable currently used
	struct TypeNode *type;
};

#define HASH_LENGTH 100
#define hashheader SymbolNode // hash header
#define sstackheader SymbolNode // stack header

struct SymbolNode* find_SymbolNode(char* name, enum SymbolKind kind);

struct SymbolNode* find_SymbolNode_scope(struct sstackheader* ssh, char* name, enum SymbolKind kind);

struct SymbolNode* find_SymbolNode_cur_scope(char* name, enum SymbolKind kind);

struct SymbolNode* add_into_shash(struct sstackheader* sshdr, char* name, enum SymbolKind  kind, struct TypeNode* type);

void set_SymbolNode_vid(struct SymbolNode* n, int vid, enum VarType vtype);

int get_SymbolNode_vid(struct SymbolNode* n);

enum VarType get_SymbolNode_var_type(struct SymbolNode* n);

struct SymbolNode* add_SymbolNode(char* name, enum SymbolKind  kind, struct TypeNode* type);

struct SymbolNode* add_SymbolNode_parent(char* name, enum SymbolKind  kind, struct TypeNode* type);

struct sstackheader* new_scope();

struct sstackheader* cur_scope();

int del_current_scope();

struct TypeNode* new_TypeNode(enum TypeKind kind);

#define new_TypeNode_basic new_TypeNode 

struct TypeNode* new_TypeNode_arr_by_basic(enum TypeKind kind, int size);

struct TypeNode* new_TypeNode_arr_by_TypeNode(struct TypeNode* sub, int size);

struct TypeNode* new_TypeNode_struct(struct TypeStructList* field);

struct TypeStructList* new_fieldlist_link(struct TypeNode* n, char* name, struct TypeStructList* next);

struct func* new_func(struct TypeNode* return_type, struct TypeStructList* param, int func_defined);

void del_func(struct func* f);

struct TypeNode* new_TypeNode_func(struct func* f);

int typecmp(struct TypeNode* n1, struct TypeNode* n2);

int fieldcmp(struct TypeStructList* f1, struct TypeStructList* f2);

int funccmp(struct func* f1, struct func* f2);

struct TypeStructList* find_field(struct TypeStructList* field, char* name);

struct TypeStructList* create_fieldlist_from_cur_scope();

void init_symboltable();

//semantic analysis

//children
#define child1(p) (p->child)
#define child2(p) (child1(p)->sibling)
#define child3(p) (child2(p)->sibling)
#define child4(p) (child3(p)->sibling)
#define child5(p) (child4(p)->sibling)
#define child6(p) (child5(p)->sibling)
#define child7(p) (child6(p)->sibling)
#define child8(p) (child7(p)->sibling)
#define child9(p) (child8(p)->sibling)
#define child10(p) (child9(p)->sibling)
#define child11(p) (child10(p)->sibling)

//error
#define ERROR_VAR_UNDEFINED 			1
#define ERROR_FUNC_UNDEFINED 			2
#define ERROR_STRUCT_UNDEFINED 			3
#define ERROR_VAR_MULTIDEFINED 			4
#define ERROR_FUNC_MULTIDEFINED			5
#define ERROR_STRUCT_MULTIDEFINED 		6
#define ERROR_ASSIGN_TYPE_UNMATCH 		7
#define ERROR_TYPE_UNMATCH 				8
#define ERROR_RETURN_TYPE_UNMATCH 		9
#define ERROR_FUNC_ARG_UNMATCH 			10
#define ERROR_USE_VAR_AS_FUNC 			11
#define ERROR_USE_VAR_AS_STRUCT 		12
#define ERROR_RVALUE_BEFORE_ASSIGN 		13
#define ERROR_ARRAY_TYPE_REQUIRED 		14
#define ERROR_INVALID_ARRAY_INDEX 		15
#define ERROR_INVALID_STRUCT_SCOPE 		16
#define ERROR_INVALID_DEF_IN_STRUCT 	17
#define ERROR_FUNC_DEC_UNDEFINED 		18
#define ERROR_FUNC_DEC_DEF_MISMATCH 	19
#define ERROR_COND_WRONG_TYPE			20


#define ERROR_VAR_UNDEFINED_MSG				"Undefined Variable" 
#define ERROR_FUNC_UNDEFINED_MSG			"Undefined Function"
#define ERROR_STRUCT_UNDEFINED_MSG			"Undefined Struct"
#define ERROR_VAR_MULTIDEFINED_MSG			"Multidefined Variable"
#define ERROR_FUNC_MULTIDEFINED_MSG			"Multidefined Function"
#define ERROR_STRUCT_MULTIDEFINED_MSG		"Multidefined Struct"
#define ERROR_ASSIGN_TYPE_UNMATCH_MSG		"Incompatible Types when 'assign'"
#define ERROR_TYPE_UNMATCH_MSG				"Incompatible Types"
#define ERROR_RETURN_TYPE_UNMATCH_MSG		"Incompatible Type when 'return'"
#define ERROR_FUNC_ARG_UNMATCH_MSG			"Incompatible Function Parameter"
#define ERROR_USE_VAR_AS_FUNC_MSG			"Incompatible Function Call"
#define ERROR_USE_VAR_AS_STRUCT_MSG			"Incompatible Struct Type"
#define ERROR_RVALUE_BEFORE_ASSIGN_MSG		"Left Value Required"
#define ERROR_ARRAY_TYPE_REQUIRED_MSG		"Array Type Required"
#define ERROR_INVALID_ARRAY_INDEX_MSG		"Invalid Array Index"
#define ERROR_INVALID_STRUCT_SCOPE_MSG		"Invalid Struct Scope"
#define ERROR_INVALID_DEF_IN_STRUCT_MSG		"Invalid Struct Definition" 
#define ERROR_FUNC_DEC_UNDEFINED_MSG		"Function Declared No Defined" 
#define ERROR_FUNC_DEC_DEF_MISMATCH_MSG		"Mismatched Function"
#define ERROR_COND_WRONG_TYPE_MSG 			"Invalid Condition"

int semantic_parse(struct TreeNode* treeroot);

/****************************************************************************************************************/
//internediate code generate

enum IROperandType {
	OT_VAR,	//A variable
	OT_CONST_INT, //A constant integer
	OT_ADDR, 	//An address
	OT_STAR	//equavalent to *x
};

struct irOperand {
	enum IROperandType operand_type; //Operand type
	union {
		int ival;		/* integer constant */
		int vid;		/* variable id, used when OT_ADDR or OT_VAR */
	};
};

enum IRNodeType {
	IT_HEADER,		/* Well, it's a header */
	IT_LABEL, 		/* LABEL sunicy1 */
	IT_ASSIGN, 		/* x = y */
	IT_ADD, 		/* x = y + z */
	IT_SUB, 		/* x = y - z */
	IT_MUL, 		/* x = y * z */
	IT_DIV, 		/* x = y / z */
	IT_MOD, 		/* x = y % z */
	IT_LSHIFT, 		/* x = y << z */
	IT_RSHIFT, 		/* x = y >> z */
	IT_BITAND, 		/* x = y & z */
	IT_BITOR, 		/* x = y | z */
	IT_BITXOR, 		/* x = y ^ z */
	IT_BITNOT, 		/* x = ~y */
	IT_ADDASSIGN, 	/* x = x + y */
	IT_SUBASSIGN, 	/* x = x - y */
	IT_MULASSIGN, 	/* x = x * y */
	IT_DIVASSIGN, 	/* x = x / y */
	IT_LSHIFTASSIGN, /* x = x << y */
	IT_RSHIFTASSIGN, /* x = x >> y */
	IT_BITANDASSIGN, /* x = x & y */
	IT_BITORASSIGN,  /* x = x | y */
	IT_BITXORASSIGN, /* x = x ^ y */
	IT_INCR,		/* x = x + 1 */
	IT_DECR,		/* x = x - 1 */
	IT_RSTAR,		/* x = *y */
	IT_LSTAR, 		/* *x = y */
	IT_GOTO, 		/* GOTO sunicy2 */
	IT_IFGOTO, 		/* IF x RELOP y GOTO sunicy3 */
	IT_RETURN, 		/* RETURN x */
	IT_DEC, 		/* DEC x [SIZE] */
	IT_ARRAY,	/* ARRAY  x [SIZE] */
	IT_VAR,		/*VAR x */
	IT_ST, 		/*STORE x */
	IT_CALL, 		/* x = CALL f */
	IT_PARAM, 		/* PARAM v */
	IT_READ, 		/* READ x */
	IT_WRITE,		/* WRITE x */
	IT_FUNC_LABEL	/* FUNCTION x : */
};

struct irNode {
	enum IRNodeType ntype; 
	union {
		struct { struct irOperand *left, *right; } assign;
		struct { struct irOperand *result, *op1, *op2; } binop;
		struct { 
			int label_id;// label id 
		} label;
		struct {
			struct irNode *label; // jump label
		} labelgoto;
		struct { 
			struct irOperand *x, *y;
			enum Relop rel;
			struct irNode *label;
		} ifgoto;
		struct { struct irOperand *x; int def;} unaryop;
		struct { struct irOperand *x; int width; int def;} dec;
		struct { struct irOperand *x; int width; int def; int *list;} array;
		struct { struct irOperand *x; char* fname; } call;
		struct { char* fname;} func;
	};
	struct irNode *prev, *next; /* link list */
};

struct irOperand* new_operand_from_old(struct irOperand* old, int vid);

// addr(&x)
struct irOperand* new_addr_operand(int vid);

// int constant(#1)
struct irOperand* new_int_const_operand(int ival);

// actual var 
struct irOperand* new_var_operand(int vid);

// STAR var 
struct irOperand* new_star_operand(int vid);

// next new variable id
int new_vid();

// next param variable id
#define new_param_vid new_vid

// next label id
int new_label_id();

// irNode: IT_LABEL
struct irNode* new_irnode_label(int label_id);

// irNode: IT_FUNC_LABEL
struct irNode* new_irnode_func_label(char* fname);

// irNode: IT_ASSIGN, RSTAR, LSTAR
struct irNode* new_irnode_assign(struct irOperand *op1, 
							struct irOperand *op2);

// irNode: IT_ADD
struct irNode* new_irnode_add(struct irOperand *result, 
		struct irOperand *op1, struct irOperand *op2);
		
// irNode: IT_SUB
struct irNode* new_irnode_sub(struct irOperand *result, 
		struct irOperand *op1, struct irOperand *op2);
		
// irNode: IT_MUL
struct irNode* new_irnode_mul(struct irOperand *result, 
		struct irOperand *op1, struct irOperand *op2);
		
// irNode: IT_DIV
struct irNode* new_irnode_div(struct irOperand *result, 
		struct irOperand *op1, struct irOperand *op2);

// irNode: IT_MOD
struct irNode* new_irnode_mod(struct irOperand *result, 
		struct irOperand *op1, struct irOperand *op2);
		
// irNode: IT_LSHIFT
struct irNode* new_irnode_lshift(struct irOperand *result, 
		struct irOperand *op1, struct irOperand *op2);

// irNode: IT_RSHIFT
struct irNode* new_irnode_rshift(struct irOperand *result, struct irOperand *op1, struct irOperand *op2);
		
// irNode: IT_BITAND
struct irNode* new_irnode_bitand(struct irOperand *result, struct irOperand *op1, struct irOperand *op2);
		
// irNode: IT_BITOR
struct irNode* new_irnode_bitor(struct irOperand *result, struct irOperand *op1, struct irOperand *op2);

// irNode: IT_BITNOT
struct irNode* new_irnode_bitnot(struct irOperand *result, struct irOperand *op1);

// irNode: IT_BITXOR
struct irNode* new_irnode_bitxor(struct irOperand *result, struct irOperand *op1, struct irOperand *op2);
		
// irNode: IT_ADDASSIGN
struct irNode* new_irnode_addassign(struct irOperand *result, struct irOperand *op1, struct irOperand *op2);
		
// irNode: IT_SUBASSIGN
struct irNode* new_irnode_subassign(struct irOperand *result, struct irOperand *op1, struct irOperand *op2);
		
// irNode: IT_MULASSIGN
struct irNode* new_irnode_mulassign(struct irOperand *result, struct irOperand *op1, struct irOperand *op2);
		
// irNode: IT_DIVASSIGN
struct irNode* new_irnode_divassign(struct irOperand *result, struct irOperand *op1, struct irOperand *op2);

// irNode: IT_LSHIFTASSIGN
struct irNode* new_irnode_lshiftassign(struct irOperand *result, struct irOperand *op1, struct irOperand *op2);

// irNode: IT_RSHIFTASSIGN
struct irNode* new_irnode_rshiftassign(struct irOperand *result, struct irOperand *op1, struct irOperand *op2);
		
// irNode: IT_BITANDASSIGN
struct irNode* new_irnode_bitandassign(struct irOperand *result, struct irOperand *op1, struct irOperand *op2);
		
// irNode: IT_BITORASSIGN
struct irNode* new_irnode_bitorassign(struct irOperand *result, struct irOperand *op1, struct irOperand *op2);

// irNode: IT_BITXORASSIGN
struct irNode* new_irnode_bitxorassign(struct irOperand *result, struct irOperand *op1, struct irOperand *op2);

// irNode: IT_INCR
struct irNode* new_irnode_incr(struct irOperand *result, struct irOperand *op1);
		
// irNode: IT_DECR
struct irNode* new_irnode_decr(struct irOperand *result, struct irOperand *op1);
		
		
// irNode: IT_GOTO
struct irNode* new_irnode_goto(struct irNode *label);

// irNode: IT_IFGOTO(X REL Y ? GOTO label_id)
struct irNode* new_irnode_ifgoto(struct irOperand *x, struct irOperand *y, enum Relop rel, struct irNode *label);

// irNode: IT_RETURN
struct irNode* new_irnode_return(struct irOperand *x);

// irNode: IT_DEC
struct irNode* new_irnode_dec(struct irOperand *x, int width, int def);

// create an irNode: IT_ARRAY
struct irNode* new_irnode_array(struct irOperand *x, int width, int def, int *list);

// irNode: IT_VAR
struct irNode* new_irnode_var(struct irOperand *x, int def);

// irNode: IT_ST
struct irNode* new_irnode_store(struct irOperand *x);

// irNode: IT_CALL, result, function name
struct irNode* new_irnode_call(struct irOperand *x, char* fname);

// irNode: IT_PARAM
struct irNode* new_irnode_param(struct irOperand *v);

// irNode: IT_READ
struct irNode* new_irnode_read(struct irOperand *x);

// irNode: IT_WRITE
struct irNode* new_irnode_write(struct irOperand *x);

// reverse ifgoto condition
struct irNode* reverse_irnode_ifgoto(struct irNode* n);

// the header
struct irNode* ir_header();

// the last element
struct irNode* ir_tail();

// insert after
struct irNode* ir_ins_after(struct irNode* prev, struct irNode* cur);

// insert before
struct irNode* ir_ins_before(struct irNode* next, struct irNode* cur);

struct irNode* ir_ins_tail(struct irNode* cur);

// delete one irNode
struct irNode* ir_del_node(struct irNode* cur);

// display the code
void ir_display_node(struct irNode* p);

// display the code
void ir_display(FILE * fout);

//initial
void ir_init();

int intermediatecode_parse(struct TreeNode* treeroot);


/****************************************************************************************************************/
//machine code generate

//temporal register t0-t7
#define REG_t0 8
#define REG_t1 9
#define REG_t2 10
#define REG_t3 11
#define REG_t4 12
#define REG_t5 13
#define REG_t6 14
#define REG_t7 15
#define REG_LAST_t REG_t7
//store register
#define REG_s0 16
#define REG_s1 17
#define REG_s2 18
#define REG_s3 19
#define REG_s4 20
#define REG_s5 21
#define REG_s6 22
#define REG_s7 23
//other register
#define REG_0 0
#define REG_sp 29
#define REG_ra 31
#define REG_v0 2
#define REG_v1 3
#define REG_a0 4
#define REG_a1 5
#define REG_a2 6
#define REG_a3 7


// variable information
struct MCVar {
	int vid; // variable id
	int offset; // addr offset
	int len; //addr length

	struct MCVar *prev, *next; // link-list
};

// create new function space
void mc_new_func();
// create new MCVar
struct MCVar* mc_new_var(int vid, int offset, int len);

// find MCVar
struct MCVar* mc_find_var(int vid);

// returns var's offset
int mc_var_offset(int vid);

// create new MCVar
struct MCVar* mc_new_global_var(int vid, int offset, int len);

// find MCVar
struct MCVar* mc_find_global_var(int vid);

// returns var's offset
int mc_var_global_offset(int vid);
// find the next function
struct irNode* mc_find_next_func(struct irNode* prev);

// load an operand into register
void mc_load_operand(struct irOperand* op, int reg, FILE * fout);

// save register into variable space
void mc_save_operand(struct irOperand* op, int reg, FILE * fout);

// push into stack
void mc_push(int reg, FILE * fout);
// pop from stack
void mc_pop(int reg, FILE * fout);

// translating intermediate code into MIPS code
//la
static void mc_generate_la(int reg_dst, int vid, FILE * fout);
// lw
static void mc_generate_lw(int reg_dst, int offset, int reg_base, FILE * fout);
// sw
static void mc_generate_sw(int reg_src, int offset, int reg_base, FILE * fout);
// li
static void mc_generate_li(int reg, int constant, FILE * fout);
// add
static void mc_generate_add(int reg_dst, int reg_src1, int reg_src2, FILE * fout);
// sub
static void mc_generate_sub(int reg_dst, int reg_src1, int reg_src2, FILE * fout);
// mul
static void mc_generate_mul(int reg_dst, int reg_src1, int reg_src2, FILE * fout);
// div
static void mc_generate_div(int reg_dst, int reg_src1, int reg_src2, FILE * fout);
// mod
static void mc_generate_mod(int reg_dst, int reg_src1, int reg_src2, FILE * fout);
// and
static void mc_generate_and(int reg_dst, int reg_src1, int reg_src2, FILE * fout);
// or
static void mc_generate_or(int reg_dst, int reg_src1, int reg_src2, FILE * fout);
// xor
static void mc_generate_xor(int reg_dst, int reg_src1, int reg_src2, FILE * fout);
// not
static void mc_generate_not(int reg_dst, int reg_src, FILE * fout);
//sll
static void mc_generate_sll(int reg_dst, int reg_src, int constant, FILE * fout);
//srl
static void mc_generate_srl(int reg_dst, int reg_src, int constant, FILE * fout);
//label
static void mc_generate_label(int label_id, FILE * fout);
//function label
static void mc_generate_functionlabel(char* functionname, FILE * fout);
//j
static void mc_generate_j(int label_id, FILE * fout);
//jal
static void mc_generate_jal(char* functionname, FILE * fout);
//return
static void mc_generate_jr(FILE * fout);
//if XXX goto XXX
static void mc_generate_ifgoto(enum Relop rel, int reg1, int reg2, int label_id, FILE * fout);
//mov $reg1, $reg2
static void mc_generate_mov(int reg1, int reg2, FILE * fout);


void mc_init(FILE* fout);

//mc generate
void mc_generate(FILE * fout);

#endif


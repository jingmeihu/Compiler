/*
	NAME:smallc.y
	FUNCTION:a yacc based syntax analyser. The syntax analyser reads the source codes of SMALLC and generates a parser.

	by Hu Jingmei
*/
%{

#include "smallc.c"
#include "def.h"

int error_type = 0;
static union TreeNode_val gval;

#define FORCE_OUTPUT 1

//Error
#define find_error if (error_type != 0) break
				
//New a Nonterminal Symbol
#define new_nonterminal_symbol(symbol, rtype) \
	tree_new_node(#symbol, yylineno, gval, NT_##symbol,rtype)

//New a Terminal Symbol
#define new_terminal_symbol(symbol) \
	tree_new_node(#symbol, yylineno, gval, NT_##symbol, RT_TERMINAL)

//Add a Sibling
#define add_sibiling(p, cur) \
	tree_append_sibling(p, cur)

//Add a Child
#define add_child(p, cur) \
	tree_append_child(p, cur)

%}

%union {
	int type_int;
	char * type_str;
	int type_rel;
	struct TreeNode * ptrTreeNode;
}
%type <ptrTreeNode> Program ExtDefS ExtDef ExtVarS Spec StSpec StmtBlock Stmt DefS Def DecS Dec Exp Args 
%type <ptrTreeNode> OptTag Tag Var Func ParaS Para StmtS

%nonassoc LOWEST
%token COMMA SEMI
%token LC RC
%token <type_int>INT <type_str> ID <type_rel> RELOP
%nonassoc SIMPLE_IF_STMT
%nonassoc ELSE
%nonassoc SIMPLE_ARRAY
%token STRUCT RETURN IF BREAK CONT FOR WHILE


%right 	ASSIGN ADDASSIGN MINUSASSIGN MULTASSIGN LSHIFTASSIGN RSHIFTASSIGN DIVASSIGN BITANDASSIGN BITXORASSIGN BITORASSIGN
%left	LOGIOR
%left 	LOGIAND
%left	BITOR 
%left	BITXOR
%left	BITAND
%left	RELOP
%left	LSHIFT RSHIFT
%left  	ADD MINUS
%left 	MULT DIV MOD
%nonassoc MINUS_NUM
%right 	LOGINOT BITNOT INCR DECR
%right  <type_str> TYPE
%left   LP RP LB RB DOT

%%
Program:	ExtDefS
			{
				find_error;
				$$ =new_nonterminal_symbol(Program, RT_Program_ExtDefS); 
				add_child($$,$1);
				if (!error_type) {treeroot = $$;}
				else treeroot = NULL;
			}
		|	ExtDefS error
			{
				$$ = NULL;
				error_type = 2;
				printf("Error at line %d\n", yylineno);
			}
		;
		
ExtDefS:	ExtDef ExtDefS	
			{
				find_error;
				$$ = new_nonterminal_symbol(ExtDefS, RT_ExtDefS_ExtDef_ExtDefS); add_sibiling(add_child($$,$1),$2);
			}
		|	{
				find_error;
				$$ = new_nonterminal_symbol(ExtDefS, RT_ExtDefS_NULL);
				//add_child($$,NULL);
			}
		;
		
ExtDef:		Spec ExtVarS SEMI	
			{
				find_error;
				$$ = new_nonterminal_symbol(ExtDef, RT_ExtDef_Spec_ExtVarS_SEMI);
				add_sibiling(add_sibiling(add_child($$,$1),$2),new_terminal_symbol(SEMI));
			}
		|	Spec SEMI 
			{
				find_error;
				$$ = new_nonterminal_symbol(ExtDef,RT_ExtDef_Spec_SEMI); 
				add_sibiling(add_child($$,$1),new_terminal_symbol(SEMI));
			}
		|	Spec Func StmtBlock	
			{
				find_error;
				$$ = new_nonterminal_symbol(ExtDef, RT_ExtDef_Spec_Func_StmtBlock); 
				add_sibiling(add_sibiling(add_child($$,$1),$2),$3);
			}
		|	Spec Func SEMI	
			{
				find_error;
				$$ = new_nonterminal_symbol(ExtDef, RT_ExtDef_Spec_Func_SEMI); 
				add_sibiling(add_sibiling(add_child($$,$1),$2),new_terminal_symbol(SEMI));
			}
		|	Spec ExtVarS error 
			{
				$$ = NULL; 
				error_type = 2;
				printf("Error at line %d: ';' expected\n",  yylineno);
			}
		;
		
ExtVarS:	Dec
			{
				find_error;
				$$ = new_nonterminal_symbol(ExtVarS, RT_ExtVarS_Dec); 
				add_child($$,$1);
			}
		|	Dec COMMA ExtVarS	
			{
				find_error;
				$$ = new_nonterminal_symbol(ExtVarS, RT_ExtVarS_Dec_COMMA_ExtVarS); 
				add_sibiling(add_sibiling(add_child($$,$1),new_terminal_symbol(COMMA)),$3);
			}
		;
Spec:		TYPE		
			{
				find_error;
				$$ = new_nonterminal_symbol(Spec, RT_Spec_TYPE); 
				gval.val_str = $1;
				add_child($$,new_terminal_symbol(TYPE));
			}
		|	StSpec	
			{
				find_error;
				$$ = new_nonterminal_symbol(Spec,RT_Spec_StSpec);
				add_child($$,$1);
			}
		;
StSpec:		STRUCT OptTag LC DefS RC		
			{
				find_error;
				$$ = new_nonterminal_symbol(StSpec,RT_StSpec_STRUCT_OptTag_LC_DefS_RC);			
				add_sibiling(add_sibiling(add_sibiling(add_sibiling(add_child($$,new_terminal_symbol(STRUCT)),$2),new_terminal_symbol(LC)),$4),new_terminal_symbol(RC));
			}
		|	STRUCT Tag	
			{
				find_error;
				$$ = new_nonterminal_symbol(StSpec,RT_StSpec_STRUCT_Tag); 
				add_sibiling(add_child($$,new_terminal_symbol(STRUCT)),$2);
			}
		;
		
OptTag:		ID	
			{
				find_error;
				$$ = new_nonterminal_symbol(OptTag,RT_OptTag_ID);
				gval.val_str = $1;
				add_child($$,new_terminal_symbol(ID));
			}
		|	{
				find_error;
				$$ = new_nonterminal_symbol(OptTag,RT_OptTag_NULL);
				//add_child($$,NULL);
			}
		;

Tag:		ID	
			{
				find_error;
				$$ = new_nonterminal_symbol(Var,RT_Var_ID);
				gval.val_str = $1;
				add_child($$,new_terminal_symbol(ID));
			}
		;
Var	:		ID 	
			{
				find_error;
				$$ = new_nonterminal_symbol(Var,RT_Var_ID);
				gval.val_str = $1;
				add_child($$,new_terminal_symbol(ID));
			}
		|	Var LB INT RB 
			{
				find_error;
				$$ = new_nonterminal_symbol(Var,RT_Var_Var_LB_INT_RB);
				gval.val_int = $3;
				add_sibiling(add_sibiling(add_sibiling(add_child($$,$1),new_terminal_symbol(LB)),new_terminal_symbol(INT)),new_terminal_symbol(RB));
			}
		;		
Func:		ID LP ParaS RP
			{
				find_error;
				$$ = new_nonterminal_symbol(Func,RT_Func_ID_LP_ParaS_RP);
				gval.val_str = $1;
				add_sibiling(add_sibiling(add_sibiling(add_child($$,new_terminal_symbol(ID)),new_terminal_symbol(LP)), $3),new_terminal_symbol(RP));
			}
		|	ID LP RP
			{
				find_error;
				$$ = new_nonterminal_symbol(Func,RT_Func_ID_LP_RP);
				gval.val_str = $1;
				add_sibiling(add_sibiling(add_child($$,new_terminal_symbol(ID)),new_terminal_symbol(LP)),new_terminal_symbol(RP));
			}
		;
		
ParaS:		Para COMMA ParaS 	
			{
				find_error;
				$$ = new_nonterminal_symbol(ParaS,RT_ParaS_Para_COMMA_ParaS); 
				add_sibiling(add_sibiling(add_child($$,$1),new_terminal_symbol(COMMA)),$3);
			}
		|	Para		
			{
				find_error;
				$$ = new_nonterminal_symbol(ParaS,RT_ParaS_Para); 
				add_child($$,$1);
			}
		;
		
Para:		Spec Var		
			{
				find_error;
				$$ = new_nonterminal_symbol(Para,RT_Para_Spec_Var);
				add_sibiling(add_child($$,$1),$2);
			}
		;
		
StmtBlock:	LC DefS StmtS RC	
			{
				find_error;
				$$ = new_nonterminal_symbol(StmtBlock,RT_StmtBlock_LC_DefS_StmtS_RC);
				add_sibiling(add_sibiling(add_sibiling(add_child($$,new_terminal_symbol(LC)),$2),$3),new_terminal_symbol(RC));
			}
		;
		
StmtS:		Stmt StmtS 
			{
				find_error;
				$$ = new_nonterminal_symbol(StmtS,RT_StmtS_Stmt_StmtS);
				add_sibiling(add_child($$,$1),$2);}
		|	{
				find_error;
				$$ = new_nonterminal_symbol(StmtS,RT_StmtS_NULL);
				//add_child($$,NULL);
			}
		;
		
Stmt:		Exp SEMI	
			{
				find_error;
				$$ =new_nonterminal_symbol(Stmt,RT_Stmt_Exp_SEMI); 
				add_sibiling(add_child($$,$1),new_terminal_symbol(SEMI));
			}
		|	StmtBlock	 
			{
				find_error;
				$$ = new_nonterminal_symbol(Stmt,RT_Stmt_StmtBlock); 
				add_child($$,$1);
			}
		|	RETURN Exp SEMI 
			{
				find_error;
				$$ = new_nonterminal_symbol(Stmt,RT_Stmt_RETURN_Exp_SEMI); 
				add_sibiling(add_sibiling(add_child($$,new_terminal_symbol(RETURN)),$2),new_terminal_symbol(SEMI));
			}
		|	IF LP Exp RP Stmt
			{
				find_error;
				$$ = new_nonterminal_symbol(Stmt,RT_Stmt_IF_LP_Exp_RP_Stmt); 
				add_sibiling(add_sibiling(add_sibiling(add_sibiling(add_child($$,new_terminal_symbol(IF)),new_terminal_symbol(LP)),$3),new_terminal_symbol(RP)),$5);
			}  %prec SIMPLE_IF_STMT 
		|	IF LP Exp RP Stmt ELSE Stmt
			{
				find_error;
				$$ = new_nonterminal_symbol(Stmt,RT_Stmt_IF_LP_Exp_RP_Stmt_ELSE_Stmt); 
				add_sibiling(add_sibiling(add_sibiling(add_sibiling(add_sibiling(add_sibiling(add_child($$,new_terminal_symbol(IF)),new_terminal_symbol(LP)),$3),new_terminal_symbol(RP)),$5),new_terminal_symbol(ELSE)),$7);
			}
		|	FOR LP Exp SEMI Exp SEMI Exp RP Stmt	
			{
				find_error;
				$$ = new_nonterminal_symbol(Stmt,RT_Stmt_FOR_LP_Exp_SEMI_Exp_SEMI_Exp_RP_Stmt);
				 add_sibiling(add_sibiling(add_sibiling(add_sibiling(add_sibiling(add_sibiling(add_sibiling(add_sibiling(add_child($$,new_terminal_symbol(FOR)),new_terminal_symbol(LP)),$3),new_terminal_symbol(SEMI)),$5),new_terminal_symbol(SEMI)),$7),new_terminal_symbol(RP)),$9);
			}
		|	FOR LP SEMI Exp SEMI Exp RP Stmt	
			{
				find_error;
				$$ = new_nonterminal_symbol(Stmt,RT_Stmt_FOR_LP_SEMI_Exp_SEMI_Exp_RP_Stmt);
				add_sibiling(add_sibiling(add_sibiling(add_sibiling(add_sibiling(add_sibiling(add_sibiling(add_child($$,new_terminal_symbol(FOR)),new_terminal_symbol(LP)),new_terminal_symbol(SEMI)),$4),new_terminal_symbol(SEMI)),$6),new_terminal_symbol(RP)),$8);
			}
		|	WHILE LP Exp RP Stmt
			{
				find_error;
				$$ = new_nonterminal_symbol(Stmt,RT_Stmt_WHILE_LP_Exp_RP_Stmt); 
				add_sibiling(add_sibiling(add_sibiling(add_sibiling(add_child($$,new_terminal_symbol(WHILE)),new_terminal_symbol(LP)),$3),new_terminal_symbol(RP)),$5);
			}
		|	CONT SEMI	
			{
				find_error;
				$$ =new_nonterminal_symbol(Stmt,RT_Stmt_CONT_SEMI); 
				add_sibiling(add_child($$,new_terminal_symbol(CONT)),new_terminal_symbol(SEMI));
			}
		|	BREAK SEMI	
			{
				find_error;
				$$ =new_nonterminal_symbol(Stmt,RT_Stmt_BREAK_SEMI); 
				add_sibiling(add_child($$,new_terminal_symbol(BREAK)),new_terminal_symbol(SEMI));
			}
		|	error SEMI 
			{
				$$ = NULL;
				error_type = 2;
				printf("Error at line %d: Syntax error.\n", yylineno);
			}
		|	Exp error 
			{
				$$ = NULL;
				error_type = 2;
				printf("Error at line %d: (';' expected ?) unexpected '%s'.\n", yylineno, yytext);
				yyerrok;
			}  %prec LOWEST

		;
		
DefS:		Def DefS	
			{
				find_error;
				$$ = new_nonterminal_symbol(DefS,RT_DefS_Def_DefS);
				add_sibiling(add_child($$,$1),$2);
			}
		|	{
				find_error;
				$$ = new_nonterminal_symbol(DefS,RT_DefS_NULL);
				//add_child($$,NULL);
			}
		;
	
Def:		Spec DecS SEMI	
			{
				find_error;
				$$ = new_nonterminal_symbol(Def,RT_Def_Spec_DecS_SEMI); 
				add_sibiling(add_sibiling(add_child($$,$1),$2),new_terminal_symbol(SEMI));
			}
		;
		
DecS:		Dec
			{
				find_error;
				$$ = new_nonterminal_symbol(DecS,RT_DecS_Dec);
				add_child($$,$1);
			}
		|	Dec COMMA DecS	
			{
				find_error;
				$$ = new_nonterminal_symbol(DecS,RT_DecS_Dec_COMMA_DecS); 
				add_sibiling(add_sibiling(add_child($$,$1),new_terminal_symbol(COMMA)),$3);
			}
		;

Dec	:		Var	
			{
				find_error;
				$$ = new_nonterminal_symbol(Dec,RT_Dec_Var); 
				add_child($$,$1);
			}
		|	Var ASSIGN Exp	
			{
				find_error;
				$$ = new_nonterminal_symbol(Dec,RT_Dec_Var_ASSIGN_Exp);
				add_sibiling(add_sibiling(add_child($$,$1),new_terminal_symbol(ASSIGN)),$3);
			}
		|	Var ASSIGN LC Args RC
			{
				find_error;
				$$ = new_nonterminal_symbol(Dec,RT_Dec_Var_ASSIGN_LC_Args_RC);
				add_sibiling(add_sibiling(add_sibiling(add_sibiling(add_child($$,$1),new_terminal_symbol(ASSIGN)),new_terminal_symbol(LC)),$4), new_terminal_symbol(RC));
			}
		;

Exp:		Exp ASSIGN Exp	
			{
				find_error;
				$$ = new_nonterminal_symbol(Exp,RT_Exp_Exp_ASSIGN_Exp); 
				add_sibiling(add_sibiling(add_child($$,$1),new_terminal_symbol(ASSIGN)),$3);
			}
		|	Exp MULT Exp	
			{
				find_error;
				$$ = new_nonterminal_symbol(Exp,RT_Exp_Exp_MULT_Exp); 
				add_sibiling(add_sibiling(add_child($$,$1),new_terminal_symbol(MULT)),$3);
			}
		|	Exp DIV Exp
			{
				find_error;
				$$ = new_nonterminal_symbol(Exp,RT_Exp_Exp_DIV_Exp); 
				add_sibiling(add_sibiling(add_child($$,$1),new_terminal_symbol(DIV)),$3);
			}
		|	Exp MOD Exp
			{
				find_error;
				$$ = new_nonterminal_symbol(Exp,RT_Exp_Exp_MOD_Exp); 
				add_sibiling(add_sibiling(add_child($$,$1),new_terminal_symbol(MOD)),$3);
			}
		|	Exp ADD Exp
			{
				find_error;
				$$ = new_nonterminal_symbol(Exp,RT_Exp_Exp_ADD_Exp); 
				add_sibiling(add_sibiling(add_child($$,$1),new_terminal_symbol(ADD)),$3);
			}
		|	Exp MINUS Exp
			{
				find_error;
				$$ = new_nonterminal_symbol(Exp,RT_Exp_Exp_MINUS_Exp); 
				add_sibiling(add_sibiling(add_child($$,$1),new_terminal_symbol(MINUS)),$3);
			}
		|	Exp LSHIFT Exp	
			{
				find_error;
				$$ = new_nonterminal_symbol(Exp,RT_Exp_Exp_LSHIFT_Exp); 
				add_sibiling(add_sibiling(add_child($$,$1),new_terminal_symbol(LSHIFT)),$3);
			}
		|	Exp RSHIFT Exp
			{
				find_error;
				$$ = new_nonterminal_symbol(Exp,RT_Exp_Exp_RSHIFT_Exp); 
				add_sibiling(add_sibiling(add_child($$,$1),new_terminal_symbol(RSHIFT)),$3);
			}
		|	Exp RELOP Exp
			{
				find_error;
				$$ = new_nonterminal_symbol(Exp,RT_Exp_Exp_RELOP_Exp); 
				gval.val_rel = $2; 
				add_sibiling(add_sibiling(add_child($$,$1),new_terminal_symbol(RELOP)),$3);
			}
		|	Exp LSHIFTASSIGN Exp	
			{
				find_error;
				$$ = new_nonterminal_symbol(Exp,RT_Exp_Exp_LSHIFTASSIGN_Exp); 
				add_sibiling(add_sibiling(add_child($$,$1),new_terminal_symbol(LSHIFTASSIGN)),$3);
			}
		|	Exp RSHIFTASSIGN Exp
			{
				find_error;
				$$ = new_nonterminal_symbol(Exp,RT_Exp_Exp_RSHIFTASSIGN_Exp); 
				add_sibiling(add_sibiling(add_child($$,$1),new_terminal_symbol(RSHIFTASSIGN)),$3);
			}
		|	Exp LOGIAND Exp	
			{
				find_error;
				$$ = new_nonterminal_symbol(Exp,RT_Exp_Exp_LOGIAND_Exp); 
				add_sibiling(add_sibiling(add_child($$,$1),new_terminal_symbol(LOGIAND)),$3);
			}
		|	Exp LOGIOR Exp
			{
				find_error;
				$$ = new_nonterminal_symbol(Exp,RT_Exp_Exp_LOGIOR_Exp); 
				add_sibiling(add_sibiling(add_child($$,$1),new_terminal_symbol(LOGIOR)),$3);
			}
		|	Exp MULTASSIGN Exp	
			{
				find_error;
				$$ = new_nonterminal_symbol(Exp,RT_Exp_Exp_MULTASSIGN_Exp); 
				add_sibiling(add_sibiling(add_child($$,$1),new_terminal_symbol(MULTASSIGN)),$3);
			}
		|	Exp DIVASSIGN Exp
			{
				find_error;
				$$ = new_nonterminal_symbol(Exp,RT_Exp_Exp_DIVASSIGN_Exp); 
				add_sibiling(add_sibiling(add_child($$,$1),new_terminal_symbol(DIVASSIGN)),$3);
			}
		|	Exp ADDASSIGN Exp
			{
				find_error;
				$$ = new_nonterminal_symbol(Exp,RT_Exp_Exp_ADDASSIGN_Exp); 
				add_sibiling(add_sibiling(add_child($$,$1),new_terminal_symbol(ADDASSIGN)),$3);
			}
		|	Exp MINUSASSIGN Exp
			{
				find_error;
				$$ = new_nonterminal_symbol(Exp,RT_Exp_Exp_MINUSASSIGN_Exp); 
				add_sibiling(add_sibiling(add_child($$,$1),new_terminal_symbol(MINUSASSIGN)),$3);
			}
		|	Exp BITANDASSIGN Exp
			{
				find_error;
				$$ = new_nonterminal_symbol(Exp,RT_Exp_Exp_BITANDASSIGN_Exp); 
				add_sibiling(add_sibiling(add_child($$,$1),new_terminal_symbol(BITANDASSIGN)),$3);
			}
		|	Exp BITXORASSIGN Exp
			{
				find_error;
				$$ = new_nonterminal_symbol(Exp,RT_Exp_Exp_BITXORASSIGN_Exp); 
				add_sibiling(add_sibiling(add_child($$,$1),new_terminal_symbol(BITXORASSIGN)),$3);
			}
		|	Exp BITORASSIGN Exp	
			{
				find_error;
				$$ = new_nonterminal_symbol(Exp,RT_Exp_Exp_BITORASSIGN_Exp); 
				add_sibiling(add_sibiling(add_child($$,$1),new_terminal_symbol(BITORASSIGN)),$3);
			}
		|	Exp BITAND Exp
			{
				find_error;
				$$ = new_nonterminal_symbol(Exp,RT_Exp_Exp_BITAND_Exp); 
				add_sibiling(add_sibiling(add_child($$,$1),new_terminal_symbol(BITAND)),$3);
			}
		|	Exp BITXOR Exp
			{
				find_error;
				$$ = new_nonterminal_symbol(Exp,RT_Exp_Exp_BITXOR_Exp); 
				add_sibiling(add_sibiling(add_child($$,$1),new_terminal_symbol(BITXOR)),$3);
			}
		|	Exp BITOR Exp	
			{
				find_error;
				$$ = new_nonterminal_symbol(Exp,RT_Exp_Exp_BITOR_Exp); 
				add_sibiling(add_sibiling(add_child($$,$1),new_terminal_symbol(BITOR)),$3);
			}
		|	LOGINOT Exp
			{
				find_error;
				$$ = new_nonterminal_symbol(Exp,RT_Exp_LOGINOT_Exp); 
				add_sibiling(add_child($$,new_terminal_symbol(LOGINOT)),$2);
			}
		|	INCR Exp
			{
				find_error;
				$$ = new_nonterminal_symbol(Exp,RT_Exp_INCR_Exp); 
				add_sibiling(add_child($$,new_terminal_symbol(INCR)),$2);
			}
		|	DECR Exp
			{
				find_error;
				$$ = new_nonterminal_symbol(Exp,RT_Exp_DECR_Exp); 
				add_sibiling(add_child($$,new_terminal_symbol(DECR)),$2);
			}
		|	BITNOT Exp
			{
				find_error;
				$$ = new_nonterminal_symbol(Exp,RT_Exp_BITNOT_Exp); 
				add_sibiling(add_child($$,new_terminal_symbol(BITNOT)),$2);
			}
		|	MINUS Exp 
			{
				find_error;
				$$ = new_nonterminal_symbol(Exp,RT_Exp_MINUS_Exp); 
				add_sibiling(add_child($$,new_terminal_symbol(MINUS)),$2);
			}  %prec MINUS_NUM
		|	LP Exp RP	
			{
				find_error;
				$$ = new_nonterminal_symbol(Exp,RT_Exp_LP_Exp_RP); 
				add_sibiling(add_sibiling(add_child($$,new_terminal_symbol(LP)),$2),new_terminal_symbol(RP));
			}
		|	ID LP Args RP	
			{ 
				find_error;
				$$ = new_nonterminal_symbol(Exp,RT_Exp_ID_LP_Args_RP);
				gval.val_str = $1;
				add_sibiling(add_sibiling(add_sibiling(add_child($$,new_terminal_symbol(ID)),new_terminal_symbol(LP)),$3),new_terminal_symbol(RP));
			}
		|	ID LP RP 	
			{
				find_error;
				$$ = new_nonterminal_symbol(Exp,RT_Exp_ID_LP_RP);
				gval.val_str = $1;
				add_sibiling(add_sibiling(add_child($$,new_terminal_symbol(ID)),new_terminal_symbol(LP)),new_terminal_symbol(RP));
			}
		|	Exp LB Exp RB 
			{
				find_error;
				$$ = new_nonterminal_symbol(Exp,RT_Exp_Exp_LB_Exp_RB); 
				add_sibiling(add_sibiling(add_sibiling(add_child($$,$1),new_terminal_symbol(LB)),$3),new_terminal_symbol(RB));
				
			}
		|	Exp DOT ID	
			{
				find_error;
				$$ = new_nonterminal_symbol(Exp,RT_Exp_Exp_DOT_ID);
				gval.val_str = $3;
				add_sibiling(add_sibiling(add_child($$,$1),new_terminal_symbol(DOT)),new_terminal_symbol(ID));
			}
		|	ID 
			{
				find_error;
				$$ = new_nonterminal_symbol(Exp,RT_Exp_ID);
				gval.val_str = $1;
				add_child($$,new_terminal_symbol(ID));
			}
		|	INT 
			{
				find_error;
				$$ = new_nonterminal_symbol(Exp,RT_Exp_INT);
				gval.val_int = $1;
				add_child($$,new_terminal_symbol(INT));
			}
		|	Exp LB error 
			{
				error_type = 2;
				$$ = NULL;
				printf("Error at line %d: parentheses error\n", yylineno);
			}
/*
		|	{
				find_error;
				$$ = new_nonterminal_symbol(Exp,RT_Exp_NULL);
				//add_child($$,NULL);
			}
*/
		;
		
Args:		Exp COMMA Args 
			{
				find_error;
				$$ = new_nonterminal_symbol(Args,RT_Args_Exp_COMMA_Args); 
				add_sibiling(add_sibiling(add_child($$,$1),new_terminal_symbol(COMMA)),$3);
			}
		|	Exp	
			{
				find_error;
				$$ = new_nonterminal_symbol(Args,RT_Args_Exp);
				add_child($$,$1);
			}
		;
		
%%
extern int yychar, yylineno;

static void print_tok()
{
	if(yychar<255)
		{printf("%c",yychar);}
	else {
		switch(yychar)
		{
			case INT: printf("interger"); break;
			case SEMI: printf(";"); break;
			case COMMA: printf(","); break;
			case TYPE: printf("int"); break;
			case LP: printf("("); break;
			case RP: printf(")"); break;
			case LB: printf("["); break;
			case RB: printf("]"); break;
			case LC: printf("{"); break;
			case RC: printf("}"); break;
			case STRUCT: printf("struct"); break;
			case RETURN: printf("return"); break;
			case IF: printf("if"); break;
			case ELSE: printf("else"); break;
			case BREAK: printf("break"); break;
			case CONT: printf("continue"); break;
			case FOR: printf("for"); break;
			case ID: printf("ID"); break;
			case INCR: printf("++"); break;
			case DECR: printf("--"); break;
			case LSHIFTASSIGN: printf("<<="); break;
			case RSHIFTASSIGN: printf(">>="); break;
			case LSHIFT: printf("<<"); break;
			case RSHIFT: printf(">>"); break;
			case RELOP: printf(">= | <= | != | == | > | <"); break;
			case LOGIAND: printf("&&"); break;
			case LOGIOR: printf("||"); break;
			case ADDASSIGN: printf("+="); break;
			case MINUSASSIGN: printf("-="); break;
			case MULTASSIGN: printf("*="); break;
			case DIVASSIGN: printf("/="); break;
			case BITANDASSIGN: printf("&="); break;
			case BITXORASSIGN: printf("^="); break;
			case BITORASSIGN: printf("|="); break;
			case ASSIGN: printf("="); break;
			case MULT: printf("*"); break;
			case DIV: printf("/"); break;
			case MOD: printf("%%"); break;
			case ADD: printf("+"); break;
			case MINUS: printf("-"); break;
			case BITAND: printf("&"); break;
			case BITXOR: printf("^"); break;
			case BITOR: printf("|"); break;
			case LOGINOT: printf("!"); break;
			case BITNOT: printf("~"); break;
		}
	}
}
  
int yyerror(char * s){
	printf("[Line %d]: %s ", yylineno, s);
	print_tok();
	printf("\n");
	exit(0);
	return 0;
}

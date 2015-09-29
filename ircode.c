#include "def.h"

static struct TypeNode* TypeNode_int;
static struct irOperand* iop_zero; // #0
static struct irOperand* iop_one;  // #1


static int layer = 0;

// defining var/fun */
#define define_new_var(name,type,errno,errmsg) add_SymbolNode((name), SK_VAR, (type))

#define exp_op(p) (((p)->var_cmplx == VC_ADDR) ? (new_star_operand((p)->complex_type.base_addr->vid)) : \
		(((p)->int_type.numeric) ? new_int_const_operand((p)->int_type.ival) : (p)->int_type.rvop))

// variable id and label id
static int last_vid = 0;
static int last_label_id = 0;

// link-list header
static struct irNode* irheader;

// create irNode
static struct irNode* new_irnode(enum IRNodeType ntype)
{
	struct irNode* n;
	n = (struct irNode *)malloc(sizeof(struct irNode));
	n->ntype = ntype;
	n->prev = n->next = NULL;
	return n;
}

// create irOperand
static struct irOperand* new_iroperand(enum IROperandType operand_type)
{
	struct irOperand* op;
	op = (struct irOperand *)malloc(sizeof(struct irOperand));
	op->operand_type = operand_type;
	return op;
}

inline struct irOperand* new_operand_from_old(struct irOperand* old, int vid)
{
	struct irOperand* op = new_iroperand(old->operand_type);
	op->vid = vid;
	return op;
}

// create an irOperand of addr(&x), vid indicates the variable
inline struct irOperand* new_addr_operand(int vid)
{
	struct irOperand* op = new_iroperand(OT_ADDR);
	op->vid = vid;
	return op;
}

// create an irOperand of int constant(#1), ival indicates the value
inline struct irOperand* new_int_const_operand(int ival)
{
	struct irOperand* op = new_iroperand(OT_CONST_INT);
	op->ival = ival;
	return op;
}

// create an irOperand of an actual var
inline struct irOperand* new_var_operand(int vid)
{
	struct irOperand* op = new_iroperand(OT_VAR);
	op->vid = vid;
	return op;
}

// create an irOperand of a STAR var
inline struct irOperand* new_star_operand(int vid)
{
	struct irOperand* op = new_iroperand(OT_STAR);
	op->vid = vid;
	return op;
}

// next new variable id
inline int new_vid()
{
	return last_vid++;
}

// next label id 
inline int new_label_id()
{
	return last_label_id++;
}

// create an irNode: IT_LABEL
struct irNode* new_irnode_label(int label_id)
{
	struct irNode* n = new_irnode(IT_LABEL);
	n->label.label_id = label_id;
	return n;
}

// create an irNode: IT_FUNC_LABEL
struct irNode* new_irnode_func_label(char* fname)
{
	struct irNode* n = new_irnode(IT_FUNC_LABEL);
	n->func.fname = fname;
	return n;
}

// create an irNode: IT_ASSIGN, RSTAR, LSTAR
struct irNode* new_irnode_assign(struct irOperand *op1, struct irOperand *op2)
{
	struct irNode* n = new_irnode(IT_ASSIGN);
	n->assign.left = op1;
	n->assign.right = op2;
	return n;
}

// create an irNode: IT_ADD
struct irNode* new_irnode_add(struct irOperand *result, struct irOperand *op1, struct irOperand *op2)
{
	struct irNode* n = new_irnode(IT_ADD);
	n->binop.result = result;
	n->binop.op1 = op1;
	n->binop.op2 = op2;
	return n;
}
		
// create an irNode: IT_SUB
struct irNode* new_irnode_sub(struct irOperand *result, struct irOperand *op1, struct irOperand *op2)
{
	struct irNode* n = new_irnode(IT_SUB);
	n->binop.result = result;
	n->binop.op1 = op1;
	n->binop.op2 = op2;
	return n;
}
		
// create an irNode: IT_MUL
struct irNode* new_irnode_mul(struct irOperand *result, struct irOperand *op1, struct irOperand *op2)
{
	struct irNode* n = new_irnode(IT_MUL);
	n->binop.result = result;
	n->binop.op1 = op1;
	n->binop.op2 = op2;
	return n;
}
		
// create an irNode: IT_DIV
struct irNode* new_irnode_div(struct irOperand *result, struct irOperand *op1, struct irOperand *op2)
{
	struct irNode* n = new_irnode(IT_DIV);
	n->binop.result = result;
	n->binop.op1 = op1;
	n->binop.op2 = op2;
	return n;
}

// create an irNode: IT_MOD
struct irNode* new_irnode_mod(struct irOperand *result, struct irOperand *op1, struct irOperand *op2)
{
	struct irNode* n = new_irnode(IT_MOD);
	n->binop.result = result;
	n->binop.op1 = op1;
	n->binop.op2 = op2;
	return n;
}
		
		
// create an irNode: IT_LSHIFT
struct irNode* new_irnode_lshift(struct irOperand *result, struct irOperand *op1, struct irOperand *op2)
{
	struct irNode* n = new_irnode(IT_LSHIFT);
	n->binop.result = result;
	n->binop.op1 = op1;
	n->binop.op2 = op2;
	return n;
}
// create an irNode: IT_RSHIFT
struct irNode* new_irnode_rshift(struct irOperand *result, struct irOperand *op1, struct irOperand *op2)
{
	struct irNode* n = new_irnode(IT_RSHIFT);
	n->binop.result = result;
	n->binop.op1 = op1;
	n->binop.op2 = op2;
	return n;
}	

// irNode: IT_BITAND
struct irNode* new_irnode_bitand(struct irOperand *result, struct irOperand *op1, struct irOperand *op2)
{
	struct irNode* n = new_irnode(IT_BITAND);
	n->binop.result = result;
	n->binop.op1 = op1;
	n->binop.op2 = op2;
	return n;
}
		
// irNode: IT_BITOR
struct irNode* new_irnode_bitor(struct irOperand *result, struct irOperand *op1, struct irOperand *op2)
{
	struct irNode* n = new_irnode(IT_BITOR);
	n->binop.result = result;
	n->binop.op1 = op1;
	n->binop.op2 = op2;
	return n;
}

// irNode: IT_BITXOR
struct irNode* new_irnode_bitxor(struct irOperand *result, struct irOperand *op1, struct irOperand *op2)
{
	struct irNode* n = new_irnode(IT_BITXOR);
	n->binop.result = result;
	n->binop.op1 = op1;
	n->binop.op2 = op2;
	return n;
}

// irNode: IT_ADDASSIGN
struct irNode* new_irnode_addassign(struct irOperand *result, struct irOperand *op1, struct irOperand *op2)
{
	struct irNode* n = new_irnode(IT_ADDASSIGN);
	n->binop.result = result;
	n->binop.op1 = op1;
	n->binop.op2 = op2;
	return n;
}
		
// irNode: IT_SUBASSIGN
struct irNode* new_irnode_subassign(struct irOperand *result, struct irOperand *op1, struct irOperand *op2)
{
	struct irNode* n = new_irnode(IT_SUBASSIGN);
	n->binop.result = result;
	n->binop.op1 = op1;
	n->binop.op2 = op2;
	return n;
}
		
// irNode: IT_MULASSIGN
struct irNode* new_irnode_mulassign(struct irOperand *result, struct irOperand *op1, struct irOperand *op2)
{
	struct irNode* n = new_irnode(IT_MULASSIGN);
	n->binop.result = result;
	n->binop.op1 = op1;
	n->binop.op2 = op2;
	return n;
}
		
// irNode: IT_DIVASSIGN
struct irNode* new_irnode_divassign(struct irOperand *result, struct irOperand *op1, struct irOperand *op2)
{
	struct irNode* n = new_irnode(IT_DIVASSIGN);
	n->binop.result = result;
	n->binop.op1 = op1;
	n->binop.op2 = op2;
	return n;
}
			
// irNode: IT_LSHIFTASSIGN
struct irNode* new_irnode_lshiftassign(struct irOperand *result, struct irOperand *op1, struct irOperand *op2)
{
	struct irNode* n = new_irnode(IT_LSHIFTASSIGN);
	n->binop.result = result;
	n->binop.op1 = op1;
	n->binop.op2 = op2;
	return n;
}

// irNode: IT_RSHIFTASSIGN
struct irNode* new_irnode_rshiftassign(struct irOperand *result, struct irOperand *op1, struct irOperand *op2)
{
	struct irNode* n = new_irnode(IT_RSHIFTASSIGN);
	n->binop.result = result;
	n->binop.op1 = op1;
	n->binop.op2 = op2;
	return n;
}
		
// irNode: IT_BITANDASSIGN
struct irNode* new_irnode_bitandassign(struct irOperand *result, struct irOperand *op1, struct irOperand *op2)
{
	struct irNode* n = new_irnode(IT_BITANDASSIGN);
	n->binop.result = result;
	n->binop.op1 = op1;
	n->binop.op2 = op2;
	return n;
}
		
// irNode: IT_BITORASSIGN
struct irNode* new_irnode_bitorassign(struct irOperand *result, struct irOperand *op1, struct irOperand *op2)
{
	struct irNode* n = new_irnode(IT_BITORASSIGN);
	n->binop.result = result;
	n->binop.op1 = op1;
	n->binop.op2 = op2;
	return n;
}

// irNode: IT_BITXORASSIGN
struct irNode* new_irnode_bitxorassign(struct irOperand *result, struct irOperand *op1, struct irOperand *op2)
{
	struct irNode* n = new_irnode(IT_BITXORASSIGN);
	n->binop.result = result;
	n->binop.op1 = op1;
	n->binop.op2 = op2;
	return n;
}

// irNode: IT_BITNOT
struct irNode* new_irnode_bitnot(struct irOperand *result, 
		struct irOperand *op1)
{
	struct irNode* n = new_irnode(IT_BITNOT);
	n->binop.result = result;
	n->binop.op1 = op1;
	return n;
}

// irNode: IT_INCR
struct irNode* new_irnode_incr(struct irOperand *result, struct irOperand *op1)
{
	struct irNode* n = new_irnode(IT_INCR);
	n->binop.result = result;
	n->binop.op1 = op1;
	n->binop.op2 = iop_one;
	
	return n;
}
		
// irNode: IT_DECR
struct irNode* new_irnode_decr(struct irOperand *result, struct irOperand *op1)
{
	struct irNode* n = new_irnode(IT_DECR);
	n->binop.result = result;
	n->binop.op1 = op1;
	n->binop.op2 = iop_one;
	
	return n;
}
		
// create an irNode: IT_GOTO
struct irNode* new_irnode_goto(struct irNode *label)
{
	struct irNode* n = new_irnode(IT_GOTO);
	n->labelgoto.label = label;
	return n;
}

// create an irNode: IT_IFGOTO(X REL Y ? GOTO label_id) 
struct irNode* new_irnode_ifgoto(struct irOperand *x, struct irOperand *y, enum Relop rel, struct irNode *label)
{
	struct irNode* n = new_irnode(IT_IFGOTO);
	n->ifgoto.x = x;
	n->ifgoto.y = y;
	n->ifgoto.rel = rel;
	n->ifgoto.label = label;
	return n;
}

// create an irNode: IT_RETURN
struct irNode* new_irnode_return(struct irOperand *x)
{
	struct irNode* n = new_irnode(IT_RETURN);
	n->unaryop.x = x;
	return n;
}

// create an irNode: IT_DEC
struct irNode* new_irnode_dec(struct irOperand *x, int width, int def)
{
	struct irNode* n = new_irnode(IT_DEC);
	n->dec.x = x;
	n->dec.width = width;
	n->dec.def = def;
	return n;
}

// create an irNode: IT_ARRAY
struct irNode* new_irnode_array(struct irOperand *x, int width, int def, int *list)
{
	struct irNode* n = new_irnode(IT_ARRAY);
	n->array.x = x;
	n->array.width = width;
	n->array.def = def;
	n->array.list = list;
	return n;
}

//create an irNode: IT_VAR
struct irNode* new_irnode_var(struct irOperand *x, int def)
{
	struct irNode* n = new_irnode(IT_VAR);
	n->unaryop.x = x;
	n->unaryop.def = def;
	return n;
}

// create an irNode: IT_ST
struct irNode* new_irnode_store(struct irOperand *x)
{
	struct irNode* n = new_irnode(IT_ST);
	n->unaryop.x = x;
	return n;
}

// create an irNode: IT_CALL, result, function name
struct irNode* new_irnode_call(struct irOperand *x, char* fname)
{
	struct irNode* n = new_irnode(IT_CALL);
	n->call.x = x;
	n->call.fname = fname; /* CAUTION: it's ok cuz fname is a pointer, and 
		the real string is in TreeNode */
	return n;
}

// create an irNode: IT_PARAM
struct irNode* new_irnode_param(struct irOperand *v)
{
	struct irNode* n = new_irnode(IT_PARAM);
	n->unaryop.x = v;
	return n;
}

// create an irNode: IT_READ
struct irNode* new_irnode_read(struct irOperand *x)
{
	struct irNode* n = new_irnode(IT_READ);
	n->unaryop.x = x;
	return n;
}

// create an irNode: IT_WRITE
struct irNode* new_irnode_write(struct irOperand *x)
{
	struct irNode* n = new_irnode(IT_WRITE);
	n->unaryop.x = x;
	return n;
}

// reverse ifgoto condition//
inline struct irNode* reverse_irnode_ifgoto(struct irNode* n)
{
	n->ifgoto.rel = reverse_relop(n->ifgoto.rel);
	return n;
}

// get the header
inline struct irNode* ir_header()
{
	return irheader;
}

// get the last element
inline struct irNode* ir_tail()
{
	return irheader->prev;
}

// insert after
struct irNode* ir_ins_after(struct irNode* prev, struct irNode* cur)
{
	struct irNode* p = cur;
	if (cur == NULL || prev == NULL)
		return NULL;
	
	cur->prev = prev;
	while (p->next)
		p = p->next;
	p->next = cur->next;
	if (prev->next)
		prev->next->prev = p;
	else 
		irheader->prev = p;
	prev->next = cur;
	return p;
}

//insert tail
struct irNode* ir_ins_tail(struct irNode* cur)
{
	return ir_ins_after(ir_tail(), cur);
}

// delete one irNode, returns its next
struct irNode* ir_del_node(struct irNode* cur)
{
	struct irNode* p = cur->next;
	if (cur->next)
		cur->next->prev = cur->prev;
	cur->prev->next = cur->next;
	free(cur);
	return p;
}

// prefix
inline char* get_var_prefix(int vid)
{
	return "t";
}

int get_output_vid(int vid)
{
	return vid;
}

void print_operand(struct irOperand* x, FILE * fout)
{
	if (x->operand_type == OT_ADDR)
	{
		fprintf(fout, "&%s%d", get_var_prefix(x->vid), get_output_vid(x->vid));
		printf("&%s%d", get_var_prefix(x->vid), get_output_vid(x->vid));
	}
	else if (x->operand_type == OT_STAR)
	{
		fprintf(fout, "*%s%d", get_var_prefix(x->vid), get_output_vid(x->vid));
		printf("*%s%d", get_var_prefix(x->vid), get_output_vid(x->vid));
	}
	else if (x->operand_type == OT_CONST_INT)
	{
		fprintf(fout, "#%d", x->ival);
		printf("#%d", x->ival);
	}
	else
	{
		fprintf(fout, "%s%d", get_var_prefix(x->vid), get_output_vid(x->vid));
		printf("%s%d", get_var_prefix(x->vid), get_output_vid(x->vid));
	}
}

void print_relop(enum Relop r, FILE * fout)
{
	if (r == R_NOLESSTHAN)
	{
		fprintf(fout, " >= ");
		printf(" >= ");
	}
	else if (r == R_GREATER)
	{
		fprintf(fout, " > ");
		printf(" > ");
	}
	else if (r == R_NOGREATERTHAN)
	{
		fprintf(fout, " <= ");
		printf(" <= ");
	}
	else if (r == R_LESS)
	{
		fprintf(fout, " < ");
		printf(" < ");
	}
	else if (r == R_NOTEQUAL)
	{
		fprintf(fout, " != ");
		printf(" != ");
	}
	else if (r == R_EQUAL)
	{
		fprintf(fout, " == ");
		printf(" == ");
	}
}
void print_assign(FILE * fout)
{
	fprintf(fout, " := ");
	printf(" := ");
}
void print_binop(char * sig, struct irNode *p, FILE * fout) 
{	
	print_operand(p->binop.result, fout);
	print_assign(fout);
	print_operand(p->binop.op1, fout); 
	fprintf(fout, " %s ", sig);
	printf(" %s ", sig);
	print_operand(p->binop.op2, fout);
}

#define LBL_PREFIX "label"

// display the code!
void ir_display(FILE * fout)
{	
	struct irNode *p = irheader;
	int arraylistsize, j;
	//one = new_int_const_operand(1); // #1
	while (p = p->next)
	{
		switch (p->ntype)
		{
			case IT_ASSIGN:
				//printf("assign\t");
				print_operand(p->assign.left, fout);
				print_assign(fout);
				print_operand(p->assign.right, fout);
				break;
			case IT_LABEL:
				//printf("label\t");
				fprintf(fout, "LABEL %s%d :", LBL_PREFIX, p->label.label_id);
				printf("LABEL %s%d :", LBL_PREFIX, p->label.label_id);
				break;
			case IT_ADD:
				//printf("add\t");
				print_binop("+", p, fout);
				break;
			case IT_SUB:
				//printf("sub\t");
				print_binop("-", p, fout);
				break;
			case IT_MUL:
				//printf("mul\t");
				print_binop("*", p, fout);
				break;
			case IT_DIV:
				//printf("div\t");
				print_binop("/", p, fout);
				break;
			case IT_MOD:
				//printf("mod\t");
				print_binop("%", p, fout);
				break;
			case IT_LSHIFT:
				//printf("lshift\t");
				print_binop("<<", p, fout);
				break;
			case IT_RSHIFT:
				//printf("rshift\t");
				print_binop(">>", p, fout);
				break;
			case IT_BITAND:
				//printf("bitand\t");
				print_binop("&", p, fout);
				break;
			case IT_BITOR:
				//printf("bitor\t");
				print_binop("|", p, fout);
				break;
			case IT_BITNOT:
				//printf("bitnot\t");
				print_operand(p->binop.result, fout);
				print_assign(fout);
				fprintf(fout, " ~ ");
				printf(" ~ ");
				print_operand(p->binop.op1, fout);
				break;
			case IT_BITXOR:
				//printf("bitxor\t");
				print_binop("^", p, fout);
				break;
			case IT_ADDASSIGN:
				//printf("addassign\t");
				print_binop("+", p, fout);
				break;
			case IT_SUBASSIGN:
				//printf("subassign\t");
				print_binop("-", p, fout);
				break;
			case IT_MULASSIGN:
				//printf("mulassign\t");
				print_binop("*", p, fout);
				break;
			case IT_DIVASSIGN:
				//printf("divassign\t");
				print_binop("/", p, fout);
				break;
			case IT_LSHIFTASSIGN:
				//printf("lshiftassign\t");
				print_binop("<<", p, fout);
				break;
			case IT_RSHIFTASSIGN:
				//printf("rshiftassign\t");
				print_binop(">>", p, fout);
				break;
			case IT_BITANDASSIGN:
				//printf("bitandassign\t");
				print_binop("&", p, fout);
				break;
			case IT_BITORASSIGN:
				//printf("bitorassign\t");
				print_binop("|", p, fout);
				break;
			case IT_BITXORASSIGN:
				//printf("bitxorassign\t");
				print_binop("^", p, fout);
				break;
			case IT_INCR:
				//printf("incr\t");
				print_binop("+", p, fout);
				break;
			case IT_DECR:
				//printf("decr\t");
				print_binop("-", p, fout);
				break;
			case IT_RSTAR: 
			case IT_LSTAR:
				break;
			case IT_GOTO:
				//printf("goto\t");
				fprintf(fout, "GOTO %s%d", LBL_PREFIX, p->labelgoto.label->label.label_id);
				printf("GOTO %s%d", LBL_PREFIX, p->labelgoto.label->label.label_id);
				break;
			case IT_IFGOTO:
				//printf("ifgoto\t");
				fprintf(fout, "IF ");
				printf("IF ");
				print_operand(p->ifgoto.x, fout);
				print_relop(p->ifgoto.rel, fout);
				print_operand(p->ifgoto.y, fout);
				fprintf(fout, " GOTO %s%d", LBL_PREFIX, p->ifgoto.label->label.label_id);
				printf(" GOTO %s%d", LBL_PREFIX, p->ifgoto.label->label.label_id);
				break;
			case IT_RETURN:
				//printf("return\t");
				fprintf(fout, "RETURN ");
				printf("RETURN ");
				print_operand(p->unaryop.x, fout);
				break;
			case IT_DEC:
				//printf("dec\t");
				fprintf(fout, "DEC ");				
				printf("DEC ");
				print_operand(p->dec.x, fout);
				fprintf(fout, " %d", p->dec.width);
				printf(" %d", p->dec.width);
				break;		
			case IT_ARRAY:
				//printf("array\t");
				fprintf(fout, "ARRAY ");				
				printf("ARRAY ");
				print_operand(p->array.x, fout);
				fprintf(fout, " %d ", p->array.width);
				printf(" %d ", p->array.width);
				arraylistsize = p->array.width/4;
				for(j = 0; j < arraylistsize; j++)
				{
					fprintf(fout, " %d", p->array.list[j]);
					printf(" %d", p->array.list[j]);
				}
				break;		
			case IT_VAR:
				//printf("var\t");
				fprintf(fout, "VAR ");
				printf("VAR ");
				print_operand(p->unaryop.x, fout);
				break;
			case IT_ST:
				//printf("store\t");
				fprintf(fout, "STORE ");
				printf("STORE ");
				print_operand(p->unaryop.x, fout);
				break;
			case IT_CALL:
				//printf("call\t");
				print_operand(p->call.x, fout);
				fprintf(fout, " := CALL %s", p->call.fname);
				printf(" := CALL %s", p->call.fname);
				break;
			case IT_PARAM:
				//printf("param\t");
				fprintf(fout, "PARAM ");
				printf("PARAM ");
				print_operand(p->unaryop.x, fout);
				break;
			case IT_READ:
				//printf("read\t");
				fprintf(fout, "READ ");
				printf("READ ");
				print_operand(p->unaryop.x, fout);
				break;
			case IT_WRITE:
				//printf("write\t");
				fprintf(fout, "WRITE ");
				printf("WRITE ");
				print_operand(p->unaryop.x, fout);
				break;
			case IT_FUNC_LABEL:
				//printf("function\t");
				fprintf(fout, "FUNCTION %s :", p->func.fname);
				printf("FUNCTION %s :", p->func.fname);
				break;
			//default:	printf("others");
		}
		fprintf(fout, "\n");
		printf("\n");
	}
}

//initial
void ir_init()
{
	irheader = new_irnode(IT_HEADER);
	irheader->prev = irheader;
}


//IR code

int judge_relop(int x, int y, enum Relop r)
{
	switch (r)
	{
		case R_GREATER: return x > y;
		case R_LESS: return x < y;
		case R_NOGREATERTHAN: return x <= y;
		case R_NOLESSTHAN: return x >= y;
		case R_EQUAL: return x == y;
		case R_NOTEQUAL: return x != y;
	}
	return 0;
}

void do_assign(struct TreeNode* lc, struct TreeNode *rc)
{
	struct irOperand *rop, *lop;
	struct SymbolNode* n;
	//printf("assign\n");
	if (lc->vtype->kind > LAST_BASIC_TYPE) {}
	else
	{
		rop = exp_op(rc);
		lop = exp_op(lc);
		ir_ins_tail(new_irnode_assign(lop, rop));
	}
}

void convert_type(struct TreeNode* p, struct TreeNode* cur)
{
	struct irNode* n;
	struct irOperand* rop;
	if (cur->exp_type == p->exp_type)
		return ;
	if (p->exp_type == ET_NUM) {}	//type error!
	else
	{
		if (!cur->int_type.numeric)
		{
			if (!p->label_false)//exp!=0
				ir_ins_tail(new_irnode_ifgoto(cur->int_type.rvop, iop_zero, R_NOTEQUAL, p->label_true));
			else if (!p->label_true)//exp==0
				ir_ins_tail(new_irnode_ifgoto(cur->int_type.rvop, iop_zero, R_EQUAL, p->label_false));
			else 
			{
				ir_ins_tail(new_irnode_ifgoto(cur->int_type.rvop, iop_zero, R_NOTEQUAL, p->label_true));
				ir_ins_tail(new_irnode_goto(p->label_false));
			}
		}
		else
		{
			if ((cur->int_type.ival == 0) ^ (p->label_true != NULL))
				ir_ins_tail(new_irnode_goto(((cur->int_type.ival) ? p->label_true : p->label_false)));
		}
		cur->exp_type = p->exp_type;
	}
}

// change num to bool
void num_to_bool(struct TreeNode* cur, struct TreeNode* p)
{
	if (p->exp_type == ET_BOOL && cur->vtype == TypeNode_int) 
	{
		if (cur->int_type.numeric) 
		{
			if (cur->int_type.ival)
			{
				if (cur->label_true)
					ir_ins_tail(new_irnode_goto(cur->label_true));
			}
			else
			{
				if (cur->label_false)
					ir_ins_tail(new_irnode_goto(cur->label_false));
			}
		}
		else
		{
			/*if t1 == 0 goto false
				goto true*/
			if (cur->label_false && cur->label_true)
			{
				ir_ins_tail(new_irnode_ifgoto(exp_op(cur), iop_zero, R_NOTEQUAL, cur->label_true));
				ir_ins_tail(new_irnode_goto(cur->label_false));
			}
			else if (cur->label_false)
				ir_ins_tail(new_irnode_ifgoto(exp_op(cur), iop_zero, R_EQUAL, cur->label_false));
			else if (cur->label_true)
				ir_ins_tail(new_irnode_ifgoto(exp_op(cur), iop_zero, R_NOTEQUAL, cur->label_true));
		}
	}
}

int arraycnt = 0;
int arraysize = 0;
int arraylist[100000];		
// intermediate code generate		
static void ir_parse(struct TreeNode* cur, struct TreeNode* p)
{
	struct SymbolNode* n;
	struct TypeStructList* f;
	struct irNode *in, *in2;
	struct irOperand *top, *rop, *lop; // tmp operand
	int rvid, tvid;
	int j = 0;
	layer++;
	if (cur->type == NT_Exp) 
	{
		cur->exp_type = p->exp_type;
	}
	switch (cur->reduce_type)
	{
		case RT_Program_ExtDefS:
			
			new_scope(); 
			// define READ and WRITE functions
			add_SymbolNode("read" , SK_VAR, new_TypeNode_func(new_func(TypeNode_int, new_fieldlist_link(TypeNode_int, "x", NULL), FUNC_DEFINED)));
			add_SymbolNode("write" , SK_VAR, new_TypeNode_func(new_func(TypeNode_int, new_fieldlist_link(TypeNode_int, "x", NULL), FUNC_DEFINED)));
			ir_parse(child1(cur), cur);
			// check functions that declared only
			n = cur_scope();
			del_current_scope();
			break;
		case RT_ExtDefS_ExtDef_ExtDefS:
			ir_parse(child1(cur), cur);
			ir_parse(child2(cur), cur);
			break;
		case RT_ExtDefS_NULL:
			break;
		case RT_ExtDef_Spec_ExtVarS_SEMI:
			ir_parse(child1(cur), cur);
			cur->vtype = child1(cur)->vtype;
			ir_parse(child2(cur), cur);
			break;
		case RT_ExtDef_Spec_SEMI:
			ir_parse(child1(cur), cur);
			cur->vtype = child1(cur)->vtype;
			break;
		case RT_ExtDef_Spec_Func_StmtBlock:
			new_scope();
			ir_parse(child1(cur), cur);
			cur->return_type = child1(cur)->vtype;	
			ir_ins_tail(new_irnode_func_label(child2(cur)->name));
			ir_parse(child2(cur), cur);		
			if (child2(cur)->name != NULL)
				if (n = find_SymbolNode(child2(cur)->name, SK_VAR)) // repeat defined
					n->type->function->defined = FUNC_DEFINED;			
				else
				// build up the SymbolNode
					add_SymbolNode_parent(child2(cur)->name, SK_VAR, 
						new_TypeNode_func(new_func(child1(cur)->return_type, child2(cur)->field, FUNC_DEFINED)));
			ir_parse(child3(cur), cur);
			del_current_scope();// function is over
			
			break;
		case RT_ExtDef_Spec_Func_SEMI:
			new_scope();
			ir_parse(child1(cur), cur);
			cur->return_type = child1(cur)->vtype;
			ir_parse(child2(cur), cur);
			if (child2(cur)->name != NULL)
				if (n = find_SymbolNode(child2(cur)->name, SK_VAR)) 
					n->type->function->defined = FUNC_DEFINED;
				else
					// build up the SymbolNode
					add_SymbolNode_parent(child2(cur)->name, SK_VAR, 
						new_TypeNode_func(new_func(child1(cur)->return_type, child2(cur)->field, FUNC_UNDEFINED)));
			del_current_scope(); // function is over

			break;
		case RT_ExtVarS_Dec:
			cur->struct_defined = p->struct_defined;
			cur->vtype = p->vtype;
			ir_parse(child1(cur), cur);
			cur->name = child1(cur)->name;
			break;
		case RT_ExtVarS_Dec_COMMA_ExtVarS:
			cur->struct_defined = p->struct_defined;
			cur->vtype = p->vtype;
			ir_parse(child1(cur), cur);
			cur->name = child1(cur)->name;
			ir_parse(child3(cur), cur);			
			break;
		case RT_Spec_TYPE:
			if (strcmp(child1(cur)->val.val_str, "int") == 0)
				cur->vtype = TypeNode_int;
			else {}
			break;
		case RT_Spec_StSpec:
			ir_parse(child1(cur), cur);
			cur->vtype = child1(cur)->vtype; 
	
			break;
		case RT_StSpec_STRUCT_OptTag_LC_DefS_RC:
			ir_parse(child2(cur), cur);
			new_scope();
			cur->struct_defined = 1; 
			ir_parse(child4(cur), cur);
			f = create_fieldlist_from_cur_scope();

			del_current_scope(); // function is over
			cur->vtype = new_TypeNode_struct(f);
			if (child2(cur)->name != NULL) 
			{
				add_SymbolNode(child2(cur)->name, SK_DEF, cur->vtype);
			}
			break;
		case RT_StSpec_STRUCT_Tag:
			ir_parse(child2(cur), cur);
			n = find_SymbolNode(child2(cur)->name, SK_DEF);
			cur->vtype = n->type;
			break;
		case RT_OptTag_ID:
			cur->name = child1(cur)->val.val_str;
			break;
		case RT_OptTag_NULL:
			cur->name = NULL;
			break;
		case RT_Tag_ID:
			cur->name = child1(cur)->val.val_str;
			break;
		case RT_Var_ID:
			cur->name = child1(cur)->val.val_str;
			cur->vtype = p->vtype;
			break;
		case RT_Var_Var_LB_INT_RB:
			cur->vtype = new_TypeNode_arr_by_TypeNode(p->vtype, child3(cur)->val.val_int);
			cur->val.val_int = child3(cur)->val.val_int;
			ir_parse(child1(cur), cur);
			cur->name = child1(cur)->name;
			cur->vtype = child1(cur)->vtype;

			break;
		case RT_Func_ID_LP_ParaS_RP:
			cur->name = child1(cur)->val.val_str;
			ir_parse(child3(cur), cur);
			cur->field = child3(cur)->field;
			f = cur->field;

			break;
		case RT_Func_ID_LP_RP:
			cur->name = child1(cur)->val.val_str;
			cur->field = NULL;
			break;
		case RT_ParaS_Para_COMMA_ParaS:
			ir_parse(child1(cur), cur);
			ir_parse(child3(cur), cur);
			cur->field = new_fieldlist_link(child1(cur)->vtype, child1(cur)->name, child3(cur)->field);
			
			break;
		case RT_ParaS_Para:
			ir_parse(child1(cur), cur);
			cur->field = new_fieldlist_link(child1(cur)->vtype, child1(cur)->name, NULL);
			break;
		case RT_Para_Spec_Var:
			ir_parse(child1(cur), cur);
			cur->vtype = child1(cur)->vtype;
			ir_parse(child2(cur), cur);
			cur->name = child2(cur)->name;
			cur->vtype = child2(cur)->vtype;
			set_SymbolNode_vid(n = add_SymbolNode(cur->name, SK_VAR, cur->vtype), new_param_vid(), (cur->vtype->kind <= LAST_BASIC_TYPE) ? VT_VARIABLE : VT_PTR);
			if (cur->vtype->kind > LAST_BASIC_TYPE)
				ir_ins_tail(new_irnode_param(new_var_operand(get_SymbolNode_vid(n))));
			else 
				ir_ins_tail(new_irnode_param(new_var_operand(get_SymbolNode_vid(n))));

			break;
		case RT_StmtBlock_LC_DefS_StmtS_RC:
			cur->return_type = p->return_type; 
			if (p->reduce_type != RT_ExtDef_Spec_Func_StmtBlock) 
				new_scope(); 
			ir_parse(child2(cur), cur);
			ir_parse(child3(cur), cur);
			if (p->reduce_type != RT_ExtDef_Spec_Func_StmtBlock)
				del_current_scope(); 

			break;
		case RT_StmtS_Stmt_StmtS:
			cur->return_type = p->return_type; 
			ir_parse(child1(cur), cur);
			ir_parse(child2(cur), cur);
			break;
		case RT_StmtS_NULL:
			break;
		case RT_Stmt_Exp_SEMI:
			cur->return_type = p->return_type;
			ir_parse(child1(cur), cur);
			break;
		case RT_Stmt_StmtBlock:
			cur->return_type = p->return_type;
			ir_parse(child1(cur), cur);
			break;
		case RT_Stmt_RETURN_Exp_SEMI:
			cur->exp_type = ET_NUM;
			cur->return_type = p->return_type;
			ir_parse(child2(cur), cur);
			ir_ins_tail(new_irnode_return(exp_op(child2(cur))));
			break;
		case RT_Stmt_IF_LP_Exp_RP_Stmt:	
			cur->exp_type = ET_BOOL;
			child3(cur)->label_true = cur->label_true = new_irnode_label(new_label_id());
			child3(cur)->label_false = cur->label_false = new_irnode_label(new_label_id());
			
			cur->return_type = p->return_type;
			ir_parse(child3(cur), cur);
			ir_ins_tail(cur->label_true);
			ir_parse(child5(cur), cur);
			ir_ins_tail(cur->label_false);
			break;
		case RT_Stmt_IF_LP_Exp_RP_Stmt_ELSE_Stmt:
			cur->exp_type = ET_BOOL;
			child3(cur)->label_true = cur->label_true = new_irnode_label(new_label_id());
			child3(cur)->label_false = cur->label_false = new_irnode_label(new_label_id());
			in = new_irnode_label(new_label_id()); 

			cur->return_type = p->return_type;
			ir_parse(child3(cur), cur);
			ir_ins_tail(cur->label_true);
			ir_parse(child5(cur), cur);
			ir_ins_tail(new_irnode_goto(in));
			ir_ins_tail(cur->label_false);
			ir_parse(child7(cur), cur);
			ir_ins_tail(in);
			break;
		case RT_Stmt_FOR_LP_Exp_SEMI_Exp_SEMI_Exp_RP_Stmt:
			cur->exp_type = ET_BOOL;
			cur->label_true = new_irnode_label(new_label_id());
			child5(cur)->label_false = cur->label_false = new_irnode_label(new_label_id());
			child5(cur)->label_true = NULL;
			//for break and continue
			child9(cur)->label_true = cur->label_true;
			//printf("%d\n",child9(cur)->label_true->label.label_id);
			child9(cur)->label_false = cur->label_false;	
			//printf("%d\n",child9(cur)->label_false->label.label_id);	

			cur->return_type = p->return_type;
			ir_parse(child3(cur), cur);	//initial
			ir_ins_tail(cur->label_true);	//label true
			ir_parse(child5(cur), cur);	//judge
			//ir_parse(child7(cur), cur);		
			ir_parse(child9(cur), cur);	
			ir_parse(child7(cur), cur);		
			ir_ins_tail(new_irnode_goto(cur->label_true));
			ir_ins_tail(cur->label_false);
			break;
		case RT_Stmt_FOR_LP_SEMI_Exp_SEMI_Exp_RP_Stmt:
			cur->exp_type = ET_BOOL;
			cur->label_true = new_irnode_label(new_label_id());
			child4(cur)->label_false = cur->label_false = new_irnode_label(new_label_id());
			child4(cur)->label_true = NULL;
			//for break and continue
			child8(cur)->label_true = cur->label_true;
			//printf("%d\n",child9(cur)->label_true->label.label_id);
			child8(cur)->label_false = cur->label_false;	
			//printf("%d\n",child9(cur)->label_false->label.label_id);	

			cur->return_type = p->return_type;
			ir_ins_tail(cur->label_true);	//label true
			ir_parse(child4(cur), cur);	//judge
			//ir_parse(child7(cur), cur);		
			ir_parse(child8(cur), cur);	
			ir_parse(child6(cur), cur);		
			ir_ins_tail(new_irnode_goto(cur->label_true));
			ir_ins_tail(cur->label_false);
			break;
		case RT_Stmt_WHILE_LP_Exp_RP_Stmt:			
			cur->exp_type = ET_BOOL;
			cur->label_true = new_irnode_label(new_label_id());
			child3(cur)->label_false = cur->label_false = new_irnode_label(new_label_id());
			child3(cur)->label_true = NULL;		
			//for break and continue	
			child5(cur)->label_true = cur->label_true;
			child5(cur)->label_false = cur->label_false;

			cur->return_type = p->return_type;
			ir_ins_tail(cur->label_true);
			ir_parse(child3(cur), cur);
			ir_parse(child5(cur), cur);
			ir_ins_tail(new_irnode_goto(cur->label_true));
			ir_ins_tail(cur->label_false);
			break;
		case RT_Stmt_CONT_SEMI:
			cur->return_type = p->return_type;
			ir_ins_tail(new_irnode_goto(p->label_true));
			//printf("ccontinue over\n");
			break;
		case RT_Stmt_BREAK_SEMI:
			cur->return_type = p->return_type;
			//printf("break begin:");
			//printf("%d\n", cur->label_false->label.label_id);
			ir_ins_tail(new_irnode_goto(p->label_false));
			//printf("break over %d\n", cur->label_false->label.label_id);
			break;
		case RT_DefS_Def_DefS:
			cur->struct_defined = p->struct_defined;
			ir_parse(child1(cur), cur);
			ir_parse(child2(cur), cur);
			break;
		case RT_DefS_NULL:
			break;
		case RT_Def_Spec_DecS_SEMI:
			cur->struct_defined = p->struct_defined;
			ir_parse(child1(cur), cur);
			cur->vtype = child1(cur)->vtype;
			ir_parse(child2(cur), cur);
			break;
		case RT_DecS_Dec:
			cur->struct_defined = p->struct_defined;
			cur->vtype = p->vtype;
			ir_parse(child1(cur), cur);
			cur->name = child1(cur)->name;
			break;
		case RT_DecS_Dec_COMMA_DecS:
			cur->struct_defined = p->struct_defined;
			cur->vtype = p->vtype;
			ir_parse(child1(cur), cur);
			cur->name = child1(cur)->name;
			ir_parse(child3(cur), cur);

			break;
		case RT_Dec_Var:
			cur->vtype = p->vtype;
			ir_parse(child1(cur), cur);
			cur->vtype = child1(cur)->vtype;
			cur->name = child1(cur)->name;
			set_SymbolNode_vid(n = add_SymbolNode(cur->name, SK_VAR, cur->vtype), new_vid(), VT_VARIABLE); 
			if (!p->struct_defined && cur->vtype->kind > LAST_BASIC_TYPE)
			{
				if(cur->vtype->kind == TK_ARRAY) 
				{
					cur->arraydec = 1;
					for(j = 0; j < arraysize; j++)
					{
						cur->vtype->array.list[j] = arraylist[j];
						//printf("%d\n", cur->vtype->array.list[j]);
					}
					ir_ins_tail(new_irnode_array(new_var_operand(get_SymbolNode_vid(n)), n->type->width, 0, cur->vtype->array.list));
				}//cur->arraydec = 1;
				else 
				{
					ir_ins_tail(new_irnode_dec(new_var_operand(get_SymbolNode_vid(n)), n->type->width, 0));

					//printf("	%d", n->type->width);
				}
			}
			else if(!p->struct_defined)
			{
				child1(cur)->int_type.rvop = new_var_operand(get_SymbolNode_vid(n));
				ir_ins_tail(new_irnode_var(new_var_operand(get_SymbolNode_vid(n)), 0));
				//ir_ins_tail(new_irnode_assign(child1(cur)->int_type.rvop, iop_zero));
			}
			break;
		case RT_Dec_Var_ASSIGN_Exp:
			cur->struct_defined = p->struct_defined;
			cur->exp_type = ET_NUM;
			cur->struct_defined = p->struct_defined;
			cur->vtype = p->vtype;
			ir_parse(child1(cur), cur);
			cur->vtype = child1(cur)->vtype;
			cur->name = child1(cur)->name;
			set_SymbolNode_vid(n = add_SymbolNode(cur->name, SK_VAR, cur->vtype), new_vid(), VT_VARIABLE);
			ir_ins_tail(new_irnode_var(new_var_operand(get_SymbolNode_vid(n)), 1));
			ir_parse(child3(cur), cur);
			child1(cur)->var_cmplx =  VC_BASIC;
			child1(cur)->int_type.numeric = FALSE;
			child1(cur)->int_type.rvop = new_var_operand(get_SymbolNode_vid(n));
			do_assign(child1(cur), child3(cur));
			cur->int_type = child1(cur)->int_type;
			num_to_bool(cur, p);
			break;
		case RT_Dec_Var_ASSIGN_LC_Args_RC:
			cur->struct_defined = p->struct_defined;
			cur->vtype = p->vtype;
			cur->arraydec = 1; //array not function parameter
			ir_parse(child1(cur), cur);
			cur->vtype = child1(cur)->vtype;
			cur->name = child1(cur)->name;			
			set_SymbolNode_vid(n = add_SymbolNode(cur->name, SK_VAR, cur->vtype), new_vid(), VT_VARIABLE); 
			ir_parse(child4(cur), cur);
			//printf(" %d\n", child1(cur)->val.val_int);
			arraysize =  child1(cur)->val.val_int;
			for(j = 0; j < arraysize; j++)
			{
				cur->vtype->array.list[j] = arraylist[j];
				//printf("%d\n", cur->vtype->array.list[j]);
			}
			ir_ins_tail(new_irnode_array(new_var_operand(get_SymbolNode_vid(n)), n->type->width, 1, cur->vtype->array.list));
			for(j = 0; j < 100000; j++)
				arraylist[j] = 0;
			arraycnt = 0;
			break;
		case RT_Exp_Exp_ASSIGN_Exp:
			cur->exp_type = ET_NUM;
			ir_parse(child1(cur), cur);
			ir_parse(child3(cur), cur);
			cur->lvalue = child1(cur)->lvalue;
			cur->vtype = child1(cur)->vtype;
			
			do_assign(child1(cur), child3(cur));
			cur->int_type = child1(cur)->int_type; 
			num_to_bool(cur, p);
			break;
			
#define Exp_Exp_Op_Exp \
	ir_parse(child1(cur), cur); \
	ir_parse(child3(cur), cur); \
	(cur->lvalue = LV_RVALUE,  \
	cur->vtype = child1(cur)->vtype);

		case RT_Exp_Exp_MULT_Exp:
			cur->exp_type = ET_NUM;
			Exp_Exp_Op_Exp
			cur->var_cmplx = VC_BASIC;
			if (child1(cur)->int_type.numeric && child3(cur)->int_type.numeric)
			{
				cur->int_type.numeric = TRUE;
				cur->int_type.ival = child1(cur)->int_type.ival * child3(cur)->int_type.ival;
			}
			else
			{
				cur->int_type.numeric = FALSE;
				ir_ins_tail(new_irnode_mul(cur->int_type.rvop = new_var_operand(new_vid()), exp_op(child1(cur)), exp_op(child3(cur))));
			}
			num_to_bool(cur, p);
			break;
		case RT_Exp_Exp_DIV_Exp:
			cur->exp_type = ET_NUM;
			Exp_Exp_Op_Exp
			cur->var_cmplx = VC_BASIC;
			if (child1(cur)->int_type.numeric && child3(cur)->int_type.numeric)
			{
				cur->int_type.numeric = TRUE;
				cur->int_type.ival = child1(cur)->int_type.ival / child3(cur)->int_type.ival;
			}
			else
			{
				cur->int_type.numeric = FALSE;
				ir_ins_tail(new_irnode_div(cur->int_type.rvop = new_var_operand(new_vid()), exp_op(child1(cur)), exp_op(child3(cur))));
			}
			num_to_bool(cur, p);
			break;
		case RT_Exp_Exp_MOD_Exp:
			cur->exp_type = ET_NUM;
			Exp_Exp_Op_Exp
			cur->var_cmplx = VC_BASIC;
			if (child1(cur)->int_type.numeric && child3(cur)->int_type.numeric)
			{
				cur->int_type.numeric = TRUE;
				cur->int_type.ival = child1(cur)->int_type.ival % child3(cur)->int_type.ival;
			}
			else
			{
				cur->int_type.numeric = FALSE;
				ir_ins_tail(new_irnode_mod(cur->int_type.rvop = new_var_operand(new_vid()), exp_op(child1(cur)), exp_op(child3(cur))));
			}
			num_to_bool(cur, p);
			break;
		case RT_Exp_Exp_ADD_Exp:
			cur->exp_type = ET_NUM;
			Exp_Exp_Op_Exp
			cur->var_cmplx = VC_BASIC;
			if (child1(cur)->int_type.numeric && child3(cur)->int_type.numeric)
			{
				cur->int_type.numeric = TRUE;
				cur->int_type.ival = child1(cur)->int_type.ival + child3(cur)->int_type.ival;
			}
			else
			{
				cur->int_type.numeric = FALSE;
				ir_ins_tail(new_irnode_add(cur->int_type.rvop = new_var_operand(new_vid()), exp_op(child1(cur)), exp_op(child3(cur))));
			}
			num_to_bool(cur, p);
			break;
		case RT_Exp_Exp_MINUS_Exp:
			cur->exp_type = ET_NUM;
			Exp_Exp_Op_Exp
			cur->var_cmplx = VC_BASIC;
			if (child1(cur)->int_type.numeric && child3(cur)->int_type.numeric)
			{
				cur->int_type.numeric = TRUE;
				cur->int_type.ival = child1(cur)->int_type.ival - child3(cur)->int_type.ival;
			}
			else
			{
				cur->int_type.numeric = FALSE;
				ir_ins_tail(new_irnode_sub(cur->int_type.rvop = new_var_operand(new_vid()), exp_op(child1(cur)), exp_op(child3(cur))));
			}
			num_to_bool(cur, p);
			break;
		case RT_Exp_Exp_MULTASSIGN_Exp:
			cur->exp_type = ET_NUM;
			ir_parse(child1(cur), cur);
			ir_parse(child3(cur), cur);
			cur->lvalue = child1(cur)->lvalue;
			cur->vtype = child1(cur)->vtype;
			cur->var_cmplx = VC_BASIC;

			if (child1(cur)->int_type.numeric && child3(cur)->int_type.numeric)
			{
				cur->int_type.numeric = TRUE;
				cur->int_type.ival = child1(cur)->int_type.ival * child3(cur)->int_type.ival;
				do_assign(child1(cur), cur);
				cur->int_type = child1(cur)->int_type; 
			}
			else
			{
				cur->int_type.numeric = FALSE;
				ir_ins_tail(new_irnode_mulassign(cur->int_type.rvop = exp_op(child1(cur)), exp_op(child1(cur)), exp_op(child3(cur))));
			}
			num_to_bool(cur, p);
			break;
		case RT_Exp_Exp_DIVASSIGN_Exp:
			cur->exp_type = ET_NUM;
			ir_parse(child1(cur), cur);
			ir_parse(child3(cur), cur);
			cur->lvalue = child1(cur)->lvalue;
			cur->vtype = child1(cur)->vtype;
			cur->var_cmplx = VC_BASIC;

			if (child1(cur)->int_type.numeric && child3(cur)->int_type.numeric)
			{
				cur->int_type.numeric = TRUE;
				cur->int_type.ival = child1(cur)->int_type.ival / child3(cur)->int_type.ival;
				do_assign(child1(cur), cur);
				cur->int_type = child1(cur)->int_type; 
			}
			else
			{
				cur->int_type.numeric = FALSE;
				ir_ins_tail(new_irnode_divassign(cur->int_type.rvop = exp_op(child1(cur)), exp_op(child1(cur)), exp_op(child3(cur))));
			}
			num_to_bool(cur, p);
			break;
		case RT_Exp_Exp_ADDASSIGN_Exp:
			cur->exp_type = ET_NUM;
			ir_parse(child1(cur), cur);
			ir_parse(child3(cur), cur);
			cur->lvalue = child1(cur)->lvalue;
			cur->vtype = child1(cur)->vtype;
			cur->var_cmplx = VC_BASIC;

			if (child1(cur)->int_type.numeric && child3(cur)->int_type.numeric)
			{
				cur->int_type.numeric = TRUE;
				cur->int_type.ival = child1(cur)->int_type.ival + child3(cur)->int_type.ival;
				do_assign(child1(cur), cur);
				cur->int_type = child1(cur)->int_type; 
			}
			else
			{
				cur->int_type.numeric = FALSE;
				ir_ins_tail(new_irnode_addassign(cur->int_type.rvop = exp_op(child1(cur)), exp_op(child1(cur)), exp_op(child3(cur))));
			}
			num_to_bool(cur, p);
			break;
		case RT_Exp_Exp_MINUSASSIGN_Exp:
			cur->exp_type = ET_NUM;
			ir_parse(child1(cur), cur);
			ir_parse(child3(cur), cur);
			cur->lvalue = child1(cur)->lvalue;
			cur->vtype = child1(cur)->vtype;
			cur->var_cmplx = VC_BASIC;

			if (child1(cur)->int_type.numeric && child3(cur)->int_type.numeric)
			{
				cur->int_type.numeric = TRUE;
				cur->int_type.ival = child1(cur)->int_type.ival * child3(cur)->int_type.ival;
				do_assign(child1(cur), cur);
				cur->int_type = child1(cur)->int_type; 
			}
			else
			{
				cur->int_type.numeric = FALSE;
				ir_ins_tail(new_irnode_subassign(cur->int_type.rvop = exp_op(child1(cur)), exp_op(child1(cur)), exp_op(child3(cur))));
			}
			num_to_bool(cur, p);
			break;
		case RT_Exp_Exp_RELOP_Exp:
			cur->exp_type = ET_NUM;
			Exp_Exp_Op_Exp

			if (child1(cur)->int_type.numeric && child3(cur)->int_type.numeric)
			{
				int result = judge_relop(child1(cur)->int_type.ival, 
						child3(cur)->int_type.ival, child2(cur)->val.val_rel);
				if (p->exp_type == ET_NUM)
				{
					cur->int_type.numeric = TRUE;
					cur->int_type.ival = result;
					cur->var_cmplx = VC_BASIC;
				}
				else //condition
				{
					if (cur->label_true && result) //goto true
						ir_ins_tail(new_irnode_goto(cur->label_true));
					else if (cur->label_false && !result)	//goto false
						ir_ins_tail(new_irnode_goto(cur->label_false));
					cur->var_cmplx = VC_REL;
				}
				break;
			}
			
			if (p->exp_type == ET_BOOL)
			{
				cur->var_cmplx = VC_REL;
				if (cur->label_true && cur->label_false) 
				{
					cur->condition_stmt.rel_node = new_irnode_ifgoto(exp_op(child1(cur)), exp_op(child3(cur)), child2(cur)->val.val_rel, cur->label_true);
					ir_ins_tail(cur->condition_stmt.rel_node);
					ir_ins_tail(new_irnode_goto(cur->label_false));
				}
				else if (cur->label_true) 
				{
					cur->condition_stmt.rel_node = new_irnode_ifgoto(exp_op(child1(cur)), exp_op(child3(cur)), child2(cur)->val.val_rel, cur->label_true);
					ir_ins_tail(cur->condition_stmt.rel_node);
				}
				else if (cur->label_false) 
				{
					cur->condition_stmt.rel_node = new_irnode_ifgoto(exp_op(child1(cur)), exp_op(child3(cur)), reverse_relop(child2(cur)->val.val_rel), cur->label_false);
					ir_ins_tail(cur->condition_stmt.rel_node);
				}
			}
			else
			{
				cur->var_cmplx = VC_BASIC;
				cur->int_type.numeric = FALSE;
				in = new_irnode_label(new_label_id());
				in2 = new_irnode_label(new_label_id());
				ir_ins_tail(new_irnode_ifgoto(exp_op(child1(cur)), 
						exp_op(child3(cur)), child2(cur)->val.val_rel, in));
				cur->int_type.rvop = new_var_operand(new_vid());
				ir_ins_tail(new_irnode_assign(cur->int_type.rvop, iop_zero));
				ir_ins_tail(new_irnode_goto(in2));
				ir_ins_tail(in); // add label
				ir_ins_tail(new_irnode_assign(cur->int_type.rvop, iop_one));
				ir_ins_tail(in2); // add label
			}			break;
		case RT_Exp_Exp_LSHIFT_Exp:
			cur->exp_type = ET_NUM;
			Exp_Exp_Op_Exp
			cur->var_cmplx = VC_BASIC;
			if (child1(cur)->int_type.numeric && child3(cur)->int_type.numeric)
			{
				cur->int_type.numeric = TRUE;
				cur->int_type.ival = child1(cur)->int_type.ival << child3(cur)->int_type.ival;
			}
			else
			{
				cur->int_type.numeric = FALSE;
				ir_ins_tail(new_irnode_lshift(cur->int_type.rvop = new_var_operand(new_vid()), exp_op(child1(cur)), exp_op(child3(cur))));
			}
			num_to_bool(cur, p);
			break;
		case RT_Exp_Exp_RSHIFT_Exp:
			cur->exp_type = ET_NUM;
			Exp_Exp_Op_Exp
			cur->var_cmplx = VC_BASIC;
			if (child1(cur)->int_type.numeric && child3(cur)->int_type.numeric)
			{
				cur->int_type.numeric = TRUE;
				cur->int_type.ival = child1(cur)->int_type.ival >> child3(cur)->int_type.ival;
			}
			else
			{
				cur->int_type.numeric = FALSE;
				ir_ins_tail(new_irnode_rshift(cur->int_type.rvop = new_var_operand(new_vid()), exp_op(child1(cur)), exp_op(child3(cur))));
			}
			num_to_bool(cur, p);
			break;
		case RT_Exp_Exp_LSHIFTASSIGN_Exp:
			cur->exp_type = ET_NUM;
			ir_parse(child1(cur), cur);
			ir_parse(child3(cur), cur);
			cur->lvalue = child1(cur)->lvalue;
			cur->vtype = child1(cur)->vtype;
			cur->var_cmplx = VC_BASIC;

			if (child1(cur)->int_type.numeric && child3(cur)->int_type.numeric)
			{
				cur->int_type.numeric = TRUE;
				cur->int_type.ival = child1(cur)->int_type.ival << child3(cur)->int_type.ival;
				do_assign(child1(cur), cur);
				cur->int_type = child1(cur)->int_type; 
			}
			else
			{
				cur->int_type.numeric = FALSE;
				ir_ins_tail(new_irnode_lshiftassign(cur->int_type.rvop = exp_op(child1(cur)), exp_op(child1(cur)), exp_op(child3(cur))));
			}
			num_to_bool(cur, p);
			break;
		case RT_Exp_Exp_RSHIFTASSIGN_Exp:
			cur->exp_type = ET_NUM;
			ir_parse(child1(cur), cur);
			ir_parse(child3(cur), cur);
			cur->lvalue = child1(cur)->lvalue;
			cur->vtype = child1(cur)->vtype;
			cur->var_cmplx = VC_BASIC;

			if (child1(cur)->int_type.numeric && child3(cur)->int_type.numeric)
			{
				cur->int_type.numeric = TRUE;
				cur->int_type.ival = child1(cur)->int_type.ival >> child3(cur)->int_type.ival;
				do_assign(child1(cur), cur);
				cur->int_type = child1(cur)->int_type; 
			}
			else
			{
				cur->int_type.numeric = FALSE;
				ir_ins_tail(new_irnode_rshiftassign(cur->int_type.rvop = exp_op(child1(cur)), exp_op(child1(cur)), exp_op(child3(cur))));
			}
			num_to_bool(cur, p);
			break;
		case RT_Exp_Exp_LOGIAND_Exp:
			if (p->exp_type == ET_BOOL)
			{
				child1(cur)->label_true = NULL; 
				child1(cur)->label_false = cur->label_false;
				child3(cur)->label_true = cur->label_true;
				child3(cur)->label_false = cur->label_false;
				if (cur->label_false == NULL)
				{
					in = new_irnode_label(new_label_id());
					child1(cur)->label_false = in;
					child3(cur)->label_false = in;
				}
				else
					in = NULL;					
				cur->exp_type = ET_BOOL; 		
				Exp_Exp_Op_Exp
				if (in) // new label
					ir_ins_tail(in);
				cur->var_cmplx = VC_REL;
			}
			else 
			{
				cur->var_cmplx = VC_BASIC; // numeric
				in = new_irnode_label(new_label_id());
				in2 = new_irnode_label(new_label_id());
				child1(cur)->label_true = NULL;
				child1(cur)->label_false = in;
				cur->exp_type = ET_BOOL; 
				ir_parse(child1(cur), cur);
				cur->vtype = child1(cur)->vtype; 
				
				if (child1(cur)->int_type.numeric) 
				{
					cur->int_type.numeric = TRUE; 
					if (child1(cur)->int_type.ival)
						cur->int_type.ival = 0;
					else
						cur->int_type.ival = 1; 

				}
				if (!(child1(cur)->int_type.numeric && cur->int_type.ival == 0)) 
				{
					child3(cur)->label_true = NULL;
					child3(cur)->label_false = in;

					ir_parse(child3(cur), cur);

					if (child3(cur)->int_type.numeric) //constant
					{
						cur->int_type.numeric = TRUE; 
						cur->int_type.ival = (child3(cur)->int_type.ival != 0);
					}
					if (!child1(cur)->int_type.numeric || !child3(cur)->int_type.numeric) // not numeric
					{
						cur->int_type.numeric = FALSE; //if exps goto FALSE
						ir_ins_tail(new_irnode_assign(cur->int_type.rvop = new_var_operand(new_vid()), iop_one)); // t1 = 1
						ir_ins_tail(new_irnode_goto(in2)); // goto END
						ir_ins_tail(new_irnode_label(in->label.label_id));// label false
						ir_ins_tail(new_irnode_assign(cur->int_type.rvop, iop_zero)); //t1 = 0
						ir_ins_tail(new_irnode_label(in2->label.label_id)); // label end
					}
				}
			}
			break;
		case RT_Exp_Exp_LOGIOR_Exp:
			if (p->exp_type == ET_BOOL)
			{
				child1(cur)->label_true = cur->label_true; 
				child1(cur)->label_false = NULL;
				child3(cur)->label_true = cur->label_true;
				child3(cur)->label_false = cur->label_false;
				if (cur->label_false == NULL)
				{
					in = new_irnode_label(new_label_id());
					child1(cur)->label_false = in;
					child3(cur)->label_false = in;
				}
				else
					in = NULL;					
				cur->exp_type = ET_BOOL; 		
				Exp_Exp_Op_Exp
				if (in) // new label
					ir_ins_tail(in);
				cur->var_cmplx = VC_REL;
			}
			else 
			{
				cur->var_cmplx = VC_BASIC; // numeric
				in = new_irnode_label(new_label_id());
				in2 = new_irnode_label(new_label_id());
				child1(cur)->label_true = in;
				child1(cur)->label_false = NULL;
				cur->exp_type = ET_BOOL; 
				ir_parse(child1(cur), cur);
				cur->vtype = child1(cur)->vtype; 
				
				if (child1(cur)->int_type.numeric) 
				{
					cur->int_type.numeric = TRUE; 
					if (child1(cur)->int_type.ival)
						cur->int_type.ival = 1;
					else
						cur->int_type.ival = 0; 

				}
				if (!(child1(cur)->int_type.numeric && cur->int_type.ival == 1)) 
				{
					child3(cur)->label_true = in;
					child3(cur)->label_false = NULL;

					ir_parse(child3(cur), cur);

					if (child3(cur)->int_type.numeric) //constant
					{
						cur->int_type.numeric = TRUE; 
						cur->int_type.ival = (child3(cur)->int_type.ival != 0);
					}
					if (!child1(cur)->int_type.numeric || !child3(cur)->int_type.numeric) // not numeric
					{
						cur->int_type.numeric = TRUE; //if exps goto TRUE
						ir_ins_tail(new_irnode_assign(cur->int_type.rvop = new_var_operand(new_vid()), iop_zero)); // t1 = 0
						ir_ins_tail(new_irnode_goto(in2)); // goto END
						ir_ins_tail(new_irnode_label(in->label.label_id));// label true
						ir_ins_tail(new_irnode_assign(cur->int_type.rvop, iop_one)); //t1 = 1
						ir_ins_tail(new_irnode_label(in2->label.label_id)); // label end
					}
				}
			}

			break;
		case RT_Exp_Exp_BITAND_Exp:
			cur->exp_type = ET_NUM;
			Exp_Exp_Op_Exp
			cur->var_cmplx = VC_BASIC;
			if (child1(cur)->int_type.numeric && child3(cur)->int_type.numeric)
			{
				cur->int_type.numeric = TRUE;
				cur->int_type.ival = child1(cur)->int_type.ival & child3(cur)->int_type.ival;
			}
			else
			{
				cur->int_type.numeric = FALSE;
				ir_ins_tail(new_irnode_bitand(cur->int_type.rvop = new_var_operand(new_vid()), exp_op(child1(cur)), exp_op(child3(cur))));
			}
			num_to_bool(cur, p);
			break;
		case RT_Exp_Exp_BITOR_Exp:
			cur->exp_type = ET_NUM;
			Exp_Exp_Op_Exp
			cur->var_cmplx = VC_BASIC;
			if (child1(cur)->int_type.numeric && child3(cur)->int_type.numeric)
			{
				cur->int_type.numeric = TRUE;
				cur->int_type.ival = child1(cur)->int_type.ival | child3(cur)->int_type.ival;
			}
			else
			{
				cur->int_type.numeric = FALSE;
				ir_ins_tail(new_irnode_bitor(cur->int_type.rvop = new_var_operand(new_vid()), exp_op(child1(cur)), exp_op(child3(cur))));
			}
			num_to_bool(cur, p);
			break;
		case RT_Exp_Exp_BITXOR_Exp:
			cur->exp_type = ET_NUM;
			Exp_Exp_Op_Exp
			cur->var_cmplx = VC_BASIC;
			if (child1(cur)->int_type.numeric && child3(cur)->int_type.numeric)
			{
				cur->int_type.numeric = TRUE;
				cur->int_type.ival = child1(cur)->int_type.ival ^ child3(cur)->int_type.ival;
			}
			else
			{
				cur->int_type.numeric = FALSE;
				ir_ins_tail(new_irnode_bitxor(cur->int_type.rvop = new_var_operand(new_vid()), exp_op(child1(cur)), exp_op(child3(cur))));
			}
			num_to_bool(cur, p);
			break;
		case RT_Exp_Exp_BITANDASSIGN_Exp:
			cur->exp_type = ET_NUM;
			ir_parse(child1(cur), cur);
			ir_parse(child3(cur), cur);
			cur->lvalue = child1(cur)->lvalue;
			cur->vtype = child1(cur)->vtype;
			cur->var_cmplx = VC_BASIC;

			if (child1(cur)->int_type.numeric && child3(cur)->int_type.numeric)
			{
				cur->int_type.numeric = TRUE;
				cur->int_type.ival = child1(cur)->int_type.ival & child3(cur)->int_type.ival;
				do_assign(child1(cur), cur);
				cur->int_type = child1(cur)->int_type; 
			}
			else
			{
				cur->int_type.numeric = FALSE;
				ir_ins_tail(new_irnode_bitandassign(cur->int_type.rvop =  exp_op(child1(cur)), exp_op(child1(cur)), exp_op(child3(cur))));
			}
			num_to_bool(cur, p);
			break;
		case RT_Exp_Exp_BITORASSIGN_Exp:
			cur->exp_type = ET_NUM;
			ir_parse(child1(cur), cur);
			ir_parse(child3(cur), cur);
			cur->lvalue = child1(cur)->lvalue;
			cur->vtype = child1(cur)->vtype;
			cur->var_cmplx = VC_BASIC;

			if (child1(cur)->int_type.numeric && child3(cur)->int_type.numeric)
			{
				cur->int_type.numeric = TRUE;
				cur->int_type.ival = child1(cur)->int_type.ival | child3(cur)->int_type.ival;
				do_assign(child1(cur), cur);
				cur->int_type = child1(cur)->int_type; 
			}
			else
			{
				cur->int_type.numeric = FALSE;
				ir_ins_tail(new_irnode_bitorassign(cur->int_type.rvop = exp_op(child1(cur)), exp_op(child1(cur)), exp_op(child3(cur))));
			}
			num_to_bool(cur, p);
			break;
		case RT_Exp_Exp_BITXORASSIGN_Exp:
			cur->exp_type = ET_NUM;
			ir_parse(child1(cur), cur);
			ir_parse(child3(cur), cur);
			cur->lvalue = child1(cur)->lvalue;
			cur->vtype = child1(cur)->vtype;
			cur->var_cmplx = VC_BASIC;

			if (child1(cur)->int_type.numeric && child3(cur)->int_type.numeric)
			{
				cur->int_type.numeric = TRUE;
				cur->int_type.ival = child1(cur)->int_type.ival ^ child3(cur)->int_type.ival;
				do_assign(child1(cur), cur);
				cur->int_type = child1(cur)->int_type; 
			}
			else
			{
				cur->int_type.numeric = FALSE;
				ir_ins_tail(new_irnode_bitxorassign(cur->int_type.rvop = exp_op(child1(cur)), exp_op(child1(cur)), exp_op(child3(cur))));
			}
			num_to_bool(cur, p);
			break;
		case RT_Exp_MINUS_Exp:
			cur->exp_type = ET_NUM;
			cur->lvalue = LV_RVALUE;
			cur->vtype = child2(cur)->vtype;
			ir_parse(child2(cur), cur);
			if (child2(cur)->int_type.numeric)
			{
				cur->int_type.numeric = TRUE;
				cur->int_type.ival = -child2(cur)->int_type.ival;
			}
			else
			{
				cur->int_type.numeric = FALSE;
				ir_ins_tail(new_irnode_sub(cur->int_type.rvop = new_var_operand(new_vid()), iop_zero, exp_op(child2(cur))));
			}
			cur->var_cmplx = VC_BASIC;
			num_to_bool(cur, p);
			break;
		case RT_Exp_LOGINOT_Exp:
			cur->exp_type = ET_BOOL;
			if (p->exp_type == ET_BOOL)
			{
				child2(cur)->label_true = cur->label_false;
				child2(cur)->label_false = cur->label_true;
				ir_parse(child2(cur), cur);
				cur->lvalue = LV_RVALUE;
				cur->vtype = child2(cur)->vtype;
				cur->var_cmplx = VC_REL;
			}
			else
			{
				if (child2(cur)->int_type.numeric) 
				{
					cur->var_cmplx = VC_BASIC;
					cur->int_type.numeric = TRUE;
					cur->int_type.ival = child2(cur)->int_type.ival;
				}
				else
				{
					in = new_irnode_label(new_label_id());
					in2 = new_irnode_label(new_label_id());
					child2(cur)->label_true = NULL;
					child2(cur)->label_false = cur->label_true;
					
					ir_parse(child2(cur), cur);

					cur->int_type.numeric = FALSE;// if xx goto false
					ir_ins_tail(new_irnode_assign(cur->int_type.rvop = new_var_operand(new_vid()), iop_zero)); // t1 = 0
					ir_ins_tail(new_irnode_goto(in2)); // goto END/
					ir_ins_tail(new_irnode_label(in->label.label_id));// label true
					ir_ins_tail(new_irnode_assign(cur->int_type.rvop, iop_one));//t1 = 1
					ir_ins_tail(new_irnode_label(in2->label.label_id)); // label end
					cur->vtype = child2(cur)->vtype;
					cur->var_cmplx = VC_REL;
				}
			}
			break;
		case RT_Exp_BITNOT_Exp:
			cur->exp_type = ET_NUM;
			cur->lvalue = LV_RVALUE;
			cur->vtype = child2(cur)->vtype;
			ir_parse(child2(cur), cur);
			if (child2(cur)->int_type.numeric)
			{
				cur->int_type.numeric = TRUE;
				cur->int_type.ival = ~child2(cur)->int_type.ival;
			}
			else
			{
				cur->int_type.numeric = FALSE;
				ir_ins_tail(new_irnode_bitnot(cur->int_type.rvop = new_var_operand(new_vid()), exp_op(child2(cur))));
			}
			cur->var_cmplx = VC_BASIC;
			num_to_bool(cur, p);			
			break;
			
		case RT_Exp_INCR_Exp:
			cur->exp_type = ET_NUM;
			cur->lvalue = LV_RVALUE;
			cur->vtype = child2(cur)->vtype;
			ir_parse(child2(cur), cur);
			if (child2(cur)->int_type.numeric)
			{
				cur->int_type.numeric = TRUE;
				cur->int_type.ival = child2(cur)->int_type.ival + 1;
			}
			else
			{
				cur->int_type.numeric = FALSE;
				ir_ins_tail(new_irnode_incr(cur->int_type.rvop = exp_op(child2(cur)), exp_op(child2(cur))));
			}
			cur->var_cmplx = VC_BASIC;
			num_to_bool(cur, p);	
			break;
		case RT_Exp_DECR_Exp:
			cur->exp_type = ET_NUM;
			cur->lvalue = LV_RVALUE;
			cur->vtype = child2(cur)->vtype;
			ir_parse(child2(cur), cur);
			if (child2(cur)->int_type.numeric)
			{
				cur->int_type.numeric = TRUE;
				cur->int_type.ival = child2(cur)->int_type.ival + 1;
			}
			else
			{
				cur->int_type.numeric = FALSE;
				ir_ins_tail(new_irnode_decr(cur->int_type.rvop = exp_op(child2(cur)), exp_op(child2(cur))));
			}
			cur->var_cmplx = VC_BASIC;
			num_to_bool(cur, p);	
			break;
			
		case RT_Exp_LP_Exp_RP:
			child2(cur)->label_true = cur->label_true;
			child2(cur)->label_false = cur->label_false;
			ir_parse(child2(cur), cur);
			cur->int_type = child2(cur)->int_type;
			cur->lvalue = child2(cur)->lvalue;
			cur->vtype = child2(cur)->vtype;
			cur->var_cmplx = child2(cur)->var_cmplx;
			break;
		case RT_Exp_ID_LP_Args_RP:
			cur->exp_type = ET_NUM;
			cur->name = child1(cur)->val.val_str;
			if (strcmp(cur->name, "write") == 0) // write
			{
				ir_parse(child3(cur), cur);
				break;
			}
			if (strcmp(cur->name, "read") == 0) // read!
			{
				ir_parse(child3(cur), cur);
				break;
			}
			n = find_SymbolNode(cur->name, SK_VAR);
			//function
			cur->field = n->type->function->param;
			ir_parse(child3(cur), cur);
			//args valid
			cur->lvalue = LV_RVALUE;
			cur->vtype = n->type->function->return_type;
			//add call
			ir_ins_tail(new_irnode_call(cur->int_type.rvop = new_var_operand(new_vid()), cur->name));
			cur->var_cmplx = VC_BASIC;
			num_to_bool(cur, p);
			break;
		case RT_Exp_ID_LP_RP:
			cur->exp_type = ET_NUM;
			cur->name = child1(cur)->val.val_str;
			n = find_SymbolNode(cur->name, SK_VAR);
			ir_ins_tail(new_irnode_call(cur->int_type.rvop = new_var_operand(new_vid()), 
						cur->name));
			cur->int_type.numeric = FALSE; 
			cur->lvalue = LV_RVALUE;
			cur->vtype = n->type->function->return_type;
			cur->var_cmplx = VC_BASIC;
			num_to_bool(cur, p);
			break;
		case RT_Exp_Exp_LB_Exp_RB:
			cur->exp_type = ET_NUM;
			ir_parse(child1(cur), cur);
			cur->ir_prev = child1(cur)->ir_prev;
			ir_parse(child3(cur), cur);
			cur->vtype = child1(cur)->vtype;
			if (!child3(cur)->int_type.numeric) 
			{
				//child1(cur)->complex_type.size = child3(cur)->int_type.ival;
				// t1 := x * width
				ir_ins_tail(new_irnode_mul(top = new_var_operand(new_vid()), child3(cur)->int_type.rvop, new_int_const_operand(cur->vtype->array.type->width)));	
			}
			else
			{
				child1(cur)->complex_type.size = child3(cur)->int_type.ival;	//save array size
				top = new_int_const_operand(child3(cur)->int_type.ival * cur->vtype->array.type->width);
			}
			// t2 := &a + t1
			ir_ins_tail(new_irnode_add(rop = new_var_operand(new_vid()),child1(cur)->complex_type.base_addr, top));
			cur->complex_type.base_addr = rop; 				
			cur->vtype = child1(cur)->vtype->array.type; 
			if (cur->vtype->kind == TK_ARRAY)
				cur->lvalue = LV_RVALUE;
			else
				cur->lvalue = LV_LVALUE;

			cur->var_cmplx = child1(cur)->var_cmplx;
			num_to_bool(cur, p);
			break;
		case RT_Exp_Exp_DOT_ID:
			cur->exp_type = ET_NUM;
			ir_parse(child1(cur), cur);
			cur->ir_prev = child1(cur)->ir_prev;
			cur->vtype = child1(cur)->vtype;
			f = find_field(cur->vtype->field_list, child3(cur)->val.val_str);
			ir_ins_tail(new_irnode_add(rop = new_var_operand(new_vid()), child1(cur)->complex_type.base_addr, new_int_const_operand(f->offset)));
			cur->complex_type.base_addr = rop;
			cur->var_cmplx = child1(cur)->var_cmplx;
			cur->vtype = f->type;
			cur->lvalue = (cur->vtype->kind == TK_ARRAY) ? LV_RVALUE : LV_LVALUE;
			num_to_bool(cur, p);
			break;
		case RT_Exp_ID:
			cur->exp_type = ET_NUM;
			cur->name = child1(cur)->val.val_str;
			cur->lvalue = LV_LVALUE; 
			n = find_SymbolNode(cur->name, SK_VAR); 
			if (n->type->kind > LAST_BASIC_TYPE)
			{
				if (n->cur_variable.vtype == VT_VARIABLE)
					cur->complex_type.base_addr = new_addr_operand(n->cur_variable.vid);
				else
					cur->complex_type.base_addr = new_var_operand(n->cur_variable.vid);
				cur->var_cmplx = VC_ADDR; 	
			}
			else
			{
				cur->int_type.numeric = FALSE;
				cur->int_type.rvop = new_var_operand(n->cur_variable.vid);
				cur->var_cmplx = VC_BASIC; 
				cur->exp_type = ET_NUM;
				convert_type(p, cur);
			}
			cur->vtype = n->type;
			num_to_bool(cur, p);
			break;
		case RT_Exp_INT:
			cur->exp_type = ET_NUM;			
			cur->int_type.numeric = TRUE; 
			cur->var_cmplx = VC_BASIC; 
			cur->int_type.ival = child1(cur)->val.val_int;
			cur->vtype = TypeNode_int;
			cur->lvalue = LV_RVALUE;
			cur->exp_type = ET_NUM;
			num_to_bool(cur, p);
			break;
		case RT_Exp_NULL:
			break;
		case RT_Args_Exp_COMMA_Args:
			cur->exp_type = ET_NUM;
			ir_parse(child1(cur), cur);
			cur->field = p->field;
			cur->arraydec = p->arraydec;
			if(cur->arraydec != 1)
			{
				f = cur->field;
				cur->field = cur->field->next;
				ir_parse(child3(cur), cur);
				cur->field = f;
				if (child1(cur)->vtype->kind == TK_INT)
				{
					ir_ins_tail(new_irnode_store(exp_op(child1(cur))));
				}
				else
				{
					ir_ins_tail(new_irnode_store(child1(cur)->complex_type.base_addr));
				}				
				cur->field = p->field; // restore
			}
			else 
			{
				arraylist[arraycnt++] = child1(cur)->int_type.ival;	//save the array element
				//printf("%d %d\n", arraycnt, arraylist[arraycnt-1]);
				ir_parse(child3(cur), cur);
			}
			break;
		case RT_Args_Exp:
			cur->exp_type = ET_NUM;
			cur->arraydec = p->arraydec;		
			ir_parse(child1(cur), cur);
			if (child1(cur)->vtype->kind == TK_INT)
			{
				arraylist[arraycnt++] = child1(cur)->int_type.ival;	//save the array element
				//printf("%d %d\n", arraycnt, arraylist[arraycnt-1]);
			}

			if(cur->arraydec != 1)
			{
				rop = ((child1(cur)->vtype->kind == TK_INT) ? exp_op(child1(cur)) : child1(cur)->complex_type.base_addr);
				if (p->name && strcmp(p->name, "write") == 0)
					ir_ins_tail(new_irnode_write(rop));
				else if (p->name && strcmp(p->name, "read") == 0)
					ir_ins_tail(new_irnode_read(rop));
				else
					ir_ins_tail(new_irnode_store(rop));
			}
			cur->field = p->field;
			break;
	}
	layer--;
}

int intermediatecode_parse(struct TreeNode* treeroot)
{
	TypeNode_int = new_TypeNode_basic(TK_INT);
	iop_zero = new_int_const_operand(0); // #0
	iop_one = new_int_const_operand(1);  // #1
	treeroot->ir_prev = ir_header(); // the header
	//printf("\n");
	ir_parse(treeroot, NULL);
}

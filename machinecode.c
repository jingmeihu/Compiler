#include "def.h"

#define LABEL_PREFIX "FUNCTION"
//register name
#define REG_STR(x) #x
//instruction
#define INSTRUCTION_LW 	"\tlw %s, %d(%s)\n" // s1 = MEM[s2 + #d1]
#define INSTRUCTION_SW 	"\tsw %s, %d(%s)\n" // MEM[s2 + #d1] = s1
#define INSTRUCTION_LI		"\tli %s, %d\n" //s1 = #d1 
#define INSTRUCTION_ADD	"\tadd %s, %s, %s\n"// s1 = s2 + s3 
#define INSTRUCTION_MUL	"\tmul %s, %s, %s\n" //s1 = s2 * s3 
#define INSTRUCTION_SUB	"\tsub %s, %s, %s\n"// s1 = s2 - s3 
#define INSTRUCTION_DIV	"\tdiv %s, %s, %s\n" // s1 = s2 / s3
#define INSTRUCTION_MOD	"\trem %s, %s, %s\n"//s1 = s2 % s3 
#define INSTRUCTION_AND	"\tand %s, %s, %s\n"// s1 = s2 & s3 
#define INSTRUCTION_OR 	"\tor %s, %s, %s\n"// s1 = s2 | s3 
#define INSTRUCTION_XOR 	"\txor %s, %s, %s\n"// s1 = s2 ^ s3 
#define INSTRUCTION_NOT 	"\tnor %s, %s, %s\n"// s1 = ~(s2 | s3)
#define INSTRUCTION_SLL 	"\tsll %s, %s, %d\n"// s1 = s2 << #d1
#define INSTRUCTION_SRL 	"\tsrl %s, %s, %d\n"// s1 = s2 >> #d1
#define INSTRUCTION_JAL	"\tjal %s\n"// call s1 
#define INSTRUCTION_JR		"\tjr %s\n"// return $ra 
#define INSTRUCTION_J		"\tj "LABEL_PREFIX"%d\n"// jump s1&d1 
#define INSTRUCTION_FLBL	"%s:\n"//label s1&d1 
#define INSTRUCTION_MOV	"\tmove %s, %s\n"// s1 = s2 
#define INSTRUCTION_RET	"\tjr %s\n"// jr s1 
#define INSTRUCTION_BEQ	"\tbeq %s, %s, "LABEL_PREFIX"%d\n"// if s1 == s2 goto s3 
#define INSTRUCTION_BNE	"\tbne %s, %s, "LABEL_PREFIX"%d\n"// if s1 != s2 goto s3 
#define INSTRUCTION_BGT	"\tbgt %s, %s, "LABEL_PREFIX"%d\n"// if s1 > s2 goto s3 
#define INSTRUCTION_BLT	"\tblt %s, %s, "LABEL_PREFIX"%d\n"// if s1 < s2 goto s3 
#define INSTRUCTION_BGE	"\tbge %s, %s, "LABEL_PREFIX"%d\n"// if s1 >= s2 goto s3 
#define INSTRUCTION_BLE	"\tble %s, %s, "LABEL_PREFIX"%d\n"// if s1 <= s2 goto s3 
#define INSTRUCTION_LBL	LABEL_PREFIX"%d:\n"

static struct irOperand* iop_zero; // #0
static struct irOperand* iop_int;  // #int

//machine code header
#define ASSEMBLY_HEADER \
".data\n" \
"_prompt: .asciiz \"Enter an integer: \"\n" \
"_ret: .asciiz \"\\n\" \n" \
".globl main \n" \
".text \n" \
"read: \n" \
"\tli $v0, 4 \n"\
"\tla $a0, _prompt \n"\
"\tsyscall \n"\
"\tli $v0, 5 \n"\
"\tsyscall \n"\
"\tjr $ra \n"\
"write: \n"\
"\tli $v0, 1 \n"\
"\tsyscall \n"\
"\tli $v0, 4 \n"\
"\tla $a0, _ret \n"\
"\tsyscall\n"\
"\tmove $v0, $0\n"\
"\tjr $ra\n"

// the header of variable link in a function
static struct MCVar* listhdr;
static struct MCVar* globallisthdr;
int stackpointer;
int global_var_size = 0; 
int offset = 0;
int offset_tmp=-4;
int var_size = 0; // local variables' size 
int main_var_size = 0; // main variables' size 
int param_size = 0; // parameters' size
// create new MCVar
struct MCVar* mc_new_var(int vid, int offset, int len)
{
	struct MCVar *n;
	if (mc_find_var(vid) || mc_find_global_var(vid))
		return NULL;
	
	n = (struct MCVar*)malloc(sizeof(struct MCVar));
	n->vid = vid;
	n->offset = offset;
	n->len = len;
	n->next = NULL; //the last one
	if (listhdr->prev)
	{
		n->prev = listhdr->prev;
		n->prev->next = n;
	}
	else
		n->prev = listhdr;
	listhdr->prev = n;
	if (!listhdr->next) //empty link
		listhdr->next = n;

	return n;
}

// create new MCVar
struct MCVar* mc_new_global_var(int vid, int offset, int len)
{
	struct MCVar *n;
	if (mc_find_global_var(vid))
		return NULL;
	n = (struct MCVar*)malloc(sizeof(struct MCVar));
	n->vid = vid;
	n->offset = offset;
	n->len = len;
	n->next = NULL; //the last one
	if (globallisthdr->prev)
	{
		n->prev = globallisthdr->prev;
		n->prev->next = n;
	}
	else
		n->prev = globallisthdr;
	globallisthdr->prev = n;
	if (!globallisthdr->next) //empty link
		globallisthdr->next = n;
	return n;
}

// create new function space
void mc_new_func()
{
	struct MCVar *p = listhdr->next;
	while (p)
	{
		struct MCVar* q = p->next;
		free(p);
		p = q;
	}
	listhdr->next = listhdr->prev = NULL; 
/*
	struct MCVar *tmp = globallisthdr->next;
	while(tmp)
	{
		mc_new_var(tmp->vid, tmp->offset, tmp->len);
		tmp=tmp->next;
	}
*/
}

//find MCVar
struct MCVar * mc_find_var(int vid)
{
	struct MCVar *n = listhdr->next;
	//printf("find var\n");
	while (n)
	{
		//printf("%d\n", n->vid);
		if (n->vid == vid)
		{	
			//printf("vid = %d\n", vid);
			return n; //find!
		}
		n = n->next;
	}
	return NULL; //not found
}
//return variable offset
int mc_var_global_offset(int vid)
{
	return mc_find_global_var(vid)->offset ;
}


//find MCVar
struct MCVar * mc_find_global_var(int vid)
{
	struct MCVar *n = globallisthdr->next;
	//printf("find var\n");
	while (n)
	{
		//printf("%d\n", n->vid);
		if (n->vid == vid)
		{	
			//printf("vid = %d\n", vid);
			return n; //find!
		}
		n = n->next;
	}
	return NULL; //not found
}
//return variable offset
int mc_var_offset(int vid)
{
	return mc_find_var(vid)->offset ;
}

// find the next function
struct irNode* mc_find_next_func(struct irNode* prev)
{
	/* well, at least one step! */
	while (prev = prev->next)
	{
		if (prev->ntype == IT_FUNC_LABEL)
			return prev;
	}
	return prev;
}
//print the register name
static char* reg_name(int reg_no)
{
	if (reg_no == REG_t0)
		return "$t0";
	else if (reg_no == REG_t1)
		return "$t1";
	else if (reg_no == REG_t2)
		return "$t2";
	else if (reg_no == REG_t3)
		return "$t3";
	else if (reg_no == REG_t4)
		return "$t4";
	else if (reg_no == REG_t5)
		return "$t5";
	else if (reg_no == REG_t6)
		return "$t6";
	else if (reg_no == REG_t7)
		return "$t7";

	else if (reg_no == REG_s0)
		return "$s0";
	else if (reg_no == REG_s1)
		return "$s1";
	else if (reg_no == REG_s2)
		return "$s2";
	else if (reg_no == REG_s3)
		return "$s3";
	else if (reg_no == REG_s4)
		return "$s4";
	else if (reg_no == REG_s5)
		return "$s5";
	else if (reg_no == REG_s6)
		return "$s6";
	else if (reg_no == REG_s7)
		return "$s7";

	else if (reg_no == REG_0)
		return "$0";
	else if (reg_no == REG_sp)
		return "$sp";
	else if (reg_no == REG_ra)
		return "$ra";
	else if (reg_no == REG_v0)
		return "$v0";
	else if (reg_no == REG_v1)
		return "$v1";
	else if (reg_no == REG_a0)
		return "$a0";
	else if (reg_no == REG_a1)
		return "$a1";
	else if (reg_no == REG_a2)
		return "$a2";
	else if (reg_no == REG_a3)
		return "$a3";
}
// load an operand into register 
//reg: t3, t4 and t5
void mc_load_operand(struct irOperand* op, int reg, FILE * fout)
{
	if (op->operand_type == OT_VAR)
		//$reg = MEM[offset+$sp]
	{
		//printf("111\n");
		if(mc_find_var(op->vid)!=NULL)
			mc_generate_lw(reg, mc_find_var(op->vid)->offset, REG_sp, fout);
		else
		{
			//printf("111\n");
			mc_generate_mov(REG_t6, REG_sp, fout);
			mc_generate_mov(REG_sp, REG_t7, fout);

			mc_generate_lw(reg, mc_find_global_var(op->vid)->offset, REG_sp, fout);

			mc_generate_mov(REG_sp, REG_t6, fout);
		}
	}
	else if (op->operand_type == OT_CONST_INT)
		//$reg = #int
		mc_generate_li(reg, op->ival, fout);
	else if (op->operand_type == OT_ADDR)
	{
		//printf("addr %d \n", op->vid);
		if(mc_find_var(op->vid)!=NULL)
		{
			// $reg = $t4(offset) + $sp) 
			mc_generate_li(REG_t4, mc_find_var(op->vid)->offset, fout);
			//$reg = $t4 + $sp
			mc_generate_add(reg, REG_t4, REG_sp, fout);
		}
		else
		{
			//printf("111\n");
			mc_generate_mov(REG_t6, REG_sp, fout);
			mc_generate_mov(REG_sp, REG_t7, fout);

			mc_generate_li(REG_t4, mc_find_global_var(op->vid)->offset, fout);
			mc_generate_add(reg, REG_t4, REG_sp, fout);

			mc_generate_mov(REG_sp, REG_t6, fout);
		}
	}
	else if (op->operand_type == OT_STAR)
	{
		if(mc_find_var(op->vid)!=NULL)
		{
			// $t4 = MEM[offset+$sp]
			mc_generate_lw(REG_t4, mc_find_var(op->vid)->offset, REG_sp, fout);
			// $reg = MEM[0 + $t4]
			mc_generate_lw(reg, 0, REG_t4, fout);
		}
		else
		{
			//printf("111\n");
			mc_generate_mov(REG_t6, REG_sp, fout);
			mc_generate_mov(REG_sp, REG_t7, fout);

			mc_generate_lw(REG_t4, mc_find_global_var(op->vid)->offset, REG_sp, fout);
			mc_generate_lw(reg, 0, REG_t4, fout);

			mc_generate_mov(REG_sp, REG_t6, fout);
		}
	}
}

// save register into variable space
void mc_save_operand(struct irOperand* op, int reg, FILE * fout)
{
	if (op->operand_type == OT_VAR)
	{
		//MEM[offset+$sp] = $reg
		if(mc_find_var(op->vid)!=NULL)
			mc_generate_sw(reg, mc_find_var(op->vid)->offset, REG_sp, fout);
		else
		{
			//printf("111\n");
			mc_generate_mov(REG_t6, REG_sp, fout);
			mc_generate_mov(REG_sp, REG_t7, fout);

			mc_generate_sw(reg, mc_find_global_var(op->vid)->offset, REG_sp, fout);

			mc_generate_mov(REG_sp, REG_t6, fout);
		}
	}		
		
	else if (op->operand_type == OT_STAR)
	{
		if(mc_find_var(op->vid)!=NULL)
		{
			// $t4 = MEM[offset+$sp]
			mc_generate_lw(REG_t4, mc_find_var(op->vid)->offset, REG_sp, fout);
			// MEM[0 + $t4] = $reg
			mc_generate_sw(reg, 0, REG_t4, fout);
		}
		else
		{
			//printf("111\n");
			mc_generate_mov(REG_t6, REG_sp, fout);
			mc_generate_mov(REG_sp, REG_t7, fout);

			mc_generate_lw(REG_t4, mc_find_global_var(op->vid)->offset, REG_sp, fout);
			mc_generate_sw(reg, 0, REG_t4, fout);

			mc_generate_mov(REG_sp, REG_t6, fout);
		}
	}
}
// push into stack
void mc_push(int reg, FILE * fout)
{
	// $t4 = 4
	mc_generate_li(REG_t4, 4, fout);
	// $sp = $sp - $t4
	mc_generate_sub(REG_sp, REG_sp, REG_t4, fout);
	// MEM[sp + 0] = $reg
	mc_generate_sw(reg, 0, REG_sp, fout);
}

// pop from stack
void mc_pop(int reg, FILE * fout)
{
	// $reg = MEM[sp + 0]
	mc_generate_lw(reg, 0, REG_sp, fout);
	//$t4 = 4
	mc_generate_li(REG_t4, 4, fout);
	// $sp = $sp + $t4
	mc_generate_add(REG_sp, REG_sp, REG_t4, fout);
}

// IR code -> machine code
//lw
static void mc_generate_lw(int reg_dst, int offset, int reg_base, FILE * fout)
{
	fprintf(fout, INSTRUCTION_LW, reg_name(reg_dst), offset, reg_name(reg_base));
	printf(INSTRUCTION_LW, reg_name(reg_dst), offset, reg_name(reg_base));
}
//sw
static void mc_generate_sw(int reg_src, int offset, int reg_base, FILE * fout)
{
	fprintf(fout, INSTRUCTION_SW, reg_name(reg_src), offset, reg_name(reg_base));
	printf(INSTRUCTION_SW, reg_name(reg_src), offset, reg_name(reg_base));

}
//li
static void mc_generate_li(int reg, int constant, FILE * fout)
{
	fprintf(fout, INSTRUCTION_LI, reg_name(reg), constant);
	printf(INSTRUCTION_LI, reg_name(reg), constant);
}
// add
static void mc_generate_add(int reg_dst, int reg_src1, int reg_src2, FILE * fout)
{
	fprintf(fout, INSTRUCTION_ADD, reg_name(reg_dst), reg_name(reg_src1), reg_name(reg_src2));
	printf(INSTRUCTION_ADD, reg_name(reg_dst), reg_name(reg_src1), reg_name(reg_src2));
}
// sub
static void mc_generate_sub(int reg_dst, int reg_src1, int reg_src2, FILE * fout)
{
	fprintf(fout, INSTRUCTION_SUB, reg_name(reg_dst), reg_name(reg_src1), reg_name(reg_src2));
	printf(INSTRUCTION_SUB, reg_name(reg_dst), reg_name(reg_src1), reg_name(reg_src2));
}
// mul
static void mc_generate_mul(int reg_dst, int reg_src1, int reg_src2, FILE * fout)
{
	fprintf(fout, INSTRUCTION_MUL, reg_name(reg_dst), reg_name(reg_src1), reg_name(reg_src2));
	printf(INSTRUCTION_MUL, reg_name(reg_dst), reg_name(reg_src1), reg_name(reg_src2));
}
// div
static void mc_generate_div(int reg_dst, int reg_src1, int reg_src2, FILE * fout)
{
	fprintf(fout, INSTRUCTION_DIV, reg_name(reg_dst), reg_name(reg_src1), reg_name(reg_src2));
	printf(INSTRUCTION_DIV, reg_name(reg_dst), reg_name(reg_src1), reg_name(reg_src2));
}
// mod
static void mc_generate_mod(int reg_dst, int reg_src1, int reg_src2, FILE * fout)
{
	fprintf(fout, INSTRUCTION_MOD, reg_name(reg_dst), reg_name(reg_src1), reg_name(reg_src2));
	printf(INSTRUCTION_MOD, reg_name(reg_dst), reg_name(reg_src1), reg_name(reg_src2));
}
// and
static void mc_generate_and(int reg_dst, int reg_src1, int reg_src2, FILE * fout)
{
	fprintf(fout, INSTRUCTION_AND, reg_name(reg_dst), reg_name(reg_src1), reg_name(reg_src2));
	printf(INSTRUCTION_AND, reg_name(reg_dst), reg_name(reg_src1), reg_name(reg_src2));
}
// or
static void mc_generate_or(int reg_dst, int reg_src1, int reg_src2, FILE * fout)
{
	fprintf(fout, INSTRUCTION_OR, reg_name(reg_dst), reg_name(reg_src1), reg_name(reg_src2));
	printf(INSTRUCTION_OR, reg_name(reg_dst), reg_name(reg_src1), reg_name(reg_src2));
}
// xor
static void mc_generate_xor(int reg_dst, int reg_src1, int reg_src2, FILE * fout)
{
	fprintf(fout, INSTRUCTION_XOR, reg_name(reg_dst), reg_name(reg_src1), reg_name(reg_src2));
	printf(INSTRUCTION_XOR, reg_name(reg_dst), reg_name(reg_src1), reg_name(reg_src2));
}
// not
static void mc_generate_not(int reg_dst, int reg_src, FILE * fout)
{
	fprintf(fout, INSTRUCTION_NOT, reg_name(reg_dst), reg_name(reg_src), reg_name(0));
	printf(INSTRUCTION_NOT, reg_name(reg_dst), reg_name(reg_src), reg_name(0));
}
//sll
static void mc_generate_sll(int reg_dst, int reg_src, int constant, FILE * fout)
{
	fprintf(fout, INSTRUCTION_SLL, reg_name(reg_dst),  reg_name(reg_src), constant);
	printf(INSTRUCTION_SLL, reg_name(reg_dst), reg_name(reg_src), constant);
}
//srl
static void mc_generate_srl(int reg_dst, int reg_src, int constant, FILE * fout)
{
	fprintf(fout, INSTRUCTION_SRL, reg_name(reg_dst),  reg_name(reg_src), constant);
	printf(INSTRUCTION_SRL, reg_name(reg_dst),  reg_name(reg_src), constant);
}
// label
static void mc_generate_label(int label_id, FILE * fout)
{
	fprintf(fout, INSTRUCTION_LBL, label_id);
	printf(INSTRUCTION_LBL, label_id);
}
// function label
static void mc_generate_functionlabel(char* functionname, FILE * fout)
{
	fprintf(fout, INSTRUCTION_FLBL, functionname);
	printf(INSTRUCTION_FLBL, functionname);
}
// j 
static void mc_generate_j(int label_id, FILE * fout)
{
	fprintf(fout, INSTRUCTION_J, label_id);
	printf(INSTRUCTION_J, label_id);
}
// jal
static void mc_generate_jal(char* functionname, FILE * fout)
{
	fprintf(fout, INSTRUCTION_JAL, functionname);
	printf(INSTRUCTION_JAL, functionname);
}
// return
static void mc_generate_jr(FILE * fout)
{
	fprintf(fout, INSTRUCTION_JR, reg_name(REG_ra));
	printf(INSTRUCTION_JR, reg_name(REG_ra));
}
// if  XXX goto XXX
static void mc_generate_ifgoto(enum Relop rel, int reg1, int reg2, int label_id, FILE * fout)
{
	switch (rel)
	{
		case R_GREATER: 
			fprintf(fout, INSTRUCTION_BGT, reg_name(reg1), reg_name(reg2), label_id);
			printf(INSTRUCTION_BGT, reg_name(reg1), reg_name(reg2), label_id);
			break;
		case R_LESS: 
			fprintf(fout, INSTRUCTION_BLT, reg_name(reg1), reg_name(reg2), label_id);
			printf(INSTRUCTION_BLT, reg_name(reg1), reg_name(reg2), label_id);
			break;
		case R_NOLESSTHAN: 
			fprintf(fout, INSTRUCTION_BGE, reg_name(reg1), reg_name(reg2), label_id);
			printf(INSTRUCTION_BGE, reg_name(reg1), reg_name(reg2), label_id);
			break;
		case R_NOGREATERTHAN: 
			fprintf(fout, INSTRUCTION_BLE, reg_name(reg1), reg_name(reg2), label_id);
			printf(INSTRUCTION_BLE, reg_name(reg1), reg_name(reg2), label_id);
			break;
		case R_EQUAL: 
			fprintf(fout, INSTRUCTION_BEQ, reg_name(reg1), reg_name(reg2), label_id);
			printf(INSTRUCTION_BEQ, reg_name(reg1), reg_name(reg2), label_id);
			break;
		case R_NOTEQUAL: 
			fprintf(fout, INSTRUCTION_BNE, reg_name(reg1), reg_name(reg2), label_id);
			printf(INSTRUCTION_BNE, reg_name(reg1), reg_name(reg2), label_id);
			break;
	}
}

// mov $reg1, $reg2
static void mc_generate_mov(int reg1, int reg2, FILE * fout)
{
	fprintf(fout, INSTRUCTION_MOV, reg_name(reg1), reg_name(reg2));
	printf(INSTRUCTION_MOV, reg_name(reg1), reg_name(reg2));
}

int array_size = 0;
//generate machine code
void mc_generate(FILE * fout)
{
	struct irNode *p, *q, *r, *s;
	struct irNode *begin, *end;
	struct irNode *global_begin = NULL, *main_begin = NULL;	
	struct irNode *global_end, *main_end;
	int i = 0;
	iop_zero = new_int_const_operand(0); /* #0 */

	// print the header 
	fprintf(fout, ASSEMBLY_HEADER);
	printf(ASSEMBLY_HEADER);
	offset = 0;
	offset_tmp=-4;
	var_size = 0; // local variables' size 
	param_size = 0; // parameters' size
	global_var_size = 0;
	int t;

	//global variable
	for (begin = ir_header()->next; begin != NULL; begin = end)
	{
		end = mc_find_next_func(begin);
		if(strcmp(begin->func.fname,"") == 0)
			global_begin = begin;
		else if(strcmp(begin->func.fname,"main") == 0)
			main_begin = begin;
	}
	if(main_begin==NULL) 
	{
		printf("Error: no main\n");
		return;
	}
	if(global_begin != NULL)
		global_end = mc_find_next_func(global_begin);
	main_end = mc_find_next_func(main_begin);
	begin = main_begin;
	end = main_end;
	// main start
	//new a function space
	mc_new_func();
	mc_generate_functionlabel(begin->func.fname, fout);	

	//parameter
	for (p = begin->next; p->ntype == IT_PARAM; p = p->next)
		mc_new_var(p->unaryop.x->vid, offset += 4, 4); 
	param_size = (-offset_tmp)+offset - 4; // parameter size
	//printf("%d\n", param_size);
	offset = offset_tmp; // reverse the offset to -4, -8 and so on
	// secondly,  add variables

	mc_generate_mov(REG_t7, REG_sp,fout); // store the original $sp

	// global variable print

	if(global_begin != NULL)
	{
		q = global_begin;
		for (; q != global_end; q = q->next)
		{
			//printf("111\n");
			//printf("%d", offset);
			switch (q->ntype)
			{

				case IT_DEC: 
					printf("dec %d\t", q->dec.width);
					offset -= q->dec.width - 4;
					//mc_new_var(q->dec.x->vid, offset, q->dec.width);
					mc_new_global_var(q->dec.x->vid, offset, q->dec.width);
					offset -= 4;
					break;

				case IT_ARRAY: 
					printf("array\t");
					offset -= q->array.width - 4;
					//mc_new_var(q->array.x->vid, offset, q->array.width);
					mc_new_global_var(q->array.x->vid, offset, q->array.width);
					//printf("%d ", mc_var_offset(q->array.x->vid));
					offset -= 4;	
					array_size = q->array.width / 4;
					if(q->array.def == 0) 
					{
						for(i = 0; i < array_size; i++)
						{
							mc_load_operand(iop_zero, REG_t0, fout);
							mc_generate_sw(REG_t0, mc_var_global_offset(q->array.x->vid)+i*4, REG_sp, fout);
						}					
					}	
					else
					{
						for(i = 0; i < array_size; i++)
						{	
							iop_int = new_int_const_operand(q->array.list[i]);
							mc_load_operand(iop_int, REG_t0, fout);
							mc_generate_sw(REG_t0, mc_var_global_offset(q->array.x->vid)+i*4, REG_sp, fout);
						}
					}	
					//iop_one = new_int_const_operand(1);  /* #1 */
					break;
				
				case IT_VAR:
					printf("var\t");
					if (mc_new_global_var(q->unaryop.x->vid, offset, 4)) 
						offset -= 4;
					if(q->unaryop.def == 0)
					{	//li t0, 0
						mc_load_operand(iop_zero, REG_t0, fout);
						mc_save_operand(q->unaryop.x, REG_t0, fout);
					}
					for (q = q->next; q->ntype == IT_ASSIGN; q = q->next)
					{
						//printf("assign\t");
						// $t0
						mc_load_operand(q->assign.right, REG_t0, fout);
						mc_save_operand(q->assign.left, REG_t0, fout);
					}
					q = q->prev;
					break;
			}
		}
	//printf("%d\n",offset);
		global_var_size = -offset + offset_tmp; // global variable size in total
	}
	//printf("%d %d %d\n", offset, offset_tmp, global_var_size);
	//printf("%d\n", global_var_size);
	offset_tmp = offset; // offset change	

	// change to main function 
	//store the original space
	p = p->prev;	
	r = p; //text begin
	for (; p != end; p = p->next)
	{
		switch (p->ntype)
		{
			case IT_ARRAY: 
				printf("array\t");
				offset -= p->array.width - 4;
				mc_new_var(p->array.x->vid, offset, p->array.width);
				//printf("%d ", mc_var_offset(q->array.x->vid));
				offset -= 4;	
				array_size = p->array.width / 4;
				if(p->array.def == 0) 
				{
					for(i = 0; i < array_size; i++)
					{
						mc_load_operand(iop_zero, REG_t0, fout);
						mc_generate_sw(REG_t0, mc_var_offset(p->array.x->vid)+i*4, REG_sp, fout);
					}					
				}	
				else
				{
					for(i = 0; i < array_size; i++)
					{	
						iop_int = new_int_const_operand(p->array.list[i]);
						mc_load_operand(iop_int, REG_t0, fout);
						mc_generate_sw(REG_t0, mc_var_offset(p->array.x->vid)+i*4, REG_sp, fout);
					}
				}	
				//iop_one = new_int_const_operand(1);  /* #1 */
				break;
			case IT_DEC: 				
				printf("dec\t");
				offset -= p->dec.width - 4;
				mc_new_var(p->dec.x->vid, offset, p->dec.width);
				offset -= 4;
				break;
			case IT_VAR:
				printf("var\t");
				if (mc_new_var(p->unaryop.x->vid, offset, 4)) 
					offset -= 4;
				if(p->unaryop.def == 0)
					//li t0, 0
					mc_load_operand(iop_zero, REG_t0, fout);
					mc_save_operand(p->unaryop.x, REG_t0, fout);
				for (p = p->next; p->ntype == IT_ASSIGN; p = p->next)
				{
					//printf("assign\t");
					// $t0
					mc_load_operand(p->assign.right, REG_t0, fout);
					mc_save_operand(p->assign.left, REG_t0, fout);
				}
				p = p->prev;
				break;
			case IT_ASSIGN: 
				if (mc_new_var(p->assign.left->vid, offset, 4)) 
					offset -= 4;
				break;
			case IT_ADD: case IT_SUB: case IT_MUL: case IT_DIV: case IT_MOD:
			case IT_BITAND: case IT_BITOR: case IT_BITXOR: case IT_BITNOT: 
			case IT_LSHIFT: case IT_RSHIFT:
				if (mc_new_var(p->binop.result->vid, offset, 4)) 
					offset -= 4;
				break;
			case IT_READ: case IT_CALL:
				if (mc_new_var(p->unaryop.x->vid, offset, 4))
					offset -= 4;
				break;
		}
	}
	main_var_size = -offset + offset_tmp; // variable size in total
	//printf("%d %d %d\n", offset, offset_tmp, main_var_size);
	//back to text
	p = r;
	for (; p != end; p = p->next)
	{
		//printf("loop begin\n");
		if(p->ntype == IT_LABEL)
		{
			printf("label\t");
			mc_generate_label(p->label.label_id, fout);
		}
		else if(p->ntype == IT_ASSIGN)
		{
			printf("assign\t");
			// $t0
			mc_load_operand(p->assign.right, REG_t0, fout);
			mc_save_operand(p->assign.left, REG_t0, fout);
		}
		else if(p->ntype == IT_ADD)
		{
			printf("add\t");
			//$t0 = $t0 op $t1
			mc_load_operand(p->binop.op1, REG_t0,fout);
			mc_load_operand(p->binop.op2, REG_t1,fout);
			mc_generate_add(REG_t0, REG_t0, REG_t1,fout);
			mc_save_operand(p->binop.result, REG_t0,fout);
		}
		else if(p->ntype == IT_SUB)
		{
			printf("sub\t");
			mc_load_operand(p->binop.op1, REG_t0,fout);
			mc_load_operand(p->binop.op2, REG_t1,fout);
			mc_generate_sub(REG_t0, REG_t0, REG_t1,fout);
			mc_save_operand(p->binop.result, REG_t0,fout);
		}
		else if(p->ntype == IT_MUL)
		{
			printf("mul\t");
			mc_load_operand(p->binop.op1, REG_t0,fout);
			mc_load_operand(p->binop.op2, REG_t1,fout);
			mc_generate_mul(REG_t0, REG_t0, REG_t1,fout);
			mc_save_operand(p->binop.result, REG_t0,fout);
		}
		else if(p->ntype == IT_DIV)
		{
			printf("div\t");
			mc_load_operand(p->binop.op1, REG_t0,fout);
			mc_load_operand(p->binop.op2, REG_t1,fout);
			mc_generate_div(REG_t0, REG_t0, REG_t1,fout);
			mc_save_operand(p->binop.result, REG_t0,fout);
		}
		else if(p->ntype == IT_MOD)
		{
			printf("mod\t");
			mc_load_operand(p->binop.op1, REG_t0,fout);
			mc_load_operand(p->binop.op2, REG_t1,fout);
			mc_generate_mod(REG_t0, REG_t0, REG_t1,fout);
			mc_save_operand(p->binop.result, REG_t0,fout);
		}
		else if(p->ntype == IT_BITAND)
		{
			printf("bitand\t");
			mc_load_operand(p->binop.op1, REG_t0,fout);
			mc_load_operand(p->binop.op2, REG_t1,fout);
			mc_generate_and(REG_t0, REG_t0, REG_t1,fout);
			mc_save_operand(p->binop.result, REG_t0,fout);
		}
		else if(p->ntype == IT_BITOR)
		{
			printf("bitor\t");
			mc_load_operand(p->binop.op1, REG_t0,fout);
			mc_load_operand(p->binop.op2, REG_t1,fout);
			mc_generate_or(REG_t0, REG_t0, REG_t1,fout);
			mc_save_operand(p->binop.result, REG_t0,fout);
		}
		else if(p->ntype == IT_BITXOR)
		{
			printf("bitxor\t");
			mc_load_operand(p->binop.op1, REG_t0,fout);
			mc_load_operand(p->binop.op2, REG_t1,fout);
			mc_generate_xor(REG_t0, REG_t0, REG_t1,fout);
			mc_save_operand(p->binop.result, REG_t0,fout);
		}
		else if(p->ntype == IT_BITNOT)
		{
			printf("bitnot\t");
			mc_load_operand(p->binop.op1, REG_t0,fout);
			//mc_load_operand(p->binop.op2, REG_t1,fout);
			mc_generate_not(REG_t0, REG_t0,fout);
			mc_save_operand(p->binop.result, REG_t0,fout);
		}
		else if(p->ntype == IT_LSHIFT)
		{
			printf("lshift\t");
			mc_load_operand(p->binop.op1, REG_t0,fout);
			//mc_load_operand(p->binop.op2, REG_t1,fout);
			mc_generate_sll(REG_t0, REG_t0,  p->binop.op2->ival,fout);
			mc_save_operand(p->binop.result, REG_t0,fout);
		}
		else if(p->ntype == IT_RSHIFT)
		{
			printf("rshift\t");
			mc_load_operand(p->binop.op1, REG_t0,fout);
			//mc_load_operand(p->binop.op2, REG_t1,fout);
			mc_generate_srl(REG_t0, REG_t0, p->binop.op2->ival, fout);
			mc_save_operand(p->binop.result, REG_t0,fout);
		}
		else if(p->ntype == IT_ADDASSIGN)
		{
			printf("addassign\t");
			mc_load_operand(p->binop.op1, REG_t0,fout);
			mc_load_operand(p->binop.op2, REG_t1,fout);
			mc_generate_add(REG_t0, REG_t0, REG_t1,fout);
			mc_save_operand(p->binop.result, REG_t0,fout);
		}
		else if(p->ntype == IT_SUBASSIGN)
		{
			printf("subassign\t");
			mc_load_operand(p->binop.op1, REG_t0,fout);
			mc_load_operand(p->binop.op2, REG_t1,fout);
			mc_generate_sub(REG_t0, REG_t0, REG_t1,fout);
			mc_save_operand(p->binop.result, REG_t0,fout);
		}
		else if(p->ntype == IT_MULASSIGN)
		{
			printf("mulassign\t");
			mc_load_operand(p->binop.op1, REG_t0,fout);
			mc_load_operand(p->binop.op2, REG_t1,fout);
			mc_generate_mul(REG_t0, REG_t0, REG_t1,fout);
			mc_save_operand(p->binop.result, REG_t0,fout);
		}
		else if(p->ntype == IT_DIVASSIGN)
		{
			printf("divassign\t");
			mc_load_operand(p->binop.op1, REG_t0,fout);
			mc_load_operand(p->binop.op2, REG_t1,fout);
			mc_generate_div(REG_t0, REG_t0, REG_t1,fout);
			mc_save_operand(p->binop.result, REG_t0,fout);
		}
		else if(p->ntype == IT_BITANDASSIGN)
		{
			printf("bitandassign\t");
			mc_load_operand(p->binop.op1, REG_t0,fout);
			mc_load_operand(p->binop.op2, REG_t1,fout);
			mc_generate_and(REG_t0, REG_t0, REG_t1,fout);
			mc_save_operand(p->binop.result, REG_t0,fout);
		}
		else if(p->ntype == IT_BITORASSIGN)
		{
			printf("bitorassign\t");
			mc_load_operand(p->binop.op1, REG_t0,fout);
			mc_load_operand(p->binop.op2, REG_t1,fout);
			mc_generate_or(REG_t0, REG_t0, REG_t1,fout);
			mc_save_operand(p->binop.result, REG_t0,fout);
		}
		else if(p->ntype == IT_BITXORASSIGN)
		{
			printf("bitxorassign\t");
			mc_load_operand(p->binop.op1, REG_t0,fout);
			mc_load_operand(p->binop.op2, REG_t1,fout);
			mc_generate_xor(REG_t0, REG_t0, REG_t1,fout);
			mc_save_operand(p->binop.result, REG_t0,fout);
		}
		else if(p->ntype == IT_LSHIFTASSIGN)
		{
			printf("lshiftassign\t");
			mc_load_operand(p->binop.op1, REG_t0,fout);
			//mc_load_operand(p->binop.op2, REG_t1,fout);
			mc_generate_sll(REG_t0, REG_t0,  p->binop.op2->ival,fout);
			mc_save_operand(p->binop.result, REG_t0,fout);
		}
		else if(p->ntype == IT_RSHIFTASSIGN)
		{
			printf("rshiftassign\t");
			mc_load_operand(p->binop.op1, REG_t0,fout);
			//mc_load_operand(p->binop.op2, REG_t1,fout);
			mc_generate_srl(REG_t0, REG_t0, p->binop.op2->ival, fout);
			mc_save_operand(p->binop.result, REG_t0,fout);
		}
		else if(p->ntype == IT_INCR)
		{
			printf("incr\t");
			mc_load_operand(p->binop.op1, REG_t0,fout);
			mc_generate_li(REG_t2, 1, fout);
			mc_generate_add(REG_t0, REG_t0, REG_t2,fout);
			mc_save_operand(p->binop.result, REG_t0,fout);
		}
		else if(p->ntype == IT_DECR)
		{
			printf("decr\t");
			mc_load_operand(p->binop.op1, REG_t0,fout);
			mc_generate_li(REG_t2, 1, fout);
			mc_generate_sub(REG_t0, REG_t0, REG_t2,fout);
			mc_save_operand(p->binop.result, REG_t0,fout);
		}
		else if(p->ntype == IT_GOTO)
		{
			printf("goto\t");
			mc_generate_j(p->labelgoto.label->label.label_id,fout);
		}
		else if(p->ntype == IT_IFGOTO)
		{
			printf("ifgoto\t");
			mc_load_operand(p->ifgoto.x, REG_t0,fout);
			mc_load_operand(p->ifgoto.y, REG_t1,fout);
			mc_generate_ifgoto(p->ifgoto.rel, REG_t0, REG_t1, p->ifgoto.label->label.label_id,fout); 
		}
		else if(p->ntype == IT_ST)
		{					
			printf("store\t");
			// find all args
			t = 0;	// store args information
			//store the original place
			for (; p->ntype == IT_ST; p = p->next)
			{
				// $t0 = ...
				mc_load_operand(p->unaryop.x, REG_t0,fout);
				// push into stack
				mc_generate_sw(REG_t0, offset  - t, REG_sp,fout);
				//mc_push(REG_t0);
				t += 4;
			}
			//printf("%d\n", t);
			if(p->ntype != IT_CALL)
				p=p->prev;
			// store $ra 
			//mc_push(REG_ra, fout);
			mc_generate_sw(REG_ra, offset - t, REG_sp,fout);
			// sp = sp - (SIZE + args + RETURN)($t4) 
			mc_generate_li(REG_t4, main_var_size + (-offset_tmp)+ t,fout);
			mc_generate_sub(REG_sp, REG_sp, REG_t4,fout);
			stackpointer = main_var_size + t + 4;
			// CALL
			//printf("%s\n",p->call.fname);
			if(p->call.fname != NULL)
			{
				mc_generate_jal(p->call.fname,fout);	
			}
			// get back to previous $sp
			// get back the $ra
			mc_pop(REG_ra,fout);
			//sp = sp + t
			mc_generate_li(REG_t4, t,fout);
			mc_generate_add(REG_sp, REG_sp, REG_t4,fout);
			// sp = sp + var_size
			mc_generate_li(REG_t4, main_var_size + (-offset_tmp) - 4,fout);
			mc_generate_add(REG_sp, REG_sp, REG_t4,fout);
			stackpointer = 0;
			// return value: $v0 
			if(p->ntype == IT_CALL)
				mc_save_operand(p->call.x, REG_v0,fout);
		}
		else if(p->ntype == IT_CALL)
		{
			printf("call\t");
			//  sp = sp - (SIZE - 4)($t4)
			mc_generate_li(REG_t4, main_var_size,fout);
			mc_generate_sub(REG_sp, REG_sp, REG_t4,fout);
			// store $ra
			mc_push(REG_ra,fout);
			stackpointer = main_var_size +  4;
			// set the argment
			//mc_load_operand(p->unaryop.x, REG_a0,fout);
			// CALL
			mc_generate_jal(p->call.fname,fout);
			// get back to previous $sp
			// get back the $ra
			mc_pop(REG_ra,fout);
			// sp = sp + var_size
			mc_generate_li(REG_t4, main_var_size,fout);
			mc_generate_add(REG_sp, REG_sp, REG_t4,fout);
			stackpointer = 0;
			// the return value: $v0
			mc_save_operand(p->call.x, REG_v0,fout);
		}
		else if(p->ntype == IT_READ)
		{
			printf("read\t");
			// save $ra to $t0
			mc_generate_mov(REG_t0, REG_ra,fout);
			// call read
			mc_generate_jal("read",fout);
			// save the return value
			mc_save_operand(p->unaryop.x, REG_v0,fout);
			// get back the $ra
			mc_generate_mov(REG_ra, REG_t0,fout);
		}
		else if(p->ntype == IT_WRITE)
		{
			printf("write\t");
			// save $ra to $t0
			mc_generate_mov(REG_t0, REG_ra,fout);
			// set the argment
			mc_load_operand(p->unaryop.x, REG_a0,fout);
			// call write
			mc_generate_jal("write",fout);
			// get back the $ra
			mc_generate_mov(REG_ra, REG_t0,fout);
		}
		else if(p->ntype == IT_RETURN)
		{
			printf("return\t");
			mc_load_operand(p->unaryop.x, REG_v0,fout);
			mc_generate_jr(fout);
		}	
	}

	// other function
	for (begin = ir_header()->next; begin != NULL; begin = end)
	{
		offset = 0;
		var_size = 0; // local variables' size 
		param_size = 0; // parameters' size
		int t;
		end = mc_find_next_func(begin);
		mc_new_func();
		// print the function
		if((strcmp(begin->func.fname,"") != 0)&&(strcmp(begin->func.fname,"main") != 0))//function not declaration
		{
			mc_generate_functionlabel(begin->func.fname, fout);
			// firstly, set all parameters' offset(with positive numbers) 
			for (p = begin->next; p->ntype == IT_PARAM; p = p->next)
				mc_new_var(p->unaryop.x->vid, offset += 4, 4); 
			param_size = (-offset_tmp)+offset + 4; // parameter size
			offset = offset_tmp; // reverse the offset to -4, -8 and so on
			// secondly,  add variables
			//store the original space
			p = p->prev;	
			s = p; //text begin
			for (; p != end; p = p->next)
			{
				//printf("111\n");
				switch (p->ntype)
				{
					case IT_ARRAY: 
						printf("array\t");
						offset -= p->array.width - 4;
						mc_new_var(p->array.x->vid, offset, p->array.width);
						//printf("%d ", mc_var_offset(q->array.x->vid));
						offset -= 4;	
						array_size = p->array.width / 4;
						if(p->array.def == 0) 
						{
							for(i = 0; i < array_size; i++)
							{
								mc_load_operand(iop_zero, REG_t0, fout);
								mc_generate_sw(REG_t0, mc_var_offset(p->array.x->vid)+i*4, REG_sp, fout);
							}					
						}	
						else
						{
							for(i = 0; i < array_size; i++)
							{	
								iop_int = new_int_const_operand(p->array.list[i]);
								mc_load_operand(iop_int, REG_t0, fout);
								mc_generate_sw(REG_t0, mc_var_offset(p->array.x->vid)+i*4, REG_sp, fout);
							}
						}	
						//iop_one = new_int_const_operand(1);  /* #1 */
						break;
					case IT_DEC: 
						printf("dec\t");
						offset -= p->dec.width - 4;
						mc_new_var(p->dec.x->vid, offset, p->dec.width);
						offset -= 4;
						break;
					case IT_VAR:
						printf("var\t");
						if (mc_new_var(p->unaryop.x->vid, offset, 4)) 
							offset -= 4;
						if(p->unaryop.def == 0)
							//li t0, 0
							mc_load_operand(iop_zero, REG_t0, fout);
							mc_save_operand(p->unaryop.x, REG_t0, fout);
						for (p = p->next; p->ntype == IT_ASSIGN; p = p->next)
						{
							printf("assign\t");
							// $t0
							mc_load_operand(p->assign.right, REG_t0, fout);
							mc_save_operand(p->assign.left, REG_t0, fout);
						}
						p = p->prev;
						break;
					case IT_ASSIGN: 
						if (mc_new_var(p->assign.left->vid, offset, 4)) 
							offset -= 4;
						break;
					case IT_ADD: case IT_SUB: case IT_MUL: case IT_DIV: case IT_MOD:
					case IT_BITAND: case IT_BITOR: case IT_BITXOR: case IT_BITNOT: 
					case IT_LSHIFT: case IT_RSHIFT:
						if (mc_new_var(p->binop.result->vid, offset, 4)) 
							offset -= 4;
						break;
					case IT_READ: case IT_CALL:
						if (mc_new_var(p->unaryop.x->vid, offset, 4))
							offset -= 4;
						break;
				}
			}
			var_size = -offset + offset_tmp; // variable size in total
			//offset_tmp = offset;				
			p = s; //back to text

			// translate
			for (; p != end; p = p->next)
			{
				//printf("loop begin\n");
				if(p->ntype == IT_LABEL)
				{
					printf("label\t");
					mc_generate_label(p->label.label_id, fout);
				}
				else if(p->ntype == IT_ASSIGN)
				{
					printf("assign\t");
					// $t0
					mc_load_operand(p->assign.right, REG_t0, fout);
					mc_save_operand(p->assign.left, REG_t0, fout);
				}
				else if(p->ntype == IT_ADD)
				{
					printf("add\t");
					//$t0 = $t0 op $t1
					mc_load_operand(p->binop.op1, REG_t0,fout);
					mc_load_operand(p->binop.op2, REG_t1,fout);
					mc_generate_add(REG_t0, REG_t0, REG_t1,fout);
					mc_save_operand(p->binop.result, REG_t0,fout);
				}
				else if(p->ntype == IT_SUB)
				{
					printf("sub\t");
					mc_load_operand(p->binop.op1, REG_t0,fout);
					mc_load_operand(p->binop.op2, REG_t1,fout);
					mc_generate_sub(REG_t0, REG_t0, REG_t1,fout);
					mc_save_operand(p->binop.result, REG_t0,fout);
				}
				else if(p->ntype == IT_MUL)
				{
					printf("mul\t");
					mc_load_operand(p->binop.op1, REG_t0,fout);
					mc_load_operand(p->binop.op2, REG_t1,fout);
					mc_generate_mul(REG_t0, REG_t0, REG_t1,fout);
					mc_save_operand(p->binop.result, REG_t0,fout);
				}
				else if(p->ntype == IT_DIV)
				{
					printf("div\t");
					mc_load_operand(p->binop.op1, REG_t0,fout);
					mc_load_operand(p->binop.op2, REG_t1,fout);
					mc_generate_div(REG_t0, REG_t0, REG_t1,fout);
					mc_save_operand(p->binop.result, REG_t0,fout);
				}
				else if(p->ntype == IT_MOD)
				{
					printf("mod\t");
					mc_load_operand(p->binop.op1, REG_t0,fout);
					mc_load_operand(p->binop.op2, REG_t1,fout);
					mc_generate_mod(REG_t0, REG_t0, REG_t1,fout);
					mc_save_operand(p->binop.result, REG_t0,fout);
				}
				else if(p->ntype == IT_BITAND)
				{
					printf("bitand\t");
					mc_load_operand(p->binop.op1, REG_t0,fout);
					mc_load_operand(p->binop.op2, REG_t1,fout);
					mc_generate_and(REG_t0, REG_t0, REG_t1,fout);
					mc_save_operand(p->binop.result, REG_t0,fout);
				}
				else if(p->ntype == IT_BITOR)
				{
					printf("bitor\t");
					mc_load_operand(p->binop.op1, REG_t0,fout);
					mc_load_operand(p->binop.op2, REG_t1,fout);
					mc_generate_or(REG_t0, REG_t0, REG_t1,fout);
					mc_save_operand(p->binop.result, REG_t0,fout);
				}
				else if(p->ntype == IT_BITXOR)
				{
					printf("bitxor\t");
					mc_load_operand(p->binop.op1, REG_t0,fout);
					mc_load_operand(p->binop.op2, REG_t1,fout);
					mc_generate_xor(REG_t0, REG_t0, REG_t1,fout);
					mc_save_operand(p->binop.result, REG_t0,fout);
				}
				else if(p->ntype == IT_BITNOT)
				{
					printf("bitnot\t");
					mc_load_operand(p->binop.op1, REG_t0,fout);
					//mc_load_operand(p->binop.op2, REG_t1,fout);
					mc_generate_not(REG_t0, REG_t0,fout);
					mc_save_operand(p->binop.result, REG_t0,fout);
				}
				else if(p->ntype == IT_LSHIFT)
				{
					printf("lshift\t");
					mc_load_operand(p->binop.op1, REG_t0,fout);
					//mc_load_operand(p->binop.op2, REG_t1,fout);
					mc_generate_sll(REG_t0, REG_t0,  p->binop.op2->ival,fout);
					mc_save_operand(p->binop.result, REG_t0,fout);
				}
				else if(p->ntype == IT_RSHIFT)
				{
					printf("rshift\t");
					mc_load_operand(p->binop.op1, REG_t0,fout);
					//mc_load_operand(p->binop.op2, REG_t1,fout);
					mc_generate_srl(REG_t0, REG_t0, p->binop.op2->ival, fout);
					mc_save_operand(p->binop.result, REG_t0,fout);
				}
				else if(p->ntype == IT_ADDASSIGN)
				{
					printf("addassign\t");
					mc_load_operand(p->binop.op1, REG_t0,fout);
					mc_load_operand(p->binop.op2, REG_t1,fout);
					mc_generate_add(REG_t0, REG_t0, REG_t1,fout);
					mc_save_operand(p->binop.result, REG_t0,fout);
				}
				else if(p->ntype == IT_SUBASSIGN)
				{
					printf("subassign\t");
					mc_load_operand(p->binop.op1, REG_t0,fout);
					mc_load_operand(p->binop.op2, REG_t1,fout);
					mc_generate_sub(REG_t0, REG_t0, REG_t1,fout);
					mc_save_operand(p->binop.result, REG_t0,fout);
				}
				else if(p->ntype == IT_MULASSIGN)
				{
					printf("mulassign\t");
					mc_load_operand(p->binop.op1, REG_t0,fout);
					mc_load_operand(p->binop.op2, REG_t1,fout);
					mc_generate_mul(REG_t0, REG_t0, REG_t1,fout);
					mc_save_operand(p->binop.result, REG_t0,fout);
				}
				else if(p->ntype == IT_DIVASSIGN)
				{
					printf("divassign\t");
					mc_load_operand(p->binop.op1, REG_t0,fout);
					mc_load_operand(p->binop.op2, REG_t1,fout);
					mc_generate_div(REG_t0, REG_t0, REG_t1,fout);
					mc_save_operand(p->binop.result, REG_t0,fout);
				}
				else if(p->ntype == IT_BITANDASSIGN)
				{
					printf("bitandassign\t");
					mc_load_operand(p->binop.op1, REG_t0,fout);
					mc_load_operand(p->binop.op2, REG_t1,fout);
					mc_generate_and(REG_t0, REG_t0, REG_t1,fout);
					mc_save_operand(p->binop.result, REG_t0,fout);
				}
				else if(p->ntype == IT_BITORASSIGN)
				{
					printf("bitorassign\t");
					mc_load_operand(p->binop.op1, REG_t0,fout);
					mc_load_operand(p->binop.op2, REG_t1,fout);
					mc_generate_or(REG_t0, REG_t0, REG_t1,fout);
					mc_save_operand(p->binop.result, REG_t0,fout);
				}
				else if(p->ntype == IT_BITXORASSIGN)
				{
					printf("bitxorassign\t");
					mc_load_operand(p->binop.op1, REG_t0,fout);
					mc_load_operand(p->binop.op2, REG_t1,fout);
					mc_generate_xor(REG_t0, REG_t0, REG_t1,fout);
					mc_save_operand(p->binop.result, REG_t0,fout);
				}
				else if(p->ntype == IT_LSHIFTASSIGN)
				{
					printf("lshiftassign\t");
					mc_load_operand(p->binop.op1, REG_t0,fout);
					//mc_load_operand(p->binop.op2, REG_t1,fout);
					mc_generate_sll(REG_t0, REG_t0,  p->binop.op2->ival,fout);
					mc_save_operand(p->binop.result, REG_t0,fout);
				}
				else if(p->ntype == IT_RSHIFTASSIGN)
				{
					printf("rshiftassign\t");
					mc_load_operand(p->binop.op1, REG_t0,fout);
					//mc_load_operand(p->binop.op2, REG_t1,fout);
					mc_generate_srl(REG_t0, REG_t0, p->binop.op2->ival, fout);
					mc_save_operand(p->binop.result, REG_t0,fout);
				}
				else if(p->ntype == IT_INCR)
				{
					printf("incr\t");
					mc_load_operand(p->binop.op1, REG_t0,fout);
					mc_generate_li(REG_t2, 1, fout);
					mc_generate_add(REG_t0, REG_t0, REG_t2,fout);
					mc_save_operand(p->binop.result, REG_t0,fout);
				}
				else if(p->ntype == IT_DECR)
				{
					printf("decr\t");
					mc_load_operand(p->binop.op1, REG_t0,fout);
					mc_generate_li(REG_t2, 1, fout);
					mc_generate_sub(REG_t0, REG_t0, REG_t2,fout);
					mc_save_operand(p->binop.result, REG_t0,fout);
				}
				else if(p->ntype == IT_GOTO)
				{
					printf("goto\t");
					mc_generate_j(p->labelgoto.label->label.label_id,fout);
				}
				else if(p->ntype == IT_IFGOTO)
				{
					printf("ifgoto\t");
					mc_load_operand(p->ifgoto.x, REG_t0,fout);
					mc_load_operand(p->ifgoto.y, REG_t1,fout);
					mc_generate_ifgoto(p->ifgoto.rel, REG_t0, REG_t1, p->ifgoto.label->label.label_id,fout); 
				}
				else if(p->ntype == IT_ST)
				{					
					printf("store\t");
					// find all args
					t = 0;	// store args information
					//store the original place
					for (; p->ntype == IT_ST; p = p->next)
					{
						// $t0 = ...
						mc_load_operand(p->unaryop.x, REG_t0,fout);
						// push into stack
						mc_generate_sw(REG_t0, offset  - t, REG_sp,fout);
						//mc_push(REG_t0);
						t += 4;
					}
					//printf("%d\n", t);
					if(p->ntype != IT_CALL)
						p=p->prev;
					// store $ra 
					//mc_push(REG_ra, fout);
					mc_generate_sw(REG_ra, offset - t, REG_sp,fout);
					// sp = sp - (SIZE + args + RETURN)($t4) 
					mc_generate_li(REG_t4, var_size + (-offset_tmp)+ t,fout);
					mc_generate_sub(REG_sp, REG_sp, REG_t4,fout);
					stackpointer = var_size + t + 4;
					// CALL
					//printf("%s\n",p->call.fname);
					if(p->call.fname != NULL)
						mc_generate_jal(p->call.fname,fout);	
					// get back to previous $sp
					// get back the $ra
					mc_pop(REG_ra,fout);
					//sp = sp + t
					mc_generate_li(REG_t4, t,fout);
					mc_generate_add(REG_sp, REG_sp, REG_t4,fout);
					// sp = sp + var_size
					mc_generate_li(REG_t4, var_size + (-offset_tmp) - 4,fout);
					mc_generate_add(REG_sp, REG_sp, REG_t4,fout);
					// return value: $v0 
					if(p->ntype == IT_CALL)
						mc_save_operand(p->call.x, REG_v0,fout);
				}
				else if(p->ntype == IT_CALL)
				{
					printf("call\t");
					//  sp = sp - (SIZE - 4)($t4)
					mc_generate_li(REG_t4, var_size,fout);
					mc_generate_sub(REG_sp, REG_sp, REG_t4,fout);
					// store $ra
					mc_push(REG_ra,fout);
					stackpointer = var_size +  4;
					// set the argment
					//mc_load_operand(p->unaryop.x, REG_a0,fout);
					// CALL
					mc_generate_jal(p->call.fname,fout);
					// get back to previous $sp
					// get back the $ra
					mc_pop(REG_ra,fout);
					// sp = sp + var_size
					mc_generate_li(REG_t4, var_size,fout);
					mc_generate_add(REG_sp, REG_sp, REG_t4,fout);
					// the return value: $v0
					mc_save_operand(p->call.x, REG_v0,fout);
				}
				else if(p->ntype == IT_READ)
				{
					printf("read\t");
					// save $ra to $t0
					mc_generate_mov(REG_t0, REG_ra,fout);
					// call read
					mc_generate_jal("read",fout);
					// save the return value
					mc_save_operand(p->unaryop.x, REG_v0,fout);
					// get back the $ra
					mc_generate_mov(REG_ra, REG_t0,fout);
				}
				else if(p->ntype == IT_WRITE)
				{
					printf("write\t");
					// save $ra to $t0
					mc_generate_mov(REG_t0, REG_ra,fout);
					// set the argment
					mc_load_operand(p->unaryop.x, REG_a0,fout);
					// call write
					mc_generate_jal("write",fout);
					// get back the $ra
					mc_generate_mov(REG_ra, REG_t0,fout);
				}
				else if(p->ntype == IT_RETURN)
				{
					printf("return\t");
					mc_load_operand(p->unaryop.x, REG_v0,fout);
					mc_generate_jr(fout);
				}	
			}
		}
	}
}

//initial
void mc_init(FILE* fout)
{
	listhdr = (struct MCVar*)malloc(sizeof(struct MCVar));
	listhdr->next = listhdr->prev = NULL;
	globallisthdr = (struct MCVar*)malloc(sizeof(struct MCVar));
	globallisthdr->next = globallisthdr->prev = NULL;
}

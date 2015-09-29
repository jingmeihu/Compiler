#include "def.h"

//symbol table
struct hashheader shash[HASH_LENGTH];
struct sstackheader* sslhdr = NULL; // Scope Stack Link
int cur_scope_depth = 0;
// HASH Table 

// get hash from string
int str_hash(char* name)
{
	unsigned int val = 0, i; 
	for ( ; *name; ++name) 
	{ 
		val = (val << 2) + *name; 
		if ( i = val & ~0x3fff)
			val = (val ^ (i >> 12)) & 0x3fff; 
	} 
	return val % HASH_LENGTH; 
}

// create an SymbolNode and initialize it
struct SymbolNode* new_SymbolNode()
{
	struct SymbolNode* p;
	p = (struct SymbolNode *) malloc(sizeof(struct SymbolNode));
	p->up = p->down = p->left = p->right = NULL;
	p->kind = SK_UNKNOWN;
	p->name = NULL;
	p->type = NULL;
	p->cur_variable.vid = -1; 
	return p;
}

// find nearest SymbolNode
struct SymbolNode* find_SymbolNode(char* name, enum SymbolKind kind)
{
	struct SymbolNode* p = &shash[str_hash(name)];
	while (p->right)
	{
		if (strcmp(p->right->name, name) == 0 && 
			(kind == SK_UNKNOWN || p->right->kind == kind))
			return p->right;
		p = p->right;
	}
	return NULL;
}

// find nearest SymbolNode through the same Scope
struct SymbolNode* find_SymbolNode_scope(struct sstackheader* ssh, char* name, enum SymbolKind kind)
{
	struct SymbolNode* p = ssh->down;
	while (p)
	{
		if (strcmp(p->name, name) == 0 && 
			(kind == SK_UNKNOWN || p->kind == kind))
			return p;
		p = p->down;
	}
	return NULL;
}

// find nearest SymbolNode through the CURRENT Scope
struct SymbolNode* find_SymbolNode_cur_scope(char* name, enum SymbolKind kind)
{
	return find_SymbolNode_scope(cur_scope(), name, kind);
}

// add an SymbolNode into hashtable, and returns SymbolNode pointer if succ
struct SymbolNode* add_into_shash(struct sstackheader* sshdr, char* name, enum SymbolKind kind, struct TypeNode* type)
{
	struct SymbolNode* p = new_SymbolNode();
	struct hashheader* hhdr = &shash[str_hash(name)];

	if (sshdr == NULL)
		return NULL;

	p->name = name;
	p->kind = kind;
	p->type = type;

	//link into the left-right list
	p->right = hhdr->right;
	p->left = hhdr;
	hhdr->right = p;
	if (p->right)
		p->right->left = p;

	// link into the up-down list
	p->down = sshdr->down;
	p->up = sshdr;
	sshdr->down = p;
	if (p->down)
		p->down->up = p;

	return p;
}
// variable property
void set_SymbolNode_vid(struct SymbolNode* n, int vid, enum VarType vtype)
{
	n->cur_variable.vid = vid;
	n->cur_variable.vtype = vtype;
}

// get variable ID */
int get_SymbolNode_vid(struct SymbolNode* n)
{
	return n->cur_variable.vid;
}

// get variable type
enum VarType get_SymbolNode_var_type(struct SymbolNode* n)
{
	return n->cur_variable.vtype;
}

// add into CURRENT scope
struct SymbolNode* add_SymbolNode(char* name, enum SymbolKind kind, struct TypeNode* type)
{
	return add_into_shash(cur_scope(), name, kind, type);
}
// add into PARENT of current scope
struct SymbolNode* add_SymbolNode_parent(char* name, enum SymbolKind kind, struct TypeNode* type)
{
	return add_into_shash(cur_scope()->right, name, kind, type);
}


// Scope Stack
struct sstackheader* new_sstackheader()
{
	struct sstackheader* p = new_SymbolNode();

	p->kind = SK_SSTACKHEADER;
	
	return p;
}
void display_scopes()
{
	struct sstackheader* p = sslhdr;
	while (p)
	{
		p = p->right;
	}
	printf("\n");
}

// create a new scope
struct sstackheader* new_scope()
{
	struct sstackheader *p = new_sstackheader();
	
	p->right = sslhdr->right;
	p->left = sslhdr;
	sslhdr->right = p;
	if (p->right)
		p->right->left = p;

	cur_scope_depth++;
	return p; 
}

// get the current scope node
struct sstackheader* cur_scope()
{
	return sslhdr->right;
}


// delete CURRENT scope
int del_current_scope()
{

	struct sstackheader* cur = cur_scope();
	if (!cur)
		return -1;
	
	cur = cur->down;
	
	while (cur)
	{
		struct SymbolNode* down = cur->down;
		cur->left->right = cur->right;
		if (cur->right)
			cur->right->left = cur->left;

		cur->up->down = cur->down;
		if (cur->down)
			cur->down->up = cur->up;

		free(cur); 

		cur = down;
	}

	cur = cur_scope();
	sslhdr->right = cur->right;
	if (cur->right)
		cur->right->left = sslhdr;
	free(cur);

	cur_scope_depth--;
	return 0;
}


//type node

// create a TypeNode
struct TypeNode* new_TypeNode(enum TypeKind kind)
{
	struct TypeNode* n;
	n = (struct TypeNode *)malloc(sizeof(struct TypeNode));
	n->width = 4;
	n->kind = kind;
	return n;
}

// try to delete a TypeNode
int del_TypeNode(struct TypeNode *n)
{
	return 0; 
}

// create an array made of basic type
struct TypeNode* new_TypeNode_arr_by_basic(enum TypeKind kind, int size)
{
	return new_TypeNode_arr_by_TypeNode(new_TypeNode_basic(kind), size);
}

// create an array made of a TypeNode
struct TypeNode* new_TypeNode_arr_by_TypeNode(struct TypeNode* sub, int size)
{
	struct TypeNode* n = new_TypeNode(TK_ARRAY);
	int i;
	n->array.type = sub;
	n->array.size = size;
	n->array.list = (int*) malloc(sizeof(int) * size);
	for(i = 0; i < size; i++) n->array.list[i] = 0;
	n->width = size * sub->width;
	return n;
}

// create a struct TypeNode
struct TypeNode* new_TypeNode_struct(struct TypeStructList* field)
{
	struct TypeNode* n = new_TypeNode(TK_STRUCT);
	n->field_list = field;
	n->width = field->width;
	return n;
}

// create a field-list and link it on 
struct TypeStructList* new_fieldlist_link(struct TypeNode* n, char* name, struct TypeStructList* next)
{
	struct TypeStructList* f;
	f = (struct TypeStructList *) malloc(sizeof(struct TypeStructList));
	f->name = name;
	f->type = n;
	f->next = next;
	f->offset = (next ? next->width : 0);
	f->width = f->offset + n->width; 
	return f;
}

// create a func
struct func* new_func(struct TypeNode* return_type, struct TypeStructList* param, int func_defined)
{
	struct func* f;
	f = (struct func *) malloc(sizeof(struct func));
	f->return_type = return_type;
	f->param = param;
	f->defined = func_defined;
	return f;
}

// delete a func
void del_func(struct func* f)
{
	struct TypeStructList *i = f->param;
	while(i)
	{
		struct TypeStructList *t = i->next;
		free(i);
		i = t;
	}
	del_TypeNode(f->return_type);
	free(f);
	
}

// create a TypeNode made of func
struct TypeNode* new_TypeNode_func(struct func* f)
{
	struct TypeNode* n = new_TypeNode(TK_FUNC);
	n->function = f;

	return n;
}

// judge whether 2 types are same, 0 if same 
int typecmp(struct TypeNode* n1, struct TypeNode* n2)
{
	// ptr is the same
	if (n1 == n2)
		return 0;
	
	// not the same kind
	if (n1->kind != n2->kind)
		return 1;	
	if (n1->kind == TK_ARRAY)
		return typecmp(n1->array.type, n2->array.type);
	if (n1->kind == TK_STRUCT)
		return fieldcmp(n1->field_list, n2->field_list);
}

// judge whether 2 field-list are same, 0 if same
int fieldcmp(struct TypeStructList* f1, struct TypeStructList* f2)
{
	while (f1 && f2)
	{
		if (typecmp(f1->type, f2->type)) 
			return 1;
		f1 = f1->next;
		f2 = f2->next;
	}
	if (f1 || f2)
		return 1;
	return 0;
}

// judge whether 2 funcs are same, 0 if same
int funccmp(struct func* f1, struct func* f2)
{
	if (typecmp(f1->return_type, f2->return_type) != 0)
		return 1;
	if (fieldcmp(f1->param, f2->param) != 0)
		return 1;
	return 0;
}

// search a field-list for one ID
struct TypeStructList* find_field(struct TypeStructList* field, char* name)
{
	while (field)
	{
		if (strcmp(name, field->name) == 0) //found
			return field;
		field = field->next;
	}
	return  NULL; //not found
}

// create a field-list from current scope
struct TypeStructList* create_fieldlist_from_cur_scope()
{
	struct SymbolNode* i = cur_scope()->down;
	struct TypeStructList *p = NULL, *q;
	while (i)
	{
		q = new_fieldlist_link(i->type, i->name, p);
		p = q;
		i = i->down;
	}
	return p;
}

// OVERALL
void init_symboltable()
{
	int i;
	sslhdr = new_sstackheader();
	for (i = 0; i < HASH_LENGTH; i++)
		shash[i].left = shash[i].right = NULL;
	printf("symbol table initializes complete!\n");
}

//semantic

struct TypeNode* TypeNode_int;
struct TypeNode* TypeNode_error;

int layer = 0;

// defining var/fun */
#define semantic_error(errno) \
	( \
		print_error((errno),cur->lineno,(errno##_MSG)), \
		cur->vtype = TypeNode_error \
	) \

#define define_new_var(name,type,errno,errmsg) \
	if (find_SymbolNode_cur_scope((name),SK_VAR)) \
	{ \
		print_error((errno),cur->lineno,(errmsg)); \
		cur->vtype = TypeNode_error; \
	} \
	else \
		add_SymbolNode((name), SK_VAR, (type))

#define Exp_Exp_Op_Exp \
	sn_parse(child1(cur), cur); \
	sn_parse(child3(cur), cur); \
	if (child1(cur)->vtype->kind > LAST_BASIC_TYPE || \
		child3(cur)->vtype->kind > LAST_BASIC_TYPE ||\
		child3(cur)->vtype->kind != child1(cur)->vtype->kind) \
	{ \
		semantic_error(ERROR_TYPE_UNMATCH); \
		break; \
	} \
	else \
		(cur->lvalue = LV_RVALUE,  \
		cur->vtype = child1(cur)->vtype);
		
		
int arraylist[100000];		
// semantic analysis		
void sn_parse(struct TreeNode* cur, struct TreeNode* p)
{
	//printf("%s [%d]\n", cur->type_name, cur->reduce_type);
	struct SymbolNode* n;
	struct TypeStructList* f;
	int j = 0;
	layer++;
	switch (cur->reduce_type)
	{
		case RT_Program_ExtDefS:
			new_scope(); 
			// define READ and WRITE functions
			add_SymbolNode("read" , SK_VAR, new_TypeNode_func(new_func(TypeNode_int, new_fieldlist_link(TypeNode_int, "x", NULL), FUNC_DEFINED)));
			add_SymbolNode("write" , SK_VAR, new_TypeNode_func(new_func(TypeNode_int, new_fieldlist_link(TypeNode_int, "x", NULL), FUNC_DEFINED)));
			sn_parse(child1(cur), cur);
			// check functions that declared only
			n = cur_scope();
			while (n = n->down)
				if (n->kind == SK_VAR && n->type->kind == TK_FUNC && n->type->function->defined == FUNC_UNDEFINED)
				{	
					printf("Error: Function '%s' cannot be resolved\n", n->name);
					set_error_status();
				}
			del_current_scope();
			break;
		case RT_ExtDefS_ExtDef_ExtDefS:
			sn_parse(child1(cur), cur);
			sn_parse(child2(cur), cur);
			break;
		case RT_ExtDefS_NULL:
			break;
		case RT_ExtDef_Spec_ExtVarS_SEMI:
			sn_parse(child1(cur), cur);
			cur->vtype = child1(cur)->vtype;
			sn_parse(child2(cur), cur);
			break;
		case RT_ExtDef_Spec_SEMI:
			sn_parse(child1(cur), cur);
			cur->vtype = child1(cur)->vtype;
			break;
		case RT_ExtDef_Spec_Func_StmtBlock:
			new_scope();
			sn_parse(child1(cur), cur);
			cur->return_type = child1(cur)->vtype;
			sn_parse(child2(cur), cur);
			if (child2(cur)->name != NULL && child2(cur)->vtype != TypeNode_error &&
						child1(cur)->vtype != TypeNode_error)
				if (n = find_SymbolNode(child2(cur)->name, SK_VAR)) // repeat defined
				{
					if (n->type->function->defined)
						semantic_error(ERROR_FUNC_MULTIDEFINED);
					else if (typecmp(n->type->function->return_type, child1(cur)->return_type) != 0 ||
						fieldcmp(n->type->function->param, child2(cur)->field) != 0)
					{
						//delete the old wrong declaration and create a new one
						del_func(n->type->function);
						add_SymbolNode_parent(child2(cur)->name, SK_VAR, 
							new_TypeNode_func(new_func(child1(cur)->return_type, child2(cur)->field, FUNC_DEFINED)));
						semantic_error(ERROR_FUNC_DEC_DEF_MISMATCH);
					}
					else
						n->type->function->defined = FUNC_DEFINED;
				}
				else
				// build up the SymbolNode
					add_SymbolNode_parent(child2(cur)->name, SK_VAR, 
						new_TypeNode_func(new_func(child1(cur)->return_type, child2(cur)->field, FUNC_DEFINED)));
			sn_parse(child3(cur), cur);
			del_current_scope();// function is over
			
			break;
		case RT_ExtDef_Spec_Func_SEMI:
			new_scope();
			sn_parse(child1(cur), cur);
			cur->return_type = child1(cur)->vtype;
			sn_parse(child2(cur), cur);
			if (child2(cur)->name != NULL && child2(cur)->vtype != TypeNode_error &&
						child1(cur)->vtype != TypeNode_error)
				if (n = find_SymbolNode(child2(cur)->name, SK_VAR)) 
				{
					if (typecmp(n->type->function->return_type, child1(cur)->return_type) != 0 ||
						fieldcmp(n->type->function->param, child2(cur)->field) != 0)
						semantic_error(ERROR_FUNC_DEC_DEF_MISMATCH);
					else
						n->type->function->defined = FUNC_DEFINED;
				}
				else
				// build up the SymbolNode
					add_SymbolNode_parent(child2(cur)->name, SK_VAR, 
						new_TypeNode_func(new_func(child1(cur)->return_type, child2(cur)->field, FUNC_UNDEFINED)));
			del_current_scope(); // function is over

			break;
		case RT_ExtVarS_Dec:
			cur->struct_defined = p->struct_defined;
			cur->vtype = p->vtype;
			sn_parse(child1(cur), cur);
			cur->name = child1(cur)->name;
			if (child1(cur)->vtype != TypeNode_error)
				if (find_SymbolNode_cur_scope(cur->name, SK_VAR) != NULL)
				{
					if (cur->struct_defined)
						semantic_error(ERROR_INVALID_DEF_IN_STRUCT);
					else
						semantic_error(ERROR_VAR_MULTIDEFINED);
				}
				else 
					add_SymbolNode(cur->name, SK_VAR, child1(cur)->vtype);
			break;
		case RT_ExtVarS_Dec_COMMA_ExtVarS:
			cur->struct_defined = p->struct_defined;
			cur->vtype = p->vtype;
			sn_parse(child1(cur), cur);
			cur->name = child1(cur)->name;
			if (child1(cur)->vtype->kind != TK_ERROR)
				if (find_SymbolNode_cur_scope(cur->name, SK_VAR))
				{
					if (cur->struct_defined)
						semantic_error(ERROR_INVALID_DEF_IN_STRUCT);
					else
						semantic_error(ERROR_VAR_MULTIDEFINED);
				}
				else
					add_SymbolNode(cur->name, SK_VAR, child1(cur)->vtype);
			sn_parse(child3(cur), cur);
			break;
		case RT_Spec_TYPE:
			if (strcmp(child1(cur)->val.val_str, "int") == 0)
				cur->vtype = TypeNode_int;
			else {}
			break;
		case RT_Spec_StSpec:
			sn_parse(child1(cur), cur);
			cur->vtype = child1(cur)->vtype; 
	
			break;
		case RT_StSpec_STRUCT_OptTag_LC_DefS_RC:
			sn_parse(child2(cur), cur);
			new_scope();
			cur->struct_defined = 1; 
			sn_parse(child4(cur), cur);
			f = create_fieldlist_from_cur_scope();

			del_current_scope(); // function is over
			cur->vtype = new_TypeNode_struct(f);
			if (child2(cur)->name != NULL) 
			{
				if (find_SymbolNode_cur_scope(child2(cur)->name, SK_DEF))
				{
					print_error(ERROR_STRUCT_MULTIDEFINED, cur->lineno, "Struct name conflicts");
					cur->vtype = TypeNode_error;
				}
				else
					add_SymbolNode(child2(cur)->name, SK_DEF, cur->vtype);
			}

			break;
		case RT_StSpec_STRUCT_Tag:
			sn_parse(child2(cur), cur);
			if (n = find_SymbolNode(child2(cur)->name, SK_DEF))
				cur->vtype = n->type;
			else //error
			{
				print_error(ERROR_STRUCT_UNDEFINED, cur->lineno, "Struct undefined");
				cur->vtype = TypeNode_error;
			}

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
			sn_parse(child1(cur), cur);
			cur->name = child1(cur)->name;
			cur->vtype = child1(cur)->vtype;

			break;
		case RT_Func_ID_LP_ParaS_RP:
			cur->name = child1(cur)->val.val_str;
			sn_parse(child3(cur), cur);
			cur->field = child3(cur)->field;
			f = cur->field;

			break;
		case RT_Func_ID_LP_RP:
			cur->name = child1(cur)->val.val_str;
			cur->field = NULL;
			break;
		case RT_ParaS_Para_COMMA_ParaS:
			sn_parse(child1(cur), cur);
			sn_parse(child3(cur), cur);
			cur->field = new_fieldlist_link(child1(cur)->vtype, child1(cur)->name, child3(cur)->field);
			
			break;
		case RT_ParaS_Para:
			sn_parse(child1(cur), cur);
			if (child1(cur)->vtype->kind != TK_ERROR)
				cur->field = new_fieldlist_link(child1(cur)->vtype, child1(cur)->name, NULL);
			else
				cur->vtype = TypeNode_error;
			break;
		case RT_Para_Spec_Var:
			sn_parse(child1(cur), cur);
			cur->vtype = child1(cur)->vtype;
			sn_parse(child2(cur), cur);
			cur->name = child2(cur)->name;
			cur->vtype = child2(cur)->vtype;
			if (find_SymbolNode_cur_scope(cur->name, SK_VAR))
			{
				print_error(ERROR_VAR_MULTIDEFINED, cur->lineno, "Variable multidefined");
				cur->vtype = TypeNode_error;
			}
			else
				add_SymbolNode(cur->name, SK_VAR, cur->vtype);

			break;
		case RT_StmtBlock_LC_DefS_StmtS_RC:
			cur->return_type = p->return_type; 
			if (p->reduce_type != RT_ExtDef_Spec_Func_StmtBlock) 
				new_scope(); 
			sn_parse(child2(cur), cur);
			sn_parse(child3(cur), cur);
			if (p->reduce_type != RT_ExtDef_Spec_Func_StmtBlock)
				del_current_scope(); 

			break;
		case RT_StmtS_Stmt_StmtS:
			cur->return_type = p->return_type; 
			sn_parse(child1(cur), cur);
			sn_parse(child2(cur), cur);
			break;
		case RT_StmtS_NULL:
			break;
		case RT_Stmt_Exp_SEMI:
			cur->return_type = p->return_type;
			sn_parse(child1(cur), cur);
			break;
		case RT_Stmt_StmtBlock:
			cur->return_type = p->return_type;
			sn_parse(child1(cur), cur);
			break;
		case RT_Stmt_RETURN_Exp_SEMI:
			cur->return_type = p->return_type;
			sn_parse(child2(cur), cur);
			if (typecmp(cur->return_type, child2(cur)->vtype)) 
				semantic_error(ERROR_RETURN_TYPE_UNMATCH);
			break;
		case RT_Stmt_IF_LP_Exp_RP_Stmt:
			cur->return_type = p->return_type;
			sn_parse(child3(cur), cur);
			sn_parse(child5(cur), cur);
			if (typecmp(TypeNode_int, child3(cur)->vtype))
				semantic_error(ERROR_COND_WRONG_TYPE);
			break;
		case RT_Stmt_IF_LP_Exp_RP_Stmt_ELSE_Stmt:
			cur->return_type = p->return_type;
			sn_parse(child3(cur), cur);
			sn_parse(child5(cur), cur);
			sn_parse(child7(cur), cur);
			
			if (typecmp(TypeNode_int, child3(cur)->vtype))
				semantic_error(ERROR_COND_WRONG_TYPE);
			break;
		case RT_Stmt_FOR_LP_Exp_SEMI_Exp_SEMI_Exp_RP_Stmt:
			cur->return_type = p->return_type;
			sn_parse(child3(cur), cur);
			sn_parse(child5(cur), cur);
			sn_parse(child7(cur), cur);
			sn_parse(child9(cur), cur);
			if (typecmp(TypeNode_int, child3(cur)->vtype)||typecmp(TypeNode_int, child5(cur)->vtype))
				semantic_error(ERROR_COND_WRONG_TYPE);
			break;
		case RT_Stmt_FOR_LP_SEMI_Exp_SEMI_Exp_RP_Stmt:
			cur->return_type = p->return_type;
			sn_parse(child4(cur), cur);
			sn_parse(child6(cur), cur);
			sn_parse(child8(cur), cur);
			if (typecmp(TypeNode_int, child4(cur)->vtype))
				semantic_error(ERROR_COND_WRONG_TYPE);
			break;
		case RT_Stmt_WHILE_LP_Exp_RP_Stmt:
			cur->return_type = p->return_type;
			sn_parse(child3(cur), cur);
			sn_parse(child5(cur), cur);
			if (typecmp(TypeNode_int, child3(cur)->vtype)) 
				semantic_error(ERROR_COND_WRONG_TYPE);
			break;
		case RT_Stmt_BREAK_SEMI:
			cur->return_type = p->return_type;
			break;
		case RT_Stmt_CONT_SEMI:
			cur->return_type = p->return_type;
			break;
		case RT_DefS_Def_DefS:
			cur->struct_defined = p->struct_defined;
			sn_parse(child1(cur), cur);
			sn_parse(child2(cur), cur);
			break;
		case RT_DefS_NULL:
			break;
		case RT_Def_Spec_DecS_SEMI:
			cur->struct_defined = p->struct_defined;
			sn_parse(child1(cur), cur);
			cur->vtype = child1(cur)->vtype;
			sn_parse(child2(cur), cur);
			break;
		case RT_DecS_Dec:
			cur->struct_defined = p->struct_defined;
			cur->vtype = p->vtype;
			sn_parse(child1(cur), cur);
			cur->name = child1(cur)->name;
			if (child1(cur)->vtype != TypeNode_error)
				if (find_SymbolNode_cur_scope(cur->name, SK_VAR) != NULL)
				{
					if (cur->struct_defined)
						semantic_error(ERROR_INVALID_DEF_IN_STRUCT);
					else
						semantic_error(ERROR_VAR_MULTIDEFINED);
				}
				else 
					add_SymbolNode(cur->name, SK_VAR, child1(cur)->vtype);
			break;
		case RT_DecS_Dec_COMMA_DecS:
			cur->struct_defined = p->struct_defined;
			cur->vtype = p->vtype;
			sn_parse(child1(cur), cur);
			cur->name = child1(cur)->name;
			if (child1(cur)->vtype->kind != TK_ERROR)
				if (find_SymbolNode_cur_scope(cur->name, SK_VAR))
				{
					if (cur->struct_defined)
						semantic_error(ERROR_INVALID_DEF_IN_STRUCT);
					else
						semantic_error(ERROR_VAR_MULTIDEFINED);
				}
				else
					add_SymbolNode(cur->name, SK_VAR, child1(cur)->vtype);
			sn_parse(child3(cur), cur);

			break;
		case RT_Dec_Var:
			cur->vtype = p->vtype;
			sn_parse(child1(cur), cur);
			cur->vtype = child1(cur)->vtype;
			cur->name = child1(cur)->name;
			break;
		case RT_Dec_Var_ASSIGN_Exp:
			cur->struct_defined = p->struct_defined;
			if (cur->struct_defined) 
			{
				semantic_error(ERROR_INVALID_DEF_IN_STRUCT);
				break;
			}
			cur->vtype = p->vtype;
			sn_parse(child1(cur), cur);
			cur->vtype = child1(cur)->vtype;
			cur->name = child1(cur)->name;
			sn_parse(child3(cur), cur);
			if (typecmp(cur->vtype, child3(cur)->vtype))
				semantic_error(ERROR_ASSIGN_TYPE_UNMATCH);
			break;
		case RT_Dec_Var_ASSIGN_LC_Args_RC:
			cur->vtype = p->vtype;
			sn_parse(child1(cur), cur);
			cur->vtype = child1(cur)->vtype;
			cur->name = child1(cur)->name;
			cur->arraydec = 1; //array not function parameter
			sn_parse(child4(cur), cur);
			break;
		case RT_Exp_Exp_ASSIGN_Exp:
			sn_parse(child1(cur), cur);
			sn_parse(child3(cur), cur);
			if (child1(cur)->vtype->kind == TK_ERROR ||
				child3(cur)->vtype->kind == TK_ERROR)
			{
				cur->vtype = TypeNode_error;
				break;
			}

			if (!(child1(cur)->lvalue & LV_LVALUE)) 
				semantic_error(ERROR_RVALUE_BEFORE_ASSIGN);
			if (typecmp(child1(cur)->vtype, child3(cur)->vtype))
				semantic_error(ERROR_ASSIGN_TYPE_UNMATCH);
			if (cur->vtype != TypeNode_error)
			{
				cur->lvalue = child1(cur)->lvalue;
				cur->vtype = child1(cur)->vtype;
			}
			break;
		case RT_Exp_Exp_MULT_Exp:
			Exp_Exp_Op_Exp
			break;
		case RT_Exp_Exp_DIV_Exp:
			Exp_Exp_Op_Exp
			break;
		case RT_Exp_Exp_MOD_Exp:
			Exp_Exp_Op_Exp
			break;
		case RT_Exp_Exp_ADD_Exp:
			Exp_Exp_Op_Exp
			break;
		case RT_Exp_Exp_MINUS_Exp:
			Exp_Exp_Op_Exp
			break;
		case RT_Exp_Exp_MULTASSIGN_Exp:
			Exp_Exp_Op_Exp
			break;
		case RT_Exp_Exp_DIVASSIGN_Exp:
			Exp_Exp_Op_Exp
			break;
		case RT_Exp_Exp_ADDASSIGN_Exp:
			Exp_Exp_Op_Exp
			break;
		case RT_Exp_Exp_MINUSASSIGN_Exp:
			Exp_Exp_Op_Exp
			break;
		case RT_Exp_Exp_RELOP_Exp:
			Exp_Exp_Op_Exp
			break;
		case RT_Exp_Exp_LSHIFT_Exp:
			Exp_Exp_Op_Exp
			break;
		case RT_Exp_Exp_RSHIFT_Exp:
			Exp_Exp_Op_Exp
			break;
		case RT_Exp_Exp_LSHIFTASSIGN_Exp:
			Exp_Exp_Op_Exp
			break;
		case RT_Exp_Exp_RSHIFTASSIGN_Exp:
			Exp_Exp_Op_Exp
			break;
		case RT_Exp_Exp_LOGIAND_Exp:
			Exp_Exp_Op_Exp
			break;
		case RT_Exp_Exp_LOGIOR_Exp:
			Exp_Exp_Op_Exp
			break;
		case RT_Exp_Exp_BITAND_Exp:
			Exp_Exp_Op_Exp
			break;
		case RT_Exp_Exp_BITOR_Exp:
			Exp_Exp_Op_Exp
			break;
		case RT_Exp_Exp_BITXOR_Exp:
			Exp_Exp_Op_Exp
			break;
		case RT_Exp_Exp_BITANDASSIGN_Exp:
			Exp_Exp_Op_Exp
			break;
		case RT_Exp_Exp_BITORASSIGN_Exp:
			Exp_Exp_Op_Exp
			break;
		case RT_Exp_Exp_BITXORASSIGN_Exp:
			Exp_Exp_Op_Exp
			break;
		case RT_Exp_MINUS_Exp:
			sn_parse(child2(cur), cur);
			cur->lvalue = LV_RVALUE;
			cur->vtype = child2(cur)->vtype;
			break;
		case RT_Exp_INCR_Exp:
			sn_parse(child2(cur), cur);
			cur->lvalue = LV_RVALUE;
			cur->vtype = child2(cur)->vtype;
			break;
		case RT_Exp_DECR_Exp:
			sn_parse(child2(cur), cur);
			cur->lvalue = LV_RVALUE;
			cur->vtype = child2(cur)->vtype;
			break;
		case RT_Exp_LOGINOT_Exp:
			sn_parse(child2(cur), cur);
			cur->lvalue = LV_RVALUE;
			cur->vtype = child2(cur)->vtype;
			break;
		case RT_Exp_BITNOT_Exp:
			sn_parse(child2(cur), cur);
			cur->lvalue = LV_RVALUE;
			cur->vtype = child2(cur)->vtype;
			break;
		case RT_Exp_LP_Exp_RP:
			sn_parse(child2(cur), cur);
			cur->lvalue = child2(cur)->lvalue;
			cur->vtype = child2(cur)->vtype;
			break;
		case RT_Exp_ID_LP_Args_RP:
			cur->name = child1(cur)->val.val_str;
			if (!(n = find_SymbolNode(cur->name, SK_VAR)))
				semantic_error(ERROR_FUNC_UNDEFINED);
			else if (n->type->kind != TK_FUNC)
				semantic_error(ERROR_USE_VAR_AS_FUNC);
			else
			{
				cur->field = n->type->function->param;
				sn_parse(child3(cur), cur);
				if (child3(cur)->vtype->kind == TK_ERROR)
					semantic_error(ERROR_FUNC_ARG_UNMATCH);
				else
				{
					cur->lvalue = LV_RVALUE;
					cur->vtype = n->type->function->return_type;
				}
			}
			break;
		case RT_Exp_ID_LP_RP:
			cur->name = child1(cur)->val.val_str;
			if (!(n = find_SymbolNode(cur->name, SK_VAR)))
				semantic_error(ERROR_FUNC_UNDEFINED);
			else if (n->type->kind != TK_FUNC)
				semantic_error(ERROR_USE_VAR_AS_FUNC);
			else
			{
				cur->lvalue = LV_RVALUE;
				cur->vtype = n->type->function->return_type;
			}
			break;
		case RT_Exp_Exp_LB_Exp_RB:
			sn_parse(child1(cur), cur);
			sn_parse(child3(cur), cur);
			cur->vtype = child1(cur)->vtype;
			if (cur->vtype->kind != TK_ERROR)
				if (cur->vtype->kind != TK_ARRAY)
					semantic_error(ERROR_ARRAY_TYPE_REQUIRED);
				else if (child3(cur)->vtype->kind != TK_INT)
					semantic_error(ERROR_INVALID_ARRAY_INDEX);
				else
				{
					cur->vtype = child1(cur)->vtype->array.type; 
					if (cur->vtype->kind == TK_ARRAY)
						cur->lvalue = LV_RVALUE;
					else
						cur->lvalue = LV_LVALUE;
				}
			break;
		case RT_Exp_Exp_DOT_ID:
			sn_parse(child1(cur), cur);
			cur->vtype = child1(cur)->vtype;
			if (cur->vtype->kind != TK_ERROR) 
				if (cur->vtype->kind != TK_STRUCT)
					semantic_error(ERROR_USE_VAR_AS_STRUCT);
				if ((f = find_field(cur->vtype->field_list, child3(cur)->val.val_str)) == NULL)
					semantic_error(ERROR_INVALID_STRUCT_SCOPE);
				else
				{
					cur->vtype = f->type;
					cur->lvalue = (cur->vtype->kind == TK_ARRAY) ? LV_RVALUE : LV_LVALUE;
				}
			break;
		case RT_Exp_ID:
			cur->name = child1(cur)->val.val_str;
			cur->lvalue = LV_LVALUE; 
			n = find_SymbolNode(cur->name, SK_VAR);
			if(n != NULL)
				cur->vtype = n->type;
			else
				semantic_error(ERROR_VAR_UNDEFINED);
			break;
		case RT_Exp_INT:
			cur->vtype = TypeNode_int;
			cur->lvalue = LV_RVALUE;
			break;
		case RT_Exp_NULL:
			break;
		case RT_Args_Exp_COMMA_Args:
			sn_parse(child1(cur), cur);
			cur->field = p->field; 
			cur->arraydec = p->arraydec;
			if(cur->arraydec != 1)
			{
				if (cur->field == NULL)
					semantic_error(ERROR_FUNC_ARG_UNMATCH);
				else if (typecmp(cur->field->type, child1(cur)->vtype))
					semantic_error(ERROR_FUNC_ARG_UNMATCH);
				else
				{
					cur->field = cur->field->next;
					sn_parse(child3(cur), cur);
					cur->field = p->field; 
				}
			}
			break;
		case RT_Args_Exp:
			sn_parse(child1(cur), cur);
			cur->field = p->field; 
			cur->arraydec = p->arraydec;
			if(cur->arraydec != 1)
			{
				if (cur->field == NULL)
					semantic_error(ERROR_FUNC_ARG_UNMATCH);
				if (cur->field->next != NULL)
					semantic_error(ERROR_FUNC_ARG_UNMATCH);
				else if (typecmp(cur->field->type, child1(cur)->vtype))
					semantic_error(ERROR_FUNC_ARG_UNMATCH);
			}
			break;
	}
	layer--;
}

int semantic_parse(struct TreeNode* treeroot)
{
	TypeNode_int = new_TypeNode_basic(TK_INT);
	TypeNode_error = new_TypeNode(TK_ERROR);
	sn_parse(treeroot, NULL);
}

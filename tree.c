#include "smallc.tab.h"
#include "def.h"

struct TreeNode* tree_new_node(char* type_name, int lineno, union TreeNode_val val, enum NodeType type, enum ReduceType reduce_type)
{
	struct TreeNode* node = (struct TreeNode*)malloc(sizeof(struct TreeNode));

	node->type = type;
	node->type_name = type_name;
	node->val = val;
	node->lineno = lineno;
	node->reduce_type = reduce_type;
	node->sibling = node->child = NULL;
	node->vtype = NULL;
	node->lvalue = LV_UNKNOWN;
	node->struct_defined = 0;
	return node;
}

struct TreeNode* tree_append_sibling(struct TreeNode* prev_sibling, struct TreeNode* cur_node)
{
	prev_sibling->sibling = cur_node;
	return cur_node;
}

struct TreeNode* tree_append_child(struct TreeNode* parent, struct TreeNode* cur_node)
{
	parent->child = cur_node;
	return cur_node;
}

void tree_display(struct TreeNode* p, int level, FILE * fout)
{
	int i;
	for (i = 0; i < level * 2; i++) 
	{
		fprintf(fout, " ");
		printf(" ");
	}
	fprintf(fout, "%s [%d]", p->type_name, p->reduce_type);
	printf("%s [%d]", p->type_name, p->reduce_type);
	
	/* otherwise, maybe we should have some details! */
	if (p->type == NT_ID) 
	{
		fprintf(fout, ": %s\n", p->val.val_str);
		printf(": %s\n", p->val.val_str);
	}
	else if (p->type == NT_INT)
	{
		fprintf(fout, ": %d\n", p->val.val_int);
		printf(": %d\n", p->val.val_int);
	}
	else if (p->type == NT_TYPE)
	{
		fprintf(fout, ": %s\n", p->val.val_str);
		printf(": %s\n", p->val.val_str);
	}
	else
	{
		fprintf(fout, "\n");
		printf("\n");
	}

	if (p->child != NULL)
	{
		tree_display(p->child, level + 1, fout);
	}
	/* print siblings! */
	if (p->sibling != NULL)
		tree_display(p->sibling, level, fout);
}

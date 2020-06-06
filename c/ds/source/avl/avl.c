/****************************									   
* Author: OL78				*									   
* Last Update: 22/12/2019	*									   
* Data structure: AVL       *
* Author: David Berman      *
* Code File				    *					   
****************************/

#include <stdlib.h>
#include <assert.h>

#include "avl_node.h"
#include "avl.h"

/******************************************************************************/
/*------------------------- Rotation Function typdef -------------------------*/

typedef avl_node_t *(*rotate_func)(avl_node_t *);

/******************************************************************************/
/*------------------------- enum definitions ---------------------------------*/

typedef enum{INSERTED, NOT_INSERTED}e_insert_flag;
typedef enum{FALSE_CASE, TRUE_CASE}e_cases;
typedef enum{SUCCESS, FAILURE}e_result;
typedef enum{RIGHT_TREE_HEAVY = -2, LEFT_TREE_HEAVY = 2}e_inbalance;
typedef enum {INITIATED, NOT_INITIATED}e_init; 

/******************************************************************************/
/*------------------------- Struct definitions -------------------------------*/

struct avl
{
	avl_node_t *root;
	int (*cmp_func)(const void *, const void *, void *);
	void *param;
};

/******************************************************************************/

typedef struct find_if_wrapper
{
	int (*is_match)(void *, void *);
	void *param;
	dlist_t *list;
}find_if_wrap_t;

/******************************************************************************/

typedef struct remove_if_wrapper
{
	int (*is_match)(void *, void *);
	void *param;
	avl_t *avl;
}remove_if_wrap_t;

/******************************************************************************/

struct avl_node
{
	void *data;
	avl_node_t *children[NUM_OF_SIDES];
	size_t height; 
};


/******************************************************************************/
/*------------------------- Auxilary functions -------------------------------*/

static avl_node_t *AVLInsertIMP(avl_t *avl, avl_node_t *root, avl_node_t *new_node);
static void AVLDestroyIMP(avl_node_t *avl_node);
static e_sides AVLSideAccordingToCmpFuncIMP(const avl_t *avl, const void *data1, const void *data2);
static avl_node_t *AVLBalanceIMP(avl_node_t *root);
static void AVLUpdateHeightIMP(avl_node_t *root);
static size_t AVLSizeIMP(avl_node_t *root);
static void *AVLFindIMP(const avl_t *avl, avl_node_t *root, const void *data);
int AVLForEachIMP(avl_node_t *root,
				 int (*op_func)(void *data, void *param), void *param);
static avl_node_t *AVLRemoveIMP(avl_t *avl, avl_node_t *root, const void *data);
static avl_node_t *AVLMoveAsFarLeftAsPossibleIMP(avl_node_t *root, avl_node_t *child);
static avl_node_t *AVLRemoveNodeIMP(avl_node_t *to_be_removed);
static avl_node_t *AVLRemoveNodeWithOneChildIMP(avl_node_t *root, e_sides side);
static avl_node_t *AVLRemoveNodeWithTwoChildrenIMP(avl_node_t *root);
static void AVLSwapDataIMP(avl_node_t *avl_node1, avl_node_t *avl_node2);
static avl_node_t *AVLBalanceNodeAndSubTreeIMP(avl_node_t *root, e_sides side_of_heavy_root_subtree);
static int AVLGetBalanceFactorIMP(avl_node_t *root);
static void InitRotationsLutIMP(rotate_func lut_rotations[NUM_OF_SIDES][NUM_OF_SIDES]);
static e_sides AVLWhichSideIsHeavierIMP(long balance_factor);
static avl_node_t *LeftLeftRotationIMP(avl_node_t *root);
static avl_node_t *LeftRightRotationIMP(avl_node_t *root);
static avl_node_t *RightLeftRotationIMP(avl_node_t *root);
static avl_node_t *RightRightRotationIMP(avl_node_t *root);
long MaxHeightIMP(long height1, long height2);
static long AVLHeightIMP(const avl_t *avl);
static int IsMatchWrapperForFindIfIMP(void *data, void *param);
static int IsMatchWrapperForRemoveIfIMP(void *data, void *param);

/******************************************************************************/
/*------------------------- AVLCreate function -------------------------------*/

/* Creates and avl, returns pointer to avl if succedes and NULL if fails */
avl_t *AVLCreate(int (*cmp_func)(const void *data1, const void *data2, 
		void *param), void *param)
{
	avl_t *new_avl = NULL;
	
	assert(NULL != cmp_func);
	
	new_avl = malloc(sizeof(*new_avl));
	if (NULL != new_avl)
	{
		new_avl->root = NULL;
		new_avl->cmp_func = cmp_func;
		new_avl->param = param;
	}
	
	return new_avl;
}
		
/******************************************************************************/
/*------------------------- AVLDestroy function ------------------------------*/

/* Destroys struct avl, freeing data is user's responsibility */
/* post order */
void AVLDestroy(avl_t *avl)
{
	assert(NULL != avl);
	
	AVLDestroyIMP(avl->root	);
	free(avl);
}

/******************************************************************************/
/*------------------------- AVLDestroyIMP function ---------------------------*/

/* Destroys struct avl, freeing data is user's responsibility */
/* post order */
static void AVLDestroyIMP(avl_node_t *avl_node)
{
	if (NULL == avl_node)
	{
		return;
	}
	
	AVLDestroyIMP(AVLNodeGetSon(avl_node, LEFT));
	AVLDestroyIMP(AVLNodeGetSon(avl_node, RIGHT));
	AVLNodeDestroy(avl_node);
}

/******************************************************************************/
/*------------------------- AVLInsert function -------------------------------*/

/* O(log n) */
int AVLInsert(avl_t *avl, void *data)
{
	avl_node_t *new_node = NULL;
	avl_node_t *root = NULL; 
	
	assert(NULL != avl);
	
	root = avl->root;
	
	new_node = AVLNodeCreate(data, NULL, NULL);
	if (NULL != new_node)
	{
		avl->root = AVLInsertIMP(avl, root, new_node);
		return INSERTED;  	
	}
	
	return NOT_INSERTED;		
}

/******************************************************************************/
/*------------------------- AVLInsertIMP function ----------------------------*/

static avl_node_t *AVLInsertIMP(avl_t *avl, avl_node_t *root, avl_node_t *new_node)
{
	e_sides side = LEFT;
	
	assert(NULL != avl);
	assert(NULL != new_node);
	
	if (NULL == root)
	{
		return new_node;
	}
	
	side = AVLSideAccordingToCmpFuncIMP(avl, new_node->data, root->data);  
	root->children[side] = AVLInsertIMP(avl, root->children[side], new_node);
	root = AVLBalanceIMP(root);
	AVLUpdateHeightIMP(root);
	
	return root;  
}

/******************************************************************************/
/*------------------------- AVLBalanceIMP function ---------------------------*/

static avl_node_t *AVLBalanceIMP(avl_node_t *root)
{
	long balance_factor = 0;
	
	assert(NULL != root);
	
	balance_factor = AVLGetBalanceFactorIMP(root);
	
	if (RIGHT_TREE_HEAVY == balance_factor || LEFT_TREE_HEAVY == balance_factor)
	{
		return AVLBalanceNodeAndSubTreeIMP(root, AVLWhichSideIsHeavierIMP(balance_factor));
	}
	
	return root;
}

/******************************************************************************/
/*------------------------- AVLGetBalanceFactorIMP function ------------------*/

static int AVLGetBalanceFactorIMP(avl_node_t *avl_node)
{
	avl_node_t *right_child = NULL;
	avl_node_t *left_child = NULL;
	long balance_factor = 0;
	
	assert(NULL != avl_node);
	
	left_child = AVLNodeGetSon(avl_node, LEFT);
	right_child = AVLNodeGetSon(avl_node, RIGHT);
	balance_factor = AVLNodeGetHeight(left_child) - AVLNodeGetHeight(right_child);
	
	return balance_factor; 
}

/******************************************************************************/
/*------------------------- AVLWhichSideIsHeavierIMP function ----------------*/

static e_sides AVLWhichSideIsHeavierIMP(long balance_factor)
{
	return balance_factor > 0 ? LEFT : RIGHT;
}

/******************************************************************************/
/*------------------------- AVLBalanceSubTreeIMP function --------------------*/

static avl_node_t *AVLBalanceNodeAndSubTreeIMP(avl_node_t *root, e_sides side_of_heavy_root_subtree)
{
	static rotate_func lut_rotations[NUM_OF_SIDES][NUM_OF_SIDES] = {0};
	long child_balance_factor = 0;
	e_sides side_of_heavy_child_subtree = 0;
	rotate_func rotate_func = NULL; 
	
	InitRotationsLutIMP(lut_rotations);
	
	assert(NULL != root);
	
	child_balance_factor = AVLGetBalanceFactorIMP(root->children[side_of_heavy_root_subtree]);
	side_of_heavy_child_subtree = (0 == child_balance_factor) ? side_of_heavy_root_subtree : AVLWhichSideIsHeavierIMP(child_balance_factor);
	rotate_func = lut_rotations[side_of_heavy_root_subtree][side_of_heavy_child_subtree];
	return rotate_func(root);
}

/******************************************************************************/
/*------------------------- InitRotationsLutIMP function ---------------------*/

static void InitRotationsLutIMP(rotate_func lut_rotations[NUM_OF_SIDES][NUM_OF_SIDES])
{	
	static e_init init_flag = NOT_INITIATED;
	
	assert(NULL != lut_rotations);
	
	if (NOT_INITIATED == init_flag)
	{
		lut_rotations[LEFT][LEFT] = &LeftLeftRotationIMP;
		lut_rotations[LEFT][RIGHT] = &LeftRightRotationIMP;
		lut_rotations[RIGHT][LEFT] = &RightLeftRotationIMP; 
		lut_rotations[RIGHT][RIGHT] = &RightRightRotationIMP;
		
		init_flag = INITIATED;
	}
}

/******************************************************************************/
/*--------------------------LeftLeftRotationIMP function ---------------------*/

static avl_node_t *LeftLeftRotationIMP(avl_node_t *root)
{
	avl_node_t *left_child = NULL;
	avl_node_t *right_grandchild = NULL;
	
	assert(NULL != root);
	
	left_child = AVLNodeGetSon(root, LEFT);
	right_grandchild = AVLNodeGetSon(left_child, RIGHT);
	
	root->children[LEFT] = right_grandchild;
	left_child->children[RIGHT] = root;
	AVLUpdateHeightIMP(root);
	AVLUpdateHeightIMP(left_child);
	
	return left_child; 	
}

/******************************************************************************/
/*--------------------------LeftRightRotationIMP function --------------------*/

static avl_node_t *LeftRightRotationIMP(avl_node_t *root)
{
	assert(NULL != root);
	
	root->children[LEFT] = LeftLeftRotationIMP(AVLNodeGetSon(root, LEFT));
	
	return LeftLeftRotationIMP(root);
}

/******************************************************************************/
/*--------------------------RightLeftRotationIMP function --------------------*/

static avl_node_t *RightLeftRotationIMP(avl_node_t *root)
{	
	assert(NULL != root);
	
	root->children[RIGHT] = RightRightRotationIMP(AVLNodeGetSon(root, RIGHT));
	
	return RightRightRotationIMP(root);
}

/******************************************************************************/
/*--------------------------RightRightRotationIMP function -------------------*/

static avl_node_t *RightRightRotationIMP(avl_node_t *root)
{
	avl_node_t *right_child = NULL;
	avl_node_t *left_grandchild = NULL;
	
	assert(NULL != root);
	
	right_child = AVLNodeGetSon(root, RIGHT);
	left_grandchild = AVLNodeGetSon(right_child, LEFT);
	
	root->children[RIGHT] = left_grandchild;
	right_child->children[LEFT] = root;
	AVLUpdateHeightIMP(root);
	AVLUpdateHeightIMP(right_child);
	
	return right_child; 	
}

/******************************************************************************/
/*------------------------- AVLUpdateHeightIMP function ----------------------*/

static void AVLUpdateHeightIMP(avl_node_t *root)
{
	long left_child_height = 0;
	long right_child_height = 0;
	
	left_child_height = AVLNodeGetHeight(AVLNodeGetSon(root, LEFT));
	right_child_height = AVLNodeGetHeight(AVLNodeGetSon(root, RIGHT));
	
	root->height = MaxHeightIMP(left_child_height, right_child_height) + 1;
}

/******************************************************************************/
/*------------------------- MaxHeightIMP function ----------------------------*/

long MaxHeightIMP(long height1, long height2)
{
	return height1 > height2 ? height1 : height2;
}

/******************************************************************************/
/*-------------------------AVLSideAccordingToCmpFuncIMP function -------------*/

static e_sides AVLSideAccordingToCmpFuncIMP(const avl_t *avl, const void *data1, const void *data2)
{
	int cmp_func_res = 0;
	
	assert(NULL != avl);
	
	cmp_func_res = avl->cmp_func(data1, data2, avl->param);
	
	assert(0 !=  cmp_func_res);
	
	return cmp_func_res < 0 ? LEFT : RIGHT;
}    

/******************************************************************************/
/*------------------------- AVLRemove function -------------------------------*/

/* O(log n) */
void AVLRemove(avl_t *avl, const void *data)
{
	assert(NULL != avl);
	
	avl->root = AVLRemoveIMP(avl, avl->root, data);
}

/******************************************************************************/
/*------------------------- AVLRemoveIMP function ----------------------------*/

static avl_node_t *AVLRemoveIMP(avl_t *avl, avl_node_t *root, const void *data)
{
	e_sides side = LEFT;
	
	assert(NULL != avl);
	
	if (NULL == root)
	{
		return NULL;
	}
	
	if (0 == avl->cmp_func(data, root->data, avl->param))
	{
		return AVLRemoveNodeIMP(root);
	}
	
	side = AVLSideAccordingToCmpFuncIMP(avl, data, root->data);
	root->children[side] = AVLRemoveIMP(avl, AVLNodeGetSon(root, side), data);
	root = AVLBalanceIMP(root);
	AVLUpdateHeightIMP(root);
	
	return root;
}

/******************************************************************************/
/*------------------------- AVLRemoveNodeIMP function ------------------------*/

static avl_node_t *AVLRemoveNodeIMP(avl_node_t *to_be_removed)
{
		if (AVLNodeIsLeaf(to_be_removed))
		{
			AVLNodeDestroy(to_be_removed);
			return NULL;
		}
		
		if (AVLNodeHasOnlyOneChild(to_be_removed))
		{
			return AVLRemoveNodeWithOneChildIMP(to_be_removed, AVLNodeValidSon(to_be_removed));
		}
		
		return AVLRemoveNodeWithTwoChildrenIMP(to_be_removed);		
}

/******************************************************************************/
/*------------------------- AVLRemoveNodeWithOneChildIMP function ------------*/

static avl_node_t *AVLRemoveNodeWithOneChildIMP(avl_node_t *root, e_sides side)
{
	avl_node_t *child = NULL;
		 
	assert(NULL != root);
	
	child = AVLNodeGetSon(root, side);
	AVLSwapDataIMP(root, child);	 
	AVLNodeDestroy(root);
	
	return child;	
}

/******************************************************************************/
/*------------------------- AVLSwapDataIMP function --------------------------*/

static void AVLSwapDataIMP(avl_node_t *avl_node1, avl_node_t *avl_node2)
{
	void *swapped_data = NULL;
	
	assert(NULL != avl_node1);
	assert(NULL != avl_node2);
	
	swapped_data = AVLNodeGetData(avl_node1);
	AVLNodeSetData(avl_node1, AVLNodeGetData(avl_node2));
	AVLNodeSetData(avl_node2, swapped_data);	
}

/******************************************************************************/
/*------------------------- AVLRemoveNodeWithTwoChildrenIMP function ---------*/

static avl_node_t *AVLRemoveNodeWithTwoChildrenIMP(avl_node_t *root)
{
	avl_node_t *right_child = NULL;
		 
	assert(NULL != root);
	
	right_child = AVLNodeGetSon(root, RIGHT);
	right_child = AVLMoveAsFarLeftAsPossibleIMP(root, right_child);
	
	root->children[RIGHT] = right_child;
	   	
	return root;
}

/******************************************************************************/
/*------------------------- AVLFind function ---------------------------------*/

/* O(log n) */
void *AVLFind(const avl_t *avl, const void *data)
{
	assert(NULL != avl);
	
	return AVLFindIMP(avl, avl->root, data);
}

/******************************************************************************/
/*------------------------- AVLFindIMP function ------------------------------*/

static void *AVLFindIMP(const avl_t *avl, avl_node_t *root, const void *data)
{
	e_sides side = LEFT;
	
	if (NULL == root)
	{
		return NULL;
	}
	if (0 == avl->cmp_func(data, root->data, avl->param))
	{
		return root->data;
	}
	
	side = AVLSideAccordingToCmpFuncIMP(avl, data, root->data);  
	
	return AVLFindIMP(avl, AVLNodeGetSon(root, side), data);  
}

/******************************************************************************/
/*------------------------- AVLIsEmpty function ------------------------------*/

int AVLIsEmpty(const avl_t *avl)
{
	assert(NULL != avl);
	
	return (NULL == avl->root? TRUE_CASE : FALSE_CASE); 
}

/******************************************************************************/
/*------------------------- AVLSize function ---------------------------------*/

/* pre order */
size_t AVLSize(const avl_t *avl)
{
	assert(NULL != avl);
	
	return AVLSizeIMP(avl->root);
}

/******************************************************************************/
/*------------------------- AVLSizeIMP function ------------------------------*/

static size_t AVLSizeIMP(avl_node_t *root)
{
	if (NULL == root)
	{
		return 0;
	}
	
	return 1 + 
		AVLSizeIMP(root->children[LEFT]) + AVLSizeIMP(root->children[RIGHT]);	
} 

/******************************************************************************/
/*------------------------- AVLHeight function -------------------------------*/

/* O(1) */
size_t AVLHeight(const avl_t *avl)
{
	assert(NULL != avl);
	
	return AVLHeightIMP(avl);
}

/******************************************************************************/
/*------------------------- AVLHeightIMP function ----------------------------*/

static long AVLHeightIMP(const avl_t *avl)
{
	assert(NULL != avl);
	
	return avl->root->height + 1;
}

/******************************************************************************/
/*------------------------- AVLForEach function ------------------------------*/

/* in order */
int AVLForEach(avl_t *avl, int (*op_func)(void *data, void *param), void *param)
{
	assert(NULL != avl);
	assert(NULL != op_func);
	
	return AVLForEachIMP(avl->root, op_func, param); 
}

/******************************************************************************/
/*------------------------- AVLForEachIMP function ---------------------------*/

int AVLForEachIMP(avl_node_t *root,
				 int (*op_func)(void *data, void *param), void *param)
{
	int result = SUCCESS; 
	
	assert(NULL != op_func);
	
	if (NULL == root)
	{
		return SUCCESS;
	}
	
	result += AVLForEachIMP(AVLNodeGetSon(root, LEFT), op_func, param);
	result += op_func(AVLNodeGetData(root), param);
	if (SUCCESS != result)
	{
		return result;
	}
		
	return AVLForEachIMP(AVLNodeGetSon(root, RIGHT), op_func, param);		
}

/******************************************************************************/
/*--------------------------AVLMoveAsFarAsPossibleIMP function----------------*/

static avl_node_t *AVLMoveAsFarLeftAsPossibleIMP(avl_node_t *root, avl_node_t *child)
{
	if (NULL == AVLNodeGetSon(child, LEFT))
	{
		AVLSwapDataIMP(child, root);
		return AVLRemoveNodeIMP(child); 
	}
	
	child->children[LEFT] = AVLMoveAsFarLeftAsPossibleIMP(root, AVLNodeGetSon(child, LEFT));
	child = AVLBalanceIMP(child);
	AVLUpdateHeightIMP(child); 
	return child;									  
}

/******************************************************************************/
/*--------------------------AVLFindIf function--------------------------------*/

int AVLFindIf(avl_t *avl, int (*is_match)(void *data, void *param), 
		void *param, dlist_t *list)
{
	find_if_wrap_t param_wrapper = {0};
	
	assert(NULL != avl);
	assert(NULL != is_match);
		
	param_wrapper.is_match = is_match;
	param_wrapper.param = param;
	param_wrapper.list = list;
	
	return AVLForEach(avl, &IsMatchWrapperForFindIfIMP, &param_wrapper);
}

/******************************************************************************/
/*--------------------------IsMatchWrapperIMP function------------------------*/

static int IsMatchWrapperForFindIfIMP(void *data, void *param)
{
	find_if_wrap_t *unwrapped_param = (find_if_wrap_t*)param;
	int is_match_res = 0;
	
	assert(NULL != param);
	
	is_match_res = (unwrapped_param->is_match(data, unwrapped_param->param));
	if (TRUE_CASE == is_match_res)
	{
		return !DListPushBack(unwrapped_param->list, data);
	}
	
	return SUCCESS;
}

/******************************************************************************/
/*--------------------------AVLRemoveIf function------------------------------*/

void AVLRemoveIf(avl_t *avl, int (*is_match)(void *data, void *param), 
		void *param)
{
	remove_if_wrap_t param_wrapper = {0};
	
	assert(NULL != avl);
	assert(NULL != is_match);
	
	param_wrapper.is_match = is_match;
	param_wrapper.param = param;
	param_wrapper.avl = avl;
	
	
	while (AVLForEach(avl, &IsMatchWrapperForRemoveIfIMP, &param_wrapper))
	{
		;
	}
}

/******************************************************************************/
/*--------------------------IsMatchWrapperIMP function------------------------*/

static int IsMatchWrapperForRemoveIfIMP(void *data, void *param)
{
	remove_if_wrap_t *unwrapped_param = (remove_if_wrap_t*)param;
	int is_match_res = 0;
	
	assert(NULL != param);
	
	is_match_res = (unwrapped_param->is_match(data, unwrapped_param->param));
	if (TRUE_CASE == is_match_res)
	{
		AVLRemove(unwrapped_param->avl, data);
		
		return FAILURE;
	}
	
	return SUCCESS;
}

/******************************************************************************/
/*------------------------- End Of File --------------------------------------*/

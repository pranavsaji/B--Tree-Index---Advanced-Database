#include "btree_implement.h"
#include "dt.h"
#include "string.h"


NodeData *makeRecord(RID *rid) {
   NodeData *record = (NodeData *) calloc(1, sizeof(NodeData));
    if (!record) {
        perror("Error creating NodeData.");
        exit(RC_INSERT_ERROR);
    }
   if(record!=NULL){
    (*record).rid.page = (*rid).page;
   (*record).rid.slot = (*rid).slot;
   }
    return record;
}

Node* createNewTree(BTreeManager* treeManager, Value* key, NodeData* pointer) {
    int b_tree_order;
    int i=0;
    b_tree_order = treeManager->order;
    Node* root = createLeaf(treeManager);
    if(i==0){
    *root->keys = key;
    *(root->pointers) = pointer;
    *(root->pointers + b_tree_order - 1) = 0;
    (*root).parent = 0;
    root->num_keys += 1;

    treeManager->numEntries += 1;
    }
    return root;
}

Node * insertIntoLeaf(BTreeManager * treeManager, Node * leaf, Value * key, NodeData * pointer) {
    int i;
    int insertion;
    insertion = 0;
    treeManager->numEntries = treeManager->numEntries + 1;
    
 

while (insertion< leaf->num_keys) {
	if (!isLess(leaf->keys[insertion], key))
		break;
	insertion += 1;
}
    
    // Shift keys and pointers to make room for the new key
    for (i = leaf->num_keys - 1; i >= insertion; i--) {
        leaf->keys[i + 1] = leaf->keys[i];
        leaf->pointers[i + 1] = leaf->pointers[i];
    }
    
    // Insert the new key and pointer
    *(leaf->keys + insertion) = key;
    *(leaf->pointers + insertion) = pointer;
    leaf->num_keys += 1;
    
    return leaf;
}

extern Node * insertIntoLeafAfterSplitting(BTreeManager * treeManager, Node * leaf, Value * key, NodeData * pointer) {
	
    Node * leaf_new; // the new leaf node
	printf("New leaf node");
	Value ** keys_tp; // temporary pointers array for keys

	void ** pointers_tp; // temporary pointers array for pointers
	printf("--");
	int sp_lit; // split point
	

	int key_new; // new key
	int insert_id; // position to insert the new key
    leaf_new = createLeaf(treeManager); // create a new leaf node
	int B_tree_order; // order of the B-tree
	B_tree_order = treeManager->order;
    
    // allocate memory for temporary pointers arrays
	keys_tp = malloc(B_tree_order * sizeof(Value));
	if (!keys_tp) {
		fprintf(stderr, "Error: Temporary pointers array. \n");
        return RC_INSERT_ERROR;
	}
    
    pointers_tp = malloc(B_tree_order * sizeof(void*));
	if (!pointers_tp) {
		fprintf(stderr, "Error: Temporary pointers array. \n");
        return RC_INSERT_ERROR;
	}

    // find the position to insert the new key
    insert_id = 0;
    while (insert_id < B_tree_order - 1) {
        if (isLess(leaf->keys[insert_id], key)) {
            insert_id++;
        } else {
            break;
        }
    }

    int iter = 0;

    // distribute the entries between the old and new nodes
   int i = 0;
while (i < leaf->num_keys) {
	bool boo = true;
	if (boo != false)
	{
		 if (iter == insert_id) {
        iter++;
    }
	}
	printf("Distributing");
   
    keys_tp[iter] = leaf->keys[i];
    pointers_tp[iter] = leaf->pointers[i];
    iter++;
    i++;
}


    *(keys_tp + insert_id) = key;
    *(pointers_tp + insert_id) = pointer;

    leaf->num_keys = NULL;

    // find the split point
    sp_lit = (B_tree_order - 1) % 2 == 0 ? (B_tree_order - 1) / 2 : (B_tree_order - 1) / 2 + 1;

    // copy the first half of the entries to the old node
    for (int i = 0; i < sp_lit; i++) {
        *(leaf->pointers + i) = *(pointers_tp + i);
        *(leaf->keys + i) = *(keys_tp + i);
        leaf->num_keys += 1;
    }

    // copy the second half of the entries to the new node
	int iterV;
    for (int i = sp_lit, iterV = 0; i < B_tree_order; i++, iterV++) {
        *(leaf_new->pointers + iterV) = *(pointers_tp + i);
        *(leaf_new->keys + iterV) = *(keys_tp + i);
        leaf_new->num_keys += 1;
    }

    free(pointers_tp);
    free(keys_tp);

    *(leaf_new->pointers + B_tree_order - 1) = *(leaf->pointers + B_tree_order - 1);
    leaf->pointers[B_tree_order - 1] = &(*leaf_new);

    // set the remaining pointers to NULL
    int b = 1;
	
	

	  for (int i = leaf->num_keys; i < B_tree_order - 1; i++) 
	
	  {
		  *(leaf->pointers + i) = 0;
      }  

	
	if(b==1){

	  for (int i = leaf_new->num_keys; i < B_tree_order - 1; i++) 
	   {
		  *(leaf_new->pointers + i) = 0;
        }
	}

	leaf_new->parent = leaf->parent;
	key_new = *(leaf_new->keys);
	treeManager->numEntries = treeManager->numEntries + 1;

	return insertIntoParent(treeManager, leaf, key_new, leaf_new);
}

extern Node* insertIntoNodeAfterSplitting(BTreeManager* treeManager, Node* oldNode, int leftIndex, Value* key, Node* rightNode) {
    int i; 
    int j;
    int splitIndex;
    int kPrime;
    Node* newNode;
	Node* childNode;
    Value** tempKeys;
    Node** tempPointers;
    int order = treeManager->order;
    int maxKeys = order - 1;

    
    tempPointers = malloc((order + 1) * sizeof(Node));

    tempKeys = malloc(order * sizeof(Value*));
    if (tempPointers == NULL || tempKeys == NULL) {
        perror("Failed to allocate memory for temporary arrays");
        exit(RC_INSERT_ERROR);
    }

    
    for (i = 0, j = 0; i < oldNode->num_keys + 1; i++) {
        if (i == leftIndex + 1) {
            tempPointers[i] = rightNode;
            j++;
        }
        tempPointers[i + 1] = oldNode->pointers[j];
        j++;
    }

    for (i = 0, j = 0; i < oldNode->num_keys; i++, j++) {
        if (j == leftIndex) {
            tempKeys[j] = key;
            j++;
        }
        tempKeys[j] = oldNode->keys[i];
    }

    if (j == leftIndex) {
        tempKeys[j] = key;
        j++;
    }

    
    if (maxKeys % 2 == 0) {
        splitIndex = maxKeys / 2;
    }
    else {
        splitIndex = (maxKeys + 1) / 2;
    }
    newNode = createNode(treeManager);

    
    oldNode->num_keys = 0;
    while (i < splitIndex) {
        *(oldNode->pointers + i) = *(tempPointers + i);    
       *(oldNode->keys + i) = *(tempKeys + i);
        oldNode->num_keys = oldNode->num_keys + 1;
        i = i + 1;
   }

    oldNode->pointers[i] = tempPointers[i];
    kPrime = *(tempKeys + splitIndex - 1);
    newNode->num_keys = NULL;
    i = splitIndex;
    j = 0;
    while (i < order) {
      newNode->pointers[j] = tempPointers[i];
       newNode->keys[j] = tempKeys[i];
       newNode->num_keys++;
      i++;
      j++;
    }

    newNode->pointers[j] = tempPointers[i];

    
    newNode->parent = oldNode->parent;

    for (i = 0; i <= newNode->num_keys; i++) {
		Node * child = newNode->pointers[i];
		child->parent = newNode;
	}

    
    treeManager->numEntries += 1;
    return insertIntoParent(treeManager, oldNode, kPrime, newNode);
}

extern Node * insertIntoParent(BTreeManager * treeManager, Node * left, Value * key, Node * right) {

	
	Node * parent = left->parent;
	

	
	int i=0;
	if(i==0){
	  if (parent == 0){

		return insertIntoNewRoot(treeManager, left, key, right);

	    }
	}
	
	int left_id_index = getLeftIndex(parent, left);
     
	if(i==0){
	    if (  treeManager->order - 1  > parent->num_keys) {

		  return insertIntoNode(treeManager, parent, left_id_index, key, right);

	     }
	}

	
	return insertIntoNodeAfterSplitting(treeManager, parent, left_id_index, key, right);
}

extern int getLeftIndex(Node * parent, Node * left) {
    // Initialize variables
    int left_index;
    left_index = 0;

    // Find the index of the left node in the parent's pointers array
    while (left_index <= parent->num_keys) {
        if (parent->pointers[left_index] == left) {
            return left_index;
        }
        left_index += 1;
    }

    // If the left node is not found, return -1
    return -1;
}


Node * insertIntoNode(BTreeManager * treeManager, Node * parent, int left_index, Value * key, Node * right) {
    int num_keys = parent->num_keys;
    int i = num_keys - 1;

    // Shift pointers and keys to make room for the new entry
    while (i >= left_index) {
        parent->pointers[i + 1] = parent->pointers[i];
        parent->keys[i + 1] = parent->keys[i];
        i--;
    }

    // Insert the new key and pointer
    parent->pointers[left_index + 1] = right;
    parent->keys[left_index] = key;
    parent->num_keys++;

    // If the parent was the root, update the root pointer in the tree manager
    if (parent == treeManager->root) {
        treeManager->root = parent;
    }

    return parent;
}

Node* insertIntoNewRoot(BTreeManager* treeManager, Node* left, Value* key, Node* right) {
    Node* newRoot = createNode(treeManager);
	int zero= 0;
	int one=1;
    newRoot->keys[zero] = key;
    newRoot->pointers[zero] = left;
    newRoot->pointers[one] = right;
    newRoot->num_keys = newRoot->num_keys + one;
    newRoot->parent = NULL;
    right->parent = newRoot;
    left->parent = newRoot;
    return newRoot;
}

Node * createNode(BTreeManager * treeManager) {
    int b_tree_order;
    b_tree_order = treeManager->order;
    int numNodes = treeManager->numNodes;
    numNodes = numNodes + 1;
    treeManager->numNodes = numNodes;
    
    
    Node * n_node = (Node*) calloc(1, sizeof(Node));

    if (!n_node) {
        fprintf(stderr, "Error creating node.\n");
        exit(RC_INSERT_ERROR);
    }

    n_node->keys = (Value **) calloc(b_tree_order - 1, sizeof(Value *));
	
    if (!n_node->keys) {
        fprintf(stderr, "Failed to create new node keys array.\n");
        exit(RC_INSERT_ERROR);
    }

    n_node->pointers = (void **) calloc(b_tree_order, sizeof(void *));

    if (!n_node->pointers) {
        fprintf(stderr, "New node pointers array..\n");
        exit(RC_INSERT_ERROR);
    }

    n_node->is_leaf = (bool) false;
    (*n_node).num_keys = 0;
   (*n_node).parent = NULL;
    (*n_node).next = NULL;

    return n_node;
}

// This function creates a new leaf node for the B-tree managed by treeManager.
// The leaf node is initialized with a default value of zero.
Node * createLeaf(BTreeManager * treeManager) {
	// Initialize a temporary variable 'a' to zero.
	int a;
	a=0;

	// Check if 'a' is equal to zero.
	if(a==0){
		// If 'a' is zero, create a new node using the createNode() function
		// provided by the B-tree manager, and set its 'is_leaf' attribute to true.
		Node * create_leaf = createNode(treeManager);
		create_leaf->is_leaf = (bool) true;

		// Return the newly created leaf node.
		return create_leaf;
	}
}

Node * findLeaf(Node * root, Value * key) {
	
	int i;
	i=0;
	Node * find_l = root;
	if (!find_l) {
		return find_l;
	}
	while (!find_l->is_leaf) {
		int i = find_l->num_keys - 1;
		while (i >= 0 && isLess(key, find_l->keys[i])) {
			i--;
		}
		find_l = (Node *) find_l->pointers[i+1];
}

	return find_l;
}

NodeData *findRecord(Node *root, Value *key) {
    Node *leaf = findLeaf(root, key);
    if (!leaf) {
        return NULL;
    }
    int i = 0;
    while (i < leaf->num_keys && !isEqual(leaf->keys[i], key)) {
        i++;
    }
    if (i == leaf->num_keys) {
        return NULL;
    } else {
        return (NodeData *) leaf->pointers[i];
    }
}


//DELETION 
/*
This function returns the index of the nearest left sibling node of a given node, if one exists.
 If the node is the leftmost child and has no left sibling, the function returns -1 to indicate this special case.
*/

int getNeighborIndex(Node * n) {

	
	bool rtnptr=true;
	/* 
	 *The function should return the index of the key to the
	  left of the pointer in the parent that is pointing to node n. 
	  If node n happens to be the leftmost child, then the function should return -1.
	 */

	int j=0;
	int b;
	b =n->parent->num_keys;
	if(rtnptr!=false)
	{

	while(j <= b)
	{
		if (n->parent->pointers[j] == n)
			return j - 1;
		j++;
	}


	exit(RC_ERROR);
	}
}

// Delete the record with the given key from the specified node.
Node * removeEntryFromNode(BTreeManager * treeManager, Node * n, Value * key, Node * pointer) {

	int j;
	int num_ptr;
	int i;
	j=0;
	
	


int ptrval;
int num_pointers;
	

	//Delete the specified key and shift the remaining keys to adjust for the removed key.
	

	while (!isEqual(n->keys[j], key))
	{
		j++;
	}
	int Ordbtree = treeManager->order;
	
	ptrval=pointer;
	j=j+1;
	while(j < n->num_keys)
	{
		n->keys[j - 1] = n->keys[j];
		j++;
	}


	// Delete the specified pointer and shift the remaining pointers to adjust for the removed pointer.
	// Initially, calculate the number of pointers.

	if (!n->is_leaf) 
	{
    num_ptr = n->num_keys+1;
	} 
	else 
	{
    num_ptr = n->num_keys;
	}
	j = 0;
	i=0;
	while (n->pointers[j] != ptrval)
	{
		i++;
		j++;
	}
	i=i+1;

	
	while(i < num_ptr)
	{
		n->pointers[i - 1] = n->pointers[i];
		i++;
	}
	treeManager->numEntries=treeManager->numEntries-1;

	n->num_keys=n->num_keys-1;
	
	// Set the remaining pointers to NULL to maintain a clean structure.
	// The final pointer in a leaf node points to the next adjacent leaf node.
if (!n->is_leaf)
{
    i = n->num_keys + 1;
    while (i < Ordbtree)
    {
        n->pointers[i] = NULL;
        i++;
    }
}
else
{
    i = n->num_keys;
    while (i < Ordbtree - 1)
    {
        n->pointers[i] = NULL;
        i++;
    }
}

	return n;

}


// The purpose of this function is to modify the root node appropriately after a record has been removed from the B+ tree.
Node * adjustRoot(Node * root) {

	Node * root_latest;
	bool free_flag=true;

	//If the root node is not empty, it indicates that the key and pointer have already been removed. 
	//In this case, the function does nothing and simply returns the root node.
		
	if (root->num_keys <= 0)
	{
	
	if (root->is_leaf)
	{
		//If the root node is not empty and is a leaf node (has no children), the entire B+ tree is empty.
		root_latest = NULL;
	}
	else
	{
	
		//If the root node is not empty and has a child, set the first (and only) child as the new root node.
		root_latest = root->pointers[0];
		root_latest->parent = NULL;
	}
	
	}
	else
	{
		return root;
	}
	if (free_flag!=false)
	{
	//Release the allocated memory space to free up memory.
	
	

	free(root->keys);
	printf("It is deallocated");
	free(root->pointers);
	printf("It is deallocated");
	free(root);
	printf("It is deallocated");
	}
	return root_latest;
}



/*
This function merges a node that has become too small after removing entries with a 
neighboring node that can accommodate the additional entries without violating the maximum limit.
*/
Node * mergeNodes(BTreeManager * treeManager, Node * n, Node * neighbor, int neighbor_index, int k_prime) {
	Node * temporary;
	int copyflag=0;
	int index_val;
	int end_val;
	int i;
	int j=0;
	int s=0;
	bool free_flag=true;
	if(copyflag!=1)
	{
	// If a node is positioned at the far left and its neighbor is to the right, swap the node with its neighbor.
	if (neighbor_index == -1) 
	{
		temporary = n;
		for(i=0;i<1;i++)
		{
		n = neighbor;
		neighbor = temporary;
		}
		printf("value stored to neighbor node");
	}
	}
	int Ord_btree = treeManager->order;
/*
In the special case where n is the leftmost child and has been swapped with its neighbor, 
the starting point for copying keys and pointers from n is located in its neighbor.
*/
	index_val = neighbor->num_keys;


	if(n->is_leaf) 
	{
		i = index_val;
		while(j < n->num_keys)
		{
			neighbor->keys[i] = n->keys[j];
			s++;
			neighbor->pointers[i] = n->pointers[j];
			i++;
			neighbor->num_keys=neighbor->num_keys+1;
			j++;
		}
		
		neighbor->pointers[Ord_btree - 1] = n->pointers[Ord_btree - 1];
	}
	else {
		neighbor->keys[index_val] = k_prime;
		neighbor->num_keys=neighbor->num_keys+1;

		end_val = n->num_keys;
		j=0;
		i = index_val + 1;
		while(j < end_val)
		{
			neighbor->keys[i] = n->keys[j];
			s++;
			neighbor->pointers[i] = n->pointers[j];
			i++;
			neighbor->num_keys=neighbor->num_keys+1;
			
			n->num_keys=n->num_keys-1;
			j++;

		}

		neighbor->pointers[i] = n->pointers[j];
		i=0;

		// Setting all child nodes to have the same parent node.
		while(i < neighbor->num_keys + 1)
		{
			temporary = (Node *) neighbor->pointers[i];
			i++;
			temporary->parent = neighbor;
		}

	} 

	treeManager->root = deleteEntry(treeManager, n->parent, k_prime, n);

	if (free_flag!=false)
	{
	//Release the allocated memory space to free up memory.
	
	

	free(n->keys);
	printf("It is deallocated");
	free(n->pointers);
	printf("It is deallocated");
	free(n);
	printf("It is deallocated");
	}

	return treeManager->root;
}

/* The purpose of this function is to remove a specific entry from the B+ tree. It first removes the record 
with the given key and pointer from the appropriate leaf node, and then makes any
 necessary adjustments to maintain the integrity of the B+ tree.
 */Node * deleteEntry(BTreeManager * treeManager, Node * n, Value * key, void * pointer) {
	Node * neighbor_node;

	int mininum_key;
	int Ordbtree = treeManager->order;
	int indx_neighbor;
	int indx_k_prime;
	int s=0;
	int k_prime_val;
	int cap;
	

	// Delete the specified key and pointer from the node.
	n = removeEntryFromNode(treeManager, n, key, pointer);
	

	// If the node n is the root of the B+ tree, perform adjustments to the tree structure.
	if (n == treeManager->root)
	{
		return adjustRoot(treeManager->root);
	}

	// Calculate the minimum size that a node can have and still preserve the B+ tree structure after a deletion.

	if(!n->is_leaf) 
	{
		if ((Ordbtree) % 2 == 1)
		{
		mininum_key = (Ordbtree) / 2 + 1;
		}
			
		else
		{
			mininum_key = (Ordbtree) / 2;
		}
		mininum_key=mininum_key-1;
	}
	else
	{
		if ((Ordbtree - 1) % 2 == 1)
		{
			mininum_key = (Ordbtree - 1) / 2 + 1;
			
		}
		else
		{
			mininum_key = (Ordbtree - 1) / 2;
		}
	} 
	if (n->num_keys < mininum_key)
	{
		printf("continuing");
	}
	
	else
	{
		return treeManager->root;
	}
/*
	If the node falls below the minimum allowable size after deletion, 
	it's necessary to either merge it with a neighbor or redistribute keys between them. 
	To do so, identify the appropriate neighbor node and the key (k_prime) 
	in the parent node that separates n from the neighbor.*/

	indx_neighbor = getNeighborIndex(n);
	if (indx_neighbor != -1) {
    indx_k_prime = indx_neighbor;
	}
	else {
    indx_k_prime = 0;
} 
	k_prime_val = n->parent->keys[indx_k_prime];

	if (indx_neighbor != -1) 
	{
    neighbor_node = n->parent->pointers[indx_neighbor];
}
	else {
    neighbor_node = n->parent->pointers[1];
} 

if (!n->is_leaf) 
{
    cap = Ordbtree-1;
} 
else 
{
    cap = Ordbtree;
}
	if (neighbor_node->num_keys + n->num_keys >= cap)
	{
		// Redistributing nodes
		return redistributeNodes(treeManager->root, n, neighbor_node, indx_neighbor, indx_k_prime, k_prime_val);
	}

	else
	{
		// Merging nodes
		return mergeNodes(treeManager, n, neighbor_node, indx_neighbor, k_prime_val);
		printf("merging is done");
	}
	printf("Continue");
	
}




// The record with the specified key is removed using this function.
Node * delete(BTreeManager * treeManager, Value * key) 
{
	printf("Starting delete");
	
	bool free_val=true;
	printf("initialising node");
	NodeData * leaf_key = findLeaf(treeManager->root, key);
	int s=0;
	Node * record_value = findRecord(treeManager->root, key);
	
	printf("continue");
	if(record_value != NULL)
	{
		if(leaf_key != NULL)
		{
			treeManager->root = deleteEntry(treeManager, leaf_key, key, record_value);
			if(free_val!=false)
			{
			free(record_value);
			}
		}

	}

	return treeManager->root;
	printf("already returned");
}

/*
When one node becomes too small after deletion but its neighbor is too large to accommodate the small node's 
entries without surpassing the maximum, this function reassigns the entries between the two nodes.
*/
Node * redistributeNodes(Node * root, Node * n, Node * neighbor, int neighbor_index, int k_prime_index, int k_prime) {
	int i;
	int s=0;
	int flg=1;
	Node * temp_node;
	int j=1;
 if (neighbor_index == -1) {
		//If "n" is the first child, get a key-pointer pair from its neighbor to the right. 
		//Afterwards, move the leftmost key-pointer pair from the neighbor to the far-right position of "n".
		if (!n->is_leaf) 
		{
			s++;
			n->keys[n->num_keys] = k_prime;
			s--;
			n->pointers[n->num_keys + 1] = neighbor->pointers[0];
			printf("running");
			if(flg!=0)
			{
			temp_node = (Node *) n->pointers[n->num_keys + 1];
			temp_node->parent = n;
			}
			s--;
			n->parent->keys[k_prime_index] = neighbor->keys[0];
		}
		else {
			n->keys[n->num_keys] = neighbor->keys[0];
			if(flg!=0)
			{
			n->pointers[n->num_keys] = neighbor->pointers[0];
			}
			s++;
			n->parent->keys[k_prime_index] = neighbor->keys[1];
		} 
		i=0;
		s=0;
		while(i < neighbor->num_keys - 1)
		{
			neighbor->keys[i] = neighbor->keys[i + 1];
			s++;
			neighbor->pointers[i] = neighbor->pointers[i + 1];
			i++;
		}
		if (n->is_leaf)
		{
			printf("continue");
		}
		else
		{
			neighbor->pointers[i] = neighbor->pointers[i + 1];
			s++;
		}
	}
	else 
	{
		s--;
		// Assuming that "n" has a neighbor to its left, move the last key-pointer pair of the neighbor from its right end to the left end of "n".
		
		if (!n->is_leaf)
		{
			n->pointers[n->num_keys + 1] = n->pointers[n->num_keys];
		}
		s=0;
		i = n->num_keys;
		while(i > 0)
		{
			n->keys[i] = n->keys[i - 1];
			s++;
			n->pointers[i] = n->pointers[i - 1];
			i--;
		}
		s=0;
		if (n->is_leaf) {
			if(flg!=0)
			{
			n->pointers[0] = neighbor->pointers[neighbor->num_keys - 1];
			}
			if(j!=0)
			{
			printf("continue");
			neighbor->pointers[neighbor->num_keys - j] = NULL;
			}
			s++;
			n->keys[0] = neighbor->keys[neighbor->num_keys - 1];
			printf("running");
			n->parent->keys[k_prime_index] = n->keys[0];
		}

		else {
			n->pointers[0] = neighbor->pointers[neighbor->num_keys];
			if(flg!=0)
			{
			temp_node = (Node *) n->pointers[0];
			}
			printf("using temp");
			temp_node->parent = n;
			s++;
			neighbor->pointers[neighbor->num_keys] = NULL;
			printf("running");
			n->keys[0] = k_prime;
			s--;
			n->parent->keys[k_prime_index] = neighbor->keys[neighbor->num_keys - 1];
		} 
	} 
	
if(flg!=0)
{
	// Currently, 'n' possesses an additional key and pointer, while its neighbor has one less of both.
	n->num_keys=n->num_keys+1;
	s--;
	neighbor->num_keys=neighbor->num_keys-1;
}
	return root;
}


// PRINT FUNCTION 

//  B+ Tree print function
void enqueue(BTreeManager * treeManager, Node * new_node) {
	
	int s=0;
	Node * encq;
	if (treeManager->queue != NULL) {
		encq = treeManager->queue;
		while (encq->next != NULL) 
		{
			encq = encq->next;
		}
		encq->next = new_node;
		s++;
		new_node->next = NULL;
	}

	else {
		treeManager->queue = new_node;
		s++;
		treeManager->queue->next = NULL;
	} 
}

// B+ Tree print function
Node * dequeue(BTreeManager * treeManager) {
	Node * n;
	bool assign=true;
	n = treeManager->queue;
	int s=0;
	if (assign!=false)
	{
	treeManager->queue = treeManager->queue->next;
	}
	n->next = NULL;
	s++;
	return n;
}

// The length in edges of the path from any node to the root can be obtained using this function.
int path_to_root(Node * root, Node * child) 
{
	int len = 0;
	bool fl = true;
	Node * chld = child;
	while(chld == root)
	{
		continue;
	}
	while (chld != root) 
	{
		chld = chld->parent;
		if(fl!=false)
		{
		len=len+1;
		}
	}
	return len;
}



/*********** SUPPORT MULTIPLE DATATYPES *************/

// The following function determines whether the first key is smaller than the second key and returns TRUE if that is the case.
bool isLess(Value * key1, Value * key2) 
{

	int d;
	int a,b;
	float c,e;
	d=key1->dt;
	a=key1->v.intV;
	b=key2->v.intV;
	c=key1->v.floatV;
	e=key2->v.floatV;
	int s=0;
	



	switch (d) 
	{

	case DT_FLOAT:
		if (c >= e)
		 {
			s++;
			printf("returning false");
			return FALSE;
		}

		else {
			s++;
			printf("returning true");
			return TRUE;
		} 
		break;
	case DT_INT:
		if (a>= b) {
			s++;
			printf("returning false");
			return FALSE;
		}
		else 
		{
			s++;
			printf("returning true");
			return TRUE;
		} 
		break;
	case DT_BOOL:
		s++;
		printf("returning false");
		
		return FALSE;
		break;
	case DT_STRING:

		if(strcmp(key1->v.stringV, key2->v.stringV) != -1) {
			s++;
			printf("returning false");
			return FALSE;
		}
		else 
		{
			s++;
			printf("returning true");
			return TRUE;
		} 
		break;
	
	}
}

// The following function determines whether two keys are equal, and returns TRUE if they are, and FALSE otherwise.
bool isEqual(Value * key1, Value * key2) {

	int d;
	int a,b;
	float c,e;
	int s=0;
	d=key1->dt;
	a=key1->v.intV;
	b=key2->v.intV;
	c=key1->v.floatV;
	e=key2->v.floatV;
	
	switch (d) {
	case DT_FLOAT:

	 if (c != e) 
	 {
			s++;
			printf("returning false");
			return FALSE;
		}
		else {
			s++;
			printf("returning true");
			return TRUE;
		}
		break;
	case DT_INT:
		if (a != b) {
			s++;
			printf("returning false");
			return FALSE;
		}
		else {
			s++;
			printf("returning true");
			return TRUE;
		} 
		break;
	case DT_BOOL:
	
		if (key1->v.boolV != key2->v.boolV) {
			s++;
			printf("returning false");
			return FALSE;
		}
		else {
			s++;
			printf("returning true");
			return TRUE;
		} 
		break;

	case DT_STRING:
		if (strcmp(key1->v.stringV, key2->v.stringV) != 0) {
			s++;
			printf("returning false");
			return FALSE;
		}
		else {
			s++;
			printf("returning true");
			return TRUE;
		} 
		break;
	printf("Done");

	}
}

// The following function evaluates whether the first key is greater than the second key and outputs a value of TRUE.
bool isGreater(Value * key1, Value * key2) {

	int d;
	int a,b;
	float c,e;
	d=key1->dt;
	a=key1->v.intV;
	b=key2->v.intV;
	c=key1->v.floatV;
	e=key2->v.floatV;
	int s=0;

	switch (d) 
	{

	case DT_FLOAT:

		if (c <= e) 
		{
			s++;
			printf("returning false");
			return FALSE;
		}
		else {
			s++;
			printf("returning true");
			return TRUE;
		} 
		break;

	case DT_INT:
		if (a <= b)
		 {
			s++;
			printf("returning false");
			return FALSE;
		}
		else {
			s++;
			printf("returning true");
			return TRUE;
		} 
		break;


	case DT_BOOL:
		s++;
		printf("returning false");
		
		return FALSE;
		break;

	case DT_STRING:

		if (strcmp(key1->v.stringV, key2->v.stringV) != 1) 
		{
			s++;
			printf("returning false");
			return FALSE;
		}
		else {
			s++;
			printf("returning true");
			return TRUE;
		} 
		break;

	}
}






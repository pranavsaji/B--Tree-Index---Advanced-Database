Academic Project

RUNNING THE SCRIPT
==========================


$ make clean 

$ make

$ make run

$ make test_expr

$ make run_expr


================================


CUSTOM B+ TREE FUNCTIONS (btree_implement.h)

===============================

findLeaf(...):
----> The function  is used to identify the leaf node in a B+ Tree data structure that holds a particular key. 
----> The function starts from the tree's root when inserting an element or looking for an entry and moves through the internal nodes by following pointers to child nodes until it finds the required leaf node.

findRecord(...):
--> A B+ Tree data structure can be searched for a certain key using the function findRecord(...), which then returns the appropriate record.
--> Starting at the tree's root, the function iteratively moves through the internal nodes by following pointers to child nodes until it reaches the required leaf node.

makeRecord(...):
---> The function makeRecord(...) is used to construct a record that will be  inserted into a B+ Tree data structure. Typically, a record is made up of a key-value pair, where the key serves as the record's identifier and the value is the data the key refers to.

----> The makeRecord(...) function generates a new record with the supplied key-value pair by taking in the key and value as inputs. The newly created record is then returned by the function.

insertIntoLeaf(...):
--->The function insertIntoLeaf(...) is used to add a fresh key-value pair to a leaf          node of a B+ Tree data structure. 
--->The insertIntoLeaf(...) function is used to add a new record to a B+ Tree after the findLeaf(...) function has discovered the proper leaf node.

createNewTree(...):
 --> When the initial element or entry is added to the B+ tree, this function generates a new tree.

createNode(...):
---->  is a function that can be used to create a new node in a tree data structure. The specifics of this function may vary depending on the type of tree being implemented.
---> With the use of this function, a new generic node is created that can be modified to act as a leaf, internal, or root node.

createLeaf(...):
---->This function that can be used to create a new leaf node in a tree data structure. A leaf node is a node that does not have any children and is typically found at the bottom of a tree.
---> A new leaf node is created by this function.

insertIntoLeafAfterSplitting(): 
---> This function that can be used to insert a new key into a leaf node of a B-tree data structure after the leaf node has been split due to being full.
---> This function splits a leaf node in half by inserting a new key and pointer to a new record into it, which exceeds the order of the tree. After splitting, the tree is adjusted to maintain the B+ Tree's characteristics.

insertIntoNode(...):
This function, insertIntoNode(...), adds a new key and a pointer to a node into a node that both of them can fit into without breaking the B+ tree's constraints.

insertIntoNodeAfterSplitting():
---> This function that can be used to insert a new key and a new child node into a non-leaf node of a tree data structure, such as a B-tree , when the node is already full and needs to be split before the insertion can be made.
---> This function splits a non-leaf node into two by inserting a new key and pointer to a node, which causes the node's size to exceed the order.

insertIntoParent():
---> This function that can be used to insert a new node into the parent of two nodes in a tree data structure, such as a B-tree. This function is typically called after a split operation, where the two nodes have been split and a new node has been created to hold the median key.
--->The function adds a new node (a leaf or inside node) to the B+ tree. After insertion, it returns the tree's root.

insertIntoNewRoot():
--->This a function that is used to create a new root node in a tree data structure when the existing root node is split. This function is typically called after a split operation, where the two nodes have been split and a new node has been created to hold the median key.
--->This function adds the relevant key to the newly created roots of the two subtrees.

getLeftIndex():
--->This function that is used in the B-tree data structure to find the index of the child node to the left of a given key in a parent node's array of keys. The left child is the child node that contains keys less than the given key.
--> The insertIntoParent(..) function uses this function to locate the index of the parent's reference to the node that is to the left of the key that has to be added.

adjustRoot(): 
--->This function that is used in the B-tree data structure to handle the case where the root node of the tree has to be adjusted or updated after a split operation. This can happen when the root node is split into two nodes and a new root node needs to be created.
--->After a record is removed from the B+ Tree, this function modifies the root while preserving the B+ Tree's attributes.

mergeNodes(...): 
---> This function combines (merges) a node that has shrunk after being deleted with a nearby node that can take in the extra entries without going over the maximum.

redistributeNodes(...):
--> When one node becomes too tiny after deletion but its neighbor is too big to append the small node's values without going over the limit, this function redistributes the entries between the two nodes.
---> This function that is used in the B-tree data structure to redistribute keys between two nodes when one of them has too few keys after a deletion operation. This function is typically called when a node is deleted and the number of keys in the node falls below a certain threshold, causing the node to become underflowed.

deleteEntry(...):
--->This function, removes an entry from the B+ tree.
--> It then makes any necessary adjustments to retain the B+ tree attributes after removing the record from the leaf that has the supplied key and pointer.
---> This function that is used in the B-tree data structure to delete a key from a node. This function is typically called when a key is to be deleted from a node and that node becomes underflowed.

delete(...): 
---> This function eliminates the record or entry with the given key.
---> This function used in a B-tree data structure to delete a key from the tree. 

removeEntryFromNode(...):
---> Removes a record with the supplied key from the specified node using the function
--->  This function used in a B-tree data structure to remove a key from a node. It takes in a node and an index of the key to be removed, and then removes the key and any associated child nodes or pointers.

getNeighborIndex(...):
--->If there is a nearest neighbor (sibling) to the left of a node, this function provides the index of that node.
--> If the node is the leftmost child and this is not the case, it returns -1.
---> This function used in a B-tree data structure to find the index of a neighboring node. It takes in a parent node and the index of a child node, and then returns the index of the left or right neighboring child node, depending on which side of the child node the neighboring node is on.

======================================================================================================================

ACCESSING B+ TREE FUNCTIONS
=========================================

findKey(...):

--> The key supplied in the parameter is looked up using this method's B+ Tree search.
--> If an entry containing the given key is located, the RID (value) for that key is stored in the memory address indicated by the "result" option.
--> We use the findRecord(..) method, which accomplishes the goal. The key is not present in the B+ Tree if findRecord(..) returns NULL, in which case the error code RC_IM_KEY_NOT_FOUND is returned.

insertKey(...):
--> We start by looking for the given key in the B+ Tree. If it is located, the error code RC_IM_KEY_ALREADY_EXISTS is returned.
If not, a NodeData structure is created and the RID is stored there.
--> We determine if the tree's root is empty. When we use createNewTree(..), a new B+ Tree is created and this entry is added to the tree if it is empty.
We find the leaf node where this entry can be added if our tree has a root element, meaning the tree was already there.
--> After locating the leaf node, we determine whether there is space for the new entry. If so, we use the function insertIntoLeaf(...) to insert the leaf.
--> In the event that the leaf node is already full, insertIntoLeafAfterSplitting(...) is called, which splits the leaf node and inserts the entry.

deleteKey(...):
--> This function removes the specified "key" from the B+ Tree's entry or record.
--> As previously said, we call our B+ Tree method delete(...). In order to maintain the B+ Tree attributes, this function removes the entry/key from the tree and modifies the tree as necessary.

openTreeScan(...) 
---> This function, which initializes the scan.
--> This function sets up the ScanManager structure, which stores additional data needed to carry out the scan process. 
--> If the B+ Tree's root node is NULL, an error code is returned. RC_NO_RECORDS_TO_SCAN.


nextEntry(...):
--->The B+ Tree entries are navigated using the function The "result" option points to a memory address where the record details, or RID, are stored.
--->When all entries have been examined and no more entries are available, we return an error code. RC_IM_NO_MORE_ENTRIES;
--->This is a function used to close a scan operation on a B-tree data structure. The scan operation allows for traversing the B-tree in a particular order and examining its contents.



==================================================

B+ TREE INDEX RELATED FUNCTIONS
=================================================

createBtree(...):
Create a new B+ Tree using the function createBtree(...).
--> It sets the TreeManager structure, which contains more data about our B+ Tree, to its initial state.
--> The buffer manager is initialized, a buffer pool is created with the help of the buffer manager, and a page with the page name "idxId" is created with the help of the storage manager.

openBtree(...): 
---> This function opens a B+ Tree that already exists and is located in the file that the "idxId" parameter specifies.
--> After getting our TreeManager back, we start the Buffer Pool.
--->  This function that opens an existing B-tree data structure that was previously created and persisted to disk. The function typically takes in configuration parameters such as the order of the B-tree and the location of the data files on disk. It then reads the data files from disk, initializes the in-memory data structures for the B-tree, and returns the B-tree object.

 closeBtree(...):
--> It flags every page as dirty so that the Buffer Manager can write it back to disk.
--> The buffer pool is subsequently shut down, freeing up all the assigned resources.
--->  This function that closes an open B-tree data structure and persists any changes made to the data structure back to disk.

deleteBtree(....): 
--> The "idxId" file name that is supplied in the parameter of this method is deleted. For this, Storage Manager is employed.
---> This is a function that deletes an existing B-tree data structure from disk. The function typically takes in the directory where the data files for the B-tree are stored and deletes all of the files associated with the B-tree.

======================================================

INITIALIZE AND SHUTDOWN INDEX MANAGER
=================================================

initIndexManager(...):
--->The index manager is set up using the  function.
--> To initialize the storage manager, we call the function of the storage manager.
--->This function that initializes an index manager for a database system. An index manager is responsible for managing the indexes that are used to optimize database queries. 

shutdownIndexManager(...)
--> This function de-allocates all of the resources assigned to the index manager and shuts off the index manager.It releases all memory and resources that the Index Manager was utilizing.
--> We de-allocate memory by setting the treeManager data structure pointer to NULL and calling the C function free().

========================================================

DEBUGGING AND TEST FUNCTIONS
=========================================

printTree(...): 
--> This command outputs the B+ Tree.
---> This is a function used to print the contents of a B-tree data structure to the console or to a file. The function typically takes in the B-tree object and recursively traverses the tree, printing out the keys and pointers of each node.

===============================================

ACCESS INFORMATION ABOUT OUR B+ TREE
=====================================


getNumNodes(...):
--> Our TreeManager structure's "numNodes" field has this data. We simply return this data, then.

 getNumEntries(...):
---> This function retrieves the total number of entries, records, and keys in our B+ Tree.
--> We keep this data in the "numEntries" variable of our TreeManager structure. We simply return this data, then.

getKeyType(...):
---> This function returns the datatype of the keys currently being stored in our B+ Tree.
--> In the "keyType" variable of our TreeManager structure, we store this data. We simply return this data, then.

===================================================









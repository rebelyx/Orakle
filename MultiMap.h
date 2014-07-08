#ifndef MULTIMAP_INCLUDED
#define MULTIMAP_INCLUDED

#include <string>

class MultiMap
{
private: 
      // To prevent MultiMaps from being copied or assigned, declare these members
      // private and do not implement them. 
    MultiMap(const MultiMap& other); 
    MultiMap& operator=(const MultiMap& rhs); 

	typedef struct LLNode_ {
		LLNode_(unsigned int v) : value(v), next(NULL), previous(NULL) {}
		unsigned int value;
		LLNode_* next;
		LLNode_* previous;
	} LLNode;

	typedef struct BSTNode_ {
		BSTNode_(std::string k) : key(k), values(NULL), leftChild(NULL), rightChild(NULL), parent(NULL) {}
		std::string key;
		LLNode* values;
		BSTNode_* leftChild;
		BSTNode_* rightChild;
		BSTNode_* parent;
	} BSTNode;

	void valuePushback(BSTNode* walker, unsigned int value);
	void treeAddNode(BSTNode* pre, BSTNode* newNode);
	void recursiveDelete(BSTNode* cur);

	BSTNode* m_root;

public: 
      // You must implement this public nested MultiMap::Iterator class
    class Iterator 
    { 
    public: 
        Iterator();  // You must have a default constructor
		Iterator(BSTNode* root);
        bool valid() const; 
        std::string getKey() const; 
        unsigned int getValue() const; 
        bool next(); 
        bool prev();
	private:
		BSTNode* m_map;
		BSTNode* m_iter_key;
		LLNode* m_iter_value;
		bool m_validState;
    };

	MultiMap(); 
	~MultiMap(); 
	void clear(); 
	void insert(std::string key, unsigned int value);
	Iterator findEqual(std::string key) const; 
	Iterator findEqualOrSuccessor(std::string  key) const;
	Iterator findEqualOrPredecessor(std::string key) const;
};

#endif // MULTIMAP_INCLUDED
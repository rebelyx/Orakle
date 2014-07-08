#include "MultiMap.h"
using namespace std;


/* =================================== MultiMap ========================================*/
MultiMap::MultiMap() {
	m_root = NULL;
}

MultiMap::~MultiMap() {
	clear();
}

void MultiMap::clear() {
	recursiveDelete(m_root);
	m_root = NULL;
}

void MultiMap::insert(string key, unsigned int value) {
	BSTNode* walker = m_root;
	BSTNode* pre = NULL;
	while (walker != NULL) {
		pre = walker;
		if (key < walker->key) {
			walker = walker->leftChild;
		} else if (key == walker->key) { // BSTNode exists, just add a new value
			valuePushback(walker, value);
			break;
		} else { // key > walker->key
			walker = walker->rightChild;
		}
	}
	if (walker == NULL) { // BSTNode doesn't exist, create a BSTNode then add a new value
		BSTNode* newTreeNode = new BSTNode(key);
		valuePushback(newTreeNode, value);
		treeAddNode(pre, newTreeNode);
	}
}

void MultiMap::valuePushback(BSTNode* BSTWalker, unsigned int value) {
	LLNode* newValue = new LLNode(value);
	if (BSTWalker->values == NULL) {
		BSTWalker->values = newValue;
	} else {
		LLNode* nodeWalker = BSTWalker->values;
		while (nodeWalker->next != NULL)
			nodeWalker = nodeWalker->next;
		nodeWalker->next = newValue;
		newValue->previous = nodeWalker;
	}
}

void MultiMap::treeAddNode(BSTNode* pre, BSTNode* newTreeNode) {
	if (m_root == NULL) {
		m_root = newTreeNode;
	} else {
		if (newTreeNode->key > pre->key) {
			pre->rightChild = newTreeNode;
		} else { // newTreeNode->key <= pre->key
			pre->leftChild = newTreeNode;
		}
		newTreeNode->parent = pre;
	}
}

void MultiMap::recursiveDelete(BSTNode* cur) {
	if (cur == NULL)
		return;

	recursiveDelete(cur->leftChild);
	recursiveDelete(cur->rightChild);

	while (cur->values != NULL) {
		LLNode* temp = cur->values;
		cur->values = cur->values->next; // next perhaps points to NULL. so no next->previous
		delete temp;
	}
	delete cur;
}

MultiMap::Iterator MultiMap::findEqual(std::string key) const {
	Iterator it = Iterator(this->m_root);
	bool canGoNext = true;
	bool canGoPrev = true;
	while (it.valid()) {
		if (key == it.getKey()) {
			break;
		} else if (key > it.getKey()) {
			if (canGoNext == true) {
				it.next();
				canGoNext = true;
				canGoPrev = false;
			} else {
				it = Iterator();
				break;
			}
		} else { // key < it.getKey()
			if (canGoPrev == true) {
				it.prev();
				canGoPrev = true;
				canGoNext = false;
			} else {
				it = Iterator();
				break;
			}
		}
	}
	return it;
}

MultiMap::Iterator MultiMap::findEqualOrSuccessor(string key) const {
	Iterator it = Iterator(this->m_root);
	bool comeFromGreater = false;
	bool comeFromMinor = false;
	while (it.valid()) {
		if (key == it.getKey()) {
			break;
		} else if (key > it.getKey()) {
			if (comeFromGreater == true) {
				it.next();
				break;
			} else {
				it.next();
				comeFromGreater = false;
				comeFromMinor = true;
			}
		} else { // key < it.getKey()
			if (comeFromMinor == true) {
				break;
			} else {
				Iterator temp = it;
				if (it.prev()) {
					comeFromGreater = true;
					comeFromMinor = false;
				} else {
					it = temp; // change invalid to valid
					break; // last valid node
				}
			}
		}
	}
	return it;
}

MultiMap::Iterator MultiMap::findEqualOrPredecessor(string key) const {
	Iterator it = Iterator(this->m_root);
	bool comeFromGreater = false;
	bool comeFromMinor = false;
	while (it.valid()) {
		if (key == it.getKey()) {
			while (it.valid()) {
				Iterator temp = it;
				if (!it.next() || temp.getKey() != it.getKey()) {
					it = temp;
					break;
				}
			}
			break;
		} else if (key < it.getKey()) {
			if (comeFromMinor == true) {
				it.prev();
				break;
			} else {
				it.prev();
				comeFromGreater = true;
				comeFromMinor = false;
			}
		} else { // key > it.getKey()
			if (comeFromGreater == true) {
				break;
			} else {
				Iterator temp = it;
				if (it.next()) {
					comeFromGreater = false;
					comeFromMinor = true;
				} else {
					it = temp; // change invalid to valid
					break; // last valid node
				}
			}
		}
	}
	return it;
}

/* =================================== Iterator ========================================*/
MultiMap::Iterator::Iterator() {
	m_map = NULL;
	m_iter_key = NULL;
	m_iter_value = NULL;
	m_validState = false;
}

MultiMap::Iterator::Iterator(BSTNode* root) {
	m_map = root;
	m_iter_key = root;
	m_iter_value = m_iter_key->values;
	m_validState = true;
}


bool MultiMap::Iterator::valid() const {
	return m_validState;
}

string MultiMap::Iterator::getKey() const {
	return m_iter_key->key;
}

unsigned int MultiMap::Iterator::getValue() const {
	return m_iter_value->value;
}

bool MultiMap::Iterator::next() {
	if (m_validState == false)
		return false;

	if (m_iter_value->next != NULL) { // same key, more multiple values
		m_iter_value = m_iter_value->next;
	} else if (m_iter_key->rightChild != NULL) { // dif key in right child
		m_iter_key = m_iter_key->rightChild;
		while (m_iter_key->leftChild != NULL) {
			m_iter_key = m_iter_key->leftChild;
		}
		m_iter_value = m_iter_key->values;
	} else if (m_iter_key->parent == NULL) { // last node in tree
		m_validState = false;
		return false;
	} else { // go back to parent
		if (m_iter_key == m_iter_key->parent->leftChild) { // curNode is a leftChild
			m_iter_key = m_iter_key->parent;
			m_iter_value = m_iter_key->values;
		} else { // curNode is a rightChild
			BSTNode* temp = m_iter_key;
			while (m_iter_key->parent != NULL && m_iter_key == m_iter_key->parent->rightChild) {
				m_iter_key = m_iter_key->parent;
			}
			if (m_iter_key->parent == NULL) { // last node
				m_iter_key = temp;
				m_validState = false;
				return false;
			} else { // finish left part
				m_iter_key = m_iter_key->parent;
				m_iter_value = m_iter_key->values;
			}
		}
	}
	return true;
}

bool MultiMap::Iterator::prev() {
	if (m_validState == false)
		return false;

	if (m_iter_value->previous != NULL) { // same key, more multiple values
		m_iter_value = m_iter_value->previous;
	} else if (m_iter_key->leftChild != NULL) { // dif key in left child
		m_iter_key = m_iter_key->leftChild;
		while (m_iter_key->rightChild != NULL) {
			m_iter_key = m_iter_key->rightChild;
		}
		m_iter_value = m_iter_key->values;
	} else if (m_iter_key->parent == NULL) { // last node in tree
		m_validState = false;
		return false;
	} else { // go back to parent
		if (m_iter_key == m_iter_key->parent->rightChild) { // curNode is a rightChild
			m_iter_key = m_iter_key->parent;
			m_iter_value = m_iter_key->values;
		} else { // curNode is a leftChild
			BSTNode* temp = m_iter_key;
			while (m_iter_key->parent != NULL && m_iter_key == m_iter_key->parent->leftChild) {
				m_iter_key = m_iter_key->parent;
			}
			if (m_iter_key->parent == NULL) { // last node
				m_iter_key = temp;
				m_validState = false;
				return false;
			} else { // finish right part
				m_iter_key = m_iter_key->parent;
				m_iter_value = m_iter_key->values;
			}
		}
	}
	return true;
}
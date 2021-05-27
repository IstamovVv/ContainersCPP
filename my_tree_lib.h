#pragma once
#include <utility>

namespace TreeLib
{
	template<class K, class T>
	struct Node
	{
		template<class... Args>
		explicit Node(K&& key, Args&&... args)
			: key(std::forward<K>(key)), value(std::forward<Args>(args)...), height(1), left(nullptr), right(nullptr) {}

		K key;
		T value;
		
		size_t height;

		Node* left;
		Node* right;
	};
	
	template<class K, class T>
	size_t height(Node<K, T>* p)
	{
		return p ? p->height : 0;
	}

	template<class K, class T>
	int bFactor(Node<K, T>* p)
	{
		return height(p->right) - height(p->left);
	}

	template<class K, class T>
	void fixHeight(Node<K, T>* p)
	{
		size_t hl = height(p->left);
		size_t hr = height(p->right);

		p->height = (hl > hr ? hl : hr) + 1;
	}

	template<class K, class T>
	Node<K, T>* rotateRight(Node<K, T>* p)
	{
		Node<K, T>* q = p->left;
		p->left = q->right;
		q->right = p;

		fixHeight(p);
		fixHeight(q);

		return q;
	}

	template<class K, class T>
	Node<K, T>* rotateLeft(Node<K, T>* q)
	{
		Node<K, T>* p = q->right;
		q->right = p->left;
		p->left = q;

		fixHeight(q);
		fixHeight(p);

		return p;
	}

	template<class K, class T>
	Node<K, T>* balance(Node<K, T>* p)
	{
		fixHeight(p);

		if (bFactor(p) == 2)
		{
			if (bFactor(p->right) < 0)
				p->right = rotateRight(p->right);

			return rotateLeft(p);
		}

		if (bFactor(p) == -2)
		{
			if (bFactor(p->left) > 0)
				p->left = rotateLeft(p->left);

			return rotateRight(p);
		}

		return p;
	}

	template<class K, class T>
	Node<K, T>* emplaceHelper(Node<K, T>* p, K&& key, T&& value, Node<K, T>*& to_return)
	{
		if (!p)
			return to_return = new Node<K, T>(std::forward<K>(key), std::forward<T>(value));
			
		if (key < p->key)
		{
			p->left = emplaceHelper(p->left, std::forward<K>(key), std::forward<T>(value), to_return);
		}
		else
		{
			p->right = emplaceHelper(p->right, std::forward<K>(key), std::forward<T>(value), to_return);
		}

		return balance(p);
	}
	
	template<class K, class T, class... Args>
	Node<K, T>* emplace(Node<K, T>*& p, K&& key, Args&&... args)
	{
		T value{ std::forward<Args>(args)... };

		Node<K, T>* to_return = nullptr;
		p = emplaceHelper(p, std::forward<K>(key), std::forward<T>(value), to_return);

		return to_return;
	}
	
	template<class K, class T>
	Node<K, T>* findMin(Node<K, T>* p)
	{
		return p->left ? findMin(p->left) : p;
	}

	template<class K, class T>
	Node<K, T>* removeMin(Node<K, T>* p)
	{
		if (p->left == 0)
			return p->right;

		p->left = removeMin(p->left);

		return balance(p);
	}

	template<class K, class T>
	Node<K, T>* remove(Node<K, T>* p, const K& key)
	{
		if (!p) return nullptr;

		if (key < p->key)
		{
			p->left = remove(p->left, key);
		}
		else if (key > p->key)
		{
			p->right = remove(p->right, key);
		}
		else
		{
			Node<K, T>* q = p->left;
			Node<K, T>* r = p->right;
			delete p;

			if (!r) return q;

			Node<K, T>* min = findMin(r);
			min->right      = removeMin(r);
			min->left       = q;

			return balance(min);
		}

		return balance(p);
	}

	template<class K, class T>
	void removeAll(Node<K, T>* ptr)
	{
		if (!ptr) return;

		if (ptr->left)  removeAll(ptr->left);
		if (ptr->right) removeAll(ptr->right);

		delete ptr;
	}

}

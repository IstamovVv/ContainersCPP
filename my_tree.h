#pragma once
#include "TreeLib.h"

#include <functional>


namespace ist {
	template<class K, class T>
	class Tree
	{
	public:
		using key_type        = K;
		using value_type      = T;

		using reference       = value_type&;
		using pointer         = value_type*;

		using const_reference = const value_type&;
		using const_pointer   = const value_type*;

		using node            = TreeLib::Node<K, T>;
		using node_ptr        = node*;

	public:
		Tree()
			: head_(nullptr), size_(0) {}

		Tree(const Tree& other)
			: head_(nullptr), size_(0)
		{
			other.for_each([this](node_ptr node)
				{
					this->emplace(node->key, node->value);
				});
		}

		~Tree()
		{
			clear();
		}

		template<class K, class... Args>
		node_ptr emplace(K&& key, Args&&... args)
		{
			size_ += 1;
			return TreeLib::emplace(head_, std::forward<K>(key), std::forward<Args>(args)...);
		}

		void erase(const key_type& key)
		{
			size_ -= 1;
			head_ = TreeLib::remove(head_, key);
		}

		void clear()
		{
			TreeLib::removeAll(head_);
			head_ = nullptr;
		}

		size_t size() const
		{
			return size_;
		}

		void for_each(std::function<void(node_ptr)> f)
		{
			postOrder(f, head_);
		}

		void for_each(std::function<void(node_ptr)> f) const
		{
			postOrder(f, head_);
		}

	private:
		void postOrder(std::function<void(node_ptr)> f, node_ptr node)
		{
			if (node == nullptr) return;
			postOrder(f, node->left);
			postOrder(f, node->right);
			f(node);
		}

		void postOrder(std::function<void(node_ptr)> f, node_ptr node) const
		{
			if (node == nullptr) return;
			postOrder(f, node->left);
			postOrder(f, node->right);
			f(node);
		}

	private:
		node_ptr head_;
		size_t   size_;
	};

}

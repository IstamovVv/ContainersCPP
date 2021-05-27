#pragma once
#include <memory>
#include <functional>

namespace ist
{
	template<class List>
	class ConstListIterator
	{
	public:
		using value_type = typename List::value_type;
		using node_ptr   = typename List::node_ptr;
		using reference  = const value_type&;
		using pointer    = const value_type*;

		ConstListIterator() noexcept : ptr{} {}

		explicit ConstListIterator(node_ptr ptr) noexcept
			: ptr{ ptr } {}

		[[nodiscard]] reference operator*() const noexcept
		{
			return ptr->value;
		}

		[[nodiscard]] pointer operator->() const noexcept
		{
			return std::addressof(ptr->value);
		}

		ConstListIterator& operator++() noexcept
		{
			ptr = ptr->next;
			return *this;
		}

		ConstListIterator operator++(int) noexcept
		{
			ConstListIterator tmp = *this;
			++* this;
			return tmp;
		}

		ConstListIterator& operator--() noexcept
		{
			ptr = ptr->prev;
			return *this;
		}

		ConstListIterator operator--(int) noexcept
		{
			ptr = ptr->prev;
			return *this;
		}

		[[nodiscard]] bool operator==(const ConstListIterator& other) const noexcept
		{
			return ptr == other.ptr && !(!visited_ && (visited_ = true));
		}

		[[nodiscard]] bool operator!=(const ConstListIterator& other) const noexcept
		{
			return !(*this == other);
		}

		node_ptr ptr;

	private:
		mutable bool visited_ = false;
	};

	template<class List>
	class ListIterator : public ConstListIterator<List>
	{
	public:
		using my_base = ConstListIterator<List>;

		using value_type = typename List::value_type;
		using node_ptr   = typename List::node_ptr;
		using reference  = value_type&;
		using pointer    = value_type*;

		ListIterator() noexcept {}

		explicit ListIterator(node_ptr ptr) noexcept
			: my_base(ptr) {}

		[[nodiscard]] reference operator*() const noexcept
		{
			return const_cast<reference>(my_base::operator*());
		}

		[[nodiscard]] pointer operator->() const noexcept
		{
			return const_cast<pointer>(my_base::operator->());
		}

		ListIterator& operator++() noexcept
		{
			my_base::operator++();
			return *this;
		}

		ListIterator operator++(int) noexcept
		{
			my_base iterator = *this;
			my_base::operator++();
			return iterator;
		}
	};

	template<class ValueType>
	struct ListNode
	{
		using value_type = ValueType;
		using node_ptr   = ListNode*;

		node_ptr   next;
		node_ptr   prev;
		value_type value;

		template<class... Args>
		explicit ListNode(node_ptr next, node_ptr prev, Args&&... args)
			: next(next), prev(prev), value(std::forward<Args>(args)...) {}

		template<class... Args>
		explicit ListNode(Args&&... args)
			: next(nullptr), prev(nullptr), value(std::forward<Args>(args)...) {}
	};

	template <class ValueType>
	class List
	{
	public:
		using value_type = ValueType;

		using reference  = value_type&;
		using pointer    = value_type*;

		using const_reference = const value_type&;
		using const_pointer	  = const value_type*;

		using node	   = ListNode<value_type>;
		using node_ptr = node*;

	public:
		using const_iterator = ConstListIterator<List>;
		using iterator		 = ListIterator<List>;

	public:
		List() : head_(nullptr), size_(0) {}

	public:
		iterator insert(const_iterator where, const value_type& val)
		{
			return emplace(where, val);
		}

		template<class... Args>
		iterator emplace(const_iterator where, Args&&... args)
		{
			return iterator{ emplace_node(where.ptr, std::forward<Args>(args)...) };
		}

		void push_back(const value_type& value)
		{
			emplace_back(value);
		}

		void push_back(value_type&& value)
		{
			emplace_back(std::move(value));
		}

		template<class... Args>
		void emplace_back(Args&&... args)
		{
			if (!head_)
				create_head(std::forward<Args>(args)...);
			else
				emplace_node(head_->prev, std::forward<Args>(args)...);
		}

		void push_front(const value_type& value)
		{
			emplace_front(value);
		}

		void push_front(value_type&& value)
		{
			emplace_front(std::move(value));
		}

		template<class... Args>
		void emplace_front(Args&&... args)
		{
			if (!head_)
				create_head(std::forward<Args>(args)...);
			else
				head_ = emplace_node(head_->prev, std::forward<Args>(args)...);
		}

		void pop_back()
		{
			erase_node(head_->prev);
		}

		void pop_front()
		{
			erase_node(head_);
		}

		void remove(const value_type& element)
		{
			for_each([this, &element](node_ptr node)
			{
				if (node->value == element)
					erase_node(node);
			});
		}
		
		reference front()
		{
			return head_->value;
		}

		const_reference front() const
		{
			return head_->value;
		}

		reference back()
		{
			return head_->prev->value;
		}

		const_reference back() const
		{
			return head_->prev->value;
		}
		
	private:
		template<class... Args>
		node_ptr emplace_node(const node_ptr where, Args&&... args)
		{
			node_ptr new_node = new node(where->next, where, std::forward<Args>(args)...);
			where->next->prev = new_node;
			where->next = new_node;

			size_ += 1;
			return new_node;
		}

		node_ptr erase_node(node_ptr ptr)
		{
			ptr->prev->next = ptr->next;
			ptr->next->prev = ptr->prev;

			node_ptr next_node = ptr->next;
			if (ptr == head_) head_ = next_node;
			delete ptr;

			size_ -= 1;
			return next_node;
		}

		template<class... Args>
		void create_head(Args&&... args)
		{
			head_ = new node(std::forward<Args>(args)...);
			head_->next = head_;
			head_->prev = head_;
		}
		
	public:
		void for_each(std::function<void(node_ptr)> f)
		{
			node_ptr ptr = head_;
			node_ptr end = head_->prev;

			while (ptr != end)
			{
				node_ptr next = ptr->next;
				f(ptr);
				ptr = next;
			}

			f(end);
		}
		
	public:
		[[nodiscard]] iterator begin() noexcept
		{
			return iterator{ head_ };
		}

		[[nodiscard]] const_iterator cbegin() const noexcept
		{
			return const_iterator{ head_ };
		}

		[[nodiscard]] iterator end() noexcept
		{
			return iterator{ head_ };
		}

		[[nodiscard]] const_iterator cend() const noexcept
		{
			return const_iterator{ head_ };
		}

		[[nodiscard]] const_iterator begin() const noexcept
		{
			return cbegin();
		}

		[[nodiscard]] const_iterator end() const noexcept
		{
			return cend();
		}
		
	private:
		node_ptr  head_;
		size_t    size_;
	};

}

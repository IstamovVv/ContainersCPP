#pragma once
#include <iostream>
#include <cassert>
#include <utility>
#include <vector>

namespace ist
{
	template<class OpenHashTable>
	class const_iterator
	{
	public:
		using data_pair  = typename OpenHashTable::data_pair;
		using value_type = typename OpenHashTable::value_type;
		using state		 = typename OpenHashTable::state;

		using value_ptr  = const value_type*;
		using value_ref  = const value_type&;

		using data_ptr   = const data_pair*;
		using data_ref   = const data_pair&;

	public:
		const_iterator() noexcept : ptr_(), end_() {}

		explicit const_iterator(data_ptr ptr, data_ptr end) noexcept
			: ptr_(ptr), end_(end) {}

		[[nodiscard]] value_ref operator*() const noexcept
		{
			return *operator->();
		}

		[[nodiscard]] value_ptr operator->() const noexcept
		{
			assert("operator->: " && ptr_ != end_);
			return &ptr_->first;
		}

		const_iterator& operator++() noexcept
		{
			assert("operator++(): " && ptr_ != end_);

			ptr_ += 1;
			while (ptr_ != end_ && ptr_->second != state::occupied)
				ptr_ += 1;

			return *this;
		}

		const_iterator operator++(int) noexcept
		{
			const_iterator iterator = *this;
			++(*this);
			return iterator;
		}

		[[nodiscard]] bool operator==(const const_iterator& other) const noexcept
		{
			return ptr_ == other.ptr_;
		}

		[[nodiscard]] bool operator!=(const const_iterator& other) const noexcept
		{
			return !(*this == other);
		}

	private:
		data_ptr ptr_;
		data_ptr end_;
	};

	template<class OpenHashTable>
	class iterator : public const_iterator<OpenHashTable>
	{
	public:
		using MyBase = const_iterator<OpenHashTable>;

		using data_pair  = typename OpenHashTable::data_pair;
		using value_type = typename OpenHashTable::value_type;
		using state		 = typename OpenHashTable::state;

		using value_ptr = value_type*;
		using value_ref = value_type&;

		using data_ptr = data_pair*;
		using data_ref = data_pair&;

		iterator() noexcept {}

		explicit iterator(data_ptr ptr, data_ptr end) noexcept
			: MyBase(ptr, end) {}

		[[nodiscard]] value_ref operator*() const noexcept
		{
			return const_cast<value_ref>(MyBase::operator*());
		}

		[[nodiscard]] value_ptr operator->() const noexcept
		{
			return const_cast<value_ptr>(MyBase::operator->());
		}

		iterator& operator++() noexcept
		{
			MyBase::operator++();
			return *this;
		}

		iterator operator++(int) noexcept
		{
			const_iterator iterator = *this;
			MyBase::operator++();
			return iterator;
		}
	};

	template<class K, class V, class Hasher = std::hash<K>, class Keyeq = std::equal_to<K>, class Allocator = std::allocator<std::pair<const K, V>>>
	class OpenHashTable
	{
	public:
		enum class state
		{
			empty,
			deleted,
			occupied
		};

		using key_type       = K;
		using mapped_type    = V;
		using value_type     = std::pair<const K, V>;
		using data_pair      = std::pair<value_type, state>;
		using hasher	     = Hasher;
		using key_equal      = Keyeq;
		using allocator_type = Allocator;

		using iterator		 = iterator<OpenHashTable>;
		using const_iterator = const_iterator<OpenHashTable>;

	protected:
		using Alloc		     = typename std::allocator_traits<Allocator>::template rebind_alloc<data_pair>;
		using Alloc_traits   = std::allocator_traits<Alloc>;

	public:
		explicit OpenHashTable(size_t capacity, const allocator_type& alloc = Allocator{}) :
			data_(capacity, alloc),
			data_end_(data_.data() + data_.size()),
			free_cells_(capacity),
			key_equal_(key_equal()),
			alloc_(alloc) {}

		OpenHashTable() : OpenHashTable(8) {}

		OpenHashTable(const OpenHashTable& other) :
			data_(other.data_, Alloc_traits::select_on_container_copy_construction(other.alloc_)),
			data_end_(other.data_end_),
			free_cells_(other.free_cells_),
			key_equal_(other.key_equal_),
			alloc_(Alloc_traits::select_on_container_copy_construction(other.alloc_)) {}

		OpenHashTable(OpenHashTable&& other) noexcept :
			data_(std::move(other.data_), std::move(other.alloc_)),
			data_end_(std::move(other.data_end_)),
			free_cells_(std::move(other.free_cells_)),
			key_equal_(std::move(other.key_equal_)),
			alloc_(std::move(other.alloc_)) {}

		OpenHashTable& operator=(const OpenHashTable& other)
		{
			if (this == &other) return *this;

			data_end_ = other.data_end_;
			free_cells_ = other.free_cells_;
			key_equal_ = other.key_equal_;

			if (!Alloc_traits::is_always_equal::value)
			{
				if (alloc_ != other.alloc_)
				{
					if (Alloc_traits::propagate_on_container_copy_assignment::value)
					{
						alloc_ = other.alloc_;
					}
				}
			}
			
			data_ = other.data_;
			return *this;
		}

		OpenHashTable& operator=(OpenHashTable&& other)
		{
			if (this == &other) return *this;

			data_end_ = other.data_end_;
			free_cells_ = other.free_cells_;
			key_equal_ = other.key_equal_;

			if (!Alloc_traits::is_always_equal::value)
			{
				if (alloc_ != other.alloc_)
				{
					if (Alloc_traits::propagate_on_container_move_assignment::value)
					{
						alloc_ = std::move(other.alloc_);
					}
				}
			}

			data_ = std::move(other);
			return *this;
		}

	public:
		void rehash_table()
		{
			free_cells_ = data_.size() * 2;

			std::vector<data_pair> data_buffer(free_cells_);
			std::swap(data_buffer, data_);

			for (size_t i = 0; i < data_buffer.size(); ++i)
				if (data_buffer[i].second == state::occupied)
					insert(data_buffer[i].first);

			_update_end();
		}

		template<class... Args>
		std::pair<iterator, bool> emplace(Args&&... args)
		{
			value_type pair(std::forward<Args>(args)...);

			if (free_cells_ == 0)
				rehash_table();

			size_t pos = hash(pair.first);
			size_t i = 0;

			while (data_[pos].second != state::empty && data_[pos].second != state::deleted && i <= max_iterations)
			{
				if (key_equal_(data_[pos].first.first, pair.first))
					return std::pair(iterator(&data_[pos], data_end_), false);

				pos = rehash(pos, i++);
			}

			if (i >= max_iterations) return std::pair(end(), false);

			free_cells_ -= 1;

			Alloc_traits::destroy(alloc_, &data_[pos]);
			Alloc_traits::construct(alloc_, &data_[pos], std::pair(std::move(pair), state::occupied));

			return std::pair(iterator(&data_[pos], data_end_), true);
		}
		
		std::pair<iterator, bool> insert(const value_type& value)
		{
			return emplace(value);
		}

		std::pair<iterator, bool> insert(value_type&& value)
		{
			return emplace(std::move(value));
		}

		bool erase(const key_type& key) 
		{
			size_t pos = hash(key);
			size_t i = 0;

			while (data_[pos].second != state::empty && i <= max_iterations)
			{
				if (key_equal_(data_[pos].first.first, key))
				{
					data_[pos].second = state::deleted;
					free_cells_ += 1;
					return true;
				}

				pos = rehash(pos, i++);
			}

			return false;
		}

		bool erase(iterator it)
		{
			return erase(it->first);
		}

		void clear()
		{
			for (auto& elem : data_)
				elem.second = state::deleted;

			rehash_table();
		}
		
		iterator find(const key_type& key) noexcept
		{
			size_t pos = hash(key);
			size_t i = 0;

			while (data_[pos].second != state::empty && i <= max_iterations)
			{
				if (key_equal_(data_[pos].first.first, key))
					return iterator(&data_[pos], data_end_);

				pos = rehash(pos, i++);
			}

			return end();
		}

		const_iterator find(const key_type& key) const noexcept
		{
			size_t pos = hash(key);
			size_t i = 0;

			while (data_[pos].second != state::empty && i <= max_iterations)
			{
				if (key_equal_(data_[pos].first.first, key))
					return const_iterator(&data_[pos], data_end_);

				pos = rehash(pos, i++);
			}

			return end();
		}

		mapped_type& at(const key_type& key)
		{
			iterator it = find(key);

			if (it != end())
				return it->second;

			throw std::out_of_range("invalid OpenHashTable<K, V> key");
		}

		mapped_type at(const key_type& key) const
		{
			const_iterator it = find(key);

			if (it != end())
				return it->second;

			throw std::out_of_range("invalid OpenHashTable<K, V> key");
		}

		mapped_type& operator[](const key_type& key)
		{
			iterator it = find(key);

			if (it != end())
				return it->second;

			return emplace(key, mapped_type()).first->second;
		}

		mapped_type operator[](const key_type& key) const
		{
			const_iterator it = find(key);

			if (it != end())
				return it->second;

			return emplace(key, mapped_type()).first->second;
		}

		size_t size() const noexcept { return data_.size(); }

		size_t hash(const key_type& key) const
		{
			static hasher hash_key;
			return hash_key(key) % data_.size();
		}

		size_t rehash(const size_t hashed_key, const size_t i) const noexcept
		{
			return (hashed_key + c_coef * i + d_coef * i * i) % size();
		}

	private:
		std::vector<data_pair, Alloc>  data_;
		data_pair* data_end_;
		size_t	   free_cells_;
		key_equal  key_equal_;
		Alloc      alloc_;

	private:
		size_t c_coef = 1;
		size_t d_coef = 1;
		size_t max_iterations = 10;

	private:
		void _update_end()
		{
			data_end_ = data_.data() + data_.size();
		}
		
	public:
		[[nodiscard]] iterator begin() noexcept
		{
			size_t index = 0;
			while (index < data_.size() && data_[index].second != state::occupied)
				index += 1;

			if (index == data_.size())
				return iterator(data_end_, data_end_);
			
			return iterator(&data_[index], data_end_);
		}

		[[nodiscard]] const_iterator cbegin() const noexcept
		{
			size_t index = 0;
			while (index < data_.size() && data_[index].second != state::occupied)
				index += 1;

			if (index == data_.size())
				return const_iterator(data_end_, data_end_);
			
			return const_iterator(&data_[index], data_end_);
		}

		[[nodiscard]] iterator end() noexcept
		{
			return iterator(data_end_, data_end_);
		}

		[[nodiscard]] const_iterator cend() const noexcept
		{
			return const_iterator(data_end_, data_end_);
		}

		[[nodiscard]] const_iterator begin() const noexcept
		{
			return cbegin();
		}

		[[nodiscard]] const_iterator end() const noexcept
		{
			return cend();
		}
	};
}

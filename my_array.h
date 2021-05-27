#pragma once
#include <cassert>
#include "Last_lab.h"

namespace ist
{
	template<class T, size_t Size>
	class Array_const_iterator
	{
	public:
		using value_type      = T;
		using pointer_type    = const value_type*;
		using reference_type  = const value_type&;
		using difference_type = ptrdiff_t;

	public:
		constexpr Array_const_iterator() noexcept : ptr_(), idx_(0) {}

		constexpr explicit Array_const_iterator(pointer_type ptr, size_t offset = 0) noexcept
			: ptr_(ptr), idx_(offset) {}

		[[nodiscard]] constexpr reference_type operator*() const noexcept
		{
			return *operator->();
		}

		[[nodiscard]] constexpr pointer_type operator->() const noexcept
		{
			assert(ptr_ && "cannot dereference value-initialized array iterator");
			assert(idx_ < Size && "cannot dereference out of range array iterator");

			return ptr_ + idx_;
		}

		constexpr Array_const_iterator& operator++() noexcept
		{
			assert(ptr_ && "cannot increment value-initialized array iterator");
			assert(idx_ < Size && "cannot increment array iterator past end");

			++idx_;
			return *this;
		}

		constexpr Array_const_iterator operator++(int) noexcept
		{
			Array_const_iterator iterator = *this;
			++(*this);
			return iterator;
		}

		constexpr Array_const_iterator& operator--() noexcept
		{
			assert(ptr_ && "cannot increment value-initialized array iterator");
			assert(idx_ != 0 && "cannot increment array iterator past end");

			--idx_;
			return *this;
		}

		constexpr Array_const_iterator operator--(int) noexcept
		{
			Array_const_iterator iterator = *this;
			--(*this);
			return iterator;
		}

		constexpr void verify_offset_(const ptrdiff_t off) const noexcept
		{
			if (off != 0)
				assert(ptr_ && "cannot seek array iterator");
			
			if (off < 0)
				assert(idx_ >= static_cast<size_t>(-off) && "cannot seek array iterator before begin");

			if (off > 0)
				assert(Size - idx_ >= static_cast<size_t>(off) && "cannot seek array iterator after end");
		}

		constexpr Array_const_iterator& operator+=(const ptrdiff_t off) noexcept
		{
			verify_offset_(off);
			idx_ += off;
			return *this;
		}

		[[nodiscard]] constexpr Array_const_iterator operator+(const ptrdiff_t off) const noexcept
		{
			Array_const_iterator iterator = *this;
			return this += off;
		}

		constexpr Array_const_iterator& operator-=(const ptrdiff_t off) noexcept
		{
			return *this += -off;
		}

		[[nodiscard]] constexpr Array_const_iterator operator-(const ptrdiff_t off) const noexcept
		{
			Array_const_iterator iterator = *this;
			return iterator -= off;
		}
		
		[[nodiscard]] constexpr bool operator==(const Array_const_iterator& other) const noexcept
		{
			assert(ptr_ == other.ptr_ && "array iterators incompatible");
			return idx_ == other.idx_;
		}

		[[nodiscard]] constexpr bool operator!=(const Array_const_iterator& other) const noexcept
		{
			return !(*this == other);
		}

		[[nodiscard]] constexpr bool operator<(const Array_const_iterator& other) const noexcept
		{
			assert(ptr_ == other.ptr_ && "array iterators incompatible");
			return idx_ < other.idx_;
		}

		[[nodiscard]] constexpr bool operator>(const Array_const_iterator& other) const noexcept
		{
			return other < *this;
		}

		[[nodiscard]] constexpr bool operator<=(const Array_const_iterator& other) const noexcept
		{
			return !(other < *this);
		}

		[[nodiscard]] constexpr bool operator>=(const Array_const_iterator& other) const noexcept
		{
			return !(*this < other);
		}
	private:
		pointer_type ptr_;
		size_t       idx_;
	};

	template<class T, size_t Size>
	class Array_iterator : public Array_const_iterator<T, Size>
	{
	public:
		using MyBase	     = Array_const_iterator<T, Size>;

		using value_type	 = T;
		using pointer_type	 = value_type*;
		using reference_type = value_type&;

		constexpr Array_iterator() noexcept {}

		constexpr explicit Array_iterator(pointer_type ptr, size_t offset = 0) noexcept
			: MyBase(ptr, offset) {}

		[[nodiscard]] constexpr reference_type operator*() const noexcept
		{
			return const_cast<reference_type>(MyBase::operator*());
		}

		[[nodiscard]] constexpr pointer_type operator->() const noexcept
		{
			return const_cast<pointer_type>(MyBase::operator->());
		}

		constexpr Array_iterator& operator++() noexcept
		{
			MyBase::operator++();
			return *this;
		}

		constexpr Array_iterator operator++(int) noexcept
		{
			Array_iterator iterator = *this;
			MyBase::operator++();
			return iterator;
		}

		constexpr Array_iterator& operator--() noexcept
		{
			MyBase::operator--();
			return *this;
		}

		constexpr Array_iterator operator--(int) noexcept
		{
			Array_iterator iterator = *this;
			MyBase::operator--();
			return iterator;
		}

		constexpr Array_iterator& operator+=(const ptrdiff_t off) noexcept
		{
			MyBase::operator+=(off);
			return *this;
		}

		[[nodiscard]] constexpr Array_iterator operator+(const ptrdiff_t off) const noexcept
		{
			Array_iterator iterator = *this;
			return iterator += off;
		}

		constexpr Array_iterator& operator-=(const ptrdiff_t off) noexcept
		{
			MyBase::operator-=(off);
			return *this;
		}

		[[nodiscard]] constexpr Array_iterator operator-(const ptrdiff_t off) const noexcept
		{
			Array_iterator iterator = *this;
			return iterator -= off;
		}
	};

	template<class T, size_t Size>
	class Array
	{
	public:
		using value_type	  = T;
		using reference		  = T&;
		using pointer		  = T*;
		using const_reference = const T&;
		using const_pointer   = const T*;
		using iterator		  = Array_iterator<T, Size>;
		using const_iterator  = Array_const_iterator<T, Size>;

		[[nodiscard]] constexpr size_t size() const noexcept { return Size; }

		reference operator[](size_t index)
		{
			if (index >= Size) throw ist::out_of_range("[[Error]]: Array subscript out of range");

			return data[index];
		}

		const_reference operator[](size_t index) const
		{
			if (index >= Size) throw ist::out_of_range("[[Error]]: Array subscript out of range");

			return data[index];
		}

		void print() const noexcept
		{
			std::cout << "----------Print array----------" << std::endl;

			for (const auto& elem : data)
				std::cout << elem << " ";

			std::cout << std::endl;

			std::cout << "----------Print array end----------" << std::endl;
		}

		[[nodiscard]] iterator begin() noexcept { return iterator{ data, 0 }; }
		[[nodiscard]] iterator end()   noexcept { return iterator{ data, Size }; }

		[[nodiscard]] const_iterator begin() const noexcept { return const_iterator{ data, 0 }; }
		[[nodiscard]] const_iterator end()   const noexcept { return const_iterator{ data, Size }; }

		[[nodiscard]] const_iterator cbegin() const noexcept { return begin(); }
		[[nodiscard]] const_iterator cend()   const noexcept { return end(); }

		T data[Size];
	};

	template<class T>
	class Array<T, 0>
	{
	public:
		using value_type	  = T;
		using reference		  = T&;
		using pointer		  = T*;
		using const_reference = const T&;
		using const_pointer   = const T*;
		using iterator		  = Array_iterator<T, 0>;
		using const_iterator  = Array_const_iterator<T, 0>;

		reference operator[](size_t) noexcept
		{
			// throw std::out_of_range("array subscript out of range");
			return data[0];
		}

		const_reference operator[](size_t) const noexcept
		{
			// throw std::out_of_range("array subscript out of range");
			return data[0];
		}
		
		[[nodiscard]] iterator begin() noexcept { return iterator{}; }
		[[nodiscard]] iterator end()   noexcept { return iterator{}; }

		[[nodiscard]] const_iterator begin() const noexcept { return const_iterator{}; }
		[[nodiscard]] const_iterator end()   const noexcept { return const_iterator{}; }

		[[nodiscard]] const_iterator cbegin() const noexcept { return begin(); }
		[[nodiscard]] const_iterator cend()   const noexcept { return end(); }

		T data[1];
	};
}

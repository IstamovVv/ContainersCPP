#pragma once

namespace ist {
	
	template<typename vector>
	class vector_iterator
	{
	public:
		using value_type = typename vector::value_type;
		using pointer_type = value_type*;
		using reference_type = value_type&;
	public:
		vector_iterator(pointer_type ptr)
			: m_Ptr(ptr) {};

		vector_iterator& operator++()
		{
			m_Ptr++;
			return *this;
		}

		vector_iterator operator++(int)
		{
			vector_iterator iterator = *this;
			++(*this);
			return iterator;
		}

		vector_iterator& operator--()
		{
			m_Ptr--;
			return *this;
		}

		vector_iterator operator--(int)
		{
			vector_iterator iterator = *this;
			--(*this);
			return iterator;
		}

		reference_type operator[](int index)
		{
			return *(m_Ptr + index);
		}

		pointer_type operator->()
		{
			return m_Ptr;
		}

		reference_type operator*()
		{
			return *m_Ptr;
		}

		bool operator==(const vector_iterator& other) const
		{
			return m_Ptr == other.m_Ptr;
		}

		bool operator!=(const vector_iterator& other) const
		{
			return !(*this == other); 
		}
		
	private:
		pointer_type m_Ptr;
	};
	
	template<typename T>
	class vector
	{
	public:
		using value_type = T;
		using iterator = vector_iterator<vector<T>>;
	public:
		vector()
		{
			//allocate
			realloc(2);
		}

		vector(const vector& other)
		{
			m_size_ = other.m_size_;
			m_capacity_ = other.m_capacity_;

			m_data_ = (T*)::operator new(m_capacity_ * sizeof(T));

			for (size_t i = 0; i < m_size_; i++)
				new (&m_data_[i]) T(std::move(other.m_data_[i]));
			
		}

		vector& operator=(const vector& other)
		{
			m_size_ = other.m_size_;
			m_capacity_ = other.m_capacity_;

			m_data_ = (T*)::operator new(m_capacity_ * sizeof(T));

			for (size_t i = 0; i < m_size_; i++)
				new (&m_data_[i]) T(std::move(other.m_data_[i]));

			return *this;
		}

		~vector()
		{
			clear();
			::operator delete(m_data_, m_capacity_ * sizeof(T));
		}

		void push_back(const T& value)
		{
			if (m_size_ >= m_capacity_)
				realloc(m_capacity_ + m_capacity_ / 2);
			
			m_data_[m_size_] = value;
			m_size_++;
		}

		void push_back(T&& value)
		{
			std::cout << "TEST: " << std::endl;
			std::cout << "OLD CAPACITY " << m_capacity_ << std::endl;
			std::cout << "NEW CAPACITY " << m_capacity_ + m_capacity_ / 2 << std::endl;
			if (m_size_ >= m_capacity_)
				realloc(m_capacity_ + m_capacity_ / 2);

			m_data_[m_size_] = std::move(value);
			m_size_++;
		}
		 
		template<typename... Args>
		T& emplace_back(Args&&... args)
		{
			if (m_size_ >= m_capacity_)
				realloc(m_capacity_ + m_capacity_ / 2);

			new(&m_data_[m_size_]) T(std::forward<Args>(args)...);
			return m_data_[m_size_++];
		}

		void pop_back()
		{
			if (m_size_ > 0)
			{
				m_size_--;
				m_data_[m_size_].~T();
			}
		}

		void insert(const T& value, size_t index)
		{
			if (index >= m_size_) return;
			
			if (m_size_ >= m_capacity_)
				realloc(m_capacity_ + m_capacity_ / 2);
			
			T b1 = m_data_[index];
			T b2;

			m_data_[index] = value;
			
			for (index++; index < m_size_; index++)
			{
				b2 = m_data_[index];
				m_data_[index] = b1;
				b1 = b2;
			}
			m_data_[index] = b1;

			m_size_++;
		}

		void pull_out(size_t index)
		{
			if (index >= m_size_) return;

			m_data_[index].~T();

			while (index < m_size_ - 1)
			{
				m_data_[index] = m_data_[index + 1];
				index++;
			}

			m_size_--;
		}
		
		void clear()
		{
			for (size_t i = 0; i < m_size_; i++)
				m_data_[i].~T();

			m_size_ = 0;
		}
		
		const T& operator[](size_t index) const
		{
			return m_data_[index];
		}

		T& operator[](size_t index)
		{
			return m_data_[index];
		}
		
		size_t size() const { return m_size_; }

		iterator begin()
		{
			return iterator(m_data_);
		}

		iterator end()
		{
			return iterator(m_data_ + m_size_);
		}
		
	private:
		void realloc(size_t new_capacity)
		{
			T* newBlock = (T*)::operator new(new_capacity * sizeof(T));

			if (new_capacity < m_size_)
				m_size_ = new_capacity;
			
			for (size_t i = 0; i < m_size_; i++)
				new (&newBlock[i]) T(std::move(m_data_[i]));
			//  newBlock[i] = std::move(m_data_[i]); does not run a constructor
			
			for (size_t i = 0; i < m_size_; i++)
				m_data_[i].~T();

			::operator delete(m_data_, m_capacity_ * sizeof(T));
			m_data_ = newBlock;
			m_capacity_ = new_capacity;
		}
	private:
		T* m_data_ = nullptr;

		size_t m_size_ = 0;
		size_t m_capacity_ = 0;
	};
};
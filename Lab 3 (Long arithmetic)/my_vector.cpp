#include "my_vector.h"

my_vector::my_vector()
{
	size_ = 0;
	capacity_ = 1;
	data_ = (uint32_t *)std::calloc(1, sizeof(*data_));
	if (data_ == nullptr)
		throw std::bad_alloc();
}

my_vector::my_vector(uint32_t s)
{
	size_ = s;
	capacity_ = s;
	data_ = (uint32_t *)std::calloc(s, sizeof(*data_));
	if (data_ == nullptr)
		throw std::bad_alloc();
}

my_vector::my_vector(uint32_t s, uint32_t val)
{
	size_ = s;
	capacity_ = s;
	data_ = (uint32_t *)std::calloc(s, sizeof(*data_));
	if (data_ == nullptr)
		throw std::bad_alloc();
	for (int i = 0; i < s; ++i)
		data_[i] = val;
}

my_vector::~my_vector()
{
	if (data_ != nullptr)
		std::free(data_);
	data_ = nullptr;
	size_ = 0;
	capacity_ = 0;
}

my_vector::my_vector(const my_vector &other)
{
	if (this == &other)
		return;
	size_ = other.size_;
	capacity_ = other.capacity_;
	data_ = (uint32_t *)std::calloc(capacity_, sizeof(*data_));
	if (data_ == nullptr)
		throw std::bad_alloc();
	for (int i = 0; i < size_; ++i)
		data_[i] = other.data_[i];
};

my_vector &my_vector::operator=(const my_vector &other)
{
	if (this == &other)
		return *this;
	size_ = other.size_;
	capacity_ = other.capacity_;
	if (data_ != nullptr)
		std::free(data_);
	data_ = (uint32_t *)std::calloc(capacity_, sizeof(*data_));
	if (data_ == nullptr)
		throw std::bad_alloc();
	for (int i = 0; i < size_; ++i)
		data_[i] = other.data_[i];
	return *this;
};

my_vector &my_vector::operator=(my_vector &&other)
{
	size_ = other.size_;
	capacity_ = other.capacity_;
	if (data_ != nullptr)
		std::free(data_);
	data_ = other.data_;
	other.data_ = nullptr;
	return *this;
};

uint32_t &my_vector::operator[](uint32_t i) const
{
	return this->data_[i];
}

void my_vector::push_back(uint32_t val)
{
	if (size_ == capacity_)
	{
		this->reserve(capacity_ * 2);
	}
	data_[size_] = val;
	size_++;
}

void my_vector::resize(uint32_t s, uint32_t val)
{
	int old_size = size_;
	if (s > capacity_)
	{
		this->reserve(s);
	}
	size_ = s;
	for (int i = old_size; i < s; ++i)
		data_[i] = val;
}

uint32_t my_vector::size() const
{
	return size_;
}

void my_vector::reserve(uint32_t cap)
{
	if (cap <= capacity_)
		return;
	data_ = (uint32_t *)std::realloc(data_, cap * sizeof(*data_));
	if (data_ == nullptr)
		throw std::bad_alloc();
	capacity_ = cap;
}

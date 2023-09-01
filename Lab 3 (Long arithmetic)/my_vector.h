#pragma once

#include "return_codes.h"
#include <string_view>

#include <cstdlib>

class my_vector
{
  public:
	my_vector();
	my_vector(uint32_t);
	my_vector(uint32_t, uint32_t);
	my_vector(const my_vector&);
	~my_vector();
	my_vector& operator=(const my_vector& x);
	my_vector& operator=(my_vector&& x);
	uint32_t& operator[](uint32_t) const;
	void push_back(uint32_t);
	void resize(uint32_t, uint32_t);
	uint32_t size() const;

  private:
	uint32_t size_;
	uint32_t capacity_;
	uint32_t* data_;
	void reserve(uint32_t);
};

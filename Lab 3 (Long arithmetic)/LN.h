#pragma once

#include "my_vector.h"
#include "return_codes.h"

#include <cstdlib>
#include <iomanip>

class LN
{
  public:
	LN(const long long num = 0ll);
	LN(std::string_view& str);
	LN(const char* str);
	LN(const LN& other);
	LN(LN&& other);

	~LN();

	LN& operator=(const LN& x);
	LN& operator=(LN&& x);

	LN operator+(const LN&) const;
	LN operator-(const LN&) const;
	LN operator*(const LN&) const;
	LN operator/(const LN&) const;
	LN operator%(const LN&) const;
	LN operator-() const;
	LN operator~() const;

	LN& operator+=(const LN&);
	LN& operator-=(const LN&);
	LN& operator*=(const LN&);
	LN& operator/=(const LN&);
	LN& operator%=(const LN&);

	LN operator==(const LN& x) const noexcept;
	LN operator!=(const LN& x) const noexcept;
	LN operator>=(const LN& x) const noexcept;
	LN operator<=(const LN& x) const noexcept;
	LN operator>(const LN& x) const noexcept;
	LN operator<(const LN& x) const noexcept;

	operator long long() const;
	operator bool() const;

	friend std::ostream& operator<<(std::ostream& out, const LN& x);

  private:
	my_vector m_chunks;
	static const uint64_t m_base = 4294967296;
	bool m_is_negative = false;
	bool m_is_NaN = false;
	int equality(const LN& a) const noexcept;
	LN multiply_ln_number(uint32_t, uint32_t) const;
};

LN operator""_ln(const char*);

#include "LN.h"

LN::LN(const LN& other)
{
	if (this == &other)
		return;
	this->m_is_NaN = other.m_is_NaN;
	this->m_is_negative = other.m_is_negative;
	this->m_chunks = other.m_chunks;
}

LN& LN::operator=(const LN& other)
{
	if (this == &other)
		return *this;
	this->m_is_NaN = other.m_is_NaN;
	this->m_is_negative = other.m_is_negative;
	this->m_chunks = other.m_chunks;
	return *this;
}

LN::LN(std::string_view& str)
{
	bool is_str_negative = (str[0] == '-');
	this->m_chunks = my_vector(1, 0);
	for (int i = (is_str_negative ? 1 : 0); i < str.length(); i++)
	{
		size_t num;
		if (str[i] >= '0' && str[i] <= '9')
		{
			num = str[i] - '0';
		}
		else if (str[i] >= 'A' && str[i] <= 'F')
		{
			num = str[i] - 'A' + 10;
		}
		else if (str[i] >= 'a' && str[i] <= 'f')
		{
			num = str[i] - 'a' + 10;
		}
		else
		{
			m_is_NaN = true;
			break;
		}
		*this = this->multiply_ln_number(16, 0);
		*this += LN((long long)num);
	}
	this->m_is_negative = is_str_negative;
}

LN::LN(const char* str)
{
	bool is_str_negative = (str[0] == '-');
	this->m_chunks = my_vector(1, 0);
	int i = is_str_negative ? 1 : 0;
	while (str[i] != 0)
	{
		size_t num;
		if (str[i] >= '0' && str[i] <= '9')
		{
			num = str[i] - '0';
		}
		else if (str[i] >= 'A' && str[i] <= 'F')
		{
			num = str[i] - 'A' + 10;
		}
		else if (str[i] >= 'a' && str[i] <= 'f')
		{
			num = str[i] - 'a' + 10;
		}
		else
		{
			m_is_NaN = true;
			break;
		}
		*this = this->multiply_ln_number(16, 0);
		*this += LN((long long)num);
		i++;
	}
	this->m_is_negative = is_str_negative;
}

LN::LN(LN&& x)
{
	if (this == &x)
		return;
	this->m_chunks = std::move(x.m_chunks);
	this->m_is_negative = x.m_is_negative;
	this->m_is_NaN = x.m_is_NaN;
}

LN& LN::operator=(LN&& x)
{
	if (this == &x)
		return *this;
	this->m_chunks = std::move(x.m_chunks);
	this->m_is_negative = x.m_is_negative;
	this->m_is_NaN = x.m_is_NaN;
	return *this;
}

LN LN::multiply_ln_number(uint32_t num, uint32_t offset) const
{
	LN result;
	result.m_chunks.resize(this->m_chunks.size() + offset, 0);
	uint32_t carry = 0;
	for (long long i = offset; i < this->m_chunks.size() + offset; i++)
	{
		uint64_t val = (uint64_t)this->m_chunks[i - offset] * num + carry;
		carry = val / (uint64_t)m_base;
		result.m_chunks[i] = val % m_base;
	}
	if (carry != 0)
	{
		result.m_chunks.push_back(carry);
	}
	return result;
}

LN::LN(const long long num)
{
	m_is_NaN = false;
	if (num < 0)
	{
		m_is_negative = true;
	}
	else
	{
		m_is_negative = false;
	}
	if (num == 0)
	{
		m_chunks.push_back(0);
	}
	else
	{
		long long num1 = num / ((long long)m_base), num2 = num % ((long long)m_base);
		if (num1 < 0)
		{
			num1 *= -1;
		}
		if (num2 < 0)
		{
			num2 *= -1;
		}
		m_chunks.push_back((uint32_t)num2);
		if (num1 != 0)
		{
			m_chunks.push_back((uint32_t)num1);
		}
	}
}

LN LN::operator-() const
{
	LN result = *this;
	result.m_is_negative = !this->m_is_negative;
	return result;
}

LN LN::operator+(const LN& a) const
{
	LN result = *this;
	result += a;
	return result;
}

LN LN::operator*(const LN& a) const
{
	LN result;
	if (a.m_is_NaN || this->m_is_NaN)
	{
		result.m_is_NaN = true;
		return result;
	}
	for (int i = 0; i < a.m_chunks.size(); i++)
	{
		result += this->multiply_ln_number(a.m_chunks[i], i);
	}
	result.m_is_negative = (this->m_is_negative != a.m_is_negative);
	return result;
}

LN& LN::operator*=(const LN& a)
{
	*this = *this * a;
	return *this;
}

LN& LN::operator+=(const LN& a)
{
	if (a.m_is_NaN || this->m_is_NaN)
	{
		this->m_is_NaN = true;
		return *this;
	}
	uint64_t buf = 0;
	uint64_t carry = 0;
	if (this->m_is_negative == a.m_is_negative)
	{
		this->m_chunks.resize(std::max(a.m_chunks.size(), this->m_chunks.size()), 0);
		for (int i = 0; i < this->m_chunks.size(); ++i)
		{
			buf = (uint64_t)this->m_chunks[i] + (a.m_chunks.size() > i ? a.m_chunks[i] : 0) + carry;
			this->m_chunks[i] = buf % m_base;
			carry = buf / m_base;
		}
		if (carry != 0)
			this->m_chunks.push_back(carry);
	}
	else
	{
		return *this -= -a;
	}
	return *this;
}

LN& LN::operator-=(const LN& a)
{
	if (this->m_is_negative == a.m_is_negative)
	{
		*this = *this - a;
	}
	else
	{
		return *this += -a;
	}
	return *this;
}

LN LN::operator-(const LN& a) const
{
	LN result = LN(0ll);
	if (a.m_is_NaN || this->m_is_NaN)
	{
		result.m_is_NaN = true;
		return result;
	}
	if (this->m_is_negative == a.m_is_negative)
	{
		bool lower = false;
		for (int i = 0; i < std::min(this->m_chunks.size(), a.m_chunks.size()); i++)
		{
			if (this->m_chunks[i] < a.m_chunks[i])
				lower = true;
			else if (this->m_chunks[i] > a.m_chunks[i])
				lower = false;
		}
		for (long long i = this->m_chunks.size(); i < a.m_chunks.size(); ++i)
		{
			if (a.m_chunks[i] != 0)
				lower = true;
		}
		for (long long i = a.m_chunks.size(); i < this->m_chunks.size(); ++i)
		{
			if (this->m_chunks[i] != 0)
				lower = false;
		}
		if (lower)
		{
			return -(a - *this);
		}
		result.m_is_negative = this->m_is_negative;
		result.m_chunks.resize(std::max(this->m_chunks.size(), a.m_chunks.size()), 0);
		uint64_t carry = 0;
		for (long long i = 0; i < result.m_chunks.size(); i++)
		{
			uint64_t val = (1ll << 32) + (uint64_t)(this->m_chunks.size() > i ? this->m_chunks[i] : 0) -
						   (a.m_chunks.size() > i ? a.m_chunks[i] : 0) - carry;
			result.m_chunks[i] = val % m_base;
			carry = val / m_base == 1 ? 0 : 1;
		}
	}
	else
	{
		return *this + -a;
	}
	return result;
}

LN LN::operator/(const LN& a) const
{
	LN result;
	if (a.m_is_NaN || this->m_is_NaN || a == LN(0ll))
	{
		result.m_is_NaN = true;
		return result;
	}
	else if (*this == LN(0ll))
	{
		return *this;
	}
	LN A = *this;
	LN B = a;
	A.m_is_negative = false;
	B.m_is_negative = false;
	A.m_chunks.resize(std::max(a.m_chunks.size(), this->m_chunks.size()), 0);
	long long n = a.m_chunks.size() - 1;
	while (a.m_chunks[n] == 0 && n >= 0)
		n--;
	if (n < 0)
	{
		result.m_is_NaN = true;
		return result;
	}
	long long nm = A.m_chunks.size() - 1;
	while (A.m_chunks[nm] == 0 && nm >= 0)
		nm--;
	if (nm < n)
	{
		result = LN(0ll);
		return result;
	}
	result.m_chunks.resize(nm - n + 1, 0);
	for (long long i = result.m_chunks.size() - 1; i >= 0; --i)
	{
		for (int j = 31; j >= 0; j--)
		{
			result.m_chunks[i] += (1 << j);
			if (B * result > A)
				result.m_chunks[i] -= (1 << j);
		}
	}
	if (this->m_is_negative != a.m_is_negative)
		result = -result;
	return result;
}

LN LN::operator%(const LN& a) const
{
	LN b = *this / a;
	LN result = *this - a * b;
	return result;
}

LN& LN::operator%=(const LN& a)
{
	LN b = *this / a;
	*this = *this - a * b;
	return *this;
}

LN& LN::operator/=(const LN& a)
{
	*this = *this / a;
	return *this;
}

LN LN::operator~() const
{
	LN result = LN(2);
	if (this->m_is_NaN || this->m_is_negative)
	{
		result.m_is_NaN = true;
		return result;
	}
	while (!((result * result <= *this) && (result * result + LN(2ll) * result + LN(1ll) > *this)))
	{
		result = (result + *this / result) / LN(2ll);
		if (result == (result + *this / result) / LN(2ll))
			break;
	}
	return result;
}

int LN::equality(const LN& a) const noexcept
{
	if (this->m_is_NaN || a.m_is_NaN)
		return -2;
	bool a_0 = true, this_0 = true;
	for (int i = 0; i < a.m_chunks.size(); ++i)
		if (a.m_chunks[i] != 0)
			a_0 = false;
	for (int i = 0; i < this->m_chunks.size(); ++i)
		if (this->m_chunks[i] != 0)
			this_0 = false;

	if (a_0 && this_0)
		return 0;
	else if (a_0)
		return this->m_is_negative ? -1 : 1;
	else if (this_0)
		return a.m_is_negative ? 1 : -1;
	else if (this->m_is_negative != a.m_is_negative)
		return this->m_is_negative ? -1 : 1;
	else
	{
		bool lower = false;
		bool greater = false;
		for (long long i = 0; i < std::min(this->m_chunks.size(), a.m_chunks.size()); i++)
		{
			if (this->m_chunks[i] < a.m_chunks[i])
			{
				lower = true;
				greater = false;
			}
			else if (this->m_chunks[i] > a.m_chunks[i])
			{
				lower = false;
				greater = true;
			}
		}
		for (long long i = this->m_chunks.size(); i < a.m_chunks.size(); ++i)
		{
			if (a.m_chunks[i] != 0)
			{
				lower = true;
				greater = false;
			}
		}
		for (long long i = a.m_chunks.size(); i < this->m_chunks.size(); ++i)
		{
			if (this->m_chunks[i] != 0)
			{
				lower = false;
				greater = true;
			}
		}

		if (!lower && !greater)
			return 0;
		else
			return lower == this->m_is_negative ? 1 : -1;
	}
}

LN LN::operator==(const LN& x) const noexcept
{
	int result = this->equality(x);
	if (result == -2)
		return LN(0ll);
	return LN(result == 0);
}

LN LN::operator!=(const LN& x) const noexcept
{
	int result = this->equality(x);
	if (result == -2)
		return LN(0ll);
	return LN(result != 0);
}

LN LN::operator<=(const LN& x) const noexcept
{
	int result = this->equality(x);
	if (result == -2)
		return LN(0ll);
	return LN(result != 1);
}

LN LN::operator>=(const LN& x) const noexcept
{
	int result = this->equality(x);
	if (result == -2)
		return LN(0ll);
	return LN(result != -1);
}

LN LN::operator>(const LN& x) const noexcept
{
	int result = this->equality(x);
	if (result == -2)
		return LN(0ll);
	return LN(result == 1);
}

LN LN::operator<(const LN& x) const noexcept
{
	int result = this->equality(x);
	if (result == -2)
		return LN(0ll);
	return LN(result == -1);
}

LN::operator bool() const
{
	return this->equality(LN(0ll)) == 1 || this->equality(LN(0ll)) == -1;
}

LN::operator long long() const
{
	if (this->m_is_NaN)
		throw(std::range_error("Value cannot be represented as long long"));
	long long x = (1ll << 32) * (this->m_chunks.size() > 1 ? this->m_chunks[1] : 0) + this->m_chunks[0];
	if (this->m_is_negative)
		x *= -1;
	if (LN(x) != *this)
		throw(std::range_error("Value cannot be represented as long long"));
	return x;
}

std::ostream& operator<<(std::ostream& out, const LN& x)
{
	if (x.m_is_NaN)
	{
		out << "NaN";
		return out;
	}
	if (x.m_is_negative && x != LN(0ll))
	{
		out << "-";
	}
	long long i = x.m_chunks.size() - 1;
	while (x.m_chunks[i] == 0 && i > 0)
		i--;
	out << std::hex << std::uppercase << x.m_chunks[i--];
	for (; i >= 0; i--)
	{
		out << std::setw(8) << std::setfill('0') << std::hex << std::uppercase << x.m_chunks[i];
	}
	return out;
}
LN::~LN()
{
	m_chunks.~my_vector();
	m_is_negative = false;
	m_is_NaN = false;
}

LN operator""_ln(const char* x)
{
	return LN(x);
}
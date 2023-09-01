#include "LN.h"
#include "return_codes.h"

#include <fstream>
#include <functional>
#include <map>
#include <stack>
#include <string>

int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		fprintf(stderr, "The parameter or number of parameters (argv) is incorrect\n");
		return ERROR_PARAMETER_INVALID;
	}
	std::ifstream in(argv[1]);
	if (!in.is_open())
	{
		fprintf(stderr, "The input file is invalid\n");
		return ERROR_CANNOT_OPEN_FILE;
	}
	std::stack< LN > stack;
	std::string str;
	std::map< std::string, std::function< LN(LN) > > unary_operations{
		{ "~",
		  [](const LN& val)
		  {
			  return ~val;
		  } },
		{ "-",
		  [](const LN& val)
		  {
			  return -val;
		  } },
	};
	std::map< std::string, std::function< LN(LN, LN) > > binary_operations{
		{ "+",
		  [](const LN& left, const LN& right)
		  {
			  return left + right;
		  } },
		{ "-",
		  [](const LN& left, const LN& right)
		  {
			  return left - right;
		  } },
		{ "*",
		  [](const LN& left, const LN& right)
		  {
			  return left * right;
		  } },
		{ "/",
		  [](const LN& left, const LN& right)
		  {
			  return left / right;
		  } },
		{ "%",
		  [](const LN& left, const LN& right)
		  {
			  return left % right;
		  } },
		{ "<=",
		  [](const LN& left, const LN& right)
		  {
			  return left <= right;
		  } },
		{ ">=",
		  [](const LN& left, const LN& right)
		  {
			  return left >= right;
		  } },
		{ ">",
		  [](const LN& left, const LN& right)
		  {
			  return left > right;
		  } },
		{ "<",
		  [](const LN& left, const LN& right)
		  {
			  return left < right;
		  } },
		{ "!=",
		  [](const LN& left, const LN& right)
		  {
			  return left != right;
		  } },
		{ "==",
		  [](const LN& left, const LN& right)
		  {
			  return left == right;
		  } },
	};
	while (std::getline(in, str))
	{
		try
		{
			LN temp(0ll);
			if (binary_operations.find(str) != binary_operations.end())
			{
				LN left = stack.top();
				stack.pop();
				LN right = stack.top();
				stack.pop();
				temp = binary_operations[str](left, right);
			}
			else if (unary_operations.find(str) != unary_operations.end())
			{
				LN val = stack.top();
				stack.pop();
				temp = unary_operations[str](val);
			}
			else
			{
				std::string_view view = str;
				temp = LN(view);
			}
			stack.push(temp);
		} catch (const std::bad_alloc& e)
		{
			fprintf(stderr, "error with allocation, error = %s", e.what());
			in.close();
			return ERROR_OUT_OF_MEMORY;
		} catch (const std::range_error& e)
		{
			fprintf(stderr, "error with converting LN to long long, error = %s", e.what());
			in.close();
			return ERROR_DATA_INVALID;
		} catch (...)
		{
			fprintf(stderr, "unknown error");
			in.close();
			return ERROR_UNKNOWN;
		}
	}
	in.close();
	std::ofstream out(argv[2]);
	if (!out.is_open())
	{
		fprintf(stderr, "The output file is invalid\n");
		return ERROR_CANNOT_OPEN_FILE;
	}
	while (!stack.empty())
	{
		LN tmp = stack.top();
		out << tmp << std::endl;
		stack.pop();
	}
	out.close();
	return SUCCESS;
}
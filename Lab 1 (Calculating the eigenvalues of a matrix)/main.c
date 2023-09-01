#include "return_codes.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
void multiplyTwoMatrix(size_t n, double (*a)[n], double (*b)[n], double (*answer)[n])
{
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			answer[i][j] = 0;
			for (int t = 0; t < n; t++)
			{
				answer[i][j] += a[i][t] * b[t][j];
			}
		}
	}
}
double squareMagnitudeOfVector(const double *v, size_t n, size_t start)
{
	double magnitude = 0;
	for (size_t i = start; i < n; i++)
	{
		magnitude += v[i] * v[i];
	}
	return magnitude;
}
void qrHouseholder(size_t n, double (*a)[n], double *w, double (*h)[n], double (*newA)[n])
{
	for (int start = 0; start < n - 2; start++)
	{
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
				newA[i][j] = a[i][j];
			}
		}
		double magnitudeOfAStartVector = 0;
		for (int i = 0; i < n; i++)
		{
			w[i] = 0;
		}
		for (int i = start + 1; i < n; i++)
		{
			magnitudeOfAStartVector += a[i][start] * a[i][start];
		}
		if (newA[start + 1][start] < 0)
		{
			w[start + 1] = sqrt(magnitudeOfAStartVector);
		}
		else
		{
			w[start + 1] = -sqrt(magnitudeOfAStartVector);
		}
		for (int i = start + 1; i < n; i++)
		{
			w[i] += -a[i][start];
		}
		double magnitudeOfV = squareMagnitudeOfVector(w, n, start);
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
				if (i == j)
				{
					h[i][j] = 1;
				}
				else
				{
					h[i][j] = 0;
				}
				if (i < start || j < start || magnitudeOfV == 0)
				{
					continue;
				}
				h[i][j] -= 2 * (w[i] * w[j]) / magnitudeOfV;
			}
		}
		multiplyTwoMatrix(n, h, a, newA);
		multiplyTwoMatrix(n, newA, h, a);
	}
}
double MaxValueUnderDiagonal(size_t n, double (*a)[n])
{
	double answer = a[1][0];
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			a[i][j] = a[i][j];
			if (i == j + 1)
				if (a[i][j] > answer)
					answer = a[i][j];
		}
	}
	return answer;
}
void qrAlgorithm(size_t n, double (*a)[n], double EPS, double *c, double *s, double (*h)[n], double (*h_c_s)[n])
{
	double prevMaxValueUnderDiagonal1 = MaxValueUnderDiagonal(n, a);
	double prevMaxValueUnderDiagonal2 = prevMaxValueUnderDiagonal1 + 2 * EPS;
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			h[i][j] = a[i][j];
		}
	}
	for (int k = 1;
		 (prevMaxValueUnderDiagonal2 - prevMaxValueUnderDiagonal1 < EPS || prevMaxValueUnderDiagonal1 - prevMaxValueUnderDiagonal2 < EPS) &&
		 k < 1500;
		 k++)
	{
		if (k % 2 == 1)
		{
			prevMaxValueUnderDiagonal1 = MaxValueUnderDiagonal(n, h);
		}
		else
		{
			prevMaxValueUnderDiagonal2 = MaxValueUnderDiagonal(n, h);
		}
		for (int i = 0; i < n - 1; i++)
		{
			c[i] = h[i][i] / sqrt(h[i][i] * h[i][i] + h[i + 1][i] * h[i + 1][i]);
			s[i] = h[i + 1][i] / sqrt(h[i][i] * h[i][i] + h[i + 1][i] * h[i + 1][i]);
			for (int l = i; l < n; l++)
			{
				h_c_s[i][l] = c[i] * h[i][l] + s[i] * h[i + 1][l];
				h_c_s[i + 1][l] = (-s[i]) * h[i][l] + c[i] * h[i + 1][l];
			}
			for (int l = i; l < n; l++)
			{
				h[i][l] = h_c_s[i][l];
				h[i + 1][l] = h_c_s[i + 1][l];
			}
		}
		for (int i = 0; i < n - 1; i++)
		{
			for (int l = 0; l <= i + 1; l++)
			{
				h_c_s[l][i] = c[i] * h[l][i] + s[i] * h[l][i + 1];
				h_c_s[l][i + 1] = (-1) * s[i] * h[l][i] + c[i] * h[l][i + 1];
			}
			for (int l = 0; l <= i + 1; l++)
			{
				h[l][i] = h_c_s[l][i];
				h[l][i + 1] = h_c_s[l][i + 1];
			}
		}
	}
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			a[i][j] = h[i][j];
		}
	}
}
double find_EPS(size_t n, double (*a)[n])
{
	double sum = 0;
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			if (a[i][j] < 0)
			{
				sum -= a[i][j];
			}
			else
			{
				sum += a[i][j];
			}
		}
	}
	if (sum == 0)
	{
		return 1.0;
	}
	sum /= (double)n * (double)n;
	return sum / 10000;
}
int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		fprintf(stderr, "The parameter or number of parameters (argv) is incorrect\n");
		return ERROR_PARAMETER_INVALID;
	}
	FILE *in = fopen(argv[1], "rb");
	if (in == NULL)
	{
		fprintf(stderr, "The data is invalid\n");
		return ERROR_CANNOT_OPEN_FILE;
	}
	size_t n;
	if ((fscanf(in, "%zu\n", &n) != 1) || 0 >= n)
	{
		free(in);
		fprintf(stderr, "The data is invalid\n");
		return ERROR_DATA_INVALID;
	}
	double(*a)[n] = malloc((sizeof(double) * n) * n);
	if (a == NULL)
	{
		free(in);
		fprintf(stderr, "Not enough memory, memory allocation failed\n");
		return ERROR_OUT_OF_MEMORY;
	}
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			if (fscanf(in, "%lf", &a[i][j]) != 1)
			{
				free(in);
				free(a);
				fprintf(stderr, "The data is invalid\n");
				return ERROR_DATA_INVALID;
			}
		}
	}
	fclose(in);
	const double EPS = find_EPS(n, a);
	double *c = malloc(n * sizeof(double));
	double *s = malloc(n * sizeof(double));
	double(*h)[n] = malloc((sizeof(double) * n) * n);
	double(*h_c_s)[n] = malloc((sizeof(double) * n) * n);
	if (c == NULL || s == NULL || h == NULL || h_c_s == NULL)
	{
		free(c);
		free(s);
		free(h);
		free(h_c_s);
		fprintf(stderr, "Not enough memory for arrays, memory allocation failed\n");
		return ERROR_OUT_OF_MEMORY;
	}
	qrHouseholder(n, a, c, h, h_c_s);
	qrAlgorithm(n, a, EPS, c, s, h, h_c_s);
	FILE *out = fopen(argv[2], "w");
	if (out == NULL)
	{
		free(a);
		fclose(in);
		fprintf(stderr, "File can't be open\n");
		return ERROR_CANNOT_OPEN_FILE;
	}
	for (size_t i = 0; i < n; i++)
	{
		if (a[i + 1][i] * a[i + 1][i] <= EPS)
		{
			if (fprintf(out, "%g\n", a[i][i]) == -1)
			{
				fclose(out);
				fclose(in);
				free(a);
				fprintf(stderr, "The data is invalid\n");
				return ERROR_DATA_INVALID;
			}
		}
		else
		{
			if (fprintf(out,
						"%g +%gi\n",
						(a[i][i] + a[i + 1][i + 1]) / 2,
						sqrt(-(a[i][i] + a[i + 1][i + 1]) * (a[i][i] + a[i + 1][i + 1]) / 4 +
							 a[i][i] * a[i + 1][i + 1] - a[i][i + 1] * a[i + 1][i])) == -1 ||
				fprintf(out,
						"%g -%gi\n",
						(a[i][i] + a[i + 1][i + 1]) / 2,
						sqrt(-(a[i][i] + a[i + 1][i + 1]) * (a[i][i] + a[i + 1][i + 1]) / 4 +
							 a[i][i] * a[i + 1][i + 1] - a[i][i + 1] * a[i + 1][i])) == -1)
			{
				fclose(out);
				fclose(in);
				free(a);
				fprintf(stderr, "The data is invalid\n");
				return ERROR_DATA_INVALID;
			}
			i++;
		}
	}
	free(a);
	fclose(out);
	return SUCCESS;
}

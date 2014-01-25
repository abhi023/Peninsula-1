#pragma once

#include <exception>
#include <vector>
#include <thread>
#include <cstdint>

template <typename elemtype, std::size_t rows, std::size_t columns>
class matrix
{
public:
	std::size_t n_rows, n_columns;

	typedef matrix<elemtype, rows, columns> matrix_t;
	typedef matrix<elemtype, rows - 1, columns - 1> minor_t;

	elemtype m[rows][columns];

	matrix()
	{
		n_rows = rows;
		n_columns = columns;
	}

	explicit matrix(elemtype *ptr)
	{
		memcpy_s(m, rows*columns*sizeof(elemtype), ptr, rows*columns*sizeof(elemtype));
	}

	explicit matrix(elemtype arr[rows][columns])
	{
		memcpy_s(m, sizeof(elemtype)* rows * columns, arr, sizeof(elemtype)* rows * columns);
	}

	matrix(initializer_list<initializer_list<elemtype>> il)
	{
		std::size_t i = 0, j = 0;

		for (initializer_list<elemtype> row_il : il)
		{
			j = 0;
			for (elemtype col_il : row_il)
			{
				m[i][j] = col_il;
				j++;
			}
			i++;
		}
	}

	// Returns the matrix without the specified row and column. Useful for calculating determinants
	minor_t minor(std::size_t r, std::size_t c)
	{
		static_assert(rows * columns > 0, "Trying to create minor of matrix with 0 elements");

		minor_t ret;

		std::vector<elemtype> row(rows - 1);

		for (std::size_t i = 0; i < rows; i++)
		{
			for (std::size_t j = 0; j < rows; j++)
			{
				if (i == r) continue;
				if (j == c) continue;

				ret.m[i < r ? i : i - 1][j < c ? j : j - 1] = m[i][j];
			}
		}
		
		return ret;
	}

	void add(const matrix<elemtype, rows, columns>& right)
	{
		vector<thread> thr(rows);

		for (std::size_t i = 0; i < rows; i++)
		{
			thr.push_back(thread([i, right](){
				for (std::size_t j = 0; j < columns; j++)
				{
					m[i][j] += right.m[i][j];
				}
			}));
		}

		for (std::size_t i = 0; i < thr.size(); i++)
			thr[i].join();
	}

	void subtract(const matrix<elemtype, rows, columns>& right)
	{
		vector<thread> thr(rows);

		for (std::size_t i = 0; i < rows; i++)
		{
			thr.push_back(thread([i, right]()
			{
				for (std::size_t j = 0; j < columns; j++)
				{
					m[i][j] -= right.m[i][j];
				}
			}));
		}

		for (std::size_t i = 0; i < thr.size(); i++)
			thr[i].join();
	}

	void multiply(const elemtype& scalar)
	{
		vector<thread> thr(rows);

		for (std::size_t i = 0; i < rows; i++)
		{
			thr.push_back(thread[i, scalar]()
			{
				for (std::size_t j = 0; j < columns; j++)
				{
					m[i][j] *= scalar;
				}
			})
		}

		for (std::size_t i = 0; i < thr.size(); i++)
			thr[i].join();
	}



};

template <typename elemtype, std::size_t s>
class sqmatrix :
	public matrix<elemtype, s, s>
{
public:
	elemtype determinant()
	{
		static_assert(rows == columns, "Cannot find determinant of non-square matrix");

		elemtype det = 0;

		if (rows == 2)
			det = (m[0][0] * m[1][1]) - (m[0][1] * m[1][0]);

		else
		{
			elemtype sgn = 1;
			for (std::size_t i = 0; i < rows; i++)
			{
				minor_t min = minor(i, 0);

				det += sgn * min.determinant();

				sgn *= -1;
			}
		}

		return det;
	}

	void transpose()
	{
		static_assert(rows == columns, "Please use external function to transpose non-square matrix");

		matrix<elemtype, rows, columns> temp;
		for (std::size_t i = 0; i < r; i++)
		{
			for (std::size_t j = 0; j < c; j++)
			{
				temp[j][i] = m[i][j];
			}
		}

		memcpy_s(m, sizeof(elemtype)* rows * columns, temp.m, sizeof(elemtype)* rows * columns);
	}
};

#ifdef XMMATRIXROUTINES
#ifdef _WINDOWS 
#include <DirectXMath.h>

XMMATRIX to_XMMATRIX(const sqmatrix<float, 2> &mat)
{
	return DirectX::XMMATRIX(
		mat.m[0][0], mat.m[0][1], 0.f, 0.f,
		mat.m[1][0], mat.m[1][1], 0.f, 0.f,
		0.f, 0.f, 0.f, 0.f,
		0.f, 0.f, 0.f, 1.f);
}

XMMATRIX to_XMMATRIX(const sqmatrix<float, 3> &mat)
{
	return DirectX::XMMATRIX(
		mat.m[0][0], mat.m[0][1], mat.m[0][2], 0.f,
		mat.m[1][0], mat.m[1][1], mat.m[1][2], 0.f,
		mat.m[2][0], mat.m[2][1], mat.m[2][2], 0.f,
		0.f, 0.f, 0.f, 1.f);
}

XMMATRIX to_XMMATRIX(const sqmatrix<float, 4> &mat)
{
	return DirectX::XMMATRIX((float*) mat.m);
}

#endif
#endif

template <typename elemtype>
class matrix<elemtype, 0, 0>
{
public:
	elemtype m[1][1];

	elemtype determinant()
	{
		throw std::bad_function_call;
	}
};

template <typename elemtype>
elemtype dot(const std::vector<elemtype>& a, const std::vector<elemtype>& b) 
{
	if (a.size() != b.size())
		throw std::exception("Vectors must be same length");

	elemtype total = 0;

	for (std::size_t i = 0; i < a.size(); i++)
	{
		total += a[i] * b[i];
	}

	return total;
}

template <typename elemtype, std::size_t n>
elemtype dot(elemtype a[n], elemtype b[n])
{
	elemtype total = 0;

	for (std::size_t i = 0; i < n; i++)
	{
		total += a[i] * b[i];
	}

	return total;
}

template <typename elemtype, std::size_t lrows, std::size_t lcolumns, std::size_t rrows, std::size_t rcolumns >
matrix<elemtype, lrows, rcolumns> multiply(const matrix<elemtype, lrows, lcolumns>& left, const matrix<elemtype, rrows, rcolumns>& right)
{
	static_assert(lcolumns == rrows, "Invalid matrix multiplication");
	static_assert(lrows == rcolumns, "Invalid matrix multiplication");

	matrix<elemtype, lrows, rcolumns> ret;

	for (std::size_t i = 0; i < lrows; i++)
	{
		for (std::size_t j = 0; j < rcolumns; j++)
		{
			// dot product of row i from left and column j of right
			elemtype row_i[lcolumns], col_j[rrows];
			
			// fill row_i with ... row i
			for (std::size_t k = 0; k < lcolumns; k++)
			{
				row_i[k] = left.m[i][k];
			}

			// fill col_j with column j
			for (std::size_t k = 0; k < rrows; k++)
			{
				col_j[k] = right.m[k][j];
			}

			// find the dot product and store that in the answer matrix
			ret.m[i][j] = dot(row_i, col_j);
		}
	}

	return ret;
}

template <typename t>
using matrix2x2t = sqmatrix<t, 2>;

template <typename t>
using matrix3x3t = sqmatrix<t, 3>;

template <typename t>
using matrix4x4t = sqmatrix<t, 4>;

typedef matrix2x2t<float> matrix2x2;
typedef matrix3x3t<float> matrix3x3;
typedef matrix4x4t<float> matrix4x4;

typedef matrix2x2t<double> matrix2x2d;
typedef matrix3x3t<double> matrix3x3d;
typedef matrix4x4t<double> matrix4x4d;

template <typename T>
matrix2x2t<T> rotation(T angle_in_radians)
{
	T[2][2] rot = {
		{ cos(angle_in_radians), sin(angle_in_radians) },
		{ -sin(angle_in_radians), cos(angle_in_radians) } };

	return matrix2x2t<T>(rot);
}

template <typename T>
matrix2x2t<T> scaling(T x_scale, T y_scale)
{
	T[2][2] sc = { { x_scale, 0 }, { 0, y_scale } };
	
	return matrix2x2t<T>(sc);
}
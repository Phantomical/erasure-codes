
#include "matrix.h"
#include "symbols.h"
#include "config.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define elem_at(m, i, j) ((m).data[i * (m).cols + j])
#define is_empty(m) (!(m).data || (m).rows == 0 || (m).cols == 0)
#define empty_matrix matrix_create(NULL, 0, 0)

void memswap(void* a_, void* b_, size_t size)
{
	char* a = a_;
	char* b = b_;

	for (size_t i = 0; i < size; ++i)
	{
		char tmp = *a;
		*a = *b;
		*b = tmp;
	}
}

struct matrix matrix_create(
	uint8_t* data,
	size_t rows,
	size_t cols)
{
	struct matrix m = {
		data,
		rows,
		cols
	};
	return m;
}
void matrix_destroy(struct matrix m)
{
	free(m.data);
}

struct matrix matrix_create_identity(size_t side_len)
{
	struct matrix m = matrix_create_zero(side_len, side_len);

	if (!m.data) // Deal with OOM
		return empty_matrix;

	for (size_t i = 0; i < side_len; ++i)
		elem_at(m, i, i) = 1;

	return m;
}
struct matrix matrix_create_zero(
	size_t rows,
	size_t cols)
{
	if (!rows || !cols)
		return empty_matrix;

	struct matrix m = {
		calloc(1, sizeof(uint8_t) * rows * cols),
		rows,
		cols
	};

	if (!m.data)
		return empty_matrix;

	return m;
}

struct matrix matrix_mul(
	const struct matrix a,
	const struct matrix b)
{
	if (is_empty(a) || is_empty(b))
		return empty_matrix;
	if (a.cols != b.rows)
		return empty_matrix;

	struct matrix m = matrix_create_zero(
		a.rows,
		b.cols);

	// If we run out of memory return an empty matrix
	if (!m.data)
		return empty_matrix;

	for (size_t i = 0; i < a.cols; ++i)
	{
		for (size_t j = 0; j < b.rows; ++j)
		{
			for (size_t k = 0; k < a.cols; ++k)
			{
				const uint8_t tmp = symbol_mul(
					elem_at(a, i, k),
					elem_at(a, k, j));

				elem_at(m, i, j) = symbol_add(
					elem_at(m, i, j),
					tmp);
			}
		}
	}

	return m;
}

struct matrix matrix_augment(
	const struct matrix a,
	const struct matrix b)
{
	if (is_empty(a) || is_empty(b))
		return empty_matrix;
	if (a.rows != b.rows)
		return empty_matrix;

	struct matrix m = matrix_create(
		malloc(sizeof(uint8_t) * a.rows * (a.cols + b.cols)),
		a.rows,
		a.cols + b.cols);

	if (!m.data) // Handle out-of-memory
		return empty_matrix;

	for (size_t i = 0; i < m.rows; ++i)
	{
		memcpy(
			&elem_at(m, i, 0),
			&elem_at(a, i, 0),
			sizeof(uint8_t) * a.cols);

		memcpy(
			&elem_at(m, i, a.cols),
			&elem_at(b, i, 0),
			sizeof(uint8_t) * b.cols);
	}

	return m;
}

bool matrix_inverse(
	struct matrix m)
{
	if (is_empty(m) || m.rows != m.cols)
		return false;
	
	struct matrix i = matrix_create_identity(m.rows);
	struct matrix a = matrix_augment(m, i);
	matrix_destroy(i);

	if (is_empty(a))
		return false;

	for (size_t r1 = 0; r1 < a.rows; ++r1)
	{
		uint8_t div = elem_at(a, r1, r1);

		// If the current diagonal element is 0,
		// swap rows so that it isn't 0. If this
		// can't be done then the matrix is singular.
		if (div == 0)
		{
			for (size_t r2 = r1 + 1; r2 < a.rows; ++r2)
			{
				if (elem_at(a, r2, r1) != 0)
				{
					memswap(
						&elem_at(a, r1, 0),
						&elem_at(a, r2, 0),
						sizeof(uint8_t) * a.cols);
					break;
				}
			}

			div = elem_at(a, r1, r1);

			// If the matrix is singular go to cleanup
			// code at the end of the function.
			if (div == 0)
				goto is_singular;
		}

		if (div != 1)
		{
			for (size_t c = r1; c < m.cols; ++c)
				elem_at(a, r1, c) = symbol_div(
					elem_at(a, r1, c), 
					div);
		}

		for (size_t r2 = 0; r2 < a.rows; ++r2)
		{
			if (r2 == r1)
				continue;

			const uint8_t mult = elem_at(a, r2, r1);

			for (size_t c = 0; c < a.cols; ++c)
			{
				const uint8_t tmp = symbol_mul(
					mult,
					elem_at(a, r1, c));

				elem_at(a, r2, c) = symbol_sub(
					elem_at(a, r2, c),
					tmp);
			}
		}
	}

	// Copy the array back into the original matrix
	// that we are modifying.
	for (size_t r = 0; r < a.rows; ++r)
	{
		memcpy(
			&elem_at(m, r, 0),
			&elem_at(a, r, m.cols),
			sizeof(uint8_t) * m.cols);
	}

	matrix_destroy(a);
	return true;

	// If the matrix is singular cleanup and 
	// return false.
is_singular:
	matrix_destroy(a);
	return false;
}

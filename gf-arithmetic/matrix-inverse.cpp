#include "matrix.h"
#include "encode.h"

#include <intrin.h>
#include <cstring>
#include <cstdio>

#include <algorithm>

namespace {
#define SWAP(a,b,e) std::swap(a,b)
	typedef uint8_t gf;

	void addmul(gf* dst, gf* src, gf c, int sz)
	{
		if (c != 0)
			gfarith::adv::mul_add_row(c, src, dst, sz);
	}

	//TODO: Update this to something
	//      that was written myself.
	//      Preferably something easier
	//      to understand and maintain.
	static int invert_mat(gf *src, int k)
	{
		gf c, *p;
		int irow, icol, row, col, i, ix;

		int error = 1;
		int* indxc = (int*)alloca(sizeof(int) * k);
		int* indxr = (int*)alloca(sizeof(int) * k);
		int* ipiv = (int*)alloca(sizeof(int) * k);
		gf* id_row = (gf*)alloca(sizeof(gf) * k);

		memset(id_row, 0, k * sizeof(gf));
		/*
		* ipiv marks elements already used as pivots.
		*/
		for (i = 0; i < k; i++)
			ipiv[i] = 0;

		for (col = 0; col < k; col++) {
			gf *pivot_row;
			/*
			* Zeroing column 'col', look for a non-zero element.
			* First try on the diagonal, if it fails, look elsewhere.
			*/
			irow = icol = -1;
			if (ipiv[col] != 1 && src[col*k + col] != 0) {
				irow = col;
				icol = col;
				goto found_piv;
			}
			for (row = 0; row < k; row++) {
				if (ipiv[row] != 1) {
					for (ix = 0; ix < k; ix++) {
						if (ipiv[ix] == 0) {
							if (src[row*k + ix] != 0) {
								irow = row;
								icol = ix;
								goto found_piv;
							}
						}
						else if (ipiv[ix] > 1) {
							fprintf(stderr, "singular matrix\n");
							goto fail;
						}
					}
				}
			}
			if (icol == -1) {
				fprintf(stderr, "XXX pivot not found!\n");
				goto fail;
			}
		found_piv:
			++(ipiv[icol]);
			/*
			* swap rows irow and icol, so afterwards the diagonal
			* element will be correct. Rarely done, not worth
			* optimizing.
			*/
			if (irow != icol) {
				for (ix = 0; ix < k; ix++) {
					SWAP(src[irow*k + ix], src[icol*k + ix], gf);
				}
			}
			indxr[col] = irow;
			indxc[col] = icol;
			pivot_row = &src[icol*k];
			c = pivot_row[icol];
			if (c == 0) {
				fprintf(stderr, "singular matrix 2\n");
				goto fail;
			}
			if (c != 1) { /* otherwhise this is a NOP */
						  /*
						  * this is done often , but optimizing is not so
						  * fruitful, at least in the obvious ways (unrolling)
						  */
				c = gfarith::div(1, c);
				pivot_row[icol] = 1;
				for (ix = 0; ix < k; ix++)
					pivot_row[ix] = gfarith::mul(c, pivot_row[ix]);
			}
			/*
			* from all rows, remove multiples of the selected row
			* to zero the relevant entry (in fact, the entry is not zero
			* because we know it must be zero).
			* (Here, if we know that the pivot_row is the identity,
			* we can optimize the addmul).
			*/
			id_row[icol] = 1;
			if (memcmp(pivot_row, id_row, k * sizeof(gf)) != 0) {
				for (p = src, ix = 0; ix < k; ix++, p += k) {
					if (ix != icol) {
						c = p[icol];
						p[icol] = 0;
						addmul(p, pivot_row, c, k);
					}
				}
			}
			id_row[icol] = 0;
		} /* done all columns */
		for (col = k - 1; col >= 0; col--) {
			if (indxr[col] < 0 || indxr[col] >= k)
				fprintf(stderr, "AARGH, indxr[col] %d\n", indxr[col]);
			else if (indxc[col] < 0 || indxc[col] >= k)
				fprintf(stderr, "AARGH, indxc[col] %d\n", indxc[col]);
			else
				if (indxr[col] != indxc[col]) {
					for (row = 0; row < k; row++) {
						SWAP(src[row*k + indxr[col]], src[row*k + indxc[col]], gf);
					}
				}
		}
		error = 0;
	fail:
		return error;
	}
}

namespace gfarith
{
	
	matrix matrix::inverse() const
	{
		if (this->is_null())
			return matrix();

		assert(size1() == size2());

		matrix m = matrix(size1(), size2() * 2);

		std::memset(m.data(), 0, m.datasize());

		// Initialize the extended matrix
		for (size_t r = 0; r < m.size1(); ++r)
		{
			// Copy our matrix value
			std::copy((*this)[r].begin(), (*this)[r].end(), m[r].begin());
			// Inverse is initialized to the identity matrix
			m(r, r + size2()) = 1;
		}

		for (size_t r1 = 0; r1 < m.size1(); ++r1)
		{
			symbol div = m(r1, r1);

			if (div.value == 0)
			{
				matrix tmp(1, m.size2());

				// Swap the row with one that doesn't have a 0
				// along the diagonal
				for (size_t r2 = r1 + 1; r2 < m.size1(); ++r2)
				{
					if (m(r2, r1).value != 0)
					{
						// Swap the rows using a matrix temporary
						tmp[0] = m[r1];
						m[r1] = m[r2];
						m[r2] = tmp[0];

						break;
					}
				}

				div = m(r1, r1);

				if (div.value == 0)
					return matrix();
			}
			
			if (div.value != 1)
			{
				for (size_t c = r1; c < m.size2(); ++c)
				{
					m(r1, c) /= div;
				}
			}

			for (size_t r2 = 0; r2 < m.size1(); ++r2)
			{
				if (r2 == r1)
					continue;

				symbol mult = m(r2, r1);

				for (size_t c = 0; c < m.size2(); ++c)
				{
					m(r2, c) -= mult * m(r1, c);
				}
			}
		}

		return m.submatrix(0, size1(), size1(), size1() * 2);
	}
	//*/
	/*
	matrix matrix::inverse() const
	{
		assert(size1() == size2());
		assert(size1() <= INT_MAX);
	
		if (this->is_null())
			return matrix();
	
		matrix m = *this;
		gf* vals = (gf*)m.data();
	
		if (invert_mat(vals, (int)size1()) != 0)
			return matrix();
	
		return m;
	}
	//*/
}

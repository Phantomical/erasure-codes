#include "matrix.h"
#include <iostream>

#include "catch-wrapper.hpp"

using gfarith::symbol;
using gfarith::matrix;

matrix vandermonde(size_t n, size_t k)
{
	matrix m{ n, k };

	for (size_t r = 0; r < n; ++r)
	{
		for (size_t c = 0; c < k; ++c)
		{
			m(r, c) = gfarith::exp((uint8_t)r, (uint8_t)c);
		}
	}

	return m;
}

constexpr size_t sizes[] = { 1, 3, 4, 5, 9, 129, 243 };

TEST_CASE("identity inverse", "[matrix]")
{
	for (size_t sz : sizes)
	{
		matrix m{ sz, sz };
		memset(m.data(), 0, sizeof(symbol) * m.size());

		for (size_t i = 0; i < sz; ++i)
			m(i, i) = 1;

		matrix m2 = m.inverse();

		REQUIRE(m == m2);
	}
}

TEST_CASE("vandermonde inverse is not null", "[matrix]")
{
	for (size_t sz : sizes)
	{
		matrix m = vandermonde(sz, sz);
		matrix m2 = m.inverse();

		REQUIRE(!m2.is_null());
	}
}

TEST_CASE("vandermonde inverse", "[matrix]")
{
	for (size_t sz : sizes)
	{
		matrix m = vandermonde(sz, sz);
		matrix m2 = m.inverse();
		matrix r = m * m2;

		REQUIRE(r == matrix(sz, sz, 1));
	}
}

TEST_CASE("singular matrix", "[matrix]")
{
	for (size_t sz : sizes)
	{
		// Create a singular matrix
		matrix m = matrix(sz, sz, 0);

		REQUIRE(m.inverse().is_null());
	}
}


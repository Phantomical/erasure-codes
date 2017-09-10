#include "liberasure.h"

#include <random>

alignas(64) uint8_t data[data_size * k];
alignas(64) uint8_t result[data_size * k];

uint8_t* ptrs[n];
alignas(64) uint8_t parity[data_size * (n - k)];
erasure_bool present[n];

// Default value from mt19937 argument default value
void generate_data(unsigned seed = 5489u)
{
	std::uniform_int_distribution<unsigned> dist{ 0, 255 };
	std::mt19937 eng{ seed };

	for (size_t i = 0; i < sizeof(data) / sizeof(uint8_t); ++i)
	{
		data[i] = 0x56;//dist(eng);
	}
}

size_t div_roundup(size_t a, size_t b)
{
	return (a + b - 1) / b;
}

void generate_ptrs()
{
	size_t i, j;
	for (i = 0; i < k; ++i)
	{
		ptrs[i] = data + data_size * i;
	}
	for (j = 0; i < n; ++i, ++j)
	{
		ptrs[i] = parity + data_size * j;
	}

	size_t rep = div_roundup(n, n - k);
	for (size_t m = 0; m < n; ++m)
	{
		present[m] = (m % rep) != 0;
	}
}

bool run_test(erasure_encoder_flags flag)
{
	generate_data();
	generate_ptrs();

	erasure_encoder_parameters params = { n, k, data_size };

	erasure_encoder* encoder = erasure_create_encoder(&params, flag);

	erasure_encode(encoder, ptrs, ptrs + k);

	memcpy(result, data, sizeof(data));

	for (size_t i = 0; i < k; ++i)
	{
		if (!present[i])
		{
			memset(ptrs[i], 0, data_size * sizeof(uint8_t));
		}
	}

	erasure_recover(encoder, ptrs, present);

	erasure_destroy_encoder(encoder);

	if (memcmp(data, result, sizeof(data)) != 0)
		return false;
	return true;
}



#include <cstdint>
#include <cstring>
#include <iostream>

uint8_t mul(uint8_t a, uint8_t b)
{
	uint8_t aa = a, bb = b, r = 0, t;

	while (aa != 0)
	{
		if (aa & 1)
			r ^= bb;
		t = bb & 0x80;
		bb <<= 1;
		if (t != 0)
			bb ^= 0x1B;
		aa = (aa & 0xFF) >> 1;
	}

	return r;
}

uint8_t exp_table[256];
uint8_t log_table[256];

void fill_exp()
{
	uint8_t x = 0x01;
	for (size_t i = 0; i < 255; ++i)
	{
		exp_table[i] = x;
		x = mul(x, 0x03);
	}
}
void fill_log()
{
	for (size_t i = 0; i < 255; ++i)
		log_table[exp_table[i] + 1] = i;
}

void print_table(uint8_t* table)
{
	std::cout << "\t{\n";
	for (size_t y = 0; y < 16; ++y)
	{
		std::cout << "\t\t";

		for (size_t x = 0; x < 16; ++x)
		{
			std::cout << (int)table[y * 16 + x] << ", ";
		}

		std::cout << '\n';
	}

	std::cout << "\t};";
}

int main()
{
	std::memset(exp_table, 0, sizeof(exp_table));
	std::memset(log_table, 0, sizeof(log_table));

	fill_exp();
	fill_log();

	std::cout << "#include <cstdint>\n\n";

	std::cout << "namespace gfarith\n{\n"
		<< "\tuint8_t exp_table[] = ";

	print_table(exp_table);

	std::cout << "\n\n";

	std::cout << "\tuint8_t log_table[] = ";

	print_table(log_table);

	std::cout << "}\n" << std::endl;
}
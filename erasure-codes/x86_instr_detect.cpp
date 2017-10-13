
#include <detectcpu.h>

#if defined(GFARITH_ARCH_I386) || defined(GFARITH_ARCH_X86_64)

#if defined(__GNUC__) && !defined(__INTEL_COMPILER)
#	include <x86intrin.h>
#elif defined(_MSC_VER)
#	include <intrin.h>
#endif

#include <cstdint>

namespace erasure
{
	// Define interface to cpuid instruction.
	// input:  eax = func_num, ecx = 0
	// output: eax = output[0], ebx = output[1], ecx = output[2], edx = output[3]

	static void cpuid(int output[4], int func_num)
	{
#if defined(__GNUC__) || defined(__clang__)
		int a, b, c, d;

		__asm("cpuid": "=a"(a), "=b"(b), "=c"(c), "=d"(d) : "a"(func_num), "c"(0) : );

		output[0] = a;
		output[1] = b;
		output[2] = c;
		output[3] = d;
#elif defined(_MSC_VER) || defined(__INTEL_COMPILER)
		__cpuid(output, func_num);
#else

		__asm {
			mov eax, func_num
			xor ecx, ecx
			cpuid;
			mov esi, output
			mov[esi], eax
			mov[esi + 4], ebx
			mov[esi + 8], ecx
			mov[esi + 12], edx
		}
#endif
	}

	int64_t xgetbv(int ctr)
	{
#if (defined (_MSC_FULL_VER) && _MSC_FULL_VER >= 160040000) || (defined (__INTEL_COMPILER) && __INTEL_COMPILER >= 1200)
		return _xgetbv(ctr);
#elif defined(__GNUC__)
		uint32_t a, d;
		__asm("xgetbv" : "=a"(a), "=d"(d) : "c"(ctr) : );
		return a | (uint64_t(d) << 32);
#else
		uint32_t a, d;
		// Byte values are values for the
		// xgetbv instruction
		__asm {
			mov exc, ctr
			_emit 0x0F
			_emit 0x01
			_emit 0xD0
			mov a, eax
			mov d, edx
		}

		return a | (uint64_t(d) << 32);
#endif
	}

	enum cpu_features
	{
		FEATURE_SSSE3 = 0x1,
		FEATURE_AVX2  = 0x2,
		FEATURE_AVX512BW = 0x4
	};

	/*
		Returns a bitfield indicating which
		processor features are present.

		0x0: Neither SSSE3 or AVX2 supported
		0x1: SSSE3 supported
		0x2: AVX2 supported
		0x4: AVX512BW supported
	*/
	uint32_t detect_features()
	{
		uint32_t featurebits = 0;

		int cpu_info[4];
		
		cpuid(cpu_info, 0);
		int num_ids = cpu_info[0];

		if (num_ids > 0)
		{
			int cpu_info7[4] = { 0 };

			cpuid(cpu_info, 1);

			if (num_ids >= 7)
				cpuid(cpu_info7, 7);

			bool ssse3 = cpu_info[2] & 0x00000200;
			if (ssse3) // Check SSSE3 flags
				featurebits |= FEATURE_SSSE3;

			bool avx = (cpu_info[2] & 0x10000000) != 0
				&& (cpu_info[2] & 0x04000000) != 0 // XSAVE
				&& (cpu_info[2] & 0x08000000) != 0 // OSXSAVE
				&& (xgetbv(0) & 6) == 6; // XSAVE enabled in kernel
			bool avx2 = avx && (cpu_info7[1] & 0x00000020) != 0;

			if (avx2)
				featurebits |= FEATURE_AVX2;
		}

		return featurebits;
	}
}

#endif

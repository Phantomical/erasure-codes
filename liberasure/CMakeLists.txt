
file(GLOB sources
	*.c
	*.h
)

include(CheckSymbolExists)
include(CheckIncludeFiles)

if (NOT DEFINED ALLOCA_EXISTS)
	check_symbol_exists(alloca "alloca.h" ALLOCA_IN_ALLOCA_H)
	if (ALLOCA_IN_ALLOCA_H)
		set(ALLOCA_EXISTS TRUE)
	endif()
endif()

if (NOT DEFINED ALLOCA_EXISTS)
	check_symbol_exists(alloca "intrin.h" ALLOCA_IN_INTRIN_H)
	if (ALLOCA_IN_INTRIN_H)
		set(ALLOCA_EXISTS TRUE)
	endif()
endif()

if (NOT DEFINED ALLOCA_EXISTS)
	check_symbol_exists(alloca "stdlib.h" ALLOCA_IN_STDLIB_H)
	if (ALLOCA_IN_STDLIB_H)
		set(ALLOCA_EXISTS TRUE)
	endif()
endif()

check_include_files("immintrin.h" HAVE_IMMINTRIN_H)
check_include_files("intrin.h" HAVE_INTRIN_H)
check_include_files("x86intrin.h" HAVE_X86INTRIN_H)

if (${ALLOCA_EXISTS})
	set(ERASURE_USE_ALLOCA TRUE)
endif()

CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/config.h.in" "${CMAKE_CURRENT_BINARY_DIR}/config.h")

add_library(erasure2 ${sources} "${CMAKE_CURRENT_BINARY_DIR}/config.h")

target_include_directories(erasure2 PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/")

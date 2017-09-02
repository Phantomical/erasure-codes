
file(GLOB sources
	*.cpp
	*.h
)

# Figure out what format flags must be in
if (MINGW OR CYGWIN OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANG OR LINUX)
	set(FLAG_TYPE "Linux")
elseif(MSVC)
	set(FLAG_TYPE "Windows")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
	if (WIN32)
		set (FLAG_TYPE "Windows")
	else()
		set (FLAG_TYPE "Linux")
	endif()
else()
	set (FLAG_TYPE "Unknown")
endif()

if (FLAG_TYPE STREQUAL "Linux")
	set_source_files_properties(encode_sse.cpp PROPERTIES COMPILE_FLAGS " -mssse3")
	set_source_files_properties(encode_avx2.cpp PROPERTIES COMPILE_FLAGS " -mavx2")
elseif(FLAG_TYPE STREQUAL "Windows")
	if (NOT MSVC)
		set_source_files_properties(encode_sse.cpp PROPERTIES COMPILE_FLAGS " /arch:ssse3")
		set_source_files_properties(encode_avx2.cpp PROPERTIES COMPILE_FLAGS " /arch:avx2")
	endif()
else()
	message(WARNING "Unknown compiler! Flags will not be passed for specific files.")
endif()

add_library(gfarithmetic ${sources})
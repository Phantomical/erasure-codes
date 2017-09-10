
file(GLOB sources
	*.cpp
	*.h
)

if (WIN32)
	set (FLAG_TYPE "Windows")
else()
	set (FLAG_TYPE "Linux")
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

# Generate log_tables.cpp
add_custom_command(
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/log_tables.cpp
	COMMAND generate-log-tables log_tables.cpp
)
# Make dependencies work
add_custom_target(log_tables ALL DEPENDS log_tables.cpp)
# log_tables depends on generate-log-tables
add_dependencies(log_tables generate-log-tables)
set_target_properties(log_tables PROPERTIES FOLDER internal-targets)

# gfarithmetic
add_library(gfarithmetic ${sources} log_tables.cpp)

# gfarithmetic needs log_tables
add_dependencies(gfarithmetic log_tables)

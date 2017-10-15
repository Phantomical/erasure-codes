
file(GLOB sources
	*.c
	*.h
)

if (ALLOCA_EXISTS)
	add_definitions(-DERASURE_USE_ALLOCA)
endif()

add_library(erasure2 ${sources})

#ifndef ERASURE_ENCODER_H
#define ERASURE_ENCODER_H

#include "erasure.h"

struct erasure_encoder_
{
	uint8_t* coding_matrix;
	size_t n_data;
	size_t n_shards;
};

#endif

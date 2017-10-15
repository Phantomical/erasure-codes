#ifndef ERASURE_ENCODER_H
#define ERASURE_ENCODER_H

#include "erasure.h"

struct erasure_encoder_
{
	uint8_t* coding_matrix;
	size_t num_data;
	size_t num_shards;
	size_t shards_required_for_recovery;
};

#endif

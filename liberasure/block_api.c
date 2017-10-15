
#include "erasure.h"
#include "erasure_encoder.h"

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#ifndef ERASURE_USE_ALLOCA
#	define stackalloc(size) malloc(size)
#	define stackfree(ptr) free(ptr)
#else
#	ifdef _MSC_VER
#		include <intrin.h>
#	else
#		include <alloca.h>
#	endif
#
#	define stackalloc(size) alloca(size)
#	define stackfree(ptr)
#endif

typedef enum erasure_error_code error_code;

bool all_nonnull(const uint8_t* const* array, size_t size)
{
	if (!array)
		return false;

	for (size_t i = 0; i < size; ++i)
	{
		if (!array[i])
			return false;
	}

	return true;
}

erasure_bool erasure_can_recover_data(
	erasure_encoder* encoder,
	const erasure_bool* present)
{
	if (!encoder || !present)
		return false;

	size_t n_present = 0;
	for (size_t i = 0; i < encoder->num_shards; ++i)
		n_present += (bool)present[i];

	return n_present < encoder->shards_required_for_recovery;
}

error_code erasure_encode_parity(
	erasure_encoder* encoder,
	const uint8_t* const* data_shards,
	uint8_t* const* parity_shards,
	size_t shard_size)
{
	if (!encoder)
		return ERASURE_INVALID_ARGUMENTS;
	
	const size_t n_parity = encoder->num_shards - encoder->num_data;

	if (!all_nonnull(data_shards, encoder->num_data) ||
		!all_nonnull(parity_shards, n_parity))
		return ERASURE_INVALID_ARGUMENTS;
	
	erasure_bool* should_encode = stackalloc(
		sizeof(erasure_bool) * n_parity);

	if (!should_encode)
		return ERASURE_INTERNAL_ERROR;

	memset(should_encode, 1, sizeof(erasure_bool) * n_parity);

	error_code err = erasure_encode_partial(
		encoder,
		data_shards,
		parity_shards,
		should_encode,
		shard_size);

	stackfree(should_encode);

	return err;
}

enum erasure_error_code erasure_encode_partial(
	erasure_encoder* encoder,
	const uint8_t* const* data_shards,
	uint8_t* const* parity_shards,
	const erasure_bool* should_encode,
	size_t shard_size)
{
	if (!encoder || !should_encode)
		return ERASURE_INVALID_ARGUMENTS;

	const size_t n_parity = encoder->num_shards - encoder->num_data;

	if (!all_nonnull(data_shards, encoder->num_data) ||
		!all_nonnull(parity_shards, n_parity))
		return ERASURE_INVALID_ARGUMENTS;
	
	erasure_encode_stream* stream = erasure_create_encode_stream(
		encoder,
		should_encode,
		shard_size);

	if (!stream)
		return ERASURE_INTERNAL_ERROR;

	error_code err = erasure_stream_encode(
		stream,
		data_shards,
		parity_shards);

	erasure_destroy_encode_stream(stream);

	return err;
}

enum erasure_error_code erasure_recover_data(
	erasure_encoder* encoder,
	uint8_t* const* shards,
	const erasure_bool* present,
	size_t shard_size)
{
	// Check arguments
	if (!encoder || !shards || !present)
		return ERASURE_INVALID_ARGUMENTS;
	if (!all_nonnull(shards, encoder->num_shards))
		return ERASURE_INVALID_ARGUMENTS;
	
	// Check that recovery is possible
	if (erasure_can_recover_data(encoder, present))
		return ERASURE_RECOVER_FAILED;

	erasure_recover_stream* stream = erasure_create_recover_stream(
		encoder,
		present,
		shard_size);

	// Check that stream creation went OK
	if (!stream)
		return ERASURE_INTERNAL_ERROR;

	error_code err = erasure_stream_recover_data(
		stream,
		shards);

	erasure_destroy_recover_stream(stream);

	return err;
}


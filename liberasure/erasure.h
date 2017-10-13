#ifndef LIBERASURE_
#define LIBERASURE_H

#include <stdlib.h>
#include <stdint.h>

typedef char erasure_bool;

enum erasure_error_code
{
	ERASURE_SUCCESS = 0,
	ERASURE_RECOVER_FAILED,
	ERASURE_INVALID_ARGUMENTS,
	ERASURE_INTERNAL_ERROR
};

/* BLOCK API */
typedef struct erasure_encoder_ erasure_encoder;

/* Encodes parity shards using the given encoder
   and data shards. The number of data shards and
   the number of parity shards is determined by
   the encoder itself and should be set at encoder
   creation time.

   This function will return ERASURE_INVALID_ARGUMENTS
   if any argument is null, or any pointer within the
   set of data shards or parity shards is null.
*/
enum erasure_error_code erasure_encode_parity(
	erasure_encoder* encoder,
	const uint8_t* const* data_shards,
	uint8_t* const* parity_shards,
	size_t shard_size);

/* Selectively encodes parity shards with the given
   encoder and data shards. The number of data and
   parity shards is determined by the encoder and is
   set at encoder creation time. should_encode is a
   boolean array which indicates whether to encode 
   a given parity shard, it has the same number of 
   elements as there are parity shards.

   This function will return ERASURE_INVALID_ARGUMENTS
   if any argument is null, or any data shard or 
   parity shard is null.
*/
enum erasure_error_code erasure_encode_partial(
	erasure_encoder* encoder,
	const uint8_t* const* data_shards,
	uint8_t* const* parity_shards,
	const erasure_bool* should_encode,
	size_t shard_size);

/* Recovers data shards, if possible, using the given
   encoder. shards is the set of all shards, both data
   shards and parity shards. The number of shards is 
   set at encoder creation time. present indicates 
   which shards have been erased and all erased data
   shards will be recovered if recovery is possible.

   This function will return ERASURE_INVALID_ARGUMENTS
   if any argument is null, or any shard is null.

   This function will return ERASURE_RECOVER_FAILED
   if it is unable to recover data.
*/
enum erasure_error_code erasure_recover_data(
	erasure_encoder* encoder,
	uint8_t* const* shards,
	const erasure_bool* present,
	size_t shard_size);

/* STREAM API */
typedef struct erasure_encode_stream_ erasure_encode_stream;
typedef struct erasure_recover_stream_ erasure_recover_stream;

/* Encodes parity shards in fixed blocks using an
   encoder embedded within the stream. Which parity
   shards to encode and block size is determined
   by the stream.
   
   Using the streaming API removes overhead 
   associated with doing redundant matrix operations
   required to encode and decode each block, instead
   they can be done once at stream creation time and
   reused for each block.
   
   This function returns ERASURE_INVALID_ARGUMENTS
   if any argument or shard is null.
*/
enum erasure_error_code erasure_stream_encode(
	erasure_encode_stream* stream,
	const uint8_t* const* shards,
	uint8_t* const* parity);

/* Recalculates erased data shards using the provided
   erasure encoder embedded within the stream. Number
   of shards is determined by the recover stream as 
   well as block size. 
   
   Using the streaming API removes overhead 
   associated with doing redundant matrix operations
   required to encode and decode each block, instead
   they can be done once at stream creation time and
   reused for each block.
   
   This function will never return 
   ERASURE_RECOVER_FAILED since it should not be
   possible to create an invalid recovery stream.

   This function will return ERASURE_INVALID_ARGUMENTS
   if any of the arguments is null or any shard is null.
*/
enum erasure_error_code erasure_stream_recover_data(
	erasure_recover_stream* stream,
	uint8_t* const* shards);

#endif

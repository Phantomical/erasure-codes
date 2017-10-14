#ifndef ERASURE_MATRIX_
#define ERASURE_MATRIX_H

#include <stdint.h>
#include <stdbool.h>

struct matrix
{
	uint8_t* data;
	size_t rows;
	size_t cols;
};

/* Multiplies two matrices to create the 
   resulting matrix ab. If the number of
   rows in a is not equal to the number
   of columns in b, then returns an empty
   matrix.
*/
struct matrix matrix_mul(
	struct matrix a,
	struct matrix b);

/* Determines the inverse matrix m^(-1) of
   the matrix m. If m is singular then it 
   returns false, otherwise it returns true.
*/
bool matrix_inverse(
	struct matrix m);

/* Creates the augmented matrix [a|b].

   If the number or rows in a is not equal to
   the number of columns in b, then it returns
   an empty matrix.
*/
struct matrix matrix_augment(
	struct matrix a,
	struct matrix b);

#endif

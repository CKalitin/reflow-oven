// To prevent recurrsive inclusion
#pragma once

#include <stddef.h>
#include <stdint.h>

// Thx Claude

/**
 * Convert a character to its uint8_t representation.
 * 
 * @param c The input character to convert
 * @param result Pointer to uint8_t where the result will be stored
 * @return 1 if conversion is successful, 0 otherwise
 */
int char_to_uint8(char c, uint8_t *result);
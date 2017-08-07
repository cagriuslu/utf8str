/// \file
/// This file defines utf8char_t, which is used by utf8str_t.

#ifndef UTF8CHAR_H
#define UTF8CHAR_H

#include <stdint.h>
#include <stdlib.h>

/// \typedef utf8char_t
/// \brief A type representing an UTF-8 character
/// \details utf8char_t is actually a 32-bit unsigned number.
/// The fixed size makes the processing easier.
typedef uint32_t utf8char_t;

/// \brief Returns the real size of a given utf8char_t (which might be 1, 2, 3, or 4)
/// \return ((size_t) -1), if **c** is not a valid UTF-8 character.
size_t utf8char_size(utf8char_t c)
{
	if (c < 128)
		return 1;
	else if (c < 57344 && 128 <= (c & 0x000000ff) && (c & 0x000000ff) < 192)
		return 2;
	else if (c < 15728640 && 128 <= (c & 0x000000ff) && (c & 0x000000ff) < 192 && 32768 <= (c & 0x0000ff00) && (c & 0x0000ff00) < 49152)
		return 3;
	else if (c < 4160749568 && 128 <= (c & 0x000000ff) && (c & 0x000000ff) < 192 && 32768 <= (c & 0x0000ff00) && (c & 0x0000ff00) < 49152 && 8388608 <= (c & 0x00ff0000) && (c & 0x00ff0000) < 12582912)
		return 4;
	else
		return ((size_t) -1);
}

/// \brief Tries to deduce the real size of the UTF-8 character (which might be 1, 2, 3, or 4), by looking at its first byte
/// \param b First byte of a UTF-8 character
/// \return 0, if **b** is not the first byte of a valid UTF-8 character but might be one of the following bytes.
/// \return ((size_t) -1), if **b** is definetly not a part of a valid UTF-8 character.
size_t deduce_utf8char_size(uint8_t b)
{
	if (b < 128)
		return 1;
	else if (b < 192)
		return 0;
	else if (b < 224)
		return 2;
	else if (b < 240)
		return 3;
	else if (b < 248)
		return 4;
	else
		return ((size_t) -1);
}

/// \brief Tries to parse a utf8char_t from the given string
/// \return ((utf8char_t) -1), if the string does not contain a valid UTF-8 character at the beginning.
utf8char_t str_to_utf8char(char *s)
{
	size_t deduced_size = deduce_utf8char_size(s[0]);
	if (deduced_size == 0 || deduced_size == ((size_t) -1))
		return ((utf8char_t) -1);

	utf8char_t c = 0;
	size_t i;
	for (i = 0; i < deduced_size; i++)
	{
		uint32_t tmp = ((uint8_t) s[i]);
		c |= tmp << (8 * (deduced_size - 1 - i));
	}

	if (utf8char_size(c) == ((size_t) -1))
		return ((utf8char_t) -1);

	return c;
}

/// \brief Writes a utf8char_t into a buffer
/// \return The number of bytes written, if **c** is a valid UTF-8 character.
/// \return ((size_t) -1), if **c** is not a valid UTF-8 character. The buffer is left untouched.
size_t write_utf8char_to_buf(char *buf, utf8char_t c)
{
	size_t size = utf8char_size(c);
	if (size == ((size_t) -1))
		return size;

	size_t i;
	for (i = 0; i < size; i++)
	{
		uint8_t tmp = c >> (8 * (size - 1 - i));
		buf[i] = tmp;
	}

	return size;
}

#endif

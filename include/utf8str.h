/// \mainpage
/// utf8str is a small library for easy UTF-8 encoded string processing.
///
/// \section Installation
/// This library is a header-only library, just copy it anywhere your compiler can find.
///
/// \section Usage
/// Read the documentation of the file "utf8str.h" and include it in your code.
///
/// \section License
/// This library is released under the MIT License.

/// \file
/// This file defines utf8str_t.

#ifndef UTF8STR_H
#define UTF8STR_H

#include "utf8char.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define UTF8STR_VERSION_MAJOR (0)
#define UTF8STR_VERSION_MINOR (1)
#define UTF8STR_VERSION_MICRO (0)

/// \typedef utf8str_t
/// \brief A type representing a string of UTF-8 characters
/// \details utf8str_t is actually an array of utf8char_t's.
typedef utf8char_t* utf8str_t;

/// \brief Returns the number of UTF-8 characters (not including the terminating zero) in a string
size_t utf8str_len(utf8str_t s)
{
	size_t len = 0;
	while (1)
	{
		if (s[len] == 0)
			return len;
		else
			len++;
	}
}

/// \brief Copies characters from one string to another
/// \param char_count The number of characters to copy
/// \return **dst**
utf8str_t copy_utf8str_n(utf8str_t dst, utf8str_t src, size_t char_count)
{
	return memcpy(dst, src, char_count * sizeof(utf8char_t));
}

/// \brief Copies characters from one string to another, up to and including the terminating zero
/// \return **dst**
utf8str_t copy_utf8str(utf8str_t dst, utf8str_t src)
{
	return copy_utf8str_n(dst, src, utf8str_len(src) + 1);
}

/// \brief Returns the number of bytes (including the terminating zero) the string would occupy in a buffer or a file
/// \return ((size_t) -1), if **s** contains an invalid UTF-8 character.
size_t utf8str_buf_size(utf8str_t s)
{
	size_t size = 0;
	size_t i = 0;
	while (1)
	{
		if (s[i] == 0)
		{
			return size + utf8char_size(0);
		}
		else
		{
			size_t tmp = utf8char_size(s[i]);
			if (tmp == ((size_t) -1))
				return tmp;
			size += tmp;
			i++;
		}
	}
}

/// \brief Returns the number of bytes the string would occupy in a buffer or a file
/// \details This function may return before reaching **char_count** if terminating zero is met.
/// \param char_count The number of characters to calculate the size of
/// \return ((size_t) -1), if **s** contains an invalid UTF-8 character.
size_t utf8str_buf_size_n(utf8str_t s, size_t char_count)
{
	size_t size = 0;
	size_t i;
	for (i = 0; i < char_count; i++)
	{
		if (s[i] == 0)
		{
			return size + utf8char_size(0);
		}
		else
		{
			size_t tmp = utf8char_size(s[i]);
			if (tmp == ((size_t) -1))
				return tmp;
			size += tmp;
		}
	}
	return size;
}

/// \brief Returns the number of bytes (including the terminating zero) the string would occupy in a buffer or a file
/// \details This function returns the number of bytes up to the invalid UTF-8 character, if **s** contains one.
size_t inval_utf8str_buf_size(utf8str_t s)
{
	size_t size = 0;
	size_t i = 0;
	while (1)
	{
		if (s[i] == 0)
		{
			return size + utf8char_size(0);
		}
		else
		{
			size_t tmp = utf8char_size(s[i]);
			if (tmp == ((size_t) -1))
				return size;
			size += tmp;
			i++;
		}
	}
}

/// \brief Writes a string into a buffer
/// \details The buffer should be large enough to hold the whole string. The required buffer size can be found using utf8str_buf_size(**s**).
/// \return The number of bytes actually written, if **s** does not contain an invalid UTF-8 character.
/// \return ((size_t) -1), if **s** contains invalid UTF-8 character. The number of bytes already written into the buffer can be found using inval_utf8str_buf_size(**s**).
size_t write_utf8str_to_buf(char *buf, utf8str_t s)
{
	size_t size = 0;
	size_t i = 0;
	while (1)
	{
		if (s[i] == 0)
		{
			return size + write_utf8char_to_buf(buf + size, 0);
		}
		else
		{
			size_t tmp = write_utf8char_to_buf(buf + size, s[i]);
			if (tmp == ((size_t) -1))
				return tmp;
			size += tmp;
			i++;
		}
	}
}

/// \brief Writes a string into a buffer
/// \details The buffer should be large enough to hold utf8str_buf_size_n(**s**, **char_count**) bytes. This function may return before reaching **char_count** if terminating zero is met.
/// \param char_count The number of characters to write
/// \return The number of bytes actually written, if **s** does not contain an invalid UTF-8 character.
/// \return ((size_t) -1), if **s** contains invalid UTF-8 character. The number of bytes already written into the buffer can be found using inval_utf8str_buf_size(**s**).
size_t write_utf8str_to_buf_n(char *buf, utf8str_t s, size_t char_count)
{
	size_t size = 0;
	size_t i;
	for (i = 0; i < char_count; i++)
	{
		if (s[i] == 0)
		{
			return size + write_utf8char_to_buf(buf + size, 0);
		}
		else
		{
			size_t tmp = write_utf8char_to_buf(buf + size, s[i]);
			if (tmp == ((size_t) -1))
				return tmp;
			size += tmp;
		}
	}
	return size;
}

/// \brief Reads from a buffer into a string
/// \details If **buf** contains an invalid UTF-8 character or a half one, the process is terminated without further processing. This function may return before reaching **bufsize** if terminating zero is met.
/// \return The number of bytes actually consumed.
size_t read_buf_to_utf8str(utf8str_t s, char *buf, size_t bufsize)
{
	size_t chars_consumed = 0;
	size_t i;
	for (i = 0; i < bufsize;)
	{
		size_t deduced_size = deduce_utf8char_size(buf[i]);
		if (deduced_size == 0 || deduced_size == ((size_t) -1) || i + deduced_size > bufsize)
			return i;

		utf8char_t c = str_to_utf8char(buf + i);
		if (c == ((utf8char_t) -1))
			return i;


		s[chars_consumed++] = c;
		i += deduced_size;

		if (c == 0)
			break;
	}
	return bufsize;
}

#endif

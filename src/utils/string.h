/*
 * This file is subject to the terms and conditions defined in
 * file 'COPYING', which is part of this source code package.
 */

#ifndef __STRING_H_
#define __STRING_H_

// use a cache for regular expressions so that you don't have to recompile them each time (a la perl)
#define STRING_REGEX_USE_CACHE

#include "utils/common.h"
#include "utils/vector.h"
#include "utils/array.h"

#ifdef HAVE_LIBPCRE // extended perl like regular expressions
#include <pcre.h>
#define USE_PCRE
#else
#include <regex.h>
#endif

#include <stdio.h>

typedef struct string {
	size_t length;
	size_t size;
	char* data;
} string_t;

string_t* string_resize(string_t* input,size_t newSize);
string_t* string_new(const char* string);
string_t* string_new_empty();
string_t* string_new_from_to(const char* string,size_t from, size_t to);
string_t* string_copy(string_t* input);
string_t* string_append_cstr(string_t* input, const char* peer);
string_t* string_prepend_cstr(string_t* input, const char* peer);
string_t* string_append(string_t* input, string_t* peer);
string_t* string_prepend(string_t* input, string_t* peer);
void string_free(string_t* input);
string_t* string_substr(string_t* input,size_t from,size_t to);
string_t* string_reverse(string_t* input);
string_t* string_chomp(string_t* input);
array_t* string_array_chomp(array_t* input);
string_t* string_join(string_t* separator, array_t* parts);
string_t* string_join_cstr(const char* separator, array_t* parts);
vector_t* string_match(string_t* input, const char* pattern, const char* flags);
array_t* string_split(string_t* input, const char* separator, const char* flags);
array_t* string_array_grep(array_t* input, const char* pattern, const char* flags);
int string_replace(string_t* input, const char* pattern, const char* replacement, const char* flags);
void string_array_free(array_t* input);
void string_vector_free(vector_t* input);
int32_t string_to_int32(string_t* input);
int64_t string_to_int64(string_t* input);
double string_to_double(string_t* input);
float string_to_float(string_t* input);
string_t* string_sprintf(const char* format, ...);
void string_array_fprintf(FILE* stream, const char* format, array_t* array);
array_t* string_argv_to_array(int argc, char** argv);
#define string_cmp(a,b) string_compare(a,b)
int string_compare(string_t* a, string_t* b);
#define string_eq(a,b) string_equal(a,b)
int string_equal(string_t* a, string_t* b);
#define string_ne(a,b) string_not_equal(a,b)
int string_not_equal(string_t* a, string_t* b);
#define string_cmp_cstr(a,b) string_compare_cstr(a,b)
int string_compare_cstr(string_t* a, const char* b);
#define string_eq_cstr(a,b) string_equal_cstr(a,b)
int string_equal_cstr(string_t* a, const char* b);
#define string_ne_cstr(a,b) string_not_equal_cstr(a,b)
int string_not_equal_cstr(string_t* a, const char* b);
string_t* string_readline(FILE* file);
#endif

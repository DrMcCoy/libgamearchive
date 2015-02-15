/**
 * @file   test-filter-ddave-rle.cpp
 * @brief  Test code for Dangerous Dave RLE algorithm.
 *
 * Copyright (C) 2010-2015 Adam Nielsen <malvineous@shikadi.net>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <boost/test/unit_test.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/copy.hpp>
#include <camoto/util.hpp>
#include "test-filter.hpp"
#include "../src/filter-ddave-rle.hpp"

#define rle_130 \
	"\x7F\x00"
#define rle_1040 \
	rle_130 rle_130 rle_130 rle_130 rle_130 rle_130 rle_130 rle_130
#define rle_4030 \
	rle_1040 rle_1040 rle_1040 \
	rle_130 rle_130 rle_130 rle_130 rle_130 rle_130 rle_130

#define dat_130 \
	"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" \
	"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" \
	"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" \
	"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" \
	"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" \
	"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" \
	"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" \
	"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" \
	"\x00\x00"
#define dat_1040 \
	dat_130 dat_130 dat_130 dat_130 dat_130 dat_130 dat_130 dat_130
#define dat_4030 \
	dat_1040 dat_1040 dat_1040 \
	dat_130 dat_130 dat_130 dat_130 dat_130 dat_130 dat_130

#define dat_128e \
	"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02" \
	"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02" \
	"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02" \
	"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02" \
	"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02" \
	"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02" \
	"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02" \
	"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
#define dat_1024e \
	dat_128e dat_128e dat_128e dat_128e dat_128e dat_128e dat_128e dat_128e
#define dat_3968e \
	dat_1024e dat_1024e dat_1024e \
	dat_128e dat_128e dat_128e dat_128e dat_128e dat_128e dat_128e

#define rle_128e \
	"\xFF" dat_128e
#define rle_1024e \
	rle_128e rle_128e rle_128e rle_128e rle_128e rle_128e rle_128e rle_128e
#define rle_3968e \
	rle_1024e rle_1024e rle_1024e \
	rle_128e rle_128e rle_128e rle_128e rle_128e rle_128e rle_128e

using namespace camoto;
using namespace camoto::gamearchive;

struct ddave_rle_sample: public test_filter {
	ddave_rle_sample()
	{
		this->filter.reset(new filter_ddave_rle());
	}
};

struct ddave_unrle_sample: public test_filter {
	ddave_unrle_sample()
	{
		this->filter.reset(new filter_ddave_unrle());
	}
};

#define DATA_ENCODED \
	"\x80\xAA\x00\x00\x81\xBB\xCC\x01\xDD" \
	"\x7F\x11" \
	"\x7E\x12" \
	"\xFE" \
	"\x31\x32\x31\x32\x31\x32\x31\x32" \
	"\x31\x32\x31\x32\x31\x32\x31\x32" \
	"\x31\x32\x31\x32\x31\x32\x31\x32" \
	"\x31\x32\x31\x32\x31\x32\x31\x32" \
	"\x31\x32\x31\x32\x31\x32\x31\x32" \
	"\x31\x32\x31\x32\x31\x32\x31\x32" \
	"\x31\x32\x31\x32\x31\x32\x31\x32" \
	"\x31\x32\x31\x32\x31\x32\x31\x32" \
	"\x31\x32\x31\x32\x31\x32\x31\x32" \
	"\x31\x32\x31\x32\x31\x32\x31\x32" \
	"\x31\x32\x31\x32\x31\x32\x31\x32" \
	"\x31\x32\x31\x32\x31\x32\x31\x32" \
	"\x31\x32\x31\x32\x31\x32\x31\x32" \
	"\x31\x32\x31\x32\x31\x32\x31\x32" \
	"\x31\x32\x31\x32\x31\x32\x31\x32" \
	"\x31\x32\x31\x32\x31\x32\x31" \
	"\x03\x44" \
	"\xFF" \
	"\x21\x22\x21\x22\x21\x22\x21\x22" \
	"\x21\x22\x21\x22\x21\x22\x21\x22" \
	"\x21\x22\x21\x22\x21\x22\x21\x22" \
	"\x21\x22\x21\x22\x21\x22\x21\x22" \
	"\x21\x22\x21\x22\x21\x22\x21\x22" \
	"\x21\x22\x21\x22\x21\x22\x21\x22" \
	"\x21\x22\x21\x22\x21\x22\x21\x22" \
	"\x21\x22\x21\x22\x21\x22\x21\x22" \
	"\x21\x22\x21\x22\x21\x22\x21\x22" \
	"\x21\x22\x21\x22\x21\x22\x21\x22" \
	"\x21\x22\x21\x22\x21\x22\x21\x22" \
	"\x21\x22\x21\x22\x21\x22\x21\x22" \
	"\x21\x22\x21\x22\x21\x22\x21\x22" \
	"\x21\x22\x21\x22\x21\x22\x21\x22" \
	"\x21\x22\x21\x22\x21\x22\x21\x22" \
	"\x21\x22\x21\x22\x21\x22\x21\x22"

// Should have no effect on the decoded data when following DATA_ENCODED
#define DATA_BAD_TRAIL "\x00"

#define DATA_DECODED \
	"\xAA\x00\x00\x00\xBB\xCC\xDD\xDD\xDD\xDD" \
	"\x11\x11\x11\x11\x11\x11\x11\x11" \
	"\x11\x11\x11\x11\x11\x11\x11\x11" \
	"\x11\x11\x11\x11\x11\x11\x11\x11" \
	"\x11\x11\x11\x11\x11\x11\x11\x11" \
	"\x11\x11\x11\x11\x11\x11\x11\x11" \
	"\x11\x11\x11\x11\x11\x11\x11\x11" \
	"\x11\x11\x11\x11\x11\x11\x11\x11" \
	"\x11\x11\x11\x11\x11\x11\x11\x11" \
	"\x11\x11\x11\x11\x11\x11\x11\x11" \
	"\x11\x11\x11\x11\x11\x11\x11\x11" \
	"\x11\x11\x11\x11\x11\x11\x11\x11" \
	"\x11\x11\x11\x11\x11\x11\x11\x11" \
	"\x11\x11\x11\x11\x11\x11\x11\x11" \
	"\x11\x11\x11\x11\x11\x11\x11\x11" \
	"\x11\x11\x11\x11\x11\x11\x11\x11" \
	"\x11\x11\x11\x11\x11\x11\x11\x11" \
	"\x11\x11" \
	"\x12\x12\x12\x12\x12\x12\x12\x12" \
	"\x12\x12\x12\x12\x12\x12\x12\x12" \
	"\x12\x12\x12\x12\x12\x12\x12\x12" \
	"\x12\x12\x12\x12\x12\x12\x12\x12" \
	"\x12\x12\x12\x12\x12\x12\x12\x12" \
	"\x12\x12\x12\x12\x12\x12\x12\x12" \
	"\x12\x12\x12\x12\x12\x12\x12\x12" \
	"\x12\x12\x12\x12\x12\x12\x12\x12" \
	"\x12\x12\x12\x12\x12\x12\x12\x12" \
	"\x12\x12\x12\x12\x12\x12\x12\x12" \
	"\x12\x12\x12\x12\x12\x12\x12\x12" \
	"\x12\x12\x12\x12\x12\x12\x12\x12" \
	"\x12\x12\x12\x12\x12\x12\x12\x12" \
	"\x12\x12\x12\x12\x12\x12\x12\x12" \
	"\x12\x12\x12\x12\x12\x12\x12\x12" \
	"\x12\x12\x12\x12\x12\x12\x12\x12" \
	"\x12" \
	"\x31\x32\x31\x32\x31\x32\x31\x32" \
	"\x31\x32\x31\x32\x31\x32\x31\x32" \
	"\x31\x32\x31\x32\x31\x32\x31\x32" \
	"\x31\x32\x31\x32\x31\x32\x31\x32" \
	"\x31\x32\x31\x32\x31\x32\x31\x32" \
	"\x31\x32\x31\x32\x31\x32\x31\x32" \
	"\x31\x32\x31\x32\x31\x32\x31\x32" \
	"\x31\x32\x31\x32\x31\x32\x31\x32" \
	"\x31\x32\x31\x32\x31\x32\x31\x32" \
	"\x31\x32\x31\x32\x31\x32\x31\x32" \
	"\x31\x32\x31\x32\x31\x32\x31\x32" \
	"\x31\x32\x31\x32\x31\x32\x31\x32" \
	"\x31\x32\x31\x32\x31\x32\x31\x32" \
	"\x31\x32\x31\x32\x31\x32\x31\x32" \
	"\x31\x32\x31\x32\x31\x32\x31\x32" \
	"\x31\x32\x31\x32\x31\x32\x31" \
	"\x44\x44\x44\x44\x44\x44" \
	"\x21\x22\x21\x22\x21\x22\x21\x22" \
	"\x21\x22\x21\x22\x21\x22\x21\x22" \
	"\x21\x22\x21\x22\x21\x22\x21\x22" \
	"\x21\x22\x21\x22\x21\x22\x21\x22" \
	"\x21\x22\x21\x22\x21\x22\x21\x22" \
	"\x21\x22\x21\x22\x21\x22\x21\x22" \
	"\x21\x22\x21\x22\x21\x22\x21\x22" \
	"\x21\x22\x21\x22\x21\x22\x21\x22" \
	"\x21\x22\x21\x22\x21\x22\x21\x22" \
	"\x21\x22\x21\x22\x21\x22\x21\x22" \
	"\x21\x22\x21\x22\x21\x22\x21\x22" \
	"\x21\x22\x21\x22\x21\x22\x21\x22" \
	"\x21\x22\x21\x22\x21\x22\x21\x22" \
	"\x21\x22\x21\x22\x21\x22\x21\x22" \
	"\x21\x22\x21\x22\x21\x22\x21\x22" \
	"\x21\x22\x21\x22\x21\x22\x21\x22"

BOOST_FIXTURE_TEST_SUITE(ddave_unrle_suite, ddave_unrle_sample)

BOOST_AUTO_TEST_CASE(decode)
{
	BOOST_TEST_MESSAGE("Un-RLE some data");

	*this->in << STRING_WITH_NULLS(DATA_ENCODED);

	BOOST_CHECK_MESSAGE(is_equal(STRING_WITH_NULLS(DATA_DECODED)),
		"Decoding RLE data failed");
}

BOOST_AUTO_TEST_CASE(decoder_trail)
{
	BOOST_TEST_MESSAGE("Un-RLE some data, with trailing garbage");

	*this->in << STRING_WITH_NULLS(DATA_ENCODED DATA_BAD_TRAIL);

	BOOST_CHECK_MESSAGE(is_equal(STRING_WITH_NULLS(DATA_DECODED)),
		"Decoding RLE data with trailing incomplete code was not ignored");

	BOOST_CHECK_MESSAGE(this->test_main::is_equal(STRING_WITH_NULLS(DATA_ENCODED
		DATA_BAD_TRAIL), this->in->data),
		"Decoding RLE data corrupted the source data");
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(ddave_rle_suite, ddave_rle_sample)

BOOST_AUTO_TEST_CASE(encode)
{
	BOOST_TEST_MESSAGE("RLE some data");

	*this->in << STRING_WITH_NULLS(DATA_DECODED);

	BOOST_CHECK_MESSAGE(is_equal(STRING_WITH_NULLS(DATA_ENCODED)),
		"Encoding RLE data failed");
}

BOOST_AUTO_TEST_CASE(encode_repeat)
{
	BOOST_TEST_MESSAGE("RLE some data ending with repeated bytes");

	*this->in << STRING_WITH_NULLS(DATA_DECODED "\x45\x45");

	BOOST_CHECK_MESSAGE(is_equal(STRING_WITH_NULLS(DATA_ENCODED "\x81\x45\x45")),
		"Encoding RLE data failed");
}

BOOST_AUTO_TEST_CASE(encode_repeat_many)
{
	BOOST_TEST_MESSAGE("RLE some data ending with many repeated bytes");

	*this->in << STRING_WITH_NULLS(DATA_DECODED "\x45\x45\x45\x45\x45");

	BOOST_CHECK_MESSAGE(is_equal(STRING_WITH_NULLS(DATA_ENCODED "\x02\x45")),
		"Encoding RLE data failed");
}

BOOST_AUTO_TEST_CASE(encode_repeat_exact)
{
	BOOST_TEST_MESSAGE("RLE some data ending with three repeated bytes");

	*this->in << STRING_WITH_NULLS(DATA_DECODED "\x00\x00\x00\xFF\xFF\xFF");

	BOOST_CHECK_MESSAGE(is_equal(STRING_WITH_NULLS(DATA_ENCODED "\x00\x00\x00\xFF")),
		"Encoding RLE data failed");
}

BOOST_AUTO_TEST_CASE(encode_escape_double00)
{
	BOOST_TEST_MESSAGE("RLE some data with escaped duplicate bytes");

	*this->in << STRING_WITH_NULLS("\x9E\x00\x00\x00\x7C\x02\x00\x00\x7C\x03\x00\x00");

	BOOST_CHECK_MESSAGE(is_equal(STRING_WITH_NULLS("\x80\x9E\x00\x00\x87\x7C\x02\x00\x00\x7C\x03\x00\x00")),
		"Encoding RLE data failed");
}

BOOST_AUTO_TEST_CASE(encode_escape_exact)
{
	BOOST_TEST_MESSAGE("RLE some data with one full-sized escaped section");

	*this->in << STRING_WITH_NULLS(
		"\x00\x00\x00\x00"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
	);

	BOOST_CHECK_MESSAGE(is_equal(STRING_WITH_NULLS(
		"\x01\x00"
		"\xFF"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
	)),
		"Encoding RLE data failed"
	);
}

BOOST_AUTO_TEST_CASE(encode_escape_overflow)
{
	BOOST_TEST_MESSAGE("RLE some data with large escaped sections");

	*this->in << STRING_WITH_NULLS(
		"\x00\x00\x00\x00"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
	);

	BOOST_CHECK_MESSAGE(is_equal(STRING_WITH_NULLS(
		"\x01\x00"
		"\xFF"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\xFF"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\xFF"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
	)),
		"Encoding RLE data failed"
	);
}

BOOST_AUTO_TEST_CASE(encode_escape_overflow_shrink)
{
	BOOST_TEST_MESSAGE("RLE some data with RLE spanning large escaped sections");

	*this->in << STRING_WITH_NULLS(
		"\x00\x00\x00\x00"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01"
		"\x00\x00\x00"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
	);

	BOOST_CHECK_MESSAGE(is_equal(STRING_WITH_NULLS(
		"\x01\x00"
		"\xFE"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01"
		"\x00\x00"
		"\xFF"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\xFD"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
		"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
	)),
		"Encoding RLE data failed"
	);
}

BOOST_AUTO_TEST_CASE(encode_splitbuf_count)
{
	BOOST_TEST_MESSAGE("RLE repeated data through 4k stream buffer");

	BOOST_REQUIRE_MESSAGE(BUFFER_SIZE <= 4096,
		"This test only works on buffers <= 4096 bytes");

	*this->in << STRING_WITH_NULLS(
		dat_4030
		dat_130
	);

	BOOST_CHECK_MESSAGE(is_equal(STRING_WITH_NULLS(
		rle_4030
		rle_130
	)),
		"Encoding RLE data failed when split between 4k stream buffers"
	);
}

BOOST_AUTO_TEST_CASE(encode_splitbuf_escape)
{
	BOOST_TEST_MESSAGE("RLE escaped data through 4k stream buffer");

	BOOST_REQUIRE_MESSAGE(BUFFER_SIZE <= 4096,
		"This test only works on buffers <= 4096 bytes");

#define rle_48 \
	"\x2D\x00"
#define dat_48 \
	"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" \
	"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" \
	"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"

	*this->in << STRING_WITH_NULLS(
		dat_4030
		dat_48
		"\xCB\xCB\xCB"
		"\xD6\xCE\xCE\xCF\xCE\xCE\xD2\xD6\xCC\xCC\x87\x86\x85\x84\x83\x83"
		"\x81\x81\x80\x81\x82\x83\x83\x84\x85\x86\x87\x87\x86\x85\x84\x83"
		"\x81\x81\x80\x80\x82\x82\x83\x85\x86\x87\x87\x86\x85\x85\x84\x83"
		"\x82\x80\x80\x81\x82\x83\x84\x85\x85\x87\x87\x87\x86\x84\x83\x83"
	);

	BOOST_CHECK_MESSAGE(is_equal(STRING_WITH_NULLS(
		rle_4030
		rle_48
		"\x00\xCB"
		"\xB8"
		"\xD6\xCE\xCE\xCF\xCE\xCE\xD2\xD6\xCC\xCC\x87\x86\x85\x84\x83\x83"
		"\x81\x81\x80\x81\x82\x83\x83\x84\x85\x86\x87\x87\x86\x85\x84\x83"
		"\x81\x81\x80\x80\x82\x82\x83\x85\x86\x87\x87\x86\x85\x85\x84\x83"
		"\x82\x80\x80\x81\x82\x83\x84\x85\x85"
		"\x00\x87\x83"
		"\x86\x84\x83\x83"
	)),
		"Encoding RLE data failed"
	);
}

BOOST_AUTO_TEST_CASE(encode_splitbuf_count2)
{
	BOOST_TEST_MESSAGE("RLE escaped data through 4k stream buffer");

	BOOST_REQUIRE_MESSAGE(BUFFER_SIZE <= 4096,
		"This test only works on buffers <= 4096 bytes");

#define dat_120e \
	"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02" \
	"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02" \
	"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02" \
	"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02" \
	"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02" \
	"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02" \
	"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02" \
	"\x01\x02\x01\x02\x01\x02\x01\x02"
#define rle_120e \
	"\xF7" dat_120e

	*this->in << STRING_WITH_NULLS(
		dat_3968e
		dat_120e
		"\xCB\xCB\xCB"
		"\xD6\xCE\xCE\xCF\xCE\xCE\xD2\xD6\xCC\xCC\x87\x86\x85\x84\x83\x83"
		"\x81\x81\x80\x81\x82\x83\x83\x84\x85\x86\x87\x87\x86\x85\x84\x83"
		"\x81\x81\x80\x80\x82\x82\x83\x85\x86\x87\x87\x86\x85\x85\x84\x83"
		"\x82\x80\x80\x81\x82\x83\x84\x85\x85\x87\x87\x87\x86\x84\x83\x83"
	);

	BOOST_CHECK_MESSAGE(is_equal(STRING_WITH_NULLS(
		rle_3968e
		rle_120e
		"\x00\xCB"
		"\xB8"
		"\xD6\xCE\xCE\xCF\xCE\xCE\xD2\xD6\xCC\xCC\x87\x86\x85\x84\x83\x83"
		"\x81\x81\x80\x81\x82\x83\x83\x84\x85\x86\x87\x87\x86\x85\x84\x83"
		"\x81\x81\x80\x80\x82\x82\x83\x85\x86\x87\x87\x86\x85\x85\x84\x83"
		"\x82\x80\x80\x81\x82\x83\x84\x85\x85"
		"\x00\x87\x83"
		"\x86\x84\x83\x83"
	)),
		"Encoding RLE data failed"
	);
}

BOOST_AUTO_TEST_CASE(encode_splitbuf_escape2)
{
	BOOST_TEST_MESSAGE("RLE escaped data through 4k stream buffer");

	BOOST_REQUIRE_MESSAGE(BUFFER_SIZE <= 4096,
		"This test only works on buffers <= 4096 bytes");

#define dat_94e \
	"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02" \
	"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02" \
	"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02" \
	"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02" \
	"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02" \
	"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
#define rle_94e \
	"\xDD" dat_94e

	*this->in << STRING_WITH_NULLS(
		dat_3968e
		dat_94e
		"\xCB\xCB\xCB"
		"\xD6\xCE\xCE\xCF\xCE\xCE\xD2\xD6\xCC\xCC\x87\x86\x85\x84\x83\x83"
		"\x81\x81\x80\x81\x82\x83\x83\x84\x85\x86\x87\x87\x86\x85\x84\x83"
		"\x81\x81\x80\x80\x82\x82\x83\x85\x86\x87\x87\x86\x85\x85\x84\x83"
		"\x82\x80\x80\x81\x82\x83\x84\x85\x85\x87\x87\x87\x86\x84\x83\x83"
	);

	BOOST_CHECK_MESSAGE(is_equal(STRING_WITH_NULLS(
		rle_3968e
		rle_94e
		"\x00\xCB"
		"\xB8"
		"\xD6\xCE\xCE\xCF\xCE\xCE\xD2\xD6\xCC\xCC\x87\x86\x85\x84\x83\x83"
		"\x81\x81\x80\x81\x82\x83\x83\x84\x85\x86\x87\x87\x86\x85\x84\x83"
		"\x81\x81\x80\x80\x82\x82\x83\x85\x86\x87\x87\x86\x85\x85\x84\x83"
		"\x82\x80\x80\x81\x82\x83\x84\x85\x85"
		"\x00\x87\x83"
		"\x86\x84\x83\x83"
	)),
		"Encoding RLE data failed"
	);
}

BOOST_AUTO_TEST_CASE(encode_splitbuf_escape3)
{
	BOOST_TEST_MESSAGE("RLE escaped data through 4k stream buffer");

	BOOST_REQUIRE_MESSAGE(BUFFER_SIZE <= 4096,
		"This test only works on buffers <= 4096 bytes");

#define dat_62e \
	"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02" \
	"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02" \
	"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02" \
	"\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02\x01\x02"
#define rle_62e \
	"\xBD" dat_62e

	*this->in << STRING_WITH_NULLS(
		dat_3968e
		dat_62e
		"\xCB\xCB\xCB"
		"\xD6\xCE\xCE\xCF\xCE\xCE\xD2\xD6\xCC\xCC\x87\x86\x85\x84\x83\x83"
		"\x81\x81\x80\x81\x82\x83\x83\x84\x85\x86\x87\x87\x86\x85\x84\x83"
		"\x81\x81\x80\x80\x82\x82\x83\x85\x86\x87\x87\x86\x85\x85\x84\x83"
		"\x82\x80\x80\x81\x82\x83\x84\x85\x85\x87\x87\x87\x86\x84\x83\x83"
	);

	BOOST_CHECK_MESSAGE(is_equal(STRING_WITH_NULLS(
		rle_3968e
		rle_62e
		"\x00\xCB"
		"\xB8"
		"\xD6\xCE\xCE\xCF\xCE\xCE\xD2\xD6\xCC\xCC\x87\x86\x85\x84\x83\x83"
		"\x81\x81\x80\x81\x82\x83\x83\x84\x85\x86\x87\x87\x86\x85\x84\x83"
		"\x81\x81\x80\x80\x82\x82\x83\x85\x86\x87\x87\x86\x85\x85\x84\x83"
		"\x82\x80\x80\x81\x82\x83\x84\x85\x85"
		"\x00\x87\x83"
		"\x86\x84\x83\x83"
	)),
		"Encoding RLE data failed"
	);
}

BOOST_AUTO_TEST_SUITE_END()

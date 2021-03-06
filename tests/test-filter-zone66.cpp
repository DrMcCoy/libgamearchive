/**
 * @file   test-filter-zone66.cpp
 * @brief  Test code for Zone 66 compression algorithm.
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
#include "../src/filter-zone66.hpp"

using namespace camoto;
using namespace camoto::gamearchive;

struct z66_compress_sample: public test_filter {
	z66_compress_sample()
	{
		this->filter.reset(new filter_z66_compress());
	}
};

struct z66_decompress_sample: public test_filter {
	z66_decompress_sample()
	{
		this->filter.reset(new filter_z66_decompress());
	}
};

#define DATA_ENCODED \
	"\x00\x03\x00\x00\x00\x00\x40\x00\x05\x40\x10\x20\x01\x51\x54\x0c" \
	"\xa8\x00\x54\x2a\x15\x83\x15\x41\xc5\x42\xa2\xa1\x53\xf8\x58\xac" \
	"\x2c\xfc\x7e\x2b\x0c\x3f\x1f\x9f\xc3\x4f\xc7\x67\x63\xb3\x41\xa1" \
	"\xa0\xb8\xb8\x5c\x50\x28\x28\x10\x90\x88\x46\xc3\x63\x61\x41\x40" \
	"\xa0\x70\x38\x38\x10\x10\x08\x0c\x06\x06\x84\x04\x02\x62\x61\x31" \
	"\x80\xb0\xb0\x78\x60\x30\x46\x19\x19\x14\x8d\x06\x8b\xc7\xe0\x00" \
	"\x03\xb8\x01\xc4\x00\xd6\x00\x65\x00\x2f\x80\x16\x40\x0a\x60\x04" \
	"\xd0\x02\x28\x00\xfc\x00\x72\x00\x33\x00\x16\x80\x09\xc0\x04\x20" \
	"\x07\xe3\x63\x61\xf9\x70\xb8\xfc\x4e\x4e\x3f\x1f\x0f\x9f\x85\xc5" \
	"\xc7\xe2\x01\x03\xf8\xf1\xa0\x98\x00\x72\x5c\x27\x3f\x13\x08\x0f" \
	"\xc8\x81\x07\xe1\xe0\x01\xc8\xd8\x00\xcc\x30\x00\x2d\x15\x00\x13" \
	"\x84\xc0\x22\x06\xd1\x02\xe8\x81\x3c\x40\x7c\x7e\x7c\x17\x3f\x1e" \
	"\x88\x0f\xcf\x69\x47\xa0\x03\x91\xb0\x00\xcc\xc4\x00\x5a\x2b\x00" \
	"\x9f\x00\x46\x00\x03\x83\x60\x01\x60\xa8\x04\x84\x02\xa6\x78\x3c" \
	"\x29\x1c\x9d\x09\x4d\x87\x04\x03\x33\x60\xe1\x80\xcc\x60\x5a\x62" \
	"\x15\x29\x17\x88\x89\x8b\x01\xc4\x62\xa0\xb1\x01\x40\x24\x70\x4c" \
	"\x0c\x19\x23\x02\x0b\x08\x40\x82\x83\xe0\x11\x10\xe8\x05\x40\xfc" \
	"\x00\x4c\x3d\x00\x12\x9e\x00\x09\x07\x60\x02\x33\x90\x01\x10\xe0" \
	"\x00\x42\x6c\x00\x21\x1a\x80\x08\x0d\x00\x03\xe3\x20\x00\xf1\x88" \
	"\x00\x74\x60\x00\x1c\x2e\x00\x0e\x0b\x40\x03\x65\x80\x01\xa8\x10" \
	"\xc8\xa4\x00\x30\x50\x00\x17\xa2\x8b\x89\x40\x02\xc4\x60\x01\x51" \
	"\x10\x00\x50\x84\x00\x26\x1f\x00\x09\x0f\x00\x04\x6f\x82\x21\xb0" \
	"\x00\x80\xd0\x00\x3c\x32\x00\x0e\x17\x00\x06\x85\x80\x01\x82\xa0" \
	"\x00\xba\x88\x58\x48\x00\x14\x22\x00\x09\x07\x80\x02\x03\x80\x00" \
	"\xe0\xc0\x00\x30\x58\x00\x18\x14\x00\x05\x08\x00\x02\x01\xc0\x00" \
	"\x60\xc0\x00\x20\x20\x00\x04\x0c\x00\x02\x02\x00\xdf\x00\x40\x03" \
	"\x84\x61\x80\xc2\x20\x50\x58\x84\x24\x14\x3e\x08\x09\x0f\x03\x82" \
	"\x07\x40\xc0\xe1\xb0\x50\x30\xd0\x10\x14\x32\x06\x04\x17\x01\x02" \
	"\x05\x80\x40\x62\xa0\x10\x20\xa0\x08\x08\x4b\x7c\x23\xbe\x10\x00" \
	"\x1e\x0d\x8f\xe8\xc7\xe3\x62\x71\xf9\xa5\x00\xca\xae\x5f\x43\x2d" \
	"\xa5\x15\x00\x0f\xe8\xa7\x22\x00\x01\x98\xed\x78\x62\xa0\x29\x87" \
	"\x11\xc8\x06\xd8\xc2\x80\x02\x03\xf3\x6a\x99\xf8\xfc\x9e\xaa\x7e" \
	"\x3f\x17\x88\x08\x44\x02\x22\x00\x03\xf3\x8a\xd9\x95\x34\xb6\xbc" \
	"\x4f\x50\x21\xc3\x8d\xe4\x05\xac\x62\x1e\xa3\xf9\x51\xf8\xdc\x00" \
	"\x7e\x5e\xf0\x93\xbc\x23\xef\x08\xbb\xc2\x1e\xf0\x83\x80\x07\x40" \
	"\x28\x00\x6c\x08\x00\x18\x80\x60\x01\x68\x10\x00\x50\xdf\x92\x00" \
	"\x20\x00\xf8\x08\x00\x36\x9b\x80\x00\x02\x48\x00\x6a\x00\x12\x80" \
	"\x06\x37\xc1\xe0\x00\x2a\xa2\x12\xe8\x87\x03\x20\x20\xa8\xe8\x04" \
	"\x38\x0d\x81\x84\x03\x20\x21\x18\x2e\x0a\x13\x89\x80\xe5\x40\x78" \
	"\x40\x5a\x16\x05\x18\xa0\x20\x11\x20\x04\x4a\x01\x13\x00\x20\x06" \
	"\x20\x50\x50\x62\x27\x11\x8e\xc4\x64\xf1\x19\x88\x14\x4e\x18\x85" \
	"\x03\xa6\x20\x50\x9c\x5e\x14\x18\x86\x44\x62\xef\x91\xf8\x72\x00" \
	"\xee\x00\x27\x80\x01\xf9\x4b\xc2\x46\xf0\x8f\x3c\x23\x0f\x08\x9b" \
	"\xc2\x00\xf5\x16\x84\x02\x61\x99\x32\x1c\x00\x88\x1f\x80"

#define DATA_DECODED \
	"\x00\x00\x00\x00\x00\x2a\x00\x2a\x00\x00\x2a\x2a\x2a\x00\x00\x2a" \
	"\x00\x2a\x2a\x15\x00\x2a\x2a\x2a\x15\x15\x15\x15\x15\x3f\x15\x3f" \
	"\x15\x15\x3f\x3f\x3f\x15\x15\x3f\x15\x3f\x3f\x3f\x15\x3f\x3f\x3f" \
	"\x3b\x3b\x3b\x34\x34\x34\x2e\x2e\x2e\x28\x28\x28\x21\x21\x21\x1b" \
	"\x1b\x1b\x14\x14\x14\x0e\x0e\x0e\x08\x08\x08\x0c\x0c\x0d\x10\x10" \
	"\x13\x13\x13\x18\x16\x16\x1e\x18\x18\x23\x19\x19\x29\x1a\x1a\x2f" \
	"\x3f\x00\x00\x3b\x00\x00\x38\x00\x00\x35\x00\x00\x32\x00\x00\x2f" \
	"\x00\x00\x2c\x00\x00\x29\x00\x00\x26\x00\x00\x22\x00\x00\x1f\x00" \
	"\x00\x1c\x00\x00\x19\x00\x00\x16\x00\x00\x13\x00\x00\x10\x00\x00" \
	"\x3f\x36\x36\x3f\x2e\x2e\x3f\x27\x27\x3f\x1f\x1f\x3f\x17\x17\x3f" \
	"\x10\x10\x3f\x08\x08\x34\x26\x00\x39\x2e\x27\x3f\x26\x10\x3f\x22" \
	"\x08\x3f\x1e\x00\x39\x1b\x00\x33\x18\x00\x2d\x15\x00\x27\x13\x00" \
	"\x3f\x3f\x36\x3f\x3f\x2e\x3f\x3f\x27\x3f\x3f\x1f\x3f\x3e\x17\x3f" \
	"\x3d\x10\x3f\x3d\x08\x3f\x3d\x00\x39\x36\x00\x33\x31\x00\x2d\x2b" \
	"\x00\x27\x27\x00\x21\x21\x00\x1c\x1b\x00\x16\x15\x00\x10\x10\x00" \
	"\x3f\x3f\x2e\x3c\x3c\x29\x39\x3a\x25\x36\x38\x20\x33\x36\x1c\x30" \
	"\x33\x18\x2d\x31\x15\x29\x2f\x11\x26\x2c\x0e\x23\x2a\x0b\x20\x28" \
	"\x09\x1c\x26\x06\x19\x23\x04\x16\x21\x02\x14\x1f\x01\x11\x1d\x00" \
	"\x00\x27\x3f\x00\x26\x3d\x00\x25\x3c\x00\x24\x3b\x00\x23\x39\x00" \
	"\x22\x38\x00\x21\x36\x00\x21\x35\x00\x20\x34\x00\x1f\x32\x00\x1e" \
	"\x31\x00\x1d\x30\x00\x1c\x2e\x00\x1c\x2d\x00\x1b\x2c\x00\x1a\x2a" \
	"\x00\x19\x29\x00\x18\x28\x00\x17\x26\x00\x17\x25\x00\x16\x23\x00" \
	"\x15\x22\x00\x14\x21\x00\x13\x1f\x00\x12\x1e\x00\x11\x1d\x00\x11" \
	"\x1b\x00\x10\x1a\x00\x0f\x19\x00\x0e\x17\x00\x0d\x16\x00\x0c\x15" \
	"\x00\x0b\x13\x00\x0b\x12\x00\x0a\x11\x00\x09\x0f\x00\x08\x0e\x00" \
	"\x07\x0c\x00\x06\x0b\x00\x06\x0a\x00\x05\x08\x00\x04\x07\x00\x03" \
	"\x06\x00\x02\x04\x00\x01\x03\x00\x01\x02\x00\x00\x01\x00\x00\x00" \
	"\x0e\x23\x0c\x0c\x22\x0a\x0b\x21\x09\x0a\x1f\x08\x09\x1e\x07\x08" \
	"\x1d\x06\x07\x1b\x05\x06\x1a\x04\x05\x19\x03\x04\x17\x02\x04\x16" \
	"\x01\x03\x15\x01\x02\x14\x01\x02\x12\x00\x01\x11\x00\x01\x10\x00" \
	"\x3c\x36\x3f\x39\x2e\x3f\x36\x27\x3f\x34\x1f\x3f\x32\x17\x3f\x2f" \
	"\x10\x3f\x2d\x08\x3f\x2a\x00\x3f\x26\x00\x39\x20\x00\x33\x1d\x00" \
	"\x2d\x18\x00\x27\x14\x00\x21\x11\x00\x1c\x0d\x00\x16\x0a\x00\x10" \
	"\x3f\x36\x3f\x3f\x2e\x3f\x3f\x27\x3f\x3f\x1f\x3f\x3f\x17\x3f\x3f" \
	"\x10\x3f\x3f\x08\x3f\x3f\x00\x3f\x38\x00\x39\x32\x00\x33\x2d\x00" \
	"\x2d\x27\x00\x27\x21\x00\x21\x1b\x00\x1c\x16\x00\x16\x10\x00\x10" \
	"\x3f\x3f\x00\x3f\x37\x00\x3f\x2f\x00\x3f\x27\x00\x3f\x1f\x00\x3f" \
	"\x17\x00\x3f\x0f\x00\x3f\x07\x00\x3a\x05\x00\x36\x04\x00\x31\x03" \
	"\x00\x2d\x02\x00\x28\x02\x00\x24\x01\x00\x1f\x01\x00\x1b\x00\x00" \
	"\x16\x00\x00\x12\x00\x00\x0d\x00\x00\x09\x00\x00\x0c\x00\x01\x0f" \
	"\x00\x05\x13\x00\x09\x16\x01\x0e\x19\x01\x15\x1d\x02\x1c\x1b\x03" \
	"\x20\x19\x04\x23\x17\x05\x27\x13\x07\x2a\x0f\x08\x2d\x0b\x0a\x31" \
	"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" \
	"\x00\x00\x00\x00\x00\x00\x00\x00\x31\x0a\x0a\x31\x13\x0a\x31\x1d" \
	"\x0a\x31\x27\x0a\x31\x31\x0a\x27\x31\x0a\x1d\x31\x0a\x13\x31\x0a" \
	"\x0a\x31\x0c\x0a\x31\x17\x3f\x3f\x00\x3f\x39\x00\x3f\x34\x00\x3f" \
	"\x2e\x00\x3f\x29\x00\x3f\x23\x00\x3f\x1e\x00\x3f\x18\x00\x3f\x13" \
	"\x00\x3f\x00\x00\x10\x2d\x20\x13\x33\x26\x3f\x15\x00\x3f\x3f\x3f"

#define DATA_DECODED_CAMOTO \
	"\x00\x00\x00\x00\x00\x2a\x00\x2a\x00\x00\x2a\x2a\x2a\x00\x00\x2a" \
	"\x00\x2a\x2a\x15\x00\x2a\x2a\x2a\x15\x15\x15\x15\x15\x3f\x15\x3f"

#define DATA_ENCODED_CAMOTO \
	"\x20\x00\x00\x00" \
	"\x00\x00\x00\x00\x00\x05\x40\x02\xa0\x00\x00\xa8\xa8\x54\x00\x00" \
	"\x2a\x00\x15\x0a\x85\x40\x05\x42\xa2\xa0\xa8\xa8\x54\x54\x2a\x7e" \
	"\x15\x3f"

BOOST_FIXTURE_TEST_SUITE(z66_decompress_suite, z66_decompress_sample)

BOOST_AUTO_TEST_CASE(decode)
{
	BOOST_TEST_MESSAGE("Decompress some Zone 66 format data");

	in << STRING_WITH_NULLS(DATA_ENCODED);

	BOOST_CHECK_MESSAGE(is_equal(STRING_WITH_NULLS(DATA_DECODED)),
		"Decompressing Zone 66 data failed");
}

BOOST_AUTO_TEST_CASE(decode_camoto)
{
	BOOST_TEST_MESSAGE("Decompress some Zone 66 format data compressed by Camoto");

	in << STRING_WITH_NULLS(DATA_ENCODED_CAMOTO);

	BOOST_CHECK_MESSAGE(is_equal(STRING_WITH_NULLS(DATA_DECODED_CAMOTO)),
		"Decompressing Zone 66 data failed");
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(z66_compress_suite, z66_compress_sample)

BOOST_AUTO_TEST_CASE(encode)
{
	BOOST_TEST_MESSAGE("Compress some data in Zone 66 format");

	in << STRING_WITH_NULLS(DATA_DECODED_CAMOTO);

	BOOST_CHECK_MESSAGE(is_equal(STRING_WITH_NULLS(DATA_ENCODED_CAMOTO)),
		"Compressing Zone 66 data failed");
}

BOOST_AUTO_TEST_CASE(encode_decode_20k)
{
	BOOST_TEST_MESSAGE("Compress >20k bytes in Zone 66 format");

	std::string src;
	for (unsigned int i = 0; i < 21000/3; i++) {
		in->write("ABC", 3);
		src += "ABC";
	}

	stream::string_sptr out(new stream::string());
	this->in_filt->open(this->in, this->filter);
	stream::copy(out, this->in_filt);
	this->in_filt.reset(new stream::input_filtered());

	this->in.reset(new stream::string());
	out->seekg(0, stream::start);
	stream::copy(in, out);

	this->filter.reset(new filter_z66_decompress());
	BOOST_CHECK_MESSAGE(is_equal(src),
		"Compressing >20k of Zone 66 data failed");
}

BOOST_AUTO_TEST_SUITE_END()

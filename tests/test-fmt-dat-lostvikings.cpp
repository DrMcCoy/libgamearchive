/**
 * @file   test-fmt-dat-lostvikings.cpp
 * @brief  Test code for The Lost Vikings .DAT archives.
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

#include "test-archive.hpp"

class test_dat_lostvikings: public test_archive
{
	public:
		test_dat_lostvikings()
		{
			this->type = "dat-lostvikings";
			this->lenMaxFilename = -1; // no filenames
		}

		void addTests()
		{
			this->test_archive::addTests();

			// c00: Initial state
			this->isInstance(ArchiveType::DefinitelyYes, this->initialstate());

			// c01: Empty archive
			this->isInstance(ArchiveType::PossiblyYes, STRING_WITH_NULLS(
				""
			));

			// c02: Archive too small
			this->isInstance(ArchiveType::DefinitelyNo, STRING_WITH_NULLS(
				"\x00\x00\x00"
			));

			// c03: First file is smaller than FAT
			this->isInstance(ArchiveType::DefinitelyNo, STRING_WITH_NULLS(
				"\x03\x00\x00\x00"
			));

			// c04: File past EOF
			this->isInstance(ArchiveType::DefinitelyNo, STRING_WITH_NULLS(
				"\x08\x00\x00\x00"
				"\xf7\x00\x00\x00"
				"This is one.dat"
				"This is two.dat"
			));

			// c05: File with negative size
			this->isInstance(ArchiveType::DefinitelyNo, STRING_WITH_NULLS(
				"\x08\x00\x00\x00"
				"\x07\x00\x00\x00"
				"This is one.dat"
				"This is two.dat"
			));

			// c06: Single empty file - valid except for Sango misdetection avoidance
			this->isInstance(ArchiveType::Unsure /*DefinitelyYes*/, STRING_WITH_NULLS(
				"\x04\x00\x00\x00"
			));
		}

		virtual std::string initialstate()
		{
			return STRING_WITH_NULLS(
				"\x08\x00\x00\x00"
				"\x17\x00\x00\x00"
				"This is one.dat"
				"This is two.dat"
			);
		}

		virtual std::string rename()
		{
			// No filenames to rename
			throw 1;
		}

		virtual std::string insert_end()
		{
			return STRING_WITH_NULLS(
				"\x0c\x00\x00\x00"
				"\x1b\x00\x00\x00"
				"\x2a\x00\x00\x00"
				"This is one.dat"
				"This is two.dat"
				"This is three.dat"
			);
		}

		virtual std::string insert_mid()
		{
			return STRING_WITH_NULLS(
				"\x0c\x00\x00\x00"
				"\x1b\x00\x00\x00"
				"\x2c\x00\x00\x00"
				"This is one.dat"
				"This is three.dat"
				"This is two.dat"
			);
		}

		virtual std::string insert2()
		{
			return STRING_WITH_NULLS(
				"\x10\x00\x00\x00"
				"\x1f\x00\x00\x00"
				"\x30\x00\x00\x00"
				"\x40\x00\x00\x00"
				"This is one.dat"
				"This is three.dat"
				"This is four.dat"
				"This is two.dat"
			);
		}

		virtual std::string remove()
		{
			return STRING_WITH_NULLS(
				"\x04\x00\x00\x00"
				"This is two.dat"
			);
		}

		virtual std::string remove2()
		{
			return STRING_WITH_NULLS(
				""
			);
		}

		virtual std::string insert_remove()
		{
			return STRING_WITH_NULLS(
				"\x08\x00\x00\x00"
				"\x19\x00\x00\x00"
				"This is three.dat"
				"This is two.dat"
			);
		}

		virtual std::string move()
		{
			return STRING_WITH_NULLS(
				"\x08\x00\x00\x00"
				"\x17\x00\x00\x00"
				"This is two.dat"
				"This is one.dat"
			);
		}

		virtual std::string resize_larger()
		{
			return STRING_WITH_NULLS(
				"\x08\x00\x00\x00"
				"\x1c\x00\x00\x00"
				"This is one.dat\0\0\0\0\0"
				"This is two.dat"
			);
		}

		virtual std::string resize_smaller()
		{
			return STRING_WITH_NULLS(
				"\x08\x00\x00\x00"
				"\x12\x00\x00\x00"
				"This is on"
				"This is two.dat"
			);
		}

		virtual std::string resize_write()
		{
			return STRING_WITH_NULLS(
				"\x08\x00\x00\x00"
				"\x1f\x00\x00\x00"
				"Now resized to 23 chars"
				"This is two.dat"
			);
		}
};

IMPLEMENT_TESTS(dat_lostvikings);

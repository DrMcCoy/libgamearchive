/**
 * @file   test-arch-dat-wacky.cpp
 * @brief  Test code for Wacky Wheels .DAT archives.
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

class test_dat_wacky: public test_archive
{
	public:
		test_dat_wacky()
		{
			this->type = "dat-wacky";
			this->lenMaxFilename = 12;
		}

		void addTests()
		{
			this->test_archive::addTests();

			// c00: Initial state
			this->isInstance(ArchiveType::DefinitelyYes, this->initialstate());

			// c01: Invalid char in filename
			this->isInstance(ArchiveType::DefinitelyNo, STRING_WITH_NULLS(
				"\x02\x00"
				"ONE.DAT\x05\0\0\0\0\0\0"      "\x0f\x00\x00\x00" "\x2c\x00\x00\x00"
				"TWO.DAT\0\0\0\0\0\0\0"        "\x0f\x00\x00\x00" "\x3b\x00\x00\x00"
				"This is one.dat"
				"This is two.dat"
			));

			// c02: File too short
			this->isInstance(ArchiveType::DefinitelyNo, STRING_WITH_NULLS(
				"\x01"
			));

			// c03: File past EOF
			this->isInstance(ArchiveType::DefinitelyNo, STRING_WITH_NULLS(
				"\x02\x00"
				"ONE.DAT\0\0\0\0\0\0\0"        "\x0f\x01\x00\x00" "\x2c\x00\x00\x00"
				"TWO.DAT\0\0\0\0\0\0\0"        "\x0f\x00\x00\x00" "\x3b\x00\x00\x00"
				"This is one.dat"
				"This is two.dat"
			));

			// c04: Content larger than file count suggests
			this->isInstance(ArchiveType::DefinitelyNo, STRING_WITH_NULLS(
				"\x00\x00"
				"ONE.DAT\0\0\0\0\0\0\0"        "\x0f\x00\x00\x00" "\x2c\x00\x00\x00"
				"TWO.DAT\0\0\0\0\0\0\0"        "\x0f\x00\x00\x00" "\x3b\x00\x00\x00"
				"This is one.dat"
				"This is two.dat"
			));
		}

		virtual std::string initialstate()
		{
			return STRING_WITH_NULLS(
				"\x02\x00"
				"ONE.DAT\0\0\0\0\0\0\0"        "\x0f\x00\x00\x00" "\x2c\x00\x00\x00"
				"TWO.DAT\0\0\0\0\0\0\0"        "\x0f\x00\x00\x00" "\x3b\x00\x00\x00"
				"This is one.dat"
				"This is two.dat"
			);
		}

		virtual std::string rename()
		{
			return STRING_WITH_NULLS(
				"\x02\x00"
				"THREE.DAT\0\0\0\0\0"          "\x0f\x00\x00\x00" "\x2c\x00\x00\x00"
				"TWO.DAT\0\0\0\0\0\0\0"        "\x0f\x00\x00\x00" "\x3b\x00\x00\x00"
				"This is one.dat"
				"This is two.dat"
			);
		}

		virtual std::string insert_end()
		{
			return STRING_WITH_NULLS(
				"\x03\x00"
				"ONE.DAT\0\0\0\0\0\0\0"        "\x0f\x00\x00\x00" "\x42\x00\x00\x00"
				"TWO.DAT\0\0\0\0\0\0\0"        "\x0f\x00\x00\x00" "\x51\x00\x00\x00"
				"THREE.DAT\0\0\0\0\0"          "\x11\x00\x00\x00" "\x60\x00\x00\x00"
				"This is one.dat"
				"This is two.dat"
				"This is three.dat"
			);
		}

		virtual std::string insert_mid()
		{
			return STRING_WITH_NULLS(
				"\x03\x00"
				"ONE.DAT\0\0\0\0\0\0\0"        "\x0f\x00\x00\x00" "\x42\x00\x00\x00"
				"THREE.DAT\0\0\0\0\0"          "\x11\x00\x00\x00" "\x51\x00\x00\x00"
				"TWO.DAT\0\0\0\0\0\0\0"        "\x0f\x00\x00\x00" "\x62\x00\x00\x00"
				"This is one.dat"
				"This is three.dat"
				"This is two.dat"
			);
		}

		virtual std::string insert2()
		{
			return STRING_WITH_NULLS(
				"\x04\x00"
				"ONE.DAT\0\0\0\0\0\0\0"        "\x0f\x00\x00\x00" "\x58\x00\x00\x00"
				"THREE.DAT\0\0\0\0\0"          "\x11\x00\x00\x00" "\x67\x00\x00\x00"
				"FOUR.DAT\0\0\0\0\0\0"         "\x10\x00\x00\x00" "\x78\x00\x00\x00"
				"TWO.DAT\0\0\0\0\0\0\0"        "\x0f\x00\x00\x00" "\x88\x00\x00\x00"
				"This is one.dat"
				"This is three.dat"
				"This is four.dat"
				"This is two.dat"
			);
		}

		virtual std::string remove()
		{
			return STRING_WITH_NULLS(
				"\x01\x00"
				"TWO.DAT\0\0\0\0\0\0\0"        "\x0f\x00\x00\x00" "\x16\x00\x00\x00"
				"This is two.dat"
			);
		}

		virtual std::string remove2()
		{
			return STRING_WITH_NULLS(
				"\x00\x00"
			);
		}

		virtual std::string insert_remove()
		{
			return STRING_WITH_NULLS(
				"\x02\x00"
				"THREE.DAT\0\0\0\0\0"          "\x11\x00\x00\x00" "\x2c\x00\x00\x00"
				"TWO.DAT\0\0\0\0\0\0\0"        "\x0f\x00\x00\x00" "\x3d\x00\x00\x00"
				"This is three.dat"
				"This is two.dat"
			);
		}

		virtual std::string move()
		{
			return STRING_WITH_NULLS(
				"\x02\x00"
				"TWO.DAT\0\0\0\0\0\0\0"        "\x0f\x00\x00\x00" "\x2c\x00\x00\x00"
				"ONE.DAT\0\0\0\0\0\0\0"        "\x0f\x00\x00\x00" "\x3b\x00\x00\x00"
				"This is two.dat"
				"This is one.dat"
			);
		}

		virtual std::string resize_larger()
		{
			return STRING_WITH_NULLS(
				"\x02\x00"
				"ONE.DAT\0\0\0\0\0\0\0"        "\x14\x00\x00\x00" "\x2c\x00\x00\x00"
				"TWO.DAT\0\0\0\0\0\0\0"        "\x0f\x00\x00\x00" "\x40\x00\x00\x00"
				"This is one.dat\0\0\0\0\0"
				"This is two.dat"
			);
		}

		virtual std::string resize_smaller()
		{
			return STRING_WITH_NULLS(
				"\x02\x00"
				"ONE.DAT\0\0\0\0\0\0\0"        "\x0a\x00\x00\x00" "\x2c\x00\x00\x00"
				"TWO.DAT\0\0\0\0\0\0\0"        "\x0f\x00\x00\x00" "\x36\x00\x00\x00"
				"This is on"
				"This is two.dat"
			);
		}

		virtual std::string resize_write()
		{
			return STRING_WITH_NULLS(
				"\x02\x00"
				"ONE.DAT\0\0\0\0\0\0\0"        "\x17\x00\x00\x00" "\x2c\x00\x00\x00"
				"TWO.DAT\0\0\0\0\0\0\0"        "\x0f\x00\x00\x00" "\x43\x00\x00\x00"
				"Now resized to 23 chars"
				"This is two.dat"
			);
		}
};

IMPLEMENT_TESTS(dat_wacky);

/*
 * test-fmt-vol-cosmo.cpp - test code for VOLArchive class.
 *
 * Copyright (C) 2010 Adam Nielsen <malvineous@shikadi.net>
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

#define FILENAME1 "ONE.DAT"
#define FILENAME2 "TWO.DAT"
#define FILENAME3 "THREE.DAT"
#define FILENAME4 "FOUR.DAT"

#define LOTS_OF_NULLS \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	\
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	\
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	\
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	\
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	\
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	\
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	\
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	\
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	\
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	\
	/* 100 */ \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	\
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	\
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	\
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	\
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	\
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	\
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	\
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	\
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	\
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	/* ^^ 196 ^^ */

#define testdata_initialstate \
	"ONE.DAT\0\0\0\0\0"        "\xa0\x0f\x00\x00" "\x0f\x00\x00\x00" \
	"TWO.DAT\0\0\0\0\0"        "\xaf\x0f\x00\x00" "\x0f\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	LOTS_OF_NULLS \
	"This is one.dat" \
	"This is two.dat"

#define testdata_rename \
	"THREE.DAT\0\0\0"          "\xa0\x0f\x00\x00" "\x0f\x00\x00\x00" \
	"TWO.DAT\0\0\0\0\0"        "\xaf\x0f\x00\x00" "\x0f\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	LOTS_OF_NULLS \
	"This is one.dat" \
	"This is two.dat"

#define testdata_insert_end \
	"ONE.DAT\0\0\0\0\0"        "\xa0\x0f\x00\x00" "\x0f\x00\x00\x00" \
	"TWO.DAT\0\0\0\0\0"        "\xaf\x0f\x00\x00" "\x0f\x00\x00\x00" \
	"THREE.DAT\0\0\0"          "\xbe\x0f\x00\x00" "\x11\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	LOTS_OF_NULLS \
	"This is one.dat" \
	"This is two.dat" \
	"This is three.dat"

#define testdata_insert_mid \
	"ONE.DAT\0\0\0\0\0"        "\xa0\x0f\x00\x00" "\x0f\x00\x00\x00" \
	"THREE.DAT\0\0\0"          "\xaf\x0f\x00\x00" "\x11\x00\x00\x00" \
	"TWO.DAT\0\0\0\0\0"        "\xc0\x0f\x00\x00" "\x0f\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	LOTS_OF_NULLS \
	"This is one.dat" \
	"This is three.dat" \
	"This is two.dat"

#define testdata_insert2 \
	"ONE.DAT\0\0\0\0\0"        "\xa0\x0f\x00\x00" "\x0f\x00\x00\x00" \
	"THREE.DAT\0\0\0"          "\xaf\x0f\x00\x00" "\x11\x00\x00\x00" \
	"FOUR.DAT\0\0\0\0"         "\xc0\x0f\x00\x00" "\x10\x00\x00\x00" \
	"TWO.DAT\0\0\0\0\0"        "\xd0\x0f\x00\x00" "\x0f\x00\x00\x00" \
	LOTS_OF_NULLS \
	"This is one.dat" \
	"This is three.dat" \
	"This is four.dat" \
	"This is two.dat"

#define testdata_remove \
	"TWO.DAT\0\0\0\0\0"        "\xa0\x0f\x00\x00" "\x0f\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	LOTS_OF_NULLS \
	"This is two.dat"

#define testdata_remove2 \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	LOTS_OF_NULLS \

#define testdata_insert_remove \
	"THREE.DAT\0\0\0"          "\xa0\x0f\x00\x00" "\x11\x00\x00\x00" \
	"TWO.DAT\0\0\0\0\0"        "\xb1\x0f\x00\x00" "\x0f\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	LOTS_OF_NULLS \
	"This is three.dat" \
	"This is two.dat"

#define testdata_move \
	"TWO.DAT\0\0\0\0\0"        "\xa0\x0f\x00\x00" "\x0f\x00\x00\x00" \
	"ONE.DAT\0\0\0\0\0"        "\xaf\x0f\x00\x00" "\x0f\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	LOTS_OF_NULLS \
	"This is two.dat" \
	"This is one.dat"

#define testdata_resize_larger \
	"ONE.DAT\0\0\0\0\0"        "\xa0\x0f\x00\x00" "\x14\x00\x00\x00" \
	"TWO.DAT\0\0\0\0\0"        "\xb4\x0f\x00\x00" "\x0f\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	LOTS_OF_NULLS \
	"This is one.dat\0\0\0\0\0" \
	"This is two.dat"

#define testdata_resize_smaller \
	"ONE.DAT\0\0\0\0\0"        "\xa0\x0f\x00\x00" "\x0a\x00\x00\x00" \
	"TWO.DAT\0\0\0\0\0"        "\xaa\x0f\x00\x00" "\x0f\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	LOTS_OF_NULLS \
	"This is on" \
	"This is two.dat"

#define testdata_resize_write \
	"ONE.DAT\0\0\0\0\0"        "\xa0\x0f\x00\x00" "\x17\x00\x00\x00" \
	"TWO.DAT\0\0\0\0\0"        "\xb7\x0f\x00\x00" "\x0f\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	"\0\0\0\0\0\0\0\0\0\0\0\0" "\x00\x00\x00\x00" "\x00\x00\x00\x00" \
	LOTS_OF_NULLS \
	"Now resized to 23 chars" \
	"This is two.dat"

#define MAX_FILENAME_LEN  12

#define ARCHIVE_CLASS fmt_vol_cosmo
#define ARCHIVE_TYPE  "vol-cosmo"
#include "test-archive.hpp"

// Test some invalid formats to make sure they're not identified as valid
// archives.  Note that they can still be opened though (by 'force'), this
// only checks whether they look like valid files or not.

// The "c00" test has already been performed in test-archive.hpp to ensure the
// initial state is correctly identified as a valid archive.

ISINSTANCE_TEST(c01,
	"ONE.DAT\x05\0\0\0\0"      "\x14\x00\x00\x00" "\x0f\x00\x00\x00"
	"This is one.dat",
	ga::EC_DEFINITELY_NO
);

ISINSTANCE_TEST(c02,
	"ONE.DAT\0\0\0\0\0"        "\x05\x00\x00\x00" "\x0f\x00\x00\x00"
	"This is one.dat",
	ga::EC_DEFINITELY_NO
);

ISINSTANCE_TEST(c03,
	"ONE.DAT\0\0\0\0\0"        "\x14\x00\x00\x00" "\x1f\x00\x00\x00"
	"This is one.dat",
	ga::EC_DEFINITELY_NO
);

// Not really possible to do any INVALIDDATA_TEST() tests here, because the
// worst that can happen is it looks like the archive has been truncated.

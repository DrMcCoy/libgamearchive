/**
 * @file  fmt-dat-wacky.cpp
 * @brief Implementation of Wacky Wheels .DAT file reader/writer.
 *
 * This file format is fully documented on the ModdingWiki:
 *   http://www.shikadi.net/moddingwiki/DAT_Format_%28Wacky_Wheels%29
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

#include <boost/algorithm/string.hpp>
#include <camoto/iostream_helpers.hpp>
#include <camoto/util.hpp>

#include "fmt-dat-wacky.hpp"

#define DAT_FILECOUNT_OFFSET     0
#define DAT_MAX_FILENAME_LEN     12
#define DAT_FILENAME_FIELD_LEN   14
#define DAT_FAT_ENTRY_LEN        22  // filename + u32le offset + u32le size
#define DAT_FAT_OFFSET           2
#define DAT_FIRST_FILE_OFFSET    DAT_FAT_OFFSET

#define DAT_FATENTRY_OFFSET(e)   (DAT_FAT_OFFSET + e->iIndex * DAT_FAT_ENTRY_LEN)

#define DAT_FILENAME_OFFSET(e)    DAT_FATENTRY_OFFSET(e)
#define DAT_FILESIZE_OFFSET(e)   (DAT_FATENTRY_OFFSET(e) + DAT_FILENAME_FIELD_LEN)
#define DAT_FILEOFFSET_OFFSET(e) (DAT_FATENTRY_OFFSET(e) + DAT_FILENAME_FIELD_LEN + 4)

namespace camoto {
namespace gamearchive {

ArchiveType_DAT_Wacky::ArchiveType_DAT_Wacky()
{
}

ArchiveType_DAT_Wacky::~ArchiveType_DAT_Wacky()
{
}

std::string ArchiveType_DAT_Wacky::getArchiveCode() const
{
	return "dat-wacky";
}

std::string ArchiveType_DAT_Wacky::getFriendlyName() const
{
	return "Wacky Wheels DAT File";
}

std::vector<std::string> ArchiveType_DAT_Wacky::getFileExtensions() const
{
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("dat");
	return vcExtensions;
}

std::vector<std::string> ArchiveType_DAT_Wacky::getGameList() const
{
	std::vector<std::string> vcGames;
	vcGames.push_back("Wacky Wheels");
	return vcGames;
}

ArchiveType::Certainty ArchiveType_DAT_Wacky::isInstance(stream::input_sptr psArchive) const
{
	stream::pos lenArchive = psArchive->size();
	// TESTED BY: fmt_dat_wacky_isinstance_c02
	if (lenArchive < DAT_FAT_OFFSET) return DefinitelyNo; // too short

	psArchive->seekg(0, stream::start);
	uint16_t numFiles;
	psArchive >> u16le(numFiles);

	// If the archive has no files, it'd better be tiny
	// TESTED BY: fmt_dat_wacky_isinstance_c04
	if ((numFiles == 0) && (lenArchive > DAT_FAT_OFFSET)) return DefinitelyNo;

	uint32_t lenFAT = numFiles * DAT_FAT_ENTRY_LEN;

	// If the FAT is larger than the entire archive then it's not a DAT file
	if (lenFAT + DAT_FAT_OFFSET > lenArchive) return DefinitelyNo;

	// Check each FAT entry
	char fn[DAT_FILENAME_FIELD_LEN];
	for (int i = 0; i < numFiles; i++) {
		psArchive->read(fn, DAT_FILENAME_FIELD_LEN);
		// Make sure there aren't any invalid characters in the filename
		for (int j = 0; j < DAT_FILENAME_FIELD_LEN; j++) {
			if (!fn[j]) break; // stop on terminating null

			// Fail on control characters in the filename
			if (fn[j] < 32) return DefinitelyNo; // TESTED BY: fmt_dat_wacky_isinstance_c01
		}

		uint32_t offEntry, lenEntry;
		psArchive >> u32le(lenEntry) >> u32le(offEntry);
		offEntry += DAT_FAT_OFFSET;

		// If a file entry points past the end of the archive then it's an invalid
		// format.
		// TESTED BY: fmt_dat_wacky_isinstance_c03
		if (offEntry + lenEntry > lenArchive) return DefinitelyNo;
	}

	// If we've made it this far, this is almost certainly a DAT file.

	// TESTED BY: fmt_dat_wacky_isinstance_c00
	return DefinitelyYes;
}

ArchivePtr ArchiveType_DAT_Wacky::newArchive(stream::inout_sptr psArchive, SuppData& suppData) const
{
	psArchive->seekp(0, stream::start);
	psArchive << u16le(0); // file count
	return ArchivePtr(new Archive_DAT_Wacky(psArchive));
}

ArchivePtr ArchiveType_DAT_Wacky::open(stream::inout_sptr psArchive, SuppData& suppData) const
{
	return ArchivePtr(new Archive_DAT_Wacky(psArchive));
}

SuppFilenames ArchiveType_DAT_Wacky::getRequiredSupps(stream::input_sptr data,
	const std::string& filenameArchive) const
{
	// No supplemental types/empty list
	return SuppFilenames();
}


Archive_DAT_Wacky::Archive_DAT_Wacky(stream::inout_sptr psArchive)
	:	FATArchive(psArchive, DAT_FIRST_FILE_OFFSET, DAT_MAX_FILENAME_LEN)
{
	stream::pos lenArchive = this->psArchive->size();
	if (lenArchive < DAT_FAT_OFFSET) {
		throw stream::error("Archive too short - missing file count!");
	}

	this->psArchive->seekg(0, stream::start);

	uint16_t numFiles;
	this->psArchive >> u16le(numFiles);
	this->vcFAT.reserve(numFiles);

	for (int i = 0; i < numFiles; i++) {
		FATEntry *fatEntry = new FATEntry();
		EntryPtr ep(fatEntry);

		fatEntry->iIndex = i;
		fatEntry->lenHeader = 0;
		fatEntry->type = FILETYPE_GENERIC;
		fatEntry->fAttr = 0;
		fatEntry->bValid = true;

		// Read the data in from the FAT entry in the file
		this->psArchive
			>> nullPadded(fatEntry->strName, DAT_FILENAME_FIELD_LEN)
			>> u32le(fatEntry->storedSize)
			>> u32le(fatEntry->iOffset);

		// Offset doesn't include the two byte file count
		fatEntry->iOffset += DAT_FAT_OFFSET;

		fatEntry->realSize = fatEntry->storedSize;
		this->vcFAT.push_back(ep);
	}

}

Archive_DAT_Wacky::~Archive_DAT_Wacky()
{
}

void Archive_DAT_Wacky::updateFileName(const FATEntry *pid, const std::string& strNewName)
{
	// TESTED BY: fmt_dat_wacky_rename
	assert(strNewName.length() <= DAT_MAX_FILENAME_LEN);
	this->psArchive->seekp(DAT_FILENAME_OFFSET(pid), stream::start);
	this->psArchive << nullPadded(strNewName, DAT_FILENAME_FIELD_LEN);
	return;
}

void Archive_DAT_Wacky::updateFileOffset(const FATEntry *pid,
	stream::delta offDelta
)
{
	// TESTED BY: fmt_dat_wacky_insert*
	// TESTED BY: fmt_dat_wacky_resize*

	// Offsets don't start from the beginning of the archive
	uint32_t deltaOffset = pid->iOffset - DAT_FAT_OFFSET;

	this->psArchive->seekp(DAT_FILEOFFSET_OFFSET(pid), stream::start);
	this->psArchive << u32le(deltaOffset);
	return;
}

void Archive_DAT_Wacky::updateFileSize(const FATEntry *pid,
	stream::delta sizeDelta
)
{
	// TESTED BY: fmt_dat_wacky_insert*
	// TESTED BY: fmt_dat_wacky_resize*
	this->psArchive->seekp(DAT_FILESIZE_OFFSET(pid), stream::start);
	this->psArchive << u32le(pid->storedSize);
	return;
}

FATArchive::FATEntry *Archive_DAT_Wacky::preInsertFile(
	const FATEntry *idBeforeThis, FATEntry *pNewEntry
)
{
	// TESTED BY: fmt_dat_wacky_insert*
	assert(pNewEntry->strName.length() <= DAT_MAX_FILENAME_LEN);

	// Set the format-specific variables
	pNewEntry->lenHeader = 0;

	// Because the new entry isn't in the vector yet we need to shift it manually
	pNewEntry->iOffset += DAT_FAT_ENTRY_LEN;

	this->psArchive->seekp(DAT_FATENTRY_OFFSET(pNewEntry), stream::start);
	this->psArchive->insert(DAT_FAT_ENTRY_LEN);
	boost::to_upper(pNewEntry->strName);

	// Offsets don't start from the beginning of the archive
	uint32_t deltaOffset = pNewEntry->iOffset - DAT_FAT_OFFSET;

	// Write out the entry
	this->psArchive
		<< nullPadded(pNewEntry->strName, DAT_FILENAME_FIELD_LEN)
		<< u32le(pNewEntry->storedSize)
		<< u32le(deltaOffset);

	// Update the offsets now there's a new FAT entry taking up space.
	this->shiftFiles(
		NULL,
		DAT_FAT_OFFSET + this->vcFAT.size() * DAT_FAT_ENTRY_LEN,
		DAT_FAT_ENTRY_LEN,
		0
	);

	this->updateFileCount(this->vcFAT.size() + 1);
	return pNewEntry;
}

void Archive_DAT_Wacky::preRemoveFile(const FATEntry *pid)
{
	// TESTED BY: fmt_dat_wacky_remove*

	// Update the offsets now there's one less FAT entry taking up space.  This
	// must be called before the FAT is altered, because it will write a new
	// offset into the FAT entry we're about to erase (and if we erase it first
	// it'll overwrite something else.)
	this->shiftFiles(
		NULL,
		DAT_FAT_OFFSET + this->vcFAT.size() * DAT_FAT_ENTRY_LEN,
		-DAT_FAT_ENTRY_LEN,
		0
	);

	this->psArchive->seekp(DAT_FATENTRY_OFFSET(pid), stream::start);
	this->psArchive->remove(DAT_FAT_ENTRY_LEN);

	this->updateFileCount(this->vcFAT.size() - 1);
	return;
}

void Archive_DAT_Wacky::updateFileCount(uint32_t iNewCount)
{
	// TESTED BY: fmt_dat_wacky_insert*
	// TESTED BY: fmt_dat_wacky_remove*
	this->psArchive->seekp(DAT_FILECOUNT_OFFSET, stream::start);
	this->psArchive << u16le(iNewCount);
	return;
}

} // namespace gamearchive
} // namespace camoto

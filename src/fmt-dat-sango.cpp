/**
 * @file  fmt-dat-sango.cpp
 * @brief Implementation of Sango Fighter archive reader/writer.
 *
 * This file format is fully documented on the ModdingWiki:
 *   http://www.shikadi.net/moddingwiki/DAT_Format_%28Sango_Fighter%29
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

#include <camoto/iostream_helpers.hpp>

#include "fmt-dat-sango.hpp"

#define DAT_FAT_ENTRY_LEN       4  // u32le offset
#define DAT_FIRST_FILE_OFFSET   4  // empty archive only

#define DAT_SAFETY_MAX_FILECOUNT  8192 // Maximum value we will load

#define DAT_FATENTRY_OFFSET(e) (e->iIndex * DAT_FAT_ENTRY_LEN)

namespace camoto {
namespace gamearchive {

ArchiveType_DAT_Sango::ArchiveType_DAT_Sango()
{
}

ArchiveType_DAT_Sango::~ArchiveType_DAT_Sango()
{
}

std::string ArchiveType_DAT_Sango::getArchiveCode() const
{
	return "dat-sango";
}

std::string ArchiveType_DAT_Sango::getFriendlyName() const
{
	return "Sango Archive File";
}

std::vector<std::string> ArchiveType_DAT_Sango::getFileExtensions() const
{
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("dat");
	vcExtensions.push_back("mid");
	vcExtensions.push_back("pbn");
	vcExtensions.push_back("pcm");
	vcExtensions.push_back("pcp");
	return vcExtensions;
}

std::vector<std::string> ArchiveType_DAT_Sango::getGameList() const
{
	std::vector<std::string> vcGames;
	vcGames.push_back("Sango Fighter");
	return vcGames;
}

ArchiveType::Certainty ArchiveType_DAT_Sango::isInstance(stream::input_sptr psArchive) const
{
	stream::pos lenArchive = psArchive->size();

	// TESTED BY: fmt_dat_sango_isinstance_c01
	if (lenArchive < DAT_FAT_ENTRY_LEN) return DefinitelyNo; // too short

	uint32_t offEndFAT;
	psArchive->seekg(0, stream::start);
	psArchive >> u32le(offEndFAT);
	// TESTED BY: fmt_dat_sango_isinstance_c02
	if (offEndFAT > lenArchive) return DefinitelyNo;

	uint32_t offNext = 4; // in case of no files
	for (unsigned int offset = 4; offset < offEndFAT; offset += 4) {
		psArchive >> u32le(offNext);
		// TESTED BY: fmt_dat_sango_isinstance_c03
		if (offNext > lenArchive) return DefinitelyNo;
	}

	// Last offset must equal file size
	// TESTED BY: fmt_dat_sango_isinstance_c04
	if (offNext != lenArchive) return DefinitelyNo;

	// TESTED BY: fmt_dat_sango_isinstance_c00
	return DefinitelyYes;
}

ArchivePtr ArchiveType_DAT_Sango::newArchive(stream::inout_sptr psArchive, SuppData& suppData) const
{
	psArchive->seekp(0, stream::start);
	psArchive->write("\x04\0\0\0", 4);
	return ArchivePtr(new Archive_DAT_Sango(psArchive));
}

ArchivePtr ArchiveType_DAT_Sango::open(stream::inout_sptr psArchive, SuppData& suppData) const
{
	return ArchivePtr(new Archive_DAT_Sango(psArchive));
}

SuppFilenames ArchiveType_DAT_Sango::getRequiredSupps(stream::input_sptr data,
	const std::string& filenameArchive) const
{
	// No supplemental types/empty list
	return SuppFilenames();
}


Archive_DAT_Sango::Archive_DAT_Sango(stream::inout_sptr psArchive)
	:	FATArchive(psArchive, DAT_FIRST_FILE_OFFSET, 0)
{
	psArchive->seekg(0, stream::end);
	this->lenArchive = psArchive->tellg();

	if (this->lenArchive < DAT_FAT_ENTRY_LEN) throw stream::error("file too short");

	uint32_t offEndFAT;
	psArchive->seekg(0, stream::start);
	psArchive >> u32le(offEndFAT);

	uint32_t offCur = offEndFAT, offNext;
	for (int i = 0; offCur < this->lenArchive; i++) {
		psArchive >> u32le(offNext);

		FATEntry *fatEntry = new FATEntry();
		EntryPtr ep(fatEntry);

		fatEntry->iIndex = i;
		fatEntry->iOffset = offCur;
		fatEntry->lenHeader = 0;
		fatEntry->type = FILETYPE_GENERIC;
		fatEntry->fAttr = 0;
		fatEntry->bValid = true;
		fatEntry->storedSize = offNext - offCur;
		fatEntry->realSize = fatEntry->storedSize;
		this->vcFAT.push_back(ep);
		if (i >= DAT_SAFETY_MAX_FILECOUNT) {
			throw stream::error("too many files or corrupted archive");
		}

		offCur = offNext;
	}
}

Archive_DAT_Sango::~Archive_DAT_Sango()
{
}

void Archive_DAT_Sango::updateFileName(const FATEntry *pid, const std::string& strNewName)
{
	throw stream::error("This archive format does not support filenames.");
}

void Archive_DAT_Sango::updateFileOffset(const FATEntry *pid, stream::delta offDelta)
{
	this->psArchive->seekp(DAT_FATENTRY_OFFSET(pid), stream::start);
	this->psArchive << u32le(pid->iOffset);
	return;
}

void Archive_DAT_Sango::updateFileSize(const FATEntry *pid, stream::delta sizeDelta)
{
	// Update the last FAT entry (the one that points to EOF.)
	this->updateLastEntry(sizeDelta);
	return;
}

FATArchive::FATEntry *Archive_DAT_Sango::preInsertFile(const FATEntry *idBeforeThis, FATEntry *pNewEntry)
{
	// TESTED BY: fmt_dat_sango_insert*

	// Set the format-specific variables
	pNewEntry->lenHeader = 0;

	// Because the new entry isn't in the vector yet we need to shift it manually
	pNewEntry->iOffset += DAT_FAT_ENTRY_LEN;

	// Update the last FAT entry (the one that points to EOF.)
	this->updateLastEntry(pNewEntry->storedSize + DAT_FAT_ENTRY_LEN);

	this->psArchive->seekp(DAT_FATENTRY_OFFSET(pNewEntry), stream::start);
	this->psArchive->insert(DAT_FAT_ENTRY_LEN);

	this->psArchive
		<< u32le(pNewEntry->iOffset);

	// Update the offsets now there's a new FAT entry taking up space.
	this->shiftFiles(
		NULL,
		(this->vcFAT.size() + 1) * DAT_FAT_ENTRY_LEN,
		DAT_FAT_ENTRY_LEN,
		0
	);

	return pNewEntry;
}

void Archive_DAT_Sango::preRemoveFile(const FATEntry *pid)
{
	// TESTED BY: fmt_dat_sango_remove*

	// Update the offsets now there's one less FAT entry taking up space.  This
	// must be called before the FAT is altered, because it will write a new
	// offset into the FAT entry we're about to erase (and if we erase it first
	// it'll overwrite something else.)
	this->shiftFiles(
		NULL,
		(this->vcFAT.size() + 1) * DAT_FAT_ENTRY_LEN,
		-DAT_FAT_ENTRY_LEN,
		0
	);

	// Update the last FAT entry (the one that points to EOF.)
	this->updateLastEntry(-((stream::delta)pid->storedSize + DAT_FAT_ENTRY_LEN));

	this->psArchive->seekp(DAT_FATENTRY_OFFSET(pid), stream::start);
	this->psArchive->remove(DAT_FAT_ENTRY_LEN);

	return;
}

void Archive_DAT_Sango::updateLastEntry(stream::delta lenDelta)
{
	this->lenArchive += lenDelta;
	this->psArchive->seekp(this->vcFAT.size() * DAT_FAT_ENTRY_LEN, stream::start);
	this->psArchive
		<< u32le(this->lenArchive);
	return;
}

} // namespace gamearchive
} // namespace camoto

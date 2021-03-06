/**
 * @file  fmt-dat-got.cpp
 * @brief Implementation of God of Thunder .DAT file reader/writer.
 *
 * This file format is fully documented on the ModdingWiki:
 *   http://www.shikadi.net/moddingwiki/DAT_Format_(God_of_Thunder)
 *
 * Copyright (C) 2011 Adam Nielsen <malvineous@shikadi.net>
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
#include <boost/bind.hpp>
#include <camoto/iostream_helpers.hpp>
#include <camoto/util.hpp>

#include "fmt-dat-got.hpp"

#define GOT_MAX_FILES         256
#define GOT_MAX_FILENAME_LEN    8
#define GOT_FILENAME_FIELD_LEN  9
#define GOT_FAT_ENTRY_LEN      23
#define GOT_FAT_LENGTH        (GOT_MAX_FILES * GOT_FAT_ENTRY_LEN)
#define GOT_FIRST_FILE_OFFSET GOT_FAT_LENGTH

#define GOT_FATENTRY_OFFSET(e)   (e->iIndex * GOT_FAT_ENTRY_LEN)

#define GOT_FILENAME_OFFSET(e)    GOT_FATENTRY_OFFSET(e)
#define GOT_FILEOFFSET_OFFSET(e) (GOT_FATENTRY_OFFSET(e) + GOT_FILENAME_FIELD_LEN)
#define GOT_FILESIZE_OFFSET(e)   (GOT_FILEOFFSET_OFFSET(e) + 4)

// Comment the next line out and also in the test file to run the tests
// with no encryption to assist in debugging.
#define USE_XOR

namespace camoto {
namespace gamearchive {

ArchiveType_DAT_GoT::ArchiveType_DAT_GoT()
{
}

ArchiveType_DAT_GoT::~ArchiveType_DAT_GoT()
{
}

std::string ArchiveType_DAT_GoT::getArchiveCode() const
{
	return "dat-got";
}

std::string ArchiveType_DAT_GoT::getFriendlyName() const
{
	return "God of Thunder Resource File";
}

std::vector<std::string> ArchiveType_DAT_GoT::getFileExtensions() const
{
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("dat");
	return vcExtensions;
}

std::vector<std::string> ArchiveType_DAT_GoT::getGameList() const
{
	std::vector<std::string> vcGames;
	vcGames.push_back("God of Thunder");
	return vcGames;
}

ArchiveType::Certainty ArchiveType_DAT_GoT::isInstance(stream::input_sptr psArchive) const
{
	stream::pos lenArchive = psArchive->size();

	// Make sure the archive is large enough to hold a FAT
	// TESTED BY: fmt_dat_got_isinstance_c02
	if (lenArchive < GOT_FAT_LENGTH) return DefinitelyNo;

	// Create a substream to decrypt the FAT
	stream::input_sub_sptr fatSubStream(new stream::sub());
	fatSubStream->open(
		psArchive,
		0,
		GOT_MAX_FILES * GOT_FAT_ENTRY_LEN
	);

	filter_sptr fatCrypt(new filter_xor_crypt(0, 128));
#ifdef USE_XOR
	stream::input_filtered_sptr fatStream(new stream::input_filtered());
	fatStream->open(fatSubStream, fatCrypt);
#else
	stream::input_sptr fatStream = fatSubStream;
#endif

	fatStream->seekg(0, stream::start);

	try {
		// Check each FAT entry
		char fn[GOT_FILENAME_FIELD_LEN];
		for (int i = 0; i < GOT_MAX_FILES; i++) {
			fatStream->read(fn, GOT_FILENAME_FIELD_LEN);
			// Make sure there aren't any invalid characters in the filename
			for (int j = 0; j < GOT_MAX_FILENAME_LEN; j++) {
				if (!fn[j]) break; // stop on terminating null

				// Fail on control characters in the filename
				if (fn[j] < 32) return DefinitelyNo; // TESTED BY: fmt_dat_got_isinstance_c01
			}

			uint32_t offEntry, lenEntry, lenDecomp;
			uint16_t flags;
			fatStream
				>> u32le(offEntry)
				>> u32le(lenEntry)
				>> u32le(lenDecomp)
				>> u16le(flags)
			;

			// If a file entry points past the end of the archive then it's an invalid
			// format.
			// TESTED BY: fmt_dat_got_isinstance_c03
			// TESTED BY: fmt_dat_got_isinstance_c04
			if (offEntry + lenEntry > lenArchive) return DefinitelyNo;
		}
	} catch (const stream::incomplete_read&) {
		return DefinitelyNo;
	}

	// If we've made it this far, this is almost certainly a GoT file.

	// TESTED BY: fmt_dat_got_isinstance_c00
	return DefinitelyYes;
}

ArchivePtr ArchiveType_DAT_GoT::newArchive(stream::inout_sptr psArchive, SuppData& suppData) const
{
	// Create an empty FAT (of 0x00 bytes) and XOR encode it.  We should really
	// use the XOR filter but it's much quicker to do it directly.
	char emptyFAT[GOT_FAT_LENGTH];
	for (int i = 0, j = 128; i < GOT_FAT_LENGTH; i++, j++) {
		emptyFAT[i] = (uint8_t)j;
	}
	psArchive->seekp(0, stream::start);
	psArchive->write(emptyFAT, GOT_FAT_LENGTH);
	return ArchivePtr(new Archive_DAT_GoT(psArchive));
}

ArchivePtr ArchiveType_DAT_GoT::open(stream::inout_sptr psArchive, SuppData& suppData) const
{
	return ArchivePtr(new Archive_DAT_GoT(psArchive));
}

SuppFilenames ArchiveType_DAT_GoT::getRequiredSupps(stream::input_sptr data,
	const std::string& filenameArchive) const
{
	// No supplemental types/empty list
	return SuppFilenames();
}


Archive_DAT_GoT::Archive_DAT_GoT(stream::inout_sptr psArchive)
	:	FATArchive(psArchive, GOT_FIRST_FILE_OFFSET, GOT_MAX_FILENAME_LEN),
		fatSubStream(new stream::sub()),
		fatStream(new stream::seg())
{
	// Create a substream to decrypt the FAT
	this->fatSubStream->open(
		psArchive,
		0,
		GOT_MAX_FILES * GOT_FAT_ENTRY_LEN,
		boost::bind(&Archive_DAT_GoT::truncateFAT, this, _1)
	);

	filter_sptr fatCryptR(new filter_xor_crypt(0, 128));
	filter_sptr fatCryptW(new filter_xor_crypt(0, 128));
#ifdef USE_XOR
	stream::filtered_sptr fatFilter(new stream::filtered());
	fatFilter->open(fatSubStream, fatCryptR, fatCryptW, NULL);
#else
	stream::inout_sptr fatFilter = fatSubStream;
#endif

	this->fatStream->open(fatFilter);

	this->fatStream->seekg(0, stream::start);

	this->vcFAT.reserve(256);

	for (int i = 0; i < GOT_MAX_FILES; i++) {
		FATEntry *fatEntry = new FATEntry();
		EntryPtr ep(fatEntry);

		uint16_t flags;
		this->fatStream
			>> nullPadded(fatEntry->strName, GOT_FILENAME_FIELD_LEN)
			>> u32le(fatEntry->iOffset)
			>> u32le(fatEntry->storedSize)
			>> u32le(fatEntry->realSize)
			>> u16le(flags)
		;

		fatEntry->iIndex = i;
		fatEntry->lenHeader = 0;
		fatEntry->type = FILETYPE_GENERIC;
		if (flags & 1) {
			fatEntry->fAttr = EA_COMPRESSED;
			fatEntry->filter = "lzss-got";
		} else {
			fatEntry->fAttr = 0;
		}
		fatEntry->bValid = true;
		// Blank FAT entries have an offset of zero
		if (fatEntry->iOffset > 0) {
			this->vcFAT.push_back(ep);
		}
	}
}

Archive_DAT_GoT::~Archive_DAT_GoT()
{
}

void Archive_DAT_GoT::flush()
{
	this->fatStream->flush();

	// Commit this->psArchive
	this->FATArchive::flush();
	return;
}

int Archive_DAT_GoT::getSupportedAttributes() const
{
	return EA_COMPRESSED;
}

void Archive_DAT_GoT::updateFileName(const FATEntry *pid, const std::string& strNewName)
{
	// TESTED BY: fmt_got_dat_rename
	assert(strNewName.length() <= GOT_MAX_FILENAME_LEN);
	this->fatStream->seekp(GOT_FILENAME_OFFSET(pid), stream::start);
	this->fatStream << nullPadded(strNewName, GOT_FILENAME_FIELD_LEN);
	return;
}

void Archive_DAT_GoT::updateFileOffset(const FATEntry *pid, stream::delta offDelta)
{
	// TESTED BY: fmt_got_dat_insert*
	// TESTED BY: fmt_got_dat_resize*
	this->fatStream->seekp(GOT_FILEOFFSET_OFFSET(pid), stream::start);
	this->fatStream << u32le(pid->iOffset);
	return;
}

void Archive_DAT_GoT::updateFileSize(const FATEntry *pid, stream::delta sizeDelta)
{
	// TESTED BY: fmt_got_dat_insert*
	// TESTED BY: fmt_got_dat_resize*
	this->fatStream->seekp(GOT_FILESIZE_OFFSET(pid), stream::start);
	this->fatStream
		<< u32le(pid->storedSize)
		<< u32le(pid->realSize)
	;
	return;
}

FATArchive::FATEntry *Archive_DAT_GoT::preInsertFile(const FATEntry *idBeforeThis, FATEntry *pNewEntry)
{
	// TESTED BY: fmt_got_dat_insert*
	assert(pNewEntry->strName.length() <= GOT_MAX_FILENAME_LEN);

	// Set the format-specific variables
	pNewEntry->lenHeader = 0;

	// Make sure there's space for one more entry
	if (this->vcFAT.size() >= GOT_MAX_FILES) {
		throw stream::error("too many files, maximum is "
			TOSTRING(GOT_MAX_FILES));
	}

	if (pNewEntry->fAttr & EA_COMPRESSED) {
		pNewEntry->filter = "lzss-got";
	}

	// Allocate the space in the FAT now, so that the correct offsets can be
	// updated on return.
	this->fatStream->seekp(GOT_FATENTRY_OFFSET(pNewEntry), stream::start);
	this->fatStream->insert(GOT_FAT_ENTRY_LEN);
	boost::to_upper(pNewEntry->strName);

	// Because the FAT is a fixed size we have to remove a blank entry to
	// compensate for the entry we just added.
	if (this->vcFAT.size() > 0) {
		unsigned int indexLast = GOT_MAX_FILES - 1;
		for (VC_ENTRYPTR::reverse_iterator i = this->vcFAT.rbegin(); i != this->vcFAT.rend(); i++) {
			FATEntry *pFAT = dynamic_cast<FATEntry *>(i->get());
			if (pFAT->iIndex != indexLast) {
				// The previous slot is free, so delete it
				this->fatStream->seekp(indexLast * GOT_FAT_ENTRY_LEN, stream::start);
				this->fatStream->remove(GOT_FAT_ENTRY_LEN);
				break;
			} else {
				indexLast = pFAT->iIndex - 1;
			}
		}

		// Make sure an entry was removed.  This should never fail as failure would
		// indicate there were more than GOT_MAX_FILES files, which means an
		// exception should've been thrown at the start of this function.
		//assert(indexLast >= 0);
		assert(indexLast < GOT_MAX_FILES); // unsigned version of previous line
	} else {
		// No files so just remove the following entry
		this->fatStream->seekp(1 * GOT_FAT_ENTRY_LEN, stream::start);
		this->fatStream->remove(GOT_FAT_ENTRY_LEN);
	}

	return pNewEntry;
}

void Archive_DAT_GoT::postInsertFile(FATEntry *pNewEntry)
{
	// Write out the entry into the space we allocated in preInsertFile(),
	// now that the sizes are set.
	this->fatStream->seekp(GOT_FATENTRY_OFFSET(pNewEntry), stream::start);
	uint16_t flags = (pNewEntry->fAttr & EA_COMPRESSED) ? 1 : 0; // 0 == not compressed
	this->fatStream
		<< nullPadded(pNewEntry->strName, GOT_FILENAME_FIELD_LEN)
		<< u32le(pNewEntry->iOffset)
		<< u32le(pNewEntry->storedSize)
		<< u32le(pNewEntry->realSize)
		<< u16le(flags)
	;
	return;
}

void Archive_DAT_GoT::preRemoveFile(const FATEntry *pid)
{
	// TESTED BY: fmt_got_dat_remove*

	// Remove the FAT entry
	this->fatStream->seekp(GOT_FATENTRY_OFFSET(pid), stream::start);
	this->fatStream->remove(GOT_FAT_ENTRY_LEN);

	// Add an empty FAT entry onto the end to keep the FAT the same size
	const FATEntry *pFAT = dynamic_cast<const FATEntry *>(this->vcFAT.back().get());
	this->fatStream->seekp((pFAT->iIndex + 1) * GOT_FAT_ENTRY_LEN, stream::start);
	this->fatStream->insert(GOT_FAT_ENTRY_LEN);

	return;
}

void Archive_DAT_GoT::truncateFAT(stream::pos newSize)
{
	// Sanity check to make sure the FAT is not actually changing size.
	assert(newSize == GOT_FAT_LENGTH);
	return;
}

} // namespace gamearchive
} // namespace camoto

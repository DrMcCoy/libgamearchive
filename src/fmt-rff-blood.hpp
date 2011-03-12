/**
 * @file   fmt-rff-blood.hpp
 * @brief  Implementation of reader/writer for Blood's .RFF format.
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

#ifndef _CAMOTO_FMT_RFF_BLOOD_HPP_
#define _CAMOTO_FMT_RFF_BLOOD_HPP_

#include <camoto/gamearchive.hpp>
#include <camoto/substream.hpp>
#include "fatarchive.hpp"

namespace camoto {
namespace gamearchive {

class RFFType: virtual public ArchiveType {

	public:

		RFFType()
			throw ();

		virtual ~RFFType()
			throw ();

		virtual std::string getArchiveCode() const
			throw ();

		virtual std::string getFriendlyName() const
			throw ();

		virtual std::vector<std::string> getFileExtensions() const
			throw ();

		virtual std::vector<std::string> getGameList() const
			throw ();

		virtual E_CERTAINTY isInstance(iostream_sptr fsArchive) const
			throw (std::ios::failure);

		virtual ArchivePtr newArchive(iostream_sptr psArchive, MP_SUPPDATA& suppData) const
			throw (std::ios::failure);

		virtual ArchivePtr open(iostream_sptr fsArchive, MP_SUPPDATA& suppData) const
			throw (std::ios::failure);

		virtual MP_SUPPLIST getRequiredSupps(const std::string& filenameArchive) const
			throw ();

};

class RFFArchive: virtual public FATArchive {

	protected:

		substream_sptr fatSubStream;  ///< Substream storing the FAT
		uint32_t version;             ///< File format version

	public:

		RFFArchive(iostream_sptr psArchive)
			throw (std::ios::failure);

		virtual ~RFFArchive()
			throw ();

		// As per Archive (see there for docs)

		virtual void rename(EntryPtr& id, const std::string& strNewName)
			throw (std::ios_base::failure);

		virtual MetadataTypes getMetadataList() const
			throw ();

		virtual std::string getMetadata(MetadataType item) const
			throw (std::ios::failure);

		virtual void setMetadata(MetadataType item, const std::string& value)
			throw (std::ios::failure);

		/// Write out the FAT with the updated encryption key.
		virtual void flush()
			throw (std::ios::failure);

		// As per FATArchive (see there for docs)

		virtual void updateFileOffset(const FATEntry *pid, std::streamsize offDelta)
			throw (std::ios::failure);

		virtual void updateFileSize(const FATEntry *pid, std::streamsize sizeDelta)
			throw (std::ios_base::failure);

		virtual FATEntry *preInsertFile(const FATEntry *idBeforeThis, FATEntry *pNewEntry)
			throw (std::ios_base::failure);

		virtual void postInsertFile(FATEntry *pNewEntry)
			throw (std::ios::failure);

		virtual void postRemoveFile(const FATEntry *pid)
			throw (std::ios::failure);

	protected:
		void updateFileCount(uint32_t newCount)
			throw (std::ios::failure);

		// Update the header with the offset of the FAT (which sits at the end of
		// the archive, after the file data.)
		void updateFATOffset(std::streamsize delta)
			throw (std::ios_base::failure);

		io::stream_offset getDescOffset() const
			throw (std::ios_base::failure);

		void splitFilename(const std::string& full, std::string *base, std::string *ext)
			throw (std::ios::failure);

};

} // namespace gamearchive
} // namespace camoto

#endif // _CAMOTO_FMT_RFF_BLOOD_HPP_
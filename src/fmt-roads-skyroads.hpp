/**
 * @file  fmt-roads-skyroads.hpp
 * @brief Skyroads ROADS.LZS format.
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

#ifndef _CAMOTO_FMT_ROADS_SKYROADS_HPP_
#define _CAMOTO_FMT_ROADS_SKYROADS_HPP_

#include <camoto/gamearchive/archivetype.hpp>
#include "fatarchive.hpp"

namespace camoto {
namespace gamearchive {

/// Skyroads ROADS.LZS format handler.
class ArchiveType_Roads_SkyRoads: virtual public ArchiveType
{
	public:
		ArchiveType_Roads_SkyRoads();
		virtual ~ArchiveType_Roads_SkyRoads();

		virtual std::string getArchiveCode() const;
		virtual std::string getFriendlyName() const;
		virtual std::vector<std::string> getFileExtensions() const;
		virtual std::vector<std::string> getGameList() const;
		virtual ArchiveType::Certainty isInstance(stream::input_sptr fsArchive)
			const;
		virtual ArchivePtr newArchive(stream::inout_sptr psArchive,
			SuppData& suppData) const;
		virtual ArchivePtr open(stream::inout_sptr fsArchive, SuppData& suppData)
			const;
		virtual SuppFilenames getRequiredSupps(stream::input_sptr data,
			const std::string& filenameArchive) const;
};

/// Skyroads ROADS.LZS archive instance.
class Archive_Roads_SkyRoads: virtual public FATArchive
{
	public:
		Archive_Roads_SkyRoads(stream::inout_sptr psArchive);
		virtual ~Archive_Roads_SkyRoads();

		virtual void updateFileName(const FATEntry *pid,
			const std::string& strNewName);
		virtual void updateFileOffset(const FATEntry *pid, stream::delta offDelta);
		virtual void updateFileSize(const FATEntry *pid, stream::delta sizeDelta);
		virtual FATEntry *preInsertFile(const FATEntry *idBeforeThis,
			FATEntry *pNewEntry);
		virtual void preRemoveFile(const FATEntry *pid);
};

} // namespace gamearchive
} // namespace camoto

#endif // _CAMOTO_FMT_ROADS_SKYROADS_HPP_

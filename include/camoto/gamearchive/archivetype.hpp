/**
 * @file  camoto/gamearchive/archivetype.hpp
 * @brief ArchiveType class, used to identify and open an instance of a
 *        particular archive format.
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

#ifndef _CAMOTO_GAMEARCHIVE_ARCHIVETYPE_HPP_
#define _CAMOTO_GAMEARCHIVE_ARCHIVETYPE_HPP_

#include <vector>
#include <map>

#include <camoto/stream.hpp>
#include <camoto/suppitem.hpp>
#include <camoto/gamearchive/archive.hpp>

/// Main namespace
namespace camoto {
/// Namespace for this library
namespace gamearchive {

/// Interface to a particular archive format.
class ArchiveType
{
	public:
		/// Confidence level when guessing a file format.
		enum Certainty {
			DefinitelyNo,  ///< Definitely not in this format
			Unsure,        ///< The checks were inconclusive, it could go either way
			PossiblyYes,   ///< Everything checked out OK, but there's no signature
			DefinitelyYes, ///< This format has a signature and it matched.
		};

		/// Get a short code to identify this file format, e.g. "grp-duke3d"
		/**
		 * This can be useful for command-line arguments.
		 *
		 * @return The archive short name/ID.
		 */
		virtual std::string getArchiveCode() const = 0;

		/// Get the archive name, e.g. "Duke Nukem 3D GRP file"
		/**
		 * @return The archive name.
		 */
		virtual std::string getFriendlyName() const = 0;

		/// Get a list of the known file extensions for this format.
		/**
		 * @return A vector of file extensions, e.g. "vol", "stn", "cmp"
		 */
		virtual std::vector<std::string> getFileExtensions() const = 0;

		/// Get a list of games using this format.
		/**
		 * @return A vector of game names, such as "Major Stryker", "Cosmo's Cosmic
		 *   Adventures", "Duke Nukem II"
		 */
		virtual std::vector<std::string> getGameList() const = 0;

		/// Check a stream to see if it's in this archive format.
		/**
		 * @param psArchive
		 *   A C++ iostream of the file to test.
		 *
		 * @return A single confidence value from \ref ArchiveType::Certainty.
		 */
		virtual ArchiveType::Certainty isInstance(stream::input_sptr psArchive) const = 0;

		/// Create a blank archive in this format.
		/**
		 * This function writes out the necessary signatures and headers to create
		 * a valid blank archive in this format.
		 *
		 * Note to format implementors: This function only needs to be overridden
		 * if there are headers to write, otherwise an empty stream is passed to
		 * open() which is expected to succeed.
		 *
		 * @param psArchive
		 *   A blank stream to store the new archive in.
		 *
		 * @param suppData
		 *   Any supplemental data required by this format (see getRequiredSupps()).
		 *
		 * @return A pointer to an instance of the Archive class, just as if a
		 *   valid empty file had been opened by open().
		 */
		virtual ArchivePtr newArchive(stream::inout_sptr psArchive,
			SuppData& suppData) const = 0;

		/// Open an archive file.
		/**
		 * @pre Recommended that isInstance() has returned > DefinitelyNo.
		 *
		 * @param psArchive
		 *   The archive file to read.
		 *
		 * @param suppData
		 *   Any supplemental data required by this format (see getRequiredSupps()).
		 *
		 * @return A pointer to an instance of the Archive class.  Will throw an
		 *   exception if the data is invalid (i.e. if isInstance() returned
		 *   DefinitelyNo) however it will try its best to read the data anyway, to
		 *   make it possible to "force" a file to be opened by a particular format
		 *   handler.
		 */
		virtual ArchivePtr open(stream::inout_sptr psArchive, SuppData& suppData)
			const = 0;

		/// Get a list of any required supplemental files.
		/**
		 * For some archive formats, data is stored externally to the archive file
		 * itself (for example the filenames may be stored in a different file than
		 * the actual file data.)  This function obtains a list of these
		 * supplementary files, so the caller can open them and pass them along
		 * to the archive manipulation classes.
		 *
		 * @param data
		 *   Read-only stream containing the archive content.  This is for archives
		 *   which contain the names of the other files they need.
		 *
		 * @param filenameArchive
		 *   The filename of the archive (no path.)  This is for supplemental files
		 *   which share the same base name as the archive, but a different filename
		 *   extension.
		 *
		 * @return A (possibly empty) map associating required supplemental file
		 *   types with their filenames.  For each returned value the file should be
		 *   opened and added to a \ref SuppData map, where it can be passed to
		 *   newArchive() or open().  Note that the filenames returned can have
		 *   relative paths, and may even have an absolute path, if one was passed
		 *   in with filenameArchive.
		 */
		virtual SuppFilenames getRequiredSupps(stream::input_sptr data,
			const std::string& filenameArchive) const = 0;
};

/// Shared pointer to an ArchiveType.
typedef boost::shared_ptr<ArchiveType> ArchiveTypePtr;

/// Vector of ArchiveType shared pointers.
typedef std::vector<ArchiveTypePtr> ArchiveTypeVector;

} // namespace gamearchive
} // namespace camoto

#endif // _CAMOTO_GAMEARCHIVE_ARCHIVETYPE_HPP_

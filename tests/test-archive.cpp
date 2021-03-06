/**
 * @file   test-archive.cpp
 * @brief  Generic test code for Archive class descendents.
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

#include <iomanip>
#include <boost/bind.hpp>
#include <camoto/util.hpp>
#include "test-archive.hpp"

#include "../src/fatarchive.hpp" // getFileAt()

using namespace camoto;
using namespace camoto::gamearchive;

/// Check whether a supp item is present and if so that the content is correct.
#define CHECK_SUPP_ITEM(item, check_func, msg) \
	if (this->suppResult[camoto::SuppItem::item]) { \
		BOOST_CHECK_MESSAGE( \
			this->is_supp_equal(camoto::SuppItem::item, \
				this->suppResult[camoto::SuppItem::item]->check_func()), \
			"[SuppItem::" TOSTRING(item) "] " msg \
		); \
	}

/// Callback function to set expanded/native file size.
void setRealSize(camoto::gamearchive::ArchivePtr arch,
	camoto::gamearchive::Archive::EntryPtr id, stream::len newRealSize)
{
	BOOST_TEST_MESSAGE(createString("Leaving stored size unchanged as "
		<< id->storedSize << " and setting real size to " << newRealSize));
	arch->resize(id, id->storedSize, newRealSize);
	return;
}

/// Apply the correct filter to the stream.
/**
 * If the given entry pointer has a filter attached, apply it to the given
 * stream pointer.
 *
 * @note This function will always apply the filter, don't call it if the user
 *   has given the -u option to bypass filtering.
 *
 * @param pStream
 *   Shared pointer to the stream.
 *
 * @param id
 *   EntryPtr for the stream.
 *
 * @return A stream providing filtered data from pStream.
 */
stream::inout_sptr applyFilter(gamearchive::ArchivePtr arch,
	gamearchive::Archive::EntryPtr id, stream::inout_sptr pStream)
{
	if (!id->filter.empty()) {
		// The file needs to be filtered first
		gamearchive::ManagerPtr pManager(gamearchive::getManager());
		gamearchive::FilterTypePtr pFilterType(
			pManager->getFilterTypeByCode(id->filter));
		if (!pFilterType) {
			throw stream::error(createString(
				"could not find filter \"" << id->filter << "\""
			));
		}

		stream::fn_truncate fn_resize = boost::bind<void>(setRealSize, arch, id, _1);
		return pFilterType->apply(pStream, fn_resize);
	}
	return pStream; // no filters to apply
}

test_archive::test_archive()
	:	init(false),
		numIsInstanceTests(0),
		numInvalidContentTests(1),
		numChangeMetadataTests(1)
{
	this->create = true;

	this->filename[0] = "ONE.DAT";
	this->filename[1] = "TWO.DAT";
	this->filename[2] = "THREE.DAT";
	this->filename[3] = "FOUR.DAT";
	this->filename_shortext = "TEST.A";
	this->lenMaxFilename = 12;
	this->insertAttr = EA_NONE;
	this->insertType = FILETYPE_GENERIC;

	this->content[0] = "This is one.dat";
	this->content[1] = "This is two.dat";
	this->content[2] = "This is three.dat";
	this->content[3] = "This is four.dat";
	this->content0_overwritten = "Now resized to 23 chars";

	//this->content0_normalSize = 15;
	this->content0_largeSize = 20;
	this->content0_smallSize = 10;
//	this->content0_overwrittenSize = this->content0_overwritten.length();
	this->content0_largeSize_unfiltered = this->content0_largeSize;
	this->content0_smallSize_unfiltered = this->content0_smallSize;

	this->hasMetadata[camoto::Metadata::Description] = false;
	this->hasMetadata[camoto::Metadata::Version] = false;

	this->metadataDesc = "Metadata description";
	this->metadataVer = "123";
}

void test_archive::addTests()
{
	// Tests on existing archives (in the initial state)

	ADD_ARCH_TEST(false, &test_archive::test_isinstance_others);
	ADD_ARCH_TEST(false, &test_archive::test_open);
	if (this->lenMaxFilename >= 0) {
		// Only perform the rename test if the archive has filenames
		ADD_ARCH_TEST(false, &test_archive::test_rename);
		ADD_ARCH_TEST(false, &test_archive::test_shortext);
	}
	if (this->lenMaxFilename > 0) {
		// Only perform these tests if the archive has a filename length limit
		ADD_ARCH_TEST(false, &test_archive::test_rename_long);
		ADD_ARCH_TEST(false, &test_archive::test_insert_long);
	}
	ADD_ARCH_TEST(false, &test_archive::test_insert_mid);
	ADD_ARCH_TEST(false, &test_archive::test_insert_end);
	ADD_ARCH_TEST(false, &test_archive::test_insert2);
	ADD_ARCH_TEST(false, &test_archive::test_remove);
	ADD_ARCH_TEST(false, &test_archive::test_remove2);
	ADD_ARCH_TEST(false, &test_archive::test_insert_remove);
	ADD_ARCH_TEST(false, &test_archive::test_remove_insert);
	ADD_ARCH_TEST(false, &test_archive::test_move);
	ADD_ARCH_TEST(false, &test_archive::test_resize_larger);
	ADD_ARCH_TEST(false, &test_archive::test_resize_smaller);
	ADD_ARCH_TEST(false, &test_archive::test_resize_write);
	ADD_ARCH_TEST(false, &test_archive::test_remove_all_re_add);
	ADD_ARCH_TEST(false, &test_archive::test_insert_zero_then_resize);
	ADD_ARCH_TEST(false, &test_archive::test_resize_over64k);

	// Only perform the metadata tests if supported by the archive format
	if (this->hasMetadata[camoto::Metadata::Description]) {
		ADD_ARCH_TEST(false, &test_archive::test_metadata_get_desc);
		ADD_ARCH_TEST(false, &test_archive::test_metadata_set_desc_larger);
		ADD_ARCH_TEST(false, &test_archive::test_metadata_set_desc_smaller);
	}
	if (this->hasMetadata[camoto::Metadata::Version]) {
		ADD_ARCH_TEST(false, &test_archive::test_metadata_get_ver);
	}

	// Tests on new archives (in an empty state)
	if (this->create) {
		ADD_ARCH_TEST(true, &test_archive::test_new_isinstance);
		ADD_ARCH_TEST(true, &test_archive::test_new_to_initialstate);
		ADD_ARCH_TEST(true, &test_archive::test_new_manipulate_zero_length_files);
	}

	return;
}

void test_archive::addBoundTest(bool empty, boost::function<void()> fnTest,
	boost::unit_test::const_string name)
{
	boost::function<void()> fnTestWrapper = boost::bind(&test_archive::runTest,
		this, empty, fnTest);
	this->ts->add(boost::unit_test::make_test_case(
		boost::unit_test::callback0<>(fnTestWrapper),
		createString(name << '[' << this->basename << ']')
	));
	return;
}

void test_archive::runTest(bool empty, boost::function<void()> fnTest)
{
	this->prepareTest(empty);
	fnTest();
	return;
}

void test_archive::prepareTest(bool emptyArchive)
{
	if (!this->init) {
		ManagerPtr pManager;
		BOOST_REQUIRE_NO_THROW(
			pManager = getManager();
			this->pArchType = pManager->getArchiveTypeByCode(this->type);
		);
		BOOST_REQUIRE_MESSAGE(pArchType, "Could not find archive type " + this->type);
		this->init = true;
	}

	if (this->suppResult[SuppItem::FAT]) {
		stream::string_sptr suppSS(new stream::string());
		if (!emptyArchive) {
			// Populate the suppitem with its initial state
			suppSS << this->suppResult[SuppItem::FAT]->initialstate();
		}
		this->suppData[SuppItem::FAT] = suppSS;
	}

	this->base.reset(new stream::string());

	if (emptyArchive) {
		BOOST_TEST_CHECKPOINT("About to create new empty instance of "
			+ this->basename);
		// This should really use BOOST_REQUIRE_NO_THROW but the message is more
		// informative without it.
		//BOOST_REQUIRE_NO_THROW(
			this->pArchive = this->pArchType->newArchive(this->base, this->suppData);
		//);
	} else {
		this->base << this->initialstate();
		BOOST_TEST_CHECKPOINT("About to open " + this->basename
			+ " initialstate as an archive");
		// This should really use BOOST_REQUIRE_NO_THROW but the message is more
		// informative without it.
		//BOOST_REQUIRE_NO_THROW(
		this->pArchive = this->pArchType->open(this->base, this->suppData);
		//);
	}
	BOOST_REQUIRE_MESSAGE(this->pArchive, "Could not create archive class");

	if (this->lenMaxFilename < 0) {
		// This format has no filenames, so set them to obviously incorrect values.
		this->filename[0] = "dummy";
		this->filename[1] = "dummy";
		this->filename[2] = "dummy";
		this->filename[3] = "dummy";
	}
	return;
}

Archive::EntryPtr test_archive::findFile(unsigned int index,
	const std::string& altname)
{
	Archive::EntryPtr ep;
	if (this->lenMaxFilename >= 0) {
		// Find the file we're going to open by name
		std::string filename = altname;
		if (filename.empty()) {
			switch (index) {
				case 0: filename = this->filename[0]; break;
				case 1: filename = this->filename[1]; break;
				case 2: filename = this->filename[2]; break;
				case 3: filename = this->filename[3]; break;
				default:
					BOOST_REQUIRE_MESSAGE(index < 4, "findFile() index out of range");
					break;
			}
		}
		ep = this->pArchive->find(filename);

		// Make sure we found it
		BOOST_REQUIRE_MESSAGE(this->pArchive->isValid(ep),
			createString("Couldn't find " << filename << " in sample archive"));

	} else {
		// No filenames in this format, do it by order
		const Archive::VC_ENTRYPTR& files = this->pArchive->getFileList();
		ep = getFileAt(files, index);

		// Make sure we found it
		BOOST_REQUIRE_MESSAGE(this->pArchive->isValid(ep),
			createString("Couldn't find file at index " << index
				<< " in sample archive"));
	}
	return ep;
}

void test_archive::isInstance(ArchiveType::Certainty result,
	const std::string& content)
{
	boost::function<void()> fnTest = boost::bind(&test_archive::test_isInstance,
		this, result, content, this->numIsInstanceTests);
	this->ts->add(boost::unit_test::make_test_case(
			boost::unit_test::callback0<>(fnTest),
			createString("test_archive[" << this->basename << "]::isinstance_c"
				<< std::setfill('0') << std::setw(2) << this->numIsInstanceTests)
		));
	this->numIsInstanceTests++;
	return;
}

void test_archive::test_isInstance(ArchiveType::Certainty result,
	const std::string& content, unsigned int testNumber)
{
	BOOST_TEST_MESSAGE(createString("isInstance check (" << this->basename
		<< "; " << std::setfill('0') << std::setw(2) << testNumber << ")"));

	ManagerPtr pManager(getManager());
	ArchiveTypePtr pTestType(pManager->getArchiveTypeByCode(this->type));
	BOOST_REQUIRE_MESSAGE(pTestType,
		createString("Could not find archive type " << this->type));

	stream::string_sptr ss(new stream::string());
	ss << content;

	BOOST_CHECK_EQUAL(pTestType->isInstance(ss), result);
	return;
}

void test_archive::invalidContent(const std::string& content)
{
	boost::function<void()> fnTest = boost::bind(&test_archive::test_invalidContent,
		this, content, this->numInvalidContentTests);
	this->ts->add(boost::unit_test::make_test_case(
			boost::unit_test::callback0<>(fnTest),
			createString("test_archive[" << this->basename << "]::invalidcontent_i"
				<< std::setfill('0') << std::setw(2) << this->numInvalidContentTests)
		));
	this->numInvalidContentTests++;
	return;
}

void test_archive::test_invalidContent(const std::string& content,
	unsigned int testNumber)
{
	BOOST_TEST_MESSAGE(createString("invalidContent check (" << this->basename
		<< "; " << std::setfill('0') << std::setw(2) << testNumber << ")"));

	ManagerPtr pManager(getManager());
	ArchiveTypePtr pTestType(pManager->getArchiveTypeByCode(this->type));
	BOOST_REQUIRE_MESSAGE(pTestType,
		createString("Could not find archive type " << this->type));

	stream::string_sptr ss(new stream::string());
	ss << content;

	// Make sure isInstance reports this is valid
	BOOST_CHECK_EQUAL(pTestType->isInstance(ss), ArchiveType::DefinitelyYes);

	// But that we get an error when trying to open the file
	BOOST_CHECK_THROW(
		ArchivePtr pArchive(pTestType->open(ss, this->suppData)),
		stream::error
	);

	return;
}

void test_archive::changeMetadata(camoto::Metadata::MetadataType item,
	const std::string& newValue, const std::string& content)
{
	boost::function<void()> fnTest = boost::bind(&test_archive::test_changeMetadata,
		this, item, newValue, content, this->numChangeMetadataTests);
	this->ts->add(boost::unit_test::make_test_case(
			boost::unit_test::callback0<>(fnTest),
			createString("test_archive[" << this->basename << "]::changemetadata_c"
				<< std::setfill('0') << std::setw(2) << this->numIsInstanceTests)
		));
	this->numChangeMetadataTests++;
	return;
}

void test_archive::test_changeMetadata(camoto::Metadata::MetadataType item,
	const std::string& newValue, const std::string& content,
	unsigned int testNumber)
{
	BOOST_TEST_MESSAGE(createString("changeMetadata check (" << this->basename
		<< "; " << std::setfill('0') << std::setw(2) << testNumber << ")"));

	this->prepareTest(false);
	this->pArchive->setMetadata(item, newValue);

	stream::string_sptr ss(new stream::string());
	ss << content;

	BOOST_CHECK_MESSAGE(
		this->is_content_equal(content),
		"Error setting metadata field"
	);
/*
	CHECK_SUPP_ITEM(FAT, metadata_set_desc_larger,
		"Error setting 'description' metadata field");
*/
	return;
}

boost::test_tools::predicate_result test_archive::is_content_equal(
	const std::string& exp)
{
	// Flush out any changes before we perform the check
	BOOST_CHECK_NO_THROW(
		this->pArchive->flush()
	);

	return this->is_equal(exp, *(this->base->str()));
}

boost::test_tools::predicate_result test_archive::is_supp_equal(
	camoto::SuppItem::Type type, const std::string& strExpected)
{
	// Flush out any changes to the main archive before we perform the check,
	// in case this function was called first.
	BOOST_CHECK_NO_THROW(
		this->pArchive->flush()
	);
	stream::string_sptr suppBase =
		boost::dynamic_pointer_cast<stream::string>(this->suppData[type]);

	// Use the supp's test-class' own comparison function, as this will use its
	// preferred outputWidth value, which might be different to the main file's.
	return this->suppResult[type]->is_equal(strExpected, *(suppBase->str()));
}

void test_archive::test_isinstance_others()
{
	// Check all file formats except this one to avoid any false positives
	BOOST_TEST_MESSAGE("isInstance check for other formats (not " << this->type
		<< ")");
	ManagerPtr pManager(camoto::gamearchive::getManager());
	int i = 0;
	ArchiveTypePtr pTestType;
	for (int i = 0; (pTestType = pManager->getArchiveType(i)); i++) {
		// Don't check our own type, that's done by the other isinstance_* tests
		std::string otherType = pTestType->getArchiveCode();
		if (otherType.compare(this->type) == 0) continue;

		// Skip any formats known to produce false detections unavoidably
		if (
			std::find(
				this->skipInstDetect.begin(), this->skipInstDetect.end(), otherType
			) != this->skipInstDetect.end()) continue;

		BOOST_TEST_MESSAGE("Checking " << this->type
			<< " content against isInstance() for " << otherType);

		BOOST_CHECK_MESSAGE(pTestType->isInstance(base) < ArchiveType::DefinitelyYes,
			"isInstance() for " << otherType << " incorrectly recognises content for "
			<< this->type);
	}
	return;
}

void test_archive::test_open()
{
	BOOST_TEST_MESSAGE("Opening file in archive");

	Archive::EntryPtr ep = this->findFile(0);

	// Open it
	camoto::stream::inout_sptr pfsIn(this->pArchive->open(ep));
	stream::string_sptr out(new stream::string());

	// Make sure the file opens at the start
	BOOST_REQUIRE_EQUAL(pfsIn->tellg(), 0);

	// Apply any decryption/decompression filter
	pfsIn = applyFilter(this->pArchive, ep, pfsIn);

	// Copy it into the stringstream
	stream::copy(out, pfsIn);

	BOOST_CHECK_MESSAGE(
		this->is_equal(this->content[0], *(out->str())),
		"Error opening file or wrong file opened"
	);
}

void test_archive::test_rename()
{
	BOOST_TEST_MESSAGE("Renaming file inside archive");

	BOOST_REQUIRE_MESSAGE(this->lenMaxFilename >= 0,
		"Tried to run test_archive::test_rename() on a format with no filenames!");

	Archive::EntryPtr ep = this->findFile(0);

	this->pArchive->rename(ep, this->filename[2]);

	BOOST_CHECK_MESSAGE(
		this->is_content_equal(this->rename()),
		"Error renaming file"
	);

	CHECK_SUPP_ITEM(FAT, rename, "Error renaming file");
}

void test_archive::test_rename_long()
{
	BOOST_TEST_MESSAGE("Rename file with name too long");

	BOOST_REQUIRE_MESSAGE(this->lenMaxFilename >= 0,
		"Tried to run test_archive::test_rename_long() on a format with no filenames!");
	BOOST_REQUIRE_MESSAGE(this->lenMaxFilename > 0,
		"Tried to run test_archive::test_rename_long() on a format with unlimited-length filenames!");

	Archive::EntryPtr ep = this->findFile(0);

	assert(this->lenMaxFilename < 256);
	char name[256 + 2];
	memset(name, 'A', this->lenMaxFilename + 1);
	name[this->lenMaxFilename + 1] = 0;

	// Make sure renaming fails when the filename is too long
	BOOST_CHECK_THROW(
		this->pArchive->rename(ep, name),
		stream::error
	);

	BOOST_CHECK_MESSAGE(
		this->is_content_equal(this->initialstate()),
		"Archive corrupted after failed rename"
	);

	CHECK_SUPP_ITEM(FAT, initialstate, "Archive corrupted after failed rename");

	memset(name, 'A', this->lenMaxFilename);
	name[this->lenMaxFilename - 4] = '.';
	name[this->lenMaxFilename] = 0;

	// Make sure the rename succeeds when the filename is exactly the max length
	BOOST_CHECK_NO_THROW(
		this->pArchive->rename(ep, name)
	);

}

void test_archive::test_insert_long()
{
	BOOST_TEST_MESSAGE("Inserting file with name too long");

	BOOST_REQUIRE_MESSAGE(this->lenMaxFilename >= 0,
		"Tried to run test_archive::test_insert_long() on a format with no filenames!");
	BOOST_REQUIRE_MESSAGE(this->lenMaxFilename > 0,
		"Tried to run test_archive::test_insert_long() on a format with unlimited-length filenames!");

	Archive::EntryPtr epb = this->findFile(0);

	assert(this->lenMaxFilename < 256);
	char name[256 + 2];
	memset(name, 'A', this->lenMaxFilename + 1);
	name[this->lenMaxFilename + 1] = 0;

	BOOST_CHECK_THROW(
		Archive::EntryPtr ep = this->pArchive->insert(epb, name,
			this->content[0].length(), this->insertType, this->insertAttr),
		stream::error
	);

	BOOST_CHECK_MESSAGE(
		this->is_content_equal(this->initialstate()),
		"Archive corrupted after failed insert"
	);

	CHECK_SUPP_ITEM(FAT, initialstate, "Archive corrupted after failed insert");

	memset(name, 'A', this->lenMaxFilename);
	name[this->lenMaxFilename - 4] = '.';
	name[this->lenMaxFilename] = 0;

	BOOST_CHECK_NO_THROW(
		Archive::EntryPtr ep = this->pArchive->insert(epb, name,
			this->content[0].length(), this->insertType, this->insertAttr)
	);

}

void test_archive::test_insert_end()
{
	BOOST_TEST_MESSAGE("Inserting file at end of archive");

	// Insert the file
	Archive::EntryPtr ep = this->pArchive->insert(Archive::EntryPtr(),
		this->filename[2], this->content[2].length(), this->insertType,
		this->insertAttr);

	// Make sure it went in ok
	BOOST_REQUIRE_MESSAGE(this->pArchive->isValid(ep),
		"Couldn't create new file in sample archive");

	// Open it
	stream::inout_sptr pfsNew(this->pArchive->open(ep));

	// Apply any encryption/compression filter
	pfsNew = applyFilter(this->pArchive, ep, pfsNew);

	// Set the size of the file we want to write
	pfsNew->truncate(this->content[2].length());
	pfsNew->seekp(0, stream::start);
	pfsNew->write(this->content[2]);
	pfsNew->flush();

	BOOST_CHECK_MESSAGE(
		this->is_content_equal(this->insert_end()),
		"Error inserting file at end of archive"
	);

	CHECK_SUPP_ITEM(FAT, insert_end, "Error inserting file at end of archive");
}

void test_archive::test_insert_mid()
{
	BOOST_TEST_MESSAGE("Inserting file into middle of archive");

	Archive::EntryPtr epBefore = this->findFile(1);

	// Insert the file
	Archive::EntryPtr ep = this->pArchive->insert(epBefore, this->filename[2],
		this->content[2].length(), this->insertType, this->insertAttr);

	// Make sure it went in ok
	BOOST_REQUIRE_MESSAGE(this->pArchive->isValid(ep),
		"Couldn't insert new file in sample archive");

	// Open it
	stream::inout_sptr pfsNew(this->pArchive->open(ep));

	// Apply any encryption/compression filter
	pfsNew = applyFilter(this->pArchive, ep, pfsNew);

	pfsNew->write(this->content[2]);
	pfsNew->flush();

	BOOST_CHECK_MESSAGE(
		this->is_content_equal(this->insert_mid()),
		"Error inserting file in middle of archive"
	);

	CHECK_SUPP_ITEM(FAT, insert_mid, "Error inserting file in middle of archive");
}

void test_archive::test_insert2()
{
	BOOST_TEST_MESSAGE("Inserting multiple files");

	Archive::EntryPtr epBefore = this->findFile(1);

	// Insert the file
	Archive::EntryPtr ep1 = this->pArchive->insert(epBefore, this->filename[2],
		this->content[2].length(), this->insertType, this->insertAttr);

	// Make sure it went in ok
	BOOST_REQUIRE_MESSAGE(this->pArchive->isValid(ep1),
		"Couldn't insert first new file in sample archive");

	// Open it
	stream::inout_sptr pfsNew1(this->pArchive->open(ep1));

	// Apply any encryption/compression filter
	pfsNew1 = applyFilter(this->pArchive, ep1, pfsNew1);

	pfsNew1->write(this->content[2]);
	pfsNew1->flush();

	epBefore = this->findFile(2, this->filename[1]); // FILENAME2 is now the third fil

	// Insert the file
	Archive::EntryPtr ep2 = this->pArchive->insert(epBefore, this->filename[3],
		this->content[3].length(), this->insertType, this->insertAttr);

	// Make sure it went in ok
	BOOST_REQUIRE_MESSAGE(this->pArchive->isValid(ep2),
		"Couldn't insert second new file in sample archive");

	stream::inout_sptr pfsNew2(this->pArchive->open(ep2));

	// Apply any encryption/compression filter
	pfsNew2 = applyFilter(this->pArchive, ep2, pfsNew2);

	pfsNew2->write(this->content[3]);
	pfsNew2->flush();

	BOOST_CHECK_MESSAGE(
		this->is_content_equal(this->insert2()),
		"Error inserting two files"
	);

	CHECK_SUPP_ITEM(FAT, insert2, "Error inserting two files");
}

void test_archive::test_remove()
{
	BOOST_TEST_MESSAGE("Removing file from archive");

	Archive::EntryPtr ep = this->findFile(0);

	// Remove it
	this->pArchive->remove(ep);

	BOOST_CHECK_MESSAGE(
		this->is_content_equal(this->remove()),
		"Error removing file"
	);

	CHECK_SUPP_ITEM(FAT, remove, "Error removing file");
}

void test_archive::test_remove2()
{
	BOOST_TEST_MESSAGE("Removing multiple files from archive");

	Archive::EntryPtr ep1 = this->findFile(0);
	Archive::EntryPtr ep2 = this->findFile(1);

	// Remove it
	this->pArchive->remove(ep1);
	this->pArchive->remove(ep2);

	BOOST_CHECK_MESSAGE(
		this->is_content_equal(this->remove2()),
		"Error removing multiple files"
	);

	CHECK_SUPP_ITEM(FAT, remove2, "Error removing multiple files");
}

void test_archive::test_insert_remove()
{
	BOOST_TEST_MESSAGE("Insert then remove file from archive");

	Archive::EntryPtr epBefore = this->findFile(1);

	// Insert the file
	Archive::EntryPtr ep = this->pArchive->insert(epBefore, this->filename[2],
		this->content[2].length(), this->insertType, this->insertAttr);

	// Make sure it went in ok
	BOOST_REQUIRE_MESSAGE(this->pArchive->isValid(ep),
		"Couldn't insert new file in sample archive");

	// Open it
	stream::inout_sptr pfsNew(this->pArchive->open(ep));

	// Apply any encryption/compression filter
	pfsNew = applyFilter(this->pArchive, ep, pfsNew);

	pfsNew->write(this->content[2]);
	pfsNew->flush();

	Archive::EntryPtr ep2 = this->findFile(0);

	// Remove it
	this->pArchive->remove(ep2);

	BOOST_CHECK_MESSAGE(
		this->is_content_equal(this->insert_remove()),
		"Error inserting then removing file"
	);

	CHECK_SUPP_ITEM(FAT, insert_remove, "Error inserting then removing file");
}

void test_archive::test_remove_insert()
{
	BOOST_TEST_MESSAGE("Remove then insert file from archive");

	Archive::EntryPtr ep2 = this->findFile(0);

	// Remove it
	this->pArchive->remove(ep2);

	Archive::EntryPtr epBefore = this->findFile(0, this->filename[1]);

	// Insert the file
	Archive::EntryPtr ep = this->pArchive->insert(epBefore, this->filename[2],
		this->content[2].length(), this->insertType, this->insertAttr);

	// Make sure it went in ok
	BOOST_REQUIRE_MESSAGE(this->pArchive->isValid(ep),
		"Couldn't insert new file in sample archive");

	// Open it
	stream::inout_sptr pfsNew(this->pArchive->open(ep));

	// Apply any encryption/compression filter
	pfsNew = applyFilter(this->pArchive, ep, pfsNew);

	pfsNew->write(this->content[2]);
	pfsNew->flush();

	BOOST_CHECK_MESSAGE(
		// This test checks against the insert_remove result instead, as the end
		// result should be the same as that test.
		this->is_content_equal(this->insert_remove()),
		"Error removing then inserting file"
	);

	// Again, use insert_remove result instead
	CHECK_SUPP_ITEM(FAT, insert_remove, "Error removing then inserting file");
}

void test_archive::test_move()
{
	BOOST_TEST_MESSAGE("Moving file inside archive");

	Archive::EntryPtr ep1 = this->findFile(0);
	Archive::EntryPtr ep2 = this->findFile(1);

	// Swap the file positions
	this->pArchive->move(ep1, ep2);

	BOOST_CHECK_MESSAGE(
		this->is_content_equal(this->move()),
		"Error moving file"
	);

	CHECK_SUPP_ITEM(FAT, move, "Error moving file");
}

void test_archive::test_resize_larger()
{
	BOOST_TEST_MESSAGE("Enlarging a file inside the archive");

	Archive::EntryPtr ep = this->findFile(0);

	this->pArchive->resize(ep, this->content0_largeSize,
		this->content0_largeSize_unfiltered);

	BOOST_CHECK_MESSAGE(
		this->is_content_equal(this->resize_larger()),
		"Error enlarging a file"
	);

	CHECK_SUPP_ITEM(FAT, resize_larger, "Error enlarging a file");
}

void test_archive::test_resize_smaller()
{
	BOOST_TEST_MESSAGE("Shrink a file inside the archive");

	// Find the file we're going to resize
	Archive::EntryPtr ep = this->findFile(0);

	this->pArchive->resize(ep, this->content0_smallSize,
		this->content0_smallSize_unfiltered);

	BOOST_CHECK_MESSAGE(
		this->is_content_equal(this->resize_smaller()),
		"Error shrinking a file"
	);

	CHECK_SUPP_ITEM(FAT, resize_smaller, "Error shrinking a file");
}

void test_archive::test_resize_write()
{
	BOOST_TEST_MESSAGE("Enlarging a file inside the archive");

	// Find the file we're going to resize
	Archive::EntryPtr ep = this->findFile(0);

	// We can't call Archive::resize() because that resizes the storage space,
	// but if there are filters involved the storage space might be quite a
	// different size to the data we want to write, so we have to open the
	// stream and use truncate() instead.
	//this->pArchive->resize(ep, sizeof(CONTENT1_OVERWRITTEN) - 1, sizeof(CONTENT1_OVERWRITTEN) - 1);

	stream::inout_sptr pfsNew(this->pArchive->open(ep));

	// Apply any encryption/compression filter
	pfsNew = applyFilter(this->pArchive, ep, pfsNew);

	// Make sure it's the right size
	BOOST_REQUIRE_EQUAL(pfsNew->size(), this->content[0].length());

	pfsNew->truncate(this->content0_overwritten.length());

	// Make sure it's the right size
	BOOST_REQUIRE_EQUAL(pfsNew->size(), this->content0_overwritten.length());

	pfsNew->seekp(0, stream::start);
	pfsNew->write(this->content0_overwritten);
	pfsNew->flush();

	// Make sure it's the right size
	BOOST_REQUIRE_EQUAL(pfsNew->size(), this->content0_overwritten.length());

	BOOST_CHECK_MESSAGE(
		this->is_content_equal(this->resize_write()),
		"Error enlarging a file then writing into new space"
	);

	CHECK_SUPP_ITEM(FAT, resize_write,
		"Error enlarging a file then writing into new space");

	// Open the file following it to make sure it was moved out of the way
	Archive::EntryPtr ep2 = this->findFile(1);

	// Open it
	camoto::stream::inout_sptr pfsIn(this->pArchive->open(ep2));
	stream::string_sptr out(new stream::string());

	// Apply any decryption/decompression filter
	pfsIn = applyFilter(this->pArchive, ep2, pfsIn);

	// Make sure it's the right size
	BOOST_REQUIRE_EQUAL(pfsIn->size(), this->content[1].length());

	// Copy it into the stringstream
	stream::copy(out, pfsIn);

	BOOST_CHECK_MESSAGE(
		this->is_equal(this->content[1], *(out->str())),
		"Unrelated file was corrupted after file resize operation"
	);
}

// Remove all the files from the archive, then add them back in again.  This
// differs from the insert/remove tests above as it takes the archive to the
// point where it has no files at all.
void test_archive::test_remove_all_re_add()
{
	BOOST_TEST_MESSAGE("Remove all files then add them again");

	Archive::EntryPtr epOne = this->findFile(0);
	this->pArchive->remove(epOne);

	Archive::EntryPtr epTwo = this->findFile(0, this->filename[1]);
	this->pArchive->remove(epTwo);

	// Make sure there are now no files in the archive
	const Archive::VC_ENTRYPTR& allfiles = this->pArchive->getFileList();
	BOOST_REQUIRE_EQUAL(allfiles.size(), 0);

	// Add the files back again
	epOne = this->pArchive->insert(Archive::EntryPtr(), this->filename[0],
		this->content[0].length(), this->insertType, this->insertAttr);

	BOOST_REQUIRE_MESSAGE(this->pArchive->isValid(epOne),
		"Couldn't insert new file after removing all files");

	stream::inout_sptr pfsNew(this->pArchive->open(epOne));

	// Apply any encryption/compression filter
	pfsNew = applyFilter(this->pArchive, epOne, pfsNew);

	pfsNew->write(this->content[0]);
	pfsNew->flush();

	epTwo = this->pArchive->insert(Archive::EntryPtr(), this->filename[1],
		this->content[1].length(), this->insertType, this->insertAttr);

	BOOST_REQUIRE_MESSAGE(this->pArchive->isValid(epTwo),
		"Couldn't insert second new file after removing all files");

	pfsNew = this->pArchive->open(epTwo);

	// Apply any encryption/compression filter
	pfsNew = applyFilter(this->pArchive, epTwo, pfsNew);

	pfsNew->write(this->content[1]);
	pfsNew->flush();

	BOOST_CHECK_MESSAGE(
		this->is_content_equal(this->initialstate()),
		"Error removing all files then reinserting them again"
	);

	CHECK_SUPP_ITEM(FAT, initialstate,
		"Error removing all files then reinserting them again");
}

// The function shifting files can get confused if a zero-length file is
// inserted, incorrectly moving it because of the zero size.
void test_archive::test_insert_zero_then_resize()
{
	BOOST_TEST_MESSAGE("Inserting empty file into archive, then resize it");

	// Insert the file
	Archive::EntryPtr ep = this->pArchive->insert(Archive::EntryPtr(),
		this->filename[2], 0, this->insertType, this->insertAttr);

	// Make sure it went in ok
	BOOST_REQUIRE_MESSAGE(this->pArchive->isValid(ep),
		"Couldn't create new file in sample archive");

	// Open it
	camoto::stream::inout_sptr pfsNew(this->pArchive->open(ep));

	// Apply any encryption/compression filter
	pfsNew = applyFilter(this->pArchive, ep, pfsNew);

	this->pArchive->resize(ep, this->content[2].length(), this->content[2].length());
	pfsNew->seekp(0, stream::start);
	pfsNew->write(this->content[2]);
	pfsNew->flush();

	BOOST_CHECK_MESSAGE(
		this->is_content_equal(this->insert_end()),
		"Error resizing newly inserted empty file"
	);

	CHECK_SUPP_ITEM(FAT, insert_end, "Error resizing newly inserted empty file");
}

void test_archive::test_resize_over64k()
{
	BOOST_TEST_MESSAGE("Enlarging a file to over the 64k limit");

	Archive::EntryPtr ep = this->findFile(0);

	// Do a potentially illegal resize
	try {
		pArchive->resize(ep, 65537, 65537);
	} catch (stream::error) {
		BOOST_CHECK_MESSAGE(
			this->is_content_equal(this->initialstate()),
			"Archive corrupted after failed file resize to over 64k"
		);
		CHECK_SUPP_ITEM(FAT, initialstate,
			"Archive corrupted after failed file resize to over 64k");
	}
}

void test_archive::test_shortext()
{
	BOOST_TEST_MESSAGE("Rename a file with a short extension");

	Archive::EntryPtr ep = this->findFile(0);
	this->pArchive->rename(ep, this->filename_shortext);
	this->pArchive->flush();
	this->pArchive.reset();

	// Reopen the archive
	ManagerPtr pManager(getManager());
	ArchiveTypePtr pTestType(pManager->getArchiveTypeByCode(this->type));
	BOOST_REQUIRE_MESSAGE(pTestType,
		createString("Could not find archive type " << this->type));

	this->pArchive = pTestType->open(this->base, this->suppData);

	// See if we can find the file again
	ep = this->pArchive->find(this->filename_shortext);

	// Make sure we found it
	BOOST_REQUIRE_MESSAGE(this->pArchive->isValid(ep),
		createString("Couldn't find file after rename to "
			<< this->filename_shortext));

	this->pArchive->rename(ep, this->filename[0]);

	BOOST_CHECK_MESSAGE(
		this->is_content_equal(this->initialstate()),
		"Failed to rename file with short extension back to long"
	);
}

//
// Tests on empty archives
//

// Make sure a newly created archive is confirmed as a valid instance of that
// archive format.
void test_archive::test_new_isinstance()
{
	BOOST_TEST_MESSAGE("Checking new archive is valid instance of itself");

	this->pArchive->flush();

	ManagerPtr pManager(getManager());
	ArchiveTypePtr pTestType(pManager->getArchiveTypeByCode(this->type));

	BOOST_REQUIRE_MESSAGE(pTestType->isInstance(this->base),
		"Newly created archive was not recognised as a valid instance");

	BOOST_TEST_CHECKPOINT("New archive reported valid, trying to open");

	// This should really use BOOST_REQUIRE_NO_THROW but the message is more
	// informative without it.
	//BOOST_REQUIRE_NO_THROW(
		boost::shared_ptr<Archive> pArchive(pTestType->open(this->base, suppData));
	//);

	// Make sure there are now no files in the archive
	const Archive::VC_ENTRYPTR& files = pArchive->getFileList();
	BOOST_REQUIRE_EQUAL(files.size(), 0);
}

void test_archive::test_new_to_initialstate()
{
	BOOST_TEST_MESSAGE("Creating archive from scratch");

	if (this->hasMetadata[camoto::Metadata::Version]) {
		// Need to set this first as (in the case of Blood RFF) it affects what type
		// of files we are allowed to insert.
		this->pArchive->setMetadata(camoto::Metadata::Version, this->metadataVer);
	}

	const Archive::VC_ENTRYPTR& files2 = this->pArchive->getFileList();
	BOOST_REQUIRE_EQUAL(files2.size(), 0);

	// Add the files to the new archive
	Archive::EntryPtr epOne = this->pArchive->insert(Archive::EntryPtr(),
		this->filename[0], this->content[0].length(), this->insertType,
		this->insertAttr);

	BOOST_REQUIRE_MESSAGE(this->pArchive->isValid(epOne),
		"Couldn't insert new file in empty archive");

	stream::inout_sptr pfsNew(this->pArchive->open(epOne));
	// Apply any encryption/compression filter
	pfsNew = applyFilter(this->pArchive, epOne, pfsNew);
	pfsNew->write(this->content[0]);
	pfsNew->flush();

	Archive::EntryPtr epTwo = pArchive->insert(Archive::EntryPtr(),
		this->filename[1], this->content[1].length(), this->insertType,
		this->insertAttr);

	BOOST_REQUIRE_MESSAGE(this->pArchive->isValid(epTwo),
		"Couldn't insert second new file in empty archive");
	pfsNew = pArchive->open(epTwo);
	// Apply any encryption/compression filter
	pfsNew = applyFilter(this->pArchive, epTwo, pfsNew);
	pfsNew->write(this->content[1]);
	pfsNew->flush();

	if (this->hasMetadata[camoto::Metadata::Description]) {
		// If this format has metadata, set it to the same value used when comparing
		// against the initialstate, so that this new archive will hopefully match
		// the initialstate itself.
		this->pArchive->setMetadata(camoto::Metadata::Description, this->metadataDesc);
	}

	BOOST_CHECK_MESSAGE(
		this->is_content_equal(this->initialstate()),
		"Error inserting files in new/empty archive"
	);

	// Make sure there are now the correct number of files in the archive
	const Archive::VC_ENTRYPTR& files = this->pArchive->getFileList();
	BOOST_REQUIRE_EQUAL(files.size(), 2);

	CHECK_SUPP_ITEM(FAT, initialstate,
		"Error inserting files in new/empty archive");
}

// The function shifting files can get confused if a zero-length file is
// inserted, incorrectly moving it because of the zero size.
void test_archive::test_new_manipulate_zero_length_files()
{
	BOOST_TEST_MESSAGE("Inserting empty files into archive, then resizing them");

	if (this->hasMetadata[camoto::Metadata::Description]) {
		// If this format has metadata, set it to the same value used when comparing
		// against the initialstate, so that this new archive will hopefully match
		// the initialstate itself.
		this->pArchive->setMetadata(camoto::Metadata::Description, this->metadataDesc);
	}
	if (this->hasMetadata[camoto::Metadata::Version]) {
		this->pArchive->setMetadata(camoto::Metadata::Version, this->metadataVer);
	}

	// Insert the file
	Archive::EntryPtr ep3 = this->pArchive->insert(Archive::EntryPtr(),
		this->filename[2], 0, this->insertType, this->insertAttr);

	// Make sure it went in ok
	BOOST_REQUIRE_MESSAGE(this->pArchive->isValid(ep3),
		"Couldn't create new file in archive");

	// Open it
	camoto::stream::inout_sptr file3(this->pArchive->open(ep3));

	// Apply any encryption/compression filter
	file3 = applyFilter(this->pArchive, ep3, file3);

	Archive::EntryPtr ep1 = pArchive->insert(ep3, this->filename[0], 0,
		this->insertType, this->insertAttr);

	// Make sure it went in ok
	BOOST_REQUIRE_MESSAGE(this->pArchive->isValid(ep1),
		"Couldn't create new file in archive");

	// Open it
	camoto::stream::inout_sptr file1(this->pArchive->open(ep1));

	// Apply any encryption/compression filter
	file1 = applyFilter(this->pArchive, ep1, file1);

	Archive::EntryPtr ep2 = pArchive->insert(ep3, this->filename[1], 0,
		this->insertType, this->insertAttr);

	// Make sure it went in ok
	BOOST_REQUIRE_MESSAGE(this->pArchive->isValid(ep2),
		"Couldn't create new file in archive");

	// Open it
	camoto::stream::inout_sptr file2(this->pArchive->open(ep2));

	// Apply any encryption/compression filter
	file2 = applyFilter(this->pArchive, ep2, file2);

	// Get offsets of each file for later testing
	FATArchive::FATEntryPtr fat1 =
		boost::dynamic_pointer_cast<FATArchive::FATEntry>(ep1);
	FATArchive::FATEntryPtr fat3 =
		boost::dynamic_pointer_cast<FATArchive::FATEntry>(ep3);

	int off1 = fat1->iOffset;
	int off3 = fat3->iOffset;

	// This will resize the second file.  Since all three files are zero-length,
	// they currently all share the same offset.  This should result in file1
	// keeping its original offset (same as file2) and file3's offset being
	// increased.
	file2->truncate(this->content[1].length());
	file2->seekp(0, stream::start);
	file2->write(this->content[1]);
	file2->flush();

	// Make sure the first file hasn't moved
	BOOST_REQUIRE_EQUAL(fat1->iOffset, off1);

	// Make sure the third file has moved.  In theory this could fail if an
	// archive format comes along that can do this correctly without moving the
	// file, but if that ever happens this test can be adjusted then.
	BOOST_REQUIRE_GT(fat3->iOffset, off3);

	file1->truncate(this->content[0].length());
	file1->seekp(0, stream::start);
	file1->write(this->content[0]);
	file1->flush();

	// Make sure the first file hasn't moved
	BOOST_REQUIRE_EQUAL(fat1->iOffset, off1);

	// Make sure the third file has moved again.  Same caveat as above.
	BOOST_REQUIRE_GT(fat3->iOffset, off3);

	file3->truncate(this->content[2].length());
	file3->seekp(0, stream::start);
	file3->write(this->content[2]);
	//file3->write(CONTENT3, sizeof(CONTENT3) - 1);
	file3->flush();

	BOOST_CHECK_MESSAGE(
		this->is_content_equal(this->insert_end()),
		"Error manipulating zero-length files"
	);

	CHECK_SUPP_ITEM(FAT, insert_end, "Error manipulating zero-length files");
}

//
// Metadata tests
//

void test_archive::test_metadata_get_desc()
{
	BOOST_TEST_MESSAGE("Get 'description' metadata field");

	// Make sure this format reports having a 'description' metadata field
	camoto::Metadata::MetadataTypes items = this->pArchive->getMetadataList();
	bool bFound = false;
	for (camoto::Metadata::MetadataTypes::iterator i = items.begin(); i != items.end(); i++) {
		if (*i == camoto::Metadata::Description) {
			bFound = true;
			break;
		}
	}
	BOOST_REQUIRE_EQUAL(bFound, true);

	// Change the field's value
	std::string value = this->pArchive->getMetadata(camoto::Metadata::Description);

	// Make sure we didn't read in extra data (e.g. 400MB with a broken length)
	BOOST_REQUIRE_EQUAL(value.length(), this->metadataDesc.length());

	// Put it in a stringstream to allow use of the standard checking mechanism
	stream::string_sptr out(new stream::string());
	out << value;

	BOOST_CHECK_MESSAGE(
		this->is_equal(this->metadataDesc, *(out->str())),
		"Error getting 'description' metadata field"
	);

}

void test_archive::test_metadata_set_desc_larger()
{
	BOOST_TEST_MESSAGE("Set 'description' metadata field to larger value");

	// We assume the format supports this metadata type, as this is checked in
	// test_metadata_get_desc() above.

	// Change the field's value
	this->pArchive->setMetadata(
		camoto::Metadata::Description, this->metadataDescLarger);

	BOOST_CHECK_MESSAGE(
		this->is_content_equal(this->metadata_set_desc_larger()),
		"Error setting 'description' metadata field"
	);

	CHECK_SUPP_ITEM(FAT, metadata_set_desc_larger,
		"Error setting 'description' metadata field");
}

void test_archive::test_metadata_set_desc_smaller()
{
	BOOST_TEST_MESSAGE("Set 'description' metadata field to smaller value");

	// We assume the format supports this metadata type, as this is checked in
	// get_metadata_description above.

	// Change the field's value
	this->pArchive->setMetadata(
		camoto::Metadata::Description, this->metadataDescSmaller);

	BOOST_CHECK_MESSAGE(
		this->is_content_equal(this->metadata_set_desc_smaller()),
		"Error setting 'description' metadata field"
	);

	CHECK_SUPP_ITEM(FAT, metadata_set_desc_smaller,
		"Error setting 'description' metadata field");
}

void test_archive::test_metadata_get_ver()
{
	BOOST_TEST_MESSAGE("Get 'version' metadata field");

	// Make sure this format reports having a 'version' metadata field
	camoto::Metadata::MetadataTypes items = this->pArchive->getMetadataList();
	bool bFound = false;
	for (camoto::Metadata::MetadataTypes::iterator i = items.begin(); i != items.end(); i++) {
		if (*i == camoto::Metadata::Version) {
			bFound = true;
			break;
		}
	}
	BOOST_REQUIRE_EQUAL(bFound, true);

	// Change the field's value
	std::string value = this->pArchive->getMetadata(camoto::Metadata::Version);

	// Make sure we didn't read in extra data (e.g. 400MB with a broken length)
	BOOST_REQUIRE_EQUAL(value.length(), this->metadataVer.length());

	// Put it in a stringstream to allow use of the standard checking mechanism
	stream::string_sptr out(new stream::string());
	out << value;

	BOOST_CHECK_MESSAGE(
		this->is_equal(this->metadataVer, *(out->str())),
		"Error getting 'version' metadata field"
	);
}


std::string test_archive::metadata_set_desc_larger()
{
	throw new test_metadata_not_supported;
}

std::string test_archive::metadata_set_desc_smaller()
{
	throw new test_metadata_not_supported;
}

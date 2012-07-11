#include <gtest/gtest.h>

#include <rpg2k/database.hxx>
#include <rpg2k/map_tree.hxx>
#include <rpg2k/map_unit.hxx>
#include <rpg2k/model.hxx>
#include <rpg2k/project.hxx>
#include <rpg2k/save_data.hxx>

#include <boost/scoped_ptr.hpp>

#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

using namespace rpg2k::model;


bool compare_file(rpg2k::system_string const& l, rpg2k::system_string const& r)
{
	std::ifstream lstream(l.c_str()), rstream(r.c_str());
	if(lstream || rstream) return false;

	std::istream_iterator<char> lIt(lstream), rIt(rstream), end;

	std::mismatch(lIt, end, rIt);
	return(lIt == end);
}

TEST(Database, OpenAndClose)
{
	char const* const LDB_TEST_FILE = "./RPG_RT.ldb.test";
	boost::scoped_ptr<database> ldb;

	ldb.reset(new database("./TestGame"));
	ldb->save_as(LDB_TEST_FILE);

	ldb.reset(new database(LDB_TEST_FILE));
	ldb.reset();

	ASSERT_TRUE(compare_file("./TestGame/RPG_RT.ldb", LDB_TEST_FILE));

	std::remove(LDB_TEST_FILE);
}

TEST(MapUnit, OpenAndClose)
{
	char const* const LMU_TEST_FILE = "./Map0001.lmu.test";
	boost::scoped_ptr<map_unit> lmu;

	lmu.reset(new map_unit("./TestGame", 1));
	lmu->save_as(LMU_TEST_FILE);

	lmu.reset(new map_unit(LMU_TEST_FILE));
	lmu.reset();

	ASSERT_TRUE(compare_file("./TestGame/Map0001.lmu", LMU_TEST_FILE));

	std::remove(LMU_TEST_FILE);
}

TEST(MapTree, OpenAndClose)
{
	char const* const LMT_TEST_FILE = "./RPG_RT.lmt.test";
	boost::scoped_ptr<map_tree> lmt;

	lmt.reset(new map_tree("./TestGame"));
	lmt->save_as(LMT_TEST_FILE);

	lmt.reset(new map_tree(LMT_TEST_FILE));
	lmt.reset();

	ASSERT_TRUE(compare_file("./TestGame/RPG_RT.lmt", LMT_TEST_FILE));

	std::remove(LMT_TEST_FILE);
}

/*
TEST(SaveData, OpenAndClose)
{
	char const* const LSD_TEST_FILE = "./Save01.lsd.test";
	boost::scoped_ptr<save_data> lsd;

	lsd.reset(new SaveData("./TestGame", 1));
	lsd->save_as(LSD_TEST_FILE);

	lsd.reset(new save_data("./", "Save01.lsd.test"));
	lsd.reset();

	ASSERT_TRUE(compare_file("./TestGame/Save01.lsd", LSD_TEST_FILE));

	std::remove(LSD_TEST_FILE);
}
*/

TEST(DefineLoader, CheckDefines)
{
	define_loader& def_lod = define_loader::instance();

#define PP_enum(r, data, elem) def_lod.get(BOOST_PP_STRINGIZE(elem));
	BOOST_PP_SEQ_FOR_EACH(PP_enum, ,
		(LcfDataBase)(LcfMapUnit)(LcfMapUnit)(LcfSaveData)
		(event_state)(music)(sound))
#undef PP_elem
}

TEST(Project, OpenAndClose)
{
	project("./TestGame");
}

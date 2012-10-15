#include <gtest/gtest.h>

#include <rpg2k/database.hxx>
#include <rpg2k/map_tree.hxx>
#include <rpg2k/map_unit.hxx>
#include <rpg2k/model.hxx>
#include <rpg2k/project.hxx>
#include <rpg2k/save_data.hxx>

#include <boost/filesystem.hpp>
#include <boost/scoped_ptr.hpp>

#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

namespace model = rpg2k::model;

/*
TEST(Project, OpenAndClose)
{
	model::project("./test/TestGame");
}
*/

TEST(Database, OpenAndClose)
{
	char const* const LDB_TEST_FILE = "./test/RPG_RT.ldb.test";
	boost::scoped_ptr<model::database> ldb;

	ldb.reset(new model::database("./test/TestGame"));
	ldb->save_as(LDB_TEST_FILE);

	ldb.reset(new model::database(LDB_TEST_FILE));
	ldb.reset();

	std::remove(LDB_TEST_FILE);
}

TEST(MapUnit, OpenAndClose)
{
	char const* const LMU_TEST_FILE = "./test/Map0001.lmu.test";
	boost::scoped_ptr<model::map_unit> lmu;

	lmu.reset(new model::map_unit("./test/TestGame", 1));
	lmu->save_as(LMU_TEST_FILE);

	lmu.reset(new model::map_unit(LMU_TEST_FILE));
	lmu.reset();

	std::remove(LMU_TEST_FILE);
}

TEST(MapTree, OpenAndClose)
{
	char const* const LMT_TEST_FILE = "./test/RPG_RT.lmt.test";
	boost::scoped_ptr<model::map_tree> lmt;

	lmt.reset(new model::map_tree("./test/TestGame"));
	lmt->save_as(LMT_TEST_FILE);

	lmt.reset(new model::map_tree(LMT_TEST_FILE));
	lmt.reset();

	std::remove(LMT_TEST_FILE);
}

/*
TEST(SaveData, OpenAndClose)
{
	char const* const LSD_TEST_FILE = "./test/Save01.lsd.test";
	boost::scoped_ptr<model::save_data> lsd;

	lsd.reset(new model::save_data("./test/TestGame", 1));
	lsd->save_as(LSD_TEST_FILE);

	lsd.reset(new model::save_data("./test/", "Save01.lsd.test"));
	lsd.reset();

	std::remove(LSD_TEST_FILE);
}
*/

TEST(DefineLoader, CheckDefines)
{
	model::define_loader& def_lod = model::define_loader::instance();

#define PP_enum(r, data, elem) def_lod.get(BOOST_PP_STRINGIZE(elem));
	BOOST_PP_SEQ_FOR_EACH(PP_enum, ,
		(LcfDataBase)(LcfMapUnit)(LcfMapUnit)(LcfSaveData)
		(event_state)(music)(sound))
#undef PP_elem
}

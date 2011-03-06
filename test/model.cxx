#include <gtest/gtest.h>

#include <rpg2k/DataBase.hxx>
#include <rpg2k/MapTree.hxx>
#include <rpg2k/Model.hxx>
#include <rpg2k/Project.hxx>

#include <boost/scoped_ptr.hpp>

#include <cassert>
#include <cstdlib>
#include <string>

using namespace rpg2k::model;


TEST(MapTree, OpenAndClose)
{
	char const* const LMT_TEST_FILE = "./test/RPG_RT.lmt.test";
	boost::scoped_ptr<MapTree> lmt;

	lmt.reset(new MapTree("./test/TestGame"));
	lmt->saveAs(LMT_TEST_FILE);

	lmt.reset(new MapTree("./test", "RPG_RT.lmt.test"));
	lmt.reset();

	std::remove(LMT_TEST_FILE);
}
/*
TEST(DataBase, OpenAndClose)
{
	DataBase("./test/TestGame");
}
*/

TEST(DefineLoader, CheckDefines)
{
	DefineLoader& defLod = DefineLoader::instance();

	#define PP_test(arg) defLod.get(#arg)
	PP_test(EventState);
	PP_test(LcfDataBase);
	PP_test(LcfMapTree);
	PP_test(LcfMapUnit);
	PP_test(LcfSaveData);
	PP_test(Music);
	PP_test(Sound);
	#undef PP_test
}

TEST(Project, OpenAndClose)
{
	Project("./test/TestGame");
}

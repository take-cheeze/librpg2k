#include <gtest/gtest.h>

#include <rpg2k/DataBase.hxx>
#include <rpg2k/MapTree.hxx>
#include <rpg2k/MapUnit.hxx>
#include <rpg2k/Model.hxx>
#include <rpg2k/Project.hxx>
#include <rpg2k/SaveData.hxx>

#include <boost/scoped_ptr.hpp>

#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

using namespace rpg2k::model;


bool compareFile(rpg2k::SystemString const& l, rpg2k::SystemString const& r)
{
	std::ifstream lstream(l.c_str()), rstream(r.c_str());
	if(lstream || rstream) return false;

	std::istream_iterator<char> lIt(lstream), rIt(rstream), end;

	std::mismatch(lIt, end, rIt);
	return(lIt == end);
}

TEST(MapUnit, OpenAndClose)
{
	char const* const LMU_TEST_FILE = "./Map0001.lmu.test";
	boost::scoped_ptr<MapUnit> lmu;

	lmu.reset(new MapUnit("./TestGame", 1));
	lmu->saveAs(LMU_TEST_FILE);

	lmu.reset(new MapUnit(".", "Map0001.lmu.test"));
	lmu.reset();

	ASSERT_TRUE(compareFile("./TestGame/Map0001.lmu", LMU_TEST_FILE));

	std::remove(LMU_TEST_FILE);
}
TEST(MapTree, OpenAndClose)
{
	char const* const LMT_TEST_FILE = "./RPG_RT.lmt.test";
	boost::scoped_ptr<MapTree> lmt;

	lmt.reset(new MapTree("./TestGame"));
	lmt->saveAs(LMT_TEST_FILE);

	lmt.reset(new MapTree(".", "RPG_RT.lmt.test"));
	lmt.reset();

	ASSERT_TRUE(compareFile("./TestGame/RPG_RT.lmt", LMT_TEST_FILE));

	std::remove(LMT_TEST_FILE);
}
/*
TEST(SaveData, OpenAndClose)
{
	char const* const LSD_TEST_FILE = "./Save01.lsd.test";
	boost::scoped_ptr<SaveData> lsd;

	lsd.reset(new SaveData("./TestGame", 1));
	lsd->saveAs(LSD_TEST_FILE);

	lsd.reset(new SaveData("./", "Save01.lsd.test"));
	lsd.reset();

	ASSERT_TRUE(compareFile("./TestGame/Save01.lsd", LSD_TEST_FILE));

	std::remove(LSD_TEST_FILE);
}
*/
/*
TEST(DataBase, OpenAndClose)
{
	DataBase("./TestGame");
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

/*
TEST(Project, OpenAndClose)
{
	Project("./TestGame");
}
*/

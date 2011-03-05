#include <gtest/gtest.h>

#include <rpg2k/DataBase.hxx>
#include <rpg2k/MapTree.hxx>
#include <rpg2k/Model.hxx>
#include <rpg2k/Project.hxx>

#include <cassert>
#include <cstdlib>
#include <string>

using namespace rpg2k::model;


TEST(MapTree, OpenAndClose)
{
	MapTree("./test/TestGame");
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

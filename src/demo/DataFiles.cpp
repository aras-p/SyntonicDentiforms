#include "DataFiles.h"

static const char* kTexturePaths[DataTexCOUNT] = {
	"data/tex/AlphaEdge.png",
	"data/tex/BondigoDuo.png",
	"data/tex/ColorLuts.png",
	"data/tex/CreditsA.png",
	"data/tex/CreditsB.png",
	"data/tex/DentATrepasa.png",
	"data/tex/DentBPokero.png",
	"data/tex/DentCCorenjo.png",
	"data/tex/DentDSmogenias.png",
	"data/tex/DentEChanChan.png",
	"data/tex/DentFPerto.png",
	"data/tex/DentGZetal.png",
	"data/tex/DentHLaVina.png",
	"data/tex/DentICarina.png",
	"data/tex/Greetings.png",
	"data/tex/Line1.png",
	"data/tex/SpotLight.png",
};

sokol_texture* g_data_tex[DataTexCOUNT];

static const char* kMeshPaths[DataMeshCOUNT] = {
	"data/mesh/Box.dmesh",
	"data/mesh/Cube.dmesh",
	"data/mesh/CubeNX.dmesh",
	"data/mesh/CubeNY.dmesh",
	"data/mesh/CubeNZ.dmesh",
	"data/mesh/CubePX.dmesh",
	"data/mesh/CubePY.dmesh",
	"data/mesh/CubePZ.dmesh",
	"data/mesh/scene1/GearA.dmesh",
	"data/mesh/scene1/GearAAxis.dmesh",
	"data/mesh/scene1/GearB.dmesh",
	"data/mesh/scene1/GearBCAxis.dmesh",
	"data/mesh/scene1/GearC.dmesh",
	"data/mesh/scene2/Axis.dmesh",
	"data/mesh/scene2/GearA.dmesh",
	"data/mesh/scene2/GearB.dmesh",
	"data/mesh/scene2/GearC.dmesh",
	"data/mesh/scene2/Inner.dmesh",
	"data/mesh/scene2/InnerEnd.dmesh",
	"data/mesh/scene2/Outer.dmesh",
	"data/mesh/scene3/Axis.dmesh",
	"data/mesh/scene3/AxisC.dmesh",
	"data/mesh/scene3/AxisD.dmesh",
	"data/mesh/scene3/GearA.dmesh",
	"data/mesh/scene3/GearB.dmesh",
	"data/mesh/scene3/GearC.dmesh",
	"data/mesh/scene3/GearD.dmesh",
	"data/mesh/scene3/HolderA.dmesh",
	"data/mesh/scene3/HolderB.dmesh",
	"data/mesh/scene3/Prop.dmesh",
	"data/mesh/scene4/AxisV1.dmesh",
	"data/mesh/scene4/CenterA.dmesh",
	"data/mesh/scene4/CenterCyl.dmesh",
	"data/mesh/scene4/Cyl.dmesh",
	"data/mesh/scene4/GearA.dmesh",
	"data/mesh/scene4/GearTop.dmesh",
	"data/mesh/scene5/CylB.dmesh",
	"data/mesh/scene5/CylC.dmesh",
	"data/mesh/scene5/GearA.dmesh",
	"data/mesh/scene5/GearB.dmesh",
	"data/mesh/scene5/GearC.dmesh",
	"data/mesh/scene5/Tri.dmesh",
	"data/mesh/scene5/TubeA.dmesh",
	"data/mesh/scene6/Axis1.dmesh",
	"data/mesh/scene6/Axis2.dmesh",
	"data/mesh/scene6/Gear1.dmesh",
	"data/mesh/scene6/Gear2.dmesh",
	"data/mesh/scene6/Tooth1.dmesh",
	"data/mesh/scene6/Tooth2.dmesh",
};

CMesh* g_data_mesh[DataMeshCOUNT];

static const char* kAnimPaths[DataAnimCOUNT] = {
    "data/anim/Anim0.danim",
    "data/anim/Anim1.danim",
    "data/anim/Anim2.danim",
    "data/anim/Anim3.danim",
    "data/anim/Anim4.danim",
    "data/anim/Anim5.danim",
    "data/anim/Anim6.danim",
    "data/anim/Synch.danim",
    "data/anim/6/Axes.danim",
    "data/anim/6/TeethA.danim",
    "data/anim/6/TeethB.danim",
    "data/anim/6/TeethC.danim",
    "data/anim/6/TeethD.danim",
};

CAnimationBunch* g_data_anim[DataAnimCOUNT];

bool load_data_files()
{
	for (int i = 0; i < DataTexCOUNT; ++i)
	{
		g_data_tex[i] = load_texture(kTexturePaths[i]);
		if (g_data_tex[i] == nullptr)
		{
			return false;
		}
	}
	for (int i = 0; i < DataMeshCOUNT; ++i)
	{
		g_data_mesh[i] = load_mesh(kMeshPaths[i]);
		if (g_data_mesh[i] == nullptr)
		{
			return false;
		}
	}
    for (int i = 0; i < DataAnimCOUNT; ++i)
    {
        g_data_anim[i] = load_animation(kAnimPaths[i]);
        if (g_data_anim[i] == nullptr)
        {
            return false;
        }
    }
	return true;
}


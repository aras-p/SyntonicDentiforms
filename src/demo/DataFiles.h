#pragma once

#include "src/animator/AnimationBunch.h"
#include "src/gfx/Mesh.h"
#include "src/gfx/Texture.h"

enum DataTexture {
	DataTexAlphaEdge,
	DataTexBondigoDuo,
	DataTexColorLuts,
	DataTexCreditsA,
	DataTexCreditsB,
	DataTexDentATrepasa,
	DataTexDentBPokero,
	DataTexDentCCorenjo,
	DataTexDentDSmogenias,
	DataTexDentEChanChan,
	DataTexDentFPerto,
	DataTexDentGZetal,
	DataTexDentHLaVina,
	DataTexDentICarina,
	DataTexGreetings,
	DataTexLine1,
	DataTexSpotLight,

	DataTexCOUNT
};

enum DataMesh {
	DataMeshBox,
	DataMeshCube,
	DataMeshCubeNX,
	DataMeshCubeNY,
	DataMeshCubeNZ,
	DataMeshCubePX,
	DataMeshCubePY,
	DataMeshCubePZ,
	DataMesh1GearA,
	DataMesh1GearAAxis,
	DataMesh1GearB,
	DataMesh1GearBCAxis,
	DataMesh1GearC,
	DataMesh2Axis,
	DataMesh2GearA,
	DataMesh2GearB,
	DataMesh2GearC,
	DataMesh2Inner,
	DataMesh2InnerEnd,
	DataMesh2Outer,
	DataMesh3Axis,
	DataMesh3AxisC,
	DataMesh3AxisD,
	DataMesh3GearA,
	DataMesh3GearB,
	DataMesh3GearC,
	DataMesh3GearD,
	DataMesh3HolderA,
	DataMesh3HolderB,
	DataMesh3Prop,
	DataMesh4AxisV1,
	DataMesh4CenterA,
	DataMesh4CenterCyl,
	DataMesh4Cyl,
	DataMesh4GearA,
	DataMesh4GearTop,
	DataMesh5CylB,
	DataMesh5CylC,
	DataMesh5GearA,
	DataMesh5GearB,
	DataMesh5GearC,
	DataMesh5Tri,
	DataMesh5TubeA,
	DataMesh6Axis1,
	DataMesh6Axis2,
	DataMesh6Gear1,
	DataMesh6Gear2,
	DataMesh6Tooth1,
	DataMesh6Tooth2,

	DataMeshCOUNT
};

enum DataAnim {
    DataAnimAnim0,
    DataAnimAnim1,
    DataAnimAnim2,
    DataAnimAnim3,
    DataAnimAnim4,
    DataAnimAnim5,
    DataAnimAnim6,
    DataAnimSynch,
    DataAnim6Axes,
    DataAnim6TeethA,
    DataAnim6TeethB,
    DataAnim6TeethC,
    DataAnim6TeethD,
    
    DataAnimCOUNT
};

extern sokol_texture* g_data_tex[DataTexCOUNT];
extern CMesh* g_data_mesh[DataMeshCOUNT];
extern CAnimationBunch* g_data_anim[DataAnimCOUNT];

bool load_data_files();

DataMesh find_mesh_by_name(int scene, const char* name);

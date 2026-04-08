#pragma once

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

extern sokol_texture* g_data_tex[DataTexCOUNT];

bool load_data_files();

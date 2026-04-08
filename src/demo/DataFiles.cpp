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
	return true;
}

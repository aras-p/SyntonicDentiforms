#pragma once

enum Effect {
	fx_blit,
	fx_billboards,
	fx_billboardsNoDestAlpha,
	fx_caster,
	fx_casterNoZ,
	fx_compositeAdd,
	fx_compositeAlpha,
	fx_filterBloom,
	fx_filterToon,
	fx_lines,
	fx_linesNoAa,
	fx_noshadowHi,
	fx_overlay2,
	fx_overlay,
	fx_receiverHi,
	fx_receiverLo,
	fx_reflective,

	fxCount
};

void effects_init();
void effect_apply(Effect fx);

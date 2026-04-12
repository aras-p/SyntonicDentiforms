#pragma once

enum Pipeline {
	pip_blit,
	pip_blitToSwap,
	pip_overlay1,
	pip_overlay2,

	pip_postComposeBloom,
	pip_postComposeToon,
	pip_postToon,
	pip_postBlurStep,

	pip_billboards,
	pip_billboardsClearDestAlpha,
	pip_lines,
	pip_linesNoAA,

	pip_renderWhite,
	pip_renderLit,
	pip_renderLitShadowed,
	pip_renderLitShadowedFlip,
	pip_renderReflective,
	pip_shadowCaster,

	pipCount
};

void pipelines_init();
void pipeline_apply(Pipeline fx);

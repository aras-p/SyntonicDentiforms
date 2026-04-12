#pragma once

// Animation curve information.
// This just has first value index (in array of values shared between several curves),
// interpolation type and a collapsed value (if curve is collapsed into single value).
struct SAnimCurve
{
	enum eIpol { NONE=0, STEP, LINEAR };

	bool isCollapsed() const { return ipol == NONE; }

	int			firstSampleIndex = 0;
	eIpol		ipol = NONE;
	float		collapsedValue[4] = { 0,0,0,0 };
};

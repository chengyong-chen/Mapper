#pragma once

namespace Scale
{
	static float pointsPerMeter = (1*100/2.54)*72.f;	//convert 1 meter to points
	static float PrecisionFromScale(unsigned int scale)
	{
		return 10*scale/ pointsPerMeter;					// the sacle in point , one point means how many meters in the real wold
	}
}

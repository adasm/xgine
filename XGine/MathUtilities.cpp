#include "MathUtilities.h"

void buildBoundingBoxLines(Vec3 *min, Vec3 *max, Vec3 *positions)
{
	if(min && max && positions)
	{
		Vec3 bbmin = *min, bbmax = *max;
		positions[0]  = bbmin;
		positions[1]  = Vec3(bbmin.x, bbmin.y, bbmax.z);
		positions[2]  = bbmin;
		positions[3]  = Vec3(bbmin.x, bbmax.y, bbmin.z);
		positions[4]  = bbmin;
		positions[5]  = Vec3(bbmax.x, bbmin.y, bbmin.z);
		positions[6]  = bbmax;
		positions[7]  = Vec3(bbmax.x, bbmax.y, bbmin.z);
		positions[8]  = bbmax;
		positions[9]  = Vec3(bbmax.x, bbmin.y, bbmax.z);
		positions[10] = bbmax;
		positions[11] = Vec3(bbmin.x, bbmax.y, bbmax.z);
		positions[12] = Vec3(bbmin.x, bbmax.y, bbmin.z);
		positions[13] = Vec3(bbmin.x, bbmax.y, bbmax.z);
		positions[14] = Vec3(bbmin.x, bbmax.y, bbmin.z);
		positions[15] = Vec3(bbmin.x, bbmax.y, bbmax.z);
		positions[16] = Vec3(bbmin.x, bbmax.y, bbmax.z);
		positions[17] = Vec3(bbmin.x, bbmin.y, bbmax.z);
		positions[18] = Vec3(bbmin.x, bbmax.y, bbmax.z);
		positions[19] = Vec3(bbmin.x, bbmin.y, bbmax.z);
		positions[20] = Vec3(bbmin.x, bbmax.y, bbmin.z);
		positions[21] = Vec3(bbmax.x, bbmax.y, bbmin.z);
		positions[22] = Vec3(bbmax.x, bbmax.y, bbmin.z);
		positions[23] = Vec3(bbmax.x, bbmin.y, bbmin.z);
		positions[24] = Vec3(bbmax.x, bbmin.y, bbmin.z);
		positions[25] = Vec3(bbmax.x, bbmin.y, bbmax.z);
		positions[26] = Vec3(bbmin.x, bbmin.y, bbmax.z);
		positions[27] = Vec3(bbmax.x, bbmin.y, bbmax.z);
	}
}
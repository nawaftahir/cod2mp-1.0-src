#pragma once

typedef float vec3_t[3];
typedef float vec4_t[4];

struct FxCamera
{
	vec3_t vieworg;
	vec4_t frustum[6];
	int numPlanes;
};

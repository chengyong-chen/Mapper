#pragma once
#include "tile.h"

class CGGLTile : public CTile
{
public:
	CGGLTile();
	~CGGLTile();


public:
	CALLBACKGLUEVAL(CGGLTile,Loaded,int)
	
};

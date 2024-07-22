// Grid.h: interface for the CEditFlexGrid class.

//////////////////////////////////////////////////////////////////////

#pragma once

#include "msflexgrid.h"

class CEditFlexGrid : public CMSFlexGrid
{
public:
	CEditFlexGrid();

	~CEditFlexGrid() override;

public:

	BOOL GoRight();
	BOOL GoLeft();
	BOOL GoDown();
	BOOL GoUp();
	BOOL OnKeyPressGrid(short FAR* KeyAscii);
	void OnClickGrid();
	void OnEnterCell();
private:
	CString m_GridData;
	BOOL bEditMode;
	BOOL TransferValue(BOOL ToGrid);
};

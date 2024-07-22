// m_Grid->cpp: implementation of the CEditFlexGrid class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MsFlexGrid.h"
#include "EditFlexGrid.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEditFlexGrid::CEditFlexGrid()
{
}

CEditFlexGrid::~CEditFlexGrid()
{
}

BOOL CEditFlexGrid::GoUp()
{
	TransferValue(TRUE);
	if(GetRow()>1)
	{
		int CurrentRow = GetRow();
		CurrentRow--;
		SetRow(CurrentRow);
		TransferValue(FALSE);
		if(!GetRowIsVisible(CurrentRow))
			SetTopRow(CurrentRow);
	}

	return TRUE;
}

BOOL CEditFlexGrid::GoDown()
{
	TransferValue(TRUE);
	if(GetRow()<GetRows()-1)
	{
		int CurrentRow = GetRow();
		CurrentRow++;
		SetRow(CurrentRow);
		TransferValue(FALSE);
		if(!GetRowIsVisible(CurrentRow))
			SetTopRow(CurrentRow);
	}

	return TRUE;
}

BOOL CEditFlexGrid::GoLeft()
{
	TransferValue(TRUE);
	if(GetCol()>1)
	{
		int CurrentCol = GetCol();
		CurrentCol--;
		SetCol(CurrentCol);
		TransferValue(FALSE);
		if(!GetColIsVisible(CurrentCol))
			SetLeftCol(CurrentCol);
	}

	return TRUE;
}

BOOL CEditFlexGrid::GoRight()
{
	TransferValue(TRUE);
	if(GetCol()<GetCols()-1)
	{
		int CurrentCol = GetCol();
		CurrentCol++;
		SetCol(CurrentCol);
		TransferValue(FALSE);
		if(!GetColIsVisible(CurrentCol))
			SetLeftCol(CurrentCol);
	}

	return TRUE;
}

BOOL CEditFlexGrid::TransferValue(BOOL ToGrid)
{
	CString sText;

	if(ToGrid)
	{
		//Transfer value to Grid from Edit Box
		SetText(m_GridData);
	}
	else
	{
		m_GridData = GetText();
		SetBackColorSel(RGB(0, 255, 255));
	}
	return TRUE;
}

BOOL CEditFlexGrid::OnKeyPressGrid(short FAR* KeyAscii)
{
	switch((int)*KeyAscii)
	{
	case VK_BACK: //Backspace - delete the last character				
		m_GridData.Delete(m_GridData.GetLength()-1, 1);
		break;
	default:
		m_GridData = m_GridData+(TCHAR*)KeyAscii;
		break;
	}

	TransferValue(TRUE);
	return TRUE;
}

void CEditFlexGrid::OnClickGrid()
{
	TransferValue(FALSE);
}

void CEditFlexGrid::OnEnterCell()
{
	TransferValue(FALSE);
}

#include "stdafx.h"

#include "ExportTxt.h"

#include <cstdio>
#include "../Layer/LayerTree.h"
#include "../Style/Hint.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL ExportTxt(LPCTSTR pathname, CLayerTree& layertree, const Gdiplus::Matrix& matrix)
{
	FILE* file = _tfopen(pathname, _T("wt"));
	if(file==nullptr)
	{
		AfxMessageBox(_T("Cann't create a writeable Txt file!"));
		return FALSE;
	}

	for(CTree<CLayer>::postiterator it = layertree.postbegin(); it!=layertree.postend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_bVisible==false)
			continue;

		layer->ExportPlaintext(file, matrix);
	}

	fclose(file);
	return TRUE;
}

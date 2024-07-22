#include "stdafx.h"

#include "SymbolDoc.h"
#include "SymbolView.h"
#include "TabletView.h"

#include "PoleDlg.h"

#include "../Tool/Global.h"
#include "../Tool/Tool.h"
#include "../Tool/ZoomTool.h"
#include "../Geometry/Global.h"
#include "../Geometry/Pole.h"

extern UNIT   a_UnitArray[3];
extern BYTE   a_nUnitIndex;

extern __declspec(dllimport) BYTE d_nUnitIndex;
extern __declspec(dllimport) CZoomTool zoominTool;


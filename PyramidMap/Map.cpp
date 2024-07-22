#include "StdAfx.h"
#include "Map.h"

CMap::CMap()
{
	m_bad=false;
	m_zoom=0;
	m_mapwidths.Alloc(20);
	m_mapwidths.SetGrow(true);
	m_mapheights.Alloc(20);
	m_mapheights.SetGrow(true);
}

CMap::~CMap(void)
{
}

void CMap::SetSize(int level,int width,int height) 
{
	m_mapwidths.SetEntry(level,width);
	m_mapheights.SetEntry(level,height);
}

void CMap::GenerateURL(kGUIString *hstr,int *serverid,int tx,int ty,int zoom);
{
}

void CMap::DrawTile(unsigned int row,unsigned int col)
{
	int drawn = 0;
	CTile tile = nullptr;
	if(title->IsCached()==true)
	{
		tile->GetCach();
		if(title->m_data == nullptr)
		{
			drawn = TILE_ERROR;
		}
		else
		{
			tile->Draw();
			drawn = TILE_OK;
		}
	}
	else
	{
		tile->Load(&gurl,&fn,row,col,serverid)==false)
			drawn = TILE_WAITING;
		else
			drawn = TILE_LOADING;
	}

	switch(drawn)
	{
	case TILE_OK:
		{
		}
		break;
	case TILE_WAITING:
		{
			tile->Dirty();
			kGUI::DrawRect(0,0,m_grid.GetCellWidth(),m_grid.GetCellHeight(),(cx+cy)&1?DrawColor(255,255,255):DrawColor(32,32,255));
			m_shapes[SHAPE_LOADINGIMAGE].ScaleTo(m_grid.GetCellWidth()/3,m_grid.GetCellHeight()/3);
			m_shapes[SHAPE_LOADINGIMAGE].Draw(0,0,0);
		}
		break;
	case TILE_LOADING:
		{
			kGUI::DrawRect(0,0,m_grid.GetCellWidth(),m_grid.GetCellHeight(),(cx+cy)&1?DrawColor(255,255,255):DrawColor(32,32,255));
			m_shapes[SHAPE_LOADINGIMAGE].ScaleTo(m_grid.GetCellWidth()/3,m_grid.GetCellHeight()/3);
			m_shapes[SHAPE_LOADINGIMAGE].Draw(0,0,0);
		}
		break;
	case TILE_NOTEXIST:
		{
			kGUI::DrawRect(0,0,m_grid.GetCellWidth(),m_grid.GetCellHeight(),(cx+cy)&1?DrawColor(255,255,255):DrawColor(32,32,255));
			m_shapes[SHAPE_BROKENIMAGE].ScaleTo(m_grid.GetCellWidth()/3,m_grid.GetCellHeight()/3);
			m_shapes[SHAPE_BROKENIMAGE].Draw(0,0,0);
		}
		break;
	case TILE_ERROR:
		{
			kGUI::DrawRect(0,0,m_grid.GetCellWidth(),m_grid.GetCellHeight(),(cx+cy)&1?DrawColor(255,255,255):DrawColor(32,32,255));
			m_shapes[SHAPE_BROKENIMAGE].ScaleTo(m_grid.GetCellWidth()/3,m_grid.GetCellHeight()/3);
			m_shapes[SHAPE_BROKENIMAGE].Draw(0,0,0);
		}
		break;
	}
}



for(int row=top;row<bottom;row++)
for(int col=left;col<right;col++)
{
	CTile* tile= nullptr;

	if(title->IsDirty()==true)
	{
		cobj->SetDirty(false);
		cobj->Update(cx,cy);
	}
	tile->Draw();
}
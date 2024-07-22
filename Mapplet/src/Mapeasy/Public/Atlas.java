package Mapeasy.Public;

import java.awt.geom.Point2D;
import java.util.Vector;
import Mapeasy.Obj.Obj;

import Mapeasy.Style.Library;

/**
 * <p>Title: </p>
 *
 * <p>Description: </p>
 *
 * <p>Copyright: Copyright (c) 2006</p>
 *
 * <p>Company: </p>
 *
 * @author not attributable
 * @version 1.0
 */
public class Atlas
{
    public String m_strName;
    public String m_strTitle;
    public String m_strIndexHtml;
    public String m_strHomePage;

    public boolean m_bBroadcast;
    public boolean m_bDatabase;
    public boolean m_bFree;
    public boolean m_bEdit;
    public boolean m_bRegionQuery;
    public boolean m_bNavigate;
    public boolean m_bTraffic;
    public boolean m_bHtml;

    // public MyDatabase m_databaseLAB;
    // public MyDatabase m_databaseOBJ;
    // public MyDatabase m_databasePOI;

    public Vector<AtlasItem> m_atlasItemList = new Vector<AtlasItem>();

    public String m_strFilePath;

    public boolean Open(String strFile, int dwDBOptions)
    {
        return false;
    }

    public AtlasItem GetItem(short dwMapID)
    {
        return null;
    }

    public AtlasItem GetItemByMapFile(String strMapFile)
    {
        return null;
    }

    public AtlasItem GetItemByMapName(String strMapName)
    {
        return null;
    }

    public AtlasItem GetParentMap(short dwMapID)
    {
        return null;
    }

    public AtlasItem GetSubItem(short wMapID, Point2D geoPoint)
    {
        return null;
    }

    public AtlasItem MatchNavyMap(Point2D geoPoint1, Point2D geoPoint2)
    {
        return null;
    }


    public void Serialize()
    {
    }

    public Vector<POI> m_POIlist = new Vector<POI>();
    public Library m_library;

    public Atlas()
    {
    }
}

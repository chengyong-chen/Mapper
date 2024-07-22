package Mapeasy.Public;

import java.awt.Point;
import java.awt.Point;
import java.awt.geom.*;
import java.io.DataInputStream;
import java.io.IOException;

public class MapInfo
{
    public float   m_fScale;

    public String m_strName = new String();
    public float  m_fVersion;
    public Point2D.Double m_Origin = new Point2D.Double();


    public MapInfo()
    {
    }

    public void Serialize(DataInputStream dataStream)
    {
        try
        {
            m_fScale   = dataStream.readFloat();
            m_strName  = dataStream.readUTF();
            m_fVersion = dataStream.readFloat();
            m_Origin.x = dataStream.readDouble();
            m_Origin.y = dataStream.readDouble();
        }
        catch (IOException ioe)
       {
           System.out.println("error when read MapInfo parameter!");
       }
    }

    public Point2D.Double DocToMap(Point point,long nDecimal)
    {

        Point2D.Double mapPoint = new Point2D.Double(0,0);
        return mapPoint;
    }

    public Point MapToDoc(Point2D.Double mapPoint,long nDecimal)
    {
        Point docPoint = new Point(0,0);
        return docPoint;
    }
}

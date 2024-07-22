package Mapeasy.Style;

import java.awt.Graphics2D;
import java.awt.Point;
import java.awt.Rectangle;
import java.io.IOException;
import java.io.DataInputStream;

public class SpotSymbol extends Spot
{
    public short m_wID;
    public short m_nScale;
    public short m_nAngle;

    public SpotSymbol()
    {
    }

    public void Serialize(DataInputStream dataStream)
    {
        super.Serialize(dataStream);

        try
        {
            m_wID = dataStream.readShort();
            m_nScale = dataStream.readShort();
            m_nAngle = dataStream.readShort();
        }
        catch (IOException ioe)
        {
            System.out.println("error when read SpotSymbol!");
        }
    }
    public Spot Clone()
    {
            SpotSymbol pClone =  new SpotSymbol();

            pClone.m_wID   = m_wID;
            pClone.m_nScale = m_nScale;
            pClone.m_nAngle = m_nAngle;

            return pClone;
    }

    public Rectangle GetRect(Library library)
    {
        Rectangle rect = new Rectangle( -10, -10, 20, 20);
        if (library == null)
        {
            return rect;
        }

        Symbol symbol = library.m_libSpotSymbol.GetSymbol(m_wID);
        if (symbol == null)
        {
            return rect;
        }

        rect = symbol.GetRect();

        rect.x = (int) (rect.x * (float) m_nScale / 100);
        rect.y = (int) (rect.y * (float) m_nScale / 100);
        rect.width = (int) (rect.width * (float) m_nScale / 100);
        rect.height = (int) (rect.height * (float) m_nScale / 100);

        return rect;
    }

    public Rectangle GetRect(Library library, float fScale)
    {
        if (library == null)
        {
            return new Rectangle( -10, -10, 20, 20);
        }

        Symbol symbol = library.m_libSpotSymbol.GetSymbol(m_wID);
        if (symbol == null)
        {
            return new Rectangle( -10, -10, 20, 20);
        }
        else
        {
            Rectangle rect = new Rectangle(symbol.GetRect());

            rect.x = (int) (rect.x * fScale * m_nScale / 100);
            rect.y = (int) (rect.y * fScale * m_nScale / 100);
            rect.width = (int) (rect.width * fScale * m_nScale / 100);
            rect.height = (int) (rect.height * fScale * m_nScale / 100);

            return rect;
        }
    }

    public void Draw(Graphics2D g2, Library pLibrary, Point location, double fSin, double fCos, float fZoom)
    {
        if (pLibrary == null)
        {
            return;
        }

        Symbol symbol = pLibrary.m_libSpotSymbol.GetSymbol(m_wID);
        if (symbol == null)
        {
            return;
        }

        fZoom *= (float) m_nScale / 100;

        double a11 = fZoom*fCos;
        double a21 = -fSin*fZoom;
        double a31 = location.x;
        double a12 = fZoom*fSin;
        double a22 = fZoom*fCos;
        double a32 = location.y;

        symbol.Draw(g2, a11, a21, a31, a12, a22, a32);
    }


}

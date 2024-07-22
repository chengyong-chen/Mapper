package Mapeasy.Style;

import java.awt.Graphics2D;
import java.awt.Point;
import java.awt.Dimension;
import java.awt.Rectangle;
import java.io.IOException;
import java.io.DataInputStream;

public class SpotPattern extends Spot
{
    public short m_nPatternID;
    public short m_nPatternIndex;
    public short m_nScale;

    public SpotPattern()
    {
    }

    public void Serialize(DataInputStream dataStream)
    {
        super.Serialize(dataStream);

        try
        {
            m_nPatternID = dataStream.readShort();
            m_nPatternIndex = dataStream.readShort();
            m_nScale = dataStream.readShort();
        }
        catch (IOException ioe)
        {
            System.out.println("error when read SpotPattern!");
        }
    }
    public Spot Clone()
    {
            SpotPattern pClone = new SpotPattern();

            pClone.m_nPatternID = m_nPatternID;
            pClone.m_nPatternIndex = m_nPatternIndex;
            pClone.m_nScale = m_nScale;

            return pClone;
    }

    public Rectangle GetRect(Library library)
    {
        if (library == null)
        {
            return new Rectangle(0, 0, 0, 0);
        }

        Pattern pattern = library.m_libPattern.GetByID(m_nPatternID);
        if (pattern == null)
        {
            return new Rectangle(0, 0, 0, 0);
        }

        Dimension size = pattern.GetSize();

        size.width *= 10;
        size.height *= 10;

        return new Rectangle( -size.width/2, -size.height/2, size.width, size.height);
    }

    public Rectangle GetRect(Library library, float fScale)
    {
        if (library == null)
        {
            return new Rectangle(0, 0, 0, 0);
        }

        Pattern pattern = library.m_libPattern.GetByID(m_nPatternID);
        if (pattern == null)
        {
            return new Rectangle(0, 0, 0, 0);
        }

        Dimension size = pattern.GetSize();
        return new Rectangle((int) ( -size.width * fScale * 10 / 2), (int) ( -size.height * fScale * 10 / 2), (int) (size.width * fScale * 10), (int) (size.height * fScale * 10));
    }

    public void Draw(Graphics2D g2, Library pLibrary, Point location, double fSin, double fCos, float fZoom)
    {
        if (pLibrary == null)
        {
            return;
        }

        Pattern pPattern = pLibrary.m_libPattern.GetByID(m_nPatternID);
        if (pPattern == null)
        {
            return;
        }
        fZoom = (fZoom * m_nScale) / 100;
        pPattern.Draw(g2, location, m_nPatternIndex, fZoom);
    }
}

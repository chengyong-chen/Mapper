package Mapeasy.Style;

import java.awt.Rectangle;
import java.io.IOException;
import java.io.DataInputStream;
import java.awt.Graphics2D;
import java.awt.Point;

public class Spot
{
    public Spot()
    {
    }

    static public Spot FromType(byte type)
    {
        Spot spot = null;

        switch (type)
        {
            case 0:
                spot = new Spot();
                break;
            case 1:
                spot = new SpotSymbol();
                break;
            case 2:
                spot = new SpotChar();
                break;
            case 3:
                spot = new SpotPattern();
                break;
        }

        return spot;
    }

    public void Serialize(DataInputStream dataStream)
    {
    }

    public Spot Clone()
    {
        Spot pClone = new Spot();

        return pClone;
    }

    public void Draw(Graphics2D g2, Library pLibrary, Point location, double fSin, double fCos, float fZoom)
    {
    }


    public Rectangle GetRect(Library library)
    {
        Rectangle rect = new Rectangle(0, 0, 0, 0);

        return rect;
    }

    public Rectangle GetRect(Library library, float fScale)
    {
        Rectangle rect = new Rectangle(0, 0, 0, 0);

        return rect;
    }

}

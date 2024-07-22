package Mapeasy.Style;

import java.awt.Color;
import java.io.DataInputStream;
import java.io.IOException;

public class Colour
{
    public byte m_bID;
    public byte alpha;

    public Colour()
    {
    }

    static public Colour FromType(byte index)
    {
        Colour colour = null;

        switch (index)
        {
            case 0:
                colour = new Colour();
                break;
            case 1:
                colour = new ColourProcess();
                break;
            case 2:
                colour = new ColourSpot();
                break;
            case 3:
                colour = new ColourTint();
                break;
        }

        return colour;
    }

    public void Serialize(DataInputStream dataStream)
    {
        try
        {
            m_bID = dataStream.readByte();
            alpha = dataStream.readByte();
        }
        catch (IOException ioe)
        {
            System.out.println("error when read Fill!");
        }
    }
    public Colour Clone()
    {
            Colour pClone =  new Colour();

            pClone.m_bID = m_bID;
            pClone.alpha = alpha;
            return pClone;
    }

    public Color GetColor()
    {
        return new Color(0, 0, 0,255);
    }
}

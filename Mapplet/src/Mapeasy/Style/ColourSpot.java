package Mapeasy.Style;

import java.awt.Color;
import java.io.IOException;
import java.io.DataInputStream;

public class ColourSpot extends Colour
{
    public byte m_bR=0;
    public byte m_bG=0;
    public byte m_bB=0;

    public ColourSpot()
    {
    }

    public void Serialize(DataInputStream dataStream)
    {
        super.Serialize(dataStream);

        try
        {
            m_bR = dataStream.readByte();
            m_bG = dataStream.readByte();
            m_bB = dataStream.readByte();
        }
        catch (IOException ioe)
        {
            System.out.println("error when Process colour Fill!");
        }
    }

    public Colour Clone()
    {
        ColourSpot pClone = new ColourSpot();

        pClone.m_bID = m_bID;
        pClone.alpha = alpha;

        pClone.m_bR = m_bR;
        pClone.m_bG = m_bG;
        pClone.m_bB = m_bB;

        return pClone;
    }

    public Color GetColor()
    {
        int R = (int)(m_bR&0XFF);
        int G = (int)(m_bG&0XFF);
        int B = (int)(m_bB&0XFF);
        int A = (int)(alpha&0XFF);

        if (R > 255)
        {
            R = 255;
        }
        if (G > 255)
        {
            G = 255;
        }
        if (B > 255)
        {
            B = 255;
        }

        if (R < 0)
        {
            R = 0;
        }
        if (G < 0)
        {
            G = 0;
        }
        if (B < 0)
        {
            B = 0;
        }
        if (A < (int) 0)
        {
            A = 255;
        }

        return new Color(R,G,B,A);
    }
}

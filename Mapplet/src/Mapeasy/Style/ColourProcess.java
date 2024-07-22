package Mapeasy.Style;

import java.awt.Color;
import java.io.IOException;
import java.io.DataInputStream;

public class ColourProcess extends Colour
{
    public byte m_bC;
    public byte m_bM;
    public byte m_bY;
    public byte m_bK;

    public ColourProcess()
    {
    }

    public void Serialize(DataInputStream dataStream)
    {
        super.Serialize(dataStream);

        try
        {
            m_bC = dataStream.readByte();
            m_bM = dataStream.readByte();
            m_bY = dataStream.readByte();
            m_bK = dataStream.readByte();
        }
        catch (IOException ioe)
        {
            System.out.println("error when Process colour Fill!");
        }
    }

    public Colour Clone()
    {
        ColourProcess pClone = new ColourProcess();

        pClone.m_bID = m_bID;
        pClone.alpha = alpha;

        pClone.m_bC = m_bC;
        pClone.m_bM = m_bM;
        pClone.m_bY = m_bY;
        pClone.m_bK = m_bK;

        return pClone;
    }

    public Color GetColor()
    {
        int R = (100 - (int)(m_bC&0XFF) - (int)(m_bK&0XFF)) * 255 / 100;
        int G = (100 - (int)(m_bM&0XFF) - (int)(m_bK&0XFF)) * 255 / 100;
        int B = (100 - (int)(m_bY&0XFF) - (int)(m_bK&0XFF)) * 255 / 100;
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

        return new Color(R, G, B, A);
    }

}

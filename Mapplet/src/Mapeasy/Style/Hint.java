package Mapeasy.Style;

import java.awt.Color;
import java.awt.Font;
import java.awt.Paint;
import java.io.IOException;
import java.io.DataInputStream;
import java.awt.BasicStroke;
import java.awt.Stroke;

public class Hint
{
    public FontDesc m_desc = new FontDesc();
    public short m_nSize;
    public Colour m_Colour = null;

    public short m_nBordWidth;
    public int m_nBordColor;
    public int m_nGroundColor;
    public byte m_cFrame;

    public Hint()
    {
    }

    public void Serialize(DataInputStream dataStream)
    {
        try
        {
            m_desc.Serialize(dataStream);
            m_nSize = dataStream.readShort();
            byte cSwitch = dataStream.readByte();

            byte colorindex = (byte) (cSwitch & 0X0F);
            m_Colour = Colour.FromType(colorindex);

            m_cFrame = (byte) (cSwitch >> 4);
            m_cFrame &= 0X0F;
            if (m_cFrame == 0X0F)
            {
                m_cFrame = 0X00;
            }
            if ((m_cFrame & 0X01) == 0X01)
            {
                m_nBordWidth = dataStream.readShort();
                m_nBordColor = dataStream.readInt();
            }
            if ((m_cFrame & 0X02) == 0X02)
            {
                m_nGroundColor = dataStream.readInt();
            }
        }
        catch (IOException ioe)
        {
            System.out.println("error when read Hint!");
        }

        if (m_Colour != null)
        {
            m_Colour.Serialize(dataStream);
        }
    }

    public Hint Clone()
    {
        Hint pClone = new Hint();

        pClone.m_desc = m_desc;
        pClone.m_nSize = m_nSize;
        pClone.m_nBordWidth = m_nBordWidth;
        pClone.m_nBordColor = m_nBordColor;
        pClone.m_nGroundColor = m_nGroundColor;
        pClone.m_cFrame = m_cFrame;

        if (pClone.m_Colour != null)
        {
            //            delete pClone->m_pColour;
            pClone.m_Colour = null;
        }
        if (m_Colour != null)
        {
            pClone.m_Colour = m_Colour.Clone();
        }

        return pClone;
    }

    public Font GetFont()
    {
        String strFont = m_desc.m_strName;
        Font font = new Font(strFont, Font.PLAIN, m_nSize);
        return font;
    }

    public Font GetFont(float fScale)
    {
        String strFont = m_desc.m_strName;
        Font font = new Font(strFont, Font.PLAIN, (int) (m_nSize*fScale));
        return font;
    }

    public Stroke GetRimPen(float fScale)
    {
        if ((m_cFrame & 0X01) == 0X01)
        {
            int nWidth = (int) (m_nBordWidth * fScale);

            BasicStroke pen = new BasicStroke(nWidth, BasicStroke.CAP_ROUND, BasicStroke.JOIN_ROUND);
            return pen;
        }
        else
            return null;
    }

    public Color GetTextColor()
    {
        if (m_Colour != null)
        {
            Color color = m_Colour.GetColor();
            return color;
        }
        else
        {
            Color color = new Color(0, 0, 0);
            return color;
        }
    }

    public Color GetRimColor()
    {
        if ((m_cFrame & 0X02) == 0X02)
        {
            byte r = Public.GetRValue(m_nBordColor);
            byte g = Public.GetGValue(m_nBordColor);
            byte b = Public.GetBValue(m_nBordColor);
            int R = (int) (r & 0XFF);
            int G = (int) (g & 0XFF);
            int B = (int) (b & 0XFF);

            Color color = new Color(R, G, B);
            return color;
        }
        else
        {
            return null;
        }
    }

    public Paint GetRimBrush()
    {
        if ((m_cFrame & 0X01) == 0X01)
        {
            byte r = Public.GetRValue(m_nGroundColor);
            byte g = Public.GetGValue(m_nGroundColor);
            byte b = Public.GetBValue(m_nGroundColor);

            int R = (int) (r & 0XFF);
            int G = (int) (g & 0XFF);
            int B = (int) (b & 0XFF);

            Color color = new Color(R, G, B);
            return color;
        }
        else
            return null;
    }
}

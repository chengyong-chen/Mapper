package Mapeasy.Style;

import java.awt.Color;
import java.awt.Font;
import java.io.IOException;
import java.io.DataInputStream;

public class Type
{
    public FontDesc m_fontdesc = new FontDesc();

    public Colour m_Colour = null;

    public short m_nFontsize;

    public byte m_nLeterspace;
    public byte m_nWordspace;
    public short m_nWeight;

    public byte m_bStyle;
    public byte m_nLeadindex;
    public byte m_nHorzscale;
    public byte m_nHSkewAngle;
    public byte m_nVSkewAngle;

    public short m_nBordWidth;
    public int m_nBordColor;
    public int m_nGroundColor;


    public Type()
    {
    }

    public void Serialize(DataInputStream dataStream)
    {
        try
        {
            m_fontdesc.Serialize(dataStream);

            m_nFontsize = dataStream.readShort();
            m_nWeight = dataStream.readShort();

            m_bStyle = dataStream.readByte();
            m_nLeadindex = dataStream.readByte();
            m_nHSkewAngle = dataStream.readByte();
            m_nVSkewAngle = dataStream.readByte();
            m_nHorzscale = dataStream.readByte();

            if ((m_bStyle & 0x01) == 0x01)
            {
                m_nBordWidth = dataStream.readShort();
                m_nBordColor = dataStream.readInt();
            }
            if ((m_bStyle & 0x02) == 0x02)
            {
                m_nGroundColor = dataStream.readInt();
            }
            byte colorindex = dataStream.readByte();
            if (m_Colour != null)
            {
                m_Colour = null;
            }
            m_Colour = Colour.FromType(colorindex);
        }
        catch (IOException ioe)
        {
            System.out.println("error when read Type!");
        }

        if (m_Colour != null)
        {
            m_Colour.Serialize(dataStream);
        }
    }

    public Type Clone()
    {
            Type pClone =  new Type();

            pClone.m_fontdesc    = m_fontdesc;
            pClone.m_nFontsize   = m_nFontsize;
            pClone.m_nLeterspace = m_nLeterspace;
            pClone.m_nWordspace  = m_nWordspace;
            pClone.m_bStyle      = m_bStyle;
            pClone.m_nLeadindex  = m_nLeadindex;
            pClone.m_nWeight     = m_nWeight;
            pClone.m_nHSkewAngle = m_nHSkewAngle;
            pClone.m_nVSkewAngle = m_nVSkewAngle;
            pClone.m_nHorzscale  = m_nHorzscale;

            if(pClone.m_Colour != null)
            {
         //           delete pClone.m_pColor;
                    pClone.m_Colour = null;
            }
            if(m_Colour != null)
            {
                    pClone.m_Colour = m_Colour.Clone();
            }

            return pClone;
}
    public Font GetFont(float fZoom)
    {
        String strFont = m_fontdesc.m_strName;
        int size = (int) (m_nFontsize * fZoom);
        Font font = new Font(strFont, Font.PLAIN, size);
        return font;
    }

    public Color GetColor()
    {
        Color color = m_Colour == null ? new Color(0, 0, 0,255) : m_Colour.GetColor();
        return color;
    }

}

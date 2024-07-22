package Mapeasy.Style;

import java.io.IOException;
import java.io.DataInputStream;
import java.awt.Point;
import java.awt.Dimension;
import java.awt.Image;
import java.awt.Color;
import java.awt.Graphics;
import java.awt.Graphics2D;

public class Pattern
{
    public short m_nCount;
    public short m_nHeight;

    public short m_wID;

//    public ImageList m_imagelist;
    public Image m_Bitmap = null;

    public Pattern()
    {
    }

    public void Serialize(DataInputStream dataStream)
    {
        try
        {
            m_wID = dataStream.readShort();

            m_nCount = dataStream.readShort();
            m_nHeight = dataStream.readShort();
        }
        catch (IOException ioe)
        {
            System.out.println("error when read Pattern!");
        }
    }

    public Pattern Clone()
    {
        Pattern pattern = new Pattern();
        pattern.m_wID = m_wID;
        pattern.m_nCount = m_nCount;
        pattern.m_nHeight = m_nHeight;

        return pattern;
    }

    public Dimension GetSize()
    {
        return new Dimension(m_nHeight, m_nHeight);
    }

    public void Draw(Graphics2D g2, Point location, int index, float fScale)
    {
        if (m_Bitmap != null)
        {
            g2.translate(location.x, location.y);
            g2.scale(10, -10);
            g2.scale(fScale, fScale);

            g2.drawImage(m_Bitmap,(int)(-m_nHeight/2),(int)(-m_nHeight/2),(int)(m_nHeight/2),(int)(m_nHeight/2),(int)(m_nHeight*index), (int)0, (int)(m_nHeight*index+1), (int)m_nHeight,new Color(192,192,192),null);

            g2.scale(1 / fScale, 1 / fScale);
            g2.scale(0.1, -0.1);
            g2.translate(-location.x, -location.y);
        }
    }
}

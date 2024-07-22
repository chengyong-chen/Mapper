package Mapeasy.Style;

import java.awt.Graphics2D;
import java.awt.Rectangle;
import java.awt.Point;
import java.awt.Paint;
import java.awt.Color;
import java.lang.String;
import java.io.DataInputStream;
import java.io.IOException;
import java.awt.Font;

public class SpotChar extends Spot
{
    public short m_wID;
    public short m_nSize;
    public String m_strFont;
    public Colour m_Color = null;

    public SpotChar()
    {
    }

    public void Serialize(DataInputStream dataStream)
    {
        super.Serialize(dataStream);

        try
        {
            m_wID = dataStream.readShort();
            m_nSize = dataStream.readShort();
            m_strFont = dataStream.readUTF();

            byte colortype = dataStream.readByte();
            m_Color = Colour.FromType(colortype);
        }
        catch (IOException ioe)
        {
            System.out.println("error when read SpotChar!");
        }

        if (m_Color != null)
        {
            m_Color.Serialize(dataStream);
        }
    }

    public Spot Clone()
{
        SpotChar pClone =  new SpotChar();

        if(pClone.m_Color != null)
        {
  //              delete pClone->m_pColor;
                pClone.m_Color = null;
        }
        if(m_Color != null)
        {
                pClone.m_Color = m_Color.Clone();
        }

        pClone.m_wID = m_wID;
        pClone.m_nSize = m_nSize;
        pClone.m_strFont = m_strFont;

        return pClone;
}


    public Rectangle GetRect(Library library)
    {
        return new Rectangle( -m_nSize / 2, -m_nSize / 2, m_nSize, m_nSize);
    }

    public Rectangle GetRect(Library library, float fScale)
    {
        return new Rectangle((int) ( -m_nSize * fScale / 2), (int) ( -m_nSize * fScale / 2), (int) (m_nSize * fScale), (int) (m_nSize * fScale));
    }


    public void Draw(Graphics2D g2, Library pLibrary, Point location, double fSin, double fCos, float fZoom)
    {
        int size = (int) (m_nSize * fZoom);
        Font font = new Font(m_strFont, Font.PLAIN, size);

        Paint textbrush = null;
        if (m_Color != null)
        {
            textbrush = m_Color.GetColor();
        }
        else
        {
            textbrush = new Color(0, 0, 0);
        }

        String strChar = new String();
        strChar.format("%c", m_wID);

        g2.translate(location.x, location.y);
        g2.scale(1.0f, -1.0f);

      //  StringFormat stringFormat;
      //  stringFormat.SetAlignment(StringAlignmentCenter);
        if (fSin != 0.0f)
        {
            g2.drawString(strChar,0,0); //不是太精确
        }
        else
        {
            g2.drawString(strChar,0,0); //不是太精确
        }

        Paint brush = new Color(255, 5, 5);
   //     g2.FillEllipse( & brush, RectF( -1, -1, 2, 2));

        g2.scale(1.0f, -1.0f);
        g2.translate( -location.x, -location.y);
    }
}

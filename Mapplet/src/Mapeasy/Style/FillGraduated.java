package Mapeasy.Style;

import java.awt.Paint;
import java.awt.GradientPaint;
import java.awt.Color;
import java.io.IOException;
import java.io.DataInputStream;

public class FillGraduated extends Fill
{
    public Colour m_colour1;
    public Colour m_colour2;

    short m_nAngle;

    public FillGraduated()
    {
    }
    public byte Gettype()
     {
         return 3;
    };

    public void Serialize(DataInputStream dataStream)
    {
        super.Serialize(dataStream);

        try
        {
            m_nAngle = dataStream.readShort();

            byte type1 = dataStream.readByte();
            byte type2 = dataStream.readByte();
            if (m_colour1 != null)
            {
                m_colour1 = null;
            }
            if (m_colour2 != null)
            {
                m_colour2 = null;
            }

            m_colour1 = Colour.FromType(type1);
            m_colour2 = Colour.FromType(type2);
        }
        catch (IOException ioe)
        {
            System.out.println("error when read FillGraduated!");
        }

        if (m_colour1 != null)
        {
            m_colour1.Serialize(dataStream);
        }
        if (m_colour2 != null)
        {
            m_colour2.Serialize(dataStream);
        }
    }

    public Fill Clone()
    {
            FillGraduated pClone = new FillGraduated();

            if(m_colour1 != null)
            {
                    pClone.m_colour1 = m_colour1.Clone();
            }
            if(m_colour2 != null)
            {
                    pClone.m_colour2 = m_colour2.Clone();
            }

            pClone.m_nAngle = m_nAngle;

            return pClone;
    }

    public Paint GetBrush(Library library)
    {
        if (m_colour1 != null || m_colour2 != null)
        {
            Color color1 = null;
            Color color2 = null;
            if (m_colour1 != null)
            {
                color1 = m_colour1.GetColor();
            }
            if(color1 == null)
            {
                color1 = new Color(255,255,255);
            }
            if (m_colour2 != null)
            {
                color2 = m_colour2.GetColor();
            }
            if(color2 == null)
            {
                color2 = new Color(0,0,0);
            }

            GradientPaint brush = new GradientPaint(10,10,color1,100,1600,color2);//m_nAngle
            return brush;
        }
        else
        {
            return null;
        }
    }

}

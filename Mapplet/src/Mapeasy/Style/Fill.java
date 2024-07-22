package Mapeasy.Style;

import java.awt.Color;
import java.awt.Paint;
import java.io.DataInputStream;
import java.io.IOException;

public class Fill
{
    public Colour m_Colour = null;

    public Fill()
    {
    }

    public byte Gettype()
    {
        return 0;
    };

    static public Fill FromType(byte type)
    {
        Fill fill = null;

        switch (type)
        {
            case 0:
                fill = new Fill();
                break;
            case 1:
                fill = new FillNormal();
                break;
            case 2:
                fill = new FillSymbol();
                break;
            case 3:
                fill = new FillGraduated();
                break;
            case 4:
                fill = new FillRadial();
                break;
            case 5:
                fill = new FillPattern();
                break;
        }

        return fill;
    }

    public void Serialize(DataInputStream dataStream)
    {
        try
        {
            byte type = dataStream.readByte();
            if (m_Colour != null)
            {
                m_Colour = null;
            }
            m_Colour = Colour.FromType(type);
        }
        catch (IOException ioe)
        {
            System.out.println("error when read Fill!");
        }

        if (m_Colour != null)
        {
            m_Colour.Serialize(dataStream);
        }
    }

    public Fill Clone()
    {
        Fill pClone = new Fill();

        if (pClone.m_Colour != null)
        {
            //   delete  pClone.m_pColor;
            pClone.m_Colour = null;
        }
        if (m_Colour != null)
        {
            pClone.m_Colour = m_Colour.Clone();
        }

        return pClone;
    }

    public Paint GetBrush(Library library)
    {
        if (m_Colour != null)
        {
            Color color = m_Colour.GetColor();
            Paint brush = color; //new Paint(color);
            return brush;
        }
        else
        {
            Paint brush = new Color(255, 255, 255);
            return brush;
        }
    }

}

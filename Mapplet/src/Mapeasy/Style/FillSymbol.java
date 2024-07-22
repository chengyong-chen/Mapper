package Mapeasy.Style;

import java.awt.Paint;
import java.io.IOException;
import java.io.DataInputStream;

public class FillSymbol extends Fill
{
    public short m_wID;

    public short m_nScale;
    public short m_nAngle;

    public FillSymbol()
    {
    }
    public byte Gettype()
     {
         return 2;
    };
    public void Serialize(DataInputStream dataStream)
    {
        super.Serialize(dataStream);

        try
        {
            m_wID = dataStream.readShort();
            m_nScale = dataStream.readShort();
            m_nAngle = dataStream.readShort();
        }
        catch (IOException ioe)
        {
            System.out.println("error when read FillSymbol!");
        }
    }
    public Fill Clone()
    {
            FillSymbol pClone =  new FillSymbol();

            pClone.m_wID = m_wID;
            pClone.m_nScale = m_nScale;
            pClone.m_nAngle = m_nAngle;

            return pClone;
    }

    public Paint GetBrush(Library library)
    {
        if (library == null)
        {
            return null;
        }

        Symbol symbol = library.m_libFillSymbol.GetSymbol(m_wID);
        if (symbol == null)
        {
            return null;
        }
        else
            return null;
    }
}

package Mapeasy.Style;

import java.awt.TexturePaint;
import java.awt.Paint;
import java.io.IOException;
import java.io.DataInputStream;

public class FillPattern extends Fill
{
    public byte m_nPatternID;
    public byte m_nPatternIndex;
    public short m_nScale;
    public short m_nAngle;

    public FillPattern()
    {
    }

    public byte Gettype()
     {
         return 5;
    };
    public void Serialize(DataInputStream dataStream)
    {
        super.Serialize(dataStream);

        try
        {
            m_nPatternID = dataStream.readByte();
            m_nPatternIndex = dataStream.readByte();
            m_nScale = dataStream.readShort();
            m_nAngle = dataStream.readShort();
        }
        catch (IOException ioe)
        {
            System.out.println("error when read FillPattern!");
        }
    }

    public Fill Clone()
    {
            FillPattern pClone =  new FillPattern();

            pClone.m_nPatternID = m_nPatternID;
            pClone.m_nPatternIndex = m_nPatternIndex;
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

        Pattern pattern = library.m_libPattern.GetByID(m_nPatternID);
        if (pattern == null)
        {
            return null;
        }
        /*
                HBITMAP bitmap = pattern.GetHBITMAP(m_nPatternIndex);

                Bitmap bitmap = Bitmap::FromHBITMAP(bitmap, NULL);
                TexturePaint brush = new TexturePaint(pBitmap);
                delete pBitmap;
                pBitmap = null;

//	DeleteObject(bitmap);
                return brush;*/
        return null;
    }

}

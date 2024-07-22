package Mapeasy.Style;

import java.io.DataInputStream;
import java.io.IOException;

public class FontDesc
{
    public String m_strName;
    public String m_strScript;
    public byte m_nCharSet;
    public byte m_nPitchAndFamily;
    public int m_dwFlags;


    public FontDesc()
    {
    }

    public void Serialize(DataInputStream dataStream)
    {
        try
        {
            m_strName = dataStream.readUTF();
            m_nCharSet = dataStream.readByte();
            m_nPitchAndFamily = dataStream.readByte();
            m_dwFlags = dataStream.readInt();
        }
        catch (IOException ioe)
        {
            System.out.println("error when read FillSymbol!");
        }
    }
}

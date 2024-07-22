package Mapeasy.Public;

import java.awt.Dimension;
import java.io.DataInputStream;
import java.io.IOException;

public class Paper
{
    public Short m_viewLower = 1;
    public short m_viewUpper = 1000;

    public Dimension m_sizePaper = new Dimension(0,0);

    public byte m_bDecimal = 1;

    public Paper()
    {
    }

    public void Serialize(DataInputStream dataStream)
    {
        try
        {
            m_sizePaper.width = dataStream.readInt();
            m_sizePaper.height = dataStream.readInt();
            m_viewLower = dataStream.readShort();
            m_viewUpper = dataStream.readShort();
        }
        catch (IOException ioe)
       {
           System.out.println("error when read Paper parameter!");
       }
    }

    public int GetDecimal()
    {
        return (int)Math.pow(10.0f,m_bDecimal);
    }
}

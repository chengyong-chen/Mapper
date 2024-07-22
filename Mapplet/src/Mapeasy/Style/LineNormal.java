package Mapeasy.Style;

import java.io.IOException;
import java.io.DataInputStream;

public class LineNormal extends Line
{
    public short m_wColorID;
    public short m_wDashID;

    public LineNormal()
    {
    }

    public byte Gettype()
        {
            return 1;
        };
    public void Serialize(DataInputStream dataStream)
    {
        super.Serialize(dataStream);

        try
        {
            m_wColorID = dataStream.readShort();
            m_wDashID = dataStream.readShort();
        }
        catch (IOException ioe)
        {
            System.out.println("error when read Line!");
        }
    }
}

package Mapeasy.Style;

import java.io.IOException;
import java.io.DataInputStream;

public class FillNormal extends Fill
{
    public short m_wColorID;

    public FillNormal()
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
       }
       catch (IOException ioe)
       {
           System.out.println("error when read FillNormal!");
       }
   }
}

package Mapeasy.Style;

import java.io.DataInputStream;
import java.io.IOException;

public class Dash
{
    public short m_wID;

    public int[] segment = new int[8];

    public Dash()
    {
        m_wID = 0;
        for (byte i = 0; i < 8; i++)
        {
            segment[i] = 0;
        }
    }

    public void Serialize(DataInputStream dataStream)
    {
        try
        {
            m_wID = dataStream.readShort();
            for (byte i = 0; i < 8; i++)
            {
                segment[i] = dataStream.readInt();
            }
        }
        catch (IOException ioe)
        {
            System.out.println("error when read Fill!");
        }
    }

    public void Copy(Dash dashDen)
    {
        dashDen.m_wID = m_wID;
        for(int index=0;index<8;index++)
        {
            dashDen.segment[index] = segment[index];
        }
     //   System.arraycopy(segment,0,dashDen.segment,0,8);
    }

    public Dash Clone()
    {
        Dash pClone = new Dash();

        pClone.m_wID = m_wID;
        for(int index=0;index<8;index++)
        {
            pClone.segment[index] = segment[index];
        }
  //    System.arraycopy(segment,0,pClone.segment,0,8);

        return pClone;
    }

    public short GetCount()
    {
        for (short i = 0; i < 4; i++)
        {
            if (segment[i * 2] == 0 || segment[i * 2 + 1] == 0)
            {
                return (short) (i * 2);
            }
        }

        return 4 * 2;
    }

}

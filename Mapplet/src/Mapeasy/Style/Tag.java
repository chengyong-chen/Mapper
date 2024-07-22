package Mapeasy.Style;

import java.io.DataInputStream;
import java.io.IOException;

public class Tag
{
    public short m_wID;
    public short m_wSymID;
    public Symbol m_pSymbol = null;

    public Tag()
    {
    }

    public void Serialize(DataInputStream dataStream)
    {
        try
        {
            m_wID = dataStream.readShort();
            m_wSymID = dataStream.readShort();
            byte type = dataStream.readByte();
            m_pSymbol = Symbol.FromType(type);
            if(m_pSymbol != null)
            {
                m_pSymbol.Serialize(dataStream);
            }
        }
        catch (IOException ioe)
        {
            System.out.println("error when read Tag!");
        }
    }

    public Tag Clone()
{
        Tag tag = new Tag();
        tag.m_wID = m_wID;
        tag.m_wSymID = m_wSymID;

        if(m_pSymbol != null)
        {
                tag.m_pSymbol = m_pSymbol.Clone(null);
        }
        return tag;
}

}

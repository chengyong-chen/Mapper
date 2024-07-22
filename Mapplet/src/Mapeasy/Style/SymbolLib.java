package Mapeasy.Style;

import java.util.Vector;
import java.io.IOException;
import java.io.DataInputStream;

public class SymbolLib
{
    public Vector<Tag> m_taglist = null;

    public SymbolLib()
    {
    }

    public void Serialize(DataInputStream dataStream)
    {
        try
        {
            m_taglist = new Vector<Tag>();
            short count = dataStream.readShort();
            for (short index = 0; index < count; index++)
            {
                Tag tag = new Tag();
                tag.Serialize(dataStream);
                m_taglist.addElement(tag);
            }
        }
        catch (IOException ioe)
        {
            System.out.println("error when read Pattern!");
        }
    }


    Symbol GetSymbol(short wID)
    {
        Tag pTag = null;
        for(short index=0;index<m_taglist.size();index++)
        {
            Tag tag = m_taglist.elementAt(index);
            if(tag == null)
            {
                continue;
            }

            if (tag.m_wID == wID)
            {
                pTag = tag;
                break;
            }
        }

        if (pTag != null)
        {
            return pTag.m_pSymbol;
        }
        else
        {
            return null;
        }
    }
}

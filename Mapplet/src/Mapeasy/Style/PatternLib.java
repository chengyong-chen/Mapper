package Mapeasy.Style;

import java.util.Vector;
import java.io.IOException;
import java.io.DataInputStream;

public class PatternLib
{
    public Vector<Pattern> m_Patternlist;

    public PatternLib()
    {
    }

    public void Serialize(DataInputStream dataStream)
    {
        try
        {
            short count = dataStream.readShort();
            for (short index = 0; index < count; index++)
            {
                Pattern pattern = new Pattern();
                pattern.Serialize(dataStream);
                m_Patternlist.addElement(pattern);
            }
        }
        catch (IOException ioe)
        {
            System.out.println("error when read PatternLib!");
        }
    }

    public Pattern GetByID(short wID)
    {
        for (short index = 0; index < m_Patternlist.size(); index++)
        {
            Pattern pattern = m_Patternlist.elementAt(index);
            if (pattern == null)
            {
                continue;
            }

            if (pattern.m_wID == wID)
            {
                return pattern;
            }
        }

        return null;
    }

}

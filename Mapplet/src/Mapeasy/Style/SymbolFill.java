package Mapeasy.Style;

import java.io.IOException;
import java.io.DataInputStream;
import Mapeasy.Obj.Obj;

public class SymbolFill extends Symbol
{
    public SymbolFill()
    {
    }

    public void Serialize(DataInputStream dataStream)
    {
        super.Serialize(dataStream);

        try
        {
            m_Rect.x = dataStream.readInt();
            m_Rect.y = dataStream.readInt();
            m_Rect.width = dataStream.readInt();
            m_Rect.height = dataStream.readInt();
        }
        catch (IOException ioe)
        {
            System.out.println("error when read SymbolFill!");
        }
    }

    public Symbol Clone(Symbol pSymbol)
    {
        if (pSymbol == null)
        {
            pSymbol = new SymbolFill();
        }

        return super.Clone(pSymbol);
    }
}

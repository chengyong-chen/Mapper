package Mapeasy.Style;

import java.io.IOException;
import java.io.DataInputStream;
import java.awt.Point;

public class SymbolSpot extends Symbol
{
    public Point m_Anchor = new Point();
    public boolean m_bDirection;

    public SymbolSpot()
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

            m_Anchor.x = dataStream.readInt();
            m_Anchor.y = dataStream.readInt();
            m_bDirection = dataStream.readBoolean();
        }
        catch (IOException ioe)
        {
            System.out.println("error when read SymbolLine!");
        }
    }

    public Symbol Clone(Symbol pSymbol)
    {
        if (pSymbol == null)
        {
            pSymbol = new SymbolSpot();
        }

        ((SymbolSpot) pSymbol).m_Anchor = m_Anchor;
        ((SymbolSpot) pSymbol).m_bDirection = m_bDirection;

        return super.Clone(pSymbol);
    }

}

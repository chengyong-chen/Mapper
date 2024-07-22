package Mapeasy.Style;

import java.awt.Stroke;
import java.awt.Graphics2D;
import java.io.IOException;
import java.io.DataInputStream;

import Mapeasy.Obj.*;
import java.awt.Color;

public class LineSymbol extends Line
{
    public short m_wID;
    public short m_nScale;

    public LineSymbol()
    {
    }

    public byte Gettype()
    {
        return 2;
    };

    public void Serialize(DataInputStream dataStream)
    {
        //      super.Serialize(dataStream);

        try
        {
            m_wID = dataStream.readShort();
            m_nScale = dataStream.readShort();
            m_nWidth = dataStream.readShort();
        }
        catch (IOException ioe)
        {
            System.out.println("error when read Line!");
        }
    }

    public Line Clone()
    {
        LineSymbol pClone = new LineSymbol();

        pClone.m_wID = m_wID;
        pClone.m_nScale = m_nScale;

        pClone.m_dash = m_dash;
        pClone.m_nWidth = m_nWidth;

        pClone.m_bCapindex = m_bCapindex;
        pClone.m_bJoinindex = m_bJoinindex;
        pClone.m_bMiterlimit = m_bMiterlimit;

        return pClone;
    }

    public Color GetColor()
    {
        return null;
    }

    public Stroke GetPen(float fZoom)
    {
        return null;
    }

    public void Draw(Graphics2D g2,Library pLibrary,Obj pObj,float fZoom)
{
        if(pLibrary == null)
                return;

        Symbol symbol = pLibrary.m_libLineSymbol.GetSymbol(m_wID);
        if(symbol == null)
                return;

        ((SymbolLine)symbol).Draw(g2,pObj,fZoom);
}

}

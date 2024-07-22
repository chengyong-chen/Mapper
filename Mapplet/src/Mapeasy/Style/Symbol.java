package Mapeasy.Style;

import java.awt.Graphics2D;
import java.awt.Point;
import java.math.*;
import java.awt.Rectangle;
import java.util.Vector;
import java.io.IOException;
import java.io.DataInputStream;

import Mapeasy.Obj.Obj;
import Mapeasy.Obj.Pole;

public class Symbol
{
    public Rectangle m_Rect = new Rectangle();

    public short m_wID;
    public Vector<Obj> m_objlist = new Vector<Obj>();

    public Symbol()
    {
    }

    static public Symbol FromType(byte type)
    {
        Symbol symbol = null;

        switch (type)
        {
            case 0:
                symbol = new Symbol();
                break;
            case 1:
                symbol = new SymbolLine();
                break;
            case 2:
                symbol = new SymbolFill();
                break;
            case 3:
                symbol = new SymbolSpot();
                break;
        }

        return symbol;
    }

    public void Serialize(DataInputStream dataStream)
    {
        try
        {
            m_objlist.removeAllElements();

            m_wID = dataStream.readShort();
            short count = dataStream.readShort();
            for (short index = 0; index < count; index++)
            {
                byte maker = dataStream.readByte();
                if (maker == 20)
                {
                    byte type = dataStream.readByte();
                    Obj obj = Obj.FromType(type);
                    obj.Serialize(dataStream);
                    m_objlist.addElement(obj);
                }
                else if (maker == 21)
                {
                    Pole pole = new Pole();
                    pole.Serialize(dataStream);
                    m_objlist.addElement(pole);
                }
                else
                {
                    System.out.println("error when read FillNormal!");
                }
            }
        }
        catch (IOException ioe)
        {
            System.out.println("error when read FillNormal!");
        }
    }

    public Symbol Clone(Symbol pSymbol)
    {
        if (pSymbol == null)
        {
            pSymbol = new Symbol();
        }

        pSymbol.m_wID = m_wID;
        pSymbol.m_Rect = new Rectangle(m_Rect);

        for (int index = 0; index < m_objlist.size(); index++)
        {
            Obj obj = m_objlist.elementAt(index);
            if (obj == null)
            {
                continue;
            }

            Obj pClone = obj.Clone();
            if (pClone != null)
            {
                pSymbol.m_objlist.addElement(pClone);
            }
        }

        return pSymbol;
    }

    public Rectangle GetRect()
    {
        return m_Rect;
    }

    public void Draw(Graphics2D g2, double a11, double a21, double a31, double a12, double a22, double a32)
    {
        double fZoom = Math.sqrt(a11*a11 + a22*a22);
        for (int index = 0; index < m_objlist.size(); index++)
        {
            Obj obj = m_objlist.elementAt(index);
            if (obj == null)
            {
                continue;
            }

            Obj pClone = obj.Clone();
            if (pClone != null)
            {
                pClone.Transformed(a11, a21, a31, a12, a22, a32);
                pClone.Draw(g2, null,null,null,null,(float)fZoom);
            }
        }
    }

}

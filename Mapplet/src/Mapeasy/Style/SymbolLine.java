package Mapeasy.Style;

import java.awt.Point;
import java.awt.Color;
import java.awt.Stroke;
import java.awt.Graphics2D;
import java.io.IOException;
import java.io.DataInputStream;

import Mapeasy.Obj.Obj;
import Mapeasy.Obj.Pole;
import Mapeasy.Obj.Poly;

public class SymbolLine extends Symbol
{
    public short m_nWidth;
    public short m_nStep;

    public SymbolLine()
    {
    }

    public void Serialize(DataInputStream dataStream)
    {
        super.Serialize(dataStream);

        try
        {
            m_nWidth = dataStream.readShort();
            m_nStep = dataStream.readShort();
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
            pSymbol = new SymbolLine();
        }

        ((SymbolLine) pSymbol).m_nStep = m_nStep;
        ((SymbolLine) pSymbol).m_nWidth = m_nWidth;

        return super.Clone(pSymbol);
    }


    public void Draw(Graphics2D g2, Obj pObj, float fScale)
    {
        if (m_wID < 0)
        {
            return;
        }
        if (pObj == null)
        {
            return;
        }

        for (int index = 0; index < m_objlist.size(); index++)
        {
            Obj obj = m_objlist.elementAt(index);
            if (obj == null)
            {
                continue;
            }

            if (obj.m_type == 1)//IsKindOf(RUNTIME_CLASS(Poly))
            {
                Line line = obj.m_Line;
                if (line == null)
                {
                    continue;
                }
                Color color = line.GetColor();
                Stroke pen = line.GetPen(fScale);
                if (pen != null)
                {
                    pObj.Draw(g2, pen,color,null,null, fScale);
                }
            }
            else if (obj.m_type == 100)//(RUNTIME_CLASS(Pole)) == TRUE)
            {
                Pole pole = (Pole) obj.Clone();
                pole.Transformed(fScale, 0, 0, 0, fScale, 0);
                long offset = (long)(pole.m_Center.x);
                offset *= fScale;

                short fHandle = 1;
                short[] tHandle = new short[1];
               tHandle[0] = 1;
                double fT = 0;
                double[] tT = new double[1];
               tT[0] = 0;

                do
                {
                    short[] thandle = new short[1];
                   thandle[0] = 1;
                    double[] tt = new double[1];
                   tt[0] = 0;

                    if (pObj.GetHandT(fHandle, fT, offset, thandle, tt) == false)
                    {
                        break;
                    }

                    Point tP = pObj.GetPoint(thandle[0], tt[0]);
                    g2.translate(tP.x, tP.y);

                    if (pole.m_bDirection == true)
                    {
                        double fAngle = pObj.GetAngle(thandle[0], tt[0]);
                        int nAngle = (int) (fAngle*180/Public.PI);
                        g2.rotate(nAngle);
                        pole.Draw(g2,null,null,null,null,fScale);
                        g2.rotate(-nAngle);
                    }
                    else
                    {
                        pole.Draw(g2,null,null,null,null,fScale);
                    }

                    g2.translate(-tP.x,-tP.y);

                    pObj.GetHandT(fHandle, fT, (long)(m_nStep*fScale), tHandle, tT);

                    if (tT[0] == 1.0f)
                    {
                        tHandle[0]++;
                        tT[0] = 0;
                    }

                    if (tHandle[0] == pObj.GetHandleCount())
                    {
                        break;
                    }

                    fHandle = tHandle[0];
                    fT = tT[0];
                }
                while (true);

                if (pole != null)
                {
                    pole = null;
                }
            }
        }
    }
}

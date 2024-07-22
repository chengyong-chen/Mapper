package Mapeasy.Style;

import java.awt.Stroke;
import java.awt.Graphics2D;
import java.io.IOException;
import java.io.DataInputStream;

import Mapeasy.Obj.Obj;

public class LineRiver extends Line
{
    public short m_nFWidth;
    public short m_nTWidth;
    public short m_nStep;


    public LineRiver()
    {
    }

    public byte Gettype()
    {
        return 3;
    };
    public void Serialize(DataInputStream dataStream)
    {
        super.Serialize(dataStream);

        try
        {
            m_nFWidth = dataStream.readShort();
            m_nTWidth = dataStream.readShort();
            m_nStep = dataStream.readShort();
        }
        catch (IOException ioe)
        {
            System.out.println("error when read Line!");
        }
    }

    public Line Clone()
    {
        LineRiver pClone = new LineRiver();
        if (pClone.m_Colour != null)
        {
            //      delete pClone->m_pColor;
            pClone.m_Colour = null;
        }
        if (m_Colour != null)
        {
            pClone.m_Colour = m_Colour.Clone();
        }

        pClone.m_nWidth = m_nWidth;

        pClone.m_dash = m_dash;
        pClone.m_nFWidth = m_nFWidth;
        pClone.m_nTWidth = m_nTWidth;
        pClone.m_nStep = m_nStep;

        pClone.m_bCapindex = m_bCapindex;
        pClone.m_bJoinindex = m_bJoinindex;
        pClone.m_bMiterlimit = m_bMiterlimit;

        return pClone;
    }

    public void Draw(Graphics2D g2, Library pLibrary, Obj pObj, float fScale)
    {
        long length = pObj.GetLength();

        long n = (m_nTWidth - m_nFWidth + 1) / m_nStep;
        float f = (float) length * 2 / (n * (n + 1));

        short fHandle = 1;
        short[] tHandle = new short[1];
       tHandle[0] = 1;
        double fT = 0;
        double[] tT = new double[1];
       tT[0] = 0;

        for (short i = 0; i < n; i++)
        {
            m_nWidth = m_nFWidth;//+(i*m_nStep);
            m_nWidth += (i*m_nStep);

            Stroke pen = this.GetPen(1.0f);
            if (pen != null)
            {
                Stroke oldPen = g2.getStroke();
                g2.setStroke(pen);
                long seglen = (long)((i + 1) * f);
                if (pObj.GetHandT(fHandle, fT, seglen, tHandle, tT) == true)
                {
                    pObj.DrawFHTToTHT(g2,fHandle, fT, tHandle[0], tT[0]);
                }
                fHandle = tHandle[0];
                fT = tT[0];

                g2.setStroke(oldPen);
                pen = null;
            }
        }

        m_nWidth = 0;
    }
}

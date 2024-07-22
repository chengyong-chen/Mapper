package Mapeasy.Style;

import java.awt.Color;
import java.awt.Stroke;
import java.awt.BasicStroke;
import java.io.IOException;
import java.io.DataInputStream;
import java.awt.Graphics2D;
import Mapeasy.Obj.Obj;

public class Line
{
    public Colour m_Colour = null;
    public Dash m_dash = new Dash();
    public short m_nWidth;
    public byte m_bCapindex;
    public byte m_bJoinindex;
    public byte m_bMiterlimit;

    public Line()
    {
    }

    public byte Gettype()
    {
        return 0;
    };

    static public Line FromType(byte type)
    {
        Line line = null;

        switch (type)
        {
            case 0:
                line = new Line();
                break;
            case 1:
                line = new LineNormal();
                break;
            case 2:
                line = new LineSymbol();
                break;
            case 3:
                line = new LineRiver();
                break;
        }

        return line;
    }

    public void Serialize(DataInputStream dataStream)
    {
        m_dash.Serialize(dataStream);

        try
        {
            m_nWidth = dataStream.readShort();
            m_bCapindex = dataStream.readByte();
            m_bJoinindex = dataStream.readByte();
            m_bMiterlimit = dataStream.readByte();

            byte colorindex = dataStream.readByte();
            if (m_Colour != null)
            {
                m_Colour = null;
            }
            m_Colour = Colour.FromType(colorindex);
        }
        catch (IOException ioe)
        {
            System.out.println("error when read Line!");
        }

        if (m_Colour != null)
        {
            m_Colour.Serialize(dataStream);
        }
    }

    public Line Clone()
    {
        Line pClone = new Line();

        if (pClone.m_Colour != null)
        {
            //      delete pClone->m_pColour;
            pClone.m_Colour = null;
        }
        if (m_Colour != null)
        {
            pClone.m_Colour = m_Colour.Clone();
        }

        pClone.m_dash = m_dash;
        pClone.m_nWidth = m_nWidth;

        pClone.m_bCapindex = m_bCapindex;
        pClone.m_bJoinindex = m_bJoinindex;
        pClone.m_bMiterlimit = m_bMiterlimit;

        return pClone;
    }

    public Color GetColor()
    {
        Color color = m_Colour == null ? null : m_Colour.GetColor();
        return color;
    }

    public Stroke GetPen(float fScale)
    {
        if (m_nWidth == 0)
        {
            return null;
        }
        else
        {
            int nWidth = (int) (m_nWidth * fScale);
            Color color = m_Colour == null ? new Color(0, 0, 0,255) : m_Colour.GetColor();
            int cap = BasicStroke.CAP_SQUARE;
            switch (m_bCapindex)
            {
                case 0:
                    cap = BasicStroke.CAP_SQUARE;
                    break;
                case 1:
                    cap = BasicStroke.CAP_ROUND;
                    break;
                case 2:
                    cap = BasicStroke.CAP_BUTT;
                    break;
            }
            int join = BasicStroke.JOIN_ROUND;
            switch (m_bJoinindex)
            {
                case 0:
                    join = BasicStroke.JOIN_BEVEL;
                    break;
                case 1:
                    join = BasicStroke.JOIN_ROUND;
                    break;
                case 2:
                    join = BasicStroke.JOIN_MITER;
                    break;
                case 3:
                    join = BasicStroke.JOIN_MITER;

//                    pen - > SetLineJoin(LineJoinMiterClipped);
                    break;
            }

            int nDashCount = m_dash.GetCount();
            if (nDashCount > 0)
            {
                float[] segment = new float[nDashCount];
                for (int index = 0; index < nDashCount; index++)
                {
                    segment[index] = (float) (m_dash.segment[index] / (10));
                }
                BasicStroke pen = new BasicStroke(nWidth, cap, join, 1.0f, segment, 0);
                //    pen.SetAlignment(PenAlignmentCenter);
                //    pen.SetDashStyle(DashStyleCustom);
                //    pen.SetDashPattern(segment, nDashCount);

                return pen;
            }
            else
            {
                BasicStroke pen = new BasicStroke(nWidth, cap, join, 1.0f);
                return pen;
            }
        }
    }

    public void Draw(Graphics2D g2, Library pLibrary, Obj pObj, float fZoom)
    {
    }
}

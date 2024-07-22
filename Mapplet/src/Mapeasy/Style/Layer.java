package Mapeasy.Style;

import java.io.DataInputStream;
import java.io.IOException;
import java.util.Vector;
import java.awt.Stroke;
import java.awt.*;
import Mapeasy.Obj.Obj;
import Mapeasy.Obj.Mark;
import Mapeasy.Obj.Text;
import Mapeasy.Obj.TextPoly;
import Mapeasy.Obj.PointTip;
import Mapeasy.Obj.FixedTip;
import Mapeasy.Obj.RectTip;
import Mapeasy.Obj.GroupTip;

public class Layer
{
    public short m_wID;
    public short m_wParentID;
    public short m_wCode;
    public String m_strName = null;

    public short m_lowerObj;
    public short m_upperObj;
    public short m_lowerTip;
    public short m_upperTip;

    public boolean m_bVisible;
    public boolean m_bShowObj;
    public boolean m_bShowTip;
    public boolean m_bKeyQuery;


    public boolean m_bHide;

    public boolean m_bSpotDynamic;
    public boolean m_bLineDynamic;
    public boolean m_bFillDynamic;
    public boolean m_bTypeDynamic;
    public boolean m_bHintDynamic;
    public boolean m_bPivot;

    public Spot m_Spot = null;
    public Line m_Line = null;
    public Fill m_Fill = null;
    public Type m_Type = null;
    public Hint m_Hint = null;

    Vector<Obj> vectorObj = new Vector<Obj>();

    public Vector<PointTip> m_pointTiplist = new Vector<PointTip>();
    public Vector<FixedTip> m_fixedTiplist = new Vector<FixedTip>();
    public Vector<RectTip> m_rectTiplist = new Vector<RectTip>();
    public Vector<GroupTip> m_groupTiplist = new Vector<GroupTip>();

    public Layer()
    {
        m_bShowObj = true;
    }

    public void Serialize(DataInputStream dataStream)
    {
        try
        {
            m_wID = dataStream.readShort();
            m_wParentID = dataStream.readShort();
            m_strName = dataStream.readUTF();
            m_lowerObj = dataStream.readShort();
            m_upperObj = dataStream.readShort();
            m_lowerTip = dataStream.readShort();
            m_upperTip = dataStream.readShort(); ;

            short wSwitch = dataStream.readShort();
            m_bVisible = (wSwitch & 0X0001) == 0X0001 ? true : false;
            m_bShowTip = (wSwitch & 0X0002) == 0X0002 ? true : false;
            m_bKeyQuery = (wSwitch & 0X0004) == 0X0004 ? true : false;
            m_bHide = (wSwitch & 0X0020) == 0X0020 ? true : false;
            m_bSpotDynamic = (wSwitch & 0X0040) == 0X0040 ? true : false;
            m_bLineDynamic = (wSwitch & 0X0080) == 0X0080 ? true : false;
            m_bFillDynamic = (wSwitch & 0X0100) == 0X0100 ? true : false;
            m_bTypeDynamic = (wSwitch & 0X0200) == 0X0200 ? true : false;
            m_bPivot = (wSwitch & 0X0400) == 0X0400 ? true : false;

            byte lineindex = -1;
            byte fillindex = -1;
            byte spotindex = -1;
            if ((wSwitch & 0X0800) == 0X0800)
            {
                lineindex = dataStream.readByte();
            }
            if ((wSwitch & 0X1000) == 0X1000)
            {
                fillindex = dataStream.readByte();
            }
            if ((wSwitch & 0X2000) == 0X2000)
            {
                spotindex = dataStream.readByte();
            }

            if (m_Line != null)
            {
                m_Line = null;
            }
            if (m_Fill != null)
            {
                m_Fill = null;
            }
            if (m_Spot != null)
            {
                m_Spot = null;
            }
            if (m_Type != null)
            {
                m_Type = null;
            }
            if (m_Hint != null)
            {
                m_Hint = null;
            }
            m_Line = Line.FromType(lineindex);
            m_Fill = Fill.FromType(fillindex);
            m_Spot = Spot.FromType(spotindex);

            if ((wSwitch & 0X4000) == 0X4000)
            {
                m_Type = new Type();
            }
            if ((wSwitch & 0X8000) == 0X8000)
            {
                m_Hint = new Hint();
            }

            if (m_strName.equalsIgnoreCase("Guide") == true)
            {
                m_bVisible = false;
                m_bHide = true;
            }
        }
        catch (IOException ioe)
        {
            System.out.println("error when read Layer!");
        }

        if (m_Line != null)
        {
            m_Line.Serialize(dataStream);
        }
        if (m_Fill != null)
        {
            m_Fill.Serialize(dataStream);
        }
        if (m_Spot != null)
        {
            m_Spot.Serialize(dataStream);
        }
        if (m_Type != null)
        {
            m_Type.Serialize(dataStream);
        }
        if (m_Hint != null)
        {
            m_Hint.Serialize(dataStream);
        }

        try
        {
            vectorObj.removeAllElements();

            int count = dataStream.readInt();
            for (int index = 0; index < count; index++)
            {
                byte type = dataStream.readByte();
                Obj obj = Obj.FromType(type);
                if (obj != null)
                {
                    obj.Serialize(dataStream);
                    vectorObj.addElement(obj);
                }
            }
        }
        catch (IOException ioe)
        {
            System.out.println("error when layer's objlist!");
        }
    }

    public void Draw(Graphics2D g2, Library library, Rectangle inRect, short uZoom)
    {
        if (m_bVisible == false)
        {
            return;
        }

        boolean bDrawObj = (m_bShowObj == false || uZoom < m_lowerObj || uZoom > m_upperObj) ? false : true;
        boolean bDrawTip = (m_bShowTip == false || uZoom < m_lowerTip || uZoom > m_upperTip) ? false : true;
        if (bDrawObj == false && bDrawTip == false)
        {
            return;
        }

        float scaleMark = m_bSpotDynamic == true ? 1.0f : 100.0f / uZoom;
        float scaleLine = m_bLineDynamic == true ? 1.0f : 100.0f / uZoom;
        float scaleType = m_bTypeDynamic == true ? 1.0f : 100.0f / uZoom;

        Color penColor = null;
        Stroke pen = null;
        if (m_Line != null)
        {
            penColor = m_Line.GetColor();
            pen = m_Line.GetPen(scaleLine);
        }

        Paint brush = null;
        if (m_Fill != null)
        {
            brush = m_Fill.GetBrush(library);
        }

        Font font = null;
        Paint textbrush = null;
        if (m_Type != null)
        {
            font = m_Type.GetFont(scaleType);
            Color textColor = m_Type.GetColor();
            textbrush = textColor;
        }
        else
        {
            int size = 200;
            size *= scaleType;

            font = new Font("新宋体", Font.PLAIN, size);
            textbrush = new Color(0, 0, 0);
        }

        for (int index = 0; index < vectorObj.size(); index++)
        {
            Obj obj = vectorObj.elementAt(index);
            if (obj == null)
            {
                continue;
            }

            Rectangle rect = obj.m_Rect;
            if (rect.intersects(inRect) == false)
            {
                continue;
            }

            switch (obj.m_type)
            {
                case 8:
                {
                    Mark mark = (Mark) obj;
                    Spot pSpot = mark.m_Spot != null ? mark.m_Spot : m_Spot;
                    if (pSpot != null)
                    {
                        if (bDrawObj == true)
                        {
                            pSpot.Draw(g2, library, mark.m_Center, mark.m_fSin, mark.m_fCos, scaleMark);

                            Rectangle markRect = pSpot.GetRect(library, scaleMark);
                            markRect.translate(mark.m_Center.x, mark.m_Center.x);
                            Mapeasy.Obj.Share.d_oRectArray.addElement(markRect);
                        }

                        if (bDrawTip == true && obj.m_string.length() != 0)
                        {
                            Rectangle markRect = pSpot.GetRect(library, scaleMark);
                            markRect.translate(mark.m_Center.x, mark.m_Center.y);

                            RectTip pTip = new RectTip();
                            pTip.m_rect = markRect;
                            pTip.m_string = obj.m_string;
                            m_rectTiplist.addElement(pTip);
                        }
                    }
                }
                break;
                case 9:
                {
                    if (bDrawObj == true)
                    {
                        Text text = (Text) obj;
                        text.Draw(g2, font, textbrush, scaleType);
                    }
                }
                break;
                case 10:
                {
                    if (bDrawObj == true)
                    {
                        TextPoly textpoly = (TextPoly) obj;
                        textpoly.Draw(g2, font, textbrush, scaleType);
                    }
                }
                break;
                case 1:
                case 2:
                case 7:
                case 12:
                case 13:
                {
                    if (bDrawObj == true)
                    {
                        Line lineSymbol = null;
                        if (m_Line != null)
                        {
                            byte type = m_Line.Gettype();
                            if (type == 2 || type == 3)
                            {
                                lineSymbol = m_Line;
                            }
                        }
                        if (pen != null || (obj.m_bClosed == true && brush != null))
                        {
                            obj.Draw(g2, pen, penColor, brush, library, scaleLine);
                        }
                        if (lineSymbol != null)
                        {
                            lineSymbol.Draw(g2, library, obj, scaleLine);
                            lineSymbol = null;
                        }
                    }

                    if (bDrawTip == true && obj.m_string.length() != 0)
                    {
                        GroupTip pTip = GroupTip.GetGroup(m_groupTiplist, obj);
                        if (pTip == null)
                        {
                            pTip = new GroupTip();
                            pTip.m_string = obj.m_string;
                            pTip.m_bDelete = false;
                            pTip.m_objlist = new Vector<Obj>();
                            m_groupTiplist.addElement(pTip);
                        }
                        pTip.m_objlist.addElement(obj);
                    }
                }
                break;
                default:
                {
                    if (bDrawObj == true)
                    {
                        obj.Draw(g2, pen, penColor, brush, library, 1.0f);
                    }
                    if (bDrawTip == true && obj.m_string.length() != 0)
                    {
                        PointTip pTip = new PointTip();
                        pTip.m_point = obj.CenterPoint();
                        pTip.m_string = obj.m_string;
                        m_pointTiplist.addElement(pTip);
                    }
                }
                break;
            }
        }
    }

    public void DrawTip(Graphics2D g2, Library library, Rectangle inRect, short uZoom, long nDecimal)
    {
        if (m_bVisible == false)
        {
            return;
        }
        if (m_bShowTip == false)
        {
            return;
        }
        if (uZoom < m_lowerTip)
        {
            return;
        }
        if (uZoom > m_upperTip)
        {
            return;
        }

        float scaleMark = m_bSpotDynamic == true ? nDecimal / 10.0f : 10.0f * nDecimal / uZoom;
        float scaleHint = m_bHintDynamic == true ? nDecimal : 100.0f * nDecimal / uZoom;

        byte cDrawFrame = 0X00;

        Color rimColor = null;
        Stroke rimPen = null;
        Paint rimBrush = null;
        Font font = null;
        Color textColor = null;
        if (m_Hint != null)
        {
            cDrawFrame = m_Hint.m_cFrame;

            font = m_Hint.GetFont(scaleHint);
            textColor = m_Hint.GetTextColor();
            rimPen = m_Hint.GetRimPen(scaleHint);
            rimColor = m_Hint.GetRimColor();
            rimBrush = m_Hint.GetRimBrush();
        }
        else
        {
            int size = (int) (9 * scaleHint);

            String sFont = "新宋体";
            font = new Font(sFont, Font.PLAIN, size);
            textColor = new Color(0, 0, 0);
        }

        boolean roundRect = ((cDrawFrame & 0X04) == 0X04);

        for (int index = 0; index < m_groupTiplist.size(); index++)
        {
            GroupTip pTip = m_groupTiplist.elementAt(index);
            if (pTip != null)
            {
                pTip.Draw(g2, font, textColor, rimPen, rimColor, rimBrush, roundRect, m_bPivot);
                pTip = null;
            }
        }
        m_groupTiplist.removeAllElements();

        for (int index = 0; index < m_pointTiplist.size(); index++)
        {
            PointTip pTip = m_pointTiplist.elementAt(index);
            if (pTip != null)
            {
                pTip.Draw(g2, font, textColor, rimPen, rimColor, rimBrush, roundRect);
                pTip = null;
            }
        }
        m_pointTiplist.removeAllElements();

        for (int index = 0; index < m_fixedTiplist.size(); index++)
        {
            FixedTip pTip = m_fixedTiplist.elementAt(index);
            if (pTip != null)
            {
                pTip.Draw(g2, font, textColor, rimPen, rimColor, rimBrush, roundRect);
                pTip = null;
            }
        }
        m_fixedTiplist.removeAllElements();

        for (int index = 0; index < m_rectTiplist.size(); index++)
        {
            RectTip pTip = m_rectTiplist.elementAt(index);
            if (pTip != null)
            {
                pTip.Draw(g2, font, textColor, rimPen, rimColor, rimBrush, roundRect);
                pTip = null;
            }
        }
        m_rectTiplist.removeAllElements();
    }
}

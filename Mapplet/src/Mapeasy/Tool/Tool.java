package Mapeasy.Tool;

import java.awt.Image;
import java.awt.Point;
import java.awt.image.BufferedImage;

import Mapeasy.Mapeasy;
import java.awt.Cursor;

public class Tool
{
    Cursor cursor = null;

    public int downX;
    public int downY;
    public Mapeasy applet = null;

    public static Tool zoominTool = new ZoomInTool();
    public static Tool zoomoutTool = new ZoomOutTool();
    public static Tool panTool = new PanTool();
    public static Tool queryTool = new QueryTool();

    public void setCursor()
    {
        if(cursor != null)
        {
            applet.setCursor(cursor);
        }
    }

    public void MouseClick(Point pt)
    {
    }

    public void LButtonDown(Point pt)
    {
    }

    public void LButtonDown()
    {
    }

    public void LButtonUp(Point pt)
    {
    }

    public void LButtonUp()
    {
    }


    public void RButtonDown()
    {
    }

    public void MouseDrag(Point pt)
    {
    }
}

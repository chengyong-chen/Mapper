package Mapeasy.Tool;

import java.awt.Cursor;
import java.awt.Point;
import java.awt.Dimension;
import java.awt.Rectangle;
import java.awt.Image.*;
import java.awt.image.BufferedImage;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Robot;

/**
 * <p>Title: 移动工具</p>
 *
 * <p>Description: </p>
 *
 * <p>Copyright: Copyright (c) 2006</p>
 *
 * <p>Company: </p>
 *
 * @author not attributable
 * @version 1.0
 */
public class PanTool extends Tool
{
    public Robot robot = null; //new Robot();
    public BufferedImage mapImage = null;

    public PanTool()
    {
        cursor = Cursor.getPredefinedCursor(Cursor.MOVE_CURSOR);
        try
        {
            robot = new Robot();
        }
        catch (Exception e)
        {
            throw new RuntimeException("unable to construct Robot");
        }
    }

    public void MouseDrag(Point pt)
    {
        if (mapImage == null)
        {
            return;
        }
        int deltaX = pt.x - downX;
        int deltaY = pt.y - downY;

        //将地图移走的地方刷白
        if (deltaX < 0)
        {
            applet.getGraphics().clearRect(applet.getWidth() - Math.abs(deltaX), 0, Math.abs(deltaX), applet.getHeight());
        }
        if (deltaY < 0)
        {
            applet.getGraphics().clearRect(0, applet.getHeight() - Math.abs(deltaY), applet.getWidth(), Math.abs(deltaY));
        }
        if (deltaX > 0)
        {
            applet.getGraphics().clearRect(0, 0, Math.abs(deltaX), applet.getHeight());
        }
        if (deltaY > 0)
        {
            applet.getGraphics().clearRect(0, 0, applet.getWidth(), Math.abs(deltaY));
        }

        applet.getGraphics().drawImage(mapImage, deltaX, deltaY, applet);
        applet.getGraphics().dispose();
    }

    public void LButtonUp(Point pt)
    {
        if (mapImage != null)
        {
            mapImage = null;
        }
        applet.m_ptViewPos.x -= pt.x - downX;
        applet.m_ptViewPos.y -= pt.y - downY;
        if ((applet.m_sizeView.width - applet.m_ptViewPos.x) < applet.getWidth())
        {
            applet.m_ptViewPos.x = applet.m_sizeView.width - applet.getWidth();
        }
        if (applet.m_ptViewPos.x < 0)
        {
            applet.m_ptViewPos.x = 0;
        }
        if ((applet.m_sizeView.height - applet.m_ptViewPos.y) < applet.getHeight())
        {
            applet.m_ptViewPos.y = applet.m_sizeView.height - applet.getHeight();
        }
        if (applet.m_ptViewPos.y < 0)
        {
            applet.m_ptViewPos.y = 0;
        }
        if ((pt.x - downX) != 0 || (pt.y - downY) != 0)
        {
            applet.repaint();
        }
    }

    public void LButtonDown(Point pt)
    {
        downX = pt.x;
        downY = pt.y;
        if (mapImage != null)
        {
            mapImage = null;
        }

        Dimension size = applet.getSize();
        Rectangle rect = new Rectangle(applet.getLocationOnScreen().x, applet.getLocationOnScreen().y, size.width, size.height);
        try
        {
            mapImage = robot.createScreenCapture(rect);
        }
        catch (Exception e)
        {
            System.out.println(e.toString());
        }
    }
}

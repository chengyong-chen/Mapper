package Mapeasy.Tool;

import java.awt.*;
import java.net.URL;


/**
 * <p>Title: 放大工具</p>
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
public class ZoomInTool extends Tool
{
    public ZoomInTool()
    {
        try
        {
            Image image = Toolkit.getDefaultToolkit().getImage(new URL("http://localhost:8080/ChinaMap/image/zoomin2.gif"));
            if (image != null)
            {
                cursor = Toolkit.getDefaultToolkit().createCustomCursor(image, new Point(8, 8), null);
            }
        }
        catch (Exception e)
        {
        }
    }

    public void MouseClick(Point pt)
    {
        applet.ZoomIn(pt);
    }
}

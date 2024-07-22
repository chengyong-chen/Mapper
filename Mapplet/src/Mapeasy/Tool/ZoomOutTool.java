package Mapeasy.Tool;


import java.awt.*;
import java.net.URL;


/**
 * <p>Title: Àı–°π§æﬂ</p>
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
public class ZoomOutTool extends Tool
{
    public ZoomOutTool()
    {
        try
       {
           Image image = Toolkit.getDefaultToolkit().getImage(new URL("http://localhost:8080/ChinaMap/image/zoomout2.gif"));
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
        if(applet != null)
        {
            applet.ZoomOut(pt);
        }
    }
}

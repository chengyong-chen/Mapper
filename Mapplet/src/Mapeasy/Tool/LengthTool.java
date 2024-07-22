package Mapeasy.Tool;

import java.net.URL;
import java.awt.Toolkit;
import java.awt.Point;
import java.awt.Image;

/**
 * <p>Title: </p>
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
public class LengthTool extends Tool
{
    public LengthTool()
    {
        try
        {
            Image image = Toolkit.getDefaultToolkit().getImage(new URL("http://localhost:8080/ChinaMap/image/lengthTool.gif"));
            if (image != null)
            {
                cursor = Toolkit.getDefaultToolkit().createCustomCursor(image, new Point(8, 8), null);
            }
        }
        catch (Exception e)
        {
        }

    }
}

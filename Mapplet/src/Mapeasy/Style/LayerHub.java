package Mapeasy.Style;

import java.io.DataInputStream;
import java.util.Vector;
import Mapeasy.Obj.Obj;
import java.io.IOException;

public class LayerHub
{
    public Vector<Layer> vectorLayer = new Vector<Layer>();

    public LayerHub()
    {
    }

    public void RemoveAll()
    {
    }

    public void Serialize(DataInputStream dataStream)
    {
        try
        {
            vectorLayer.removeAllElements();

            short count = dataStream.readShort();
            for(short index=0;index<count;index++)
            {
                Layer layer = new Layer();
                layer.Serialize(dataStream);

                vectorLayer.addElement(layer);
            }
        }
        catch (IOException ioe)
        {
            System.out.println("error when read LayerHub!");
        }
    }

}

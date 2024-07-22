package Mapeasy.Project;

import java.io.DataInputStream;
import java.io.IOException;

public class Gauss extends Peking54
{
    public double m_longitude0;

    public Gauss()
    {
    }

    public void Serialize(DataInputStream dataStream)
    {
        super.Serialize(dataStream);

        try
        {
            m_longitude0 = dataStream.readDouble();
        }
        catch (IOException ioe)
        {
            System.out.println("error when read Gauss project!");
        }

    }

}

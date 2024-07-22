package Mapeasy.Style;


import java.awt.Color;
import java.io.IOException;
import java.io.DataInputStream;

public class ColourTint extends Colour
{
    public byte m_bT;

    public ColourTint()
    {
    }

    public void Serialize(DataInputStream dataStream)
    {
        super.Serialize(dataStream);

        try
        {
            m_bT = dataStream.readByte();
        }
        catch (IOException ioe)
        {
            System.out.println("error when Process colour Fill!");
        }
    }
    public Colour Clone()
    {
            ColourTint pClone =  new ColourTint();

            pClone.m_bID = m_bID;
            pClone.alpha = alpha;

            pClone.m_bT = m_bT;

            return pClone;
}
    public Color GetColor()
    {
        return new Color(0, 0, 0,alpha);
    }

}

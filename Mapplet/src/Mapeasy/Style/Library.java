package Mapeasy.Style;

import java.io.DataInputStream;

public class Library
{
    public PatternLib m_libPattern = new PatternLib();
    public SymbolLib m_libLineSymbol = new SymbolLib();
    public SymbolLib m_libFillSymbol = new SymbolLib();
    public SymbolLib m_libSpotSymbol = new SymbolLib();

    public Library()
    {
    }

    public void RemoveAll()
    {
    }

    public void Serialize(DataInputStream dataStream)
    {
        m_libPattern.Serialize(dataStream);
        m_libLineSymbol.Serialize(dataStream);
        m_libFillSymbol.Serialize(dataStream);
        m_libSpotSymbol.Serialize(dataStream);
    }
}

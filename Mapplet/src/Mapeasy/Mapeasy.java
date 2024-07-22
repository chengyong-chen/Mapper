package Mapeasy;

import java.applet.Applet;
import java.awt.Point;
import java.awt.Dimension;
import java.awt.Rectangle;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.BorderLayout;
import java.awt.Toolkit;
import java.awt.Color;
import java.awt.Image;
import java.awt.event.*;
import java.awt.image.BufferedImage;
import java.awt.geom.*;
import java.net.URL;
import java.util.Vector;
import java.net.*;
import java.io.*;

import netscape.javascript.JSException;
import netscape.javascript.JSObject;

import Mapeasy.Obj.Obj;
import Mapeasy.Obj.Share;
import Mapeasy.Tool.Tool;
import Mapeasy.Public.Paper;
import Mapeasy.Public.MapInfo;
import Mapeasy.Public.Atlas;
import Mapeasy.Project.Project;
import Mapeasy.Style.Library;
import Mapeasy.Style.Layer;
import Mapeasy.Style.LayerHub;

public class Mapeasy extends Applet //implements ActionListener
{
    public Atlas m_pAtlas = null;
    public int m_wMapID = 0;
    public Paper m_pPaper = new Paper();
    public Library m_library = new Library();
    public LayerHub m_layerHub = new LayerHub();
    public MapInfo m_MapInfo = new MapInfo();
    public Project m_Project = null;

    public Dimension m_sizeWindow = new Dimension(0, 0);
    public Dimension m_sizeView = new Dimension(0, 0);
    public Point m_ptViewPos = new Point(0, 0);
    protected short m_uZoom = 100;

    protected short m_nAngle = 0;

    protected AffineTransform matrixDoctoWin = new AffineTransform();
    protected AffineTransform matrixDoctoCli = new AffineTransform();
    protected AffineTransform matrixWintoDoc = new AffineTransform();
    protected AffineTransform matrixClitoDoc = new AffineTransform();
    protected AffineTransform matrixMaptoDoc = new AffineTransform();

    public Tool tool = null;

    //用于调用javascript的变量
    public JSObject parentJSP = null;
    public JSObject docJSO = null;

    //Get a parameter value
    public String getParameter(String key, String def)
    {
        return getParameter(key) != null ? getParameter(key) : def;
    }

    //Construct the applet
    public Mapeasy()
    {
    }

    //Initialize the applet
    public void init()
    {
        setLayout(new BorderLayout()); //设定整个Applet的布局策略为BorderLayout
        resize(this.getWidth(), this.getHeight());

        m_sizeWindow.width = this.getWidth();
        m_sizeWindow.height = this.getHeight();

        /**鼠标动作监听**/
        addMouseListener(new MouseAdapter()
        {
            public void mouseClicked(MouseEvent e)
            {
                if (tool != null)
                {
                    tool.MouseClick(new Point(e.getX(), e.getY()));
                }
            }

            public void mouseReleased(MouseEvent e)
            {
                if (tool != null)
                {
                    tool.LButtonUp(new Point(e.getX(), e.getY()));
                }
            }

            public void mousePressed(MouseEvent e)
            {
                if (tool != null)
                {
                    tool.LButtonDown(new Point(e.getX(), e.getY()));
                }
            }
        });
        /**鼠标动作监听**/
        addMouseMotionListener(new MouseMotionAdapter()
        {
            public void mouseDragged(MouseEvent e)
            {
                if (tool != null)
                {
                    tool.MouseDrag(new Point(e.getX(), e.getY()));
                }
            }
        });

        try //与JavaScript通信
        {
            parentJSP = JSObject.getWindow(this); //获取JavaScript窗口句柄，引用当前文档窗口
            if (parentJSP != null)
            {
                docJSO = (JSObject) parentJSP.getMember("document"); //访问JavaScript对象
            }
        }
        catch (JSException e)
        {
            e.printStackTrace();
        }

        try
        {
            jbInit();
        }
        catch (Exception e)
        {
            e.printStackTrace();
        }
    }

    public void OpenAtlas(String mapUrl)
    {
    }

    public void CloseMap()
    {
        if (m_Project != null)
        {
            m_Project = null;
        }

        m_library.RemoveAll();
        m_layerHub.RemoveAll();

        m_ptViewPos = new Point(0, 0);
        m_sizeView = new Dimension(0, 0);
        m_uZoom = 0;
    }

    public void OpenMap(String mapUrl)
    {
        CloseMap();

        try
        {
            URL nurl = new URL(mapUrl);
            URLConnection conWis = nurl.openConnection();
            if (conWis == null)
            {
                return;
            }
            conWis.connect();
            InputStream inputStream = conWis.getInputStream();
            if (inputStream == null)
            {
                return;
            }
            DataInputStream dataStream = new DataInputStream(inputStream);

            if (m_MapInfo == null)
            {
                m_MapInfo = new MapInfo();
            }

            m_MapInfo.Serialize(dataStream);

            if (m_Project != null)
            {
                m_Project = null;
            }
            byte type = dataStream.readByte();
            m_Project = Project.FromType(type);
            if (m_Project != null)
            {
                m_Project.Serialize(dataStream);
            }

            m_pPaper.Serialize(dataStream);
            m_library.Serialize(dataStream);
            m_layerHub.Serialize(dataStream);

            dataStream.close();

            if (parentJSP != null)
            {
                parentJSP.eval("doInit();");
            }

            if (m_uZoom < m_pPaper.m_viewLower)
            {
                m_uZoom = m_pPaper.m_viewLower;
            }
            else if (m_uZoom > m_pPaper.m_viewUpper)
            {
                m_uZoom = m_pPaper.m_viewUpper;
            }

            int xDPI = 96; //72; //dc.GetDeviceCaps(LOGPIXELSX);
            int yDPI = 96; //72; //dc.GetDeviceCaps(LOGPIXELSY);

            int cx = m_pPaper.m_sizePaper.width;
            int cy = m_pPaper.m_sizePaper.height;
            if (m_nAngle != 0)
            {
                double fViewRotateSin = Math.sin(m_nAngle * 3.1415926535 / 180);
                double fViewRotateCos = Math.cos(m_nAngle * 3.1415926535 / 180);

                Point point1 = new Point();
                point1.x = (int) (fViewRotateCos * cx);
                point1.y = (int) (fViewRotateSin * cx);

                Point point2 = new Point();
                point2.x = (int) (fViewRotateCos * cx - fViewRotateSin * cy);
                point2.y = (int) (fViewRotateSin * cx + fViewRotateCos * cy);

                Point point3 = new Point();
                point3.x = (int) ( -fViewRotateSin * cy);
                point3.y = (int) (fViewRotateCos * cy);

                Rectangle rect = new Rectangle();
                rect.x = Math.min(Math.min(0, point1.x), Math.min(point2.x, point3.x));
                rect.y = Math.min(Math.min(0, point1.y), Math.min(point2.y, point3.y));
                rect.width = Math.max(Math.max(0, point1.x), Math.max(point2.x, point3.x)) - rect.x;
                rect.height = Math.max(Math.max(0, point1.y), Math.max(point2.y, point3.y)) - rect.y;

                cx = rect.width;
                cy = rect.height;
            }

            int cxView = (int) ((cx / 72.0f) * (xDPI * m_uZoom / 100.0f));
            int cyView = (int) ((cy / 72.0f) * (yDPI * m_uZoom / 100.0f));
            if (this.WIDTH > cxView && this.HEIGHT > cyView)
            {
                float xscale = (this.WIDTH * 72.0f) / (cx * xDPI);
                float yscale = (this.HEIGHT * 72.0f) / (cy * yDPI);

                float fZoom = Math.min(xscale, yscale);
                if ((int) (fZoom * 100) <= m_pPaper.m_viewUpper)
                {
                    m_uZoom = (short) (fZoom * 100);
                    m_pPaper.m_viewLower = m_uZoom;
                }
            }

            {
                int nDecimal = m_pPaper.GetDecimal();
                double m11 = m_MapInfo.m_fScale * nDecimal;
                double m22 = m_MapInfo.m_fScale * nDecimal;
                double dx = (double) (m_MapInfo.m_Origin.x * nDecimal);
                double dy = (double) (m_MapInfo.m_Origin.y * nDecimal);

                matrixMaptoDoc.setTransform(m11, 0.0d, 0.0d, m22, dx, dy);
            }

            this.SetViewSize(new Point(0, 0), new Point(0, 0));

            if (parentJSP != null)
            {
                parentJSP.eval("doInit();");
            }
        }
        catch (Exception e)
        {
            e.printStackTrace();
            System.out.println("找不到相关文件");
        }

        setTool(4);
    }

    public void setTool(int which)
    {
        switch (which)
        {
            case 2: //放大
                tool = Tool.zoominTool;
                break;
            case 3: //缩小
                tool = Tool.zoomoutTool;
                break;
            case 4: //漫游
                tool = Tool.panTool;
                break;
            case 5: //访问数据库
                tool = Tool.queryTool;
                break;
            case 6: //访问数据库

                //           tool = Tool.lengthTool;
                break;
            default:
                tool = null;
                break;
        }

        if (tool != null)
        {
            tool.applet = this;
            tool.setCursor();
        }
    }

    private void jbInit() //Component initialization
            throws Exception
    {
    }

    public void start()
    {
        OpenMap("http://localhost:8080/ChinaMap/hrb.Wis");
    }

    public void ClientToDoc(Point point)
    {
        Point point1 = new Point(point.x, point.y);

        matrixClitoDoc.transform(point1, point1);

        point.x = point1.x;
        point.y = point1.y;
    }

    public void ClientToDoc(Dimension size)
    {
        Point point1 = new Point(0, 0);
        Point point2 = new Point(size.width, size.height);

        matrixClitoDoc.transform(point1, point1);
        matrixClitoDoc.transform(point2, point2);

        size.width = point2.x - point1.x;
        size.height = point2.y - point1.y;
    }

    public void ClientToDoc(Rectangle rect)
    {
        Point point1 = new Point((int) rect.getMinX(), (int) rect.getMinY());
        Point point2 = new Point((int) rect.getMaxX(), (int) rect.getMinY());
        Point point3 = new Point((int) rect.getMaxX(), (int) rect.getMaxY());
        Point point4 = new Point((int) rect.getMinX(), (int) rect.getMaxY());

        matrixClitoDoc.transform(point1, point1);
        matrixClitoDoc.transform(point2, point2);
        matrixClitoDoc.transform(point3, point3);
        matrixClitoDoc.transform(point4, point4);

        rect.x = Math.min(Math.min(point1.x, point2.x), Math.min(point3.x, point4.x));
        rect.y = Math.min(Math.min(point1.y, point2.y), Math.min(point3.y, point4.y));
        rect.width = Math.max(Math.max(point1.x, point2.x), Math.max(point3.x, point4.x)) - rect.x;
        rect.height = Math.max(Math.max(point1.y, point2.y), Math.max(point3.y, point4.y)) - rect.y;
    }

    public void WindowToDoc(Rectangle rect)
    {
        Point point1 = new Point((int) rect.getMinX(), (int) rect.getMinY());
        Point point2 = new Point((int) rect.getMaxX(), (int) rect.getMaxY());

        matrixWintoDoc.transform(point1, point1);
        matrixWintoDoc.transform(point2, point2);

        rect.x = point1.x;
        rect.y = point1.y;
        rect.width = point2.x - point1.x;
        rect.height = point2.y - point1.y;
    }

    public void DocToMap(Point docPoint, Point2D.Double mapPoint)
    {
        long nDecimal = m_pPaper.GetDecimal();

        mapPoint = m_MapInfo.DocToMap(docPoint, nDecimal);
    }

    public void MapToGeo(Point2D.Double mapPoint, Point2D.Double geoPoint)
    {
        if (m_Project != null)
        {
            double z = 0;
            m_Project.MaptoGeo(mapPoint.x, mapPoint.y, 0, geoPoint.x, geoPoint.y, z);
        }
        else
        {
            geoPoint.x = mapPoint.x;
            geoPoint.y = mapPoint.y;
        }
    }


    public void GeoToMap(Point2D.Double geoPoint, Point2D.Double mapPoint)
    {
        if (m_Project != null)
        {
            double Z = 0;
            Point2D.Double point = geoPoint;
            m_Project.GeotoMap(point.x, point.y, 0, mapPoint.x, mapPoint.y, Z);
        }
        else
        {
            mapPoint.x = geoPoint.x;
            mapPoint.y = geoPoint.y;
        }
    }

    public void MapToDoc(Point2D.Double mapPoint, Point docPoint)
    {
        long nDecimal = m_pPaper.GetDecimal();

        docPoint = m_MapInfo.MapToDoc(mapPoint, nDecimal);
    }

    public void DocToClient(Point point)
    {
        Point point1 = new Point(point.x, point.y);

        matrixDoctoCli.transform(point1, point1);

        point.x = point1.x;
        point.y = point1.y;
    }

    public void DocToClient(Dimension size)
    {
        Point point1 = new Point(0, 0);
        Point point2 = new Point(size.width, size.height);

        matrixDoctoCli.transform(point1, point1);
        matrixDoctoCli.transform(point2, point2);

        size.width = Math.abs(point2.x - point1.x);
        size.height = Math.abs(point2.y - point1.y);
    }

    public void DocToClient(Rectangle rect)
    {
        Point point1 = new Point((int) rect.getMinX(), (int) rect.getMinY());
        Point point2 = new Point((int) rect.getMaxX(), (int) rect.getMinY());
        Point point3 = new Point((int) rect.getMaxX(), (int) rect.getMaxY());
        Point point4 = new Point((int) rect.getMinX(), (int) rect.getMaxY());

        matrixDoctoCli.transform(point1, point1);
        matrixDoctoCli.transform(point2, point2);
        matrixDoctoCli.transform(point3, point3);
        matrixDoctoCli.transform(point4, point4);

        rect.x = Math.min(Math.min(point1.x, point2.x), Math.min(point3.x, point4.x));
        rect.y = Math.min(Math.min(point1.y, point2.y), Math.min(point3.y, point4.y));
        rect.width = Math.max(Math.max(point1.x, point2.x), Math.max(point3.x, point4.x)) - rect.x;
        rect.height = Math.max(Math.max(point1.y, point2.y), Math.max(point3.y, point4.y)) - rect.y;
    }

    public void paint(Graphics g)
    {
        if (Share.d_oRectArray != null)
        {
            Share.d_oRectArray.removeAllElements(); ;
        }
        Rectangle rect = g.getClipBounds();
        if (rect.isEmpty() == true)
        {
            return;
        }

        PrepareMatrix();

        Rectangle inRect = new Rectangle(this.getBounds());
        ClientToDoc(inRect);

        BufferedImage ImBuffer = (BufferedImage) createImage(getWidth(), getHeight());
        Graphics2D g2 = ImBuffer.createGraphics();
        g2.setRenderingHint(java.awt.RenderingHints.KEY_ANTIALIASING, java.awt.RenderingHints.VALUE_ANTIALIAS_ON); //抗锯齿
        g2.setRenderingHint(java.awt.RenderingHints.KEY_DITHERING, java.awt.RenderingHints.VALUE_DITHER_ENABLE); //抗抖动
        g2.setTransform(matrixDoctoCli);
        g2.setBackground(Color.white);
        //    g2.clearRect(inRect.x, inRect.y, inRect.width, inRect.height);

        long nDecimal = m_pPaper.GetDecimal();
        int size = m_layerHub.vectorLayer.size();
        for (int index = size - 1; index >= 0; index--)
        {
            Layer layer = m_layerHub.vectorLayer.elementAt(index);
            if (layer == null)
            {
                continue;
            }

            layer.Draw(g2, m_library, inRect, m_uZoom);
        }
        for (int index = 0; index < size; index++)
        {
            Layer layer = m_layerHub.vectorLayer.elementAt(index);
            if (layer == null)
            {
                continue;
            }

            layer.DrawTip(g2, m_library, inRect, m_uZoom, nDecimal);
        }

        g2.dispose();
        g.drawImage(ImBuffer, 0, 0, this);
        g.dispose();

        if (parentJSP != null)
        {
            parentJSP.eval("boxPan();");
        }
    }

    public void SetViewSize(Point docPoint, Point cliPoint)
    {
        Toolkit kit = Toolkit.getDefaultToolkit();
        int xDPI = kit.getScreenResolution();
        int yDPI = kit.getScreenResolution();

        int nDecimal = m_pPaper.GetDecimal();
        Rectangle rect = new Rectangle(0, 0, (int) (m_pPaper.m_sizePaper.width * nDecimal), (int) (m_pPaper.m_sizePaper.height * nDecimal));
        double fViewRotateSin = Math.sin(m_nAngle * 3.1415926535 / 180);
        double fViewRotateCos = Math.cos(m_nAngle * 3.1415926535 / 180);
        if (m_nAngle != 0)
        {
            int cx = rect.width;
            int cy = rect.height;
            Point point1 = new Point();
            point1.x = (int) (fViewRotateCos * cx);
            point1.y = (int) (fViewRotateSin * cx);

            Point point2 = new Point();
            point2.x = (int) (fViewRotateCos * cx - fViewRotateSin * cy);
            point2.y = (int) (fViewRotateSin * cx + fViewRotateCos * cy);

            Point point3 = new Point();
            point3.x = (int) ( -fViewRotateSin * cy);
            point3.y = (int) (fViewRotateCos * cy);

            rect.x = Math.min(Math.min(0, point1.x), Math.min(point2.x, point3.x));
            rect.y = Math.max(Math.min(0, point1.y), Math.min(point2.y, point3.y));
            rect.width = Math.max(Math.max(0, point1.x), Math.max(point2.x, point3.x)) - rect.x;
            rect.height = Math.max(Math.max(0, point1.y), Math.max(point2.y, point3.y)) - rect.y;
        }

        {
            float fScale = ((m_uZoom / 100.0f) * xDPI) / (72.0f * nDecimal);

            double m11 = fScale * fViewRotateCos;
            double m21 = -fScale * fViewRotateSin;
            double m12 = -fScale * fViewRotateSin;
            double m22 = -fScale * fViewRotateCos;

            double dx = -rect.getMinX() * fScale;
            double dy = rect.getMaxY() * fScale;
            matrixDoctoWin.setTransform(m11, m21, m12, m22, dx, dy);
        }

        {
            double fScale = (72.0f * nDecimal) / ((m_uZoom / 100.0f) * xDPI);

            double m11 = fScale * fViewRotateCos;
            double m21 = -fScale * fViewRotateSin;
            double m12 = -fScale * fViewRotateSin;
            double m22 = -fScale * fViewRotateCos;

            double dx = rect.getMinX() * fViewRotateCos + rect.getMaxY() * fViewRotateSin;
            double dy = -rect.getMinX() * fViewRotateSin + rect.getMaxY() * fViewRotateCos;
            matrixWintoDoc.setTransform(m11, m21, m12, m22, dx, dy);
        }

        m_ptViewPos = new Point(0, 0);

        float fZoom = (float) m_uZoom / 100;
        m_sizeView.width = (int) ((rect.width / (72.0f * nDecimal)) * (xDPI * fZoom));
        m_sizeView.height = (int) ((rect.height / (72.0f * nDecimal)) * (yDPI * fZoom));

        if (m_sizeView.width < this.getWidth())
        {
            m_ptViewPos.x = (m_sizeView.width - this.getWidth()) / 2;
        }
        else
        {
            m_ptViewPos.x = 0;
        }

        if (m_sizeView.height < this.getHeight())
        {
            m_ptViewPos.y = (m_sizeView.height - this.getHeight()) / 2;
        }
        else
        {
            m_ptViewPos.y = 0;
        }

        if (docPoint.equals(new Point(0, 0)) == true)
        {
            docPoint.x = m_pPaper.m_sizePaper.width * nDecimal / 2;
            docPoint.y = m_pPaper.m_sizePaper.height * nDecimal / 2;
        }
        if (cliPoint.equals(new Point(0, 0)) == true)
        {
            cliPoint.x = this.getWidth() / 2;
            cliPoint.y = this.getHeight() / 2;
        }

        Point point = new Point(docPoint.x, docPoint.y);
        matrixDoctoWin.transform(point, point);

        Dimension sizeScroll = new Dimension();
        sizeScroll.width = point.x - cliPoint.x - m_ptViewPos.x;
        sizeScroll.height = point.y - cliPoint.y - m_ptViewPos.y;
        this.OnScrollBy(sizeScroll, true);
    }

    public boolean OnScrollBy(Dimension sizeScroll, boolean bDoScroll)
    {
        int xOrig, x;
        int yOrig, y;

        // adjust current x position
        xOrig = x = m_ptViewPos.x;
        int xMax = m_sizeView.width - this.getWidth() + 1;
        if (xMax > 0)
        {
            x += sizeScroll.width;
            if (x < 0)
            {
                x = 0;
            }
            else if (x > xMax)
            {
                x = xMax;
            }
        }

        // adjust current y position
        yOrig = y = m_ptViewPos.y;
        int yMax = m_sizeView.height - this.getHeight() + 1;
        if (yMax > 0)
        {
            y += sizeScroll.height;
            if (y < 0)
            {
                y = 0;
            }
            else if (y > yMax)
            {
                y = yMax;
            }
        }

        if (bDoScroll == true)
        {
        }
        // did anything change?
        if (x != xOrig || y != yOrig)
        {
            m_ptViewPos.x = x;
            m_ptViewPos.y = y;
        }

        if (bDoScroll == true)
        {
            repaint();
        }

        if (sizeScroll.height != 0)
        {
        }

//	if(Tool != NULL)
        {
        }

        if (x != xOrig || y != yOrig)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    public void PrepareMatrix()
    {
        if (true)
        {
            if (m_sizeView.width < this.getWidth())
            {
                m_ptViewPos.x = -(this.getWidth() - m_sizeView.width) / 2;
            }
            if (m_sizeView.height < this.getHeight())
            {
                m_ptViewPos.y = -(this.getHeight() - m_sizeView.height) / 2;
            }
        }

        int yDPI = 96; //72;
        int xDPI = 96; //72;

        int nDecimal = m_pPaper.GetDecimal();
        Rectangle rect = new Rectangle(0, 0, m_pPaper.m_sizePaper.width * nDecimal, m_pPaper.m_sizePaper.height * nDecimal);

        double fViewRotateSin = 0.0f;
        double fViewRotateCos = 1.0f;
        if (m_nAngle != 0)
        {
            fViewRotateSin = Math.sin(m_nAngle * 3.1415926535 / 180);
            fViewRotateCos = Math.cos(m_nAngle * 3.1415926535 / 180);

            Point point1 = new Point();
            point1.x = (int) (fViewRotateCos * rect.width);
            point1.y = (int) (fViewRotateSin * rect.width);

            Point point2 = new Point();
            point2.x = (int) (fViewRotateCos * rect.width - fViewRotateSin * rect.height);
            point2.y = (int) (fViewRotateSin * rect.width + fViewRotateCos * rect.height);

            Point point3 = new Point();
            point3.x = (int) ( -fViewRotateSin * rect.height);
            point3.y = (int) (fViewRotateCos * rect.height);

            rect.x = Math.min(Math.min(0, point1.x), Math.min(point2.x, point3.x));
            rect.y = Math.min(Math.min(0, point1.y), Math.min(point2.y, point3.y));
            rect.width = Math.max(Math.max(0, point1.x), Math.max(point2.x, point3.x)) - rect.x;
            rect.height = Math.max(Math.max(0, point1.y), Math.max(point2.y, point3.y)) - rect.y;
        }

        {
            float fScale = ((m_uZoom / 100.0f) * xDPI) / (72.0f * nDecimal);

            double m11 = fScale * fViewRotateCos;
            double m21 = -fScale * fViewRotateSin;
            double m12 = -fScale * fViewRotateSin;
            double m22 = -fScale * fViewRotateCos;

            double dx = -rect.getMinX() * fScale - m_ptViewPos.x;
            double dy = rect.getMaxY() * fScale - m_ptViewPos.y;

            matrixDoctoCli.setTransform(m11, m21, m12, m22, dx, dy);
        }

        {
            float fScale = (72.0f * nDecimal) / ((m_uZoom / 100.0f) * xDPI);

            double m11 = fScale * fViewRotateCos;
            double m21 = -fScale * fViewRotateSin;
            double m12 = -fScale * fViewRotateSin;
            double m22 = -fScale * fViewRotateCos;

            double dx = (rect.getMinX() + m_ptViewPos.x * fScale) * fViewRotateCos + (rect.getMaxY() - m_ptViewPos.y * fScale) * fViewRotateSin;
            double dy = -(rect.getMinX() + m_ptViewPos.x * fScale) * fViewRotateSin + (rect.getMaxY() - m_ptViewPos.y * fScale) * fViewRotateCos;

            matrixClitoDoc.setTransform(m11, m21, m12, m22, dx, dy);
        }
    }

    public void ZoomOut(Point point) //地图缩小
    {
        if (m_uZoom <= m_pPaper.m_viewLower)
        {

        }
        else
        {
            Point docPoint = new Point(point);
            ClientToDoc(docPoint);
            m_uZoom /= 2;
            SetViewSize(docPoint, point);
        }
    }

    public void ZoomIn(Point point) //地图放大
    {
        if (m_uZoom >= m_pPaper.m_viewUpper)
        {

        }
        else
        {
            Point docPoint = new Point(point);
            ClientToDoc(docPoint);
            m_uZoom *= 2;
            SetViewSize(docPoint, point);
        }
    }

    public void stop() //Stop the applet
    {
    }

    public void destroy() //Destroy the applet
    {
    }

    public String getAppletInfo() //Get Applet information
    {
        return "Mapeasy Information";
    }

    public String[][] getParameterInfo() //Get parameter info
    {
        return null;
    }
}

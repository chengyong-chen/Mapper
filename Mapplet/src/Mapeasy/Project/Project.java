package Mapeasy.Project;

import java.io.DataInputStream;
import java.io.IOException;

public class Project
{
    public String m_strUnit;

    public Project()
    {
    }

    static public Project FromType(byte type)
    {
        Project project = null;

        switch(type)
        {
        case 0:
                break;
        case 1:
                project = new Peking54();
                break;
        case 2:
                project = new Gauss();
                break;
        case 3:
  //              project = new CMercator;
                break;
        case 4:
    //            project = new CUTM;
                break;
        case 5:
                project = new WGS84();
                break;
        case 6:
//                project = new CPostel;
                break;
        case 7:
  //              project = new CConic;
                break;
        case 8:
      //          project = new CDLConicEquiAngular;
                break;
        case 9:
         //       project = new CDLConicEquiArea;
               break;
        }

        return project;
    }

    public void Serialize(DataInputStream dataStream)
   {
   }

    public void GeotoMap(double longitude,double latitude,double altitude,double X,double Y,double Z)
    {
    }

    public void MaptoGeo(double X,double Y,double Z,double longitude,double latitude,double altitude)
    {
    }
}

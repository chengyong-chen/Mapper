========================================================================
    ACTIVEX 控件 DLL : Mapeasy 项目概况
========================================================================

控件向导为您的 Mapeasy ActiveX 控件 
DLL 创建了此项目，其中包含一个控件。

此主干项目不仅说明了编写 
ActiveX 控件的基础知识，而且是编写特定控件
功能的起点。

此文件包含组成 Mapeasy ActiveX 控件 DLL
的每个文件的内容摘要。

Mapeasy.vcproj
    这是使用应用程序向导生成的 VC++ 项目的主
    项目文件。它包含有关生成该文件的
    Visual C++ 版本的信息，以及有关使用应用程序向导选择的平台、
    配置和项目功能的信息。

Mapeasy.h
    这是 ActiveX 控件 DLL 的主包含文件。
    它包括其他项目特定的包含文件，如 resource.h。

Mapeasy.cpp
    这是包含 DLL 初始化、终止和其他
    簿记的代码的主源文件。

Mapeasy.rc
    这是项目使用的 Microsoft Windows 资源列表。
    可使用 Visual C++ 资源编辑器
    直接编辑此文件。

Mapeasy.def
    此文件包含有关运行 Microsoft Windows 
    所需的 ActiveX 控件 DLL 的信息。

Mapeasy.idl
    此文件包含控件
    类型库的对象描述语言源代码。

/////////////////////////////////////////////////////////////////////////////
CMapeasyCtrl 控件:

MapeasyCtrl.h
    此文件包含 CMapeasyCtrl C++ 类的声明。

MapeasyCtrl.cpp
    此文件包含 CMapeasyCtrl C++ 类的实现。

MapeasyPropPage.h
    此文件包含 CMapeasyPropPage C++ 类的声明。

MapeasyPropPage.cpp
    此文件包含 CMapeasyPropPage C++ 类的实现。

CMapeasyCtrl.bmp
    此文件包含一个位图，容器将在
    CMapeasyCtrl 控件出现在工具面板上时使用它来表示该控件。此位图
    位于主资源文件 Mapeasy.rc 中。

/////////////////////////////////////////////////////////////////////////////
其他标准文件:

stdafx.h、stdafx.cpp
    这些文件用于生成名为 Mapeasy.pch
    的预编译头(PCH)文件以及名为 stdafx.obj 的预编译类型(PCT)文件。

resource.h
    这是标准的头文件，它定义了新的资源 ID。
    Visual C++ 资源编辑器读取和更新此文件。

/////////////////////////////////////////////////////////////////////////////
其他注释:

控件向导使用 "TODO:" 指示
应增加或自定义的源代码部分。

/////////////////////////////////////////////////////////////////////////////

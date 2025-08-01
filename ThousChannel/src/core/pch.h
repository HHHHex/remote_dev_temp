// pch.h: 这是预编译标头文件。
// 下方列出的文件仅编译一次，提高了将来生成的生成性能。
// 这还将影响 IntelliSense 性能，包括代码完成和许多代码浏览功能。
// 但是，如果此处列出的文件中的任何一个在生成之间有更新，它们全部都将被重新编译。
// 请勿在此处添加要频繁更新的文件，这将使得性能优势无效。

#pragma once

#include "targetver.h"

// MFC 头文件
#include <afxwin.h>         // MFC 核心和标准组件
#include <afxext.h>         // MFC 扩展
#include <afxcontrolbars.h> // MFC 支持功能区和控制栏
#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE 类
#include <afxodlgs.h>       // MFC OLE 对话框类
#endif
#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>          // MFC ODBC 数据库类
#endif
#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>         // MFC DAO 数据库类
#endif
#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>       // MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>         // MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT

// Agora RTE SDK 头文件
// 注意：RTC相关头文件已移除，现在使用RTE SDK


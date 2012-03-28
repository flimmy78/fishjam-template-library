///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file   ftlComDetect.h
/// @brief  Functional Template Library Base Header File.
/// @author fujie
/// @version 0.6 
/// @date 03/30/2008
/// @defgroup ftlComDetect ftl Com Detect function and class
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef FTL_COM_DETECT_H
#define FTL_COM_DETECT_H
#pragma once

#ifndef FTL_BASE_H
#  error ftlComDetect.h requires ftlbase.h to be included first
#endif

//TODO -- HKLM�µ�Software\Classes\Interface ��Ҳ�нӿڣ��ƺ���һ����
/**********************************************************************************************************
* ������ģʽ����һ�����֧�ֵĽӿڼ���Ŀ��QI��
*   1.ע����� HKEY_CLASSES_ROOT\Interface Ŀ¼���б�����ע������нӿ�(��������) -- CoDetectInterfaceFromRegister
*     �ŵ㣺��������ע��Ľӿ���������չ���������ҵ��Զ���ӿڣ�
*     ȱ�㣺�����ҵ�������û��ע��ģ��磺���ܴ�ActiveX�ؼ����ҵ� IForegroundTransfer �ӿڣ�
*           ���ܴ��ҵ��Ľӿ��л�ȡ��һ������Ϣ��
*           �ٶȱȽ�����
*   2.���Զ���Ľӿ��б��в���(CoDetectInterfaceFromList) -- �����ҵ��ӿ��б���û��д��ģ�
*     �ŵ㣺���Զ�����Ҫ���ҵĽӿڣ������ٶȿ죻
*           �ܴӽӿ��л�ý�һ������Ϣ -- ͨ�� DumpInterfaceInfo ����
*     ȱ�㣺ֻ�ܴ��б��в��ң���Χ��С�����ҶԲ�ͬ�ı�������Ҫ���岻ͬ�ı�����������������ͬ��ͷ�ļ�
* 
* �Զ��� IDL ������ .h �ļ�����ȡ���ӿڵĺ�(��VS2003������) -- �����ļ�֮���л������Ҳ�����
* ע�⣺���ֻ�������ļ�������  Visual Assist���ȼ���IDL��ͷ�ļ���������������ǰ��
Option Strict Off
Option Explicit Off
Imports EnvDTE
Imports System.Diagnostics

Public Module RecordingModule
Sub GetInterfaceName()
DTE.ExecuteCommand("Edit.Find")
DTE.Find.FindWhat = "typedef interface "
DTE.Find.Target = vsFindTarget.vsFindTargetCurrentDocument
DTE.Find.MatchCase = True
DTE.Find.MatchWholeWord = False
DTE.Find.Backwards = False
DTE.Find.MatchInHiddenText = False
DTE.Find.PatternSyntax = vsFindPatternSyntax.vsFindPatternSyntaxLiteral
DTE.Find.Action = vsFindAction.vsFindActionFind
DTE.Find.Execute()
DTE.Windows.Item(Constants.vsWindowKindFindReplace).Close()
DTE.ActiveDocument.Selection.CharRight()
DTE.ActiveDocument.Selection.WordRight(True)
DTE.ActiveDocument.Selection.CharLeft(True)
DTE.ActiveDocument.Selection.Copy()
DTE.ExecuteCommand("Window.NextDocumentWindow")
DTE.ActiveDocument.Selection.Text = "DETECT_INTERFACE_ENTRY("
DTE.ActiveDocument.Selection.Paste()
DTE.ActiveDocument.Selection.Text = ")"
DTE.ActiveDocument.Selection.NewLine()
DTE.ExecuteCommand("Window.NextDocumentWindow")
End Sub
End Module
**********************************************************************************************************/


#include <atlconv.h>

namespace FTL
{
    FTLINLINE HRESULT GetInterfaceNameByIID(REFIID rclsid, BSTR * pszName);
}

#ifdef FTL_DEBUG

# define INCLUDE_DETECT_ACTIVSCP    1
# define INCLUDE_DETECT_CONTROL     1

#ifndef INCLUDE_DETECT_DDRAW
#    define INCLUDE_DETECT_DDRAW    0
#endif //INCLUDE_DETECT_DDRAW

# define INCLUDE_DETECT_DISPEX      1

# define INCLUDE_DETECT_DOCOBJ      1

#ifndef INCLUDE_DETECT_DTE
#   define INCLUDE_DETECT_DTE       0
#endif 

# define INCLUDE_DETECT_EXDISP      1
# define INCLUDE_DETECT_KSPROXY     0
# define INCLUDE_DETECT_MEDIAOBJ    0
# define INCLUDE_DETECT_MSHTMLC     1
# define INCLUDE_DETECT_MSXML       1
# define INCLUDE_DETECT_OAIDL       1
# define INCLUDE_DETECT_OBJIDL      1
# define INCLUDE_DETECT_OBJSAFE     1
# define INCLUDE_DETECT_OCIDL       1
# define INCLUDE_DETECT_OLEIDL      1
# define INCLUDE_DETECT_QEDIT       0
# define INCLUDE_DETECT_SERVPROV    1
# define INCLUDE_DETECT_SHLOBJ      0
# define INCLUDE_DETECT_STRMIF      0
# define INCLUDE_DETECT_URLMON      1

#ifndef INCLUDE_DETECT_VSIP
#    define INCLUDE_DETECT_VSIP     0
#endif 

//Windows Media Format(asf,wma,wmv)
# define INCLUDE_DETECT_WMF         0

# if INCLUDE_DETECT_WMF
#   define INCLUDE_DETECT_DSHOWASF  1
#   define INCLUDE_DETECT_WMSDKIDL  1
# else
#   define INCLUDE_DETECT_DSHOWASF  0
#   define INCLUDE_DETECT_WMSDKIDL  0
# endif

                                      
//#include <initguid.h>

#if INCLUDE_DETECT_ACTIVSCP
#  include <ActivScp.h>
#endif

#if INCLUDE_DETECT_CONTROL
#  include <control.h>
#endif 

#if INCLUDE_DETECT_DDRAW
#  include <ddraw.h>
#endif

#if INCLUDE_DETECT_DISPEX
#  include <DispEx.h>
#endif

#if INCLUDE_DETECT_DOCOBJ
#  include <DocObj.h>
#endif

#if INCLUDE_DETECT_DSHOWASF
#  include <dshowasf.h>
#endif

#if INCLUDE_DETECT_EXDISP
#  include <exdisp.h>
#endif

#if INCLUDE_DETECT_KSPROXY
#  include <ks.h>
#  include <ksproxy.h>
#endif 

#if INCLUDE_DETECT_MEDIAOBJ
#  include <mediaobj.h>
#endif 

#if INCLUDE_DETECT_MSHTMLC
#  include <Mshtmlc.h>
#endif

#if INCLUDE_DETECT_MSXML
#  include <msxml.h>
#endif 

#if INCLUDE_DETECT_OAIDL
#  include <Oaidl.h>
#endif

#if INCLUDE_DETECT_OBJIDL
#  include <ObjIdl.h>
#endif

#if INCLUDE_DETECT_OBJSAFE
#  include <ObjSafe.h>
#endif

#if INCLUDE_DETECT_OCIDL
#  include <OCIdl.h>
#endif 

#if INCLUDE_DETECT_OLEIDL
#  include <OleIdl.h>
#endif 

#if INCLUDE_DETECT_QEDIT
#  include <qedit.h>
#endif 

#if INCLUDE_DETECT_SERVPROV
#  include <ServProv.h>
#endif 

#if INCLUDE_DETECT_SHLOBJ
#  include <ShlObj.h>
#endif 

#if INCLUDE_DETECT_STRMIF
#  include <strmif.h>
#endif 

#if INCLUDE_DETECT_URLMON
#  include <UrlMon.h>
#endif 

#if INCLUDE_DETECT_WMSDKIDL
#  include <wmsdkidl.h>
#endif 

//���Զ����б��м��ӿ�ָ��֧��(�ܽ���QI)�Ľӿ�
# define COM_DETECT_INTERFACE_FROM_LIST(pUnk) \
    {\
        FTLTRACEEX(FTL::tlTrace, TEXT("%s(%d) : Begin Detect Interface %s( 0x%p ) From List\n"),TEXT(__FILE__),__LINE__,TEXT(#pUnk),pUnk);\
        DWORD dwIntCount = FTL::CFComDetect::CoDetectInterfaceFromList(pUnk,GUID_NULL,FTL::CFComDetect::cdtInterface);\
        FTLTRACEEX(FTL::tlInfo,TEXT("%s's Interfaces Count are at least %d\n\n"),TEXT(#pUnk),dwIntCount);\
    }

//���Զ����б��м�� IMoniker �ӿ�ָ���� BindToObject �Ľӿ�
# define COM_DETECT_MONIKER_BIND_TO_OBJECT_FROM_LIST(pMoniker)\
    {\
        FTLTRACEEX(FTL::tlTrace,TEXT("%s(%d) : Begin Detect Moniker Can Bind to Object %s( 0x%p ) From List\n"),\
        TEXT(__FILE__),__LINE__,TEXT(#pMoniker),pMoniker);\
        DWORD dwIntCount = FTL::CFComDetect::CoDetectInterfaceFromList(pMoniker,GUID_NULL,FTL::CFComDetect::cdtMonikerBind);\
        FTLTRACEEX(FTL::tlTrace,TEXT("%s Can Bind to at least %d Interfaces\n\n"),TEXT(#pMoniker),dwIntCount);\
    }

//���б��м�� IServiceProvider::QueryService ��Query ���Ľӿ�
//  ���е� guidService Ҳʹ�� riid���ܼ����󲿷ֵĽӿڣ����в��ֲ��ܼ�����
//  ���磺EnvDTE �� �� QueryService(SID_SVsTextManager, IID_IVsTextManager, xxxx) -- SID_SVsTextManager �� IID_IVsTextManager ��һ��
# define COM_DETECT_SERVICE_PROVIDER_FROM_LIST(pServiceProvider) \
    {\
        FTLTRACEEX(FTL::tlTrace, TEXT("%s(%d) : Begin Detect Service Provider %s( 0x%p ) From List\n"),\
            TEXT(__FILE__),__LINE__,TEXT(#pServiceProvider),pServiceProvider);\
        DWORD dwServiceCount = FTL::CFComDetect::CoDetectInterfaceFromList(pServiceProvider,GUID_NULL,FTL::CFComDetect::cdtService);\
        FTLTRACEEX(FTL::tlInfo,TEXT("%s's Services Count are at least %d\n\n"),TEXT(#pServiceProvider),dwServiceCount);\
    }

//���Զ����б��м��ϣ������RIID��ʲô�ӿڣ������� DirectShow �е� NonDelegatingQueryInterface��
//ʹ��ͬǰ����ͬ���Զ����б�
# define COM_DETECT_RIID_FROM_LIST(riid)\
    {\
        USES_CONVERSION;\
        LPOLESTR  lpszRIID = NULL;\
        StringFromIID(riid,&lpszRIID);\
        FTLTRACEEX(FTL::tlTrace,TEXT("%s(%d) : Begin Detect RIID %s(%s) From List\n"),TEXT(__FILE__),__LINE__,TEXT(#riid),OLE2T(lpszRIID));\
        DWORD dwIntCount = FTL::CFComDetect::CoDetectInterfaceFromList(NULL,riid,FTL::CFComDetect::cdtIID);\
        if(0 == dwIntCount)\
        {\
            FTLTRACEEX(tlWarning, TEXT("Can't Detect RIID %s(%s).\n"),TEXT(#riid),OLE2T(lpszRIID));\
        }\
        CoTaskMemFree(lpszRIID);\
    }


//��ע����м��ӿ�ָ��֧��(�ܽ���QI)�Ľӿ�
# define COM_DETECT_INTERFACE_FROM_REGISTER(pUnknown) \
    {\
        FTLTRACEEX(FTL::tlTrace,TEXT("%s(%d) : Begin Detect Interface %s( 0x%p ) From Register\n"),TEXT(__FILE__),__LINE__,TEXT(#pUnknown),pUnknown);\
        DWORD dwIntCount = FTL::CFComDetect::CoDetectInterfaceFromRegister(pUnknown, GUID_NULL,FTL::CFComDetect::cdtInterface);\
        FTLTRACEEX(FTL::tlTrace,TEXT("%s's Interfaces Count are at least %d\n\n"),TEXT(#pUnknown),dwIntCount);\
    }

# define COM_DETECT_SERVICE_PROVIDER_FROM_REGISTER(pUnknown) \
	{\
		FTLTRACEEX(FTL::tlTrace, TEXT("%s(%d) : Begin Detect Service Provider %s( 0x%p ) From Register\n"),\
		TEXT(__FILE__),__LINE__,TEXT(#pUnknown),pUnknown);\
		DWORD dwServiceCount = FTL::CFComDetect::CoDetectInterfaceFromRegister(pUnknown,GUID_NULL,FTL::CFComDetect::cdtService);\
		FTLTRACEEX(FTL::tlInfo,TEXT("%s's Services Count are at least %d\n\n"),TEXT(#pUnknown),dwServiceCount);\
	}

#else //FTL_DEBUG
# define COM_DETECT_INTERFACE_FROM_LIST(pUnk)        (void)pUnk;
# define COM_DETECT_RIID_FROM_LIST(riid)             (void)riid;
# define COM_DETECT_INTERFACE_FROM_REGISTER(pUnk)    (void)pUnk;
#endif  //NONE FTL_DEBUG

#ifdef FTL_DEBUG

	#define DETECT_INTERFACE_ENTRY_IID(IntType, riid) \
		DETECT_INTERFACE_ENTRY_EX_IID(IntType,riid,CFDummyDump)

	#define DETECT_INTERFACE_ENTRY(IntType) \
		DETECT_INTERFACE_ENTRY_EX_IID(IntType,__uuidof(IntType),CFDummyDump)

	#define DETECT_INTERFACE_ENTRY_EX(IntType,classDumpInfo) \
		DETECT_INTERFACE_ENTRY_EX_IID(IntType,__uuidof(IntType),classDumpInfo)

//�ɵķ�ʽ -- Ч�ʷǳ��������ʹ�ö�Ӧ�ӿڵ�ʵ��
#if 0
    #define BEGIN_DETECT_INTERFACE() \
        {\
            HRESULT hr = E_FAIL;\
            DWORD dwInterfaceCount = 0;\
            DWORD dwTotalCheckCount = 0;\
            IMoniker* pMoniker = NULL;\
            IServiceProvider* pSvrProvider = NULL;\
            if(CFComDetect::cdtMonikerBind == detectType)\
            {\
                COM_VERIFY((pUnknown)->QueryInterface(IID_IMoniker,(void**)(&pMoniker)));\
            }\
            else if(CFComDetect::cdtService == detectType)\
            {\
                COM_VERIFY((pUnknown)->QueryInterface(IID_IServiceProvider,(void**)(&pSvrProvider)));\
            }

    #define DETECT_INTERFACE_ENTRY_EX_IID(IntType,riid,classDumpInfo)\
            {\
                dwTotalCheckCount++;\
                if(FTL::CFComDetect::cdtInterface == detectType)\
                {\
                    IntType* p##IntType = NULL;\
                    hr = (pUnknown)->QueryInterface(riid,(void**)(&p##IntType));\
                    if(SUCCEEDED(hr) && p##IntType != NULL)\
                    {\
                        dwInterfaceCount++;\
                        FTLTRACEEX(FTL::tlTrace,TEXT("\t%d: %s\n"),dwInterfaceCount,TEXT(#IntType));\
                        classDumpInfo::DumpInterfaceInfo((IntType*)(p##IntType));\
                        p##IntType->Release();\
                        p##IntType = NULL;\
                    }\
                    else if(E_NOINTERFACE != hr)\
                    {\
                        FTLTRACEEX(tlWarning,TEXT("Warning: Detect %s ,return 0x%p\n"),TEXT(#IntType),hr);\
                    }\
                }\
                else if(FTL::CFComDetect::cdtIID == detectType)\
                {\
                    if(riid == checkRIID)\
                    {\
                        dwInterfaceCount++;\
                        FTLTRACEEX(FTL::tlTrace,TEXT("\tRiid is %s\n"),TEXT(#IntType));\
                    }\
                }\
                else if(FTL::CFComDetect::cdtMonikerBind == detectType)\
                {\
                    IntType* p##IntType = NULL;\
                    hr = (pMoniker)->BindToObject(NULL,NULL,riid,(void**)(&p##IntType));\
                    if(SUCCEEDED(hr) && p##IntType != NULL)\
                    {\
                        dwInterfaceCount++;\
                        FTLTRACEEX(FTL::tlTrace,TEXT("\t%d: %s\n"),dwInterfaceCount,TEXT(#IntType));\
                        p##IntType->Release();\
                        p##IntType = NULL;\
                    }\
                }\
                else if(FTL::CFComDetect::cdtService == detectType)\
                {\
                    IntType* p##IntType = NULL;\
                    hr = (pSvrProvider)->QueryService(riid, riid,(void**)(&p##IntType));\
                    if(SUCCEEDED(hr) && p##IntType != NULL)\
                    {\
                        dwInterfaceCount++;\
                        FTLTRACEEX(FTL::tlTrace,TEXT("\t%d: %s\n"),dwInterfaceCount,TEXT(#IntType));\
                        p##IntType->Release();\
                        p##IntType = NULL;\
                    }\
                    else if(E_NOINTERFACE != hr)\
                    {\
                        FTLTRACEEX(tlWarning,TEXT("Warning: Detect %s ,return 0x%p\n"),TEXT(#IntType),hr);\
                    }\
                }\
                else\
                {\
                    FTLTRACEEX(tlError,TEXT("\tUnknown Operation \n"));\
                }\
            }

    #define END_DETECT_INTERFACE()\
            SAFE_RELEASE(pMoniker);\
            SAFE_RELEASE(pSvrProvider);\
            if(CFComDetect::cdtInterface == detectType || CFComDetect::cdtService == detectType)\
            {\
                FTLTRACEEX(FTL::tlTrace,TEXT("\tTotal Check %d Interfaces\n"),dwTotalCheckCount);\
            }\
            return dwInterfaceCount;\
        }

#else

//�µķ�ʽ -- Ч�ʸ�(ÿ���ӿڶ�Ӧ�ĺ�ֻ�������е�һ��)��������ʹ�ýӿڵ�ʵ��
//  ��ô�������д洢 �ӿ� �����ͣ����߳����ڽӿڵ������б���ӿڵ�ʵ��ָ�� ?
typedef HRESULT (*DumpInterfaceInfoProc)(IUnknown* pUnknown);

struct CFInterfaceEntryExIID
{
	//DWORD_PTR m_vtbl;   // filled in with USE_INTERFACE_PART
	//IUnknown*  m_pUnk;
	LPCTSTR		pszInterfaceName;
	GUID	  id;
	DumpInterfaceInfoProc	m_pDumpInfoProc;
	//CFInterfaceEntryExIID() { m_pUnk = 0; }
};


#define BEGIN_DETECT_INTERFACE() \
		HRESULT hr = E_FAIL;\
		DWORD dwInterfaceCount = 0;\
		DWORD dwTotalCheckCount = 0;\
		IMoniker* pMoniker = NULL;\
		IServiceProvider* pSvrProvider = NULL;\
		if(CFComDetect::cdtMonikerBind == detectType)\
		{\
			COM_VERIFY((pUnknown)->QueryInterface(IID_IMoniker,(void**)(&pMoniker)));\
			if(!pMoniker) {return DWORD(-1);}\
		}\
		else if(CFComDetect::cdtService == detectType)\
		{\
			COM_VERIFY((pUnknown)->QueryInterface(IID_IServiceProvider,(void**)(&pSvrProvider)));\
			if(!pSvrProvider) {return DWORD(-1);}\
		}\
		CFInterfaceEntryExIID allInterfaceEntries[] = \
		{\

#define DETECT_INTERFACE_ENTRY_EX_IID(IntType,riid,classDumpInfo) \
			{	TEXT(#IntType), riid, classDumpInfo::DumpInterfaceInfo },
	
#define END_DETECT_INTERFACE() \
			{	NULL, GUID_NULL, CFDummyDump::DumpInterfaceInfo }\
		};\
		CFInterfaceEntryExIID* pEntry = &allInterfaceEntries[0];\
		while(pEntry && pEntry->id != GUID_NULL)\
		{\
			dwTotalCheckCount++;\
			if(FTL::CFComDetect::cdtInterface == detectType)\
			{\
				IUnknown* pQueryUnknown = NULL;\
				hr = (pUnknown)->QueryInterface(pEntry->id,(void**)(&pQueryUnknown));\
				if(SUCCEEDED(hr) && pQueryUnknown)\
				{\
					dwInterfaceCount++;\
					FTLTRACEEX(FTL::tlTrace,TEXT("\t%d: %s\n"),dwInterfaceCount,pEntry->pszInterfaceName);\
					pEntry->m_pDumpInfoProc(pQueryUnknown);\
					pQueryUnknown->Release();\
					pQueryUnknown = NULL;\
				}\
				else if(E_NOINTERFACE != hr)\
				{\
					FTLTRACEEX(tlWarning,TEXT("Warning: Detect %s ,return 0x%p\n"),pEntry->pszInterfaceName,hr);\
				}\
			}\
			else if(FTL::CFComDetect::cdtIID == detectType)\
			{\
				if(pEntry->id == checkRIID)\
				{\
					dwInterfaceCount++;\
					FTLTRACEEX(FTL::tlTrace,TEXT("\tRiid is %s\n"),pEntry->pszInterfaceName);\
				}\
			}\
			else if(FTL::CFComDetect::cdtMonikerBind == detectType)\
			{\
				IUnknown* pBindUnknown = NULL;\
				hr = (pMoniker)->BindToObject(NULL,NULL,pEntry->id,(void**)(&pBindUnknown));\
				if(SUCCEEDED(hr) && pBindUnknown != NULL)\
				{\
					dwInterfaceCount++;\
					FTLTRACEEX(FTL::tlTrace,TEXT("\t%d: %s\n"),dwInterfaceCount,pEntry->pszInterfaceName);\
					pBindUnknown->Release();\
					pBindUnknown = NULL;\
				}\
			}\
			else if(FTL::CFComDetect::cdtService == detectType)\
			{\
				IUnknown* pServiceUnknown = NULL;\
				hr = (pSvrProvider)->QueryService(pEntry->id, pEntry->id,(void**)(&pServiceUnknown));\
				if(SUCCEEDED(hr) && pServiceUnknown != NULL)\
				{\
					dwInterfaceCount++;\
					FTLTRACEEX(FTL::tlTrace,TEXT("\t%d: %s\n"),dwInterfaceCount,pEntry->pszInterfaceName);\
					pServiceUnknown->Release();\
					pServiceUnknown = NULL;\
				}\
				else if(E_NOINTERFACE != hr)\
				{\
					FTLTRACEEX(tlWarning,TEXT("Warning: Detect Service %s ,return 0x%p\n"),pEntry->pszInterfaceName,hr);\
				}\
			}\
			else\
			{\
				FTLTRACEEX(tlError,TEXT("\tUnknown Operation \n"));\
			}\
			pEntry++;\
		}\
		if(CFComDetect::cdtInterface == detectType || CFComDetect::cdtService == detectType)\
		{\
			FTLTRACEEX(FTL::tlTrace,TEXT("\tTotal Check %d Interfaces\n"),dwTotalCheckCount);\
		}\
		SAFE_RELEASE(pMoniker);\
		SAFE_RELEASE(pSvrProvider);\
		return dwInterfaceCount;

	
#endif 

namespace FTL
{
    class CFComDetect
    {
    public:
        typedef enum ComDetectType
        {
            cdtInterface,       //QueryInterface
            cdtIID,             //
            cdtMonikerBind,     //
            cdtService,         //QueryService(���� SID_XXX һ�㶨��Ϊ IID_XXX �Ļ����ж�)
        }ComDetectType;

		FTLINLINE static DWORD CoDetectInterfaceFromRegister(IUnknown* pUnknown, REFIID checkRIID, ComDetectType detectType);
        FTLINLINE static DWORD CoDetectInterfaceFromList(IUnknown* pUnknown, REFIID checkRIID, 
            ComDetectType detectType);
	private:
		FTLINLINE static HRESULT _innerCoDtectInterfaceFromRegister(IUnknown* pUnknown, REFGUID guidInfo);
		FTLINLINE static HRESULT _innerCoDtectServiceFromRegister(IServiceProvider* pServiceProvider, REFGUID guidInfo);
    }; //class CFComDetect
}//namespace FTL

#endif //FTL_DEBUG

#endif //FTL_COM_DETECT_H

#ifndef USE_EXPORT
#  include "ftlComDetect.hpp"
#endif
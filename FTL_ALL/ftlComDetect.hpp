#ifndef FTL_COM_DETECT_HPP
#define FTL_COM_DETECT_HPP
#pragma once

#ifdef USE_EXPORT
#  include "ftlComDetect.h"
#endif

#pragma TODO(unhandled interface)
//IVsFontAndColorDefaultsProvider
//IVsCodePageSelection
//IVsTextManager2
//LearnVSIP�� IWin32Window(.NET��WinForm��?) ��ToolWindowPane ������ģ�

#include <atlcomcli.h>
#include <OAIdl.h>

namespace FTL
{
    FTLINLINE HRESULT GetInterfaceNameByIID(REFIID rclsid, BSTR * pszName)
    {
        HRESULT hr = E_FAIL;
        BOOL bRet = FALSE;

        LPOLESTR iidString = NULL;
        COM_VERIFY(StringFromIID(rclsid, &iidString));
        if(SUCCEEDED(hr))
        {
            USES_CONVERSION;
            LONG lRet = ERROR_SUCCESS;
            FTL::CFStringFormater strFormater;
            strFormater.Format(TEXT("Interface\\%s"), OLE2T(iidString));
            HKEY hKeyInterface = NULL;
            REG_VERIFY(::RegOpenKeyEx(HKEY_CLASSES_ROOT, strFormater, 0, KEY_READ,&hKeyInterface));
            if (ERROR_SUCCESS == lRet)
            {
                TCHAR interfaceName[MAX_BUFFER_LENGTH] = {0};
                DWORD dwInterfaceNameLength = MAX_BUFFER_LENGTH;
                DWORD dwType;
                REG_VERIFY(::RegQueryValueEx(hKeyInterface,TEXT(""),NULL,&dwType,(LPBYTE)interfaceName,&dwInterfaceNameLength));
                *pszName = SysAllocString(T2W(interfaceName));
                SAFE_CLOSE_REG(hKeyInterface);
            }
            CoTaskMemFree(iidString);
        }
        return hr;
    }


#ifdef FTL_DEBUG

    class CFDummyDump
    {
    public:
        static HRESULT DumpInterfaceInfo(IUnknown* /* pUnk*/)
        {
            //Do Nothing
            return S_FALSE;
        }
    };

    class CFTypeInfoDump
    {
    public:
        static HRESULT DumpInterfaceInfo(IUnknown* pUnknown)
        {
            HRESULT hr = E_FAIL;
            TYPEATTR*   pta = NULL; 
			CComQIPtr<ITypeInfo> pTypeInfo(pUnknown);
            COM_VERIFY(pTypeInfo-> GetTypeAttr(&pta));
            USES_CONVERSION;

            if(TKIND_COCLASS == pta->typekind)
            {
                //pTypeAttr->guid contains the CLSID of the CoClass
                WCHAR* pwszProgID = NULL;
                COM_VERIFY(ProgIDFromCLSID(pta->guid,&pwszProgID));
                FTLTRACEEX(FTL::tlTrace,TEXT("\t\t\tProgId = %s\n"), W2T(pwszProgID));
                CoTaskMemFree(pwszProgID);
            }

            //��ȡ��������Ϣ
            for (int i = 0; i < pta->cVars; ++i)
            {
                VARDESC* pvd = NULL;
                COM_VERIFY(pTypeInfo->GetVarDesc(i, &pvd));
                BSTR   bstrName = NULL;
                COM_VERIFY(pTypeInfo->GetDocumentation(pvd->memid,&bstrName,NULL,NULL,NULL));
                FTLTRACEEX(FTL::tlTrace,TEXT("\t\t\tVar[%d in %d] = %s\n"), i, pta->cVars, OLE2T(bstrName));
                SysFreeString(bstrName);
                pTypeInfo->ReleaseVarDesc(pvd);
            }

            //��ȡ��������Ϣ -- 3(IUnknown) + 4(IDispatch) + ҵ��ӿ�
            for(int i = 0; i< pta->cFuncs;++i)
            {
                FUNCDESC* pfd = NULL; 
                COM_VERIFY(pTypeInfo->GetFuncDesc(i,&pfd));
                BSTR   bstrName = NULL;
                COM_VERIFY(pTypeInfo->GetDocumentation(pfd->memid,&bstrName,NULL,NULL,NULL));
                FTLTRACEEX(FTL::tlTrace,TEXT("\t\t\tFun[%d in %d] = %s\n"), i, pta->cFuncs, OLE2T(bstrName));
                SysFreeString(bstrName);
                pTypeInfo->ReleaseFuncDesc(pfd);
            }
            pTypeInfo->ReleaseTypeAttr(pta);
            return hr;
        }
    };

    class CFDispatchDump
    {
    public:
        static HRESULT DumpInterfaceInfo(IUnknown* pUnknown)
        {
            HRESULT hr = E_FAIL;
            unsigned int infoCount = 0;
			CComQIPtr<IDispatch> pDisp(pUnknown);
            COM_VERIFY(pDisp->GetTypeInfoCount(&infoCount));
            FTLTRACEEX(FTL::tlTrace,TEXT("\t\tIn CDispatchDump, TypeInfoCount=%d\n"), infoCount);

            for (unsigned int infoIndex = 0; infoIndex < infoCount; ++infoIndex)
            {
                ATL::CComPtr<ITypeInfo>  spTypeInfo;
				//���� VSIP �е� IVsHierarchy �᷵�� Library not registered
                COM_VERIFY_EXCEPT1(pDisp->GetTypeInfo(infoIndex, LOCALE_SYSTEM_DEFAULT, &spTypeInfo), TYPE_E_LIBNOTREGISTERED);
				if (SUCCEEDED(hr) && spTypeInfo)
				{
					COM_VERIFY(CFTypeInfoDump::DumpInterfaceInfo(spTypeInfo));
				}
            }
            return S_OK;
        }
    };

    class CFServiceProviderDump
    {
    public:
        static HRESULT DumpInterfaceInfo(IUnknown* pUnknown)
        {
            HRESULT hr = S_FALSE;
			CComQIPtr<IServiceProvider> pServiceProvider(pUnknown);
            //COM_DETECT_SERVICE_PROVIDER_FROM_LIST(pServiceProvider);
            //pServiceProvider->QueryService()
            return hr;
        }
    };

    class CFEnumVARIANTDump
    {
    public:
        static HRESULT DumpInterfaceInfo(IUnknown* pUnknown)
        {
            HRESULT hr = S_FALSE;
			//CComPtr<IEnumVARIANT> pEnumVariant(pUnknown);
#pragma TODO(complete this)
            return hr;
        }
    };

    class CFEnumConnectionPointsDump
    {
    public:
        static HRESULT DumpInterfaceInfo(IEnumConnectionPoints* pEnumCP)
        {
            LONG cpCount = 0;
            HRESULT hr = S_OK;
            IConnectionPoint *pCP = NULL;
            USES_CONVERSION;

            hr = pEnumCP->Reset();
            IID lastIID = {0};
            while (S_OK == hr)
            {
                hr = pEnumCP->Next(1,&pCP,NULL);//if No more CP, will return S_FALSE
                if (S_OK == hr && pCP)
                {
                    IID iid = {0};
                    COM_VERIFY(pCP->GetConnectionInterface(&iid));
                    if (IsEqualIID(lastIID, iid))
                    {
                        //In some COM object(such as Microsoft_VisualStudio_CommandBars::_CommandBars), 
                        //will get same iid and endless loop, this will make Access Violation (AV) Error
                        FTLTRACEEX(FTL::tlError, TEXT("%s : CFEnumConnectionPointsDump Find Same IID\n"), __FILE__LINE__);
                        pCP->Release();
                        pCP = NULL;
                        break;
                    }
                    lastIID = iid;
                    BSTR strInterfaceName = NULL;
                    COM_VERIFY(GetInterfaceNameByIID(iid, &strInterfaceName));
                    FTLTRACEEX(FTL::tlTrace,TEXT("\t\t  ConnectionPointName[%d] is %s\n"),
                        cpCount, OLE2T(strInterfaceName));
                    SysFreeString(strInterfaceName);
                    pCP->Release();
                    pCP = NULL;
                    cpCount++;
                }
            }
            FTLTRACEEX(FTL::tlTrace,TEXT("\t\tIn CEnumConnectionPointsDump, cpCount = %d\n"),cpCount);
            return hr;
        }
    };

    class CFConnectionContainerPointDump
    {
    public:
        static HRESULT DumpInterfaceInfo(IUnknown* pUnknown)
        {
            HRESULT hr = E_FAIL;
            //if (pContainer)
            {
				CComQIPtr<IConnectionPointContainer> pContainer(pUnknown);
                IEnumConnectionPoints* pEnumCP = NULL;
                hr = pContainer->EnumConnectionPoints(&pEnumCP);
                if (SUCCEEDED(hr) && pEnumCP)
                {
                    hr = CFEnumConnectionPointsDump::DumpInterfaceInfo(pEnumCP);
                    pEnumCP->Release();
                    pEnumCP = NULL;
                }
            }
            return hr;
        }
    };

#if INCLUDE_DETECT_STRMIF
    class CFMediaSeekingDump
    {
    public:
        static HRESULT DumpInterfaceInfo(IMediaSeeking* pMediaSeeking)
        {
            HRESULT hr = S_OK;
            //pMediaSeeking->IsFormatSupported();
            return hr;
        }
    };
#endif //INCLUDE_DETECT_STRMIF

    DWORD CFComDetect::CoDetectInterfaceFromRegister(IUnknown* pUnknown, REFIID checkRIID, ComDetectType detectType)
    {
        CHECK_POINTER_RETURN_VALUE_IF_FAIL(pUnknown, (DWORD)(-1));

        //û�� CATID_INTERFACE ����˲����� ICatInformation ��
        LONG lRet = ERROR_SUCCESS;
        DWORD dwCount = 0;
        HRESULT hr = E_FAIL;
        BOOL bRet = FALSE;
		IServiceProvider* pServiceProvider = NULL;
		if (cdtService == detectType)
		{
			COM_VERIFY(pUnknown->QueryInterface(IID_IServiceProvider, (void**)&pServiceProvider));
			if (FAILED(hr) || !pServiceProvider)
			{
				return DWORD(-1);
			}
		}

        HKEY hKeyInterface;
        REG_VERIFY(::RegOpenKeyEx(HKEY_CLASSES_ROOT,TEXT("Interface"),0,KEY_READ,&hKeyInterface));
        if (ERROR_SUCCESS == lRet)
        {
            DWORD dwIndex = 0;
            TCHAR bufName[MAX_BUFFER_LENGTH] = {0};
            LPTSTR pName = bufName;// + 10;
            DWORD cchValue = MAX_BUFFER_LENGTH;// - 10; 
            GUID guidInfo = GUID_NULL;

            REG_VERIFY(::RegEnumKey(hKeyInterface,dwIndex,pName,cchValue));
            while (ERROR_SUCCESS == lRet)
            {
                cchValue = MAX_BUFFER_LENGTH;
                COM_VERIFY_EXCEPT1(CLSIDFromString(ATL::CComBSTR(pName),&guidInfo), CO_E_CLASSSTRING);
                if (SUCCEEDED(hr))
                {
					switch(detectType)
					{
					case cdtInterface:
						hr = _innerCoDtectInterfaceFromRegister(pUnknown, guidInfo);
						break;
					case cdtService:
						hr = _innerCoDtectServiceFromRegister(pServiceProvider, guidInfo);
						break;
					default:
						FTLASSERT(FALSE);
						break;
					}
					if (SUCCEEDED(hr))
					{
						TCHAR interfaceName[MAX_BUFFER_LENGTH] = {0};
						DWORD dwInterfaceNameLength = MAX_BUFFER_LENGTH;
						DWORD dwType;

						HKEY hKeyGuid = NULL;
						dwCount++;
						REG_VERIFY(::RegOpenKeyEx(hKeyInterface,pName,0,KEY_READ,&hKeyGuid));
						lRet = ::RegQueryValueEx(hKeyGuid,TEXT(""),NULL,&dwType,(LPBYTE)interfaceName,&dwInterfaceNameLength);
						if (ERROR_SUCCESS == lRet)
						{
							FTLTRACEEX(FTL::tlTrace,TEXT("\t%d : %s\n"),dwCount,interfaceName);
						}
						else
						{
							FTLTRACEEX(FTL::tlTrace,TEXT("\t%d : %s\n"),dwCount, pName);
						}
						RegCloseKey(hKeyGuid);
					}
                }
                else
                {
                    FTLTRACEEX(FTL::tlWarning, TEXT("%s: ERROR ClassID String: %s\n"),__FILE__LINE__, pName);
                }
                dwIndex++;
                lRet = ::RegEnumKey(hKeyInterface,dwIndex,pName,cchValue);
                API_VERIFY(ERROR_SUCCESS == lRet || ERROR_NO_MORE_ITEMS == lRet);
            }
            FTLTRACEEX(FTL::tlTrace,TEXT("Total Check %d Interfaces\n"),dwIndex);
            SAFE_CLOSE_REG(hKeyInterface);
        }
        return dwCount;
    }

	HRESULT CFComDetect::_innerCoDtectInterfaceFromRegister(IUnknown* pUnknown, REFGUID guidInfo)
	{
		HRESULT hr = E_FAIL;
		IUnknown* pDetectedInterface = NULL;
		COM_VERIFY_EXCEPT1(pUnknown->QueryInterface(guidInfo,(void**)&pDetectedInterface), E_NOINTERFACE);
		if(SUCCEEDED(hr) && pDetectedInterface)
		{
			pDetectedInterface->Release();
			pDetectedInterface = NULL;
		}
		return hr;
	}

	HRESULT CFComDetect::_innerCoDtectServiceFromRegister(IServiceProvider* pServiceProvider, REFGUID guidInfo)
	{
		HRESULT hr = E_FAIL;
		IUnknown* pDetectedInterface = NULL;
		hr = pServiceProvider->QueryService(guidInfo, guidInfo, (void**)&pDetectedInterface);
		if(SUCCEEDED(hr) && pDetectedInterface)
		{
			pDetectedInterface->Release();
			pDetectedInterface = NULL;
		}
		return hr;
	}

    //������ͬʱ֧�ּ�����COM��Ϣ -- ��Ȼ���Ǻõı��ϰ�ߣ����ܼ���ά����
    DWORD CFComDetect::CoDetectInterfaceFromList(IUnknown* pUnknown, REFIID checkRIID, ComDetectType detectType)
    {
        CHECK_POINTER_RETURN_VALUE_IF_FAIL(pUnknown, (DWORD)(-1));
        BEGIN_DETECT_INTERFACE()

            //Unknwn.h  //IUnknown
            DETECT_INTERFACE_ENTRY(IUnknown)
            DETECT_INTERFACE_ENTRY(AsyncIUnknown)
            DETECT_INTERFACE_ENTRY(IClassFactory) //�ͻ��˲�֪��COM�������������������Ӧ���๤��֪������˿��Դ���
            //ATL��ÿһ���������һ���๤��(thisָ�벻ͬ)

            //ComCat.h
            DETECT_INTERFACE_ENTRY(IEnumGUID)
            DETECT_INTERFACE_ENTRY(IEnumCATEGORYINFO)
            DETECT_INTERFACE_ENTRY(ICatRegister)
            DETECT_INTERFACE_ENTRY(ICatInformation)

#if INCLUDE_DETECT_ACTIVSCP
            DETECT_INTERFACE_ENTRY(IActiveScriptSite)
            DETECT_INTERFACE_ENTRY(IActiveScriptError)
            DETECT_INTERFACE_ENTRY(IActiveScriptError64)
            DETECT_INTERFACE_ENTRY(IActiveScriptSiteWindow)
            DETECT_INTERFACE_ENTRY(IActiveScriptSiteInterruptPoll)
            DETECT_INTERFACE_ENTRY(IActiveScript)
            DETECT_INTERFACE_ENTRY(IActiveScriptParse32)
            DETECT_INTERFACE_ENTRY(IActiveScriptParse64)
            DETECT_INTERFACE_ENTRY(IActiveScriptParseProcedureOld32)
            DETECT_INTERFACE_ENTRY(IActiveScriptParseProcedureOld64)
            DETECT_INTERFACE_ENTRY(IActiveScriptParseProcedure32)
            DETECT_INTERFACE_ENTRY(IActiveScriptParseProcedure64)
            DETECT_INTERFACE_ENTRY(IActiveScriptParseProcedure2_32)
            DETECT_INTERFACE_ENTRY(IActiveScriptParseProcedure2_64)
            DETECT_INTERFACE_ENTRY(IActiveScriptEncode)
            DETECT_INTERFACE_ENTRY(IActiveScriptHostEncode)
            DETECT_INTERFACE_ENTRY(IBindEventHandler)
            DETECT_INTERFACE_ENTRY(IActiveScriptStats)
            DETECT_INTERFACE_ENTRY(IActiveScriptProperty)
            DETECT_INTERFACE_ENTRY(ITridentEventSink)
            DETECT_INTERFACE_ENTRY(IActiveScriptGarbageCollector)
            DETECT_INTERFACE_ENTRY(IActiveScriptSIPInfo)
#endif //INCLUDE_DETECT_ACTIVSCP

#if INCLUDE_DETECT_CONTROL
            DETECT_INTERFACE_ENTRY(IAMCollection)   //FilterGraph���󼯺ϣ�����Filter��Pin

            //!�Զ����ӿڣ�����ý�岥�ſ���,�ܻ�ȡFilter�ļ���
            // Run/Stop/GetState/StopWhenReady/get_FilterCollection/...
            // Stop��ͨ����Ҫʹ�� IMediaSeeking->SetPosition ����ʼ�����ҵ���IMediaControl->Pause ����ʾ��һ֡ 
            DETECT_INTERFACE_ENTRY(IMediaControl)

            //!����ý���¼�������GetEvent/WaitForCompletion/FreeEventParams/RestoreDefaultHandling/...
            DETECT_INTERFACE_ENTRY(IMediaEvent)     //�������������¼�֪ͨ��������дFilterGraph��������Ĭ���¼������ķ���

            //!��IMediaEvent ���������ӷ���ͨ���Զ����Windows��Ϣ���лص�֪ͨ
            //!  ���нϺõ�����, SetNotifyWindow/...
            DETECT_INTERFACE_ENTRY(IMediaEventEx)

            //!��ȡ������ý��λ����Ϣ�� Dispatch �ӿڡ�
            //!  Duration/CurrentPosition/StopTime/PrerollTime/Rate/CanSeekBackward
            DETECT_INTERFACE_ENTRY(IMediaPosition)
            DETECT_INTERFACE_ENTRY(IBasicAudio)     //����Ӧ�ó��������Ƶ����������ƽ��(Volume/Balance)

            //! �����������ô��������ߡ����ڵ�λ�úͳߴ缰�����������Եķ���
            //! ����Video�����Dispatch�ӿڣ��������ø������ԣ�Caption,Style,AutoShow,FullScreen,WindowPosition,HideCursor �ȣ�
            //! ��: pVW->put_Owner((OAHWND)m_hWnd);
            //!     pVW->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
            DETECT_INTERFACE_ENTRY(IVideoWindow)    

            //! ����Ӧ�ó���������Ƶ���ԣ�����Ŀ����κ�Դ����,�ܶԵ�ǰ���ŵ�λ�ý���ץͼ
            //  BitRate/VideoWidth/SourceWidth/DestinationWidth/SourcePosition/DestinationPosition/VideoSize
            DETECT_INTERFACE_ENTRY(IBasicVideo)     

            DETECT_INTERFACE_ENTRY(IBasicVideo2)    //�� IBasicVideo �ӿ�������ΪӦ�ó����ṩ��һ�����ӷ�����ͨ�������Լ�����Ƶ������ѡ�ݺ��
            DETECT_INTERFACE_ENTRY(IDeferredCommand)//����Ӧ�ó���ȡ�����޸ĸ�Ӧ�ó�����ǰʹ�� IQueueCommand �ӿ�������е�ͼ��-��������
            DETECT_INTERFACE_ENTRY(IQueueCommand)   //����Ӧ�ó���Ԥ�Ƚ�ͼ��-���������������
            DETECT_INTERFACE_ENTRY(IFilterInfo)     //����Filter����Ϣ���ṩ����Filter�ͱ�ʾFilter�ϵ�Pin�� IPinInfo �ӿ�
            DETECT_INTERFACE_ENTRY(IRegFilterInfo)  //�ṩ�� Windows ע����е�Filter�ķ��ʣ��Լ���FilterGraph��������ע���Filter��
            DETECT_INTERFACE_ENTRY(IMediaTypeInfo)  //�������ڼ���Pin���ӵ�ý�����͵ķ���
            DETECT_INTERFACE_ENTRY(IPinInfo)        //�������ڼ���Pin��Ϣ������Pin�ķ���

            //����Ӧ�ó����ͼ�ι������м����������ݡ�Filter����ʹ�ô˽ӿڼ�¼��������,Active Movie Status(����/���Զԣ�)
            DETECT_INTERFACE_ENTRY(IAMStats)        
#endif //INCLUDE_DETECT_CONTROL

#if INCLUDE_DETECT_DDRAW
            //DDraw.h
            DETECT_INTERFACE_ENTRY_IID(IDirectDraw,IID_IDirectDraw)
            DETECT_INTERFACE_ENTRY_IID(IDirectDraw2,IID_IDirectDraw2)
            DETECT_INTERFACE_ENTRY_IID(IDirectDraw4,IID_IDirectDraw4)
            DETECT_INTERFACE_ENTRY_IID(IDirectDraw7,IID_IDirectDraw7)
            DETECT_INTERFACE_ENTRY_IID(IDirectDrawSurface,IID_IDirectDrawSurface)
            DETECT_INTERFACE_ENTRY_IID(IDirectDrawSurface2,IID_IDirectDrawSurface2)
            DETECT_INTERFACE_ENTRY_IID(IDirectDrawSurface3,IID_IDirectDrawSurface3)
            DETECT_INTERFACE_ENTRY_IID(IDirectDrawSurface4,IID_IDirectDrawSurface4)
            DETECT_INTERFACE_ENTRY_IID(IDirectDrawSurface7,IID_IDirectDrawSurface7)
            DETECT_INTERFACE_ENTRY_IID(IDirectDrawPalette,IID_IDirectDrawPalette)
            DETECT_INTERFACE_ENTRY_IID(IDirectDrawClipper,IID_IDirectDrawClipper)
            DETECT_INTERFACE_ENTRY_IID(IDirectDrawColorControl,IID_IDirectDrawColorControl)
            DETECT_INTERFACE_ENTRY_IID(IDirectDrawGammaControl,IID_IDirectDrawGammaControl)
#endif //INCLUDE_DETECT_DDRAW

#if INCLUDE_DETECT_DISPEX
            DETECT_INTERFACE_ENTRY(IDispatchEx)     //�����˶Գ�Ա�Ĺ����������Ƕ�̬���Ӻ�ɾ����Ա������
            DETECT_INTERFACE_ENTRY(IDispError)
            DETECT_INTERFACE_ENTRY(IVariantChangeType)
            DETECT_INTERFACE_ENTRY(IObjectIdentity)
#endif //INCLUDE_DETECT_DISPEX

#if INCLUDE_DETECT_DOCOBJ
            DETECT_INTERFACE_ENTRY(IOleDocument)
            DETECT_INTERFACE_ENTRY(IOleDocumentSite)
            DETECT_INTERFACE_ENTRY(IOleDocumentView)
            DETECT_INTERFACE_ENTRY(IEnumOleDocumentViews)
            DETECT_INTERFACE_ENTRY(IContinueCallback)
            DETECT_INTERFACE_ENTRY(IPrint)
            DETECT_INTERFACE_ENTRY(IOleCommandTarget)
            DETECT_INTERFACE_ENTRY(IZoomEvents)
            DETECT_INTERFACE_ENTRY(IProtectFocus)
            DETECT_INTERFACE_ENTRY(IProtectedModeMenuServices)
#endif //INCLUDE_DETECT_DOCOBJ

#if INCLUDE_DETECT_DTE
            //{
				DETECT_INTERFACE_ENTRY(EnvDTE::Globals)
                DETECT_INTERFACE_ENTRY(EnvDTE::_CommandEvents)
                DETECT_INTERFACE_ENTRY(EnvDTE::_dispCommandEvents)
                DETECT_INTERFACE_ENTRY(EnvDTE::_SelectionEvents)
                DETECT_INTERFACE_ENTRY(EnvDTE::_dispSelectionEvents)
                DETECT_INTERFACE_ENTRY(EnvDTE::_SolutionEvents)
                DETECT_INTERFACE_ENTRY(EnvDTE::_dispSolutionEvents)
                DETECT_INTERFACE_ENTRY(EnvDTE::_BuildEvents)
                DETECT_INTERFACE_ENTRY(EnvDTE::_dispBuildEvents)
                DETECT_INTERFACE_ENTRY(EnvDTE::_WindowEvents)
                DETECT_INTERFACE_ENTRY(EnvDTE::_dispWindowEvents)
                DETECT_INTERFACE_ENTRY(EnvDTE::_OutputWindowEvents)
                DETECT_INTERFACE_ENTRY(EnvDTE::_dispOutputWindowEvents)
                DETECT_INTERFACE_ENTRY(EnvDTE::_FindEvents)
                DETECT_INTERFACE_ENTRY(EnvDTE::_dispFindEvents)
                DETECT_INTERFACE_ENTRY(EnvDTE::_TaskListEvents)
                DETECT_INTERFACE_ENTRY(EnvDTE::_dispTaskListEvents)
                DETECT_INTERFACE_ENTRY(EnvDTE::_DTEEvents)
                DETECT_INTERFACE_ENTRY(EnvDTE::_dispDTEEvents)
                DETECT_INTERFACE_ENTRY(EnvDTE::_DocumentEvents)
                DETECT_INTERFACE_ENTRY(EnvDTE::_dispDocumentEvents)
                DETECT_INTERFACE_ENTRY(EnvDTE::_ProjectItemsEvents)
                DETECT_INTERFACE_ENTRY(EnvDTE::_dispProjectItemsEvents)
                DETECT_INTERFACE_ENTRY(EnvDTE::_DebuggerEvents)
                DETECT_INTERFACE_ENTRY(EnvDTE::_dispDebuggerEvents)
                DETECT_INTERFACE_ENTRY(EnvDTE::_TextEditorEvents)
                DETECT_INTERFACE_ENTRY(EnvDTE::_dispTextEditorEvents)
                DETECT_INTERFACE_ENTRY(EnvDTE::StatusBar)
                DETECT_INTERFACE_ENTRY(EnvDTE::IExtenderSite)
                DETECT_INTERFACE_ENTRY(EnvDTE::IExtenderProvider)
                DETECT_INTERFACE_ENTRY(EnvDTE::IExtenderProviderUnk)
                DETECT_INTERFACE_ENTRY(EnvDTE::ObjectExtenders)
                DETECT_INTERFACE_ENTRY(EnvDTE::Find)
                DETECT_INTERFACE_ENTRY(EnvDTE::UndoContext)
                DETECT_INTERFACE_ENTRY(EnvDTE::Macros)
                DETECT_INTERFACE_ENTRY(EnvDTE::SourceControl)
                DETECT_INTERFACE_ENTRY(EnvDTE::ISupportVSProperties)
                DETECT_INTERFACE_ENTRY(EnvDTE::IExtensibleObjectSite)
                DETECT_INTERFACE_ENTRY(EnvDTE::IVsProfferCommands)
                DETECT_INTERFACE_ENTRY(EnvDTE::IFilterProperties)
                DETECT_INTERFACE_ENTRY(EnvDTE::TextEditor)
                DETECT_INTERFACE_ENTRY(EnvDTE::_DebuggerEventsRoot)
                DETECT_INTERFACE_ENTRY(EnvDTE::IDTWizard)
                DETECT_INTERFACE_ENTRY(EnvDTE::IVsGlobalsCallback)
                DETECT_INTERFACE_ENTRY(EnvDTE::IVsGlobals)
                DETECT_INTERFACE_ENTRY(EnvDTE::IDTCommandTarget)
                DETECT_INTERFACE_ENTRY(EnvDTE::_ProjectsEvents)
                DETECT_INTERFACE_ENTRY(EnvDTE::_dispProjectsEvents)
                DETECT_INTERFACE_ENTRY(EnvDTE::_MiscSlnFilesEventsRoot)
                DETECT_INTERFACE_ENTRY(EnvDTE::_CommandBarControlEvents)
                DETECT_INTERFACE_ENTRY(EnvDTE::_dispCommandBarControlEvents)
                DETECT_INTERFACE_ENTRY(EnvDTE::_OutputWindowEventsRoot)
                DETECT_INTERFACE_ENTRY(EnvDTE::_TaskListEventsRoot)
                DETECT_INTERFACE_ENTRY(EnvDTE::IDTToolsOptionsPage)
                DETECT_INTERFACE_ENTRY(EnvDTE::_EnvironmentWebBrowser)
                DETECT_INTERFACE_ENTRY(EnvDTE::_EnvironmentHelp)
                DETECT_INTERFACE_ENTRY(EnvDTE::ColorableItems)
                DETECT_INTERFACE_ENTRY(EnvDTE::FontsAndColorsItems)
                DETECT_INTERFACE_ENTRY(EnvDTE::_FontsAndColors)
                DETECT_INTERFACE_ENTRY(EnvDTE::_EnvironmentGeneral)
                DETECT_INTERFACE_ENTRY(EnvDTE::_EnvironmentFontsAndColors)
                DETECT_INTERFACE_ENTRY(EnvDTE::_EnvironmentProjectsAndSolution)
                DETECT_INTERFACE_ENTRY(EnvDTE::_EnvironmentDocuments)
                DETECT_INTERFACE_ENTRY(EnvDTE::_EnvironmentTaskList)
                DETECT_INTERFACE_ENTRY(EnvDTE::_EnvironmentKeyboard)
                DETECT_INTERFACE_ENTRY(EnvDTE::IExtensibleObject)
                DETECT_INTERFACE_ENTRY(EnvDTE::IVsTextEditGeneral)
                DETECT_INTERFACE_ENTRY(EnvDTE::IVsTextEditPerLanguage)
                DETECT_INTERFACE_ENTRY(EnvDTE::IVsTextEditFonts)
                DETECT_INTERFACE_ENTRY(EnvDTE::_DTE)
                DETECT_INTERFACE_ENTRY(EnvDTE::Windows)
                DETECT_INTERFACE_ENTRY(EnvDTE::Window)
                DETECT_INTERFACE_ENTRY(EnvDTE::LinkedWindows)
                DETECT_INTERFACE_ENTRY(EnvDTE::TextBuffer)
                DETECT_INTERFACE_ENTRY(EnvDTE::_WindowEventsRoot)
                DETECT_INTERFACE_ENTRY(EnvDTE::HTMLWindow)
                DETECT_INTERFACE_ENTRY(EnvDTE::ProjectItem)
                DETECT_INTERFACE_ENTRY(EnvDTE::ItemOperations)
                DETECT_INTERFACE_ENTRY(EnvDTE::ProjectItems)
                DETECT_INTERFACE_ENTRY(EnvDTE::Project)
                DETECT_INTERFACE_ENTRY(EnvDTE::Projects)
                DETECT_INTERFACE_ENTRY(EnvDTE::Properties)
                DETECT_INTERFACE_ENTRY(EnvDTE::Property)
                DETECT_INTERFACE_ENTRY(EnvDTE::ConfigurationManager)
                DETECT_INTERFACE_ENTRY(EnvDTE::Configuration)
                DETECT_INTERFACE_ENTRY(EnvDTE::Configurations)
                DETECT_INTERFACE_ENTRY(EnvDTE::OutputGroups)
                DETECT_INTERFACE_ENTRY(EnvDTE::OutputGroup)
                DETECT_INTERFACE_ENTRY(EnvDTE::CodeModel)
                DETECT_INTERFACE_ENTRY(EnvDTE::CodeElements)
                DETECT_INTERFACE_ENTRY(EnvDTE::CodeElement)
                DETECT_INTERFACE_ENTRY(EnvDTE::TextPoint)
                DETECT_INTERFACE_ENTRY(EnvDTE::VirtualPoint)
                DETECT_INTERFACE_ENTRY(EnvDTE::CodeAttribute)
                DETECT_INTERFACE_ENTRY(EnvDTE::TextDocument)
                DETECT_INTERFACE_ENTRY(EnvDTE::CommandWindow)
                DETECT_INTERFACE_ENTRY(EnvDTE::_TextEditorEventsRoot)
                DETECT_INTERFACE_ENTRY(EnvDTE::Document)
                DETECT_INTERFACE_ENTRY(EnvDTE::Documents)
                DETECT_INTERFACE_ENTRY(EnvDTE::Events)
                DETECT_INTERFACE_ENTRY(EnvDTE::IVsExtensibility)
                DETECT_INTERFACE_ENTRY(EnvDTE::_DocumentEventsRoot)
                DETECT_INTERFACE_ENTRY(EnvDTE::TextSelection)
                DETECT_INTERFACE_ENTRY(EnvDTE::TextRanges)
                DETECT_INTERFACE_ENTRY(EnvDTE::EditPoint)
                DETECT_INTERFACE_ENTRY(EnvDTE::TextRange)
                DETECT_INTERFACE_ENTRY(EnvDTE::TextPane)
                DETECT_INTERFACE_ENTRY(EnvDTE::TextPanes)
                DETECT_INTERFACE_ENTRY(EnvDTE::TextWindow)
                DETECT_INTERFACE_ENTRY(EnvDTE::CodeType)
                DETECT_INTERFACE_ENTRY(EnvDTE::CodeTypeRef)
                DETECT_INTERFACE_ENTRY(EnvDTE::CodeParameter)
                DETECT_INTERFACE_ENTRY(EnvDTE::CodeFunction)
                DETECT_INTERFACE_ENTRY(EnvDTE::CodeVariable)
                DETECT_INTERFACE_ENTRY(EnvDTE::CodeNamespace)
                DETECT_INTERFACE_ENTRY(EnvDTE::CodeEnum)
                DETECT_INTERFACE_ENTRY(EnvDTE::CodeClass)
                DETECT_INTERFACE_ENTRY(EnvDTE::CodeProperty)
                DETECT_INTERFACE_ENTRY(EnvDTE::CodeInterface)
                DETECT_INTERFACE_ENTRY(EnvDTE::CodeDelegate)
                DETECT_INTERFACE_ENTRY(EnvDTE::CodeStruct)
                DETECT_INTERFACE_ENTRY(EnvDTE::FileCodeModel)
                DETECT_INTERFACE_ENTRY(EnvDTE::ContextAttributes)
                DETECT_INTERFACE_ENTRY(EnvDTE::ContextAttribute)
                DETECT_INTERFACE_ENTRY(EnvDTE::AddIn)
                DETECT_INTERFACE_ENTRY(EnvDTE::AddIns)
                DETECT_INTERFACE_ENTRY(EnvDTE::OutputWindowPane)
                DETECT_INTERFACE_ENTRY(EnvDTE::OutputWindowPanes)
                DETECT_INTERFACE_ENTRY(EnvDTE::OutputWindow)
                DETECT_INTERFACE_ENTRY(EnvDTE::TaskItem)
                DETECT_INTERFACE_ENTRY(EnvDTE::TaskItems)
                DETECT_INTERFACE_ENTRY(EnvDTE::TaskList)
                DETECT_INTERFACE_ENTRY(EnvDTE::Process)
                DETECT_INTERFACE_ENTRY(EnvDTE::Programs)
                DETECT_INTERFACE_ENTRY(EnvDTE::Program)
                DETECT_INTERFACE_ENTRY(EnvDTE::Threads)
                DETECT_INTERFACE_ENTRY(EnvDTE::Thread)
                DETECT_INTERFACE_ENTRY(EnvDTE::StackFrames)
                DETECT_INTERFACE_ENTRY(EnvDTE::StackFrame)
                DETECT_INTERFACE_ENTRY(EnvDTE::Expressions)
                DETECT_INTERFACE_ENTRY(EnvDTE::Expression)
                DETECT_INTERFACE_ENTRY(EnvDTE::Debugger)
                DETECT_INTERFACE_ENTRY(EnvDTE::Processes)
                DETECT_INTERFACE_ENTRY(EnvDTE::Breakpoints)
                DETECT_INTERFACE_ENTRY(EnvDTE::Breakpoint)
                DETECT_INTERFACE_ENTRY(EnvDTE::Languages)
                DETECT_INTERFACE_ENTRY(EnvDTE::Language)
                DETECT_INTERFACE_ENTRY(EnvDTE::_Solution)
                DETECT_INTERFACE_ENTRY(EnvDTE::SolutionBuild)
                DETECT_INTERFACE_ENTRY(EnvDTE::SolutionConfiguration)
                DETECT_INTERFACE_ENTRY(EnvDTE::SolutionConfigurations)
                DETECT_INTERFACE_ENTRY(EnvDTE::SolutionContexts)
                DETECT_INTERFACE_ENTRY(EnvDTE::SolutionContext)
                DETECT_INTERFACE_ENTRY(EnvDTE::BuildDependencies)
                DETECT_INTERFACE_ENTRY(EnvDTE::BuildDependency)
                
                //Command routing order:
                //  Add-ins and specially registered VsPackages
                //  Active Window 
                //  Active Project (VsHierarchy) and its parents
                //    Parent Hierarchy has priority over child Hierarchy
                //  Global Environment & VsPackages 
                DETECT_INTERFACE_ENTRY(EnvDTE::Commands)   //�������еĶ�����һ���������ɶ��ַ�ʽ����
                DETECT_INTERFACE_ENTRY(EnvDTE::Command)    //���� -- ע��ʹ����������û�����(CommandBarControl)������
                DETECT_INTERFACE_ENTRY(EnvDTE::SelectedItems)
                DETECT_INTERFACE_ENTRY(EnvDTE::SelectedItem)
                DETECT_INTERFACE_ENTRY(EnvDTE::SelectionContainer)
                DETECT_INTERFACE_ENTRY(EnvDTE::WindowConfigurations)
                DETECT_INTERFACE_ENTRY(EnvDTE::WindowConfiguration)
                DETECT_INTERFACE_ENTRY(EnvDTE::ToolBox)
                DETECT_INTERFACE_ENTRY(EnvDTE::ToolBoxTab)
                DETECT_INTERFACE_ENTRY(EnvDTE::ToolBoxTabs)
                DETECT_INTERFACE_ENTRY(EnvDTE::ToolBoxItems)
                DETECT_INTERFACE_ENTRY(EnvDTE::ToolBoxItem)
                DETECT_INTERFACE_ENTRY(EnvDTE::UIHierarchyItem)
                DETECT_INTERFACE_ENTRY(EnvDTE::UIHierarchyItems)
                DETECT_INTERFACE_ENTRY(EnvDTE::UIHierarchy)
                //DETECT_INTERFACE_ENTRY(EnvDTE::XXXXXXXXXXXXXXXXX)
            //}
            //{
                //using namespace EnvDTE80;

                DETECT_INTERFACE_ENTRY(EnvDTE80::Commands2)
                DETECT_INTERFACE_ENTRY(EnvDTE80::SourceControlBindings)
                DETECT_INTERFACE_ENTRY(EnvDTE80::SourceControl2)
                DETECT_INTERFACE_ENTRY(EnvDTE80::CodeAttributeArgument)
                DETECT_INTERFACE_ENTRY(EnvDTE80::CodeEvent)
                DETECT_INTERFACE_ENTRY(EnvDTE80::CodeElement2)
                DETECT_INTERFACE_ENTRY(EnvDTE80::CodeImport)
                DETECT_INTERFACE_ENTRY(EnvDTE80::FileCodeModel2)
                DETECT_INTERFACE_ENTRY(EnvDTE80::CodeModel2)
                DETECT_INTERFACE_ENTRY(EnvDTE80::CodeClass2)
                DETECT_INTERFACE_ENTRY(EnvDTE80::CodeParameter2)
                DETECT_INTERFACE_ENTRY(EnvDTE80::CodeFunction2)
                DETECT_INTERFACE_ENTRY(EnvDTE80::CodeAttribute2)
                DETECT_INTERFACE_ENTRY(EnvDTE80::CodeVariable2)
                DETECT_INTERFACE_ENTRY(EnvDTE80::CodeDelegate2)
                DETECT_INTERFACE_ENTRY(EnvDTE80::CodeStruct2)
                DETECT_INTERFACE_ENTRY(EnvDTE80::CodeInterface2)
                DETECT_INTERFACE_ENTRY(EnvDTE80::CodeTypeRef2)
                DETECT_INTERFACE_ENTRY(EnvDTE80::CodeProperty2)
                DETECT_INTERFACE_ENTRY(EnvDTE80::_dispCodeModelEvents)
                DETECT_INTERFACE_ENTRY(EnvDTE80::_CodeModelEvents)
                DETECT_INTERFACE_ENTRY(EnvDTE80::_CodeModelEventsRoot)
                DETECT_INTERFACE_ENTRY(EnvDTE80::_dispDebuggerProcessEvents)
                DETECT_INTERFACE_ENTRY(EnvDTE80::_DebuggerProcessEventsRoot)
                DETECT_INTERFACE_ENTRY(EnvDTE80::_DebuggerProcessEvents)
                DETECT_INTERFACE_ENTRY(EnvDTE80::_dispDebuggerExpressionEvaluationEvents)
                DETECT_INTERFACE_ENTRY(EnvDTE80::_DebuggerExpressionEvaluationEvents)
                DETECT_INTERFACE_ENTRY(EnvDTE80::_DebuggerExpressionEvaluationEventsRoot)
                DETECT_INTERFACE_ENTRY(EnvDTE80::Windows2)
                DETECT_INTERFACE_ENTRY(EnvDTE80::Window2)
                DETECT_INTERFACE_ENTRY(EnvDTE80::WindowVisibilityEventsRoot)
                DETECT_INTERFACE_ENTRY(EnvDTE80::_WindowVisibilityEvents)
                DETECT_INTERFACE_ENTRY(EnvDTE80::_dispWindowVisibilityEvents)
                DETECT_INTERFACE_ENTRY(EnvDTE80::_TextDocumentKeyPressEventsRoot)
                DETECT_INTERFACE_ENTRY(EnvDTE80::_TextDocumentKeyPressEvents)
                DETECT_INTERFACE_ENTRY(EnvDTE80::_dispTextDocumentKeyPressEvents)
                DETECT_INTERFACE_ENTRY(EnvDTE80::_PublishEvents)
                DETECT_INTERFACE_ENTRY(EnvDTE80::_dispPublishEvents)
                DETECT_INTERFACE_ENTRY(EnvDTE80::Events2)
                DETECT_INTERFACE_ENTRY(EnvDTE80::Solution2)
                DETECT_INTERFACE_ENTRY(EnvDTE80::SolutionFolder)
                DETECT_INTERFACE_ENTRY(EnvDTE80::TaskItems2)
                DETECT_INTERFACE_ENTRY(EnvDTE80::EditPoint2)
                DETECT_INTERFACE_ENTRY(EnvDTE80::IVsExtensibility2)
                DETECT_INTERFACE_ENTRY(EnvDTE80::IInternalExtenderProvider)
                DETECT_INTERFACE_ENTRY(EnvDTE80::Find2)
                DETECT_INTERFACE_ENTRY(EnvDTE80::LifetimeInformation)
                DETECT_INTERFACE_ENTRY(EnvDTE80::ToolBoxItem2)
                DETECT_INTERFACE_ENTRY(EnvDTE80::ToolBoxTab2)
                DETECT_INTERFACE_ENTRY(EnvDTE80::IncrementalSearch)
                DETECT_INTERFACE_ENTRY(EnvDTE80::TextPane2)
                DETECT_INTERFACE_ENTRY(EnvDTE80::SolutionConfiguration2)
                DETECT_INTERFACE_ENTRY(EnvDTE80::IVsProfferCommands2)
                DETECT_INTERFACE_ENTRY(EnvDTE80::SolutionBuild2)
                DETECT_INTERFACE_ENTRY(EnvDTE80::ErrorItem)
                DETECT_INTERFACE_ENTRY(EnvDTE80::ErrorItems)
                DETECT_INTERFACE_ENTRY(EnvDTE80::ErrorList)
                DETECT_INTERFACE_ENTRY(EnvDTE80::ToolWindows) //�������ߴ��ڣ��� Solution Explorer��Error List��
                DETECT_INTERFACE_ENTRY(EnvDTE80::DTE2)
                DETECT_INTERFACE_ENTRY(EnvDTE80::Debugger2)
                DETECT_INTERFACE_ENTRY(EnvDTE80::Transport)
                DETECT_INTERFACE_ENTRY(EnvDTE80::Transports)
                DETECT_INTERFACE_ENTRY(EnvDTE80::Process2)
                DETECT_INTERFACE_ENTRY(EnvDTE80::Breakpoint2)
                DETECT_INTERFACE_ENTRY(EnvDTE80::Engines)
                DETECT_INTERFACE_ENTRY(EnvDTE80::Engine)
            //}
            //{
                //using namespace Microsoft_VisualStudio_CommandBars;
                //VS IDE ʹ�� Office CommandBars
                DETECT_INTERFACE_ENTRY(Microsoft_VisualStudio_CommandBars::_IVsMsoDispObj)

                //typedef Microsoft_VisualStudio_CommandBars::IAccessible     IVSCommandBarsAccessible;
                DETECT_INTERFACE_ENTRY(Microsoft_VisualStudio_CommandBars::IAccessible)  //ambiguous with oleacc.h 556 --IAccessible, So Change Name

                DETECT_INTERFACE_ENTRY(Microsoft_VisualStudio_CommandBars::_IVsMsoOleAccDispObj)
                DETECT_INTERFACE_ENTRY(Microsoft_VisualStudio_CommandBars::ICommandBarsEvents)
                DETECT_INTERFACE_ENTRY(Microsoft_VisualStudio_CommandBars::_CommandBarsEvents)
                DETECT_INTERFACE_ENTRY(Microsoft_VisualStudio_CommandBars::ICommandBarComboBoxEvents)
                DETECT_INTERFACE_ENTRY(Microsoft_VisualStudio_CommandBars::_CommandBarComboBoxEvents)
                DETECT_INTERFACE_ENTRY(Microsoft_VisualStudio_CommandBars::ICommandBarButtonEvents)
                DETECT_INTERFACE_ENTRY(Microsoft_VisualStudio_CommandBars::_CommandBarButtonEvents)
                DETECT_INTERFACE_ENTRY(Microsoft_VisualStudio_CommandBars::CommandBarControl)       //�˵���(����ʵ����Ϊ XXXButton/Combox ��)���� Tools �˵����е� Options
                DETECT_INTERFACE_ENTRY(Microsoft_VisualStudio_CommandBars::_CommandBarButton)
                DETECT_INTERFACE_ENTRY(Microsoft_VisualStudio_CommandBars::_CommandBarComboBox)
                DETECT_INTERFACE_ENTRY(Microsoft_VisualStudio_CommandBars::_CommandBarActiveX)
                DETECT_INTERFACE_ENTRY(Microsoft_VisualStudio_CommandBars::CommandBar)          //һ���ض��Ĳ˵���������
                DETECT_INTERFACE_ENTRY(Microsoft_VisualStudio_CommandBars::CommandBarControls)  //�����˵����������ϵİ�ť���� Tools �еĶ���˵���
                DETECT_INTERFACE_ENTRY(Microsoft_VisualStudio_CommandBars::_CommandBars)    //���еĲ˵����������ȣ���Լ300���
                DETECT_INTERFACE_ENTRY(Microsoft_VisualStudio_CommandBars::CommandBarPopup)
                //DETECT_INTERFACE_ENTRY(XXXXXXXXXXXXXX)
            //}

#endif //INCLUDE_DETECT_DTE

#if INCLUDE_DETECT_DSHOWASF
            DETECT_INTERFACE_ENTRY(IConfigAsfWriter)
#endif //INCLUDE_DETECT_DSHOWASF

#if INCLUDE_DETECT_EXDISP
            DETECT_INTERFACE_ENTRY(IWebBrowser)
            DETECT_INTERFACE_ENTRY(DWebBrowserEvents)
            DETECT_INTERFACE_ENTRY(IWebBrowserApp)
            DETECT_INTERFACE_ENTRY(IWebBrowser2)
            DETECT_INTERFACE_ENTRY(DWebBrowserEvents2)
            DETECT_INTERFACE_ENTRY(DShellWindowsEvents)
            DETECT_INTERFACE_ENTRY(IShellWindows)
            DETECT_INTERFACE_ENTRY(IShellUIHelper)
            DETECT_INTERFACE_ENTRY(IShellUIHelper2)
            DETECT_INTERFACE_ENTRY(DShellNameSpaceEvents)
            DETECT_INTERFACE_ENTRY(IShellFavoritesNameSpace)
            DETECT_INTERFACE_ENTRY(IShellNameSpace)
            DETECT_INTERFACE_ENTRY(IScriptErrorList)
#endif 

#if INCLUDE_DETECT_KSPROXY  //STATIC_CLSID_Proxy
            DETECT_INTERFACE_ENTRY(IKsAggregateControl)
            DETECT_INTERFACE_ENTRY_IID(IKsAllocator,IID_IKsAllocator)
            DETECT_INTERFACE_ENTRY(IKsAllocatorEx)
            DETECT_INTERFACE_ENTRY(IKsClockPropertySet)
            DETECT_INTERFACE_ENTRY(IKsControl)
            DETECT_INTERFACE_ENTRY(IKsDataTypeHandler)
            DETECT_INTERFACE_ENTRY(IKsDataTypeCompletion)
            DETECT_INTERFACE_ENTRY(IKsInterfaceHandler)
            DETECT_INTERFACE_ENTRY(IKsNotifyEvent)
            DETECT_INTERFACE_ENTRY(IKsObject)
            DETECT_INTERFACE_ENTRY(IKsPin)
            DETECT_INTERFACE_ENTRY(IKsPinEx)
            DETECT_INTERFACE_ENTRY(IKsPinPipe)
            DETECT_INTERFACE_ENTRY(IKsPinFactory)
            DETECT_INTERFACE_ENTRY(IKsQualityForwarder)
            DETECT_INTERFACE_ENTRY(IKsPropertySet)
            DETECT_INTERFACE_ENTRY(IKsTopology)
#endif //INCLUDE_DETECT_KSPROXY

#if INCLUDE_DETECT_MEDIAOBJ
            DETECT_INTERFACE_ENTRY(IMediaBuffer)
            DETECT_INTERFACE_ENTRY(IMediaObject)
            DETECT_INTERFACE_ENTRY(IEnumDMO)
            DETECT_INTERFACE_ENTRY(IMediaObjectInPlace)
            DETECT_INTERFACE_ENTRY(IDMOQualityControl)
            DETECT_INTERFACE_ENTRY(IDMOVideoOutputOptimizations)
#endif //INCLUDE_DETECT_MEDIAOBJ

#if INCLUDE_DETECT_MSHTMLC
            //Mshtmlc.h
            DETECT_INTERFACE_ENTRY(IHTMLFiltersCollection)
            DETECT_INTERFACE_ENTRY(IHTMLEventObj)
            DETECT_INTERFACE_ENTRY(IElementBehaviorSite)
            DETECT_INTERFACE_ENTRY(IElementBehavior)
            DETECT_INTERFACE_ENTRY(IElementBehaviorFactory)
            DETECT_INTERFACE_ENTRY(IElementBehaviorSiteOM)
            DETECT_INTERFACE_ENTRY(IElementBehaviorRender)
            DETECT_INTERFACE_ENTRY(IElementBehaviorSiteRender)
            DETECT_INTERFACE_ENTRY(IHTMLStyle)
            DETECT_INTERFACE_ENTRY(IHTMLStyle2)
            DETECT_INTERFACE_ENTRY(IHTMLStyle3)
            DETECT_INTERFACE_ENTRY(IHTMLStyle4)
            DETECT_INTERFACE_ENTRY(IHTMLRuleStyle)
            DETECT_INTERFACE_ENTRY(IHTMLRuleStyle2)
            DETECT_INTERFACE_ENTRY(IHTMLRuleStyle3)
            DETECT_INTERFACE_ENTRY(IHTMLRuleStyle4)
            DETECT_INTERFACE_ENTRY(DispHTMLStyle)
            DETECT_INTERFACE_ENTRY(DispHTMLRuleStyle)
            DETECT_INTERFACE_ENTRY(IHTMLRenderStyle)
            DETECT_INTERFACE_ENTRY(DispHTMLRenderStyle)
            DETECT_INTERFACE_ENTRY(IHTMLCurrentStyle)
            DETECT_INTERFACE_ENTRY(IHTMLCurrentStyle2)
            DETECT_INTERFACE_ENTRY(IHTMLCurrentStyle3)
            DETECT_INTERFACE_ENTRY(DispHTMLCurrentStyle)
            DETECT_INTERFACE_ENTRY(IHTMLElement)
            DETECT_INTERFACE_ENTRY(IHTMLRect)
            DETECT_INTERFACE_ENTRY(IHTMLRectCollection)
            DETECT_INTERFACE_ENTRY(IHTMLDOMNode)
            DETECT_INTERFACE_ENTRY(IHTMLDOMNode2)
            DETECT_INTERFACE_ENTRY(IHTMLDOMAttribute)
            DETECT_INTERFACE_ENTRY(IHTMLDOMAttribute2)
            DETECT_INTERFACE_ENTRY(IHTMLDOMTextNode)
            DETECT_INTERFACE_ENTRY(IHTMLDOMTextNode2)
            DETECT_INTERFACE_ENTRY(IHTMLDOMImplementation)
            DETECT_INTERFACE_ENTRY(DispHTMLDOMAttribute)
            DETECT_INTERFACE_ENTRY(DispHTMLDOMTextNode)
            DETECT_INTERFACE_ENTRY(DispHTMLDOMImplementation)
            DETECT_INTERFACE_ENTRY(IHTMLAttributeCollection)
            DETECT_INTERFACE_ENTRY(IHTMLAttributeCollection2)
            DETECT_INTERFACE_ENTRY(IHTMLDOMChildrenCollection)
            DETECT_INTERFACE_ENTRY(DispHTMLAttributeCollection)
            DETECT_INTERFACE_ENTRY(DispDOMChildrenCollection)
            DETECT_INTERFACE_ENTRY(HTMLElementEvents2)
            DETECT_INTERFACE_ENTRY(HTMLElementEvents)
            DETECT_INTERFACE_ENTRY(IHTMLElementCollection)
            DETECT_INTERFACE_ENTRY(IHTMLFiltersCollection)
            DETECT_INTERFACE_ENTRY(IHTMLElement2)
            DETECT_INTERFACE_ENTRY(IHTMLElement3)
            DETECT_INTERFACE_ENTRY(IHTMLElement4)
            DETECT_INTERFACE_ENTRY(IHTMLElementRender)
            DETECT_INTERFACE_ENTRY(IHTMLUniqueName)
            DETECT_INTERFACE_ENTRY(IHTMLDatabinding)
            DETECT_INTERFACE_ENTRY(IHTMLDocument)
            DETECT_INTERFACE_ENTRY(IHTMLElementDefaults)
            DETECT_INTERFACE_ENTRY(DispHTMLDefaults)
            DETECT_INTERFACE_ENTRY(IHTCDefaultDispatch)
            DETECT_INTERFACE_ENTRY(IHTCPropertyBehavior)
            DETECT_INTERFACE_ENTRY(IHTCMethodBehavior)
            DETECT_INTERFACE_ENTRY(IHTCEventBehavior)
            DETECT_INTERFACE_ENTRY(IHTCAttachBehavior)
            DETECT_INTERFACE_ENTRY(IHTCAttachBehavior2)
            DETECT_INTERFACE_ENTRY(IHTCDescBehavior)
            DETECT_INTERFACE_ENTRY(DispHTCDefaultDispatch)
            DETECT_INTERFACE_ENTRY(DispHTCPropertyBehavior)
            DETECT_INTERFACE_ENTRY(DispHTCMethodBehavior)
            DETECT_INTERFACE_ENTRY(DispHTCEventBehavior)
            DETECT_INTERFACE_ENTRY(DispHTCAttachBehavior)
            DETECT_INTERFACE_ENTRY(DispHTCDescBehavior)
            DETECT_INTERFACE_ENTRY(IHTMLUrnCollection)
            DETECT_INTERFACE_ENTRY(IHTMLGenericElement)
            DETECT_INTERFACE_ENTRY(DispHTMLGenericElement)
            DETECT_INTERFACE_ENTRY(IHTMLStyleSheetRule)
            DETECT_INTERFACE_ENTRY(IHTMLStyleSheetRulesCollection)
            DETECT_INTERFACE_ENTRY(IHTMLStyleSheetPage)
            DETECT_INTERFACE_ENTRY(IHTMLStyleSheetPagesCollection)
            DETECT_INTERFACE_ENTRY(IHTMLStyleSheetsCollection)
            DETECT_INTERFACE_ENTRY(IHTMLStyleSheet)
            DETECT_INTERFACE_ENTRY(IHTMLStyleSheet2)
            DETECT_INTERFACE_ENTRY(DispHTMLStyleSheet)
            DETECT_INTERFACE_ENTRY(HTMLLinkElementEvents2)
            DETECT_INTERFACE_ENTRY(HTMLLinkElementEvents)
            DETECT_INTERFACE_ENTRY(IHTMLLinkElement)
            DETECT_INTERFACE_ENTRY(IHTMLLinkElement2)
            DETECT_INTERFACE_ENTRY(IHTMLLinkElement3)
            DETECT_INTERFACE_ENTRY(DispHTMLLinkElement)
            DETECT_INTERFACE_ENTRY(IHTMLTxtRange)
            DETECT_INTERFACE_ENTRY(IHTMLTextRangeMetrics)
            DETECT_INTERFACE_ENTRY(IHTMLTextRangeMetrics2)
            DETECT_INTERFACE_ENTRY(IHTMLTxtRangeCollection)
            DETECT_INTERFACE_ENTRY(HTMLFormElementEvents2)
            DETECT_INTERFACE_ENTRY(HTMLFormElementEvents)
            DETECT_INTERFACE_ENTRY(IHTMLFormElement)
            DETECT_INTERFACE_ENTRY(IHTMLFormElement2)
            DETECT_INTERFACE_ENTRY(IHTMLFormElement3)
            DETECT_INTERFACE_ENTRY(IHTMLSubmitData)
            DETECT_INTERFACE_ENTRY(DispHTMLFormElement)
            DETECT_INTERFACE_ENTRY(HTMLControlElementEvents2)
            DETECT_INTERFACE_ENTRY(HTMLControlElementEvents)
            DETECT_INTERFACE_ENTRY(IHTMLControlElement)
            DETECT_INTERFACE_ENTRY(IHTMLTextElement)
            DETECT_INTERFACE_ENTRY(DispHTMLTextElement)
            DETECT_INTERFACE_ENTRY(HTMLTextContainerEvents2)
            DETECT_INTERFACE_ENTRY(HTMLTextContainerEvents)
            DETECT_INTERFACE_ENTRY(IHTMLTextContainer)
            DETECT_INTERFACE_ENTRY(IHTMLControlRange)
            DETECT_INTERFACE_ENTRY(IHTMLControlRange2)
            DETECT_INTERFACE_ENTRY(HTMLImgEvents2)
            DETECT_INTERFACE_ENTRY(HTMLImgEvents)
            DETECT_INTERFACE_ENTRY(IHTMLImgElement)
            DETECT_INTERFACE_ENTRY(IHTMLImgElement2)
            DETECT_INTERFACE_ENTRY(IHTMLImageElementFactory)
            DETECT_INTERFACE_ENTRY(DispHTMLImg)
            DETECT_INTERFACE_ENTRY(IHTMLBodyElement)
            DETECT_INTERFACE_ENTRY(IHTMLBodyElement2)
            DETECT_INTERFACE_ENTRY(DispHTMLBody)
            DETECT_INTERFACE_ENTRY(IHTMLFontElement)
            DETECT_INTERFACE_ENTRY(DispHTMLFontElement)
            DETECT_INTERFACE_ENTRY(HTMLAnchorEvents2)
            DETECT_INTERFACE_ENTRY(HTMLAnchorEvents)
            DETECT_INTERFACE_ENTRY(IHTMLAnchorElement)
            DETECT_INTERFACE_ENTRY(IHTMLAnchorElement2)
            DETECT_INTERFACE_ENTRY(DispHTMLAnchorElement)
            DETECT_INTERFACE_ENTRY(HTMLLabelEvents2)
            DETECT_INTERFACE_ENTRY(HTMLLabelEvents)
            DETECT_INTERFACE_ENTRY(IHTMLLabelElement)
            DETECT_INTERFACE_ENTRY(IHTMLLabelElement2)
            DETECT_INTERFACE_ENTRY(DispHTMLLabelElement)
            DETECT_INTERFACE_ENTRY(IHTMLListElement)
            DETECT_INTERFACE_ENTRY(IHTMLListElement2)
            DETECT_INTERFACE_ENTRY(DispHTMLListElement)
            DETECT_INTERFACE_ENTRY(IHTMLUListElement)
            DETECT_INTERFACE_ENTRY(DispHTMLUListElement)
            DETECT_INTERFACE_ENTRY(IHTMLOListElement)
            DETECT_INTERFACE_ENTRY(DispHTMLOListElement)
            DETECT_INTERFACE_ENTRY(IHTMLLIElement)
            DETECT_INTERFACE_ENTRY(DispHTMLLIElement)
            DETECT_INTERFACE_ENTRY(IHTMLBlockElement)
            DETECT_INTERFACE_ENTRY(IHTMLBlockElement2)
            DETECT_INTERFACE_ENTRY(DispHTMLBlockElement)
            DETECT_INTERFACE_ENTRY(IHTMLDivElement)
            DETECT_INTERFACE_ENTRY(DispHTMLDivElement)
            DETECT_INTERFACE_ENTRY(IHTMLDDElement)
            DETECT_INTERFACE_ENTRY(DispHTMLDDElement)
            DETECT_INTERFACE_ENTRY(IHTMLDTElement)
            DETECT_INTERFACE_ENTRY(DispHTMLDTElement)
            DETECT_INTERFACE_ENTRY(IHTMLBRElement)
            DETECT_INTERFACE_ENTRY(DispHTMLBRElement)
            DETECT_INTERFACE_ENTRY(IHTMLDListElement)
            DETECT_INTERFACE_ENTRY(DispHTMLDListElement)
            DETECT_INTERFACE_ENTRY(IHTMLHRElement)
            DETECT_INTERFACE_ENTRY(DispHTMLHRElement)
            DETECT_INTERFACE_ENTRY(IHTMLParaElement)
            DETECT_INTERFACE_ENTRY(DispHTMLParaElement)
            DETECT_INTERFACE_ENTRY(IHTMLElementCollection2)
            DETECT_INTERFACE_ENTRY(IHTMLElementCollection3)
            DETECT_INTERFACE_ENTRY(DispHTMLElementCollection)
            DETECT_INTERFACE_ENTRY(IHTMLHeaderElement)
            DETECT_INTERFACE_ENTRY(DispHTMLHeaderElement)
            DETECT_INTERFACE_ENTRY(HTMLSelectElementEvents2)
            DETECT_INTERFACE_ENTRY(HTMLSelectElementEvents)
            DETECT_INTERFACE_ENTRY(IHTMLSelectElement)
            DETECT_INTERFACE_ENTRY(IHTMLSelectElement2)
            DETECT_INTERFACE_ENTRY(IHTMLSelectElement4)
            DETECT_INTERFACE_ENTRY(DispHTMLSelectElement)
            DETECT_INTERFACE_ENTRY(IHTMLSelectionObject)
            DETECT_INTERFACE_ENTRY(IHTMLSelectionObject2)
            DETECT_INTERFACE_ENTRY(IHTMLOptionElement)
            DETECT_INTERFACE_ENTRY(IHTMLOptionElement3)
            DETECT_INTERFACE_ENTRY(IHTMLOptionElementFactory)
            DETECT_INTERFACE_ENTRY(DispHTMLOptionElement)
            DETECT_INTERFACE_ENTRY(HTMLButtonElementEvents2)
            DETECT_INTERFACE_ENTRY(HTMLButtonElementEvents)
            DETECT_INTERFACE_ENTRY(HTMLInputTextElementEvents2)
            DETECT_INTERFACE_ENTRY(HTMLOptionButtonElementEvents2)
            DETECT_INTERFACE_ENTRY(HTMLInputFileElementEvents2)
            DETECT_INTERFACE_ENTRY(HTMLInputImageEvents2)
            DETECT_INTERFACE_ENTRY(HTMLInputTextElementEvents)
            DETECT_INTERFACE_ENTRY(HTMLOptionButtonElementEvents)
            DETECT_INTERFACE_ENTRY(HTMLInputFileElementEvents)
            DETECT_INTERFACE_ENTRY(HTMLInputImageEvents)
            DETECT_INTERFACE_ENTRY(IHTMLInputElement)
            DETECT_INTERFACE_ENTRY(IHTMLInputElement2)
            DETECT_INTERFACE_ENTRY(IHTMLInputButtonElement)
            DETECT_INTERFACE_ENTRY(IHTMLInputHiddenElement)
            DETECT_INTERFACE_ENTRY(IHTMLInputTextElement)
            DETECT_INTERFACE_ENTRY(IHTMLInputFileElement)
            DETECT_INTERFACE_ENTRY(IHTMLOptionButtonElement)
            DETECT_INTERFACE_ENTRY(IHTMLInputImage)
            DETECT_INTERFACE_ENTRY(DispHTMLInputElement)
            DETECT_INTERFACE_ENTRY(IHTMLTextAreaElement)
            DETECT_INTERFACE_ENTRY(DispHTMLTextAreaElement)
            DETECT_INTERFACE_ENTRY(DispHTMLRichtextElement)
            DETECT_INTERFACE_ENTRY(IHTMLButtonElement)
            DETECT_INTERFACE_ENTRY(DispHTMLButtonElement)
            DETECT_INTERFACE_ENTRY(HTMLMarqueeElementEvents2)
            DETECT_INTERFACE_ENTRY(HTMLMarqueeElementEvents)
            DETECT_INTERFACE_ENTRY(IHTMLMarqueeElement)
            DETECT_INTERFACE_ENTRY(DispHTMLMarqueeElement)
            DETECT_INTERFACE_ENTRY(IHTMLHtmlElement)
            DETECT_INTERFACE_ENTRY(IHTMLHeadElement)
            DETECT_INTERFACE_ENTRY(IHTMLTitleElement)
            DETECT_INTERFACE_ENTRY(IHTMLMetaElement)
            DETECT_INTERFACE_ENTRY(IHTMLMetaElement2)
            DETECT_INTERFACE_ENTRY(IHTMLBaseElement)
            DETECT_INTERFACE_ENTRY(IHTMLIsIndexElement)
            DETECT_INTERFACE_ENTRY(IHTMLIsIndexElement2)
            DETECT_INTERFACE_ENTRY(IHTMLNextIdElement)
            DETECT_INTERFACE_ENTRY(DispHTMLHtmlElement)
            DETECT_INTERFACE_ENTRY(DispHTMLHeadElement)
            DETECT_INTERFACE_ENTRY(DispHTMLTitleElement)
            DETECT_INTERFACE_ENTRY(DispHTMLMetaElement)
            DETECT_INTERFACE_ENTRY(DispHTMLBaseElement)
            DETECT_INTERFACE_ENTRY(DispHTMLIsIndexElement)
            DETECT_INTERFACE_ENTRY(DispHTMLNextIdElement)
            DETECT_INTERFACE_ENTRY(IHTMLBaseFontElement)
            DETECT_INTERFACE_ENTRY(DispHTMLBaseFontElement)
            DETECT_INTERFACE_ENTRY(IHTMLUnknownElement)
            DETECT_INTERFACE_ENTRY(DispHTMLUnknownElement)
            DETECT_INTERFACE_ENTRY(IOmHistory)
            DETECT_INTERFACE_ENTRY(IHTMLMimeTypesCollection)
            DETECT_INTERFACE_ENTRY(IHTMLPluginsCollection)
            DETECT_INTERFACE_ENTRY(IHTMLOpsProfile)
            DETECT_INTERFACE_ENTRY(IOmNavigator)
            DETECT_INTERFACE_ENTRY(IHTMLLocation)
            DETECT_INTERFACE_ENTRY(IHTMLBookmarkCollection)
            DETECT_INTERFACE_ENTRY(IHTMLDataTransfer)
            DETECT_INTERFACE_ENTRY(IHTMLEventObj2)
            DETECT_INTERFACE_ENTRY(IHTMLEventObj3)
            DETECT_INTERFACE_ENTRY(IHTMLEventObj4)
            DETECT_INTERFACE_ENTRY(DispCEventObj)
            DETECT_INTERFACE_ENTRY(HTMLWindowEvents2)
            DETECT_INTERFACE_ENTRY(HTMLWindowEvents)
            DETECT_INTERFACE_ENTRY(IHTMLDocument2)
            DETECT_INTERFACE_ENTRY(IHTMLFramesCollection2)
            DETECT_INTERFACE_ENTRY(IHTMLWindow2)
            DETECT_INTERFACE_ENTRY(IHTMLWindow3)
            DETECT_INTERFACE_ENTRY(IHTMLFrameBase)
            DETECT_INTERFACE_ENTRY(IHTMLScreen)
            DETECT_INTERFACE_ENTRY(IHTMLScreen2)
            DETECT_INTERFACE_ENTRY(IHTMLWindow4)
            DETECT_INTERFACE_ENTRY(DispHTMLScreen)
            DETECT_INTERFACE_ENTRY(DispHTMLWindow2)
            DETECT_INTERFACE_ENTRY(DispHTMLWindowProxy)
            DETECT_INTERFACE_ENTRY(HTMLDocumentEvents2)
            DETECT_INTERFACE_ENTRY(HTMLDocumentEvents)
            DETECT_INTERFACE_ENTRY(IHTMLDocument3)
            DETECT_INTERFACE_ENTRY(IHTMLDocument4)
            DETECT_INTERFACE_ENTRY(IHTMLDocument5)
            DETECT_INTERFACE_ENTRY(DispHTMLDocument)
            DETECT_INTERFACE_ENTRY(DWebBridgeEvents)
            DETECT_INTERFACE_ENTRY(IWebBridge)
            DETECT_INTERFACE_ENTRY(IWBScriptControl)
            DETECT_INTERFACE_ENTRY(IHTMLEmbedElement)
            DETECT_INTERFACE_ENTRY(DispHTMLEmbed)
            DETECT_INTERFACE_ENTRY(HTMLMapEvents2)
            DETECT_INTERFACE_ENTRY(HTMLMapEvents)
            DETECT_INTERFACE_ENTRY(IHTMLAreasCollection)
            DETECT_INTERFACE_ENTRY(IHTMLAreasCollection2)
            DETECT_INTERFACE_ENTRY(IHTMLAreasCollection3)
            DETECT_INTERFACE_ENTRY(IHTMLMapElement)
            DETECT_INTERFACE_ENTRY(DispHTMLAreasCollection)
            DETECT_INTERFACE_ENTRY(DispHTMLMapElement)
            DETECT_INTERFACE_ENTRY(HTMLAreaEvents2)
            DETECT_INTERFACE_ENTRY(HTMLAreaEvents)
            DETECT_INTERFACE_ENTRY(IHTMLAreaElement)
            DETECT_INTERFACE_ENTRY(DispHTMLAreaElement)
            DETECT_INTERFACE_ENTRY(IHTMLTableCaption)
            DETECT_INTERFACE_ENTRY(DispHTMLTableCaption)
            DETECT_INTERFACE_ENTRY(IHTMLCommentElement)
            DETECT_INTERFACE_ENTRY(IHTMLCommentElement2)
            DETECT_INTERFACE_ENTRY(DispHTMLCommentElement)
            DETECT_INTERFACE_ENTRY(IHTMLPhraseElement)
            DETECT_INTERFACE_ENTRY(IHTMLPhraseElement2)
            DETECT_INTERFACE_ENTRY(IHTMLSpanElement)
            DETECT_INTERFACE_ENTRY(DispHTMLPhraseElement)
            DETECT_INTERFACE_ENTRY(DispHTMLSpanElement)
            DETECT_INTERFACE_ENTRY(HTMLTableEvents2)
            DETECT_INTERFACE_ENTRY(HTMLTableEvents)
            DETECT_INTERFACE_ENTRY(IHTMLTableSection)
            DETECT_INTERFACE_ENTRY(IHTMLTable)
            DETECT_INTERFACE_ENTRY(IHTMLTable2)
            DETECT_INTERFACE_ENTRY(IHTMLTable3)
            DETECT_INTERFACE_ENTRY(IHTMLTableCol)
            DETECT_INTERFACE_ENTRY(IHTMLTableCol2)
            DETECT_INTERFACE_ENTRY(IHTMLTableSection2)
            DETECT_INTERFACE_ENTRY(IHTMLTableSection3)
            DETECT_INTERFACE_ENTRY(IHTMLTableRow)
            DETECT_INTERFACE_ENTRY(IHTMLTableRow2)
            DETECT_INTERFACE_ENTRY(IHTMLTableRow3)
            DETECT_INTERFACE_ENTRY(IHTMLTableRowMetrics)
            DETECT_INTERFACE_ENTRY(IHTMLTableCell)
            DETECT_INTERFACE_ENTRY(IHTMLTableCell2)
            DETECT_INTERFACE_ENTRY(DispHTMLTable)
            DETECT_INTERFACE_ENTRY(DispHTMLTableCol)
            DETECT_INTERFACE_ENTRY(DispHTMLTableSection)
            DETECT_INTERFACE_ENTRY(DispHTMLTableRow)
            DETECT_INTERFACE_ENTRY(DispHTMLTableCell)
            DETECT_INTERFACE_ENTRY(HTMLScriptEvents2)
            DETECT_INTERFACE_ENTRY(HTMLScriptEvents)
            DETECT_INTERFACE_ENTRY(IHTMLScriptElement)
            DETECT_INTERFACE_ENTRY(IHTMLScriptElement2)
            DETECT_INTERFACE_ENTRY(DispHTMLScriptElement)
            DETECT_INTERFACE_ENTRY(IHTMLNoShowElement)
            DETECT_INTERFACE_ENTRY(DispHTMLNoShowElement)
            DETECT_INTERFACE_ENTRY(HTMLObjectElementEvents2)
            DETECT_INTERFACE_ENTRY(HTMLObjectElementEvents)
            DETECT_INTERFACE_ENTRY(IHTMLObjectElement)
            DETECT_INTERFACE_ENTRY(IHTMLObjectElement2)
            DETECT_INTERFACE_ENTRY(IHTMLObjectElement3)
            DETECT_INTERFACE_ENTRY(IHTMLParamElement)
            DETECT_INTERFACE_ENTRY(DispHTMLObjectElement)
            DETECT_INTERFACE_ENTRY(DispHTMLParamElement)
            DETECT_INTERFACE_ENTRY(HTMLFrameSiteEvents2)
            DETECT_INTERFACE_ENTRY(HTMLFrameSiteEvents)
            DETECT_INTERFACE_ENTRY(IHTMLFrameBase2)
            DETECT_INTERFACE_ENTRY(IHTMLFrameBase3)
            DETECT_INTERFACE_ENTRY(DispHTMLFrameBase)
            DETECT_INTERFACE_ENTRY(IHTMLFrameElement)
            DETECT_INTERFACE_ENTRY(IHTMLFrameElement2)
            DETECT_INTERFACE_ENTRY(DispHTMLFrameElement)
            DETECT_INTERFACE_ENTRY(IHTMLIFrameElement)
            DETECT_INTERFACE_ENTRY(IHTMLIFrameElement2)
            DETECT_INTERFACE_ENTRY(DispHTMLIFrame)
            DETECT_INTERFACE_ENTRY(IHTMLDivPosition)
            DETECT_INTERFACE_ENTRY(IHTMLFieldSetElement)
            DETECT_INTERFACE_ENTRY(IHTMLFieldSetElement2)
            DETECT_INTERFACE_ENTRY(IHTMLLegendElement)
            DETECT_INTERFACE_ENTRY(IHTMLLegendElement2)
            DETECT_INTERFACE_ENTRY(DispHTMLDivPosition)
            DETECT_INTERFACE_ENTRY(DispHTMLFieldSetElement)
            DETECT_INTERFACE_ENTRY(DispHTMLLegendElement)
            DETECT_INTERFACE_ENTRY(IHTMLSpanFlow)
            DETECT_INTERFACE_ENTRY(DispHTMLSpanFlow)
            DETECT_INTERFACE_ENTRY(IHTMLFrameSetElement)
            DETECT_INTERFACE_ENTRY(IHTMLFrameSetElement2)
            DETECT_INTERFACE_ENTRY(DispHTMLFrameSetSite)
            DETECT_INTERFACE_ENTRY(IHTMLBGsound)
            DETECT_INTERFACE_ENTRY(DispHTMLBGsound)
            DETECT_INTERFACE_ENTRY(IHTMLFontNamesCollection)
            DETECT_INTERFACE_ENTRY(IHTMLFontSizesCollection)
            DETECT_INTERFACE_ENTRY(IHTMLOptionsHolder)
            DETECT_INTERFACE_ENTRY(HTMLStyleElementEvents2)
            DETECT_INTERFACE_ENTRY(HTMLStyleElementEvents)
            DETECT_INTERFACE_ENTRY(IHTMLStyleElement)
            DETECT_INTERFACE_ENTRY(DispHTMLStyleElement)
            DETECT_INTERFACE_ENTRY(IHTMLStyleFontFace)
            DETECT_INTERFACE_ENTRY(ICSSFilterSite)
            DETECT_INTERFACE_ENTRY(IMarkupPointer)
            DETECT_INTERFACE_ENTRY(IMarkupContainer)
            DETECT_INTERFACE_ENTRY(IMarkupContainer2)
            DETECT_INTERFACE_ENTRY(IHTMLChangeLog)
            DETECT_INTERFACE_ENTRY(IHTMLChangeSink)
            DETECT_INTERFACE_ENTRY(IActiveIMMApp)
            DETECT_INTERFACE_ENTRY(ISegmentList)
            DETECT_INTERFACE_ENTRY(ISegmentListIterator)
            DETECT_INTERFACE_ENTRY(IHTMLCaret)
            DETECT_INTERFACE_ENTRY(ISegment)
            DETECT_INTERFACE_ENTRY(IElementSegment)
            DETECT_INTERFACE_ENTRY(IHighlightSegment)
            DETECT_INTERFACE_ENTRY(IHighlightRenderingServices)
            DETECT_INTERFACE_ENTRY(ILineInfo)
            DETECT_INTERFACE_ENTRY(IDisplayPointer)
            DETECT_INTERFACE_ENTRY(IDisplayServices)
            DETECT_INTERFACE_ENTRY(IHtmlDlgSafeHelper)
            DETECT_INTERFACE_ENTRY(IBlockFormats)
            DETECT_INTERFACE_ENTRY(IFontNames)
            DETECT_INTERFACE_ENTRY(ICSSFilter)
            DETECT_INTERFACE_ENTRY(ISecureUrlHost)
            DETECT_INTERFACE_ENTRY(IMarkupServices)
            DETECT_INTERFACE_ENTRY(IMarkupServices2)
            DETECT_INTERFACE_ENTRY(IHTMLChangePlayback)
            DETECT_INTERFACE_ENTRY(IMarkupPointer2)
            DETECT_INTERFACE_ENTRY(IMarkupTextFrags)
            DETECT_INTERFACE_ENTRY(IXMLGenericParse)
            DETECT_INTERFACE_ENTRY(IHTMLEditHost)
            DETECT_INTERFACE_ENTRY(IHTMLEditHost2)
            DETECT_INTERFACE_ENTRY(ISequenceNumber)
            DETECT_INTERFACE_ENTRY(IIMEServices)
            DETECT_INTERFACE_ENTRY(ISelectionServicesListener)
            DETECT_INTERFACE_ENTRY(ISelectionServices)
            DETECT_INTERFACE_ENTRY(IHTMLEditDesigner)
            DETECT_INTERFACE_ENTRY(IHTMLEditServices)
            DETECT_INTERFACE_ENTRY(IHTMLEditServices2)
            DETECT_INTERFACE_ENTRY(IHTMLComputedStyle)
            DETECT_INTERFACE_ENTRY(HTMLNamespaceEvents)
            DETECT_INTERFACE_ENTRY(IHTMLNamespace)
            DETECT_INTERFACE_ENTRY(IHTMLNamespaceCollection)
            DETECT_INTERFACE_ENTRY(IHTMLPainter)
            DETECT_INTERFACE_ENTRY(IHTMLPaintSite)
            DETECT_INTERFACE_ENTRY(IHTMLPainterEventInfo)
            DETECT_INTERFACE_ENTRY(IHTMLPainterOverlay)
            DETECT_INTERFACE_ENTRY(IHTMLIPrintCollection)
            DETECT_INTERFACE_ENTRY(IEnumPrivacyRecords)
            DETECT_INTERFACE_ENTRY(IHTMLDialog)
            DETECT_INTERFACE_ENTRY(IHTMLDialog2)
            DETECT_INTERFACE_ENTRY(IHTMLDialog3)
            DETECT_INTERFACE_ENTRY(IHTMLModelessInit)
            DETECT_INTERFACE_ENTRY(IHTMLPopup)
            DETECT_INTERFACE_ENTRY(DispHTMLPopup)
            DETECT_INTERFACE_ENTRY(IHTMLAppBehavior)
            DETECT_INTERFACE_ENTRY(IHTMLAppBehavior2)
            DETECT_INTERFACE_ENTRY(IHTMLAppBehavior3)
            DETECT_INTERFACE_ENTRY(DispHTMLAppBehavior)
            DETECT_INTERFACE_ENTRY(DispIHTMLInputButtonElement)
            DETECT_INTERFACE_ENTRY(DispIHTMLInputTextElement)
            DETECT_INTERFACE_ENTRY(DispIHTMLInputFileElement)
            DETECT_INTERFACE_ENTRY(DispIHTMLOptionButtonElement)
            DETECT_INTERFACE_ENTRY(DispIHTMLInputImage)
            DETECT_INTERFACE_ENTRY(IElementNamespace)
            DETECT_INTERFACE_ENTRY(IElementNamespaceTable)
            DETECT_INTERFACE_ENTRY(IElementNamespaceFactory)
            DETECT_INTERFACE_ENTRY(IElementNamespaceFactory2)
            DETECT_INTERFACE_ENTRY(IElementNamespaceFactoryCallback)
            DETECT_INTERFACE_ENTRY(IElementBehaviorSiteOM2)
            DETECT_INTERFACE_ENTRY(IElementBehaviorCategory)
            DETECT_INTERFACE_ENTRY(IElementBehaviorSiteCategory)
            DETECT_INTERFACE_ENTRY(IElementBehaviorSubmit)
            DETECT_INTERFACE_ENTRY(IElementBehaviorFocus)
            DETECT_INTERFACE_ENTRY(IElementBehaviorLayout)
            DETECT_INTERFACE_ENTRY(IElementBehaviorLayout2)
            DETECT_INTERFACE_ENTRY(IElementBehaviorSiteLayout)
            DETECT_INTERFACE_ENTRY(IElementBehaviorSiteLayout2)
            DETECT_INTERFACE_ENTRY(IHostBehaviorInit)
#endif //INCLUDE_DETECT_MSHTMLC


#if INCLUDE_DETECT_MSXML
            //msxml.h
            DETECT_INTERFACE_ENTRY(IXMLDOMImplementation)
            DETECT_INTERFACE_ENTRY(IXMLDOMNode)
            DETECT_INTERFACE_ENTRY(IXMLDOMDocumentFragment)
            DETECT_INTERFACE_ENTRY(IXMLDOMDocument)
            DETECT_INTERFACE_ENTRY(IXMLDOMNodeList)
            DETECT_INTERFACE_ENTRY(IXMLDOMNamedNodeMap)
            DETECT_INTERFACE_ENTRY(IXMLDOMCharacterData)
            DETECT_INTERFACE_ENTRY(IXMLDOMAttribute)
            DETECT_INTERFACE_ENTRY(IXMLDOMElement)
            DETECT_INTERFACE_ENTRY(IXMLDOMText)
            DETECT_INTERFACE_ENTRY(IXMLDOMComment)
            DETECT_INTERFACE_ENTRY(IXMLDOMProcessingInstruction)
            DETECT_INTERFACE_ENTRY(IXMLDOMCDATASection)
            DETECT_INTERFACE_ENTRY(IXMLDOMDocumentType)
            DETECT_INTERFACE_ENTRY(IXMLDOMNotation)
            DETECT_INTERFACE_ENTRY(IXMLDOMEntity)
            DETECT_INTERFACE_ENTRY(IXMLDOMEntityReference)
            DETECT_INTERFACE_ENTRY(IXMLDOMParseError)
            DETECT_INTERFACE_ENTRY(IXTLRuntime)
            DETECT_INTERFACE_ENTRY(XMLDOMDocumentEvents)
            DETECT_INTERFACE_ENTRY(IXMLHttpRequest)
            DETECT_INTERFACE_ENTRY(IXMLDSOControl)
            DETECT_INTERFACE_ENTRY(IXMLElementCollection)
            DETECT_INTERFACE_ENTRY(IXMLDocument)
            DETECT_INTERFACE_ENTRY(IXMLDocument2)
            DETECT_INTERFACE_ENTRY(IXMLElement)
            DETECT_INTERFACE_ENTRY(IXMLElement2)
            DETECT_INTERFACE_ENTRY(IXMLAttribute)
            DETECT_INTERFACE_ENTRY(IXMLError)
#endif //INCLUDE_DETECT_MSXML

#if INCLUDE_DETECT_OAIDL
            //OaIdl.h
            DETECT_INTERFACE_ENTRY(ICreateTypeInfo)
            DETECT_INTERFACE_ENTRY(ICreateTypeInfo2)
            DETECT_INTERFACE_ENTRY(ICreateTypeLib)
            DETECT_INTERFACE_ENTRY(ICreateTypeLib2)

            //�õ�Dispatch�ӿ�ָ��󣬵���GetTypeInfoCount����������1����2����һ���ӿھ������Ժͷ�����������
            //  ��2��ITypeInfo�ӿڿ������¼������Ľӿ�
            DETECT_INTERFACE_ENTRY_EX(IDispatch,CFDispatchDump)
            DETECT_INTERFACE_ENTRY_EX(IEnumVARIANT, CFEnumVARIANTDump)
            DETECT_INTERFACE_ENTRY(ITypeComp)
            DETECT_INTERFACE_ENTRY_EX(ITypeInfo, CFTypeInfoDump)   //�ܻ�ȡȫ����������Ϣ(CComTypeInfoHolder)
            DETECT_INTERFACE_ENTRY(ITypeInfo2)
            DETECT_INTERFACE_ENTRY(ITypeLib)
            DETECT_INTERFACE_ENTRY(ITypeLib2)
            DETECT_INTERFACE_ENTRY(ITypeChangeEvents)
            DETECT_INTERFACE_ENTRY(IErrorInfo)
            DETECT_INTERFACE_ENTRY(ICreateErrorInfo)
            DETECT_INTERFACE_ENTRY(ISupportErrorInfo)   //�ṩ���ḻ��Ϣ�Ĵ������ӿڣ�ͨ�� SetErrorInfo(...IErrorInfo) ������
            DETECT_INTERFACE_ENTRY(ITypeFactory)
            DETECT_INTERFACE_ENTRY(ITypeMarshal)
            DETECT_INTERFACE_ENTRY(IRecordInfo)
            DETECT_INTERFACE_ENTRY(IErrorLog)
            DETECT_INTERFACE_ENTRY(IPropertyBag)
#endif //INCLUDE_DETECT_OAIDL

#if INCLUDE_DETECT_OBJIDL
            //ObjIdl.h
            DETECT_INTERFACE_ENTRY(IMarshal)
            DETECT_INTERFACE_ENTRY(IMarshal2)
            DETECT_INTERFACE_ENTRY(IMalloc)
            DETECT_INTERFACE_ENTRY(IMallocSpy)
            DETECT_INTERFACE_ENTRY(IStdMarshalInfo)
            DETECT_INTERFACE_ENTRY(IExternalConnection)
            DETECT_INTERFACE_ENTRY(IMultiQI)
            DETECT_INTERFACE_ENTRY(AsyncIMultiQI)
            DETECT_INTERFACE_ENTRY(IInternalUnknown)
            DETECT_INTERFACE_ENTRY(IEnumUnknown)
            DETECT_INTERFACE_ENTRY(IBindCtx)
            DETECT_INTERFACE_ENTRY(IEnumMoniker)
            DETECT_INTERFACE_ENTRY(IRunnableObject)
            DETECT_INTERFACE_ENTRY(IRunningObjectTable)
            DETECT_INTERFACE_ENTRY(IPersist)
            DETECT_INTERFACE_ENTRY(IPersistStream)
            DETECT_INTERFACE_ENTRY(IMoniker)
            DETECT_INTERFACE_ENTRY(IROTData)
            DETECT_INTERFACE_ENTRY(IEnumString)
            DETECT_INTERFACE_ENTRY(ISequentialStream)
            DETECT_INTERFACE_ENTRY(IStream)
            DETECT_INTERFACE_ENTRY(IEnumSTATSTG)
            DETECT_INTERFACE_ENTRY(IStorage)
            DETECT_INTERFACE_ENTRY(IPersistFile)
            DETECT_INTERFACE_ENTRY(IPersistStorage)
            DETECT_INTERFACE_ENTRY(ILockBytes)
            DETECT_INTERFACE_ENTRY(IEnumFORMATETC)
            DETECT_INTERFACE_ENTRY(IEnumSTATDATA)
            DETECT_INTERFACE_ENTRY(IRootStorage)
            DETECT_INTERFACE_ENTRY(IAdviseSink) //����ͨ������(OnXXXX)���ͻ�����ͨ�� IDataObject::DAdvise�ѽ���������COM����
            //�¼�����ʱ�����ݶ�����÷�������֪ͨ
            DETECT_INTERFACE_ENTRY(AsyncIAdviseSink)
            DETECT_INTERFACE_ENTRY(IAdviseSink2)
            DETECT_INTERFACE_ENTRY(AsyncIAdviseSink2)
            DETECT_INTERFACE_ENTRY(IDataObject) //Ϊ�����ṩͼ�λ��ƹ��ܣ��������Գ������ṩ֧�֣�
            DETECT_INTERFACE_ENTRY(IDataAdviseHolder)
            DETECT_INTERFACE_ENTRY(IMessageFilter)
            DETECT_INTERFACE_ENTRY(IRpcChannelBuffer)
            DETECT_INTERFACE_ENTRY(IRpcChannelBuffer2)
            DETECT_INTERFACE_ENTRY(IAsyncRpcChannelBuffer)
            DETECT_INTERFACE_ENTRY(IRpcChannelBuffer3)
            DETECT_INTERFACE_ENTRY(IRpcSyntaxNegotiate)
            DETECT_INTERFACE_ENTRY(IRpcProxyBuffer)
            DETECT_INTERFACE_ENTRY(IRpcStubBuffer)
            DETECT_INTERFACE_ENTRY(IPSFactoryBuffer)
            DETECT_INTERFACE_ENTRY(IChannelHook)
            DETECT_INTERFACE_ENTRY(IClientSecurity)
            DETECT_INTERFACE_ENTRY(IServerSecurity)
            DETECT_INTERFACE_ENTRY(IClassActivator)
            DETECT_INTERFACE_ENTRY(IRpcOptions)
            DETECT_INTERFACE_ENTRY(IFillLockBytes)
            DETECT_INTERFACE_ENTRY(IProgressNotify)
            DETECT_INTERFACE_ENTRY(ILayoutStorage)
            DETECT_INTERFACE_ENTRY(IBlockingLock)
            DETECT_INTERFACE_ENTRY(ITimeAndNoticeControl)
            DETECT_INTERFACE_ENTRY(IOplockStorage)
            DETECT_INTERFACE_ENTRY(ISurrogate)
            DETECT_INTERFACE_ENTRY(IGlobalInterfaceTable)
            DETECT_INTERFACE_ENTRY(IDirectWriterLock)
            DETECT_INTERFACE_ENTRY(ISynchronize)
            DETECT_INTERFACE_ENTRY(ISynchronizeHandle)
            DETECT_INTERFACE_ENTRY(ISynchronizeEvent)
            DETECT_INTERFACE_ENTRY(ISynchronizeContainer)
            DETECT_INTERFACE_ENTRY(ISynchronizeMutex)
            DETECT_INTERFACE_ENTRY(ICancelMethodCalls)
            DETECT_INTERFACE_ENTRY(IAsyncManager)
            DETECT_INTERFACE_ENTRY(ICallFactory)
            DETECT_INTERFACE_ENTRY(IRpcHelper)
            DETECT_INTERFACE_ENTRY(IReleaseMarshalBuffers)
            DETECT_INTERFACE_ENTRY(IWaitMultiple)
            DETECT_INTERFACE_ENTRY(IUrlMon)
            DETECT_INTERFACE_ENTRY(IForegroundTransfer)
            DETECT_INTERFACE_ENTRY(IAddrTrackingControl)
            DETECT_INTERFACE_ENTRY(IAddrExclusionControl)
            DETECT_INTERFACE_ENTRY(IPipeByte)
            DETECT_INTERFACE_ENTRY(AsyncIPipeByte)
            DETECT_INTERFACE_ENTRY(IPipeLong)
            DETECT_INTERFACE_ENTRY(AsyncIPipeLong)
            DETECT_INTERFACE_ENTRY(IPipeDouble)
            DETECT_INTERFACE_ENTRY(AsyncIPipeDouble)
            DETECT_INTERFACE_ENTRY(IThumbnailExtractor)
            DETECT_INTERFACE_ENTRY(IDummyHICONIncluder)
#if _MSC_VER >= 1400  //VS2005
            //DETECT_INTERFACE_ENTRY_IID(IEnumContextProps,IID_IEnumContextProps)
            //DETECT_INTERFACE_ENTRY(IContext)
#if defined USE_COM_CONTEXT_DEF || defined BUILDTYPE_COMSVCS || defined _OLE32_
            DETECT_INTERFACE_ENTRY(IObjContext)
#endif

#else
            DETECT_INTERFACE_ENTRY(IEnumContextProps)
            DETECT_INTERFACE_ENTRY(IContext)
            DETECT_INTERFACE_ENTRY(IObjContext)
#endif
            DETECT_INTERFACE_ENTRY(IProcessLock)
            DETECT_INTERFACE_ENTRY(ISurrogateService)
            DETECT_INTERFACE_ENTRY(IComThreadingInfo)
            DETECT_INTERFACE_ENTRY(IProcessInitControl)
            //DETECT_INTERFACE_ENTRY(IInitializeSpy)

#endif //INCLUDE_DETECT_OBJIDL

#if INCLUDE_DETECT_OBJSAFE
            DETECT_INTERFACE_ENTRY(IObjectSafety) //��ȫ�԰�����ʼ����ȫ�Ժͽű���ȫ�ԣ�����ͨ��ע�����ġ����������ð�ȫ��
#endif //INCLUDE_DETECT_OBJSAFE

#if INCLUDE_DETECT_OCIDL
            //OcIdl.h
            DETECT_INTERFACE_ENTRY(IEnumConnections)
            DETECT_INTERFACE_ENTRY(IConnectionPoint)
            DETECT_INTERFACE_ENTRY(IEnumConnectionPoints)
            DETECT_INTERFACE_ENTRY_EX(IConnectionPointContainer,CFConnectionContainerPointDump) //���ӵ������ӿ�
            DETECT_INTERFACE_ENTRY(IClassFactory2) //֧������֤����
            DETECT_INTERFACE_ENTRY(IProvideClassInfo) //����ͨ��GetClassInfo��Ա�����ṩ����������Ϣ
            DETECT_INTERFACE_ENTRY(IProvideClassInfo2) //���Ի�ȡ���Ĭ�������ӿڵ�GUID(����ͻ�����ʹ�����ӵ�ʱAdvise)
            DETECT_INTERFACE_ENTRY(IProvideMultipleClassInfo)
            DETECT_INTERFACE_ENTRY(IOleControl) //�����ؼ��������ڴ�����������ʱ�ܺܺõ�Э������,
            //����ؼ�֧�ֿ�ݼ������߷��ʰ������Ļ������ԣ����߿ؼ�Ҫ������������¼�����ô����ʵ�ִ˽ӿ�
            DETECT_INTERFACE_ENTRY(IOleControlSite)
            DETECT_INTERFACE_ENTRY(IPropertyPage)
            DETECT_INTERFACE_ENTRY(IPropertyPage2) //��ÿһ������ҳ���ʵ��
            DETECT_INTERFACE_ENTRY(IPropertyPageSite)  //����ҳվ�����,�������ٽ�������ҳ�����Ա仯֪ͨ
            DETECT_INTERFACE_ENTRY(IPropertyNotifySink) //���Ա仯֪ͨ
            DETECT_INTERFACE_ENTRY(ISpecifyPropertyPages) //�ؼ�ͨ��ʵ�ָýӿ��������ṩ�����������ҳ��һ���б�
            //(GetPages �������ض�Ӧ����ҳ��CLSID����)
            DETECT_INTERFACE_ENTRY(IPersistMemory)
            DETECT_INTERFACE_ENTRY(IPersistStreamInit)
            DETECT_INTERFACE_ENTRY(IPersistPropertyBag)
            DETECT_INTERFACE_ENTRY(ISimpleFrameSite)
            DETECT_INTERFACE_ENTRY(IFont)
            DETECT_INTERFACE_ENTRY(IPicture)
            DETECT_INTERFACE_ENTRY(IFontEventsDisp)
            DETECT_INTERFACE_ENTRY(IFontDisp)
            DETECT_INTERFACE_ENTRY(IPictureDisp)
            DETECT_INTERFACE_ENTRY(IOleInPlaceObjectWindowless)     //Ϊʲô��ActiveX���޷���ã�
            DETECT_INTERFACE_ENTRY(IOleInPlaceSiteEx)
            DETECT_INTERFACE_ENTRY(IOleInPlaceSiteWindowless)
            DETECT_INTERFACE_ENTRY(IViewObjectEx)                   //Ϊʲô��ActiveX���޷���ã�
            DETECT_INTERFACE_ENTRY(IOleUndoUnit)
            DETECT_INTERFACE_ENTRY(IOleParentUndoUnit)
            DETECT_INTERFACE_ENTRY(IEnumOleUndoUnits)
            DETECT_INTERFACE_ENTRY(IOleUndoManager)
            DETECT_INTERFACE_ENTRY(IPointerInactive)

            //!֧�ֶ������������ͨ�� SetSite/GetSite
            DETECT_INTERFACE_ENTRY(IObjectWithSite)
            DETECT_INTERFACE_ENTRY(IPerPropertyBrowsing)
            DETECT_INTERFACE_ENTRY(IPropertyBag2)
            DETECT_INTERFACE_ENTRY(IPersistPropertyBag2)
            DETECT_INTERFACE_ENTRY(IAdviseSinkEx)
            DETECT_INTERFACE_ENTRY(IQuickActivate)

#endif //INCLUDE_DETECT_OCIDL

#if INCLUDE_DETECT_OLEIDL
            //OleIdl.h
            DETECT_INTERFACE_ENTRY(IOleAdviseHolder)
            DETECT_INTERFACE_ENTRY(IOleCache)
            DETECT_INTERFACE_ENTRY(IOleCache2)
            DETECT_INTERFACE_ENTRY(IOleCacheControl)
            DETECT_INTERFACE_ENTRY(IParseDisplayName)
            DETECT_INTERFACE_ENTRY(IOleContainer)
            DETECT_INTERFACE_ENTRY(IOleClientSite)
            DETECT_INTERFACE_ENTRY(IOleObject)  //�ṩ��OLE�ĵ��ܹ��Ļ������ݣ��������������Э�̱�Ƕ��������Ϣ
            //����ؼ�Ҫ������������վ��������ͨѶ����ô����ʵ�ִ˽ӿڡ�
            DETECT_INTERFACE_ENTRY(IOleWindow)
            DETECT_INTERFACE_ENTRY(IOleLink)
            DETECT_INTERFACE_ENTRY(IOleItemContainer)
            DETECT_INTERFACE_ENTRY(IOleInPlaceUIWindow)
            DETECT_INTERFACE_ENTRY(IOleInPlaceActiveObject)//ʹ�ú�ת�����ټ�
            DETECT_INTERFACE_ENTRY(IOleInPlaceFrame)
            DETECT_INTERFACE_ENTRY(IOleInPlaceObject)//֧����������ʵ����λ�����ʵЧ
            DETECT_INTERFACE_ENTRY(IOleInPlaceSite)
            DETECT_INTERFACE_ENTRY(IContinue)
            DETECT_INTERFACE_ENTRY(IViewObject)
            DETECT_INTERFACE_ENTRY(IViewObject2) //�ṩ���ӵ����,�ڷ�ʵ�ؼ����״̬��Ҳ������ʾ��Ϣ
            DETECT_INTERFACE_ENTRY(IDropSource)
            DETECT_INTERFACE_ENTRY(IDropTarget)
            DETECT_INTERFACE_ENTRY(IEnumOLEVERB)
#endif //INCLUDE_DETECT_OLEIDL

#if INCLUDE_DETECT_QEDIT
            DETECT_INTERFACE_ENTRY(IPropertySetter)
            DETECT_INTERFACE_ENTRY(IDxtCompositor)
            DETECT_INTERFACE_ENTRY(IDxtAlphaSetter)
            DETECT_INTERFACE_ENTRY(IDxtJpeg)
            DETECT_INTERFACE_ENTRY(IDxtKey)
            DETECT_INTERFACE_ENTRY(IMediaLocator)
            //! ý��̽����
            DETECT_INTERFACE_ENTRY(IMediaDet)
            DETECT_INTERFACE_ENTRY(IGrfCache)
            DETECT_INTERFACE_ENTRY(IRenderEngine)
            DETECT_INTERFACE_ENTRY(IRenderEngine2)
            DETECT_INTERFACE_ENTRY(IFindCompressorCB)
            DETECT_INTERFACE_ENTRY(ISmartRenderEngine)
            //!������Ŀ��ƽӿڣ�ÿ��DES���󶼻�ʵ��
            DETECT_INTERFACE_ENTRY(IAMTimelineObj)
            DETECT_INTERFACE_ENTRY(IAMTimelineEffectable)
            DETECT_INTERFACE_ENTRY(IAMTimelineEffect)
            DETECT_INTERFACE_ENTRY(IAMTimelineTransable)
            DETECT_INTERFACE_ENTRY(IAMTimelineSplittable)
            DETECT_INTERFACE_ENTRY(IAMTimelineTrans)
            DETECT_INTERFACE_ENTRY(IAMTimelineSrc)
            DETECT_INTERFACE_ENTRY(IAMTimelineTrack)
            DETECT_INTERFACE_ENTRY(IAMTimelineVirtualTrack)
            DETECT_INTERFACE_ENTRY(IAMTimelineComp)
            DETECT_INTERFACE_ENTRY(IAMTimelineGroup)
            //! ʹ��CreateEmptyNode ��������DES����
            DETECT_INTERFACE_ENTRY(IAMTimeline)
            DETECT_INTERFACE_ENTRY(IXml2Dex)
            DETECT_INTERFACE_ENTRY(IAMErrorLog)
            DETECT_INTERFACE_ENTRY(IAMSetErrorLog)
            DETECT_INTERFACE_ENTRY(ISampleGrabberCB)
            DETECT_INTERFACE_ENTRY(ISampleGrabber)
            DETECT_INTERFACE_ENTRY(IResize)
#endif //INCLUDE_DETECT_QEDIT

#if INCLUDE_DETECT_SERVPROV
            //!�� Windows Media 9 Series ��ǰ��
            //�����ṩ��(��VsPackage)����Ϊȫ�ַ���(global service)�ͱ��ط���(local service)
            DETECT_INTERFACE_ENTRY_EX(IServiceProvider, CFServiceProviderDump)
#endif //INCLUDE_DETECT_SERVPROV

#if INCLUDE_DETECT_SHLOBJ

            //shlobj.h
            DETECT_INTERFACE_ENTRY_IID(IACList, IID_IACList)
            DETECT_INTERFACE_ENTRY_IID(IACList2, IID_IACList2)
            DETECT_INTERFACE_ENTRY_IID(IActionProgress, IID_IActionProgress)
            DETECT_INTERFACE_ENTRY_IID(IActionProgressDialog, IID_IActionProgressDialog)
#if _WININET_
            DETECT_INTERFACE_ENTRY_IID(IActiveDesktop, IID_IActiveDesktop)
            DETECT_INTERFACE_ENTRY_IID(IActiveDesktopP, IID_IActiveDesktopP)
            DETECT_INTERFACE_ENTRY_IID(IADesktopP2, IID_IADesktopP2)
#endif  //_WININET_
#if _MSC_VER >= 1400  //VS2005

#if (_WIN32_IE >= _WIN32_IE_IE60SP2)    //SDK V6.1
            DETECT_INTERFACE_ENTRY_IID(IAttachmentExecute, IID_IAttachmentExecute)
            DETECT_INTERFACE_ENTRY_IID(INewWindowManager, IID_INewWindowManager)
            DETECT_INTERFACE_ENTRY_IID(IDelegateFolder, IID_IDelegateFolder)
            DETECT_INTERFACE_ENTRY_IID(IShellMenu, IID_IShellMenu)
            DETECT_INTERFACE_ENTRY_IID(IShellMenuCallback, IID_IShellMenuCallback)
            DETECT_INTERFACE_ENTRY_IID(IShellTaskScheduler,IID_IShellTaskScheduler)
#endif

#endif
            //DETECT_INTERFACE_ENTRY_IID(IAppPublisher, IID_IAppPublisher)
            //DETECT_INTERFACE_ENTRY_IID(IBrowserService, IID_IBrowserService)
            //DETECT_INTERFACE_ENTRY_IID(IBrowserService2, IID_IBrowserService2)
            //DETECT_INTERFACE_ENTRY_IID(IBrowserService3, IID_IBrowserService3)
            //DETECT_INTERFACE_ENTRY_IID(IEmptyVolumeCache, IID_IEmptyVolumeCache)
            //DETECT_INTERFACE_ENTRY_IID(IEmptyVolumeCache2, IID_IEmptyVolumeCache2)
            //DETECT_INTERFACE_ENTRY_IID(IEmptyVolumeCacheCallBack, IID_IEmptyVolumeCacheCallBack)
            //DETECT_INTERFACE_ENTRY_IID(IEnumPublishedApps, IID_IEnumPublishedApps)
            //DETECT_INTERFACE_ENTRY_IID(IEnumSyncItems, IID_IEnumSyncItems)
            //DETECT_INTERFACE_ENTRY_IID(IEnumSyncSchedules, IID_IEnumSyncSchedules)
            //DETECT_INTERFACE_ENTRY_IID(IEnumUserIdentity, IID_IEnumUserIdentity)
            //DETECT_INTERFACE_ENTRY_IID(IExpDispSupport, IID_IExpDispSupport)
            //DETECT_INTERFACE_ENTRY_IID(IIdentityChangeNotify, IID_IIdentityChangeNotify)
            //DETECT_INTERFACE_ENTRY_IID(INotifyReplica, IID_INotifyReplica)
            //DETECT_INTERFACE_ENTRY_IID(IPreviousVersionsInfo, IID_IPreviousVersionsInfo)
            //DETECT_INTERFACE_ENTRY_IID(IPrivateIdentityManager, IID_IPrivateIdentityManager)
            //DETECT_INTERFACE_ENTRY_IID(IPrivateIdentityManager2, IID_IPrivateIdentityManager2)
            //DETECT_INTERFACE_ENTRY_IID(IPublishedApp, IID_IPublishedApp)
            //DETECT_INTERFACE_ENTRY_IID(IReconcilableObject, IID_IReconcilableObject)
            //DETECT_INTERFACE_ENTRY_IID(IReconcileInitiator, IID_IReconcileInitiator)
            //DETECT_INTERFACE_ENTRY_IID(IShellApp, IID_IShellApp)
            //DETECT_INTERFACE_ENTRY_IID(IShellImageData, IID_IShellImageData)
            //DETECT_INTERFACE_ENTRY_IID(IShellImageDataAbort, IID_IShellImageDataAbort)
            //DETECT_INTERFACE_ENTRY_IID(IShellImageDataFactory, IID_IShellImageDataFactory)
            //DETECT_INTERFACE_ENTRY_IID(IShellService, IID_IShellService)
            //DETECT_INTERFACE_ENTRY_IID(ISyncSchedule, IID_ISyncSchedule)
            //DETECT_INTERFACE_ENTRY_IID(ISyncScheduleMgr, IID_ISyncScheduleMgr)
            //DETECT_INTERFACE_ENTRY_IID(ITravelEntry, IID_ITravelEntry)
            //DETECT_INTERFACE_ENTRY_IID(ITravelLog, IID_ITravelLog)
            //DETECT_INTERFACE_ENTRY_IID(IUserIdentity, IID_IUserIdentity)
            //DETECT_INTERFACE_ENTRY_IID(IUserIdentity2, IID_IUserIdentity2)
            //DETECT_INTERFACE_ENTRY_IID(IUserIdentityManager, IID_IUserIdentityManager)
            //DETECT_INTERFACE_ENTRY_IID(IShellTaskScheduler2,IID_IShellTaskScheduler2)
            //DETECT_INTERFACE_ENTRY_IID(IFileViewerA, IID_IFileViewerA)
            //DETECT_INTERFACE_ENTRY_IID(IFileViewerW, IID_IFileViewerW)
#if _WIN32_IE >= 0x0602
            DETECT_INTERFACE_ENTRY_IID(IQueryAssociations, IID_IQueryAssociations)
#endif  //_WIN32_IE >= 0x0602
            DETECT_INTERFACE_ENTRY_IID(IAsyncOperation, IID_IAsyncOperation)
            DETECT_INTERFACE_ENTRY_IID(IAutoComplete, IID_IAutoComplete)
            DETECT_INTERFACE_ENTRY_IID(IAutoComplete2, IID_IAutoComplete2)
            DETECT_INTERFACE_ENTRY_IID(IAutoCompleteDropDown, IID_IAutoCompleteDropDown)
            DETECT_INTERFACE_ENTRY(IBandSite)
            //DETECT_INTERFACE_ENTRY_IID(ICategorizer, IID_ICategorizer)
            // DETECT_INTERFACE_ENTRY_IID(ICategoryProvider, IID_ICategoryProvider)
            //DETECT_INTERFACE_ENTRY_IID(ICDBurn, IID_ICDBurn)
            //DETECT_INTERFACE_ENTRY(ICDBurnExt)
            //DETECT_INTERFACE_ENTRY_IID(IColumnProvider, IID_IColumnProvider)
            DETECT_INTERFACE_ENTRY_IID(ICommDlgBrowser, IID_ICommDlgBrowser)
            DETECT_INTERFACE_ENTRY_IID(ICommDlgBrowser2, IID_ICommDlgBrowser2)
            DETECT_INTERFACE_ENTRY_IID(IContextMenu, IID_IContextMenu)
            DETECT_INTERFACE_ENTRY_IID(IContextMenu2, IID_IContextMenu2)
            DETECT_INTERFACE_ENTRY_IID(IContextMenu3, IID_IContextMenu3)
            DETECT_INTERFACE_ENTRY(IContextMenuSite)
            DETECT_INTERFACE_ENTRY_IID(ICopyHookA,IID_ICopyHookA)
            DETECT_INTERFACE_ENTRY_IID(ICopyHookW,IID_ICopyHookW)
            DETECT_INTERFACE_ENTRY_IID(ICurrentWorkingDirectory, IID_ICurrentWorkingDirectory)
            DETECT_INTERFACE_ENTRY_IID(IDefViewFrame, IID_IDefViewFrame)
            DETECT_INTERFACE_ENTRY_IID(IDefViewID, IID_IDefViewID)
            //DETECT_INTERFACE_ENTRY(IDefViewSafety)
            DETECT_INTERFACE_ENTRY_IID(IDeskBand, IID_IDeskBand)
            DETECT_INTERFACE_ENTRY(IDeskBar)
            DETECT_INTERFACE_ENTRY_IID(IDockingWindow, IID_IDockingWindow)
            DETECT_INTERFACE_ENTRY_IID(IDockingWindowFrame, IID_IDockingWindowFrame)
            DETECT_INTERFACE_ENTRY_IID(IDockingWindowSite, IID_IDockingWindowSite)
            DETECT_INTERFACE_ENTRY_IID(IDocViewSite, IID_IDocViewSite)
            //DETECT_INTERFACE_ENTRY_IID(IDragSourceHelper, IID_IDragSourceHelper)
            //DETECT_INTERFACE_ENTRY_IID(IDropTargetHelper, IID_IDropTargetHelper)
            //DETECT_INTERFACE_ENTRY(IDVGetEnum)
            //DETECT_INTERFACE_ENTRY_IID(IEnumExtraSearch, IID_IEnumExtraSearch)
            DETECT_INTERFACE_ENTRY_IID(IEnumIDList, IID_IEnumIDList)
            DETECT_INTERFACE_ENTRY_IID(IExtractIconA, IID_IExtractIconA)
            DETECT_INTERFACE_ENTRY_IID(IExtractIconW, IID_IExtractIconW)
            DETECT_INTERFACE_ENTRY_IID(IExtractImage, IID_IExtractImage)
            //DETECT_INTERFACE_ENTRY_IID(IExtractImage2, IID_IExtractImage2)
            //DETECT_INTERFACE_ENTRY_IID(IFileSystemBindData, IID_IFileSystemBindData)
            //DETECT_INTERFACE_ENTRY_IID(IFileViewerSite, IID_IFileViewerSite)
            DETECT_INTERFACE_ENTRY(IFolderBandPriv)
            DETECT_INTERFACE_ENTRY_IID(IFolderFilter, IID_IFolderFilter)
            DETECT_INTERFACE_ENTRY_IID(IFolderFilterSite, IID_IFolderFilterSite)
            DETECT_INTERFACE_ENTRY_IID(IFolderView, IID_IFolderView)
            DETECT_INTERFACE_ENTRY_IID(IFolderViewHost, IID_IFolderViewHost)
            DETECT_INTERFACE_ENTRY_IID(IHWEventHandler, IID_IHWEventHandler)
            DETECT_INTERFACE_ENTRY(IImageRecompress)
            DETECT_INTERFACE_ENTRY_IID(IInputObject, IID_IInputObject)
            DETECT_INTERFACE_ENTRY_IID(IInputObjectSite, IID_IInputObjectSite)
            DETECT_INTERFACE_ENTRY(IInsertItem)
            DETECT_INTERFACE_ENTRY_IID(IItemNameLimits, IID_IItemNameLimits)
            DETECT_INTERFACE_ENTRY(IMenuBand)
            DETECT_INTERFACE_ENTRY(IMenuPopup)
            DETECT_INTERFACE_ENTRY(IModalWindow)
            DETECT_INTERFACE_ENTRY_IID(INamedPropertyBag, IID_INamedPropertyBag)
            DETECT_INTERFACE_ENTRY(INamespaceWalk)
            DETECT_INTERFACE_ENTRY(INamespaceWalkCB)
            DETECT_INTERFACE_ENTRY_IID(INewShortcutHook, IID_INewShortcutHook)
            DETECT_INTERFACE_ENTRY_IID(IObjMgr, IID_IObjMgr)
            //DETECT_INTERFACE_ENTRY_IID(IPassportClientServices, IID_IPassportClientServices)
            //DETECT_INTERFACE_ENTRY(IPassportWizard)
            DETECT_INTERFACE_ENTRY_IID(IPersistFolder, IID_IPersistFolder)
            DETECT_INTERFACE_ENTRY_IID(IPersistFolder2, IID_IPersistFolder2)
            //DETECT_INTERFACE_ENTRY_IID(IPersistFolder3, IID_IPersistFolder3)
            DETECT_INTERFACE_ENTRY_IID(IPersistIDList, IID_IPersistIDList)
            DETECT_INTERFACE_ENTRY_IID(IProfferService, IID_IProfferService)
            //DETECT_INTERFACE_ENTRY_IID(IProgressDialog, IID_IProgressDialog)
            DETECT_INTERFACE_ENTRY_IID(IPublishingWizard, IID_IPublishingWizard)
            DETECT_INTERFACE_ENTRY_IID(IQueryCancelAutoPlay, IID_IQueryCancelAutoPlay)
            DETECT_INTERFACE_ENTRY_IID(IQueryContinue, IID_IQueryContinue)
            DETECT_INTERFACE_ENTRY_IID(IQueryInfo, IID_IQueryInfo)
            DETECT_INTERFACE_ENTRY(IRegTreeItem)
            DETECT_INTERFACE_ENTRY_IID(IRemoteComputer, IID_IRemoteComputer)
            //DETECT_INTERFACE_ENTRY_IID(IResolveShellLink, IID_IResolveShellLink)
            DETECT_INTERFACE_ENTRY_IID(IRunnableTask, IID_IRunnableTask)
            DETECT_INTERFACE_ENTRY_IID(ISearchContext, IID_ISearchContext)
            DETECT_INTERFACE_ENTRY_IID(IShellBrowser, IID_IShellBrowser)
            DETECT_INTERFACE_ENTRY_IID(IShellChangeNotify, IID_IShellChangeNotify)
            DETECT_INTERFACE_ENTRY_IID(IShellDetails, IID_IShellDetails)
            //DETECT_INTERFACE_ENTRY_IID(IShellExecuteHookA, IID_IShellExecuteHookA)
            //DETECT_INTERFACE_ENTRY_IID(IShellExecuteHookW, IID_IShellExecuteHookW)
            DETECT_INTERFACE_ENTRY_IID(IShellExtInit, IID_IShellExtInit)
            DETECT_INTERFACE_ENTRY_IID(IShellFolder, IID_IShellFolder)
            //DETECT_INTERFACE_ENTRY_IID(IShellFolder2, IID_IShellFolder2)
            DETECT_INTERFACE_ENTRY_IID(IShellFolderBand, IID_IShellFolderBand)
            DETECT_INTERFACE_ENTRY_IID(IShellFolderViewCB, IID_IShellFolderViewCB)
            DETECT_INTERFACE_ENTRY_IID(IShellIcon, IID_IShellIcon)
            DETECT_INTERFACE_ENTRY_IID(IShellIconOverlay, IID_IShellIconOverlay)
            DETECT_INTERFACE_ENTRY_IID(IShellIconOverlayIdentifier, IID_IShellIconOverlayIdentifier)
            //DETECT_INTERFACE_ENTRY_IID(IShellImageStore, IID_IShellImageStore)
            DETECT_INTERFACE_ENTRY(IShellItem)
            DETECT_INTERFACE_ENTRY_IID(IShellLinkA, IID_IShellLinkA)
            DETECT_INTERFACE_ENTRY_IID(IShellLinkW, IID_IShellLinkW)
            DETECT_INTERFACE_ENTRY_IID(IShellLinkDataList, IID_IShellLinkDataList)
            DETECT_INTERFACE_ENTRY_IID(IShellPropSheetExt, IID_IShellPropSheetExt)
            DETECT_INTERFACE_ENTRY_IID(IShellView, IID_IShellView)
            DETECT_INTERFACE_ENTRY_IID(IShellView2, IID_IShellView2)
            DETECT_INTERFACE_ENTRY_IID(ITaskbarList, IID_ITaskbarList)      //��������Item����(����������������ť��������ʾ������ֻ���֣�Ϊʲô��)
            DETECT_INTERFACE_ENTRY_IID(ITaskbarList2, IID_ITaskbarList2)    //�˿�������ȫ����־ -- ��ʲô�ã�
            DETECT_INTERFACE_ENTRY_IID(IThumbnailCapture, IID_IThumbnailCapture)
            DETECT_INTERFACE_ENTRY_IID(IURLSearchHook, IID_IURLSearchHook)
            DETECT_INTERFACE_ENTRY_IID(IURLSearchHook2, IID_IURLSearchHook2)
            DETECT_INTERFACE_ENTRY_IID(IUserNotification, IID_IUserNotification)
            DETECT_INTERFACE_ENTRY_IID(IWebWizardExtension, IID_IWebWizardExtension)
            DETECT_INTERFACE_ENTRY_IID(IWizardExtension, IID_IWizardExtension)
            DETECT_INTERFACE_ENTRY_IID(IWizardSite, IID_IWizardSite)
#if _MSC_VER < 1400
            DETECT_INTERFACE_ENTRY_IID(IShellIconOverlayManager, IID_IShellIconOverlayManager)
#endif //#if _MSC_VER < 1400

#endif //INCLUDE_DETECT_SHLOBJ

#if INCLUDE_DETECT_STRMIF
            //! ����ϵͳ�豸��ö�� -- CLSID_SystemDeviceEnum
            DETECT_INTERFACE_ENTRY(ICreateDevEnum)
            DETECT_INTERFACE_ENTRY(IPin)
            DETECT_INTERFACE_ENTRY(IEnumPins)
            DETECT_INTERFACE_ENTRY(IEnumMediaTypes)

            //! ����Filter������ֱ�����ӣ�����Ĭ�ϵ�ͬ��Դ
            //! AddFilter/EnumFilters/ConnectDirect/Reconnect/Disconnect/SetDefaultSyncSource/...
            DETECT_INTERFACE_ENTRY(IFilterGraph)

            //!�� Graph �е� IBaseFilter ����ö�� -- pGraphBuilder->EnumFilters
            DETECT_INTERFACE_ENTRY(IEnumFilters)
            DETECT_INTERFACE_ENTRY(IMediaFilter)

            //!Filter������Ƴ�ʱ������� JoinFilterGraph ֪ͨ
            DETECT_INTERFACE_ENTRY(IBaseFilter)
            DETECT_INTERFACE_ENTRY(IReferenceClock)
            DETECT_INTERFACE_ENTRY(IReferenceClock2)
            DETECT_INTERFACE_ENTRY(IMediaSample)
            DETECT_INTERFACE_ENTRY(IMediaSample2)
            DETECT_INTERFACE_ENTRY(IMemAllocator)
            DETECT_INTERFACE_ENTRY(IMemAllocatorCallbackTemp)
            DETECT_INTERFACE_ENTRY(IMemAllocatorNotifyCallbackTemp)

            //!������Pin��ʵ�֣����������������Ҵ��� IMediaSample
            DETECT_INTERFACE_ENTRY(IMemInputPin)
            DETECT_INTERFACE_ENTRY(IAMovieSetup)

            //!��ȡ������ ��ʽ����λ��Ϣ�� UnKnown �ӿڡ�
            //!  Capabilities/PreferredFormat/TimeFormat/Duration/CurrentPosition/Positions/Rate/Preroll/...
            //!  ֧�����¼��ָ�ʽ�Ķ�λ����
            //!    1.TIME_FORMAT_MEDIA_TIME(Ĭ��)   -- ���ο�ʱ��(��100nsΪ��λ)
            //!    2.TIME_FORMAT_SAMPLE             -- ��Sample����
            //!    3.TIME_FORMAT_FIELD              -- ������
            //!    4.TIME_FORMAT_BYTE               -- ���ֽ����е�ƫ��
            //!    5.TIME_FORMAT_FRAME              -- ����Ƶ֡
            DETECT_INTERFACE_ENTRY_EX(IMediaSeeking,CFMediaSeekingDump)
            DETECT_INTERFACE_ENTRY(IEnumRegFilters)
            DETECT_INTERFACE_ENTRY(IFilterMapper)
            DETECT_INTERFACE_ENTRY(IFilterMapper2)
            DETECT_INTERFACE_ENTRY(IFilterMapper3)
            DETECT_INTERFACE_ENTRY(IQualityControl)
            DETECT_INTERFACE_ENTRY(IOverlayNotify)
            DETECT_INTERFACE_ENTRY(IOverlayNotify2)
            DETECT_INTERFACE_ENTRY(IOverlay)
            DETECT_INTERFACE_ENTRY(IMediaEventSink)

            //!����֧�������ļ�(OpenFileDialog),Load/GetCurFile
            DETECT_INTERFACE_ENTRY(IFileSourceFilter)

            //! ����֧������ļ�(SaveFileDialog),SetFileName/GetCurFile
            DETECT_INTERFACE_ENTRY(IFileSinkFilter)

            //! ��IFileSinkFilter�Ļ����������� SetMode(AM_FILE_OVERWRITE),GetMode
            DETECT_INTERFACE_ENTRY(IFileSinkFilter2)

            //!��IFilterGraph�̳У����ɽ����������ӣ�������־�ļ��ȡ�Connect/Render/RenderFile/SetLogFile/...
            DETECT_INTERFACE_ENTRY(IGraphBuilder)
            DETECT_INTERFACE_ENTRY(ICaptureGraphBuilder)
            DETECT_INTERFACE_ENTRY(IAMCopyCaptureFileProgress)
            //! ������������Ƶ�ɼ���FilterGraph
            DETECT_INTERFACE_ENTRY(ICaptureGraphBuilder2)
            DETECT_INTERFACE_ENTRY(IFilterGraph2)
            DETECT_INTERFACE_ENTRY(IStreamBuilder)

            //!��Source Filter�����Pin��ʵ����ģʽ�Ľӿ�
            DETECT_INTERFACE_ENTRY(IAsyncReader)
            DETECT_INTERFACE_ENTRY(IGraphVersion)
            DETECT_INTERFACE_ENTRY(IResourceConsumer)
            DETECT_INTERFACE_ENTRY(IResourceManager)
            DETECT_INTERFACE_ENTRY(IDistributorNotify)
            DETECT_INTERFACE_ENTRY(IAMStreamControl)
            DETECT_INTERFACE_ENTRY(ISeekingPassThru)
            //! �ɼ�ʱ����������ݵ�ý������(�����ʽʹ�� WAVEFORMATEX ������)
            DETECT_INTERFACE_ENTRY(IAMStreamConfig)
            DETECT_INTERFACE_ENTRY(IConfigInterleaving)
            DETECT_INTERFACE_ENTRY(IConfigAviMux)
            DETECT_INTERFACE_ENTRY(IAMVideoCompression)
            //! �жϲ���ʾVFW���ϵĶԻ���
            DETECT_INTERFACE_ENTRY(IAMVfwCaptureDialogs)
            DETECT_INTERFACE_ENTRY(IAMVfwCompressDialogs)
            DETECT_INTERFACE_ENTRY(IAMDroppedFrames)
            DETECT_INTERFACE_ENTRY(IAMAudioInputMixer)
            //! ���û����С
            DETECT_INTERFACE_ENTRY(IAMBufferNegotiation)
            DETECT_INTERFACE_ENTRY(IAMAnalogVideoDecoder)
            DETECT_INTERFACE_ENTRY(IAMVideoProcAmp)
            DETECT_INTERFACE_ENTRY(IAMCameraControl)
            DETECT_INTERFACE_ENTRY(IAMVideoControl)
            DETECT_INTERFACE_ENTRY(IAMCrossbar)
            DETECT_INTERFACE_ENTRY(IAMTuner)
            DETECT_INTERFACE_ENTRY(IAMTunerNotification)
            DETECT_INTERFACE_ENTRY(IAMTVTuner)
            DETECT_INTERFACE_ENTRY(IBPCSatelliteTuner)
            DETECT_INTERFACE_ENTRY(IAMTVAudio)
            DETECT_INTERFACE_ENTRY(IAMTVAudioNotification)
            DETECT_INTERFACE_ENTRY(IAMAnalogVideoEncoder)
            DETECT_INTERFACE_ENTRY(IKsPropertySet)
            DETECT_INTERFACE_ENTRY(IMediaPropertyBag)
            DETECT_INTERFACE_ENTRY(IPersistMediaPropertyBag)
            DETECT_INTERFACE_ENTRY(IAMPhysicalPinInfo)
            DETECT_INTERFACE_ENTRY(IAMExtDevice)
            DETECT_INTERFACE_ENTRY(IAMExtTransport)
            DETECT_INTERFACE_ENTRY(IAMTimecodeReader)
            DETECT_INTERFACE_ENTRY(IAMTimecodeGenerator)
            DETECT_INTERFACE_ENTRY(IAMTimecodeDisplay)
            DETECT_INTERFACE_ENTRY(IAMDevMemoryAllocator)
            DETECT_INTERFACE_ENTRY(IAMDevMemoryControl)
            DETECT_INTERFACE_ENTRY(IAMStreamSelect)
            DETECT_INTERFACE_ENTRY(IAMResourceControl)
            DETECT_INTERFACE_ENTRY(IAMClockAdjust)
            DETECT_INTERFACE_ENTRY(IAMFilterMiscFlags)
            DETECT_INTERFACE_ENTRY(IDrawVideoImage)
            DETECT_INTERFACE_ENTRY(IDecimateVideoImage)
            DETECT_INTERFACE_ENTRY(IAMVideoDecimationProperties)

            //!  ������Ƶ�ĵ�������, Step/CanStep/...
            DETECT_INTERFACE_ENTRY(IVideoFrameStep)
            DETECT_INTERFACE_ENTRY(IAMLatency)
            DETECT_INTERFACE_ENTRY(IAMPushSource)
            DETECT_INTERFACE_ENTRY(IAMDeviceRemoval)
            DETECT_INTERFACE_ENTRY(IDVEnc)
            DETECT_INTERFACE_ENTRY(IIPDVDec)
            DETECT_INTERFACE_ENTRY(IDVRGB219)
            DETECT_INTERFACE_ENTRY(IDVSplitter)
            DETECT_INTERFACE_ENTRY(IAMAudioRendererStats)
            DETECT_INTERFACE_ENTRY(IAMGraphStreams)
            DETECT_INTERFACE_ENTRY(IAMOverlayFX)
            DETECT_INTERFACE_ENTRY(IAMOpenProgress)
            DETECT_INTERFACE_ENTRY(IMpeg2Demultiplexer)
            DETECT_INTERFACE_ENTRY(IEnumStreamIdMap)
            DETECT_INTERFACE_ENTRY(IMPEG2StreamIdMap)
            DETECT_INTERFACE_ENTRY(IRegisterServiceProvider)
            DETECT_INTERFACE_ENTRY(IAMDecoderCaps)
            DETECT_INTERFACE_ENTRY(IAMClockSlave)
            DETECT_INTERFACE_ENTRY(IDvdControl)
            DETECT_INTERFACE_ENTRY(IDvdInfo)
            DETECT_INTERFACE_ENTRY(IDvdCmd)
            DETECT_INTERFACE_ENTRY(IDvdState)
            DETECT_INTERFACE_ENTRY(IDvdControl2)
            DETECT_INTERFACE_ENTRY(IDvdInfo2)
            DETECT_INTERFACE_ENTRY(IDvdGraphBuilder)
            DETECT_INTERFACE_ENTRY(IDDrawExclModeVideo)
            DETECT_INTERFACE_ENTRY(IDDrawExclModeVideoCallback)
            DETECT_INTERFACE_ENTRY(IPinConnection)
            DETECT_INTERFACE_ENTRY(IPinFlowControl)

            //! ���Զ�Filter Graph ���й�������Reconnect/AddFilterToCache/RemoveFilterEx/...
            DETECT_INTERFACE_ENTRY(IGraphConfig)
            DETECT_INTERFACE_ENTRY(IGraphConfigCallback)
            DETECT_INTERFACE_ENTRY(IFilterChain)
            DETECT_INTERFACE_ENTRY(IVMRImagePresenter)
            DETECT_INTERFACE_ENTRY(IVMRSurfaceAllocator)
            DETECT_INTERFACE_ENTRY(IVMRSurfaceAllocatorNotify)
            DETECT_INTERFACE_ENTRY(IVMRWindowlessControl)
            DETECT_INTERFACE_ENTRY(IVMRMixerControl)
            DETECT_INTERFACE_ENTRY(IVMRMonitorConfig)
            DETECT_INTERFACE_ENTRY(IVMRFilterConfig)
            DETECT_INTERFACE_ENTRY(IVMRMixerBitmap)
            DETECT_INTERFACE_ENTRY(IVMRImageCompositor)
            DETECT_INTERFACE_ENTRY(IVMRVideoStreamControl)
            DETECT_INTERFACE_ENTRY(IVMRSurface)
            DETECT_INTERFACE_ENTRY(IVMRImagePresenterConfig)
            DETECT_INTERFACE_ENTRY(IVMRImagePresenterExclModeConfig)
            DETECT_INTERFACE_ENTRY(IVPManager)
#endif //INCLUDE_DETECT_STRMIF

#if INCLUDE_DETECT_URLMON
            DETECT_INTERFACE_ENTRY(IPersistMoniker)
            DETECT_INTERFACE_ENTRY(IMonikerProp)
            DETECT_INTERFACE_ENTRY(IBindProtocol)
            DETECT_INTERFACE_ENTRY(IBinding)
            DETECT_INTERFACE_ENTRY(IBindStatusCallback)
            DETECT_INTERFACE_ENTRY(IAuthenticate)
            DETECT_INTERFACE_ENTRY(IHttpNegotiate)
            DETECT_INTERFACE_ENTRY(IHttpNegotiate2)
            DETECT_INTERFACE_ENTRY(IWindowForBindingUI)
            DETECT_INTERFACE_ENTRY(ICodeInstall)
            DETECT_INTERFACE_ENTRY(IWinInetInfo)
            DETECT_INTERFACE_ENTRY(IHttpSecurity)
            DETECT_INTERFACE_ENTRY(IWinInetHttpInfo)
            DETECT_INTERFACE_ENTRY(IBindHost)
            DETECT_INTERFACE_ENTRY(IInternet)
            DETECT_INTERFACE_ENTRY(IInternetBindInfo)
            DETECT_INTERFACE_ENTRY(IInternetProtocolRoot)
            DETECT_INTERFACE_ENTRY(IInternetProtocol)
            DETECT_INTERFACE_ENTRY(IInternetProtocolSink)
            DETECT_INTERFACE_ENTRY(IInternetProtocolSinkStackable)
            DETECT_INTERFACE_ENTRY(IInternetSession)
            DETECT_INTERFACE_ENTRY(IInternetThreadSwitch)
            DETECT_INTERFACE_ENTRY(IInternetPriority)
            DETECT_INTERFACE_ENTRY(IInternetProtocolInfo)
            DETECT_INTERFACE_ENTRY(IInternetSecurityMgrSite)
            DETECT_INTERFACE_ENTRY(IInternetSecurityManager)
            DETECT_INTERFACE_ENTRY(IInternetHostSecurityManager)
            DETECT_INTERFACE_ENTRY(IInternetZoneManager)
            DETECT_INTERFACE_ENTRY(ISoftDistExt)
            DETECT_INTERFACE_ENTRY(ICatalogFileInfo)
            DETECT_INTERFACE_ENTRY(IDataFilter)
            DETECT_INTERFACE_ENTRY(IEncodingFilterFactory)
            DETECT_INTERFACE_ENTRY(IWrappedProtocol)
#endif //INCLUDE_DETECT_URLMON

#if INCLUDE_DETECT_VSIP
            //{
                //containedlanguage.h
                DETECT_INTERFACE_ENTRY(IVsIntellisenseProjectManager)
                DETECT_INTERFACE_ENTRY(SVsIntellisenseProjectManager)
                DETECT_INTERFACE_ENTRY(IVsIntellisenseEngine)
                DETECT_INTERFACE_ENTRY(SVsIntellisenseEngine)
                DETECT_INTERFACE_ENTRY(IVsIntellisenseProjectEventSink)
                DETECT_INTERFACE_ENTRY(IVsItemTypeResolutionService)
                DETECT_INTERFACE_ENTRY(IVsItemTypeResolutionSite)
                DETECT_INTERFACE_ENTRY(IVsIntellisenseProjectHost)
                DETECT_INTERFACE_ENTRY(SVsIntellisenseProjectHost)
                DETECT_INTERFACE_ENTRY(IVsIntellisenseProject)
                DETECT_INTERFACE_ENTRY(IVsDataEnvironment)

                //textmgr.h
                DETECT_INTERFACE_ENTRY(IVsCompoundAction)
                DETECT_INTERFACE_ENTRY(IVsCompoundViewChange)
                DETECT_INTERFACE_ENTRY(IVsCompoundActionEvents)
                DETECT_INTERFACE_ENTRY(IVsAction)
                DETECT_INTERFACE_ENTRY(IVsTextEditorPropertyCategoryContainer)
                DETECT_INTERFACE_ENTRY(IVsTextEditorPropertyContainer)
                DETECT_INTERFACE_ENTRY(IVsCompletionSet)
                DETECT_INTERFACE_ENTRY(IVsTipWindow)
                DETECT_INTERFACE_ENTRY(IVsMethodTipWindow)
                DETECT_INTERFACE_ENTRY(IVsTextTipWindow)
                DETECT_INTERFACE_ENTRY(IVsMethodData)
                DETECT_INTERFACE_ENTRY(IVsTextTipData)
                DETECT_INTERFACE_ENTRY(IVsTextView)
                DETECT_INTERFACE_ENTRY(IVsThreadSafeTextView)
                DETECT_INTERFACE_ENTRY(IVsLayeredTextView)
                DETECT_INTERFACE_ENTRY(IVsTextViewFilter)
                DETECT_INTERFACE_ENTRY(IVsLanguageContextProvider)
                DETECT_INTERFACE_ENTRY(IVsTextMarkerContextProvider)
                DETECT_INTERFACE_ENTRY(IVsViewRangeClient)
                DETECT_INTERFACE_ENTRY(IVsTextViewEvents)
                DETECT_INTERFACE_ENTRY(IVsTextManager) // ���ʱ༭��,����Ϊ SID_SVsTextManager
                DETECT_INTERFACE_ENTRY(IVsShortcutManager)
                DETECT_INTERFACE_ENTRY(IVsEnumTextBuffers)
                DETECT_INTERFACE_ENTRY(IVsTextManagerEvents)
                DETECT_INTERFACE_ENTRY(IVsTextSelectionAction)
                DETECT_INTERFACE_ENTRY(IVsEnumTextViews)
                DETECT_INTERFACE_ENTRY(IVsEnumIndependentViews)
                DETECT_INTERFACE_ENTRY(IVsColorableItem)
                DETECT_INTERFACE_ENTRY(IVsMergeableUIItem)
                DETECT_INTERFACE_ENTRY(IVsProvideColorableItems)
                DETECT_INTERFACE_ENTRY(IVsColorizer)
                DETECT_INTERFACE_ENTRY(IVsEnumGUID)
                DETECT_INTERFACE_ENTRY(IVsLanguageInfo)
                DETECT_INTERFACE_ENTRY(IVsLanguageBlock)
                DETECT_INTERFACE_ENTRY(IVsEnumBSTR)
                DETECT_INTERFACE_ENTRY(IVsDebugName)
                DETECT_INTERFACE_ENTRY(IVsEnumDebugName)
                DETECT_INTERFACE_ENTRY(IVsLanguageDebugInfo)
                DETECT_INTERFACE_ENTRY(IVsTextBufferTempInit)
                DETECT_INTERFACE_ENTRY(IVsTextBuffer)
                DETECT_INTERFACE_ENTRY(IVsTextLines)
                DETECT_INTERFACE_ENTRY(IVsTextColorState)
                DETECT_INTERFACE_ENTRY(IVsTextLinesEvents)
                DETECT_INTERFACE_ENTRY(IVsTextStream)
                DETECT_INTERFACE_ENTRY(IVsEnumStreamMarkers)
                DETECT_INTERFACE_ENTRY(IVsEnumLineMarkers)
                DETECT_INTERFACE_ENTRY(IVsEnumLayerMarkers)
                DETECT_INTERFACE_ENTRY(IVsTextMarker)
                DETECT_INTERFACE_ENTRY(IVsTextStreamMarker)
                DETECT_INTERFACE_ENTRY(IVsTextLineMarker)
                DETECT_INTERFACE_ENTRY(IVsTextLayerMarker)
                DETECT_INTERFACE_ENTRY(IVsTextMarkerType)
                DETECT_INTERFACE_ENTRY(IVsTextMarkerColorSet)
                DETECT_INTERFACE_ENTRY(IVsPackageDefinedTextMarkerType)
                DETECT_INTERFACE_ENTRY(IVsTextMarkerTypeProvider)
                DETECT_INTERFACE_ENTRY(IVsTextMarkerClient)
                DETECT_INTERFACE_ENTRY(IVsMouseCursorProvider)
                DETECT_INTERFACE_ENTRY(IVsTextMarkerGlyphDropHandler)
                DETECT_INTERFACE_ENTRY(IVsTextTrackingPoint)
                DETECT_INTERFACE_ENTRY(IVsFullTextScanner)
                DETECT_INTERFACE_ENTRY(IVsTextScanner)
                DETECT_INTERFACE_ENTRY(IVsTextFind)
                DETECT_INTERFACE_ENTRY(IVsTextBufferEvents)
                DETECT_INTERFACE_ENTRY(IVsTextBufferDataEvents)
                DETECT_INTERFACE_ENTRY(IVsTextStreamEvents)
                DETECT_INTERFACE_ENTRY(IVsSplitPane)
                DETECT_INTERFACE_ENTRY(IVsSplitter)
                DETECT_INTERFACE_ENTRY(IVsSplitRoot)
                DETECT_INTERFACE_ENTRY(IVsCodeWindow)
                DETECT_INTERFACE_ENTRY(IVsCodeWindowManager)
                DETECT_INTERFACE_ENTRY(IVsDropdownBarClient)
                DETECT_INTERFACE_ENTRY(IVsDropdownBar)
                DETECT_INTERFACE_ENTRY(IVsDropdownBarManager)
                DETECT_INTERFACE_ENTRY(IVsButtonBarClient)
                DETECT_INTERFACE_ENTRY(IVsButtonBar)
                DETECT_INTERFACE_ENTRY(IVsButtonBarManager)
                DETECT_INTERFACE_ENTRY(IVsDefaultButtonBarImages)
                DETECT_INTERFACE_ENTRY(IVsTextMacroHelper)
                DETECT_INTERFACE_ENTRY(IVsUserData)
                DETECT_INTERFACE_ENTRY(IVsUserDataEvents)
                DETECT_INTERFACE_ENTRY(IVsUndoUnit)
                DETECT_INTERFACE_ENTRY(IVsUndoTrackingEvents)
                DETECT_INTERFACE_ENTRY(IVsChangeTrackingUndoManager)
                DETECT_INTERFACE_ENTRY(IVsLinkedUndoClient)
                DETECT_INTERFACE_ENTRY(IVsLinkCapableUndoManager)
                DETECT_INTERFACE_ENTRY(IVsLifetimeControlledObject)
                DETECT_INTERFACE_ENTRY(IVsLinkedUndoTransactionManager)
                DETECT_INTERFACE_ENTRY(IVsChangeClusterEvents)
                DETECT_INTERFACE_ENTRY(IVsDynamicTabProvider)
                DETECT_INTERFACE_ENTRY(IVsLastChangeTimeProvider)
                DETECT_INTERFACE_ENTRY(IVsEnumTextSpans)
                DETECT_INTERFACE_ENTRY(IVsTextLayer)
                DETECT_INTERFACE_ENTRY(IVsSyntheticRegion)
                DETECT_INTERFACE_ENTRY(IVsEnumSyntheticRegions)
                DETECT_INTERFACE_ENTRY(IVsSyntheticTextClient)
                DETECT_INTERFACE_ENTRY(IVsSyntheticTextSession)
                DETECT_INTERFACE_ENTRY(IVsSyntheticTextManager)
                DETECT_INTERFACE_ENTRY(IVsTextHidingLayerModule)
                DETECT_INTERFACE_ENTRY(IVsHiddenRegion)
                DETECT_INTERFACE_ENTRY(IVsEnumHiddenRegions)
                DETECT_INTERFACE_ENTRY(IVsHiddenTextClient)
                DETECT_INTERFACE_ENTRY(IVsHiddenTextManager)
                DETECT_INTERFACE_ENTRY(IVsHiddenTextSession)
                DETECT_INTERFACE_ENTRY(IVsOutliningSession)
                DETECT_INTERFACE_ENTRY(IVsOutliningCapableLanguage)
                DETECT_INTERFACE_ENTRY(IVsTextLayerEvents)
                DETECT_INTERFACE_ENTRY(IVsTextStorage)
                DETECT_INTERFACE_ENTRY(IVsPersistentTextImage)
                DETECT_INTERFACE_ENTRY(IVsTextStorageColorState)
                DETECT_INTERFACE_ENTRY(IVsFinalTextChangeCommitEvents)
                DETECT_INTERFACE_ENTRY(IVsPreliminaryTextChangeCommitEvents)
                DETECT_INTERFACE_ENTRY(IVsUndoRedoClusterWithCommitEvents)
                DETECT_INTERFACE_ENTRY(IVsTextImageEvents)
                DETECT_INTERFACE_ENTRY(IVsCommitGestureSink)
                DETECT_INTERFACE_ENTRY(IVsTextImage)
                DETECT_INTERFACE_ENTRY(IVsEditorGoBackLocations)
                DETECT_INTERFACE_ENTRY(IVsTextReplaceEvents)
                DETECT_INTERFACE_ENTRY(IVsFormatFilterProvider)
                DETECT_INTERFACE_ENTRY(IVsNavigableLocationResolver)
                DETECT_INTERFACE_ENTRY(IVsLanguageTextOps)
                DETECT_INTERFACE_ENTRY(IVsLanguageClipboardOps)
                DETECT_INTERFACE_ENTRY(IVsQueryLineChangeCommit)
                DETECT_INTERFACE_ENTRY(IVsCodeWindowEvents)
                DETECT_INTERFACE_ENTRY(IVsFileBackup)
                DETECT_INTERFACE_ENTRY(IVsLineAttributes)


                //vsshell.h
                DETECT_INTERFACE_ENTRY(IVsPackage)
                DETECT_INTERFACE_ENTRY(IVsToolWindowFactory)
                DETECT_INTERFACE_ENTRY(IVsPersistSolutionOpts)
                DETECT_INTERFACE_ENTRY(IVsPersistSolutionProps)
                DETECT_INTERFACE_ENTRY(IVsSolutionPersistence)
                DETECT_INTERFACE_ENTRY(IVsProjectFactory)
                DETECT_INTERFACE_ENTRY(IVsNonSolutionProjectFactory)
                DETECT_INTERFACE_ENTRY(IVsRegisterProjectTypes)
                DETECT_INTERFACE_ENTRY(IVsOwnedProjectFactory)
                DETECT_INTERFACE_ENTRY(IVsHierarchy)
                DETECT_INTERFACE_ENTRY(IVsUIHierarchy)
                DETECT_INTERFACE_ENTRY(IVsHierarchyEvents)
                DETECT_INTERFACE_ENTRY(IVsParentHierarchy)
                DETECT_INTERFACE_ENTRY(IVsSolution)
                DETECT_INTERFACE_ENTRY(IVsSolution2)
                DETECT_INTERFACE_ENTRY(IVsSolutionEvents)
                DETECT_INTERFACE_ENTRY(IVsSolutionEvents2)
                DETECT_INTERFACE_ENTRY(IVsSolutionEvents3)
                DETECT_INTERFACE_ENTRY(IVsFireSolutionEvents)
                DETECT_INTERFACE_ENTRY(IVsProject)
                DETECT_INTERFACE_ENTRY(IVsProject2)
                DETECT_INTERFACE_ENTRY(IVsProject3)
                DETECT_INTERFACE_ENTRY(IVsParentProject)
                DETECT_INTERFACE_ENTRY(IVsNonLocalProject)
                DETECT_INTERFACE_ENTRY(IVsProjectSpecificEditorMap)
                DETECT_INTERFACE_ENTRY(IVsProjectSpecificEditorMap2)
                DETECT_INTERFACE_ENTRY(IVsProjectResources)
                DETECT_INTERFACE_ENTRY(IVsSupportItemHandoff)
                DETECT_INTERFACE_ENTRY(IVsAddProjectItemDlg)
                DETECT_INTERFACE_ENTRY(IVsAddProjectItemDlg2)
                DETECT_INTERFACE_ENTRY(IVsFilterAddProjectItemDlg)
                DETECT_INTERFACE_ENTRY(IVsFilterAddProjectItemDlg2)
                DETECT_INTERFACE_ENTRY(IVsProjectTextImageProvider)
                DETECT_INTERFACE_ENTRY(IVsSaveOptionsDlg)
                DETECT_INTERFACE_ENTRY(IEnumRunningDocuments)
                DETECT_INTERFACE_ENTRY(IVsRunningDocumentTable)
                DETECT_INTERFACE_ENTRY(IVsRunningDocTableEvents)
                DETECT_INTERFACE_ENTRY(IVsRunningDocTableEvents2)
                DETECT_INTERFACE_ENTRY(IVsRunningDocTableEvents3)

                //A file can only be opened by one project -- RDT ?
                DETECT_INTERFACE_ENTRY(IVsDocumentLockHolder) 
                DETECT_INTERFACE_ENTRY(IVsSimpleDocFactory)
                DETECT_INTERFACE_ENTRY(IVsInvisibleEditor)
                DETECT_INTERFACE_ENTRY(IVsInvisibleEditorManager)
                DETECT_INTERFACE_ENTRY(IVsXMLMemberIndex)
                DETECT_INTERFACE_ENTRY(IVsXMLMemberIndexService)
                DETECT_INTERFACE_ENTRY(IVsXMLMemberData)
                DETECT_INTERFACE_ENTRY(IVsWindowFrame)  //���ṩDock��Hide �ȹ��ܣ����������Ĵ���
                DETECT_INTERFACE_ENTRY(IVsWindowFrameNotify)
                DETECT_INTERFACE_ENTRY(IVsWindowFrameNotify2)
                DETECT_INTERFACE_ENTRY(IVsBackForwardNavigation)
                DETECT_INTERFACE_ENTRY(IVsWindowView)
                DETECT_INTERFACE_ENTRY(IVsToolWindowToolbarHost)
                DETECT_INTERFACE_ENTRY(IVsToolWindowToolbar)
                DETECT_INTERFACE_ENTRY(IVsUIShell)  //�����ڵ���Message��?  C#��GetServiceʱ��SVsUIShell,
                DETECT_INTERFACE_ENTRY(IVsUIShellDocumentWindowMgr)
                DETECT_INTERFACE_ENTRY(IVsPackageDynamicToolOwner)
                DETECT_INTERFACE_ENTRY(IVsExternalFilesManager)
                DETECT_INTERFACE_ENTRY(IVsExternalFilesManager2)
                DETECT_INTERFACE_ENTRY(IVsFileChangeEvents)
                DETECT_INTERFACE_ENTRY(IVsFileChangeEx)
                DETECT_INTERFACE_ENTRY(IVsFileChange)
                DETECT_INTERFACE_ENTRY(IVsIME)
                DETECT_INTERFACE_ENTRY(IVsRelativePathResolver)
                DETECT_INTERFACE_ENTRY(IVsUIShellOpenDocument)
                DETECT_INTERFACE_ENTRY(IVsMultiViewDocumentView)
                DETECT_INTERFACE_ENTRY(IVsPersistDocData)
                DETECT_INTERFACE_ENTRY(IVsPersistDocData2)
                DETECT_INTERFACE_ENTRY(IVsDocDataFileChangeControl)
                DETECT_INTERFACE_ENTRY(IVsPersistHierarchyItem)
                DETECT_INTERFACE_ENTRY(IVsPersistHierarchyItem2)
                DETECT_INTERFACE_ENTRY(IVsUIHierarchyWindow)

                //������VS��Ĵ�����ʽ���û�����ӿ�(�� Solution Explorer��Error List ��)��
                //C#��ʵ�ֳ������� ToolWindowPane
                DETECT_INTERFACE_ENTRY(IVsWindowPane)
                DETECT_INTERFACE_ENTRY(IEnumPackages)
                DETECT_INTERFACE_ENTRY(IEnumHierarchies)
                DETECT_INTERFACE_ENTRY(IEnumWindowFrames)
                DETECT_INTERFACE_ENTRY(IVsShell)
                DETECT_INTERFACE_ENTRY(IVsBroadcastMessageEvents)
                DETECT_INTERFACE_ENTRY(IVsShellPropertyEvents)

                //CBscEditorFactory �� EditorFactory ? CBscEditorDocument

                DETECT_INTERFACE_ENTRY(IVsEditorFactory)
                DETECT_INTERFACE_ENTRY(IVsRegisterEditors)
                DETECT_INTERFACE_ENTRY(IVsEditorFactoryNotify)
                DETECT_INTERFACE_ENTRY(IVsMultiItemSelect)
                DETECT_INTERFACE_ENTRY(IEnumHierarchyItems)
                DETECT_INTERFACE_ENTRY(IVsEnumHierarchyItemsFactory)
                DETECT_INTERFACE_ENTRY(IVsSwatchClient)
                DETECT_INTERFACE_ENTRY(IVsTrackSelectionEx)
                DETECT_INTERFACE_ENTRY(IVsSelectionEvents)
                DETECT_INTERFACE_ENTRY(IVsMonitorSelection)
                DETECT_INTERFACE_ENTRY(IVsTaskList)
                DETECT_INTERFACE_ENTRY(IVsTaskProvider)
                DETECT_INTERFACE_ENTRY(IVsTaskProvider2)
                DETECT_INTERFACE_ENTRY(IVsTaskItem)
                DETECT_INTERFACE_ENTRY(IVsTaskItem2)
                DETECT_INTERFACE_ENTRY(IVsEnumTaskItems)
                DETECT_INTERFACE_ENTRY(IVsCommentTaskToken)
                DETECT_INTERFACE_ENTRY(IVsEnumCommentTaskTokens)
                DETECT_INTERFACE_ENTRY(IVsCommentTaskInfo)
                DETECT_INTERFACE_ENTRY(IVsTaskListEvents)
                DETECT_INTERFACE_ENTRY(IVsOutputWindowPane)
                DETECT_INTERFACE_ENTRY(IVsOutputWindow)
                DETECT_INTERFACE_ENTRY(IVsAsyncEnum)
                DETECT_INTERFACE_ENTRY(IVsAsyncEnumCallback)
                DETECT_INTERFACE_ENTRY(IVsHierarchyDropDataSource)
                DETECT_INTERFACE_ENTRY(IVsHierarchyDropDataSource2)
                DETECT_INTERFACE_ENTRY(IVsHierarchyDropDataTarget)
            //}

#endif //INCLUDE_DETECT_VSIP

#if INCLUDE_DETECT_WMSDKIDL
            DETECT_INTERFACE_ENTRY(IWMMediaProps)
            DETECT_INTERFACE_ENTRY(IWMVideoMediaProps)
            DETECT_INTERFACE_ENTRY(IWMWriter)
            DETECT_INTERFACE_ENTRY(IWMInputMediaProps)
            DETECT_INTERFACE_ENTRY(IWMReader)
            DETECT_INTERFACE_ENTRY(IWMOutputMediaProps)
            DETECT_INTERFACE_ENTRY(IWMStatusCallback)
            DETECT_INTERFACE_ENTRY(IWMReaderCallback)
            DETECT_INTERFACE_ENTRY(IWMCredentialCallback)
            DETECT_INTERFACE_ENTRY(IWMMetadataEditor)
            DETECT_INTERFACE_ENTRY(IWMHeaderInfo)
            DETECT_INTERFACE_ENTRY(IWMHeaderInfo2)
            DETECT_INTERFACE_ENTRY(IWMProfileManager)
            DETECT_INTERFACE_ENTRY(IWMProfileManager2)
            DETECT_INTERFACE_ENTRY(IWMProfile)
            DETECT_INTERFACE_ENTRY(IWMProfile2)
            DETECT_INTERFACE_ENTRY(IWMStreamConfig)
            DETECT_INTERFACE_ENTRY(IWMPacketSize)
            DETECT_INTERFACE_ENTRY(IWMStreamList)
            DETECT_INTERFACE_ENTRY(IWMMutualExclusion)
            DETECT_INTERFACE_ENTRY(IWMWriterAdvanced)
            DETECT_INTERFACE_ENTRY(IWMWriterAdvanced2)
            DETECT_INTERFACE_ENTRY(IWMWriterPostViewCallback)
            DETECT_INTERFACE_ENTRY(IWMWriterPostView)
            DETECT_INTERFACE_ENTRY(IWMWriterSink)
            DETECT_INTERFACE_ENTRY(IWMRegisterCallback)
            DETECT_INTERFACE_ENTRY(IWMWriterFileSink)
            DETECT_INTERFACE_ENTRY(IWMWriterFileSink2)
            DETECT_INTERFACE_ENTRY(IWMWriterNetworkSink)
            DETECT_INTERFACE_ENTRY(IWMClientConnections)
            DETECT_INTERFACE_ENTRY(IWMReaderAdvanced)
            DETECT_INTERFACE_ENTRY(IWMReaderAdvanced2)
            DETECT_INTERFACE_ENTRY(IWMReaderAllocatorEx)
            DETECT_INTERFACE_ENTRY(IWMReaderTypeNegotiation)
            DETECT_INTERFACE_ENTRY(IWMReaderCallbackAdvanced)
            DETECT_INTERFACE_ENTRY(IWMDRMReader)
            DETECT_INTERFACE_ENTRY(IWMReaderNetworkConfig)
            DETECT_INTERFACE_ENTRY(IWMReaderStreamClock)
            DETECT_INTERFACE_ENTRY(IWMIndexer)
            DETECT_INTERFACE_ENTRY(IWMLicenseBackup)
            DETECT_INTERFACE_ENTRY(IWMLicenseRestore)
            DETECT_INTERFACE_ENTRY(IWMBackupRestoreProps)
            DETECT_INTERFACE_ENTRY(IWMCodecInfo)
            DETECT_INTERFACE_ENTRY(IWMCodecInfo2)
#endif //INCLUDE_DETECT_WMSDKIDL

#if INCLUDE_DETECT_STPD
            DETECT_INTERFACE_ENTRY(IWMCodecInfo2)
            DETECT_INTERFACE_ENTRY(IVideoPlayBack)
            DETECT_INTERFACE_ENTRY(IVideoPlayBack2)
            DETECT_INTERFACE_ENTRY(ITrickPlayProp)
            DETECT_INTERFACE_ENTRY(ITrickPlayLight)
            DETECT_INTERFACE_ENTRY_IID(IMultiFileLoader,IID_IMultiFileLoader)
#endif //INCLUDE_DETECT_STPD

            END_DETECT_INTERFACE()
    }//CoDetectInterfaceFromList 

#endif //FTL_DEBUG
}

#endif //FTL_COM_DETECT_HPP
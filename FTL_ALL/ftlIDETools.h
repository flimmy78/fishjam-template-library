#ifndef FTL_IDE_TOOLS_H
#define FTL_IDE_TOOLS_H

#pragma once

#ifndef FTL_BASE_H
#  error ftlIDETools.h requires ftlbase.h to be included first
#endif

/***********************************************************************************************
* IDE -- ͳһ��������
*   VS6--Windows 9xƽ̨��VS�����汾
*   VS2002(7.0)--.NET Framework 1.0,�������йܴ��룬��.NET�����³������ΪMSIL��΢���м����ԣ������ǻ�������
*   VS2003(7.1)--֧���°汾��.NET 1.1����һ���ȶ��汾
*   VS2005(8.0)--.NET 2.0����ʱ��΢���Ƶ������Ʒ�ġ�.NET��ǰ׺����Visual Studio��Visual Basic
*   VS2008(9.0)--��һ��֧�ֲ�ͬ��.NET�汾������2.0��3.0��3.5��֧��WPF��WCF��WF�Ŀ�����
*     ���� LINQ��Silverlight ��������
*   VS2010()--
* �汾
* 
* Windows SDK
*   7.0 -- Microsoft Windows SDK for Windows 7 and .NET Framework 3.5 SP1
*          http://www.microsoft.com/en-us/download/details.aspx?id=18950
*   7.1 -- Microsoft Windows SDK for Windows 7 and .NET Framework 4
*          http://www.microsoft.com/en-us/download/details.aspx?id=8442
************************************************************************************************/

/************************************************************************************************
* ������ CRT��MFC �ȶ���汾ʱ���ض��� -- ȱ�� DebugCRT.9.0.30729.6161 ���޷����г��������
* 1.������Ӧ�汾��  Microsoft.VC90.CRT.manifest�� msvcr90.dll��msvcp90.dll ���ļ��� ��ִ�г���Ŀ¼ -- �ɽ���󲿷�����
* 2.���� ������.exe.config �ļ�(�� Application Config File��Ӧ�ó�������ʱ��������Ϣ)�����ݿ�����£��������е� name , oldVersion, newVersion �ȣ���
*   <?xml version="1.0"?>
*   <configuration>
*     <windows>    <!-- ע��˴�ԭ���� runtime(ָ��  .NET assemblies ?) -->
*       <assemblyBinding xmlns="urn:schemas-microsoft-com:asm.v1">
*         <dependentAssembly>
*           <assemblyIdentity type="win32" name="Microsoft.VC90.DebugCRT" processorArchitecture="x86" publicKeyToken="1fc8b3b9a1e18e3b"  /> 
*             <bindingRedirect oldVersion="9.0.30729.6161" newVersion="9.0.21022.8"/>  
*             <bindingRedirect oldVersion="9.0.30729.1" newVersion="9.0.21022.8"/>  
*           </dependentAssembly>
*       </assemblyBinding>
*     </windows>
*   </configuration>
*
* ��ǰ���Թ���Ӱ����̫��ķ����Ǹ��� winsxs �е��ļ���(��Ҫ���� WinSxs �������߲����޸�)
* �޸� C:\Windows\winsxs\Manifests\x86_policy.9.0.microsoft.vc90.debugcrt_1fc8b3b9a1e18e3b_9.0.30729.1_none_61305e07e4f1bc01.manifest �ļ���
*   ԭ��ֻ�����У�
*  		<bindingRedirect  oldVersion="9.0.20718.0-9.0.21022.8" newVersion="9.0.30729.1"/>
*       <bindingRedirect  oldVersion="9.0.30201.0-9.0.30729.1" newVersion="9.0.30729.1"/>
*   �������µ� 
*       <bindingRedirect  oldVersion="9.0.30729.6161-9.0.30729.6161" newVersion="9.0.30729.1"/>
*   ע������ļ��Ǵ� SxsTrace ���ɵ���־�ļ����ҳ����ġ�
*       bindingRedirect ָ�� ���оɰ汾��Χ�Ŀͻ�����ӳ�䵽ָ�����°汾�⣬Ҳ�ɶԱ���ֶν���ӳ�䣿��ʽΪ old + �ؼ��� ?
* WinSxs(Windows Side-by-Side)
*
* ���ؼ���
*   A.����ϵͳ�� manifest �ļ���Ӧ�ó��򱾵��ļ�����,����Ϊ <assemblyName>.manifest�� �� Microsoft.VC90.CRT.manifest������Ӧ�� DLL �ļ�(�� msvcr90.dll,msvcp90.dll �ȣ���������ͬ��Ŀ¼
*   B.Ҳ���Խ��� <assemblyName> �ļ��У���������  <assemblyName>.manifest �� ��Ӧ�� DLL �ļ�
* 
* ManiFest -- ��������
*   �ͻ��� -- ���������Exe��Dll�ȣ�ָ�����ļ�������DLL��Ϣ����Ҫ�� type,version, processorArchitecture, publicKeyToken ����Ϣ
*   �������� -- ΢���ȿ����� DLL, ָ�����ļ���ǿ������Ϣ������ͨ�� file( hashalg/hash/DigestValue ��) ָ������ص�������֤��Ϣ
*
* policy�������ļ���ȷ��ӳ���ϵ��-- 
*  
* ǿ��������Ϣ
*   type -- ϵͳ���ͣ��� win32��win32-policy
*   name -- �� assemblyIdentity  ��������Ϣ�� �� Microsoft.VC80.CRT �� policy.8.0.Microsoft.VC80.CRT ��
*   version -- �汾�ţ�������Ƕ���汾�� DLL�� ���������
*   processorArchitecture -- ƽ̨����, �� x86, ia64 ��
*   publicKeyToken -- ��Կ(���ڶ�����ǩ���ģ���ͬ����������ͬ? ΢���� ATL/CRT/OPENMP �� ���� "1fc8b3b9a1e18e3b"���� Common-Controls �� 6595b64144ccf1df)
*
*   hash -- �ļ���Hashǩ����ÿ���ļ�Ψһ��ȷ���ļ��������� -- ��������汾��һ(bindingRedirect), �Ƿ������������
* 
* 
*   ����Ӧ�ó�����ԣ�Manifest������һ����exe/dll�ļ�ͬһĿ¼�µ�. manifest �ļ���
*   Ҳ��������Ϊһ����ԴǶ���ִ���ļ���(Embed Manifest)��
*     Exe ����ԴIDΪ 1( CREATEPROCESS_MANIFEST_RESOURCE_ID )
*     Dll ����ԴIDΪ 2( ISOLATIONAWARE_MANIFEST_RESOURCE_ID )
*     ���⣺3( ISOLATIONAWARE_NOSTATICIMPORT_MANIFEST_RESOURCE_ID ) ��ʲô��?
*           24( RT_MANIFEST ) ��ʲô��?
* 
*   WinXP��ǰ��Windows����� manifest �����ݡ�
*   VS2010 �ֲ���ʹ��manifest�ˣ�
*
*   ע�⣺VC������������ atlassem.h��crtassem.h��MFCassem.h ���ļ������ж����˵�ǰ���뻷���»�ʹ�õ� manifest�� 
*         Ҳ�������ļ���ͨ�����µĴ���ָ�� -- #pragma comment(linker, "/manifestdependency  ...)
*           �� stdafx.h ��ָ�� Microsoft.Windows.Common-Controls
*   
* manifest �е� assemblyIdentity ��Ϣ�� winsxs ���ļ��Ĺ�ϵ( ǿ�ļ�����Ϣ -- ϵͳ�п����ж����ͬ�汾����ͬ�Ŀ⹲������ᷢ����ͻ)
*   xml�е���Ϣ��<assemblyIdentity type="win32" name="Microsoft.VC90.CRT" version="9.0.21022.8" processorArchitecture="x86" publicKeyToken="1fc8b3b9a1e18e3b"></assemblyIdentity>
*   ��Ӧ�ļ�Ϊ ��C:\Windows\winsxs\x86_microsoft.vc90.crt_1fc8b3b9a1e18e3b_9.0.21022.8_none_bcb86ed6ac711f91\msvcp90.dll
*            ����C:\Windows\winsxs\<processorArchitecture>_<name>_<publicKeyToken>_<version>_<???>_<???>\xxx.dll
*
* "Unable to start program" ����Ľ��(ʹ�� SxsTrace ���ߣ�ֻ�� Vista/Win7 ���� ?)
*  1.�Թ���Ա�˺����� cmd.exe�����л���Ŀ�ĳ����Ŀ¼��
*  2.SxsTrace Trace -logfile:SxsTrace.etl
*  3.ִ�� ������ĳ���
*  4.SxsTrace Parse -logfile:SxsTrace.etl -outfile:SxsTrace.txt�� ���ɶ� mainfest ��������־�ļ���
*  5.������־�ļ������в��ҡ�����/Error�� �ȹؼ��֣�������ʾ�Ĵ����ļ�ͨ�� EveryThing �ȹ��߽��в���ȷ��
*    ��(SxsTrace���������Ĵ�����Ϣ)
*    ����: �޷������ο� Microsoft.VC90.DebugCRT,processorArchitecture="x86",publicKeyToken="1fc8b3b9a1e18e3b",type="win32",version="9.0.30729.6161"��
*    ������Debug �� XXX.exe.embed.manifest �� XXX.exe.intermediate.manifest �������˱����ϲ����ڵ� Microsoft.VC90.DebugCRT �汾
*         (������ WinSXS ��ֻ�� 9.0.21022.8 �� 9.0.30729.1 �� x86_microsoft.vc90.debugcrt �汾�����嵥�ļ��������� 9.0.30729.6161 �汾)
*    ԭ�������ӵľ�̬��ʹ���� 9.0.30729.6161(��ͨ�� dumpbin /all ���ض���������� manifestdependency ȷ��)
*      
*   vcredist_x86.exe �ļ����汾������·��
*      9.0.30729.1(VS2008 SP1) -- http://www.microsoft.com/en-us/download/confirmation.aspx?id=5582
*      9.0.30729.6161 -- http://www.microsoft.com/en-us/download/details.aspx?id=26368
*   
* Fuslogvw.exe -- Assembly Binding Log Viewer,��ô�ã�
************************************************************************************************/

/******************************************************************************************************************************
* ���� SxsTrace ȷ���������ĸ���ԭ��?
* VS2008 ����
* 1.��������Ϊ manifest �ļ���������(processorArchitecture�ȳ������⣬����Ϊ0xc000007b)��
*   �����޷���������Ҫ�ӹ�����ɾ��manifest�ļ�����stdafx.h���������´��룺
#ifdef _UNICODE
#  if defined _M_IX86
#    pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#  elif defined _M_IA64
#    pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#  elif defined _M_X64
#    pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#  else
#    pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#  endif
#endif
******************************************************************************************************************************/


//�ж�ָ��IP�Ļ�����Ϣ(�������)�� nbtstat -a IP

#pragma TODO(���ǲο�һ�� ATL_DLL_ ���÷�)
// ��������˸ĺ꣬���̽�ʹ��ATL.DLL������ATLIMPL.CPP �ļ�����ʵ����Ϊģ����һ���ְ���

namespace FTL
{
    //Wp64 ���棬�µ�IDE�ǲ���û����������ˣ�
    // Gotta love it.  I've NEVER found a way to cast away the /Wp64
    // warnings with the functions Microsoft tells you to use for
    // Win64 readiness!
#pragma warning ( disable : 4244 )
    //����(�� SetWindowLongPtr)
#pragma warning ( default: 4244 )

    /*************************************************************************************************************************
    * IDE��������͹���
    *   nmake
    *   dumpbin �����ִ���ļ�(EXE��DLL)����Ϣ��
	*   mt.exe -- manifest �Ŀ��ƹ���
	*     mt.exe -inputresource:ExePath;#1 -out:extracted.manifest <== ���� Exe �е� manifest
    *   ϵͳ�豸ɨ�蹤�� -- gmer.exe
    *   �����ܵ�ɨ�蹤�� -- pipelist.exe(http://technet.microsoft.com/zh-cn/sysinternals/dd581625)
    *************************************************************************************************************************/

    /*************************************************************************************************************************
    * Avalanche -- һ��(HTTP)ѹ�����Ե�Ӳ����
    *************************************************************************************************************************/

    /*************************************************************************************************************************
    * C++ Test,һ�����ɽ���������ɽ���
    *   a.��̬����(Static Analysis) -- ��̬��������,�����ָ������淶�����һ����
    *   b.��Ԫ����(Unit Testing) -- �����Զ����ɴ�����Ԫ��������������ִ���쳣�����ܡ��ع�ȶ��ֲ��ԣ�
    *     �ɲ������Ը����ʡ��ڲ��Կ���ڣ��ṩ��˽�кͱ�����Ա�����Լ���Ա�����ķ��ʡ�
    *     ͨ��׮����(������ǰ׺CppTest_Stub_)�ṩ����������ռλ��ʹ�ÿ��Զ����ؽ��е�Ԫ���Զ��������ⲿ�ļ�������
    *   c.Ƕ��ʽ֧��(Embedded Support)
    *   d.���븴��(Code Review)
    *   e.�Զ�����򴴽�(Rule Wizard) -- ��ͼ�λ��ķ�ʽ��ͨ����������ͼ�η�ʽ�������Զ���������ͨ���ṩ��ʾ��������Υ���Ĵ��룩��
    *     ��Ҫ python ��֧��?
    *   f.�������?[ԭ�����Ե���?(Native Tests Import)]
    *   g.BugDetective(���������ṩ) -- ģ��Ӧ�ó���ִ��·������ʶ����ܴ�������ʱȱ�ݵ�·�����漰��ʶ��͸��ٸ���·����
    *   h.�����н���(Desktop/Extended Command Line Mode)
    *
    * TCM(Team Configuration Manager -- �Ŷ����ù�����),ȷ�������Ŷӳ�Ա���ܷ�����Ӧ�Ŷӵ� Test Configuration���������ã���
    *   suppressions(����)�������ļ��Ͳ��������ļ���Ȼ���ɸ��������Ա����Import��
    *   �Ż����Ŷ����÷�ʽ�ǣ�
    *     ���Ŷӹ��������ϰ�װһ�� C++test��Server Edition��-- ÿ���Զ�����,��֤����Ĵ���,��������͵�GRS�������Ա.
    *     ��ÿ̨������Ա����վ�ϰ�װһ�� C++test��Developer Edition�� -- �����Լ���д���޸ĵĴ���,������Դ�������.
    *     �ڼܹ����ʦ�����ϰ�װһ�� C++test��Architect Edition����
    *     ���Ŷӹ�����������һ̨�Ŷӻ����ϰ�װһ�� Team Configuration Manager���Ŷ����ù���������
    *   ���裺
    *     1.������Ĭ��װ�����ã�����ʾ�򵼣��������ý��а�װ�����ã�-- ��װһ�κ󣬻����� setup.iss�ļ�������
    *       ��ֵ����Ҫ��װ�Ļ����ϣ���ʹ�� -s ��־���� setup.exe
    *     2.������ C++test ������Դ�������(����ȷ����������,�Ӵ�����л�ȡͳһ�Ĵ���):
    *       Preferences->Scope and Authorship �� Source Control
    *     2.������ C++test ��װ������ TCM(�ṩ�ļ��������ļ�����):Preferences->Team
    *     3.�� C++test ������������ GRS(�ṩ�Ǳ��塢����Ͷ���ֵ,�ɰ���������Ŀ����������׼�����).
    *     4.�����ŶӲ������ú͹���
    *     5.������������
    *     6.���г�ʼ���Ժͼ�������
    *     7.ͨ��Դ�������������Ŀ�Ͳ�����Դ
    *     8.�����Զ�ҹ�����

    * �����������(�簲ȫ�ԡ��Ż�����ʼ����),�������Էּ�.
    * �����ڼ���Test Configuration����ѡ��Ĺ������ܼ�飬��ƥ��Suppressions(����)�����Ľ����������ʾ����.
    * �ȿ���ͨ��IDE����,Ҳ����ͨ�������н��桢��������ʽ���лع���ԡ�
    * ����֧��Ƕ��ʽƽ̨�ͽ�������������Ժ� Eclipse + CDT һ������
    * GRS(�鱨��ϵͳ)--��һ������֧��ϵͳ,ʹ�������������̾��п����Ժ��ܿ���

    * ������(cpptestcli.exe)--������������,��������͸�GRS��TCM.
    * AEP(�Զ�����Ԥ��)--�Զ��������岽��:ʶ�����->�ҵ�����ԭ��->��λ��Ʒ��������ĵط�->�޸����е�ʵ��->��������
    *
    * C++Test�����������Ĵ������⹲��Ϊ5�����س̶�:
    *   1.I   -- Informational
    *   2.PV  -- Possible Violation
    *   3.V	  -- Violation
    *   4.PSV -- Possible Severe Violation
    *   5.SV  -- Severe Violation
    *
    *************************************************************************************************************************/

    /*************************************************************************************************************************
    * ClearCase(CC) -- 
    *   IBM�İ汾����ϵͳ������C/S�ṹ��֧�ִ��͡�����������֧�ֿ�ƽ̨(Win/Linux)��������һ������eclipse��CCRC(CC Remote Control)���ߣ�
    *   ��ʹ������Ϊ MVFS �������ļ�ϵͳ(Clear Case Virtual File System)
    *
    *   VOB(Versioned Object Base) -- �汾����⣬�����˽��а汾���Ƶ��ļ���Ŀ¼(Element)������������Ԫ����(Metadata)��������
    *     �������ˣ��û���VOB�����в�������Ҫͨ��View����ͼ����ʵ��
    *   View -- �ͻ��˲�������ͼ�������֣�
    *     ��̬Dynamic : Զ�˾���ʵʱ���£��ʺϹ�����Ա
    *     ��̬Snapshot : ���ؾ����ֶ����£��ʺϿ�����Ա
    *   Version -- �ͻ���ÿ�� CheckOut->Edit->CheckIn ��������һ����Version
    *   Version Tree(�汾��) -- һ��Ԫ�ص����а汾��ɵ���״�ṹ��չʾ��Ԫ�ذ汾���ݻ����
    *   Label(��ǩ) -- ����ĳһ�汾�ϵı�ʶ��ͨ��ת����ʱ��Ҫ���ǩ����ǩ��ͬ�汾��һ�£����Ժ���Կ��ٶ�λ���뼯��
    *   Bracnch(��֧) -- ������VOB��������Ԫ���ض��汾�ļ��ϣ���ʵ�ֲ�ͬ�汾�Ĳ��п������÷����ʾ�� -- ע���Label������
    *   ���ù��(Config Spec) -- ȷ����VOB������View��Ԫ�ؼ���汾��ȡ��ϸ�ڣ�������ÿ��profile��
    *     CCRC�У�����->��ʾClearCase��ͼ����->�汾ѡ����� -> �����Ӧ��Ԫ�ذ汾���������£�latest��
    *************************************************************************************************************************/

    /*************************************************************************************************************************
    * CppUnit --
    *    VC9����ʱ���ִ���error C3505: cannot load type library '{80CC9F66-E7D8-4DDD-85B6-D9E6CD0E93E2}
    *        ����Ӧ������ (MsDevCallerListCtrl.cpp:67 ) �� version("7.0")  ��Ϊ version("9.0") ����
    *************************************************************************************************************************/

    /*************************************************************************************************************************
    * curl-loader -- http://curl-loader.sourceforge.net
    *   ��Ƶ��http://www.boobooke.com/v/bbk1808/
    *         http://www.boobooke.com/v/bbk1809/
    *         http://www.boobooke.com/v/bbk1835/
    *   һ����C���Ա�д�� Web Ӧ�ò��Ժ�ģ�⸺�ع��ߣ�һ�����̿���ģ��2.5K~10K�Ŀͻ��˲�������
    *   ����ÿ���������Բ�ͬ��IP��ַ(����LoadRunner��IPαװ)��֧���û���֤����¼��Web�������̡�
    *   ��ͨ�� libcurl ��֧�� sftp��telnet��tftp��ssh��scp ��Э��
    * ��װ(����ʹ��root�û�)��
    *   tar -zxvf ��ѹ�� make ����
    *   ./curl-loader -f �����ļ� [ѡ��]
    * �����ļ�(conf-examples)
    * ѡ��
    *   -c :����ʱ�ĳ�ʱ
    *   -d :��ϸ��־(���������ݻᱣ�浽�ı����ʺ��ı�����)
    *   -i ����ʱ�䣬Ĭ��3��
    *   -f �����ļ�: 
    *   -r :��ֹ����TCP����(�رպ����´�)
    *   -t �߳���:�ڶ�CPU�����CPUʱ���Բ�������ѹ��( cat /proc/cpuinfo )
    *   -v :verbose
    *   -u -- 
    * ѹ��ͼʾ
    *   D -- ��Ӧʱ��(ms)
    *   Response Time(��Ӧʱ��)
    *   Throughtput(������)����ΪTi(nput)��To(utput)
    *************************************************************************************************************************/

    /*************************************************************************************************************************
    * Devpartner(VC6) -- ��̬���Թ��ߣ���ҪӦ���ڰ׺в��ԣ������ڵ�Ԫ���Խ׶κͼ��ɲ��Խ׶Ρ�
    * ֻ�ܼ�鱻ִ�й��Ĵ������Ƿ���ڴ����ڿ�ʼʹ��BoundsCheckerǰ���ƶ�����ȫ��Ĳ�������
    * �ǳ���Ҫ(ʹ�� TrueCoverage )��
    * �������������ӹ��ܣ�
    *  1.BoundsChecker --Run-Time������
    *    ����ʹ��ģʽ��
    *    a.ActiveCheck -- �����ݣ����ܼ��Ĵ�����������(�ڴ����Դй¶��API����ʹ�ô���)
    *    b.FinalCheck()-- �������м������(�ڴ�������),�����������ٶȱ�����ʹ��BC�ı���������Rebuild����
    *  2.TrueCoverage -- (���)������ͳ�ƹ��ߣ������ڲ������������
    *    ���εĲ��Խ�����Ժϲ���
    *    ѡ�С�Instrument inline function��-- ���Բ������������ĸ�����(�����ڼ��ģ���Ч�ʣ�)
    *  3.TrueTime -- ���ܲ��Թ��ߡ�
    *    Ϊ���������ṩ�����ֲ�ͬ�ĵ�λ(Scale)���ֱ��ǣ�CPUʱ�����ڡ�΢����롢��.
    *    ʹ��"Quantum"������ֻ���㻨��Ӧ�ó�����߳��ϵ�ʱ��Ƭ��
    *    �䡰Real����ʾ��ʱ���ǡ�����ϵͳ��ͬʱ�����ŵ�����Ӧ�ó�����߳����ķѵ�ʱ��(ǽ��ʱ�䣿)��
    * 
    *
    *************************************************************************************************************************/


    /*******************************************************************************************************************************
    * �ĵ�����doxygen -- ������ĳ�ָ�ʽ��ע���Զ�ת��Ϊ��Ӧ���ĵ� http://www.stack.nl/~dimitri/doxygen/index.html
    * ���裺
    *   1.ΪProject ���������ĵ� -- ����ʹ�ø������� Doxygen Wizard ����(�������� ��doxygen -g �ĵ��� ����һ��ȱʡ�������ĵ�)
    *     һЩ Key ��ֵ���趨��ÿ���趨Ϊһ�С�����һ�п�ͷΪ"#" ��ʾ����Ϊ��ע��
    *     TAG = value [value, ...] �� TAG += value [value, ...]
    *     ���� TAG ѡ��:
    *       PROJECT_NAME -- Project �����֣���һ������Ϊ�������������ʹ��˫������ס
    *       PROJECT_VERSION -- Project�İ汾����
    *       OUTPUT_DIRECTORY -- ���·�������û�������·����������Ŀǰ����·������Ϊ���·��
    *       OUTPUT_LANGUAGE -- Ԥ��ΪEnglish������ʹ��Chinese-Traditional ��������ķ���
    *       INPUT -- ָ�����ػ���ѰҪ�����ĳ�����뵵��·���������һ������ʽ����̬�����ҿ�ָ��������·����
    *         ������˵������a.c, b.c, c.c ��������������ʹ��INPUT = a.c, b.c, c.c �ķ�ʽ��
    *         ��������һ��Ŀ¼����Ŀ¼�������е������ᱻ������
    *       FILE_PATTERNS -- �ļ���չ����������� INPUT Tag ��ָ����Ŀ¼��������͸�����Tag��Ҫ��Doxygen�ڴ���ʱ��
    *         ֻ����ض����ļ����ж��������磺��ϣ����Ŀ¼�µ���չ��Ϊ.c, .cpp��.h�ĵ�����������
    *         �����趨 FILE_PATTERNS = *.c, *.cpp, *.h��
    *       RECURSIVE -- ����ֵ��ֻ����YES��NO�����趨ΪYESʱ��INPUT��ָ��Ŀ¼��������Ŀ¼���ᱻ������    
    *       EXCLUDE -- �������ĳ�����ض���������Ŀ¼����ϣ������Doxygen���������������Tag��ָ����
    *       EXCLUDE_PATTERNS -- ������FILE_PATTERNS���÷���ֻ�����Tag�ǹ�EXCLUDE��ʹ�á�
    *       SOURCE_BROWSER -- ����趨ΪYES���� Doxygen �������Դ�ļ����б����Թ����ġ�
    *       INLINE_SOURCES -- ����趨ΪYES ����������Ҳ�ᱻǶ����˵���ļ��С�
    *       ALPHABETICAL_INDEX -- ����趨ΪYES����һ��������ĸ������б�������ڲ������ļ��С�
    *       GENERATE_HTML -- ���趨ΪYES,�����HTML�汾��˵���ļ���HTML�ļ���DoxygenԤ������ĸ�ʽ֮һ
    *       HTML_OUTPUT -- HTML�ļ������Ŀ¼������һ�����·��������ʵ�ʵ�·��Ϊ OUTPUT_DIRECTORY ���� HTML_OUTPUT ������趨Ԥ��Ϊhtml��       
    *       HTML_FILE_EXTENSION -- HTML�ļ�����չ����Ԥ��Ϊ.html��
    *       HTML_HEADER -- Ҫʹ����ÿһҳHTML�ļ��е�Header�����û��ָ����Doxygen��ʹ���Լ�Ԥ���Header�� 
    *       HTML_FOOTER -- Ҫʹ����ÿһҳHTML�ļ��е�Footer�����û��ָ����Doxygen��ʹ���Լ�Ԥ���Footer�� 
    *       HTML_STYLESHEET -- ���ɸ���һ��CSS ���趨����HTML����������������
    *       GENERATE_HTMLHELP -- ���趨ΪYES��Doxygen�����һ�������ļ�����������ļ�������Ҫ���� windows �ϵ�HTML��ʽ��HELP����ʱ���õ��ϡ�
    *       GENERATE_TREEVIEW -- ���趨ΪYES��Doxygen���������һ����״�ṹ���ڻ�����ࡣ�����״�ṹ����JavaScript��д�ɡ�������Ҫ�°��Browser������ȷ��ʾ��
    *       TREEVIEW_WIDTH -- �����趨��״�ṹ�ڻ����ϵĿ��ȡ�
    *       GENERATE_LATEX -- �趨ΪYES ʱ�������LaTeX ���ļ�����������ϵͳ����Ҫ�а�װLaTeX ����ع��ߡ�  
    *       LATEX_OUTPUT -- LaTeX�ļ������Ŀ¼����HTML_OUTPUT�÷���ͬ��һ����ָ��OUTPUT_DIRECTORY֮�µ�·����Ԥ��Ϊlatex��                         
    *       LATEX_CMD_NAME -- LaTeX������������Ƽ��������ڡ�Ԥ��Ϊlatex 
    *       GENERATE_RTF -- ���趨ΪYES ��������RTF ��ʽ��˵������
    *       RTF_OUTPUT -- ��HTML_OUTPUT �÷���ͬ������ָ��RTF �������·����Ԥ��Ϊrtf��
    *       GENERATE_MAN -- ���趨ΪYES ��������Unix Man Page ��ʽ��˵���ļ���
    *       MAN_OUTPUT -- ��HTML_OUTPUT �÷���ͬ������ָ��Man Page�����Ŀ¼��Ԥ��Ϊman��
    *       GENERATE_XML -- ���趨ΪYES ��������XML ��ʽ��˵���ļ���
    *       ENABLE_PREPROCESSING -- ���趨ΪYES ����Doxygen ������C ��ǰ�ô�����������ԭʼ����
    *       PREDEFINED -- �����������ж���һЩ�ꡣ������gcc �е�-Dѡ��
    *   2.�ڳ�������м��Ϸ���Doxygen��������ע��ʽ
    *     ԭ���ϣ�Doxygen �����������ṹ�йص�ע�ͣ��� �ļ����ࡢ�ṹ���������������� �ȵ�ע�͡������Ժ����ڱ���������ȵ�ע�͡�
    *     ͨ��һ��ע�Ϳ����һ����Ҫ˵����brief����һ����ϸ˵����detailed��������ͷ�ļ��и������Ͷ��壬�ؼ���������Ķ��壬
    *     doxygen�ṩ��group�ĸ�����ɵ�ģ���ע�ͻᵥ������һ��ģ���ҳ����(defgroup)����
    *     �κ���������������ࡢ�����������ļ������Ҫ���뵽ĳ��ģ�飬��������doxygenע����ʹ��ingroup����ɡ�
    *     Group֮��ʹ��ingroup������������״��ϵ��
    *
    *       ����ǰʹ�� @brief �������Ҫ˵������һ�к����д����ϸ��ע�ͼ��ɡ�
    *     / **
    *      * @brief [some brief description ]
    *      *      [ brief description more. ]
    *      * @param
    *      * 
    *      * /
    *     �����¸�ʽ��
    *       /// ע�ͣ�
    *       //! ע��
    * 
    *     Doxygen����ע�����
    *        @addtogroup ������
    *        @author ����
    *        @brief  ��Ҫ˵��
    *        @bug ���ڵ�Bug˵��
    *        @code ע����˵���Ĵ��� -- �� @endcode ����
    *        @date ����
    *        @defgroup ���� ������
    *        @deprecated -- ˵���ǲ���֧�ֵ�
    *        @exception �쳣���� [�쳣˵��]
    *        @file �ļ���
    *        @ingroup [group_name] -- ����һ���ض���ע����
    *        @image html "��Ҫ�����ͼƬ�ļ���"'
    *        @name ����ע�͵�����
    *        @param ������ [in|out|in,out] ����˵��
    *        @post ����֮���ʹ������
    *        @pre ǰ������
    *        @relates �ǳ�Ա���������� -- ͨ�������ѷǳ�Ա������ע���ĵ����������˵���ĵ���
    *        @see ��������
    *        @since �汾������ -- ͨ������˵����ʲô�汾��ʱ��д�˲��ִ���
    *        @version �汾
    *        @warning ������Ϣ
    *        @return ����ֵ˵��
    *        @retval value �ض�����ֵ������
    *        @todo Ҫ��������˵��
    *    ��չ��
    *        @par requirements  -- ��Ҫ�Ļ���
    *        @par license
    *    �б���
    *        @return 
    *         <dl>
    *           <dt>escSuccess</dt> <dd>������Ϣ</dd>
    *           <dt>escInvalidParameter</dt> <dd>������Ϣ</dd>
    *         </dl>
    *   3.ʹ��Doxygen�������ĵ�
    *     ���Ҫ����CHM��ʽ���ĵ��������ڡ�HHC_LOCATION����ָ�� HTML Help Workshop �� hhc.exe ��·��
    *
    * ����
    *   GhostDoc -- VS����Ѳ��������������Ա��дXML��ʽ��ע���ĵ�
    *******************************************************************************************************************************/

    /******************************************************************************************************************************
    * Git -- ��������linux�ں˿����Ŀ�Դ�ķֲ�ʽ�汾����ϵͳ������Ҫ��������������֧��
	*   ����ʽ(SVN) -- �����Ƿ��������зǳ���ȷ��Ȩ�޹�������(��֧��������)�����ʺϿ�Դ����
	*   �ֲ�ʽ(Git) -- �����߿��Ա������߹���(�ύ���ϲ���������֧�ȣ�ÿ�������߻����϶���һ�������������ݿ�)��Ȼ�󱾵��޸ĺ����ɲ������͸��������ߣ�
	*     ��������ȷ�Ϻ�ϲ�������������
	* 
    *   EGit -- Eclipse �µ� Git Plugin
    *   TortoiseGit -- Windows �µ� Git Plugin
    *   GitHub -- ���� Hudson ����?
    *   �뿪��һ�� nGit(�ο� Git �� Gemit��ʹ�ã�����VS���IE View)
	*   �������(git, ʹ�� git help ���� ������ʾ��ϸ��HTML����)
	*     add �ļ� -- �����ύ���ļ�add��
	*     branch -- ��֧�������� ������֧���鿴��֧��ɾ����֧
	*     checkout -- �л���֧
	*     clone Զ��Ŀ¼ ����Ŀ¼ -- �ӷ������Ͽ�¡���ݿ⣨��������Ͱ汾��Ϣ����������
	*     commit -- �ύ���������ݿ�
	*     config -- ���������Ϣ������email��name
	*     fetch -- �ӷ������������ݿ⣬���ŵ��·�֧�������Լ������ݿ�ϲ�
	*     init -- ����һ�����ݿ�
	*     log -- �鿴�汾��ʷ
	*     merge -- �ϲ���֧����Ŀ���֧�ϲ�����ǰ��֧
	*     pull -- �ӷ������������ݿ⣬�����Լ������ݿ�ϲ�
	*     reset -- �ָ���֮ǰ�İ汾
	*     show -- �鿴�汾�Ŷ�Ӧ�汾����ʷ�����������HEAD�鿴���°汾
	*     whatchanged -- �鿴������֧�ı仯
    * Gerrit -- ����Web�Ĵ������󹤾ߺ���Ŀ��������
    *  
    *******************************************************************************************************************************/

	/******************************************************************************************************************************
	* GNU ����(Windows �µ�)
	*   MinGW(http://mingw.sourceforge.net/) -- ������û�е�������̬���ӿ�������ʹ��GCC����Win32����
	*     ���к�ͨ�� mount �ɼ��ر����ϵ����д���, ��ֱ�ӷ��ʣ����� / �¿�����)��Ҳ�����޸� MinGW\msys\1.0\etc\fstab �ļ�ָ��Ŀ¼
	*   yasm -- �������ص����ĸ��汾���������Ϊ yasm.exe,���� bin ��
	******************************************************************************************************************************/

    /******************************************************************************************************************************
    * Google Performance Tools(google-perftools)
    * 
    *******************************************************************************************************************************/

    /*******************************************************************************************************************************
    * Google Code(http://code.google.com)
    *  Project Hosting(��Ŀ�й�)-> Project Hosting on Google Code

    *******************************************************************************************************************************/


    /******************************************************************************************************************************
    * GUI Design Studio -- ����ԭ����ƹ��ߡ������������� AxureRP-Pro(��Ҫ����Web?),Balsamiq mockup,
    *   UIDesigner(��Ѷ���Ĵ���ԭ����ƣ������ִ���ļ�)
    * ����
    *   ��Ļ���� -- ��ɫ���ο�
    *   ��Ԫ��(Representative Element) -- ��ʾ����ʼԪ��
    *   ���°�(StoryBoard)��� -- �ṩ���ڹ�������ĵ��п������̵�Ԫ����ʵ��һ��ģ��ԭ��
    * �������ͣ�
    *   Modal Popup -- ģ̬����
    *   Modal Choice Popup -- ��������������ⴰ�ڻ��Զ��رյ�ģ̬���ڣ�������Ƕ�׼����˵�����������ʽѡ�񴰿�
    *   Show Window -- ��ģ̬���ڣ������ڲ����滻����
    *   Hide Window
    *   Toggle Window -- �л���ʾ
    *   Toggle Overlay -- ͬToggle Window����Ŀ�괰�ڲ��Ǳ�����¼�����������Ӧ��꾭���¼���ͬʱ����ť��ѡ�������Ԫ����Ȼ��Ҫ�����л������
    ******************************************************************************************************************************/

    /******************************************************************************************************************************
    * Hudson -- ��������(Continuous Integration)�Ĺ���
    *   NHN�İ�����http://devcafe.nhncorp.com/QPTalk/529396
    * ִ�����̣�Svn Update->Build(batch/shell)->PostBuild(����gtest���Ա����)
    * 
    * ���ذ�װ��
    *   1.http://jdk6.java.net/download.html, �����ذ�װJDK(6.0),���� JAVA_HOME 
    *   2.http://tomcat.apache.org/ <== 6.0, Binaray Distribution, Core zip����ѹ���� TOMCAT_HOME 
    *   3.http://hudson-ci.org/downloads/war/  <== �� 1.395�汾�Ժ��Ϊ hudson �� JeXXXX ? ������֧��
    *     �� hudson.war ���� TOMCAT �� webapps Ŀ¼�У�
    *     ���� HUDSON_HOME ������ָ���Ĺ���Ŀ¼(�������ص�Դ����б���)��Ĭ��Ϊ CSIDL_APPDATA
    *   4.���ؽ�ѹ apache-maven ? �� ���� MAVEN_HOME 
    *   ͨ�� startup.bat ����������Tomcat,Ȼ����� http://localhost:8080/hudson/�� ֹͣʱ�� shutdown.bat
    *
    * ����
    *   Build Triggers -- schedule �����α�ʾ ����/Сʱ/��/��/�ܣ� �� X/10 * * * * ��ʾÿ10���Ӵ���һ�� 
    *     Commit Build -- ��ѯ(��10����)���б䶯ʱ�����͵�Ԫ���ԣ���֤��ʱ���ֱ�������
    *     Integration Build -- ��ʱ(��ÿ��),�������������ȫ�����(UT����̬��CC��)
    *   NHN Project Plugin(Assign NHN Project) -- ����NHN��Project�����ã�
    *     ��Ŀ���������淶?
    * 
    * �ļ�·��: ** ��ʾĿ¼����Ŀ¼���� JUnit ����ļ�Ӧ������Ϊ xx/test_detail.xml ��ʾ������ǰĿ¼����Ŀ¼�еĸ��ļ�
    * 
    * ����Plugin -- http://wiki.hudson-ci.org/display/HUDSON/Plugins
    *   Publish JUnit test result report -- ���Բ鿴 JUnit/GTest���Ժ�� xml(�� gtestĬ�� ��test_detail.xml)
    *   locale <== ���ػ�(��ֹ������ʾ�к��ġ�������ʾӢ�ĵ�), ����Ϊ en_us ��ѡ��"Ignore XXX"
    *   E-mail Notification -- ���򼯳�������ʱ�����ʼ�֪ͨ����Ҫ�������ʼ���������
    *   Quality Dashboard(NHN) -- ͨ��plugin�Ӹ���Hudson�������ϱ�ָ�굽ͳһ��QD������������ͳһ��չʾ��
    *     http://nsiq.nhncorp.com/, ÿ����Ŀ��Ψһ��API KEY��������
    *     ѡ�������ʱ��
    *       Analysis Defect Density ѡ�� klocwork
    *       Coding Standard Conformence Rate ѡ�� NSIQ cppstyle
    *       CC ѡ�� NSIQ Collector for Complexity
    *   SimpleUpdateSite(NHN) -- ���ƻ���Plugin��������,http://devcode.nhncorp.com/projects/updatecenter/downoad,
    *     ���ذ�װ�������� New RSS URLΪ http://devcafe.nhncorp.com/QPTalk/notice/rss
    *     ���� UpdateSiteURL Ϊ http://hudson02.nhncorp.com:9080/update/SimpleUpdateSite.json
    *   JUnit test result report-- ���Բ鿴JUint��Gtest�����ɵĽ��XML(��GTest��Ӧ�� xx/test_detail.xml--ע:���Ǻ�)
    *   CovComplPort(NHN) -- ͬʱ��ʾ�Ƚϸ��ӶȺ͸����ʵĲ��,Խ���ӵ�ԽҪ���ԣ�Ŀǰֻ֧��Clover��
    *     ���ʱΪ Publish Coverage/Complexity Scatter Plot
    *
    * ��������ά�ȼ�ͳ�Ʋ��
    *   Coding Convention(����淶) -- N'SIQ CppStyle(http://nsiqcppstyle.appspot.com)
    *   Static Analysis(��̬����) -- klockwork, http://devcafe.nhncorp.com/QPTalk/205688
    *     ���� Host:klocwork9.nhncorp.com, Port:27000, ��װʱ�汾Ϊ 9.X, �е�����build��
    *     Linux:����Config FileName Ϊ���������ű�����ɾ�������е��������ظ�����?��
    *     Windows:����Config FileName Ϊ xxx.sln, Build Parameter ������ --config Debug ��
    *   Code Coverage(���븲����,Ҫ�����>50%,��֧>40%) -- BullsEye/GCov, �ռ������� Clover/Cobertura
    *     ʵ�ֻ��������֣�1.���������(GCov); 2.����Դ���ٽ��б���(BullsEye Coverage)
    *     1.GCov(Linux) -- ��������ѡ���м���: -fprofie-arcs -ftest-coverage -lgcov
    *       ��Cobertura�ɼ��������������������Ҫ�� gcovr -r src --xml > coverage.xml ��������Ҫ���ļ�
    *     2.Bullseye Coverage(Win/Linux) -- Ҫ���� COVBUILDZONE=${WORKSPACE}; COVFILE=${WORKSPACE}/test.gov,
    *       VS2008 �п���ͨ�� Tools->Enable/Disable Bullseye Coverage Build ����򿪻�ر��书��
    *       cov01 -1(��ѡ��); make clean all; cov01 -0(�ر�); ִ�в��Գ���; bullshtml target/coverage(��ָ��Ŀ¼�����ɸ����ʽ��);
    *       ��Clover�ɼ��������report directory ���� target/coverage, file name Ϊ cover.xml
    *   LOC/CC(Ȧ���Ӷ�,Ҫ��<30) -- N'SIQ Collector, ע����Ҫ�ų����������롢Lex/Yacc���Զ����ɡ�Windows����Ϣӳ���
    *     http://dev.naver.com/projects/nsiqCollector, ���غ�������ʱ�����ִ���ļ���·�����е�����Build Step
    *   CQ -- Code Quality
    *   Code Duplication Analysis(�����ظ�����) -- CPD
    *   Defect density(ȱ���ܶ�,Ҫ�� < 4/KLOC)
    ******************************************************************************************************************************/

    /******************************************************************************************************************************
    * Intel C++��������9.0 ����߰汾�� -- ֧�� OpenMP* �淶 2.5 ��
    *   �Ż�������
    *   Linux	Windows	      ����
    *    -O0	  /Od	    ��ֹ�Ż�
    *    -g	      /Zi	    ���ɱ��
    *    -O1	  /O1	  �Ż������ƴ���
    *    -O2	  /O2	  �Ż��ٶȣ�Ĭ�ϣ�
    *    -O3	  /O3	   �Ż����ݻ���
    *   
    * 
    * Intel VTune ���ܷ����� 7.2 ����߰汾
    *   ��Ҫ���ܣ�
    *     a.ȡ�����ܡ���������ͼ���ܡ��������������Լ������Ľ���Intel��������;
    *       ȡ������ -- ���԰��������߷ֱ������������ʱ��ĺ�����ģ�飬����������ϵͳ��Ӧ�ó������ϸ��ͼ��
    *       ��������ͼ -- ͨ��������������ʱ��������ڵ�ͳ��ڵ㣬����һ�ŵ�������ͼ����ȷ������˳�����ʾ�ؼ�·����
    *       ����������� -- ������ʱ����ϵͳ���ȷ���Ƿ����Ϊ�����ڴ���ٻ��ļ�����/����������������Ӧ�ó����ٶȱ�����
    *       Intel��������(ǿ���Ƽ����úÿ���) -- ���ݷḻ��֪ʶ��ֱ��������⣬�Զ��Ƽ�����Ľ��취��������߿����ߵĹ���Ч�ʡ�
    *         ���ṩ�˻������ܼ���������ͼ���Լ�����Windows����ϵͳ��Ӣ�ض����´������Ľ��飬�Լ�Դ����ĸĽ����顣
    *     b.��Windows��ͬʱ�ṩͼ�λ��������н��棬Linux �£�
    *     ����ʶ������̹߳��ϣ����������ã����������ȣ���ͨ��ʶ��Դ����ʶ������ƿ����ʶ����Դ���롢ʶ���Ч�߳���Ƶ�,
    *     ��VTune Performance Analyzer Essentials��
    * Intel Thread Checker �̼߳����2.2 ����߰汾
    *    �ɿ��ٲ��Һ��޸�Win*��OpenMP*�߳������е�bug��
    *    ��س���ִ�й����е��߳���Ϊ���������д��ڵľ��������߳������Լ�Ǳ�ڵ��߳��������⣬
    *    ��ʾͬ�̴߳�����ص�Դ����λ�á���Ȩ�����Լ���ջ���ٵȡ�
    * 
    * Intel Thread Profiler �̵߳����� 2.2 ����߰汾
    *    ���԰�������Win32*��OpenMP*�̻߳����������ܡ�
    *    ͨ����س��������������߳����ܵ�������⣬�����̹߳��غ�ͬ����ͻ���ܹ��������Ҹ���ƽ��, ͬ���������߳��������⡣
    *    ���Խ��йؼ�·��������
    *
    * Intel ��ѧ���ĺ�����( MKL ) -- ��һ�׾����߶��Ż��ġ��̰߳�ȫ����ѧ�����⡣�������������
    *     a. ���Դ��� ����BLAS��LAPACK
    *     b. ScaLAPACK
    *     c. PARDISOϡ����������
    *     d. ��ɢ����Ҷ�任��Discrete Fourier transforms��DFTs��
    *     e. ʸ����ѧ�⣨Vector Math Library��VML��
    *     f. ʸ��ͳ�ƿ⣨Vector Statistical Library��VSL�����������������
    * 
    *******************************************************************************************************************************/

    /******************************************************************************************************************************
    * LoadRunner -- HP��վ���أ�֧��Win��Linux�����ð�ֻ��10��
    *   9.5�ƽ�汾��License(δ����)��globa100 -- AEAMAUIKY-AFEKEKJJKEEA-BCJGI
    *                                 web10000 -- AEABEXFR-YTIEKEKJJMFKEKEKWBRAUNQJU-KBYGB
    *   
    *   Ԥ��ϵͳ��Ϊ�����ܵĸ��ز��Թ��ߣ�ͨ����ģ����ǧ���û�ʵʩ�������ؼ�ʵʱ���ܼ��ķ�ʽ��ȷ��
    * �Ͳ������⣬LoadRunner �ܹ���������ҵ�ܹ����в��ԡ���Ҫ��������Ҫ������ɣ�
    *   Controller -- ��֯����û��Ĳ��Է����������Թ����Զ���
    *   VuGen(Virtual User Generator) -- ���� Vuser �ű�����Ҫ���ߣ�����¼�ƺ����нű����ܼ��ش�����ϵͳ����
    *   Analysis -- ���������Թ����У���¼�����в����ĺ������ӵ����ݣ�������ͼ��
    *
    ******************************************************************************************************************************/

    /******************************************************************************************************************************
    * NSIS(Nullsoft Scriptable Install System) -- http://nsis.sf.net, ��Դ�� Windows ϵͳ����Ѱ�װ������������ͨ���ű�����
	*   ע�⣺
	*     1.����û�в������ݡ�
	*       ����������ͨ�� ȫ�ֱ��� �� ��ջ���� Pop��Push �� 20 ���Ĵ������� $0��$9��$R0��$R9 ����
	*     2.����ѹ���㷨(ZLib, BZip2, LZMA) -- LZMA ѹ�����б�����ͨ��ѹ���������õ�Ч��
	*     3.��ͨ��"XXXX"ʵ���Զ���Ի���ͽ���
	* 
	*   �ű�(.nsh/.nsi -- �� makensis ͬĿ¼���� nsisconf.nsh ͷ�ļ������Զ�����)
	*     �������Ľű��������ɻ�����
	*       HM NIS EDIT -- http://hmne.sourceforge.net/
	*       Venis IX -- http://www.spaceblue.com/products/venis/
	*     �﷨(�����ṹ���� ��װ�����������ԡ�ҳ�桢���Ρ����� )
	*       0.�����﷨
	*         ת���ַ� -- $ǰ׺���� $\n ��ʾ���У� $$ ��ʾ��Ԫ��"$", $\"��ʾ˫���ţ�����
	*         !define ������ "����ֵ"   -- Name �� OutFile ?
	*         !include ͷ�ļ��� -- ����ָ����ͷ�ļ�
	*         !macro ���� 
	*         var ������, ����ʱ $������
	*             ��ɫ -- ����HTML�е�RGB��ʾ���������þ���"#"
	*         ���õ�Ԥ����ϵͳ������
	*           $INSTDIR��$OUTDIR��$CMDLINE��$LANGUAGE
	*           $PROGRAMFILES��$COMMONFILES��$DESKTOP��$STARTMENU
	*           $MUSIC��$FONTS��$APPDATA
	*           $RESOURCES��$RESOURCES_LOCALIZED
	*       0.5��װ�������ԣ��� Name��InstallDir ��)
	*          ȷ����װ��������ܡ���ۺ�ϰ�ߣ�������ʾ���ı�����װ���͵������ȣ������������ʱֻ����
	*       1.ҳ�� -- �Ǿ�Ĭ��װ����Ҫ����ҳ�棬����ָ���û����а�װ��
	*         Page/UninstPage/PageEx ҳ���� -- ָ���ض��İ�װ/ж��ҳ��, ��ͨ���ص�����������֤,��û�лص������Ӧ��λ���� ""
	*         ���ý���(??) -- license��components��directory�� uninstConfirm
	*           Page <ҳ����> [pre_function] [show_function] [leave_function]
	*         ���ƽ���
	*           Page <ҳ����> [creator_function] [leave_function] [caption]
	*       2.Section "[un.]������"
	*            �������ݣ���Ӧĳ�ְ�װ/ж��ѡ��Ĵ����߼�
	*         SectionEnd
	*         ���������η����� RO 
	*         �����Σ�SubSection 
	*       3.�û��Զ��庯��(ʹ�� Call ָ������) �� �ص������� �� .onInit ) 
	*         a.�û��Զ��庯����
	*           Function <������>
	*             ������
	*           FunctionEnd
	*         b.NSIS Ԥ����Ļص���������װ��ж�أ�
	*             .onGUIInit
	*             .onInit
	*             .onInstFailed
	*             .onInstSuccess
	*             .onGUIEnd
	*             .onMouseOverSection
	*             .onRebootFailed
	*             .onSelChange
	*             .onUserAbort
	*             .onVerifyInstDir
	*             un.onGUIInit
	*             un.onInit
	*             un.onUninstFailed
	*             un.onUninstSuccess
	*             un.onGUIEnd
	*             un.onRebootFailed
	*             un.onUserAbort
	*       4.ָ�� -- �ɴ��������� /REBOOTOK
	*           ����ָ�Delete��Exec��ExecShell��ExecWait��Rename��RMDir
	*             File -- ָ���ļ�, /r �ݹ飬 /x �ų�ָ���ļ��� �� File /r *.exe
	*             
	*           ��֧�жϣ�IfErrors��IfFileExists��StrCmp ��
	*           ע�����ReadRegStr
	*           ���Ժ�����
	*              MessageBox MB_YESNO "��ʾ��Ϣ"
	*              DetailPrint
	*              Dumpstate
	*           
	*           SetOutPath -- ָ��Ŀ��λ��
	*       6.���(��չNSIS��װ�����DLL��ϵͳԤ��װ���� Plugins Ŀ¼�£��û����� !addplugindir ����Ŀ¼λ��)
	*         ʹ���﷨�� DllName::FunctionName "����1" ����2" "����3"
	&         �磨�����ļ����� NSISdl::download http://download.nullsoft.com/winamp/client/winamp291_lite.exe $R0
	*   ���߳���makensis/makensisw 
	*
    ******************************************************************************************************************************/
	
    /******************************************************************************************************************************
    * PC-Lint(FlexeLint) -- http://www.gimpel.com/ �� Ŀǰ��߰汾��9
    *   ���ص�ַ��http://download.csdn.net/source/2258242  8.x
    *   �� C/C++ Դ����о�̬�����Ĺ��ߣ���Ҫ���ָ�롢����Ƚ��з��������޷�����
    *   ����ʱ������߼�����֧�� VC��gcc �ȣ����Ա�һ���C/C++���������и�������������
    *   ����ƽ̨�� Windows��Dos��Linux�¿����� FlexeLint������������Բμ� gcc-readme.txt
    *
    * �ļ����ܣ�
    *   lint-nt.exe [ѡ��] [ѡ���ļ�(*.lnt)] �����ļ�(֧��ͨ���*.cpp)���ļ��б��ļ� 
    *     +ffn -- ����ļ�ȫ·������ȷ����IDE�����п���˫�����ڲ�ͬ���ļ����л�
    *     -i -- ָ��lint���ʱ��Ҫ������·��
    *     -u -- ���е�Ԫ��飬��������ģ�������澯
    *     -os(����ļ�·��)   
    *   �ļ��б��ļ������ɣ�dir d:\MyWork\*.cpp /s/b/a:-d > d:\pc-lint\filelist.lnt
    *
    *   LIN.bat -- ͨ�������ɵ��������ļ����ܰ����û�ָ��������(һ���� std.lnt��
    *     �����ж����ͨ�� _a �� _z �ĺ�׺����)����pclint�����Էŵ�ϵͳ��Path·����
    *   std.lnt -- ͨ�������ɵ������ļ������л���õ� .lnt �ļ���Ҫ��lntĿ¼������ͬһĿ¼��
    *   options.lnt -- �������δ�����Ϣ�������ļ������Դ򿪻�ر�ĳЩ���أ��Ӷ����������ĳЩ����
    *     (�� if(b = fun())�� �����޷��Ÿ��Ƶ�)
    *
    *   au-xxx.lnt -- C++�ش����߽����ѡ��
    *   co-xxx.lnt -- �������澯����ѡ��������ļ����� gnu,mscX0,
    *   env-xxx.lnt --  ֧�ֶ�Ӧ��IDE������������Ҫ�������ļ������Ұ����˶�ӦIDE���õ���ϸ˵��
    *   lib-xxx.lnt -- ���ֿ��ļ��澯����ѡ��������ļ�����stl��atl��mfc��
    *   xxx.h/xxx.c(pp) -- ����ʱ��Ҫ��ͷ�ļ���Դ�ļ������������ļ�����һЩ���������еĹؼ��ֺ�����
    *     �÷�����һ������c/c++��׼��PC-lintΪ֧����Щ��׼��ͨ��ʹ����Щͷ�ļ���Դ�ļ�������б�ʶ�滻
    *     �����ļ����Ӧ�Ĵ���
    *   xxx.lob -- lint��������ʱ���м��ļ�(Lint Object Module)
    * �������ɵ�һ�� std.lnt (������VC2008)
    *   co-msc90.lnt
    *   lib-atl.lnt lib-mfc.lnt lib-stl.lnt lib-w32.lnt
    *   options.lnt  -si4 -sp4
    *   -iE:\FJSDK\Windows\FTL
    *   -i"C:\Program Files\Microsoft Visual Studio 9.0\VC\include"
    *   -i"C:\Program Files\Microsoft Visual Studio 9.0\VC\atlmfc\include"
    *   -i"C:\Program Files\Microsoft SDKs\Windows\v6.0A\Include"
    *
    *  
    * ��VC�Ļ�����ʹ�ã�ͨ�������ⲿ����ķ�ʽʹ�ã�ִ�м���ͨ����ݼ� F8 �ڸ�������֮���л�
    *   ��鵱ǰ���ڱ༭�ĵ����ļ���
    *     1.����Ϊ: PC-INT File Check
    *     2.����Ϊ: D:\PCLint\LINT-NT.EXE
    *     3.����Ϊ: +v -i"D:\PCLint"  std.lnt env-vc9.lnt "$(ItemPath)"  -- �ƺ����Բ�Ҫ��env-vc9.lnt��?
    *     4.��ʼĿ¼Ϊ: "$(ItemDir)"
    *     5.ѡ�С�ʹ��������ڡ������Զ�ѡ�С��˳�ʱ�رա�
    *   ��鵱ǰ���ڱ༭�Ĺ��̣�������������������ص�.lnt -> ����.lnt
    *     1.����Ϊ��PC-LINT Project Creation
    *     2.����Ϊ: D:\PCLint\LINT-NT.EXE
    *     3.����Ϊ: +v -os($(TargetName).lnt) $(ProjectFileName)
    *     4.��ʼĿ¼Ϊ: $(ProjectDir)
    *     5.ѡ�С�ʹ��������ڡ������Զ�ѡ�С��˳�ʱ�رա�
    *     6.����Ϊ: PC-LINT Check Project��
    *     7.����Ϊ: D:\PCLint\LINT-NT.EXE
    *     8.����Ϊ: +v -i"D:\PCLint" std.lnt env-vc9.lnt $(TargetName).lnt
    *     9.��ʼĿ¼Ϊ��$(ProjectDir)
    *     10.ѡ�С�ʹ��������ڡ������Զ�ѡ�С��˳�ʱ�رա�
    *   ����һ����ν�� unit check (�ڴ����� Project.lnt ��)��������ʲô�ã�
    *     ������-i"D:\PCLint" std.lnt env-vc9.lnt --u $(TargetName).lnt $(ItemPath)
    *     ��ʼ��Ŀ¼��$(ProjectDir)
    *     
    * format ˵��(�� format=%f %l %t %n:%m )
    *   %f -- �ļ�
    *   %l -- �к�
    *
    * �澯����(���ָ澯���󱨣�������ʱ�������Σ���Ҫʹ�������ע�͸�ʽ��������)���������������εķ�ʽ���磺//lint !e813
    *   /��lint -save -e813 ��/    -- ע�⣺����� �� ʵ����Ӧ���� *
    *   �ᱨ�澯��Ĵ��룬�� -save �� -restore ���������󣬾Ͳ����ٱ�ָ���Ĵ���(�˴�Ϊ813)��
    *   /��lint -restore ��/
    *   "-" -- ��ֹ�����Ӧ�Ĵ�����Ϣ
    *   "+" -- ���������Ӧ�Ĵ�����Ϣ
    *   "#" -- ����ʹ��ͨ��� "?" �� "*"
    *   -e(�澯��1,[�澯��2]) -- ��ֹ��һ������ʽ�ĸ澯
    *   --e(�澯��1,�澯��2) -- �Ե�ǰ��������ʽ��ֹ����澯
    *   -e{�澯��} -- ����ʽ��������(����һ��������)
    *   //lint !e�澯�� -- ���н�ֹ���ָ���ĸ澯��һ�������β
    *   -efile(�澯��,�ļ��б�) -- ��ָֹ�����ļ��Ը澯
    *   -efunc(�澯��,�������б�) --
    *   -elibsym(�澯��,�������б�) --
    *   -emacro(�澯��,����) -- ��չ��ʱ��ֹ�澯
    *   -esym(�澯��,������) -- �ɶ�ָ��������(����������������) ���и澯����
    *   -etemplate(�澯��) -- ģ��չ��ʱ��ֹ�澯
    *   -s�����ֽ��� -- ���÷��ŵ��ֽ������� -sb8(�ֽ�Ϊ8����)�� -sc1(charΪ1�ֽ�)��-sl4(longΪ4�ֽ�)
    *   -function(�Ƚϻ�������,�º���) -- ����ģ�£�����"�º���"���ú�"�Ƚϻ�������"��ͬ�ıȽ����ý��м�飬��
    *     -function(strcpy,myStrCpy) ��char buf[4]; myStrCpy(buf,"abcde"); �ر�419 �ľ���(bufԽ��)
    *   +libdir(���������ļ�Ŀ¼) -- ��ָ��Ŀ¼�µ��ļ�����Ϊlib��֮������� -elib/-wlib �Ƚ�������
    *   -wlib(�ȼ�) -- �Կ��ļ����ø߼����ܹ�����(0~4��0����ӡ)
    *
    * �����Ĺ���
    *   ALOA(A Lint Output Analyzer ) -- �ɶԽ��ͳ�Ʋ�����
    *   lintProject -- ��Դ�Ĺ��ߣ����Խ� sln/vcproj �ȹ����ļ�ת���� �ļ��б��ļ�(lnt)
    *   
    ******************************************************************************************************************************/

    /******************************************************************************************************************************
    * Purify
    ******************************************************************************************************************************/

    /******************************************************************************************************************************
    * ReviewBoard -- Web Code Review Tool( http://www.reviewboard.org/screenshots )
    *   ���: http://code.google.com/p/reviewboardvsx/
    ******************************************************************************************************************************/

    /******************************************************************************************************************************
    * �汾���ƹ��� SVN��CVS����������ڶ������ļ�ֻ�������һ�汾��֮ͬ������SVNһ��������Ŀ¼��branches��tgs��trunk
    *   ������(Subversion/VisualSVN-Server ��)  -- http://subversion.tigris.org/
    *     �Դ������е� svn.exe ��Ϊ�ͻ���
    *   �ͻ���(TortoiseSVN) -- http://tortoisesvn.net/downloads
    *   VS ���(AnkhSvn)    -- http://ankhsvn.open.collab.net/
    * 
    * ȫ�������ļ��� ~/.subversion/config
    *   global-ignores -- ����ȫ�ֹ����б�(Ŀ¼����չ����)
    * 
    * ��������
    *   SVN_EDITOR <==
    *   VISUAL��EDITOR
    *
    * svnadmin.exe 
    *   create �յ�Ŀ¼�� -- �����汾�⣬�ȼ���TortoiseSVN�� �Ҽ�->TortoiseSVN->Create Repository here
    * svnserve.exe <== ���������ڶ˿�3690�ȴ�����
    *   --daemon : ���ػ����̷�ʽ���У��ֶ���ֹ֮ǰ�����˳�
    *   --root ����Ŀ¼��: ���ø�λ�������Ʒ������ķ���Ŀ¼
    *   ��������ʽ����svn��
    *     set CUR_PATH=%~dp0�� svnserve --daemon --root %CUR_PATH% 
    *   ��������(�������У�ע��ÿһ���Ⱥź��涼Ҫ��һ���ո񣬷���ᱨ��)��
    *     sc create SVN binpath= "E:\Subversion\bin\svnserve.exe --service --root E:\fishjam\Code_SVN" displayname= "SVN Service" depend= Tcpip start= auto
    *
    * �����ļ�(�汾��Ŀ¼��conf��Ŀ¼��)
    *   svnserve.conf
    *     password-db=passwd -- 
    *   passwd
    *     [users] -- ���ÿһ�ж���һ���û������� �û���=���� �ķ�ʽ
    *
    * SVN��������( svn.exe )
    *   add �����ļ���Ŀ¼ <== ����
    *   blame �ļ� <== �鿴�ļ����޶������ߵ���Ϣ��ÿ�е�����޸��κΰ汾������ͨ�� | sort �鿴�����޸�
    *   checkout https://xxxxx ����·�� --username �û���  <== �����
    *   cleanup <== ����������������������Ĵ�����������ô�����ģ��Ƿ�������
    *   copy SRC DST <== ��<-->���� �� ����֮�俽����ע����֧�ֿ�֮�䣩
    *   export SRC DST <== �ɴӿ⡢�����޸��е��������汾������Ϣ�Ľṹ
    *   import PATH URL <== ���ް汾���ļ��������絽��Ŀ�洢����
    *   info <== �鿴��·����Ϣ
    *   list <== �г����е�����(��ǰĿ¼�µ��ļ���Ŀ¼)
    *   log <== ������ʷ
    *   merge <== �Կ��е����°汾���ļ��ϲ�������������
    *   resolved �ļ� <== �ѽ����ͻ
    *   status(st) <== �鿴���ص��ļ��޸������ͨ������ svn st | grep ^[^?] �ķ�ʽ�鿴����
    *   switch <== ���¸�����URL
    *   proplist Ŀ¼ -R <== �г�ָ��Ŀ¼�����е�����(�� svn:executable)
    *   propedit ������(�� propedit) �汾·�� [ --editor-cmd notepad ]  
    *     <== ʹ�� notepad ��������(���������SVN_EDITOR ���������Ļ���������ָ���ı༭�����б༭���� )
    *   ��������( propset )
    *       svn:externals "[-r�汾] ������URL��ַ ������"  <== ʵ���ļ���Ŀ¼������
    *         ע�⣬URL�еĿո������%20���棬�������Ŀո������˫���š�"^/"������URL
    *       svn:executable <== ���ÿ�ִ������(����linux��so�ȣ�)
    *       svn:ignore  "*.obj *.exe *.bak" Ҫ���õ�Ŀ¼(��.) <== ����SVN���Ե��ļ�(����ͨ�� * ָ���������չ��)
    *    update <== ���´��룬���ܻᷢ����ͻ
    *    del --keep-local MyConfig.xml  <== �������ظ�����ÿ���в�ͬ�ĸ�������δ�����Ƿ����У�
    * 
    * ͨ��SVN������ӣ������ñ��ش���Ŀ¼--ʹ�ñ��ص� Local Repository ��Ϊ��������ת��������������
    *   1.svnadmin create ���زֿ�Ŀ¼ <== 
    *   2.svn co file:///���زֿ�Ŀ¼ . <== �������ֿ�Ŀ¼checkout����ʱĿ¼�£�֮����һ����������
    *   3.������Ҫ��Ŀ¼�ṹ��������Ҫ���ļ���Ȼ��ʹ�� svn add ����;
    *   4.$SVN_EDITOR=gedit svn pe svn:externals ������ <== ���� svn ����������
    *     �ڵ����� gedit �������ⲿ���ӵ�·�����ⲿ������ SVN��ַ(�� swanfalconapp https://....)
    *   5.svn commit -m "ע��" <== ���Ѿ����úõı���Ŀ¼�ṹ commit �� ���ص� Local Repository ��
    *   6.Eclipse ������ Resposity location����ʹ�� URL ָ�򱾵ص� Local Repository��Checkout��
    *     �Ϳ���ֱ����Eclipse�й���������
    *
    * ��ͻ���
    *   p(ostpone) <== �Ƴ٣��ϲ����벢���û��������ͻ�ĵط����� <=== �ȷָ[Windows�µ�Ĭ�Ͻ����ʽ]
    *   mc(�ҵİ汾)
    *   tc(���˵İ汾)
    *   df(diff-full)��ʾȫ������
    *   edit <== �༭������Ҫ���� SVN_EDITOR �Ȼ�������
    *   r(esolved) <== �ѽ��
    *   dc()
    *   mf <== ȫ���ҵ��ļ�
    *   tf <== ȫ�����ǵ��ļ�(��tc������)
    ******************************************************************************************************************************/
    
    /******************************************************************************************************************************
    * UltraEdit(�����滻ʱ���������ʽ)�������֣�P(erl)��U(nix)��(Ultra)E(dit)�� L(ex,ͨ��ͬU)
    *   ��string              L<== ֱ������string
    *   ��"a+b"��(����)��   U<== "a+b"
    *   ת���ַ���          U<== \              E<== ^
    *   �滻����ʽ(����)��  U<== (KEYWORD)      E<== ^(KEYWORD^)
    *   �滻��ǩ(1-9)��     U<== \n             E<== ^n
    *   �߼���(����A��B)��  U<== (A|B)          E<== ^{A^}^{B^} 
    *   ab��1�λ��Σ�     U<== (ab)+
    *   ���ף�              U<== ^              E<== %
    *   ��β(r$�ȼ���r/\n)��U<== $              E<== $
    *   ���������ַ���                          E<== *
    *   ǰ�ַ�0��1�Σ�      U<== ?
    *   ǰ�ַ�0���Σ�     U<== *              E<== ++
    *   ǰ�ַ�1���Σ�     U<== +              E<== +
    *   ǰ�ַ�N��M�Σ�      U<== \{N, M\}
    *   ���ִ���A����N��M)��  L<== A{N,M}   <==˵����Lex���﷨���� [a-z]{5,} ��ʾ���ȴ���5�� a-z ��ɵ��ַ�
    *   ��\n���κε��ַ���  U<== .              E<== ?
    *   ��ҳ����            U<== \f             E<== ^b
    *   0D0A(DOS)��         U<== \p             E<== ^p
    *   0D(MAC)��           U<== \r             E<== ^r
    *   ���з�OA(UNIX)��    U<== \n             E<== ^n
    *   �ո��Ʊ���          U<== \s
    *   �ǿ��ַ���          U<== \S
    *   �Ʊ�(TAB)��         U<== \t             E<== ^t
    *   ��ֱ�Ʊ���          U<== \v
    *   �»��߻��ַ���      U<== \w
    *   ���ַ���            U<== \W
    *   ��Χ(��Сд��ĸ)    U<== [a-z]          E<== [a-z]
    *   �ų�(��0-9)��       U<== [^0-9]         E<== [~0-9]
    *     ��ab��������ַ�   <== [^ab]
    *   ��ǰƥ��(β�����)��   L<== r/s ƥ��������s��r���� ABC/D ƥ�� ABCD ����ƥ�� ABCE�����ҵ���ģʽΪ ABC
    * ע�⣺
    *   *��+ ֻƥ��֮ǰ�����Ԫ�أ�������С���Ž����Ԫ������������ͨ����Ϳ���ƥ���������ŵ�������
    *   "^" ���ڷ����ŵ��ײ���ʾ"����"����˼���������ط�û���ر�����
    *   "|"���ڷ������б�ʾ�����ߡ����ڷ�������ֻ��ʾ"|" ����
    * ���ӣ�
    *   1.���� create poa ok �� create orb ok��Ȼ���򣨼�Ϊ ok poa create��
    *     Unix<== (create) (orb|poa) (ok)���滻�ַ����� \3 \2 \1  -- ע�⣺��BUG
    *     UltraEdit<== ^(create^) ^(^{orb^}^{poa^}^) ^(ok^)�� �滻�ַ�����^3 ^2 ^1
    ******************************************************************************************************************************/

    /******************************************************************************************************************************
    * ��Ԫ����(Unit Testing) --
    *   �쳣����--ȷ�ϴ���Ľṹ�ɿ��ԣ��ܴ������п��е������Լ�������ϣ��������Ԥ��֮����쳣��
    *   ���ܲ���--��֤������ģ���Ƿ��������͹����Ƿ�����������
    *   �ع����--��֤���д�����Ϊ�������Ŵ����Ľ�������,�ڴ������֮�����У��Բ�����Ĵ�����Ϊ�е�ƫ��
    *
    ******************************************************************************************************************************/

    /******************************************************************************************************************************
    * Visual Unit -- �����ĵ�Ԫ���Թ���
    *   
    ******************************************************************************************************************************/

    /******************************************************************************************************************************
    * Visual Studio
    *   �༭ UTF-8��ʽ���ĵ���Linuxͬ����������
    *     Options->Text Editor->General->Auto-detect UTF-8 encoding without signature
    * 
    * ���������� :a+ ��ѡ��"�������ʽ", �������� �ĵ���Ӳ������ַ���
    *
    * Visual Studio Team System(VSTS)
    *   ���ù��ߺ͹��̹������
    ******************************************************************************************************************************/

    /******************************************************************************************************************************
    * VC++Ĭ������£����԰汾�д����쳣�������汾�в����������� try {*NULL = 100;} catch(...){����ֻ�� Debug ����Ч} 
    * ͬ���쳣ģ��(/Gx �ı�������), ������첽�쳣ģ�ͣ��κδ���ָ�������쳣(/Eha)
    * C++�쳣ֻ���� throw ����׳����ṹ���쳣(���� __try ... __except ����) 
    * 
    * 
    * C++ �쳣�ͽṹ���쳣�ıȽ� -- P139
    * VC++ ��֧���쳣�淶(C4290) -- ����֧���� throw �������������׳����쳣����
    *
    * ���������ã�
    *   1.��֤���а汾(���������汾)�ж����е��Է���PDB�ļ���
    *     �Ż���ķ����汾����ʼ���ܹ�ƥ��Դ�����ִ���������׽��е������٣�
    *     �Զ�ջ�Ĵ����������Ż����޷����������ĵ��ö�ջ��Ϣ�����ڻ����ļ������ӱ�ʶPDB�ļ��ĵ�����
    *       (section)�ַ���������΢�����ļ��Ĵ�С��
    *       a..NET(C#/VB) �У�   Build -> Generate Debugging Infomation => True
    *       b.��������(C/C++)�У�C/C++ -> General -> Debug Information Format => Program Database(/Zi)
    *         Linker ->General -> Enable Incremental Linking => No(/INCREMENTAL:NO)�����������ӿ�����ΪNo��
    *         Linker -> Debugging -> Generate Debug Info => Yes(/DEBUG),���������Ϣ����ʹ���������Զ���
    *           �����к��������������Ƿ����ã���Ҫ /OPT:REF ɾ����
    *         Linker -> Debugging -> Generate Program Database File => $(OutDir)/$(ProjectName).pdb��
    *         Linker -> Debugging -> Strip Private Symbols => $(OutDir)/$(ProjectName)_Stripped.pdb��
    *         Linker -> Optimization -> References => Eliminate Unreferenced Data(/OPT:REF)��ֻ������Щ������ֱ�ӵ��õĺ���
    *         Linker -> Optimization -> Enable COMDAT Folding => Remove Redundant COMDATs(OPT:ICF)����Ҫ��ʱ��
    *           �ϲ���ͬ������COMDAT��¼��ʹ����������ĳ����ֵ�ĵط�ֻ��ʹ��Ψһ�ĳ������ݱ�����
    *   2.���йܴ����еı��뾯����Ϊ�������/WARASERROR+�������󲿷ֱ�������ı��뾯����Ϊ�������
    *
    *******************************************************************************************************************************/

    /******************************************************************************************************************************
    * Visual Assist -- VC����(���ƵĻ��� DPack / Resharper ��)
    * 1.AutoText �еĺ� �� ������������ϵͳ�ж��壩
    *   %FILE%              Full filename with path 
    *   %FILE_PATH%         Path of file
    *   %FILE_BASE%         Filename without path or extension
    *   %FILE_EXT%          Filename extension
    *   %DATE%              Year/month/day formatted as %04d/%02d/%02d
    *   %YEAR%              Year formatted as %d
    *   %MONTH%             Month formatted as %d
    *   %MONTH_02%          Month formatted as %02d
    *   %MONTHNAME%         Three-character abbreviation of month
    *   %DAY%               Day of month formatted as %d
    *   %DAY_02%            Day of month formatted as %02d
    *   %DAYNAME%           Three-character abbreviation of day
    *   %HOUR%              Hour formatted as %d
    *   %HOUR_02%           Hour formatted as %02d
    *   %MINUTE%            Minute formatted as %02d
    *   %SECOND%            Second formatted as %02d
    *   %%                  %
    *   %USER_NAME%         USERNAME
    *   %0                  Current selection
    *   %1 through %8       User input field
    ******************************************************************************************************************************/

}

#endif //FTL_IDE_TOOLS_H

//#ifndef USE_EXPORT
//#  include "ftlIDETools.hpp"
//#endif
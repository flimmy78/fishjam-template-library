package com.fishjam.springtester.framework;

import java.io.IOException;
import java.io.PrintWriter;

import javax.servlet.ServletConfig;
import javax.servlet.ServletContext;
import javax.servlet.ServletException;
//import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**************************************************************************************************************************************
 * TODO:
 *   1.要编译JaveEE(Ant?),需要将  %TOMCAT_HOME%/lib/jsp-api.jar;%TOMCAT_HOME%/lib/servlet-api.jar 加到 %CLASSPATH% 中
 *     
 *   2.多个客户端共享同一个Servlet实例？
 *   3.每个请求由一个Java线程处理?
 *   
 * 一个使用 Servlet 的典型过程(用户登录访问资源)
 *   1.用户访问任意资源
 *   2.在 Filter 中判断 session.getAttribute("name") 是否为null, forward 到 login.jsp
 *   3.login.jsp 中用户输入信息并 submit 到名为 /login 的 servlet
 *   4./login 中 获取 用户名 + 密码 进行身份判定，若成功则 session.setAttribute("name", username); 并 getRequestDispatcher(xxx).forward 
 **************************************************************************************************************************************/

/**************************************************************************************************************************************
 * Servlet3.0的Web模块化部署(jar 包) 
 *   META-INF\
 *     web-fragment.xml -- web模块部署描述符，语法类似 web.xml， 但可多指定 《name》和 《ordering》元素
 *   Web模块所用到的类、资源文件等
 *   使用时，将jar包复制到Web应用的 WEB-INF/lib 目录下，重启Web应用即可生效。
 *   
 *  MVC规范出现后，Servlet 仅仅作为控制器用，不再需要生成页面标签，也不再作为视图层角色使用。
 *  
 * 功能点
 *   Annotation标注 -- 3.0 中加入，通过Annotation修饰 Servlet/Filter/Listener 等类，支持模块化部署方式(不用修改web.xml)
 *     WebServlet -- 修饰部署Servlet类
 *     WebInitParam -- 与WebServlet或WebFilter一起使用，为其配置参数
 *     WebListener -- 修饰部署Listener类
 *     WebFilter -- 修饰部署Filter类
 *     MultipartConfig -- 修饰Servlet，指定该Servlet将会负责处理 multipart/form-data 类型的请求，主要用于文件上传
 *     ServletSecurity -- 与JAAS相关，指定该Servlet的安全与授权控制
 *     ? HttpConstraint -- 与 ServletSecurity 一起使用，指定安全与授权控制
 *     ? HttpMethodConstraint -- 与 ServletSecurity 一起使用，指定安全与授权控制
 *
 * 注意：
 *    1.Http有多种请求方式(GET/POST/PUT 等),HttpServlet中对应提供了 doGet/doPost/doPut 等服务方法分别对应。
 *      但如果想处理所有类型的请求，只需简单地实现 service 方法即可。
 *    2.Servlet中没有内置对象(不像JSP)，out 等对象需要由程序显示创建
 *   
 * Servlet(Server Applet) -- 在服务器上运行的小程序,主要用于交互式地浏览和修改数据，生成动态Web内容
 *   Servlet容器负责创建HttpServlet对象，并把Http请求直接封装到HttpServlet对象中
 *   1.编译好后的 .class 文件需要放在 WEB-INF/classes 路径下(注意包路径)
 *   2.将 Servlet 配置到Web应用中(详见后)，才能响应客户请求。
 *
 * javax.servlet.http.HttpServlet。
 *   init() -- 负责初始化Servlet对象，若重写，记住调用 super.init(config);
 *   service() -- 核心，负责响应客户的请求。如调用 super.service()方法可能激活其它方法以处理请求，如doGet()/doPost()/doPut/doDelete 等
 *    a.通过 Request 获得请求，如 表单数据(getParameter)或URL上的查询字符串
 *    b.通过 Response 生成响应数据, getWriter 输出文本信息，getOutputStream() 输出二进制信息
 *   destory() -- 负责释放占有的资源
 * 
 * HttpServletRequest -- 
 *   getParameter -- 
 *   getPart/getParts -- 获取文件上传域，支持文件上传，返回的 Part 类对应于一个文件上传域，可访问上传文件的大小、类型、输入流等。
 *     注意：用 @MultipartConfig 修饰 Servlet 
 *     传统的文件上传需要借助于 common-fileupload 等工具
 * HttpServletResponse
 *   setContentType("text/html;charset=GB2312");
 * 
 * 异步处理(3.0) -- 允许Servlet重新发起一条线程去调用耗时的业务方法，(通过 Annotation 指定 asyncSupported=true 启用)
 *   request.startAsync() -- 开启异步调用，创建并返回 AsyncContext 对象，此后响应不会被阻塞。线程执行完毕后，新的响应再次送往客户端  
 *   AsyncContext
 *     start(new Runnable() { ...mAsyncCtx.dispatch("/async.jsp"); } ) -- 异步方式启动后台线程，执行业务,完成业务后
 *       再通过 AsyncContext.dispatch 方法把请求 dispatch 到指定JSP页面。
 *       注意：1.被异步请求 dispatch 的目标页面(async.jsp)需要指定 session="false" 表明该页面不会重新创建 session。
 *                2.JSP页面中 request.getAsyncContext().complete();
 *     addListener -- 设置 AsyncListener 接口类型的异步监听器实例 ，可监听 开始、结束、出错、超时 事件
 *    
 **************************************************************************************************************************************/

/**************************************************************************************************************************************
 * 注意： 
 *   1.若要使用 Annotation 来配置， 则不能在 web.xml 的 <web-app> 中指定 metadata-complete="true" 
 *   2.不要在 web.xml 文件中配置该 Servlet，否则会冲突
 *   3.★ 为什么使用 MyElcipse 创建的 Servlet 访问时 404? ★
 * 
 * 将 Servlet 配置到Web应用中有两种方法： 
 *   1.修改 web.xml,
 *     a.<servlet> 元素指定名字(servlet-name)和对应的Java类(servlet-class) | jsp文件(jsp-file)
 *        注意：每一个 <servlet-name> 定义的 servlet 有自己的 WebApplicationContext 上下文对象
 *                 可通过 contextConfigLocation 参数指定 配置文件，该文件用于生成定义的Bean
 *     b.<servlet-mapping> 元素指定 Servlet(servlet-name) 映射到的 URL(url-pattern), 如 *.do 或 *.form 等
 *   2.(Servlet 3.0 以后)使用 @WebServlet 的 Annotation 修饰该Servlet类， 支持的属性： 
 *     asyncSupported[O] --是否支持异步操作模式 
 *     displayName[O] -- 指定显示名 
 *     initParams[O] -- 配置参数，然后可通过 ServletConfig.getInitParameter() 访问, 如 initParams={ @WebInitParam(name="driver", value="xxxx"), ... }
 *     loadOnStartup[O] --将该Servlet配置成Web应用一启动就创建，是一个整数，值越小越早实例化
 *     name[M] -- 指定名称，如 "login"
 *     urlPatterns/value[M] --指定该Servlet处理的URL, 如 {"/login"} 
 **************************************************************************************************************************************/

/**************************************************************************************************************************************
 * TODO: 
 * 1.使用 urlrewrite(www.tuckey.org/urlrewrite)可以过滤并重定向访问地址，实现网站的伪静态(即重定向*.html到对应的*.jsp，使得搜索引擎能收录)
 * 2.实现地图信息重定向：配置文件(urlrewrite.xml), <from>/pos/(\w*)</from> <to type="forward">/pos.jsp?mapid=$1</to>， 
 *   即将 /pos/* 重定向到 /pos.jsp 网页，且自动设置mapid参数为前面正则表达式匹配的字符串
 *  
 * Filter -- Servlet的加强版，主要用于对Request进行预处理，也可对Response进行后处理，是个典型的处理链。
 *   使用方式：Filter对Request进行预处理 => 将请求交给Servlet进行处理并生成响应 => Filter对Response进行后处理
 *   使用场景：
 *     用户授权Filter -- 检查用户请求，根据请求过滤用户非法请求；例：如检查请求的Session，若发现权限不够，直接重定向。
 *     日志Filter -- 详细记录某些特殊的用户请求
 *     解码Filter -- 对非标准编码的请求解码
 *     XSLT Filter -- 改变XML内容
 *   步骤：
 *     1.从 javax.servlet.Filter 接口继承，创建Filter处理类，实现 doFilter(request, response, FilterChain chain) 等方法
 *     2.web.xml文件中(filter + filter_mapping) 或通过 @WebFilter 的方式配置
 *       a.Filter 名，如 filterName="log" 
 *       b.Filter 拦截URL 的模式，如 urlPatterns= {"/*" }， 通常使用模式字符串拦截复合条件的多个请求URL
 *       可配置项：
 *         dispatcherType[O]--指定该Filter仅对哪种dispatcher模式的请求进行过滤，支持 ASYNC/ERROR/FORWARD/INCLUDE/REQUEST 的任意组合，默认同时过滤这五种模式的请求
 *         servletNames[O]--可指定多个Servlet的名称，用于指定该Filter仅对这几个Servlet执行过滤
 *         urlPatterns -- 指定该Filter所拦截的URL 
 *   常用类：
 *     FilterChain -- Filter是链式处理，在Filter.doFilter()方法中，调用 chain.doFilter 之前的处理是对用户请求进行预处理；
 *       之后的处理是对服务器响应进行后处理。  
 *     FilterConfig -- 
**************************************************************************************************************************************/
/**************************************************************************************************************************************
 * Listener -- 监听Web应用中的各种事件
 *   常用Web事件监听器接口：
 *     ServletContextListener -- 监听Web应用的启动和关闭
 *     ServletContextAttributeListener -- 监听application范围内属性的改变
 *     ServletRequestListener -- 监听用户请求
 *     ServletRequestAttributeListener -- 监听request范围内属性的改变
 *     HttpSessionListener -- 监听用户session的开始和结束
 *     HttpSessionAttributeListener -- 监听session范围内属性的改变 
 * 配置：向Web应用注册Listener类即可，不能配置参数
 *   a.@WebListener
 *   b.web.xml 中使用 <listner .../> 
 *   
**************************************************************************************************************************************/


// 使用 @WebServlet Annotation 进行部署
// 理论上说访问地址是 http://localhost:8888/JavaEEStudy/ServletStudy， 但测试是 404
//TODO: 调整 WEB-INF 的位置来尝试

//@WebServlet(loadOnStartup=1,  name="ServletStudy", urlPatterns={ "/ServletStudy" }  )
public class ServletTest extends HttpServlet {
	private static final long serialVersionUID = -9155460960679805493L;

	@Override
	public void init(ServletConfig config) throws ServletException {
		super.init(config);
		log("in ServletStudy init");
		System.out.println("in ServletStudy init");
		//throw new ServletException("i am here");
}
	@Override
	protected void service(HttpServletRequest request,
			HttpServletResponse response) throws ServletException, IOException {
		// super.service(request, response);
		System.out.println("in ServletStudy service");

		request.setCharacterEncoding("UTF-8");
		response.setContentType("text/html;charset=utf-8");

		// PrintStream out = new PrintStream(response.getOutputStream());
		// //获取二进制输出流 out
		PrintWriter out = response.getWriter(); // 获取文本输出 out

		out.println("<html><head><title>");
		out.println(ServletTest.class.getName());
		out.println("</title></head><body>");
		ServletContext application = getServletConfig().getServletContext();

		// 获取 web.xml 中使用 context-param 配置的参数
		String dbDriverString = application.getInitParameter("db_driver");
		String dbUrlString = application.getInitParameter("db_url");
		out.print("application init Params are:" + "db_driver="
				+ dbDriverString + ", db_url=" + dbUrlString);
		out.println("</body></html>");

	}

}

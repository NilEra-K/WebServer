# WebServer
***A web server project implemented with C.*** <p>
*基于 C语言开发的 Web服务器项目*
### ⭐ 项目开发流程
1. **需求分析 :** 做什么东西, 实现什么功能, 达到什么效果, 解决什么问题 —— 一般是由 **产品经理/项目经理** 来提供需求文档
2. **概要设计 :** 搭建项目主体框架, 如技术选型, 采用什么框架, 主要的技术路线, 大体结构 —— 一般是由**架构师/技术大牛** 来进行概要设计
3. **制定计划 :** 评估各功能模块的投入产出, 编写研发计划 —— 一般是由**开发人员/程序员**本身来提供
4. **详细设计 :** 设计每个模块的功能细节(文件、库、函数、数据类型)
5. **编写代码 :** 根据详细设计给出代码实现
6. **系统测试 :** 黑盒测试、白盒测试、灰盒测试 —— 一般是 **测试部门** 来进行测试

### 需求分析
- 基于 HTTP协议的 Web服务器, 提供静态页面的下载
- 服务器和浏览器进行通信, 传输层使用 TCP协议, 应用层使用 HTTP协议
- 浏览器向服务器发送 HTTP请求, 服务器接收请求后, 对请求内容进行解析, 明确浏览器目的, 服务器需要从请求中提取关键数据, 用于后续的操作, 判断对方所请求的文件是否存在, 并确定类型
- 构造响应, 并回传给浏览器 
- 服务器要支持并发访问

### 概要设计
- 主模块
    - main()
    - 主模块调用服务器模块
- 服务器模块
    - 接收连接请求, 开线程
    - 服务器模块调用线程模块、通信模块、信号模块
- 线程模块
    - 负责和客户端所通信
    - 线程模块调用 HTTP模块: 对 HTTP请求进行解析
    - 线程模块调用 资源模块: 对文件进行判断
    - 线程模块调用 HTTP模块: 构造响应
    - 线程模块调用 通信模块: 发送响应
- HTTP 模块
    - 对 HTTP请求进行解析
    - 对 HTTP响应进行构造
- 资源模块
    - 判断文件是否存在
    - 判断文件类型
- 通信模块
    - 创建套接字
    - 组织地址结构
    - 绑定
    - 监听
    - 接收连接请求
    - 接收 HTTP 请求
    - 发送 HTTP 响应
- 信号模块
    - 忽略大部分信号 : 提高服务器的生存能力

### 详细设计
| 模块名 | 包含文件 |
| :-: | :-: |
| *HTTP 模块* | _`http.h`_ **&** _`http.c`_ |
| *通信模块* | _`socket.h`_ **&** _`socket.c`_|
| *资源模块* | _`resource.h`_ **&** _`mime.h`_ **&** _`resource.c`_|
| *信号模块*| _`signals.h`_ **&** _`signals.c`_ |
| *线程模块* | _`client.h`_ **&** _`client.c`_ |
| *服务器模块* | _`server.h`_ **&** _`server.c`_ |
| *主模块* | _`main.c`_ |

### 编写代码

### 系统测试




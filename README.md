# Message-Queues

## [项目文档](https://ye-yu-mo.github.io/Message-Queues/)

## 项目介绍

在分布式系统中，跨主机之间的消费者模型是一个非常普遍的需求，通常会把阻塞队列封装成一个独立的服务器程序，并且添加上更多的功能，也就是消息队列（Message Queue，MQ）

## 开发环境

* Ubuntu 22.04(WSL)
* VS Code/Vim
* g++/gdb
* Makefile

## 技术选型

* 开发主语言: C++
* 序列化框架: [Protobuf](https://github.com/protocolbuffers/protobuf) 二进制序列化框架
* 网络通信: [muduo网络库](https://github.com/chenshuo/muduo) + 自定义应用层协议 
* 数据库: [SQLite3](https://github.com/sqlite/sqlite)
* 单元测试框架: [Gtest](https://github.com/google/googletest)
* 日志系统: [Xulog](https://github.com/Ye-Yu-Mo/LogSystem)

## 需求分析

### 核心概念

* 生产者-Producer
* 消费者-Consumer
* 中间人-Broker
* 发布-Publish
* 订阅-Subscribe

生产者与消费者模型

![null.png](https://s2.loli.net/2024/10/03/npBkOAHi4WT2tmq.png)

这里的消息队列服务器是中间人服务器, 就是作为整个系统最核心的部分, 负责消息的存储和转发

在AMQP中, 存在下面的概念

> AMQP(Advanced Message Queuing Protocol-高级消息队列协议)是一个提供统一消息服务的应用层标准高级消息队列协议, 是为面向消息的中间件设计, 使得遵从该规范的客户端应用和消息中间件服务器的全功能互操作称为可能

* 虚拟机-VirtualHost: 类似于MySQL中的数据库或者Docker搭建的虚拟容器, 是一个逻辑上的集合, 一个消息队列服务器中可以有多个虚拟机

* 交换机-Exchange: 消息是先被生产者发布给中间人的交换机上的, 交换机根据不同的规则, 将消息转发给相应的队列

* 队列-Queue: 是真正用来存储信息的部分, 起到一定的分类作用, 让消费者决定订阅哪一个队列, 再将相应的信息推送给消费者

* 绑定-Binding: Exchange和Queue之间的关联关系, 他们之间是多对多的关系, 使用一个关联表将这两个概念连接起来

* 消息-Message: 传递的内容

中间人服务器架构示意图如下

![null _1_.png](https://s2.loli.net/2024/10/03/SmqsxOVf4rz5wZE.png)

消息流程图如下

![null _2_.png](https://s2.loli.net/2024/10/03/PQIBjTeS2dora5m.png)

### 核心API

对于BrokerServer来说, 核心API如下

* 创建交换机-exchangeDeclare
* 销毁交换机-exchangeDelete
* 创建队列-queueDeclare
* 销毁队列-queueDelete
* 创建绑定-queueBind
* 解除绑定-queueUnbind
* 发布消息-basicPublish
* 订阅消息-basicConsume
* 确认消息-basicAck
* 取消订阅-basicCancel

ProducerClient和ConsumerClient通过网络的方式远程调用这些API实现消费者生产者模型

### 交换机类型

* Direct-直接交换: 指定被该交换机绑定的队列名(完全匹配)
* Fanout-广播交换: 复制发布到该交换机的所有队列
* Topic-主题交换: 队列在创建时指定了一个bindingKey, 当交换机收到一个消息时, 该消息内部存在一个字符串routingKey, 当routingKey与bindKey满足一定条件时, 把消息存到该队列中

> 匹配规则<br> <br> 
binding_key: `news.music.#` 约定字符: 数字`0~9`\\字母`a~z A~Z`\\ `_` \\ `.` \\ `#` \\ `*`<br> **注意**  <br> 1. `*` 通配符可以代替一个任意单词 例如: `news.*.football`是合法的<br> 2. `#` 通配符可以代替任意多个(0个或任意个)单词 例如: `news.sport.#`是合法的<br> 3. 在两个点直接不允许出现通配符和其他字符 例如: `news.*a.football`是不合法的<br> 4. `#`通配符不允许连续使用通配符 没有意义 例如`news.#.*.football #.# *.# #.*`是不合法的<br> <br> routing_key: news.music.pop 约定字符: 数字`0~9`\\字母`a~z A~Z`\\ `_` \\ `.`

### 网络通信

作为服务器客户端架构, 通过网络通信让客户端能够操作服务器

* 创建Connection
* 关闭Connection
* 创建Channel
* 关闭Channel
* 创建队列
* 销毁队列
* 创建交换机
* 销毁交换机
* 创建绑定
* 解除绑定
* 发布信息
* 订阅消息
* 确认消息
* 取消订阅

### 消息应答

被消费的信息需要进行应答, 有两种方式

* 自动应答: 消费者只要消费了消息, 就认为应答完毕, BrokerServer直接删除消息
* 手动应答: 消费者调用应答接口, 当BrokerServer收到应答请求时, 才删除消息

手动应答是为了确保消息确实被处理成功, 在可靠性要求高的情况下可以使用

## 模块划分

### 服务端模块

#### 持久化数据管理中心

在数据管理模块中主要管理交换机, 队列, 队列绑定, 消息等部分的数据

1. 交换机数据管理
    * 管理信息: 
        * 名称(唯一标识)
        * 类型(三种类型)
        * 持久化标志
        * 自动删除标志(TODO:当所有客户端退出时, 自动删除交换机)
        * 其他参数(当前未使用, 保留扩展)
    * 管理操作:
        * 创建交换机(有则OK, 没有则创建)
        * 删除交换机(需要删除绑定信息)
        * 获取指定名称的交换机
        * 获取当前交换机数量(测试用)
2. 队列数据管理
    * 管理信息:
        * 名称(唯一标识)
        * 持久化存储标志
        * 独占标志(只允许当前客户端订阅)
        * 自动删除标志(TODO:所有客户端退出后, 自动删除队列)
        * 其他参数(保留扩展)
    * 管理操作:
        * 创建队列
        * 删除队列
        * 获取指定队列信息
        * 获取队列数量
        * 获取所有队列名称(当系统重启, 需要重新加载历史消息数据, 以队列为单位存储到文件中)
3. 队列绑定数据管理
    * 管理信息:
        * 交换机名称
        * 队列名称
        * binding_key(主题交换和直接交换的匹配规则)
    * 管理操作:
        * 添加绑定
        * 解除绑定
        * 获取交换机所有绑定信息(删除交换机时, 删除绑定信息; 交换机收到消息时, 需要查找所有绑定队列)
        * 获取队列所有绑定信息(删除队列时, 删除绑定信息)
        * 获取绑定信息数量(测试用)

![null _4_.png](https://s2.loli.net/2024/10/06/758NLYto1bzyiRQ.png)

4. 消息数据管理

分别实现增删查的管理, 并实现持久化存储, 以内存存储为主, 确保查找信息处理信息的速度, 以硬盘存储为辅, 确保服务器重启之后, 以前的信息仍然存在

管理信息: 
* 消息属性
    * ID(唯一标识)
    * 持久化标识(受队列持久化的影响)
    * routing_key(与binding_key进行比对分配队列)
* 消息主体(内容)
* 存储偏移量(服务端): 以队列为单位存储在文件中, 是当前消息相对于文件起始位置的偏移量
* 消息长度(服务端): 从偏移量位置取出指定长度的消息(避免粘包)
* 有效标志(服务端): 标识当前消息是否被删除(回收时(阈值为50%, 总数据量在200以上时触发)统一整理文件存储; 重启时只加载有效消息)
* 消息管理
    * 管理方式: 以队列为单元进行管理
    * 管理数据
        * 消息链表(所有待推送消息)
        * 待确认消息hash(等待确认回复, 收到确认时, 才会真正删除消息)
        * 持久化消息hash(持久化存储中存在垃圾回收操作, 需要同步内存中硬盘数据的消息位置)
        * 持久化有效消息数量
        * 持久化总消息数量
    * 管理操作
        * 向队列新增消息
        * 获取队首消息, 获取消息后, 将消息在待推送链表中删除, 加入到待确认消息中
        * 确认消息, 从待确认消息中移除, 删除相应的持久化数据
        * 恢复历史消息, 构造函数中, 重启进行
        * 垃圾回收(消息持久化子模块完成)
        * 删除队列相关消息文件
* 队列消息管理
    * 初始化队列消息结构
    * 移除队列消息结构
    * 向队列新增消息
    * 恢复队列历史消息

#### 虚拟机管理模块

交换机/队列/绑定/消息都是虚拟机单元的一部分, 因此虚拟机是对上述模块的整合

管理信息:
* 交换机数据管理句柄
* 队列数据管理句柄
* 绑定信息管理句柄
* 消息数据管理句柄

管理操作:
* 声明\\删除交换机(需要删除绑定信息)
* 声明\\删除队列(需要删除绑定信息和消息数据)
* 绑定\\解绑队列
* 获取指定队列的消息
* 确认消息
* 获取交换机的所有绑定信息

#### 交换路由模块

消息发布是将消息传达到交换机中, 交换机再决定分发到队列中去

分为直接交换, 广播交换, 主题交换

交换路由模块是专门负责匹配过程

管理操作:
* 获取匹配结果
* 判断格式是否符合规定

#### 消费者管理模块

![null.png](https://s2.loli.net/2024/10/09/lgWNoAP95ZnyfS4.png)

消费者指订阅了消息队列的客户端, 当队列中有消息会自动推送给客户端

订阅消息指的是订阅一整个队列的消息

消费者信息:
* 消费者标识-tag
* 订阅队列名称
* 自动确认标志
* 消费处理回调函数指针(队列有消息后, 通过对应函数处理)

消费者管理:
* 以队列为单位进行管理
* 消费者链表(当前队列的所有消费者信息), RR轮转, 每次取出下一个消费者进行推送, 一条消息只需要被一个客户端处理
* 操作:
    * 新增消费者
    * 获取消费者
    * 删除消费者
    * 获取队列消费者数量
    * 是否为空

管理操作:
* 初始化队列消费者结构
* 删除队列消费者结构
* 向指定队列添加消费者
* 获取指定队列消费者
* 删除指定队列消费者
    
#### 信道管理模块

同一个连接(Connection)中可以有多个通信通道(Channel)

某个客户端关闭通信实质上是关闭信道,并非关闭连接

当信道关闭时, 服务端会将客户端的订阅取消

每一个信道在用户看来是独立的

管理信息:
* 信道ID
* 信道关联的虚拟机句柄
* 信道关联的消费者句柄
* 工作线程池句柄(信道发布消息到指定队列, 从指定队列获取消费者, 对信息进行消费)

管理操作:
* 声明\\删除交换机
* 声明\\删除队列
* 绑定\\解绑队列交换机
* 发布\\确认\\订阅\\取消订阅消息队列

外部操作:
* 创建\\关闭信道
* 获取指定信道的句柄

#### 连接管理模块

网络通信连接的管理

连接关闭时, 需要将信道全部关闭

muduo实现底层通信, 需要加上信道进行二次封装

管理数据:
* muduo库的通信连接
* 当前连接关联的信道管理句柄

连接操作:
* 创建\\关闭信道

管理操作:
* 新增\\关闭连接
* 获取指定连接的句柄

#### BrokerServer模块

是以上所有模块的整合, 形成一个服务器, 向外提供服务

管理信息:
* 虚拟机管理模块句柄
* 消费者管理模块句柄
* 连接管理模块句柄
* 工作线程池句柄
* muduo库通信所需元素

### 客户端模块

#### 消费者管理

订阅客户端订阅一个队列时, 就是创建了一个消费者

* 消费者标识
* 订阅的队列名称
* 自动确认标志
* 消息回调处理函数指针

#### 信道管理模块

与服务端信道一一对应, 服务端信道提供的服务器, 客户端都有, 客户端再为用户提供服务

向服务端的请求都是通过信道完成的

与服务端类似

* 信道ID
* 消费者管理句柄
* 线程池句柄
* 信道关联连接

* 创建\\关闭信道

#### 连接管理模块

客户端连接的管理本质上是对客户端TcpClient的二次封装和管理

客户端对用户是透明的, 用户是通过连接创建信道, 通过信道完成相应的服务

管理操作:
* 连接服务器
* 创建信道
* 关闭信道
* 关闭连接

管理信息:
* 管理线程池句柄
* 连接关联的信道管理句柄

#### 异步工作池模块

TcpClient需要一个EventLoopThread模块进行IO事件监控

收到推送消息后, 需要线程池对推送来的消息进行处理

#### 客户端模块

封装实现订阅客户端/发布客户端

## 模块关系图

![null _3_.png](https://s2.loli.net/2024/10/03/bgqxMvCPpHr9ksn.png)

## TODO

* 使用工厂模式创建对象
* ~~虚拟机管理~~ DONE
* 用户管理
* 服务端对发布用户的确认
* 客户端主动拉取消息功能
* 消息管理的方式
* 管理接口+页面
* 服务端交换机检测到当前所有客户端退出, 自动删除交换机\\队列
* 独占功能: 只有自己发布的消息才能消费 

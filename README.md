# Message-Queues

## 项目介绍

在分布式系统中，跨主机之间的消费者模型是一个非常普遍的需求，通常会把阻塞队列封装成一个独立的服务器程序，并且添加上更多的功能，也就是消息队列（Message Queue，MQ）

## 开发环境

* Ubuntu 22.04(WSL)
* VS Code/Vim
* g++/gdb
* Makefile

## 技术选型

* 开发主语言: C++
* 序列化框架: Protobuf 二进制序列化框架
* 网络通信: muduo库 + 自定义应用层协议 
* 数据库: SQLite3
* 单元测试框架: Gtest

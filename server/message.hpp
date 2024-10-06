/**
 * @file message.hpp
 * @brief 消息队列文件存储的实现
 * 
 * 该文件定义了 XuMQ 命名空间中的 MessageMapper 类，用于处理消息队列的文件管理。
 * 它提供了消息的插入、删除、垃圾回收等功能，并管理与消息队列相关的数据文件。
 * 
 * 主要功能包括：
 * - 创建、移除消息文件
 * - 插入、删除消息
 * - 对无效消息进行垃圾回收
 * 
 * 使用文件存储消息队列数据，通过指定的目录和文件名进行管理。
 */


#pragma once
#include "../common/logger.hpp"
#include "../common/helper.hpp"
#include "../common/msg.pb.h"
#include <iostream>
#include <unordered_map>
#include <mutex>
#include <memory>
#include <list>

namespace XuMQ
{
    const char *DATAFILE_SUBFIX = ".mqd";              ///< 数据文件后缀名
    const char *TMPFILE_SUBFIX = ".mqd.tmp";           ///< 临时文件后缀名
    const char *MSG_VALID = "1";                       ///< 消息有效标志
    const char *MSG_INVALID = "0";                     ///< 消息无效标志
    using MessagePtr = std::shared_ptr<XuMQ::Message>; ///< proto生成的Message类型指针
    /// @class MessageMapper
    /// @brief 处理消息队列的文件存储和管理类
    class MessageMapper
    {
    public:
        /// @brief 构造函数 创建必要的目录和数据文件
        /// @param basedir 基础目录
        /// @param qname 队列名称
        MessageMapper(std::string &basedir, const std::string &qname)
            : _qname(qname)
        {
            if (basedir.back() != '/' && basedir.back() != '\\')
                basedir.push_back('/');
            _datafile = basedir + qname + DATAFILE_SUBFIX;
            _tmpfile = basedir + qname + TMPFILE_SUBFIX;
            int ret = FileHelper::createDirectory(basedir);
            if (ret == false)
            {
                fatal(logger, "创建文件夹失败!");
                abort();
            }
            createMsgFile();
        }
        /// @brief 创建消息文件
        /// @return 成功返回true 失败返回false
        bool createMsgFile()
        {
            bool ret = FileHelper::createFile(_datafile);
            if (ret == false)
            {
                error(logger, " %s :创建队列数据文件失败!", _datafile.c_str());
                return false;
            }
            return true;
        }
        /// @brief 移除消息文件 包括移除数据文件和临时文件
        void removeMsgFIle()
        {
            FileHelper::removeFile(_datafile);
            FileHelper::removeFile(_tmpfile);
        }
        /// @brief 插入消息 将消息添加到数据文件中
        /// @param msg 消息指针
        /// @return 插入成功返回true 失败返回false
        bool insert(const MessagePtr &msg)
        {
            return insert(_datafile, msg);
        }
        /// @brief 移除消息 将消息中的有效标记置为false 更新到数据文件中
        /// @param msg 消息指针
        /// @return 移除成功返回true 失败返回false
        bool remove(MessagePtr &msg)
        {
            // 将msg中的有效标志为设置为'0'(false)
            msg->mutable_payload()->set_valid(MSG_INVALID);
            // 对msg进行序列化
            std::string body = msg->payload().SerializeAsString();
            if (body.size() != msg->length())
            {
                error(logger, "不能修改文件中的数据信息, 新生成的数据与原数据长度不一致!");
                return false;
            }
            // 将序列化的消息 写入到数据中的指定位置(覆盖原有的数据)
            FileHelper helper(_datafile);
            bool ret = helper.write(body.c_str(), msg->offset(), body.size());
            if (ret = false)
            {
                error(logger, " %s :队列数据文件写入失败!", _datafile.c_str());
                return false;
            }
            return true;
        }
        /// @brief 垃圾回收 加载所有有效消息 存储到临时文件后更新数据文件
        /// @return 有效消息列表
        std::list<MessagePtr> garbageCollection()
        {
            std::list<MessagePtr> result;
            // 加载文件中所有的有效数据 存储格式 4字节长度|数据|4字节长度|数据...
            bool ret = load(result);
            if (ret == false)
            {
                error(logger, "加载有效数据失败!");
                return result;
            }
            // 有效数据进行序列化存储到临时文件中
            for (auto &msg : result)
            {
                ret = insert(_tmpfile, msg);
                if (ret == false)
                {
                    error(logger, " %s :临时文件写入消息数据失败!", _tmpfile);
                    return result;
                }
            }
            // 删除原文件
            ret = FileHelper::removeFile(_datafile);
            if (ret == false)
            {
                error(logger, " %s :删除原文件失败!", _datafile);
                return result;
            }

            // 修改临时文件名为原文件名称
            ret = FileHelper(_tmpfile).rename(_datafile);
            if (ret == false)
            {
                error(logger, " %s :修改临时文件名称失败!", _tmpfile);
                return result;
            }
            // 返回新的有效数据
            return result;
        }

    private:
        /// @brief 插入消息到指定文件 负责数据文件和临时文件的写入工作
        /// @param filename 文件名
        /// @param msg 文件指针
        /// @return 成功返回true 失败返回false
        bool insert(const std::string &filename, const MessagePtr &msg)
        {
            // 新增数据添加在文件末尾
            // 消息序列化
            std::string body = msg->payload().SerializeAsString();
            // 获取文件长度
            FileHelper helper(filename);
            size_t fsize = helper.size();
            // 写入指定位置
            bool ret = helper.write(body.c_str(), fsize, body.size());
            if (ret = false)
            {
                error(logger, " %s :队列数据文件写入失败!", filename.c_str());
                return false;
            }
            // 更新msg中的存储信息
            msg->set_offset(fsize);
            msg->set_length(body.size());
            return true;
        }
        /// @brief 加载有效消息 从数据文件中读取所有消息并存为有效的消息对象
        /// @param result 存储有效消息的列表
        /// @return 成功返回true 失败返回false
        bool load(std::list<MessagePtr> &result)
        {
            FileHelper helper(_datafile);
            size_t offset = 0, msg_size;
            size_t fsize = helper.size();
            bool ret;
            while (offset < fsize)
            {
                ret = helper.read((char *)&msg_size, offset, 4);
                if (ret == false)
                {
                    error(logger, " %s :读取消息长度失败!", _datafile);
                    return false;
                }
                offset += 4;
                std::string msg_body(msg_size, '\0');
                ret = helper.read(&msg_body[0], offset, msg_size);
                if (ret == false)
                {
                    error(logger, " %s :读取消息数据失败!", _datafile);
                    return false;
                }
                offset += msg_size;
                MessagePtr msgp = std::make_shared<Message>();
                msgp->ParseFromString(msg_body);
                if (msgp->payload().valid() == MSG_INVALID) // 无效消息则处理下一个
                    continue;
                result.push_back(msgp); // 有效消息保存
            }
            return true;
        }

    private:
        std::string _qname;    ///< 队列名称
        std::string _datafile; ///< 数据文件
        std::string _tmpfile;  ///< 临时文件
    };
}
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

            if (FileHelper(basedir).exists() == false)
            {
                int ret = FileHelper::createDirectory(basedir);
                if (ret == false)
                {
                    fatal(logger, "创建文件夹失败!");
                    abort();
                }
            }
            createMsgFile();
        }
        /// @brief 创建消息文件
        /// @return 成功返回true 失败返回false
        bool createMsgFile()
        {
            if (FileHelper(_datafile).exists() == false)
            {
                bool ret = FileHelper::createFile(_datafile);
                if (ret == false)
                {
                    error(logger, " %s :创建队列数据文件失败!", _datafile.c_str());
                    return false;
                }
            }
            return true;
        }
        /// @brief 移除消息文件 包括移除数据文件和临时文件
        void removeMsgFile()
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
            if (ret == false)
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
            FileHelper::createFile(_tmpfile);
            for (auto &msg : result)
            {
                ret = insert(_tmpfile, msg);
                if (ret == false)
                {
                    error(logger, " %s :临时文件写入消息数据失败!", _tmpfile);
                    return result;
                }
            }
            info(logger, "垃圾回收有效消息数量为: %d", result.size());
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
                error(logger, " %s :修改临时文件名称失败!", _tmpfile.c_str());
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
            // 先写入4字节数据长度
            size_t message_size = body.size();
            bool ret = helper.write((char *)&message_size, fsize, sizeof(size_t));
            if (ret == false)
            {
                error(logger, " %s :队列数据文件写入长度失败!", filename.c_str());
                return false;
            }

            // 写入数据到指定位置
            ret = helper.write(body.c_str(), fsize + sizeof(size_t), body.size());
            if (ret == false)
            {
                error(logger, " %s :队列数据文件写入内容失败!", filename.c_str());
                return false;
            }
            // 更新msg中的存储信息
            msg->set_offset(fsize + sizeof(size_t));
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
                ret = helper.read((char *)&msg_size, offset, sizeof(size_t));
                if (ret == false)
                {
                    error(logger, " %s :读取消息长度失败!", _datafile);
                    return false;
                }
                offset += sizeof(size_t);
                std::string msg_body(msg_size, '\0');
                ret = helper.read(&msg_body[0], offset, msg_size);
                if (ret == false)
                {
                    error(logger, " %s :读取消息数据失败!", _datafile);
                    return false;
                }
                offset += msg_size;
                MessagePtr msgp = std::make_shared<Message>();
                msgp->mutable_payload()->ParseFromString(msg_body);
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

    /// @class QueueMessage
    /// @brief 推送消息队列管理
    class QueueMessage
    {
    public:
        using ptr = std::shared_ptr<QueueMessage>;
        /// @brief 推送消息队列构造函数 恢复历史消息
        /// @param basedir 基础目录
        /// @param qname 队列名称
        QueueMessage(std::string &basedir, const std::string &qname)
            : _mapper(basedir, qname), _qname(qname), _valid_count(0), _total_count(0)
        {
        }
        /// @brief 恢复历史消息
        void recovery()
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _msgs = _mapper.garbageCollection();
            for (auto &msg : _msgs)
                _durable_msgs.insert(std::make_pair(msg->payload().properties().id(), msg));
            _valid_count = _total_count = _msgs.size();
        }
        /// @brief 插入推送消息队列
        /// @param bp 消息属性
        /// @param body 消息内容主体
        /// @param delivery_mode 持久化标志
        /// @return 成功返回true 失败返回false
        bool insert(const BasicProperties *bp, const std::string &body, DeliveryMode delivery_mode)
        {
            // 构造消息对象
            MessagePtr msg = std::make_shared<Message>();
            msg->mutable_payload()->set_body(body);
            if (bp != nullptr)
            {
                msg->mutable_payload()->mutable_properties()->set_id(bp->id());
                msg->mutable_payload()->mutable_properties()->set_delivery_mode(bp->delivery_mode());
                msg->mutable_payload()->mutable_properties()->set_routing_key(bp->routing_key());
            }
            else
            {
                msg->mutable_payload()->mutable_properties()->set_id(UUIDHelper::uuid());
                msg->mutable_payload()->mutable_properties()->set_delivery_mode(delivery_mode);
                msg->mutable_payload()->mutable_properties()->set_routing_key("");
            }
            std::unique_lock<std::mutex> lock(_mutex);
            // 判断消息是否需要持久化
            if (msg->payload().properties().delivery_mode() == DeliveryMode::DURABLE)
            {
                msg->mutable_payload()->set_valid(MSG_VALID); // 持久化存储中表示数据有效
                // 持久化存储
                bool ret = _mapper.insert(msg);
                if (ret == false)
                {
                    error(logger, " %s :持久化存储消息失败!", body.c_str());
                    return false;
                }
                _valid_count++;
                _total_count++;
                _durable_msgs.insert(std::make_pair(msg->payload().properties().id(), msg));
            }
            // 内存管理
            _msgs.push_back(msg);
            return true;
        }
        /// @brief 获取队头消息
        /// @return 消息指针
        MessagePtr front()
        {
            if (_msgs.size() == 0)
                return MessagePtr();
            std::unique_lock<std::mutex> lock(_mutex);
            // 获取队头消息 从msgs取出数据
            MessagePtr msg = _msgs.front();
            _msgs.pop_front();
            // 将消息对象插入待确认映射表 等到收到确认ack后删除
            _waitack_msgs.insert(std::make_pair(msg->payload().properties().id(), msg));
            return msg;
        }
        /// @brief 移除接收到确认ack的消息
        /// @param msg_id 消息id
        /// @return 成功返回true 失败返回false
        bool remove(const std::string &msg_id)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            // 从待确认映射表中查找消息
            auto it = _waitack_msgs.find(msg_id);
            if (it == _waitack_msgs.end())
            {
                warn(logger, "没有找到要删除的消息! 消息id: %s", msg_id.c_str());
                return true;
            }
            // 查看持久化模式
            if (it->second->payload().properties().delivery_mode() == DeliveryMode::DURABLE)
            {
                // 删除持久化信息
                _mapper.remove(it->second);
                _durable_msgs.erase(msg_id);
                _valid_count--;
                garbageCollection();
            }
            // 删除内存中的信息
            _waitack_msgs.erase(msg_id);
            return true;
        }
        /// @brief 获取可获取消息数量
        /// @return 可获取消息数量
        size_t availableCount()
        {
            std::unique_lock<std::mutex> lock(_mutex);
            return _msgs.size();
        }
        /// @brief 获取总消息数量
        /// @return 总消息数量
        size_t totalCount()
        {
            std::unique_lock<std::mutex> lock(_mutex);
            return _total_count;
        }
        /// @brief 获取待确认消息数量
        /// @return 待确认消息数量
        size_t waitAckCount()
        {
            std::unique_lock<std::mutex> lock(_mutex);
            return _waitack_msgs.size();
        }
        /// @brief 获取持久化消息数量
        /// @return 持久化消息数量
        size_t durableCount()
        {
            std::unique_lock<std::mutex> lock(_mutex);
            return _durable_msgs.size();
        }
        /// @brief 清空数据
        void clear()
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _mapper.removeMsgFile();
            _msgs.clear();
            _durable_msgs.clear();
            _waitack_msgs.clear();
            _valid_count = 0;
            _total_count = 0;
        }

    private:
        /// @brief 垃圾回收条件检测
        /// @return 消息总量大于2000 且 有效消息占比小于50% 返回true 否则返回false
        bool garbageCollectionCheck()
        {
            if (_total_count > 2000 && _valid_count * 10 / _total_count < 5)
                return true;
            return false;
        }
        /// @brief 垃圾回收
        void garbageCollection()
        {
            // 垃圾回收 获取有效消息信息链表
            if (garbageCollectionCheck() == false)
                return;
            // 更新消息的实际存储位置
            std::list<MessagePtr> msgs = _mapper.garbageCollection();
            for (auto &msg : msgs)
            {
                auto it = _durable_msgs.find(msg->payload().properties().id());
                if (it == _durable_msgs.end())
                {
                    _msgs.push_back(msg);
                    _durable_msgs.insert(std::make_pair(msg->payload().properties().id(), msg));
                    info(logger, "垃圾回收后 有一条消息在内存尚未被管理 已插入待推送消息列表");
                    continue;
                }
                it->second->set_offset(msg->offset());
                it->second->set_length(msg->length());
            }
            // 更新有效消息数量 总持久化消息数量
            _valid_count = _total_count = msgs.size();
        }

    private:
        std::mutex _mutex;                                         ///< 互斥锁
        std::string _qname;                                        ///< 队列名称
        size_t _valid_count;                                       ///< 有效消息数量
        size_t _total_count;                                       ///< 总消息数量
        MessageMapper _mapper;                                     ///< 消息队列持久化管理类
        std::list<MessagePtr> _msgs;                               ///< 待推送消息列表
        std::unordered_map<std::string, MessagePtr> _durable_msgs; ///< 持久化消息映射表
        std::unordered_map<std::string, MessagePtr> _waitack_msgs; ///< 待确认消息映射表
    };

    /// @brief 消息管理类
    class MessageManager
    {
    public:
        using ptr = std::shared_ptr<MessageManager>; ///< 消息管理类指针
        /// @brief 构造函数
        /// @param basedir 基础目录
        /// @param qname 消息队列名称
        MessageManager(const std::string &basedir) : _basedir(basedir) {}
        /// @brief 初始化推送消息队列管理类
        /// @param qname 消息队列名称
        void initQueueMessage(const std::string &qname)
        {
            QueueMessage::ptr qmp;
            {
                std::unique_lock<std::mutex> lock(_mutex);
                auto it = _queue_msgs.find(qname);
                if (it != _queue_msgs.end())
                    return;
                qmp = std::make_shared<QueueMessage>(_basedir, qname);
                _queue_msgs.insert(std::make_pair(qname, qmp));
            }
            qmp->recovery();
        }
        /// @brief 销毁推送消息队列管理类
        /// @param qname
        void destroyQueueMessage(const std::string &qname)
        {
            QueueMessage::ptr qmp;
            {
                std::unique_lock<std::mutex> lock(_mutex);
                auto it = _queue_msgs.find(qname);
                if (it == _queue_msgs.end())
                    return;
                qmp = it->second;
                _queue_msgs.erase(qname);
            }
            qmp->clear();
        }
        /// @brief 向指定队列插入新消息
        /// @param qname 消息队列名称
        /// @param bp 消息属性
        /// @param body 消息主体
        /// @param mode 持久化标志
        /// @return 插入成功返回true 失败返回false
        bool insert(const std::string &qname, BasicProperties *bp, const std::string &body, DeliveryMode mode)
        {
            QueueMessage::ptr qmp;
            {
                std::unique_lock<std::mutex> lock(_mutex);
                auto it = _queue_msgs.find(qname);
                if (it == _queue_msgs.end())
                {
                    error(logger, "插入消息失败, 没有找到 %s 队列", qname.c_str());
                    return false;
                }
                qmp = it->second;
            }
            return qmp->insert(bp, body, mode);
        }
        /// @brief 获取队头消息
        /// @param qname 消息队列名称
        /// @return 消息指针
        MessagePtr front(const std::string &qname)
        {
            QueueMessage::ptr qmp;
            {
                std::unique_lock<std::mutex> lock(_mutex);
                auto it = _queue_msgs.find(qname);
                if (it == _queue_msgs.end())
                {
                    error(logger, "获取队头消息失败, 没有找到 %s 队列", qname.c_str());
                    return MessagePtr();
                }
                qmp = it->second;
            }
            return qmp->front();
        }
        /// @brief 应答消息
        /// @param qname 消息队列名称
        /// @param msg_id 消息id
        void ack(const std::string &qname, const std::string &msg_id)
        {
            QueueMessage::ptr qmp;
            {
                std::unique_lock<std::mutex> lock(_mutex);
                auto it = _queue_msgs.find(qname);
                if (it == _queue_msgs.end())
                {
                    error(logger, "确认消息失败, 没有找到 %s 队列", qname.c_str());
                    return;
                }
                qmp = it->second;
            }
            qmp->remove(msg_id);
        }

        /// @brief 获取可获取消息数量
        /// @return 可获取消息数量
        size_t availableCount(const std::string &qname)
        {
            QueueMessage::ptr qmp;
            {
                std::unique_lock<std::mutex> lock(_mutex);
                auto it = _queue_msgs.find(qname);
                if (it == _queue_msgs.end())
                {
                    error(logger, "获取可获取消息数量失败, 没有找到 %s 队列", qname.c_str());
                    return 0;
                }
                qmp = it->second;
            }
            return qmp->availableCount();
        }
        /// @brief 获取总消息数量
        /// @return 总消息数量
        size_t totalCount(const std::string &qname)
        {
            QueueMessage::ptr qmp;
            {
                std::unique_lock<std::mutex> lock(_mutex);
                auto it = _queue_msgs.find(qname);
                if (it == _queue_msgs.end())
                {
                    error(logger, "获取总消息数量失败, 没有找到 %s 队列", qname.c_str());
                    return 0;
                }
                qmp = it->second;
            }
            return qmp->totalCount();
        }
        /// @brief 获取待确认消息数量
        /// @return 待确认消息数量
        size_t waitAckCount(const std::string &qname)
        {
            QueueMessage::ptr qmp;
            {
                std::unique_lock<std::mutex> lock(_mutex);
                auto it = _queue_msgs.find(qname);
                if (it == _queue_msgs.end())
                {
                    error(logger, "获取待确认消息数量失败, 没有找到 %s 队列", qname.c_str());
                    return 0;
                }
                qmp = it->second;
            }
            return qmp->waitAckCount();
        }
        /// @brief 获取持久化消息数量
        /// @return 持久化消息数量
        size_t durableCount(const std::string &qname)
        {
            QueueMessage::ptr qmp;
            {
                std::unique_lock<std::mutex> lock(_mutex);
                auto it = _queue_msgs.find(qname);
                if (it == _queue_msgs.end())
                {
                    error(logger, "获取持久化消息数量失败, 没有找到 %s 队列", qname.c_str());
                    return 0;
                }
                qmp = it->second;
            }
            return qmp->durableCount();
        }
        /// @brief 清空
        void clear()
        {
            QueueMessage::ptr qmp;
            for (auto &qmsg : _queue_msgs)
            {
                qmsg.second->clear();
            }
        }

    private:
        std::mutex _mutex;
        std::string _basedir;
        std::unordered_map<std::string, QueueMessage::ptr> _queue_msgs;
    };
}
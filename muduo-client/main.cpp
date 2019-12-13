#include <iostream>

#include "codec.h"

#include <muduo/base/Logging.h>
#include <muduo/base/Mutex.h>
#include <muduo/net/EventLoopThread.h>
#include <muduo/net/TcpClient.h>

#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>

#include <stdio.h>

using namespace muduo;
using namespace muduo::net;

class ChatClient : boost::noncopyable
{
public:

    ChatClient(EventLoop* loop, const InetAddress& serverAddr)
            : loop_(loop),
              client_(loop, serverAddr, "ChatClient"),
              codec_(std::bind(&ChatClient::onStringMessage, this, muduo::_1, muduo::_2, muduo::_3))
    {
        client_.setConnectionCallback(
                std::bind(&ChatClient::onConnection, this, muduo::_1));
        client_.setMessageCallback(
                std::bind(&LengthHeaderCodec::onMessage, &codec_, muduo::_1, muduo::_2,muduo::_3));
        client_.enableRetry();
    }

    void connect()
    {
        client_.connect();
    }

    void disconnect()
    {
        // client_.disconnect();
    }


    void write(const StringPiece& message)
    {
        // mutex��������connection_���shared_ptr
        MutexLockGuard lock(mutex_);
        if (connection_)
        {
            codec_.send(get_pointer(connection_), message);
        }
    }

private:
    // �ú�����IO�߳���ִ�У�IO�߳������̲߳���ͬһ���߳�
    void onConnection(const TcpConnectionPtr& conn)
    {
        LOG_INFO << conn->localAddress().toIpPort() << " -> "
                 << conn->peerAddress().toIpPort() << " is "
                 << (conn->connected() ? "UP" : "DOWN");

        // mutex��������connection_���shared_ptr
        MutexLockGuard lock(mutex_);
        if (conn->connected())
        {
            connection_ = conn;
        }
        else
        {
            connection_.reset();
        }
    }

    void onStringMessage(const TcpConnectionPtr&,
                         const string& message,
                         Timestamp)
    {
        printf("<<< %s\n", message.c_str());
    }

    EventLoop* loop_;
    TcpClient client_;
    LengthHeaderCodec codec_;
    MutexLock mutex_;
    TcpConnectionPtr connection_;
};

int main(int argc, char* argv[])
{
    LOG_INFO << "pid = " << getpid();
    if (argc > 2)
    {
        EventLoopThread loopThread;
        uint16_t port = static_cast<uint16_t>(atoi(argv[2]));
        InetAddress serverAddr(argv[1], port);

        ChatClient client(loopThread.startLoop(), serverAddr);
        client.connect();
        std::string line;
        while (std::getline(std::cin, line))
        {

            client.write(line);
        }
        client.disconnect();
    }
    else
    {
        printf("Usage: %s host_ip port\n", argv[0]);
    }
}


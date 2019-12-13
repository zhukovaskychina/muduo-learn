#include "codec.h"

#include <muduo/base/Logging.h>
#include <muduo/base/Mutex.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpServer.h>

#include <boost/bind.hpp>

#include <set>
#include <stdio.h>
using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;
using namespace muduo;
using namespace muduo::net;

class ChatServer : noncopyable
{
public:
    ChatServer(EventLoop* loop,
               const InetAddress& listenAddr)
            : server_(loop, listenAddr, "ChatServer"),
              codec_(std::bind(&ChatServer::onStringMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))
    {
        server_.setConnectionCallback(
                std::bind(&ChatServer::onConnection, this, muduo::_1));
        server_.setMessageCallback(
                std::bind(&LengthHeaderCodec::onMessage, &codec_, muduo::_1, muduo::_2, muduo::_3));

    }

    void start()
    {
        server_.start();
    }

private:
    void onConnection(const TcpConnectionPtr& conn)
    {
        LOG_INFO << conn->localAddress().toIpPort() << " -> "
                 << conn->peerAddress().toIpPort() << " is "
                 << (conn->connected() ? "UP" : "DOWN");

        if (conn->connected())
        {
            connections_.insert(conn);
            LOG_INFO<<conn.get()->getTcpInfoString();

            //conn.get()->send("发送成功");
        }
        else
        {
            connections_.erase(conn);
        }
    }

    void onStringMessage(const TcpConnectionPtr&,
                         const string& message,
                         Timestamp)
    {

        LOG_INFO<<"接收到消息"<<message<<"";
        for (ConnectionList::iterator it = connections_.begin();
             it != connections_.end();
             ++it)
        {
            codec_.send(get_pointer(*it), "慢慢切图");
        }
    }

    typedef std::set<TcpConnectionPtr> ConnectionList;
    TcpServer server_;
    LengthHeaderCodec codec_;
    ConnectionList connections_;
};

int main(int argc, char* argv[])
{
    LOG_INFO << "pid = " << getpid();
    if (argc > 1)
    {
        EventLoop loop;
        uint16_t port = static_cast<uint16_t>(atoi(argv[1]));
        InetAddress serverAddr(port);
        ChatServer server(&loop, serverAddr);
        server.start();
        loop.loop();
    }
    else
    {
        printf("Usage: %s port\n", argv[0]);
    }
}


import socket
import os
import sys

if __name__ == '__main__':
    ServerHost='127.0.0.1'
    ServerPort=1081
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    print("服务器的socket建立了")
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock.bind((ServerHost, ServerPort))
    print('服务器的socket绑定到%s:%d'%(ServerHost,ServerPort))
    sock.listen(500)
    print('服务器开始监听端口%d，等待连接。'%ServerPort)    
    while True:
        connection,address = sock.accept()
        print("服务器sock连接到客户端地址：",address)
        try:
            connection.settimeout(5)
            buf = connection.recv(2048)
            print('客户端发来数据：%s'%buf)
            # if headers["Method"]=="CONNECT":
            connection.send(b'HTTP/1.1 200 Connection Established\r\n\r\n')
            print("向客户端发送了HTTP/1.1 200 Connection Established\r\n\r\n")
            buf=connection.recv(2048)
            print('客户端再次发来数据：%s'%buf)
            #else:  
                #print("error") 
        except socket.timeout:  
            print ('time out')
        connection.close()
    sock.close()

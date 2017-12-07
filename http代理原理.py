if __name__ == '__main__':  
    import socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  
    sock.bind(("0.0.0.0", 1080))  
    sock.listen(1)  
    while True:
        print("OK.....................")
        connection,address = sock.accept()
        print("OK..........ok...........")
        try:  
            # connection.settimeout(5)  
            buf = connection.recv(10240)
            if buf:  
                print('%s'%buf)
                connection.send(b'HTTP/1.1 200 Connection Established\r\n\r\n')
            else:  
                print("error") 
        except socket.timeout:  
            print ('time out')
        connection.close()
    sock.close()

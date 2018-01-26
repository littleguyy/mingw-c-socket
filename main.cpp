#include <malloc.h> //_msize() 函数调用
#include <stdio.h>
#include <winsock.h>
//https网站是443端口
#define PORT 80
#define USERAGENT "233 1.0"//定义USERAGENT头
int test;
char *build_get_query(char *, char *);
//全局变量
int client_socket;
char *host;
struct hostent *hent;
int iplen = 15; //XXX.XXX.XXX.XXX
struct sockaddr_in *remote;
int tmpres;
char *get;
char buf[BUFSIZ+1];

int main(int argc, char *argv[])
{


    printf("Socket client example\n");

    // create tcp socket
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    client_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client_socket < 0)
    {
        printf("client_socket = %d\n",client_socket);
        perror("Can't create TCP socket\n");
        exit(1);
    }

    // get host ip  www.ultratone.com.my
    host = "www.ultratone.com.my";
    printf("Host: %s\n",host);
    char *ip = (char *)malloc(iplen+1); //申请内存空间大小，返回一个空间地址给ip指针
    memset(ip, 0, iplen+1); //把ip指向的内存空间填满0
    if((hent = gethostbyname(host)) == NULL) //获取host域名的ip值的结构
    {
        perror("Can't get IP");
        exit(1);
    }
    ip = inet_ntoa(*(struct in_addr *)*hent->h_addr_list); //把hent结构值转换成ip地址xxx.xxx.xxx.xxx
    printf("The IP: %s\n",ip);

    // setup remote socket
    remote = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in *)); //remote结构申请内存空间
    remote->sin_family = AF_INET; //以下是给remote结构赋值
    printf("s_addr:%d\n",remote->sin_addr.s_addr);
    remote->sin_addr.s_addr = inet_addr(ip);
    remote->sin_port = htons(PORT);
    printf("s_addr:%d\n",remote->sin_addr.s_addr);
    // have to read
    // http://msdn.microsoft.com/en-us/library/windows/desktop/ms737625(v=vs.85).aspx

    // connect socket 把本地client_socket连接到remote服务器
    if(connect(client_socket, (struct sockaddr *)remote, sizeof(struct sockaddr)) == SOCKET_ERROR)
    { //如果连接失败执行这些
        closesocket(client_socket);
        perror("Could not connect");
        WSACleanup();
        exit(1);
    }
    //连接远端服务器成功后，执行以下程序
    // prepare query
    get = build_get_query(host,"/"); //组建一个http协议的get请求，从而可以通过这个请求一个http网页内容
    printf("询问query: \n%s\n",get);

    //Send the query to the server 发送这个get请求到服务器
    int sent = 0;
    while(sent < strlen(get))
    {
        tmpres = send(client_socket, get+sent, strlen(get)-sent, 0);
        if(tmpres == -1)
        {
            perror("Can't send query");
            exit(1);
        }
        sent += tmpres;
    }

    //now it is time to receive the page开始接收服务器应答的网页内容
    memset(buf, 0, sizeof(buf));
    int htmlstart = 0;
    char * htmlcontent;
    //while((tmpres = recv(client_socket, buf, BUFSIZ, 0)) > 0) {
    while((tmpres = recv(client_socket, buf, BUFSIZ, 0)) > 0)
    {
        if(htmlstart == 0)
        {
            /* Under certain conditions this will not work.
             * If the \r\n\r\n part is splitted into two messages
             * it will fail to detect the beginning of HTML content
             */
             //fprintf(stdout,htmlcontent);
            htmlcontent = strstr(buf, "\r\n\r\n"); //查找第一个字符串"\r\n\r\n"，并把此字符串之后的字符都赋值给htmlcontent
            //这个函数不错，可方便分离http协议头
            if(htmlcontent != NULL)
            {
                htmlstart = 1;
                htmlcontent += 4;
            }
        }
        else
        {
            htmlcontent = buf;
        }
        if(htmlstart)
        {
            fprintf(stdout, htmlcontent);
        }

        memset(buf, 0, tmpres);
    }
    if(tmpres < 0)
    {
        perror("Error receiving data");
    }
    //网页接收完毕，收尾工作，释放并关闭所有变量
    free(get);
    free(remote);
    closesocket(client_socket);
    WSACleanup();

    printf("\nProgram end");

    return 0;
}

void init()
{

}

char *build_get_query(char *host, char *page)
{
    char *query;
    char *getpage = page;
    //char *tpl = "GET /%s HTTP/1.0\r\nHost: %s\r\nUser-Agent: %s\r\n\r\n";
    char *tpl = "GET /%s HTTP/1.0\r\nHost: %s\r\n\r\n"; //设定get请求格式公式
    //char *tpl = "GET /%s HTTP/1.0\r\nHost: %\r\n";
    //test=strlen(getpage);
    if(getpage[0] == '/')
    {
        getpage = getpage + 1;
        fprintf(stderr,"Removing leading \"/\", converting %s to %s\n", page, getpage);
    }
    // -5 is to consider the %s %s %s in tpl and the ending \0
    query = (char *)malloc(strlen(host)+strlen(getpage)+strlen(tpl)); //申请内存空间 本来-3，这里多申请3个字符内存空间，没有问题，只不过浪费3个字符内存空间。
    if(query!=NULL)
    {
        sprintf(query, tpl, getpage, host); //给query变量赋值，也就是把get请求字符串存入query指向的内存空间。
        //test=_msize(query);
        //test=strlen(query);
    }
    else
    {
        printf("mallocerror!\n");
        exit(-1);
    }
    return query;
}

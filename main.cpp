#include <malloc.h> //_msize() ��������
#include <stdio.h>
#include <winsock.h>
//https��վ��443�˿�
#define PORT 80
#define USERAGENT "233 1.0"//����USERAGENTͷ
int test;
char *build_get_query(char *, char *);
//ȫ�ֱ���
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
    char *ip = (char *)malloc(iplen+1); //�����ڴ�ռ��С������һ���ռ��ַ��ipָ��
    memset(ip, 0, iplen+1); //��ipָ����ڴ�ռ�����0
    if((hent = gethostbyname(host)) == NULL) //��ȡhost������ipֵ�Ľṹ
    {
        perror("Can't get IP");
        exit(1);
    }
    ip = inet_ntoa(*(struct in_addr *)*hent->h_addr_list); //��hent�ṹֵת����ip��ַxxx.xxx.xxx.xxx
    printf("The IP: %s\n",ip);

    // setup remote socket
    remote = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in *)); //remote�ṹ�����ڴ�ռ�
    remote->sin_family = AF_INET; //�����Ǹ�remote�ṹ��ֵ
    printf("s_addr:%d\n",remote->sin_addr.s_addr);
    remote->sin_addr.s_addr = inet_addr(ip);
    remote->sin_port = htons(PORT);
    printf("s_addr:%d\n",remote->sin_addr.s_addr);
    // have to read
    // http://msdn.microsoft.com/en-us/library/windows/desktop/ms737625(v=vs.85).aspx

    // connect socket �ѱ���client_socket���ӵ�remote������
    if(connect(client_socket, (struct sockaddr *)remote, sizeof(struct sockaddr)) == SOCKET_ERROR)
    { //�������ʧ��ִ����Щ
        closesocket(client_socket);
        perror("Could not connect");
        WSACleanup();
        exit(1);
    }
    //����Զ�˷������ɹ���ִ�����³���
    // prepare query
    get = build_get_query(host,"/"); //�齨һ��httpЭ���get���󣬴Ӷ�����ͨ���������һ��http��ҳ����
    printf("ѯ��query: \n%s\n",get);

    //Send the query to the server �������get���󵽷�����
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

    //now it is time to receive the page��ʼ���շ�����Ӧ�����ҳ����
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
            htmlcontent = strstr(buf, "\r\n\r\n"); //���ҵ�һ���ַ���"\r\n\r\n"�����Ѵ��ַ���֮����ַ�����ֵ��htmlcontent
            //������������ɷ������httpЭ��ͷ
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
    //��ҳ������ϣ���β�������ͷŲ��ر����б���
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
    char *tpl = "GET /%s HTTP/1.0\r\nHost: %s\r\n\r\n"; //�趨get�����ʽ��ʽ
    //char *tpl = "GET /%s HTTP/1.0\r\nHost: %\r\n";
    //test=strlen(getpage);
    if(getpage[0] == '/')
    {
        getpage = getpage + 1;
        fprintf(stderr,"Removing leading \"/\", converting %s to %s\n", page, getpage);
    }
    // -5 is to consider the %s %s %s in tpl and the ending \0
    query = (char *)malloc(strlen(host)+strlen(getpage)+strlen(tpl)); //�����ڴ�ռ� ����-3�����������3���ַ��ڴ�ռ䣬û�����⣬ֻ�����˷�3���ַ��ڴ�ռ䡣
    if(query!=NULL)
    {
        sprintf(query, tpl, getpage, host); //��query������ֵ��Ҳ���ǰ�get�����ַ�������queryָ����ڴ�ռ䡣
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

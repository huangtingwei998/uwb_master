#include "nlink_linktrack_nodeframe1.h"
#include "cJSON.h"
#include "s2j.h"
#include <Windows.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <mmsystem.h>
#include <stdint.h>
HANDLE hCom;
#include <winsock2.h>

/*
 * 引入socket的win版本
 */
#pragma comment(lib,"ws2_32.lib")

#pragma pack()


typedef struct
{
    uint8_t count;
    /*
     * 这里没怎么优化，就是简单一个节点对应xyz三个变量
     */
    double node1[3];
    double node2[3];
} UWB;

static cJSON *uwb_to_json(void* struct_obj) {
    UWB *uwb = (UWB *)struct_obj;

    /* 创建json对象 */
    s2j_create_json_obj(json_uwb);
    /* 序列化参数 */
    s2j_json_set_basic_element(json_uwb, uwb, int, count);
    s2j_json_set_array_element(json_uwb, uwb, double , node1, 3);
    s2j_json_set_array_element(json_uwb, uwb, double , node2, 3);
    return json_uwb;
}

int main()
{
    /******************************************************************socket设置****************************************************************************/
    //初始化WSA
    WORD sockVersion = MAKEWORD(2, 2);
    WSADATA wsaData;
    if (WSAStartup(sockVersion, &wsaData) != 0)
    {
        return 0;
    }

    //创建套接字
    SOCKET slisten = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (slisten == INVALID_SOCKET)
    {
        printf("socket error !");
        return 0;
    }

    //绑定IP和端口
    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(21578);
    sin.sin_addr.S_un.S_addr = INADDR_ANY;
    if (bind(slisten, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
    {
        printf("bind error !");
    }

    //开始监听
    if (listen(slisten, 5) == SOCKET_ERROR)
    {
        printf("listen error !");
        return 0;
    }

    //循环接收数据
    SOCKET sClient;
    struct sockaddr_in remoteAddr;
    int nAddrlen = sizeof(remoteAddr);
    //char revData[255];
    printf("waiting for connect...\n");
    sClient = accept(slisten, (SOCKADDR *)&remoteAddr, &nAddrlen);

    /******************************************************************com设置****************************************************************************/

    hCom = CreateFile("\\\\.\\COM15",//COM1口
                      GENERIC_READ, //允许读
                      0, //指定共享属性，由于串口不能共享，所以该参数必须为0
                      NULL,
                      OPEN_EXISTING, //打开而不是创建
                      0, //属性描述，该值为FILE_FLAG_OVERLAPPED，表示使用异步I/O，该参数为0，表示同步I/O操作
                      NULL);

    if (hCom == INVALID_HANDLE_VALUE){
        printf("open COM failed!\n");
        return FALSE;
    }else{
        printf("open COM sucesses\n");
    }

    SetupComm(hCom, 1024, 1024); //输入缓冲区和输出缓冲区的大小都是1024

    /***超时设置***/
    COMMTIMEOUTS TimeOuts;
    //设定读超时
    TimeOuts.ReadIntervalTimeout = MAXDWORD;//读间隔超时
    TimeOuts.ReadTotalTimeoutMultiplier = 0;//读时间系数
    TimeOuts.ReadTotalTimeoutConstant = 0;//读时间常量
    //设定写超时
    TimeOuts.WriteTotalTimeoutMultiplier = 1;//写时间系数
    TimeOuts.WriteTotalTimeoutConstant = 1;//写时间常量
    SetCommTimeouts(hCom, &TimeOuts); //设置超时

    /**配置串口****/
    DCB dcb;
    GetCommState(hCom, &dcb);
    dcb.BaudRate = 921600; //波特率为9600
    dcb.ByteSize = 8; //每个字节有8位
    dcb.Parity = NOPARITY; //无奇偶校验位
    dcb.StopBits = ONESTOPBIT; //一个停止位
    SetCommState(hCom, &dcb);

    DWORD wCount;//实际读取的字节数
    bool bReadStat;

    uint8_t output[68];

    /******************************************************************总程序****************************************************************************/
    clock_t start = clock();
    while (1) {
        /*
         * 解释为什么是99，设定的UWB模块的频率是10hz，为了保证不发生粘包，所以快于0.1s，取99，这个暂时还不知道怎么解决。有的时候也会失效。
         */
        if ((clock() - start) == 99) {
            start = clock();
            /*
             * 串口读取数据
             */
            bReadStat = ReadFile(hCom, output, sizeof(output), &wCount, NULL);
            /*
             * 如果解析成功，返回true。
             */
            if (g_nlt_nodeframe1.UnpackData(output, 68))
            {
                nlt_nodeframe1_result_t *result = &g_nlt_nodeframe1.result;
                /*
                 * 拿出数据
                 */
                uint8_t count = result->valid_node_count;
                double x0 = (double)result->nodes[0]->pos_3d[0];
                double y0 = (double)result->nodes[0]->pos_3d[1];
                double z0 = (double)result->nodes[0]->pos_3d[2];

                printf("x:%f, y:%f\r\n",
                       result->nodes[0]->pos_3d[0], result->nodes[0]->pos_3d[0]);

                double x1 = (double)result->nodes[1]->pos_3d[0];
                double y1 = (double)result->nodes[1]->pos_3d[1];
                double z1 = (double)result->nodes[1]->pos_3d[2];

                printf("x:%f, y:%f\r\n",
                       result->nodes[1]->pos_3d[0], result->nodes[1]->pos_3d[0]);

                UWB uwb = {
                        .count = count,
                        .node1 = {x0,y0,z0},
                        .node2 = {x1,y1,z1},
                };
            /*
             * 转成json
             */
            cJSON *json_uwb = uwb_to_json(&uwb);
            char *json_uwb_string = cJSON_PrintUnformatted(json_uwb);
            /*
             * socket发送数据
             */
            send(sClient, json_uwb_string, strlen(json_uwb_string), 0);


            }
            /*
             * 每次读完，就清空缓冲区，防止粘包和拆包。
             */
            PurgeComm(hCom, PURGE_TXCLEAR | PURGE_RXCLEAR); //清空缓冲区
        }

    }
    CloseHandle(hCom);
  return EXIT_SUCCESS;
}

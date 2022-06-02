#include "nlink_linktrack_nodeframe1.h"
#include <Windows.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <conio.h>
#include <mmsystem.h>
#include <stdint.h>
HANDLE hCom;

#include <winsock2.h>
#include <windows.h>
#include <stdio.h>

#pragma comment(lib,"ws2_32.lib")
#pragma pack(1)
typedef struct
{
  uint8_t a;
  uint8_t b;
  uint32_t c;
  double d;
  uint8_t e;
} pack_test_t;
#pragma pack()


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
        if ((clock() - start) == 100) {
            start = clock();
            bReadStat = ReadFile(hCom, output, sizeof(output), &wCount, NULL);

            if (g_nlt_nodeframe1.UnpackData(output, 68))
            {
                nlt_nodeframe1_result_t *result = &g_nlt_nodeframe1.result;
                printf("LinkTrack NodeFrame1 data unpack successfully:\r\n");
                printf("id:%d, system_time:%d, valid_node_count:%d\r\n", result->id,
                       result->system_time, result->valid_node_count);
                for (int i = 0; i < result->valid_node_count; ++i)
                {
                    nlt_nodeframe1_node_t *node = result->nodes[i];
                    printf("role:%d, id:%d, x:%f, y:%f\r\n", node->role, node->id,
                           node->pos_3d[0], node->pos_3d[1]);
                }
            }
            PurgeComm(hCom, PURGE_TXCLEAR | PURGE_RXCLEAR); //清空缓冲区
        }

    }
    CloseHandle(hCom);
  return EXIT_SUCCESS;
}

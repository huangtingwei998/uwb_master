#include <stdint.h>
#include "s2j.h"

//
// Created by 老黄 on 2022/6/11.
//
typedef struct
{
    uint8_t count;
    /*
     * 这里不想怎么优化，就是简单一个节点对应xyz三个变量
     */
    uint8_t node1[3];
    uint8_t node2[3];
    uint8_t node3[3];
} UWB;

static cJSON *uwb_to_json(void* struct_obj) {
    UWB *uwb = (UWB *)struct_obj;

    /* 创建json对象 */
    s2j_create_json_obj(json_uwb);
    /* 序列化参数 */
    s2j_json_set_basic_element(json_uwb, uwb, int, count);
    s2j_json_set_array_element(json_uwb, uwb, int , node1, 3);
    s2j_json_set_array_element(json_uwb, uwb, int , node2, 3);
    s2j_json_set_array_element(json_uwb, uwb, int , node3, 3);
    return json_uwb;
}
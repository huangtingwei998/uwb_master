

#ifndef __S2J_H__
#define __S2J_H__

#include "cJSON.h"
#include <string.h>
#include "s2jdef.h"

#ifdef __cplusplus
extern "C" {
#endif

/* struct2json software version number */
#define S2J_SW_VERSION                "1.0.2"

/* Create JSON object */
#define s2j_create_json_obj(json_obj) \
    S2J_CREATE_JSON_OBJECT(json_obj)

/* Delete JSON object */
#define s2j_delete_json_obj(json_obj) \
    S2J_DELETE_JSON_OBJECT(json_obj)

/* Set basic type element for JSON object */
#define s2j_json_set_basic_element(to_json, from_struct, type, element) \
    S2J_JSON_SET_BASIC_ELEMENT(to_json, from_struct, type, element)

/* Set array type element for JSON object */
#define s2j_json_set_array_element(to_json, from_struct, type, element, size) \
    S2J_JSON_SET_ARRAY_ELEMENT(to_json, from_struct, type, element, size)

/* Set child structure type element for JSON object */
#define s2j_json_set_struct_element(child_json, to_json, child_struct, from_struct, type, element) \
    S2J_JSON_SET_STRUCT_ELEMENT(child_json, to_json, child_struct, from_struct, type, element)

/* Create structure object */
#define s2j_create_struct_obj(struct_obj, type) \
    S2J_CREATE_STRUCT_OBJECT(struct_obj, type)

/* Delete structure object */
#define s2j_delete_struct_obj(struct_obj) \
    S2J_DELETE_STRUCT_OBJECT(struct_obj)

/* Get basic type element for structure object */
#define s2j_struct_get_basic_element(to_struct, from_json, type, element) \
    S2J_STRUCT_GET_BASIC_ELEMENT(to_struct, from_json, type, element)

/* Get array type element for structure object */
#define s2j_struct_get_array_element(to_struct, from_json, type, element) \
    S2J_STRUCT_GET_ARRAY_ELEMENT(to_struct, from_json, type, element)

/* Get child structure type element for structure object */
#define s2j_struct_get_struct_element(child_struct, to_struct, child_json, from_json, type, element) \
    S2J_STRUCT_GET_STRUCT_ELEMENT(child_struct, to_struct, child_json, from_json, type, element)

/* s2j.c */
extern S2jHook s2jHook;
void s2j_init(S2jHook *hook);

#ifdef __cplusplus
}
#endif

#endif /* __S2J_H__ */

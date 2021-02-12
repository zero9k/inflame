/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : database.c
 * Author : Clode@RevivalWorld
 * Date   : 2000-10-29
 * Note   : database package
 * Update :
 *  o 2000-00-00

 -----------------------------------------
 */

 #include "base/package_api.h"


// Copy from mapping.c to do mapping building by Clode
static svalue_t *insert_in_mapping (mapping_t * m, char * key) {
    svalue_t lv;
    svalue_t *ret;

    lv.type = T_STRING;
    lv.subtype = STRING_CONSTANT;
    lv.u.string = key;
    ret = find_for_insert(m, &lv, 1);
    /* lv.u.string will have been converted to a shared string */
    free_string(lv.u.string);
    return ret;
}

//
// set()
// by Clode@Repulsion.World on 2001/07/07
//
#ifdef F_SET
void f_set () {
    int i, j;
    object_t *ob;
    svalue_t *dbase, *value = NULL;
    mapping_t *map;
    unsigned short type;
    char *src, *dst;
    char *tmpstr;

    if( st_num_arg == 3 )
    {
    	ob = sp->u.ob;
    	pop_stack();
    }
    else
    	ob = current_object;

    i = find_global_variable(ob->prog, "database", &type, 0);
    if (i == -1)
    {
       	pop_2_elems();
        error("(set) %s 物件未宣告全域映射資料庫變數。\n", ob->obname);
    }

    dbase = &ob->variables[i];

    if( dbase->type != T_MAPPING )
    {
    	pop_2_elems();
    	error("(set) %s 物件的資料庫變數型態錯誤。\n", ob->obname);
    }

    map = dbase->u.map;
    src = (char *)(sp-1)->u.string;
    dst = tmpstr = (char *)DMALLOC(SVALUE_STRLEN(sp-1) + 1, TAG_STRING, "set");
    j=0;

    while (*src)
    {
    	i=0;
    	if( ++j > 20 )
	{
    		pop_2_elems();
    		error("(set) %s too deep mapping(20)。\n", ob->obname);
    	}

	while (*src != '/' && *src)
	{
	    *(dst+i) = *src++;
	    i++;
	}

	if (*src == '/')
	{
	    while (*++src == '/');
	    if(!i) continue;
	}
	*(dst+i) = '\0';

	if(!*src)
	{
    		value = insert_in_mapping(map, dst);
    		*value = *sp--;
    		break;
    	}

	value = find_string_in_mapping(map, tmpstr);

	if(value == &const0u || value->type != T_MAPPING)
	{
    		value = insert_in_mapping(map, dst);
		value->type = T_MAPPING;
		value->u.map = allocate_mapping(0);
	}

	map = value->u.map;
	dst = tmpstr;
    }

    FREE(tmpstr);
    free_string_svalue(sp--);
    push_svalue(value);
}
#endif

//
// set_temp()
// by Clode@Repulsion.World on 2001/07/07
//
#ifdef F_SET_TEMP
void f_set_temp () {
    int i, j;
    object_t *ob;
    svalue_t *dbase, *value = NULL;
    mapping_t *map;
    unsigned short type;
    char *src, *dst;
    char *tmpstr;

    if( st_num_arg == 3 )
    {
    	ob = sp->u.ob;
    	pop_stack();
    }
    else
    	ob = current_object;

    i = find_global_variable(ob->prog, "temp_database", &type, 0);
    if (i == -1)
    {
        pop_2_elems();
        error("(set_temp) %s 物件未宣告全域映射資料庫變數。\n", ob->obname);
    }

    dbase = &ob->variables[i];

    if( dbase->type != T_MAPPING )
    {
    	pop_2_elems();
    	error("(set_temp) %s 物件的資料庫變數型態錯誤。\n", ob->obname);
    }

    map = dbase->u.map;
    src = (char *)(sp-1)->u.string;
    dst = tmpstr = (char *)DMALLOC(SVALUE_STRLEN(sp-1) + 1, TAG_STRING, "set_temp");
    j=0;

    while (*src)
    {
    	i=0;
    	if( ++j > 20 )
	{
    		pop_2_elems();
    		error("(set_temp) %s too deep mapping(20)。\n", ob->obname);
    	}

	while (*src != '/' && *src)
	{
	    *(dst+i) = *src++;
	    i++;
	}

	if (*src == '/')
	{
	    while (*++src == '/');
	    if(!i) continue;
	}
	*(dst+i) = '\0';

	if(!*src)
	{
    		value = insert_in_mapping(map, dst);
    		*value = *sp--;
       		break;
    	}

	value = find_string_in_mapping(map, tmpstr);

	if(value == &const0u || value->type != T_MAPPING)
	{
    		value = insert_in_mapping(map, dst);
		value->type = T_MAPPING;
		value->u.map = allocate_mapping(0);
	}

	map = value->u.map;
	dst = tmpstr;
    }

    FREE(tmpstr);
    free_string_svalue(sp--);
    push_svalue(value);
}
#endif

//
// query()
// by Clode@Repulsion.World on 2000/10/29
//
// 多層次 mapping 結構搜尋
// 記憶體節省機制 shadow_ob 檢查
//

#ifdef F_QUERY
void f_query () {
    int idx;
    object_t *ob;
    unsigned short type;
    svalue_t *value, *shadow;
    char *src, *dst;
    mapping_t *map;
    char *tmpstr;

    if( st_num_arg==2 )
    {
        ob=sp->u.ob;
        pop_stack();
    }
    else
        ob = current_object;

    idx = find_global_variable(ob->prog, "database", &type, 0);
    if (idx == -1)
    {
        free_string_svalue(sp--);
        push_undefined();
        return;
    }
    value = &ob->variables[idx];

    if( value->type != T_MAPPING )
    {
    	free_string_svalue(sp--);
    	error("(query) %s 物件的資料庫變數型態錯誤。\n", ob->obname);
    }

    shadow = find_string_in_mapping(value->u.map, "shadow_ob");

    if( shadow != &const0u && shadow->type == T_OBJECT && ob != shadow->u.ob)
    {
    	idx = find_global_variable(shadow->u.ob->prog, "database", &type, 0);
    	if (idx != -1)
    	{
        	value = &shadow->u.ob->variables[idx];

        	if( value->type != T_MAPPING )
    			value = &ob->variables[idx];
    		else
    			ob = shadow->u.ob;
    	}
    }

    map = value->u.map;
    src = (char *)sp->u.string;
    dst = tmpstr = (char *)DMALLOC(SVALUE_STRLEN(sp) + 1, TAG_STRING, "query");

    while (*src)
    {
	while (*src != '/' && *src)
	    *dst++ = *src++;
	if (*src == '/')
	{
	    while (*++src == '/');
	    if( dst == tmpstr ) continue;
	}
	*dst = '\0';
	value = find_string_in_mapping(map, tmpstr);

	if( value == &const0u ) break;
	if( value->type != T_MAPPING )
	{
	    if(*src) value = &const0u;
	    break;
	}
	map = value->u.map;
	dst = tmpstr;
    }

    FREE(tmpstr);
    free_string_svalue(sp--);
    push_svalue(value);
}
#endif

//
// query_temp()
// by Clode@Repulsion.World on 2000/10/29
//
// 多層次 mapping 結構搜尋
//
#ifdef F_QUERY_TEMP
void f_query_temp () {
    int idx;
    object_t *ob;
    unsigned short type;
    svalue_t *value;
    char *src, *dst;
    mapping_t *map;
    char *tmpstr;

    if( st_num_arg==2 ) {
        ob=sp->u.ob;
        pop_stack();
    } else
        ob = current_object;

    idx = find_global_variable(ob->prog, "temp_database", &type, 0);
    if (idx == -1)
    {
        free_string_svalue(sp--);
        push_undefined();
        return;
    }
    value = &ob->variables[idx];

    if( value->type != T_MAPPING )
    {
    	free_string_svalue(sp--);
    	error("(query_temp) %s 物件的資料庫變數型態錯誤。\n", ob->obname);
    }

    map = value->u.map;
    src = (char *)sp->u.string;
    dst = tmpstr = (char *)DMALLOC(SVALUE_STRLEN(sp) + 1, TAG_STRING, "query_temp");

    while (*src)
    {
	while (*src != '/' && *src)
	    *dst++ = *src++;
	if (*src == '/')
	{
	    while (*++src == '/');
	    if( dst == tmpstr ) continue;
	}
	*dst = '\0';
	value = find_string_in_mapping(map, tmpstr);

	if( value == &const0u ) break;
	if( value->type != T_MAPPING )
	{
	    if(*src) value = &const0u;
	    break;
	}
	map = value->u.map;
	dst = tmpstr;
    }

    FREE(tmpstr);
    free_string_svalue(sp--);
    push_svalue(value);
}
#endif

//
// addn()
// by Clode@Repulsion.World on 2001/03/23
//
#ifdef F_ADDN
void f_addn () {
    int i, j;
    object_t *ob;
    unsigned short type;
    mapping_t *map;
    svalue_t *value;
    char *src, *dst;
    char *tmpstr;

    if( st_num_arg == 3 )
    {
        ob = sp->u.ob;
        pop_stack();
    }
    else
        ob = current_object;

    i = find_global_variable(ob->prog, "database", &type, 0);
    if (i == -1)
    {
        pop_2_elems();
        error("(addn) %s 物件未宣告全域映射資料庫變數。\n", ob->obname);
    }

    value = &ob->variables[i];

    if( value->type != T_MAPPING )
    {
    	pop_2_elems();
    	error("(addn) %s 物件的資料庫變數型態錯誤。\n", ob->obname);
    }

    map = value->u.map;
    src = (char *)(sp-1)->u.string;
    dst = tmpstr = (char *)DMALLOC(SVALUE_STRLEN(sp-1) + 1, TAG_STRING, "addn");
    j=0;

    while (*src)
    {
    	i=0;
    	if( ++j > 20 )
	{
    		pop_2_elems();
    		error("(addn) %s too deep mapping(20)。\n", ob->obname);
    	}

	while (*src != '/' && *src)
	{
	    *(dst+i) = *src++;
	    i++;
	}

	if (*src == '/')
	{
	    while (*++src == '/');
	    if(!i) continue;
	}
	*(dst+i) = '\0';

	value = find_string_in_mapping(map, tmpstr);

	if(!*src)
	{
    		if( value == &const0u || value->type != T_NUMBER )
    		{
    			value = insert_in_mapping(map, dst);
			*value = *sp--;
    		}
    		else
    			value->u.number += (sp--)->u.number;
    		break;
    	}

	if(value == &const0u || value->type != T_MAPPING)
	{
    		value = insert_in_mapping(map, dst);
		value->type = T_MAPPING;
		value->u.map = allocate_mapping(0);
	}

	map = value->u.map;
	dst = tmpstr;
    }

    FREE(tmpstr);
    free_string_svalue(sp--);
    push_svalue(value);
}
#endif

//
// addn_temp()
// by Clode@Repulsion.World on 2001/03/23
//
#ifdef F_ADDN_TEMP
void f_addn_temp () {
    int i, j;
    object_t *ob;
    unsigned short type;
    mapping_t *map;
    svalue_t *value;
    char *src, *dst;
    char *tmpstr;

    if( st_num_arg == 3 )
    {
        ob = sp->u.ob;
        pop_stack();
    }
    else
        ob = current_object;

    i = find_global_variable(ob->prog, "temp_database", &type, 0);
    if (i == -1)
    {
        pop_2_elems();
        error("(addn_temp) %s 物件未宣告全域映射資料庫變數。\n", ob->obname);
    }

    value = &ob->variables[i];

    if( value->type != T_MAPPING )
    {
    	pop_2_elems();
    	error("(addn_temp) %s 物件的資料庫變數型態錯誤。\n", ob->obname);
    }

    map = value->u.map;
    src = (char *)(sp-1)->u.string;
    dst = tmpstr = (char *)DMALLOC(SVALUE_STRLEN(sp-1) + 1, TAG_STRING, "addn_temp");
    j=0;

    while (*src)
    {
    	i=0;
    	if( ++j > 20 )
	{
    		pop_2_elems();
    		error("(addn_temp) %s too deep mapping(20)。\n", ob->obname);
    	}

	while (*src != '/' && *src)
	{
	    *(dst+i) = *src++;
	    i++;
	}

	if (*src == '/')
	{
	    while (*++src == '/');
	    if(!i) continue;
	}
	*(dst+i) = '\0';

	value = find_string_in_mapping(map, tmpstr);

	if(!*src)
	{
    		if( value == &const0u || value->type != T_NUMBER )
    		{
    			value = insert_in_mapping(map, dst);
			*value = *sp--;
    		}
    		else
    			value->u.number += (sp--)->u.number;
    		break;
    	}

	if(value == &const0u || value->type != T_MAPPING)
	{
    		value = insert_in_mapping(map, dst);
		value->type = T_MAPPING;
		value->u.map = allocate_mapping(0);
	}

	map = value->u.map;
	dst = tmpstr;
    }

    FREE(tmpstr);
    free_string_svalue(sp--);
    push_svalue(value);
}
#endif

//
// delete()
// by Clode@Repulsion.World on 2001/03/23
//
#ifdef F_DELETE
void f_delete () {
    int i;
    object_t *ob;
    svalue_t *value, lv;
    mapping_t *map;
    unsigned short type;
    char *src, *dst;

    if( st_num_arg == 2 )
    {
    	ob = sp->u.ob;
    	pop_stack();
    }
    else
    	ob = current_object;

    i = find_global_variable(ob->prog, "database", &type, 0);
    if (i == -1)
    {
        free_string_svalue(sp--);
        error("(delete) %s 物件未宣告全域映射資料庫變數。\n", ob->obname);
    }

    value = &ob->variables[i];

    if( value->type != T_MAPPING )
    {
        free_string_svalue(sp--);
    	error("(delete) %s 物件的資料庫變數型態錯誤。\n", ob->obname);
    }

    map = value->u.map;
    src = (char *)sp->u.string;
    dst = (char *)DMALLOC(SVALUE_STRLEN(sp)+1, TAG_STRING, "delete");

    while (*src)
    {
    	i = 0;
	while (*src != '/' && *src)
	{
	    *(dst+i) = *src++;
	    i++;
	}
	if (*src == '/')
	{
	    while (*++src == '/');
	    if(!i) continue;
	}

	*(dst+i) = '\0';

	value = find_string_in_mapping(map, dst);

	if( value == &const0u ) break;
	if(!*src)
	{
   	    	lv.type = T_STRING;
   	    	lv.subtype = STRING_CONSTANT;
    		lv.u.string = dst;
	    	mapping_delete(map, &lv);
	    	FREE(dst);
	    	free_string_svalue(sp--);
	    	return;
	}
	if( value->type != T_MAPPING ) break;

	map = value->u.map;
    }

    FREE(dst);
    free_string_svalue(sp--);
}
#endif

//
// delete_temp()
// by Clode@Repulsion.World on 2001/03/23
//
#ifdef F_DELETE_TEMP
void f_delete_temp () {
    int i;
    object_t *ob;
    svalue_t *value, lv;
    mapping_t *map;
    unsigned short type;
   char *src, *dst;

    if( st_num_arg == 2 )
    {
    	ob = sp->u.ob;
    	pop_stack();
    }
    else
    	ob = current_object;

    i = find_global_variable(ob->prog, "temp_database", &type, 0);
    if (i == -1)
    {
        free_string_svalue(sp--);
        error("(delete_temp) %s 物件未宣告全域映射資料庫變數。\n", ob->obname);
    }

    value = &ob->variables[i];

    if( value->type != T_MAPPING )
    {
        free_string_svalue(sp--);
    	error("(delete_temp) %s 物件的資料庫變數型態錯誤。\n", ob->obname);
    }

    map = value->u.map;
    src = (char *)sp->u.string;
    dst = (char *)DMALLOC(SVALUE_STRLEN(sp)+1, TAG_STRING, "delete_temp");

    while(*src)
    {
    	i = 0;

	while(*src != '/' && *src)
	    *(dst+(i++)) = *src++;

	if(*src == '/')
	{
	    while (*++src == '/');
	    if(!i) continue;
	}

	*(dst+i) = '\0';

	value = find_string_in_mapping(map, dst);

	if( value == &const0u ) break;

	if(!*src)
	{
   	    	lv.type = T_STRING;
   	    	lv.subtype = STRING_CONSTANT;
    		lv.u.string = dst;
	    	mapping_delete(map, &lv);
	    	FREE(dst);
	    	free_string_svalue(sp--);
	    	return;
	}
	if( value->type != T_MAPPING ) break;

	map = value->u.map;
    }

    FREE(dst);
    free_string_svalue(sp--);
}
#endif

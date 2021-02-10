/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : rw.c
 * Author : Clode@RevivalWorld
 * Date   : 2007-05-26
 * Note   : database package
 * Update :
 *  o 2000-00-00

 -----------------------------------------
 */


#include "base/package_api.h"

/* ���N�즳 find_player() */
#ifdef NO_ADD_ACTION
void f_find_player () {

	int len;
	object_t *ob;
	char *name, path[26] = "/data/user/ /";

	name = (char *)sp->u.string;
	len = strlen(name)+13;

	if( len > 25 )
	{
        	*sp = const0;
        	return;
	}

	path[11] = name[0];

	len=13;
	while(*name) path[len++] = *name++;

	path[len] = '\0';

	ob = find_object2(path);

	free_string_svalue(sp);

    	if (ob && object_visible(ob))
		put_unrefed_undested_object(ob, "find_player");
    	else
		*sp = const0;
}
#endif

/*
  Added by Clode@RW 2001/10/8 05:37�U��
  �ΨӤ���@�Ӥ��ʪ��� input_to ���A
*/
#ifdef F_REMOVE_INPUT_TO
void f_remove_input_to ()
{
    object_t *ob;

    if (st_num_arg) {
	ob = sp->u.ob;
	pop_stack();
    }
    else ob = current_object;

    if( ob->interactive != 0 )
    {
    	ob->interactive->carryover = NULL;
    	ob->interactive->num_carry = 0;
    	ob->interactive->input_to = 0;
    }
}
#endif

/*
  �Τ@�]�w�t�� HEARTBEAT_INTERVAL ���
*/
/*
#ifdef F_SET_HEARTBEAT_INTERVAL
void f_set_heartbeat_interval()
{
	heartbeat_interval = sp->u.number;

	pop_stack();
}
#endif
*/
#ifdef F_BUFTOSTR
void f_buftostr()
{
    char *ret = new_string(sp->u.buf->size, "f_to_string");

    ret[sp->u.buf->size] = '\0';
    memcpy(ret, sp->u.buf->item, sp->u.buf->size);

    free_buffer(sp->u.buf);
    put_malloced_string(ret);
}
#endif

#ifdef F_STRTOBUF
void f_strtobuf()
{
    int len = SVALUE_STRLEN(sp);
    buffer_t *buf = allocate_buffer(len);

    memcpy(buf->item, sp->u.string, len);

    pop_stack();
    push_refed_buffer(buf);
}
#endif

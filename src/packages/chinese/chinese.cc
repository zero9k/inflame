#include "base/package_api.h"

void f_cwrap ()
{
	register int i, j;
	int width, slen, blanks;
	register char *istr, *ostr, *ret;

	if(st_num_arg >= 2)
	{
		if(st_num_arg == 3)
		{
			blanks = sp->u.number;
			pop_stack();

			if(blanks < 0)
				blanks = 0;
		}
		else
			blanks = 4;

		width = sp->u.number;
		pop_stack();

		if(width < 2)
			width = 64;

		if(width - blanks < 2)
		{
			width = 64;
			blanks = 4;
		}
	}
	else
	{
		blanks = 4;
		width = 64;
	}

	istr = (char *)sp->u.string;
	slen = SVALUE_STRLEN(sp);

	if(slen < blanks)
		blanks = 0;

	i = blanks;

	ostr = ret = new_string(slen+slen/width+blanks, "f_cwrap");

	while(blanks--)
		*ostr++ = ' ';

	while(*istr)
	{
		if(*istr == '\n')
		{
			istr++;
			continue;
		}

		if(istr[0] == 27 && istr[1] == '[')
		{
			j=2;

			while(istr[j] && (isdigit(istr[j]) || istr[j]==';'))
				j++;

			if(istr[j++] == 'm')
			{
				while(j--)
					*ostr++ = *istr++;

				continue;
			}
		}

		i++;
		*ostr++ = *istr++;

		if(is_B51((unsigned char)*(istr-1)))
		{
			i++;
			*ostr++ = *istr++;
		}
		else if((unsigned char)*(istr-1) == '\t')
			i += 4;

		if(i >= width)
		{
			*ostr++ = '\n';
			i = 0;
		}
	}

	*ostr = '\0';

	ostr = string_copy(ret, "f_cwrap");
	FREE_MSTR(ret);

	free_string_svalue(sp);
	put_malloced_string(ostr);
}

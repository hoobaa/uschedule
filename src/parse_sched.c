#include "scheduled.h"
#include "byte.h"
#include "scan.h"

int 
preparse_schedspec(struct schedinfo *si, const char *s, unsigned int len)
{
	unsigned int i;

	static unsigned char allowed[256];
	static int init_done=0;
	if (!init_done) {
		init_done++;
		allowed[',']=1; allowed['0']=1; allowed['1']=1;
		allowed['2']=1; allowed['3']=1; allowed['4']=1;
		allowed['5']=1; allowed['6']=1; allowed['7']=1;
		allowed['8']=1; allowed['9']=1;
	}

	si->Y=0; si->Ylen=0;
	si->M=0; si->Mlen=0;
	si->D=0; si->Dlen=0;
	si->h=0; si->hlen=0;
	si->m=0; si->mlen=0;
	si->s=0; si->slen=0;
	si->W=0;
	for (i=0;i<len;) {
		unsigned int j;
		int iswd=0;
		const char **p=0;
		unsigned int *pl=0;
		switch (s[i]) {
		case 'Y': p=&si->Y; pl=&si->Ylen; break; 
		case 'M': p=&si->M; pl=&si->Mlen; break; 
		case 'D': p=&si->D; pl=&si->Dlen; break; 
		case 'W': iswd=1; break; 
		case 'h': p=&si->h; pl=&si->hlen; break; 
		case 'm': p=&si->m; pl=&si->mlen; break; 
		case 's': p=&si->s; pl=&si->slen; break; 
		default: return 0;
		}
		i++;
		if (i==len)
			return 0;
		j=i;
		if (p)
			*p=s+j;
		while (j<len && allowed[(unsigned char)s[j]])
			j++;
		if (j==i)
			return 0;
		if (pl)
			*pl=j-i;
		if (iswd) {
			char nb[4];
			unsigned long ul;
			unsigned int k;
			if (j-i>sizeof(nb)-1) /* 3 characters are enough */
				return 0;
			byte_copy(nb,j-i,s+i);
			nb[j-i]=0;
			k=scan_ulong(nb,&ul);
			if (k!=j-i)
				return 0;
			if (ul>127)
				return 0;
			si->W=ul;
		}	
		i=j;
	}
	return 1;
}




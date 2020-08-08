/*

 * regex - Regular expression pattern matching  and replacement
 *
 * By:  Ozan S. Yigit (oz)
 *      Dept. of Computer Science
 *      York University
 *
 * These routines are the PUBLIC DOMAIN equivalents of regex
 * routines as found in 4.nBSD UN*X, with minor extensions.
 *
 * These routines are derived from various implementations found
 * in software tools books, and Conroy's grep. They are NOT derived
 * from licensed/restricted software.
 * For more interesting/academic/complicated implementations,
 * see Henry Spencer's regexp routines, or GNU Emacs pattern
 * matching module.
 *
 * const correctness patch by Tibor 'Igor2' Palinkas in 2009..2010
 * new subs code by Tibor 'Igor2' Palinkas in 2015
 */
#include <stdlib.h>
#include <string.h>
#include "regex.h"

#define MAXNFA  1024
#define MAXTAG  10

#define OKP     1
#define NOP     0

#define CHR     1
#define ANY     2
#define CCL     3
#define BOL     4
#define EOL     5
#define BOT     6
#define EOT     7
#define BOW	8
#define EOW	9
#define REF     10
#define CLO     11

#define END     0

/*
 * The following defines are not meant to be changeable.
 * They are for readability only.
 */
#define MAXCHR	128
#define CHRBIT	8
#define BITBLK	MAXCHR/CHRBIT
#define BLKIND	0170
#define BITIND	07

#define ASCIIB	0177

#ifdef NO_UCHAR
typedef char CHAR;
#else
typedef unsigned char CHAR;
#endif

static int  tagstk[MAXTAG];             /* subpat tag stack..*/
static CHAR nfa[MAXNFA];                /* automaton..       */
static int  sta = NOP;                  /* status of lastpat */

static CHAR bittab[BITBLK];             /* bit table for CCL */
                                        /* pre-set bits...   */
static CHAR bitarr[] = {1,2,4,8,16,32,64,128};

static void
chset(CHAR c)
{
	bittab[(CHAR) ((c) & BLKIND) >> 3] |= bitarr[(c) & BITIND];
}

#define badpat(x)	(*nfa = END, x)
#define store(x)	*mp++ = x

char *
re_comp(const char *pat)
{
	register const char *p;         /* pattern pointer   */
	register CHAR *mp=nfa;          /* nfa pointer       */
	register CHAR *lp;              /* saved pointer..   */
	register CHAR *sp=nfa;          /* another one..     */

	register int tagi = 0;          /* tag stack index   */
	register int tagc = 1;          /* actual tag count  */

	register int n;
	register CHAR mask;		/* xor mask -CCL/NCL */
	int c1, c2;

	if (!pat || !*pat) {
		if (sta)
			return 0;
		else
			return badpat("No previous regular expression");
	}
	sta = NOP;

	for (p = pat; *p; p++) {
		lp = mp;
		switch(*p) {

		case '.':               /* match any char..  */
			store(ANY);
			break;

		case '^':               /* match beginning.. */
			if (p == pat)
				store(BOL);
			else {
				store(CHR);
				store(*p);
			}
			break;

		case '$':               /* match endofline.. */
			if (!*(p+1))
				store(EOL);
			else {
				store(CHR);
				store(*p);
			}
			break;

		case '[':               /* match char class..*/
			store(CCL);

			if (*++p == '^') {
				mask = 0377;
				p++;
			}
			else
				mask = 0;

			if (*p == '-')		/* real dash */
				chset(*p++);
			if (*p == ']')		/* real brac */
				chset(*p++);
			while (*p && *p != ']') {
				if (*p == '-' && *(p+1) && *(p+1) != ']') {
					p++;
					c1 = *(p-2) + 1;
					c2 = *p++;
					while (c1 <= c2)
						chset((CHAR)c1++);
				}
#ifdef EXTEND
				else if (*p == '\\' && *(p+1)) {
					p++;
					chset(*p++);
				}
#endif
				else
					chset(*p++);
			}
			if (!*p)
				return badpat("Missing ]");

			for (n = 0; n < BITBLK; bittab[n++] = (char) 0)
				store(mask ^ bittab[n]);

			break;

		case '*':               /* match 0 or more.. */
		case '+':               /* match 1 or more.. */
			if (p == pat)
				return badpat("Empty closure");
			lp = sp;		/* previous opcode */
			if (*lp == CLO)		/* equivalence..   */
				break;
			switch(*lp) {

			case BOL:
			case BOT:
			case EOT:
			case BOW:
			case EOW:
			case REF:
				return badpat("Illegal closure");
			default:
				break;
			}

			if (*p == '+')
				for (sp = mp; lp < sp; lp++)
					store(*lp);

			store(END);
			store(END);
			sp = mp;
			while (--mp > lp)
				*mp = mp[-1];
			store(CLO);
			mp = sp;
			break;

		case '\\':              /* tags, backrefs .. */
			switch(*++p) {

			case '(':
				if (tagc < MAXTAG) {
					tagstk[++tagi] = tagc;
					store(BOT);
					store(tagc++);
				}
				else
					return badpat("Too many \\(\\) pairs");
				break;
			case ')':
				if (*sp == BOT)
					return badpat("Null pattern inside \\(\\)");
				if (tagi > 0) {
					store(EOT);
					store(tagstk[tagi--]);
				}
				else
					return badpat("Unmatched \\)");
				break;
			case '<':
				store(BOW);
				break;
			case '>':
				if (*sp == BOW)
					return badpat("Null pattern inside \\<\\>");
				store(EOW);
				break;
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				n = *p-'0';
				if (tagi > 0 && tagstk[tagi] == n)
					return badpat("Cyclical reference");
				if (tagc > n) {
					store(REF);
					store(n);
				}
				else
					return badpat("Undetermined reference");
				break;
#ifdef EXTEND
			case 'b':
				store(CHR);
				store('\b');
				break;
			case 'n':
				store(CHR);
				store('\n');
				break;
			case 'f':
				store(CHR);
				store('\f');
				break;
			case 'r':
				store(CHR);
				store('\r');
				break;
			case 't':
				store(CHR);
				store('\t');
				break;
#endif
			default:
				store(CHR);
				store(*p);
			}
			break;

		default :               /* an ordinary char  */
			store(CHR);
			store(*p);
			break;
		}
		sp = lp;
	}
	if (tagi > 0)
		return badpat("Unmatched \\(");
	store(END);
	sta = OKP;
	return 0;
}


static const char *bol;
const char *bopat[MAXTAG];
const char *eopat[MAXTAG];
static const char *pmatch(const char *, CHAR *, int *);

/*
 * re_exec:
 *	execute nfa to find a match.
 *
 *	special cases: (nfa[0])
 *		BOL
 *			Match only once, starting from the
 *			beginning.
 *		CHR
 *			First locate the character without
 *			calling pmatch, and if found, call
 *			pmatch for the remaining string.
 *		END
 *			re_comp failed, poor luser did not
 *			check for it. Fail fast.
 *
 *	If a match is found, bopat[0] and eopat[0] are set
 *	to the beginning and the end of the matched fragment,
 *	respectively.
 *
 */

int
re_exec(const char *lp)
{
	register CHAR c;
	register const char *ep = 0;
	register CHAR *ap = nfa;
	int score = 1;

	bol = lp;

	bopat[0] = 0;
	bopat[1] = 0;
	bopat[2] = 0;
	bopat[3] = 0;
	bopat[4] = 0;
	bopat[5] = 0;
	bopat[6] = 0;
	bopat[7] = 0;
	bopat[8] = 0;
	bopat[9] = 0;

	switch(*ap) {

	case BOL:			/* anchored: match from BOL only */
		ep = pmatch(lp,ap, &score);
		break;
	case CHR:			/* ordinary char: locate it fast */
		c = *(ap+1);
		while (*lp && *lp != c)
			lp++;
		if (!*lp)		/* if EOS, fail, else fall thru. */
			return 0;
	default:			/* regular matching all the way. */
#ifdef OLD
		while (*lp) {
			if ((ep = pmatch(lp,ap, &score)))
				break;
			lp++;
		}
#else	/* match null string */
		do {
			if ((ep = pmatch(lp,ap, &score)))
				break;
		} while (*lp++);
#endif
		break;
	case END:			/* munged automaton. fail always */
		return 0;
	}
	if (!ep)
		return 0;

	bopat[0] = lp;
	eopat[0] = ep;
	return score;
}

/*
 * pmatch: internal routine for the hard part
 *
 *	This code is partly snarfed from an early grep written by
 *	David Conroy. The backref and tag stuff, and various other
 *	innovations are by oz.
 *
 *	special case optimizations: (nfa[n], nfa[n+1])
 *		CLO ANY
 *			We KNOW .* will match everything up to the
 *			end of line. Thus, directly go to the end of
 *			line, without recursive pmatch calls. As in
 *			the other closure cases, the remaining pattern
 *			must be matched by moving backwards on the
 *			string recursively, to find a match for xy
 *			(x is ".*" and y is the remaining pattern)
 *			where the match satisfies the LONGEST match for
 *			x followed by a match for y.
 *		CLO CHR
 *			We can again scan the string forward for the
 *			single char and at the point of failure, we
 *			execute the remaining nfa recursively, same as
 *			above.
 *
 *	At the end of a successful match, bopat[n] and eopat[n]
 *	are set to the beginning and end of subpatterns matched
 *	by tagged expressions (n = 1 to 9).
 *
 */

#ifndef re_fail
extern void re_fail(char *, unsigned char);
#endif

/*
 * character classification table for word boundary operators BOW
 * and EOW. the reason for not using ctype macros is that we can
 * let the user add into our own table. see re_modw. This table
 * is not in the bitset form, since we may wish to extend it in the
 * future for other character classifications.
 *
 *	TRUE for 0-9 A-Z a-z _
 */
static CHAR chrtyp[MAXCHR] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
	0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 0, 0, 0, 0, 1, 0, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 0, 0, 0, 0, 0
	};

#define inascii(x)	(0177&(x))
#define iswordc(x)	chrtyp[inascii(x)]
#define isinset(x,y)	((x)[((y)&BLKIND)>>3] & bitarr[(y)&BITIND])

/*
 * skip values for CLO XXX to skip past the closure
 */

#define ANYSKIP	2	/* [CLO] ANY END ...	     */
#define CHRSKIP	3	/* [CLO] CHR chr END ...     */
#define CCLSKIP 18	/* [CLO] CCL 16bytes END ... */

static const char *
pmatch(const char *lp, CHAR *ap, int *score)
{
	register int op, c, n;
	register const char *e;		/* extra pointer for CLO */
	register const char *bp;		/* beginning of subpat.. */
	register const char *ep;		/* ending of subpat..	 */
	const char *are;			/* to save the line ptr. */

	while ((op = *ap++) != END)
		switch(op) {

		case CHR:
			if (*lp++ != *ap++)
				return 0;
			(*score) += 100;
			break;
		case ANY:
			if (!*lp++)
				return 0;
			(*score)++;
			break;
		case CCL:
			c = *lp++;
			if (!isinset(ap,c))
				return 0;
			ap += BITBLK;
			(*score) += 2;
			break;
		case BOL:
			if (lp != bol)
				return 0;
			(*score) += 10;
			break;
		case EOL:
			if (*lp)
				return 0;
			(*score) += 10;
			break;
		case BOT:
			bopat[*ap++] = lp;
			break;
		case EOT:
			eopat[*ap++] = lp;
			break;
		case BOW:
			if ((lp!=bol && iswordc(lp[-1])) || !iswordc(*lp))
				return 0;
			(*score) += 5;
			break;
		case EOW:
			if (lp==bol || !iswordc(lp[-1]) || iswordc(*lp))
				return 0;
			(*score) += 5;
			break;
		case REF:
			n = *ap++;
			bp = bopat[n];
			ep = eopat[n];
			while (bp < ep) {
				if (*bp++ != *lp++)
					return 0;
			(*score) += 2;
			}
			break;
		case CLO:
			are = lp;
			switch(*ap) {

			case ANY:
				while (*lp)
					lp++;
				n = ANYSKIP;
				(*score)++;
				break;
			case CHR:
				c = *(ap+1);
				while (*lp && c == *lp)
					lp++;
				n = CHRSKIP;
				(*score) += 100;
				break;
			case CCL:
				while ((c = *lp) && isinset(ap+1,c))
					lp++;
				n = CCLSKIP;
				(*score) += 2;
				break;
			default:
				re_fail("closure: bad nfa.", *ap);
				return 0;
			}

			ap += n;

			while (lp >= are) {
				e = pmatch(lp, ap, score);
				if (e)
					return e;
				--lp;
			}
			return 0;
		default:
			re_fail("re_exec: bad nfa.", op);
			return 0;
		}
	return lp;
}

/*
 * re_modw:
 *	add new characters into the word table to change re_exec's
 *	understanding of what a word should look like. Note that we
 *	only accept additions into the word definition.
 *
 *	If the string parameter is 0 or null string, the table is
 *	reset back to the default containing A-Z a-z 0-9 _. [We use
 *	the compact bitset representation for the default table]
 */

static CHAR deftab[16] = {
	0, 0, 0, 0, 0, 0, 0377, 003, 0376, 0377, 0377, 0207,
	0376, 0377, 0377, 007
};

void
re_modw(char *s)
{
	register int i;

	if (!s || !*s) {
		for (i = 0; i < MAXCHR; i++)
			if (!isinset(deftab,i))
				iswordc(i) = 0;
	}
	else
		while(*s)
			iswordc(*s++) = 1;
}

/* Substitute the matching part in the last re_exec call with sub. The
   result is returned in a newly allocated string. */
char *re_subs_dup(char *sub)
{
	char *dst;
	const char *end;
	int l1, l2, l3;
	end = bol + strlen(bol);
	l1 = bopat[0] - bol;
	if (sub != NULL)
		l2 = strlen(sub);
	else
		l2 = 0;
	l3 = end - eopat[0];
	if (l3 < 0)
		l3 = 0;
	dst = malloc(l1+l2+l3+1);
	memcpy(dst, bol, l1);
	if (l2 != 0)
		memcpy(dst+l1, sub, l2);
	memcpy(dst+l1+l2, eopat[0], l3+1);
	return dst;
}

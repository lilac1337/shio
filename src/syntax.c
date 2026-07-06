#include "syntax.h"

#include "filetypes.h"
#include "output.h"
#include "types.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

bool isseparator(int ch) {
    return isspace(ch) || ch == '\0' || strchr(",.()+-/*=~%<>[];", ch) != NULL;
}

void updatesyntax(row *r) {
    r->hl = realloc(r->hl, r->rsize);
    memset(r->hl, HL_NORMAL, r->rsize);

    if (c.syn == NULL)
        return;

    const char **kws = c.syn->kws;

    const char *ss = c.syn->ss;
    const char *scs = c.syn->slcmtst;
    const char *mcs = c.syn->mlcmtst;
    const char *mce = c.syn->mlcmtend;

    size_t sslen = ss ? strlen(ss) : 0ul;
    size_t scslen = scs ? strlen(scs) : 0ul;
    size_t mcslen = mcs ? strlen(mcs) : 0ul;
    size_t mcelen = mce ? strlen(mce) : 0ul;
    
    bool prevsep = true;
    bool incomment = (r->idx > 0 && c.r[r->idx - 1].hlmlcmt);
    char instring = 0;

    size_t i = 0;

    if (r->render == NULL) // useless check, but it shuts up the overzealous compiler
        goto error;

    while (i < r->rsize) {
        char ch = r->render[i];
        u32 prevhl = (i > 0) ? r->hl[i - 1] : HL_NORMAL;

        if (sslen && i == 0 && !incomment && !instring) {
            if (!strncmp(&r->render[i], ss, sslen)) {
                memset(&r->hl[i], HL_SPECIAL, r->rsize - i);
                break;
            }
        }

        if (scslen && !instring && !incomment) {
            if (!strncmp(&r->render[i], scs, scslen)) {
                memset(&r->hl[i], HL_COMMENT, r->rsize - i);
                break;
            }
        }

        if (incomment) {
            r->hl[i] = HL_MLCOMMENT;

            if (!strncmp(&r->render[i], mce, mcelen)) {
                memset(&r->hl[i], HL_MLCOMMENT, mcelen);
                incomment = false;
                prevsep = true;
                
                i += mcelen;
                continue;
            }

            ++i;
            continue;
        }

        if (!strncmp(&r->render[i], mcs, mcslen)) {
            memset(&r->hl[i], HL_MLCOMMENT, mcslen);
            incomment = true;
            
            i += mcslen;
            continue;
        }

        if (c.syn->flags & HLF_STRINGS) {
            if (instring) {
                r->hl[i] = HL_STRING;

                if (ch == '\\' && i + 1 < r->rsize) {
                    r->hl[i + 1] = HL_STRING;
                    
                    i += 2;
                    continue;
                }

                if (ch == instring)
                    instring = 0;

                prevsep = true;
                
                ++i;
                continue;
            }

            if (ch == '"' || ch == '\'') {
                instring = ch;
                r->hl[i] = HL_STRING;

                ++i;
                continue;
            }
        }
        
        if (c.syn->flags & HLF_NUMBERS) {
            if ((isdigit(ch) && (prevsep || prevhl == HL_NUMBER)) ||
                (ch == '.' && prevhl == HL_NUMBER)) {
                r->hl[i] = HL_NUMBER;
                prevsep = false;
                ++i;
            
                continue;
            }
        }

        if (prevsep) {
            size_t j;
            for (j = 0; kws[j]; ++j) {
                size_t klen = strlen(kws[j]);
                bool kw2 = kws[j][klen - 1u] == '|';

                if (kw2)
                    --klen;

                if (!strncmp(&r->render[i], kws[j], klen) &&
                    isseparator(r->render[i + klen])) {
                    memset(&r->hl[i], kw2 ? HL_KEYWORD2 : HL_KEYWORD1, klen);

                    i += klen;
                    break;
                }
            }

            if (kws[j] != NULL) {
                prevsep = false;
                continue;
            }
        }
            
        prevsep = isseparator(ch);
        ++i;
    }

    bool changed = (r->hlmlcmt != incomment);
    r->hlmlcmt = incomment;

    if (changed && r->idx + 1 < c.nrows)
        updatesyntax(&c.r[r->idx + 1]);

    return;

error:
    setstatus("r->render was NULL, couldn't update syntax");
}

u32 syntax2color(u32 hl) {
    switch (hl) {
    case HL_COMMENT:
    case HL_MLCOMMENT:return 36u;
    case HL_SPECIAL: return 33u;
    case HL_KEYWORD1: return 33u;
    case HL_KEYWORD2: return 32u;
    case HL_STRING: return 35u;
    case HL_NUMBER: return 31u;
    case HL_MATCH: return 34u;
    case HL_LINENUM: return 36u;
    default: return 37u;
    }
}

void selectsyntaxhl() {
    c.syn = NULL;

    if (c.fn == NULL)
        return;

    char *ext = strrchr(c.fn, '.');
    size_t j;

    for (j = 0; j < hldb_entries; ++j) {
        syntax *s = &hldb[j];
        size_t i = 0;

        while (s->fm[i]) {
            bool isext = (s->fm[i][0] == '.');

            if ((isext && ext && !strcmp(ext, s->fm[i])) ||
                (!isext && strstr(c.fn, s->fm[i]))) {
                c.syn = s;

                size_t frow;
                for (frow = 0; frow < c.nrows; ++frow) {
                    updatesyntax(&c.r[frow]);
                }

                return;
            }

            ++i;
        }
    }
}

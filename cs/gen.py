import re
def cstr(s):
    s=s.replace(' <-> ',' \x18 ').replace('<->','\x18').replace('->','\x1e')
    out=''
    for ch in s:
        o=ord(ch)
        if ch=='\\': out+='\\\\'
        elif ch=='"': out+='\\"'
        elif o<32 or o>126:
            if o>126: raise SystemExit('non-ascii: %r in %r'%(ch,s))
            out+='\\x%02x""'%o
        else: out+=ch
    return '"'+out+'"'
units=[]
for f in ['notes1.txt','notes2.txt','notes3.txt','notes4.txt']:
    for line in open(f):
        line=line.rstrip('\n')
        if not line.strip(): continue
        if line.startswith('=== '): units[-1][1][-1][1].append([line[4:],[]])
        elif line.startswith('== '): units[-1][1].append([line[3:],[]])
        elif line.startswith('= '): units.append([line[2:],[]])
        else: units[-1][1][-1][1][-1][1].append(line)
c=['#include "../common/core.h"','']
nid=0; tabs=[]
for ui,(un,tops) in enumerate(units):
    ttab=[]
    for ti,(tn,subs) in enumerate(tops):
        stab=[]
        for sn,lines in subs:
            nid+=1; fn='n%d'%nid
            c.append('static void %s(void) {'%fn)
            for l in lines:
                if l.startswith('# '): c.append('    out_line(HEAD %s);'%cstr(l[2:]))
                elif l.startswith('! '): c.append('    out_line(ANS %s);'%cstr('Exam tip: '+l[2:]))
                else: c.append('    out_line(%s);'%cstr(l))
            c.append('}')
            stab.append((sn,fn))
        arr='c_%d_%d'%(ui,ti)
        c.append('static const calc_t %s[] = {%s};'%(arr,', '.join('{%s, %s}'%(cstr(s),f) for s,f in stab)))
        ttab.append((tn,arr,len(stab)))
    c.append('const topic_t cs_u%d[] = {%s};'%(ui+1,', '.join('{%s, %s, %d}'%(cstr(t),a,n) for t,a,n in ttab)))
c.append('void bool_calc(void);')
c.append('static const calc_t bool_c[] = {{"Boolean algebra calculator", bool_calc}};')
c.append('static const topic_t bool_t[] = {{"Boolean algebra calculator", bool_c, 1}};')
c.append('void app_main(void) {')
c.append('    extern int ui_notes_mode; ui_notes_mode = 1;')
c.append('    static unit_t units[%d];'%(len(units)+1))
for ui,(un,tops) in enumerate(units):
    c.append('    units[%d] = (unit_t){%s, cs_u%d, %d};'%(ui,cstr(un),ui+1,len(tops)))
c.append('    units[%d] = (unit_t){"Boolean algebra calculator", bool_t, 1};'%len(units))
c.append('    app_run("Computer Science Notes", units, %d);'%(len(units)+1))
c.append('}')
open('notes.c','w').write('\n'.join(c)+'\n')
print(nid,'notes;',sum(len(t) for u in units for t in [u[1]]),'topics')

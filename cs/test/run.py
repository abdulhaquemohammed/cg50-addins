import subprocess, itertools, re
def norm(s):
    # calculator output -> python expr; bars only on single vars in answers
    s=s.replace('\x09','not ').replace('\x0d','')
    return s
def py(e):
    e=e.replace('!','not ').replace('.',' and ').replace('+',' or ')
    return e
def tt(e, vs):
    out=[]
    for bits in itertools.product([0,1],repeat=len(vs)):
        env=dict(zip(vs,bits)); out.append(bool(eval(e,{},env)))
    return out
bad=0
for line in open('cases.txt'):
    name,expr,exp=line.strip().split('|')
    r=subprocess.run(['../../desk/cs','5',':'+expr,'EXE','3','DUMP'],capture_output=True)
    r.stdout=r.stdout.decode('latin1')
    ans=[l for l in r.stdout.split('\n') if 'Simplified:' in l]
    if not ans: print('NO ANSWER',name,r.stdout[-200:]); bad+=1; continue
    a=ans[0].split('Simplified:')[1].strip(' ')
    a=a.upper(); a_py=a.replace('\x09','(not ').replace('\x0d',')').replace('.',' and ').replace('+',' or ')
    expr=expr.upper().replace('NOT','not'); exp=exp.upper()
    vs=sorted(set(re.findall('[A-Z]',expr+exp)))
    src=py(expr); e_py=py(exp)
    def fix(x): return re.sub(r'\b1\b','True',re.sub(r'\b0\b','False',x))
    t1=tt(fix(src),vs); t2=tt(fix(e_py),vs); t3=tt(fix(a_py),vs)
    lits=lambda s: len(re.findall('[A-Za-z]',s))
    ok_ms = t1==t2
    ok_me = t3==t1
    shown=a.replace('\x09','~').replace('\x0d','')
    flag='' if ok_me and lits(shown)<=lits(exp) else '  <<< CHECK'
    if not ok_ms: flag+='  (mark scheme/reconstruction disagree with original)'
    print(f'{name:10} ans={shown:20} ms={exp:15}{flag}')
    if flag: bad+=1
print('flags:',bad)

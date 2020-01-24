<int,KW> <fact5,1,ID> <;> 
<int,KW> <factorial,2,ID> <(> <int,KW> <n,3,ID> <)> <{> 
<int,KW> <val,4,ID> <;> 
<if,KW> <(> <n,3,ID> <>,OP> <1,NC> <)> <{> 
<val,4,ID> <=,OP> <n,3,ID> <*,OP> <factorial,2,ID> <(> <n,3,ID> <-,OP> <1,NC> <)> <;> 
<return,KW> <(> <val,4,ID> <)> <;> 
<}> 
<else,KW> <{> 
<return,KW> <1,NC> <;> 
<}> 
<}> 
<int,KW> <main,5,ID> <(> <)> <{> 
<printf,6,ID> <(> <"factorial program\n,SL> <)> <;> 
<fact5,1,ID> <=,OP> <factorial,2,ID> <(> <5,NC> <)> <;> 
<printf,6,ID> <(> <"fact5 = %d,SL> <,> <fact5,1,ID> <)> <;> 
<}> 
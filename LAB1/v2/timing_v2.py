import re
import time

data_list = {}
bad_var = set()
file = open("C:\\Users\\79192\\Desktop\\Automata_Theory\\LAB1\\file100k.txt",  "r")
t_work = []

for i in range(0,10):
    file = open("C:\\Users\\79192\\Desktop\\Automata_Theory\\LAB1\\file100k.txt",  "r")
    line_list = []
    while (True):
        line = file.readline()
        
        if not line:
            break
        if line[-1]=='\n':
            line = line[:-1]
        line_list.append(line)

    t_start = time.time_ns()
    for i in range(0,len(line_list)):
        match = re.fullmatch(r"(?P<string_number>[0-9]+) (?P<variable_type>((int)|(long)|(short)) )?(?P<variable>[a-zA-Z][a-zA-Z0-9]{,15})(=(?P<literal>(([a-zA-Z][a-zA-Z0-9]{,15})|([0-9]+))))?", line)
    t_end = time.time_ns()
    print(t_end-t_start)
    t_work.append(t_end-t_start)
print(sum(t_work)/10)
"""    if (match!=None):
        #print(line + " : Line is Correct!")
        var = match.group("variable")
        variable_type = match.group("variable_type")[:-1] if (match.group("variable_type")!=None) else 'int'
        if var not in data_list.keys():            
            data_list[var]=[(variable_type, match.group("string_number"))]
        else:
            if data_list[var][0][0]!=variable_type:
                bad_var.add(var)
            data_list[var].append((variable_type,match.group("string_number")))
    else:
        #print(line + " : Line is Incorrect!");
        count+=1
    
print('\nStatistic:\n')
for var in bad_var:
    for line in data_list[var]:
        print(line[1] + ' - ' + var + ' type: ' + line[0])
"""
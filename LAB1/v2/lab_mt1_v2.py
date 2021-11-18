import re

data_list = {}
bad_var = set()
file = open("C:\\Users\\79192\\Desktop\\Automata_Theory\\LAB1\\file100k.txt",  "r")
count = 0 ###
while (True):
    line = file.readline()
    if not line:
        break
    if line[-1]=='\n':
        line = line[:-1]
    match = re.fullmatch(r"(?P<string_number>[0-9]+) (?P<variable_type>((int)|(long)|(short)) )?(?P<variable>[a-zA-Z][a-zA-Z0-9]{,15})(=(?P<literal>(([a-zA-Z][a-zA-Z0-9]{,15})|([0-9]+))))?", line)
    if (match!=None):
        print(line + " : Line is Correct!")
        var = match.group("variable")
        variable_type = match.group("variable_type")[:-1] if (match.group("variable_type")!=None) else 'int'
        if var not in data_list.keys():            
            data_list[var]=[(variable_type, match.group("string_number"))]
        else:
            if data_list[var][0][0]!=variable_type:
                bad_var.add(var)
            data_list[var].append((variable_type,match.group("string_number")))
    else:
        print(line + " : Line is Incorrect!");
        count+=1
    
print('\nStatistic:\n')
for var in bad_var:
    for line in data_list[var]:
        print(line[1] + ' - ' + var + ' type: ' + line[0])

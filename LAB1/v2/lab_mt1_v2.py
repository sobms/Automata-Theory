import re

data_list = {}
bad_var = set()
input_string = input()
while (len(input_string)):
    match = re.fullmatch(r"(?P<string_number>[0-9]+) (?P<variable_type>((int)|(long)|(short)) )?(?P<variable>[a-zA-Z][a-zA-Z0-9]{,15})(=(?P<literal>(([a-zA-Z][a-zA-Z0-9]{,15})|([0-9]+))))?", input_string)
    if (match!=None):
        print("Line is Correct!")
        var = match.group("variable")
        variable_type = match.group("variable_type")[:-1] if (match.group("variable_type")!=None) else 'int'
        if var not in data_list.keys():            
            data_list[var]=[(variable_type, match.group("string_number"))]
        else:
            if data_list[var][0][0]!=variable_type:
                bad_var.add(var)
            data_list[var].append((variable_type,match.group("string_number")))
    else:
        print("Line is Incorrect!");
    input_string = input()
for var in bad_var:
    for line in data_list[var]:
        print(line[1] + ' - ' + var)
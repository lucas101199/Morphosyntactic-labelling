f = open("corpus_pos_train.txt", "r")

data = open("basic_pos.data", "w")

lines = f.readlines()
new = [x for x in lines if x != '\n']

for i in range(0, len(new)):
    if "_,_" in new[i]:
        new[i] = new[i].replace("_,_", "_._")

    l = new[i].split('\t')
    if '.' in l:
        new[i] = new[i].replace('.', "!POINT")
    if ',' in l:
        new[i] = new[i].replace(',', "!VIRGULE")

for i in range(0, len(new)):
    line = new[i].split('\t')
    line[2] = line[2][:-1]
    index = line[0]
    word = line[1]
    pos = line[2]
    if i == 0:
        line_to_print = index + ' , ' + 'XX' + ' , ' + word + ' , ' + new[i + 1].split('\t')[1] + ' , ' + pos + ' .'
        data.write(line_to_print + '\n')
    elif i == len(new)-1:
        line_to_print = index + ' , ' + new[i - 1].split('\t')[1] + ' , ' + word + ' , ' + 'XX' + ' , ' + pos + ' .'
        data.write(line_to_print + '\n')
    else:
        line_to_print = index + ' , ' + new[i-1].split('\t')[1] + ' , ' + word + ' , ' + new[i+1].split('\t')[1] + ' , ' + pos + ' .'
        data.write(line_to_print + '\n')

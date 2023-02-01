def find_time(time):
    new_time = 1224 / time
    if time < 0:
        new_time = -1 * new_time * 1.5
    return int(new_time)


# reads all notes from note.txt and saves them to notes_dic
notes_dic = {}
with open('note.txt') as f:
    for line in f.readlines():
        temp = line.split()
        notes_dic[temp[1]] = int(temp[2])
print(notes_dic)

# reads from melody.txt and generates defined struct, all comments and empty lines should be deleted beforehand
print('{')
with open('harrypotter.txt') as f:
    for line in f.readlines():
        temp = [x.strip() for x in line.split(',')]
        i = 0
        for x in range(len(temp)):
            print('{' + str(notes_dic[temp[i]]) + ',' + str(find_time(int(temp[i + 1]))) + '},')
            i = i + 2
            if i + 1 > len(temp) - 1:
                break
print('};')

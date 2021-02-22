
#import os
#DIR = os.path.dirname(os.path.abspath(__file__))

headers = 'stdio '

headers += input('headers: %s'%headers)
headers = list(map(lambda h:h.strip(), headers.strip().split(' ')))

line_num = 1
source = ''

def print_line(line):
	global line_num
	print('%2s |'%line_num, line)
	
def add_line(line):
	global source, line_num
	source += line + '\n'
	print_line(line)
	line_num += 1

for h in headers:
	add_line('#include <%s.h>' % h)

add_line('void main() {')

while True:
	line = input('%2s |    '%line_num)
	source += line + '\n'
	line_num += 1
	
	if line.strip() == '':
		break

line = '}'
add_line(line)

with open('ccli_temp.c', 'w') as f:
	f.write(source)

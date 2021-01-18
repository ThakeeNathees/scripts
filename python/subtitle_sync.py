from datetime import timedelta

### EDIT THESE 3 VARIABLES ###################################
SUBTITLE_FILE_PATH = "subtitle_file_path.srt"
OUTPUT_FILE_PATH   = "output_file_path.srt"
DELAY_MILLI_SEC = 3200

## Copyright (c) 2020 Thakee Nathees
##############################################################

output = ""; delim = ' --> '
def str_to_time(t_str):
    t = t_str.split(',')
    return [int(x) for x in t[0].split(':')] + [int(t[1])]

def add_delay(time):
    h, m, s, ms = time
    t1 = timedelta(hours=h, minutes=m, seconds=s, milliseconds=ms)
    t2 = timedelta(milliseconds=DELAY_MILLI_SEC)
    td = t1+t2
    
    hours, remainder = divmod(td.seconds, 3600)
    minutes, seconds = divmod(remainder, 60)
    millisec = int(td.microseconds/1000)
    return "%s:%s:%s,%s"%(hours, minutes, seconds, millisec)
    

with open(SUBTITLE_FILE_PATH, 'r') as file_subtitle:
    for line in file_subtitle.readlines():
        if delim in line:
            times = line.strip().split(delim)
            t_from = str_to_time(times[0])
            t_to = str_to_time(times[1])
            output += add_delay(t_from) + delim + add_delay(t_to) + '\n'
        else:
            output += line
with open(OUTPUT_FILE_PATH, 'w') as out_file:
    out_file.write(output)
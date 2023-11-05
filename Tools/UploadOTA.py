#!python3
import hashlib
import sys
import subprocess
import winsound
import time

if len (sys.argv) < 3:
  print ('Missing command line arguments [file, url]')
  exit (1)

File = sys.argv[1]
Url  = f'http://{sys.argv[2]}/update'

with open(File, 'rb') as fi:
    data = fi.read()
    md5 = hashlib.md5(data).hexdigest()

winsound.Beep (250, 300)
winsound.Beep (350, 300)

p = subprocess.Popen(f'curl --compressed -L -X POST -F "MD5={md5}" -F "name=firmware" -F "data=@{File};filename=firmware" "{Url}"', shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
retval = p.wait()

LastLine = ''
for line in p.stdout.readlines():
    LastLine = line

if LastLine.decode () == 'OK':
    winsound.Beep (250, 300)
    winsound.Beep (350, 300)
    winsound.Beep (450, 300)
    winsound.Beep (550, 300)
    winsound.Beep (650, 300)
    winsound.Beep (750, 300)
    print ('###############################')
    print ('####    Upload complete    ####')
    print ('###############################')
    exit (0)
else:
    winsound.Beep (550, 300)
    winsound.Beep (450, 300)
    winsound.Beep (350, 300)
    winsound.Beep (250, 300)

print (LastLine.decode ())
exit (1)



#!python
import winsound
import sys

status = ''
if len (sys.argv) > 1:
    status = sys.argv[1]

match status:
    case 'UPLOAD':
        winsound.Beep (250, 250)
        winsound.Beep (520, 400)
    case 'OK':
        winsound.Beep (450, 250)
        winsound.Beep (550, 250)
        winsound.Beep (650, 250)
        winsound.Beep (750, 250)
    case 'FAIL':
        winsound.Beep (550, 400)
        winsound.Beep (450, 400)
        winsound.Beep (350, 400)
        winsound.Beep (250, 400)
    case _:
        winsound.Beep (450, 1000)

exit (0)



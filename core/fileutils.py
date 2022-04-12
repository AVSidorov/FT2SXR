import datetime
import os


def work_dir(path=None):
    if path is None:
        wdir = os.path.abspath('./')
    else:
        wdir = os.path.abspath(path)

    return wdir


def today_dir(path=None):
    td = datetime.date.today()
    td_dir = format(td.year-2000, '02d')+format(td.month, '02d')+format(td.day, '02d')
    if path is None:
        path = os.path.abspath('./')

    td_dir = os.path.join(path, td_dir)
    if not(os.path.exists(td_dir)):
        os.mkdir(td_dir)

    return td_dir

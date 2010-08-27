#! python
# -*- coding: utf-8 -*-
# (c) 2010 Werner Mayer GPL

Usage = """updatets - update all .ts files found in the source directories

Usage:
   updatets 
   
Autor:
  (c) 2010 Werner Mayer
  Licence: GPL

Version:
  0.1
"""

import os, re

DirFilter = ["^Attic$",
             "^CVS$",
             "^\\.svn$",
             "^\\.deps$",
             "^\\.libs$"]
             
QMAKE = ""
LUPDATE = ""

def find_tools():
    global QMAKE, LUPDATE
    if (os.system("qmake-qt4 -v") == 0):
        QMAKE = "qmake-qt4"
    elif (os.system("qmake -v") == 0):
        QMAKE = "qmake"
    else
        raise Exception("Cannot find qmake")
    if (os.system("lupdate-qt4 -v") == 0):
        QMAKE = "lupdate-qt4"
    elif (os.system("lupdate -v") == 0):
        QMAKE = "lupdate"
    else
        raise Exception("Cannot find lupdate")

def filter_dirs(item):
    global DirFilter
    if not os.path.isdir(item):
        return False
    for regexp in DirFilter:
        a = re.compile(regexp)
        if (re.match(a, item)):
            return False
    return True

def update_translation(path):
    global QMAKE, LUPDATE
    cur = os.getcwd()
    os.chdir(path)
    filename = os.path.basename(path) + ".pro"
    os.system(QMAKE + " -project")
    os.system(LUPDATE + " " + filename)
    #os.remove(filename)
    os.chdir(cur)

def main():
    find_tools()
    dirs=os.listdir("src/Mod")
    for i in range(len(dirs)):
        dirs[i] = "src/Mod/" + dirs[i]
    dirs.append("src/Base")
    dirs.append("src/App")
    dirs.append("src/Gui")
    dirs = filter(filter_dirs, dirs)
    for i in dirs:
        update_translation(i)

if __name__ == "__main__":
    os.chdir(os.path.dirname(__file__))
    os.path.chdir("..")
    os.path.chdir("..")
    main()


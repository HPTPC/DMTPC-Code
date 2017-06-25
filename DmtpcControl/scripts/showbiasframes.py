#!/usr/bin/python
import os

# http://stackoverflow.com/questions/4500564/directory-listing-based-on-time
def sorted_ls(path):
    mtime = lambda f: os.stat(os.path.join(path, f)).st_mtime
    return list(sorted(os.listdir(path), key=mtime))

def main():
    print "Content-type: text/html\n\n"
    print "<html><head><title>title</title></head>"
    print "<body>"

    path='../html/biasframes/'
    #        print '<img width=100% src=\"../biasframes/'+file+'\" alt=\"'+file+'\" /><br>'

    files = sorted_ls(path)
    files.reverse()
    #    print files

    nfiles=0
    for file in files:
        print '<img width=100% src=\"../biasframes/'+file+'\" alt=\"'+file+'\" /><br>'
        nfiles+=1
        if nfiles>24:
            break

    print "</body>"
    print "</html>"


if __name__ == "__main__":
    main()

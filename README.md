tt - typing timer/tutor/test
============================
`tt` is an extremely basic tool for practicing touch typing, and monitoring your
typing speed using ASCII documents provided by the user.

![tt demo](https://raw.githubusercontent.com/runrin/tt/gh-pages/tt-demo.gif)

Requirements
------------
In order to build `tt` you need `ncurses`.

Installation
------------
Edit `config.mk` to match your local setup (`tt` is installed into the
/usr/local namespace by default).

Afterwards enter the following command to build and install `tt` (if
necessary as root):
        
    make install

Running tt
----------
Running `tt` requires a formatted text document to work correctly. The document
must be encoded with ASCII and have lines of 80 or fewer characters in length
with no blank lines.

here is the one-liner that I use:

    iconv -f utf-8 -t ascii//translit foo.txt | sed 's/--/-/g' \
            | sed '/^$/d' | sed 's/^[ \t]*//' | sed 's/\(^.\{1,80\}\).*/\1/' \
            > bar.txt

Usage
-----
    tt /path/to/ascii/file.txt

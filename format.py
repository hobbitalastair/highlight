#!/usr/bin/env python3

import sys
import subprocess
import html

C_END       = '\033[0m'
C_BOLD      = '\033[1m'
C_GREY      = '\033[2m'
C_REVERSE   = '\033[7m'
C_UNDERLINE = '\033[4m'
C_BLACK     = '\033[30m'
C_RED       = '\033[91m'
C_GREEN     = '\033[92m'
C_YELLOW    = '\033[93m'
C_BLUE      = '\033[94m'
C_WHITE      = '\033[97m'
C_WHITEBG   = '\033[107m'

colours = {
    "string":       C_BLUE,
    "comment":      C_REVERSE,
    "identifier":   C_END,
}
C_DEFAULT = C_WHITE + C_BOLD

def format_term(tokens):
    for name, orig in tokens:
        colour = C_DEFAULT
        if name in colours:
            colour = colours[name]

        text = ('\n' + colour).join(orig.split('\n'))
        print("{}{}{}".format(colour, text, C_END), end='')


def format_html(tokens):
    print("<code><pre><b>")
    for name, orig in tokens:
        text = html.escape(orig)
        if name == 'string':
            print("</b><i>{}</i><b>".format(text), end='')
        elif name == 'comment':
            print("</b><mark>{}</mark><b>".format(text), end='')
        elif name== 'identifier':
            print("</b>{}<b>".format(text), end='')
        else:
            print("{}".format(text), end='')
    print("<b></pre></code>")


if __name__ == "__main__":
    enc = 'ascii'
    if len(sys.argv) == 3:
        formatter = 'term'
        lexer = sys.argv[1]
        path = sys.argv[2]
    elif len(sys.argv) == 4:
        formatter = sys.argv[1]
        lexer = sys.argv[2]
        path = sys.argv[3]
    else:
        print("usage: {} [format] lexer file".format(sys.argv[0]))
        quit(1)

    if formatter == 'term':
        format_method = format_term
    elif formatter == 'html':
        format_method = format_html
    else:
        print("formats are html, term")
        quit(1)

    with open(path) as f:
        tokens = subprocess.run([lexer], stdin=f, stdout=subprocess.PIPE,
                        check=True, timeout=2
                    ).stdout.decode(enc).split('\n')

    parsed_tokens = []
    with open(path) as orig:
        for line in tokens:
            name, length = line.split(' ')
            parsed_tokens.append((name, orig.read(int(length))))

    format_method(parsed_tokens)


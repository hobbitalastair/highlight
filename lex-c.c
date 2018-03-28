/* lex-c.c
 *
 * Author:  Alastair Hughes
 * Contact: hobbitalastair at yandex dot com
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <regex.h>

#define NAME            __FILE__
#define ERR_BUFSIZE     200
#define BUF_SIZEINCR    4096

typedef struct {
    char* ptr;
    size_t size;
    size_t length;
} Buf;

typedef struct {
    const char* name;
    const char* regex;
    regex_t re;
} Token;

Token tokens[] = {
    {"if"},
    {"for"},
    {"while"},
    {"do"},
    {"typedef"},
    {"struct"},
    {"#define"},
    {"#include"},
    {"("},
    {")"},
    {"[", "\\["},
    {"]", "\\]"},
    {"{"},
    {"}"},
    {"\\n", "\n"},
    {";"},
    {","},
    {".", "\\."},
    {"="},
    {"&"},
    {"*", "\\*"},
    {"&&"},
    {"||"},
    {"!"},
    {">"},
    {"<"},
    {"<="},
    {">="},
    {"-"},
    {"--"},
    {"+"},
    {"++"},
    {"/"},
    {"whitespace", "[ \t]*"},
    {"comment", "//[^\n]*"},
    {"comment", "/\\*[^*]*\\(\\**[^*/][^*]*\\)*\\**\\*/"},
    {"identifier", "[_a-zA-Z][_a-zA-Z0-9]*"},
    {"string", "\"\\([^\\\\\"]*\\(\\\\.\\)*\\)*\""},
    {"character", "'[^']'"},
    {"character", "'\\\\.'"},
    {"integer", "[0-9][0-9]*"},
    {"float", "[0-9][0-9]*.[0-9][0-9]*"},
};
#define TOK_COUNT (sizeof(tokens)/sizeof(Token))

void compile() {
    for (size_t i = 0; i < TOK_COUNT; i++) {
        if (!tokens[i].regex) tokens[i].regex = tokens[i].name;
        int ret = regcomp(&tokens[i].re, tokens[i].regex, 0);
        if (ret) {
            char errbuf[ERR_BUFSIZE] = {0};
            regerror(ret, &tokens[i].re, errbuf, sizeof(errbuf));
            fprintf(stderr, "%s: compiling %s failed: %s\n", NAME, tokens[i].regex, errbuf);
            exit(1);
        }
    }
}

Buf load_stdin() {
    Buf b = {0};
    ssize_t count = 0;
    do {
        size_t new_size = b.size;
        while (count + b.length >= new_size) {
            new_size += BUF_SIZEINCR;
        }
        if (new_size > b.size) {
            char* new_ptr = realloc(b.ptr, new_size);
            if (!new_ptr) {
                fprintf(stderr, "%s: realloc(): %s\n", NAME, strerror(errno));
                exit(1);
            }
            b.ptr = new_ptr;
            b.size = new_size;
        }
        
        count = read(0, b.ptr + b.length, b.size - b.length);
        b.length += count;
    } while (count > 0);
    if (count < 0) {
        fprintf(stderr, "%s: read(): %s\n", NAME, strerror(errno));
        exit(1);
    }

    b.ptr[b.length] = '\0'; /* Buf should always be large enough */
    return b;
}

int main() {
    compile();
    Buf b = load_stdin();

    size_t offset = 0;
    size_t unmatched = 0;
    while (offset < b.length) {
        ssize_t max_match = -1;
        size_t max_len = 0;
        for (ssize_t i = 0; i < TOK_COUNT; i++) {
            regmatch_t match;
            int ret = regexec(&tokens[i].re, b.ptr + offset, 1, &match, 0);
            if (ret != REG_NOMATCH && match.rm_so == 0 && match.rm_eo > max_len) {
                max_match = i;
                max_len = match.rm_eo;
            }
        }
        if (max_match == -1) {
            unmatched++;
            offset++;
        } else {
            if (unmatched > 0) {
                printf("unmatched %ld\n", unmatched);
                unmatched = 0;
            }
            printf("%s %ld\n", tokens[max_match].name, max_len);
            offset += max_len;
        }
    }
    printf("eof 0");
}

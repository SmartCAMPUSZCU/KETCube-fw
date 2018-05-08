1. Writing text files
---------------------

Please limit all text files line len to max. 80 characters:

Fold USAGE:
  fold -w 80 -s FILENAME


2. Writing C code
-----------------

KETCube uses the Kernighan & Ritchie style (plus minor enhancements). 
Please, use GNU Indent to enforce coding style.

The K&R coding style applies to KETCube code only. The third-party code 
integrated with KETCube must not necessairly follow this coding style.

To enforce K&R coding style, use GNU/indent.

From GNU Indent manual:

The Kernighan & Ritchie style is used throughout their well-known book The C 
Programming Language. It is enabled with the ‘-kr’ option. The Kernighan & 
Ritchie style corresponds to the following set of options:

-nbad -bap -bbo -nbc -br -brs -c33 -cd33 -ncdb -ce -ci4 -cli0
-cp33 -cs -d0 -di1 -nfc1 -nfca -hnl -i4 -ip0 -l75 -lp -npcs
-nprs -npsl -saf -sai -saw -nsc -nsob -nss

Additionally, following options are used in KETCube code:
-nut

Notes:
Prior to use indent on Linux, please convert windows end-lines (CR+LF)
to unix (LF):
  dos2unix FILENAME

Indent USAGE for KETCube code:
  indent -kr -nut FILENAME


For details, see 
[https://www.gnu.org/software/indent/manual/indent.pdf](https://www.gnu.org/software/indent/manual/indent.pdf)

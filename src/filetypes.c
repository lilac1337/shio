#include "filetypes.h"

const char *c_hl_ext[] = { ".c", ".h", ".cpp", ".cxx", "hxx", ".hpp", NULL };
const char *c_hl_kws[] = { "switch", "if", "while", "for", "break", "continue", "return",
                           "else", "struct", "union", "typedef", "static", "enum", "case",
                           "const", "constexpr", "default", "extern", "false", "true", "inline",
                           "goto", "nullptr", "register", "restrict", "sizeof", "static_assert",
                           "thread_local", "typeof", "typeof_unequal",
                           "volatile",
                           
                           "int|", "long|", "double|", "float|", "char|", "unsigned|", "signed|",
                           "void|", "bool|", "auto|", "_Atomic|", "_Complex|", "_Decimal32|",
                           "_Decimal64|", "_Decimal128|", "_Generic|", "_Imaginary|", NULL };

syntax hldb[] = {
    { "c", c_hl_ext, c_hl_kws, "#", "//", "/*", "*/", HLF_NUMBERS | HLF_STRINGS },
}; 

const size_t hldb_entries = sizeof(hldb) / sizeof(hldb[0]);

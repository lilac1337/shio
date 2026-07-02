#include "filetypes.h"

const char *c_hl_ext[] = { ".c", ".h", ".cpp", ".cxx", "hxx", ".hpp", NULL };
const char *c_hl_kws[] = { "switch", "if", "while", "for", "break", "continue", "return",
                           "else", "struct", "union", "typedef", "static", "enum", "case",
                           
                           "int|", "long|", "double|", "float|", "char|", "unsigned|", "signed|",
                           "void|", NULL };

syntax hldb[] = {
    { "c", c_hl_ext, c_hl_kws, "#", "//", "/*", "*/", HLF_NUMBERS | HLF_STRINGS },
}; 

const size_t hldb_entries = sizeof(hldb) / sizeof(hldb[0]);

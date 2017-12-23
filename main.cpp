#include <iostream>
#include <wchar.h>
#include <string>

#include "Parser.h"
#include "Scanner.h"


//! @cond fileScope
//
//  Create by copying str - only used locally
inline static wchar_t* coco_string_create(const wchar_t* str)
{
    const int len = wcslen(str);
    wchar_t* dst = new wchar_t[len + 1];
    wcsncpy(dst, str, len);
    dst[len] = 0;
    return dst;
}


// Free storage and nullify the argument
inline static void coco_string_delete(wchar_t* &str)
{
    delete[] str;
    str = NULL;
}
//
//! @endcond


int main (int argc, char *argv[]) {
    if (argc > 1 ) {
        int len = strlen(argv[1]);
        std::wstring wc(len, L'#');
        mbstowcs( &wc[0], argv[1], len);
        wchar_t *fileName = coco_string_create(&wc[0]);
        Scanner *scanner = new Scanner(fileName);
        Parser *parser = new Parser(scanner);
        coco_string_delete(fileName);
        parser->Parse();

        int errors = parser->errors->count;

        delete parser;
        delete scanner;

        return errors == 0;
    } else {
        std::cout << "No source file" << std::endl;
    }
    return 1;
}


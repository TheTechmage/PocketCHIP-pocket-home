/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#ifndef BINARYDATA_H_30357753_INCLUDED
#define BINARYDATA_H_30357753_INCLUDED

namespace BinaryData
{
    extern const char*   bluetooth_json;
    const int            bluetooth_jsonSize = 1119;

    extern const char*   config_json;
    const int            config_jsonSize = 1639;

    extern const char*   LatoRegular_ttf;
    const int            LatoRegular_ttfSize = 120196;

    extern const char*   wifi_json;
    const int            wifi_jsonSize = 1167;

    // Points to the start of a list of resource names.
    extern const char* namedResourceList[];

    // Number of elements in the namedResourceList array.
    const int namedResourceListSize = 4;

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding data and its size (or a null pointer if the name isn't found).
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes) throw();
}

#endif

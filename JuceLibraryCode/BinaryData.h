/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#ifndef BINARYDATA_H_30357753_INCLUDED
#define BINARYDATA_H_30357753_INCLUDED

namespace BinaryData
{
    extern const char*   appsIcon_svg;
    const int            appsIcon_svgSize = 1318;

    extern const char*   appsIcon_sel_svg;
    const int            appsIcon_sel_svgSize = 1318;

    extern const char*   gamesIcon_svg;
    const int            gamesIcon_svgSize = 1316;

    extern const char*   gamesIcon_sel_svg;
    const int            gamesIcon_sel_svgSize = 1316;

    extern const char*   settingsIcon_svg;
    const int            settingsIcon_svgSize = 1593;

    extern const char*   settingsIcon_sel_svg;
    const int            settingsIcon_sel_svgSize = 1593;

    // Points to the start of a list of resource names.
    extern const char* namedResourceList[];

    // Number of elements in the namedResourceList array.
    const int namedResourceListSize = 6;

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding data and its size (or a null pointer if the name isn't found).
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes) throw();
}

#endif

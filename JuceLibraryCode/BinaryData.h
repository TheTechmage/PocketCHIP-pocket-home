/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#ifndef BINARYDATA_H_30357753_INCLUDED
#define BINARYDATA_H_30357753_INCLUDED

namespace BinaryData
{
    extern const char*   appsIcon_svg;
    const int            appsIcon_svgSize = 1323;

    extern const char*   appsIconSel_svg;
    const int            appsIconSel_svgSize = 1318;

    extern const char*   backIcon_svg;
    const int            backIcon_svgSize = 846;

    extern const char*   bluetoothIcon_svg;
    const int            bluetoothIcon_svgSize = 993;

    extern const char*   brightnessIconHi_svg;
    const int            brightnessIconHi_svgSize = 1757;

    extern const char*   brightnessIconLo_svg;
    const int            brightnessIconLo_svgSize = 1781;

    extern const char*   check_svg;
    const int            check_svgSize = 626;

    extern const char*   gamesIcon_svg;
    const int            gamesIcon_svgSize = 1319;

    extern const char*   gamesIconSel_svg;
    const int            gamesIconSel_svgSize = 1316;

    extern const char*   lock_svg;
    const int            lock_svgSize = 854;

    extern const char*   settingsIcon_svg;
    const int            settingsIcon_svgSize = 1585;

    extern const char*   settingsIconSel_svg;
    const int            settingsIconSel_svgSize = 1593;

    extern const char*   volumeIconHi_svg;
    const int            volumeIconHi_svgSize = 1162;

    extern const char*   volumeIconLo_svg;
    const int            volumeIconLo_svgSize = 699;

    extern const char*   wifiIcon_svg;
    const int            wifiIcon_svgSize = 1306;

    // Points to the start of a list of resource names.
    extern const char* namedResourceList[];

    // Number of elements in the namedResourceList array.
    const int namedResourceListSize = 15;

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding data and its size (or a null pointer if the name isn't found).
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes) throw();
}

#endif

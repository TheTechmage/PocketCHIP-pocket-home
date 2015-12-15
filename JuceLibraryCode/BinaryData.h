/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#ifndef BINARYDATA_H_30357753_INCLUDED
#define BINARYDATA_H_30357753_INCLUDED

namespace BinaryData
{
    extern const char*   appsIcon_png;
    const int            appsIcon_pngSize = 2391;

    extern const char*   backIcon_png;
    const int            backIcon_pngSize = 629;

    extern const char*   backIcon_svg;
    const int            backIcon_svgSize = 846;

    extern const char*   bluetoothIcon_svg;
    const int            bluetoothIcon_svgSize = 993;

    extern const char*   brightnessIconHi_png;
    const int            brightnessIconHi_pngSize = 1094;

    extern const char*   brightnessIconLo_png;
    const int            brightnessIconLo_pngSize = 803;

    extern const char*   check_svg;
    const int            check_svgSize = 626;

    extern const char*   gamesIcon_png;
    const int            gamesIcon_pngSize = 2375;

    extern const char*   lock_svg;
    const int            lock_svgSize = 894;

    extern const char*   settingsIcon_png;
    const int            settingsIcon_pngSize = 2098;

    extern const char*   volumeIconHi_png;
    const int            volumeIconHi_pngSize = 963;

    extern const char*   volumeIconLo_png;
    const int            volumeIconLo_pngSize = 325;

    extern const char*   wifiIcon_svg;
    const int            wifiIcon_svgSize = 1306;

    extern const char*   wifiStrength0_svg;
    const int            wifiStrength0_svgSize = 1010;

    extern const char*   wifiStrength1_svg;
    const int            wifiStrength1_svgSize = 1010;

    extern const char*   wifiStrength2_svg;
    const int            wifiStrength2_svgSize = 1010;

    extern const char*   wifiStrength3_svg;
    const int            wifiStrength3_svgSize = 1010;

    // Points to the start of a list of resource names.
    extern const char* namedResourceList[];

    // Number of elements in the namedResourceList array.
    const int namedResourceListSize = 17;

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding data and its size (or a null pointer if the name isn't found).
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes) throw();
}

#endif

#include <string>
#include <iostream>

#include "DT5742.h"
#include "CaenError.h"

int main(void) {

    try {
        DT5742 digitizer(0, 1000, false);

        digitizer.Setup("input.txt");

        digitizer.Print();

        digitizer.Capture();

        digitizer.Summary();

        digitizer.Close();

    } catch (CaenError error) {
        error.print_error();
    }

    return 0;
}

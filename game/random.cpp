#include "libgame.h"
#include "random.h"
#include <stdlib.h>

void random_setup()
{
    srand((int)millis());
}

#include "Config.hpp"
#include "RenderFrontends/RenderContext.hpp"
#include "RenderFrontends/SfmlRenderer.hpp"
#include "Timer.hpp"

int main () {

    InitTimers (TIMERS_COUNT);

    RenderContext context = {};
    InitRenderContext (&context);

    SfmlRenderCycle (&context);   

    DestroyRenderContext (&context);
    DestroyTimers ();

    return 0;
}

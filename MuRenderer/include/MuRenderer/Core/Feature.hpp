#ifndef MURENDERER_CORE_FEATURE_HPP
#define MURENDERER_CORE_FEATURE_HPP


namespace murenderer
{
    #ifdef MU_MULTIGPU
        #define MU_MULTIGPU_FEATURE 1
    #else
        #define MU_MULTIGPU_FEATURE 0
    #endif
}

#endif // !MURENDERER_CORE_FEATURE_HPP

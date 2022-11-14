add_library(cheat-console SHARED
    DeathValley.cpp
    # It was easier to just compile Detours as part of the target...
    ${EXTERN}/Detours/src/creatwth.cpp
    ${EXTERN}/Detours/src/detours.cpp
    ${EXTERN}/Detours/src/disasm.cpp
    ${EXTERN}/Detours/src/disolx86.cpp
    ${EXTERN}/Detours/src/image.cpp
    ${EXTERN}/Detours/src/modules.cpp
)

target_include_directories(cheat-console
    PUBLIC ${EXTERN}/Detours/src
)

target_compile_options(cheat-console
    PUBLIC "/W4"
)

set_target_properties(cheat-console
    # We'll want to rename this to dsound.dll eventually, so
    # might as well do it right away since I couldn't figure
    # out how to do this during the install step.
    PROPERTIES OUTPUT_NAME dsound
)

install(
    TARGETS     cheat-console
    DESTINATION /
)

add_custom_command(
        OUTPUT KernelThunk.cpp KernelThunk.hpp
        COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/thunkgen.py ${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS Kernel.hpp thunkgen.py
        COMMENT "Regenerating kernel thunks"
)

add_executable(nightbeliever.krnl
        bootstrap.s
        C.cpp
        entry.cpp
        Hypercall.cpp
        Kernel.cpp
        KernelThunk.cpp
        liballoc.cpp
        liballoc_nightbeliever.cpp
        Log.cpp
        mini-printf.cpp
        TIB.cpp
)
enable_language(ASM-ATT)
set_target_properties(nightbeliever.krnl PROPERTIES COMPILE_FLAGS
        "-std=c++11 -ffreestanding -c -nostdlib -O2 -Wall -Wextra -fno-exceptions -fno-rtti -m32 -target i686-apple-darwin-macho -Wno-unused-parameter -Wno-implicit-exception-spec-mismatch -Wno-self-assign"
)
set_target_properties(nightbeliever.krnl PROPERTIES LINK_FLAGS
        "-ffreestanding -nostdlib -Wl,-image_base,0xC0000000 -m32 -target i686-apple-darwin-macho"
)

add_custom_command(TARGET nightbeliever.krnl POST_BUILD COMMAND cp nightbeliever.krnl ../nightbeliever.krnl)

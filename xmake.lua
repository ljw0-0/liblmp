target("lmp")
    set_kind("$(kind)")
    add_files("limb/*.c", "lmpn/*.c", "lmpz/**.c")
    
    set_languages("c99")
    
    add_includedirs(".", "limb", "lmpn")
    
    add_headerfiles("lmp.h")
    
    if not is_plat("windows") then
        add_links("m")
    end

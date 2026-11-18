target("lmp")
    set_kind("$(kind)")
    add_files("limb/*.c", "lmpn/*.c")
    
    -- 1. 指定 C 标准（对应你之前要求的 C99）
    set_languages("c99")
    
    -- 2. 头文件搜索路径
    -- 公共路径：让使用者能找到 mpn.h（根目录）
    -- 私有路径：让 limb/ 和 mpn/ 下的 .c 能找到各自的 impl.h
    add_includedirs(".", {public = true})
    add_includedirs("limb", "lmpn")  -- 私有，默认不公开
    
    -- 3. 安装公共头文件（mpn.h）到 include 目录
    add_headerfiles("lmp.h")
    
    -- 4. 链接数学库（非 Windows 平台）
    if not is_plat("windows") then
        add_links("m")
    end

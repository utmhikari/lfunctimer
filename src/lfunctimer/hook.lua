local hook = {}

--- see https://www.lua.org/pil/23.3.html
local function getfuncname(nameinfo)
    if nameinfo.what == "C" then
        return nameinfo.name
    end
    local loc = string.format("[%s]:%s", nameinfo.short_src, nameinfo.linedefined)
    if nameinfo.namewhat ~= "" then
        return string.format("%s (%s)", loc, nameinfo.name)
    else
        return string.format("%s", loc)
    end
end

function hook.new(lfunctimer)
    --- &func: func name
    local funcnames = {}
    --- &func: { timestamps }
    local functimestack = {}
    --- ignored functions
    local ignoredfuncs = {}
    return function (evt, line_nr, lvl)
        lvl = lvl or 2
        local f = debug.getinfo(lvl, "f").func
        if ignoredfuncs[f] then
            return
        end
        if not funcnames[f] then
            local nameinfo = debug.getinfo(2, "Sn")
            local funcname = getfuncname(nameinfo)
            if not funcname then
                table.insert(ignoredfuncs, f)
                return
            end
            funcnames[f] = funcname
        end
        if evt == "call" then
            if not functimestack[f] then
                functimestack[f] = {}
            end
            table.insert(functimestack[f], os.clock())
        elseif evt == "return" then
            --- maybe add hook after function call
            if functimestack[f] then
                if #functimestack[f] == 1 then
                    local exec_time = os.clock() - functimestack[f][1]
                    lfunctimer.save(funcnames[f], exec_time)
                end
                table.remove(functimestack[f])
            end
        end
    end
end

return hook
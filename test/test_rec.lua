package.path = package.path .. ";E:/Projects/Lua/lfunctimer"
local lfunctimer = require "lfunctimer"
lfunctimer.start()

local function rec(i)
    if i <= 1 then
        return 1
    else
        return rec(i - 1) + i
    end
end

rec(100000)

lfunctimer.stop()
lfunctimer.print()
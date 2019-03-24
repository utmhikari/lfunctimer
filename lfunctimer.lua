--[[
    lfunctimer: lua function execution timer
    for further development, take reference to luacov for a better hook~
]]
local debug = require "debug"
local hook = require "lib.hook"


local lfunctimer = {}

lfunctimer.data = {}

function lfunctimer.print()
    for func_name, exec_time in pairs(lfunctimer.data) do
        print(func_name .. " ------ " .. tostring(exec_time))
    end
end

function lfunctimer.save(func_name, exec_time)
    if not lfunctimer.data[func_name] then
        lfunctimer.data[func_name] = 0
    end
    lfunctimer.data[func_name] = lfunctimer.data[func_name] + exec_time
end

function lfunctimer.start()
    debug.sethook(hook.new(lfunctimer), "cr")
end

function lfunctimer.stop()
    debug.sethook()
end

function lfunctimer.reset()
    lfunctimer.data = {}
end

return lfunctimer

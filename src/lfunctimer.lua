--[[
    lfunctimer: lua function execution timer
    for further development, take reference to luacov for a better hook~
]]
local debug = require "debug"
local hook = require "lfunctimer.hook"
local util = require "lfunctimer.util"
local config = require "lfunctimer.config"

-- hookmask will always be "cr", not clash royale
local LFUNCTIMER_HOOKMASK = "cr"

-- TODO: add filters
local lfunctimer = {}

-- TODO: add configuration
lfunctimer.config = config.default
lfunctimer.data = {}

-- log message
function lfunctimer.log(msg)
    util.log(msg)
end

-- TODO: add custom print function
function lfunctimer.print_data()
    util.print_data(lfunctimer.data)
end

-- reset data
function lfunctimer.reset()
    lfunctimer.data = {}
end

-- TODO: add custom save function
function lfunctimer.save(funcname, exectime)
    if not lfunctimer.data[funcname] then
        lfunctimer.data[funcname] = 0
    end
    lfunctimer.data[funcname] = lfunctimer.data[funcname] + exectime
end

-- set config
function lfunctimer.set_config(config)
    for k, v in pairs(config) do 
        lfunctimer.config[k] = v
    end
end

-- start lfunctimer hook
function lfunctimer.start()
    local debug_hook = hook.new(lfunctimer)  -- lazy loading
    debug.sethook(debug_hook, LFUNCTIMER_HOOKMASK)
end

-- stop lfunctimer hook
function lfunctimer.stop()
    debug.sethook()
end

return lfunctimer

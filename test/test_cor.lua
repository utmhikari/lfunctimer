local lfunctimer = require "lfunctimer"
lfunctimer.start()

local coroutine = require "coroutine"

local function c1(x)
    local a = 10 + x
    coroutine.yield(a)
    x = x + 10
    coroutine.yield(x)
    return x + 999
end

local function c2(y)
    local b = y + 5
    coroutine.yield(b)
    return y + 99
end

co1 = coroutine.create(c1)
co2 = coroutine.create(c2)

local _, r1 = coroutine.resume(co1, 1)
print("C1 first: " .. tostring(r1))
local _, r2 = coroutine.resume(co2, r1)
print("C2 first: " .. tostring(r2))
local _, r3 = coroutine.resume(co1, r2)
print("C1 second: " .. tostring(r3))
local _, r4 = coroutine.resume(co2, r3)
print("C2 second: " .. tostring(r4))
local _, r5 = coroutine.resume(co1, r5)
print("C1 third: " .. tostring(r5))

lfunctimer.stop()
lfunctimer.print()
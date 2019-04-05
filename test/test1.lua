local lfunctimer = require "lfunctimer"
lfunctimer.start()

local function test1(x)
    a = x % 10
    for i = 1, 100000 do
        if a % 2 == 0 then
            a = a + 1
        else
            a = a + 3
        end
    end
    return a
end

function test2(s)
    local t = {}
    for i = 1, 100000 do
        table.insert(t, s)
    end
    return table.concat(t)
end

test1(999)
test2("a")

lfunctimer.stop()
lfunctimer.print()



# lfunctimer

a hook module for gathering statistics on executing time of functions

## Installation

`luarocks install --server=http://luarocks.org/manifests/utmhikari lfunctimer`

## examples

examples are in the tests lol~

```lua
local lfunctimer = require "lfunctimer"
lfunctimer.set_config({ key = "value" })  -- set config before start hook
lfunctimer.start()  -- set hook on "call"s and "return"s
-- do your stuff
lfunctimer.stop()  -- set hook to noop
lfunctimer.print_data()  -- print result of function timer
lfunctimer.reset()  -- reset lfunctimer.data
```

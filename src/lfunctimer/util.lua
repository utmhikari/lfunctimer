local util = {}

function util.log(msg)
    print('[LFUNCTIMER] ' .. tostring(msg))
end

function util.print_data(data)
    for funcname, exectime in pairs(data) do
        print(funcname .. " ------ " .. tostring(exectime))
    end
end

return util
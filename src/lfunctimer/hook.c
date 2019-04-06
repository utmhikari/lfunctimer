#include "stddef.h"
#include "lua.h"
#include "lauxlib.h"
#include "time.h"
#include "string.h"

#ifndef LFUNCTIMER_STKLVL
#define LFUNCTIMER_STKLVL 1
#endif

#ifndef LFUNCTIMER_UPVAL_IDX
#define LFUNCTIMER_UPVAL_IDX 1
#endif

#ifndef LFUNCTIMER_FUNCMAP_UPVAL_IDX
#define LFUNCTIMER_FUNCMAP_UPVAL_IDX 2
#endif

#ifndef LFUNCTIMER_TIMESTACK_UPVAL_IDX
#define LFUNCTIMER_TIMESTACK_UPVAL_IDX 3
#endif

#ifndef LFUNCTIMER_CFG_UPVAL_IDX
#define LFUNCTIMER_CFG_UPVAL_IDX 4
#endif

// get os_clock without pushnumber
lua_Number lfunctimer_os_clock() {
    return (lua_Number)clock()/(lua_Number)CLOCKS_PER_SEC;
}

// push function name to top of stack
// not the same as builtin "getfuncname" method
static int lfunctimer_getfuncname(lua_State *L) {
    lua_Debug ar;
    if (!lua_getstack(L,  LFUNCTIMER_STKLVL, &ar)) {
        return 0;
    }
    // check if function name already exists
    lua_getinfo(L, "f", &ar);
    lua_pushvalue(L, -1);
    lua_gettable(L, lua_upvalueindex(LFUNCTIMER_FUNCMAP_UPVAL_IDX));
    if (lua_toboolean(L, -1)) {
        lua_remove(L, 1);  // remove funcinfo
        return 1;
    }
    lua_pop(L, 1);
    // check if it's a builtin function
    lua_getinfo(L, "Sn", &ar);
    const char *what = ar.what;
    if (strcmp(what, "C") == 0) {
        lua_pushfstring(L, "<Builtin> %s", ar.name);
    } else if (ar.namewhat[0] == '\0') {
        lua_pushfstring(L, "<%s:%d> ::UNKNOWN::", ar.short_src, ar.linedefined); 
    } else {
        lua_pushfstring(L, "<%s:%d> %s", ar.short_src, ar.linedefined, ar.name);
    }
    // save function name to function map
    lua_pushvalue(L, 1);
    lua_pushstring(L, lua_tostring(L, -2));
    lua_settable(L, lua_upvalueindex(LFUNCTIMER_FUNCMAP_UPVAL_IDX));
    // remove funcinfo
    lua_remove(L, 1);
    return 1;
}

// handle "call" event
static int handle_call(lua_State *L) {
    lua_getfield(L, lua_upvalueindex(LFUNCTIMER_TIMESTACK_UPVAL_IDX), lua_tostring(L, 1));
    if (!lua_toboolean(L, -1)) {
        // if does not exist, create time stack and gettable again
        lua_pop(L, 1);
        lua_newtable(L);
        lua_setfield(L, lua_upvalueindex(LFUNCTIMER_TIMESTACK_UPVAL_IDX), lua_tostring(L, 1));
        // get table on funcname at bottom of stack~
        lua_gettable(L, lua_upvalueindex(LFUNCTIMER_TIMESTACK_UPVAL_IDX));
    }
    // push clock number to time stack
    int nextIdx = (int)lua_rawlen(L, -1) + 1;
    lua_pushnumber(L, lfunctimer_os_clock());
    lua_rawseti(L, -2, nextIdx);
    return 1;
}

// handle "return" event
static int handle_return(lua_State *L) {
    lua_getfield(L, lua_upvalueindex(LFUNCTIMER_TIMESTACK_UPVAL_IDX), lua_tostring(L, 1));
    if (!lua_toboolean(L, -1)) {
        return 0;  // ignore unrecorded functions this time
    }
    int size = (int)lua_rawlen(L, -1);
    if (size == 1) {
        // call lfunctimer.save(funcname, exectime) when stack size equals 1
        lua_getfield(L, lua_upvalueindex(LFUNCTIMER_UPVAL_IDX), "save");
        lua_pushvalue(L, 1);
        lua_rawgeti(L, -3, size);
        lua_call(L, 2, 0);
    }
    // pop a number at time stack
    lua_pushnil(L);
    lua_rawseti(L, -2, size);
    return 1;
}

// debug hook of lfunctimer
static int lfunctimer_debug_hook(lua_State *L) {
    const char *evt = lua_tostring(L, 1);
    lua_settop(L, 0);
    // get function name
    if (lfunctimer_getfuncname(L) == 0) {
        return 0;
    }
    // print log
    lua_getfield(L, lua_upvalueindex(LFUNCTIMER_CFG_UPVAL_IDX), "verbose");
    if (lua_toboolean(L, -1)) {
        lua_getfield(L, lua_upvalueindex(LFUNCTIMER_UPVAL_IDX), "log");
        lua_pushfstring(L, "%s: %s", lua_tostring(L, 1), evt);
        lua_call(L, 1, 0);
    }
    lua_pop(L, 1);
    // dispatch events
    if (evt[0] == 'c') {
        return handle_call(L);
    } else if (evt[0] == 'r') {
        return handle_return(L);
    }
    return 0;
}

// hook.new(lfunctimer)
int lfunctimer_new_hook(lua_State *L) {
    // upval1: lfunctimer
    lua_settop(L, 1);
    // upval2: funcname table --- { func: funcname }
    lua_newtable(L);
    // upval3: TIMESTACK table --- { funcname: { ...timestack } }
    lua_newtable(L);
    // upval4: lfunctimer.config
    lua_getfield(L, 1, "config");
    // link upvalues with debug hook
    lua_pushcclosure(L, lfunctimer_debug_hook, 4);
    return 1;
}

// local hook = require "lfunctimer.hook"
int luaopen_lfunctimer_hook(lua_State *L) {
    lua_newtable(L);
    lua_pushcfunction(L, lfunctimer_new_hook);
    lua_setfield(L, -2, "new");
    return 1;
}
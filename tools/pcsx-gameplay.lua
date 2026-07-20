-- Deep-runtime PCSX-Redux probe used by tools/pcsx_gameplay.py.
--
-- Boots the full disc chain unattended and requires genuine stage
-- simulation on the selected executable: stage construction (CreateStage),
-- the per-frame character activator running steadily (ActivateHumans, with
-- ActSTATE/ActNORMAL counted when they dispatch), and sustained CD/XA
-- streaming callbacks (cbCheckCD).  All breakpoints are
-- image-word-verified, so SLPS/MENU code executing the same addresses
-- before MAIN.EXE loads never counts.
--
-- PCSX-Redux Lua lessons baked in: every addBreakpoint/createEventListener
-- return value must stay referenced (a collected wrapper tears down its
-- native side mid-run), and breakpoint callbacks must not throw (PCSX
-- deletes a throwing breakpoint, silently weakening the probe) — hence the
-- pcall guards.

local ffi = require("ffi")

local function requiredNumber(name)
    local value = os.getenv(name)
    local number = value and tonumber(value)
    if number == nil then
        printError("TENCHU_GAMEPLAY FAIL missing numeric environment variable " .. name)
        PCSX.quit(2)
        error("missing " .. name)
    end
    return number
end

local function optionalNumber(name)
    local value = os.getenv(name)
    return value and tonumber(value) or nil
end

local addresses = {
    main = requiredNumber("TENCHU_GAMEPLAY_MAIN"),
    loop = requiredNumber("TENCHU_GAMEPLAY_LOOP"),
    createstage = requiredNumber("TENCHU_GAMEPLAY_CREATESTAGE"),
    activate = requiredNumber("TENCHU_GAMEPLAY_ACTIVATE"),
    actstate = requiredNumber("TENCHU_GAMEPLAY_ACTSTATE"),
    actnormal = requiredNumber("TENCHU_GAMEPLAY_ACTNORMAL"),
    cbcheck = requiredNumber("TENCHU_GAMEPLAY_CBCHECK"),
}
local words = {
    main = requiredNumber("TENCHU_GAMEPLAY_MAIN_W"),
    loop = requiredNumber("TENCHU_GAMEPLAY_LOOP_W"),
    createstage = requiredNumber("TENCHU_GAMEPLAY_CREATESTAGE_W"),
    activate = requiredNumber("TENCHU_GAMEPLAY_ACTIVATE_W"),
    actstate = requiredNumber("TENCHU_GAMEPLAY_ACTSTATE_W"),
    actnormal = requiredNumber("TENCHU_GAMEPLAY_ACTNORMAL_W"),
    cbcheck = requiredNumber("TENCHU_GAMEPLAY_CBCHECK_W"),
}
local requiredActs = requiredNumber("TENCHU_GAMEPLAY_ACT_MIN")
local requiredCallbacks = requiredNumber("TENCHU_GAMEPLAY_CB_MIN")
local maxVsyncs = optionalNumber("TENCHU_GAMEPLAY_MAX_VSYNCS") or 60000

local function readU32(address)
    local base = ffi.cast("uint8_t*", PCSX.getMemPtr())
    return tonumber(ffi.cast("uint32_t*", base + bit.band(address, 0x1fffff))[0])
end

TenchuGameplay = {
    main = 0, loop = 0, createstage = 0, activate = 0, actstate = 0, actnormal = 0,
    cbcheck = 0, vsyncs = 0, failed = false, passed = false,
    breakpoints = {}, callbackErrors = {},
}
local GP = TenchuGameplay

local function fail(message)
    if GP.failed or GP.passed then return end
    GP.failed = true
    local regs = PCSX.getRegisters()
    printError(string.format(
        "TENCHU_GAMEPLAY FAIL %s pc=0x%08x create=%d act=%d/%d/%d cb=%d vsyncs=%d",
        message, tonumber(regs.pc), GP.createstage, GP.activate,
        GP.actstate, GP.actnormal, GP.cbcheck, GP.vsyncs))
    PCSX.quit(1)
end

local function counter(key)
    GP.breakpoints[key] = PCSX.addBreakpoint(
        addresses[key], "Exec", 4, "", function()
            local ok, err = pcall(function()
                if GP.main == 0 and key ~= "main" then return end
                if readU32(addresses[key]) ~= words[key] then return end
                GP[key] = GP[key] + 1
            end)
            if not ok and not GP.callbackErrors[key] then
                GP.callbackErrors[key] = true
                printError(string.format(
                    "TENCHU_GAMEPLAY callback error at %s: %s", key,
                    tostring(err)))
            end
            return true
        end, "Tenchu gameplay " .. key)
end
for key in pairs(addresses) do counter(key) end

local pad = PCSX.SIO0.slots[1].pads[1]
local buttons = PCSX.CONSTS.PAD.BUTTON
local pulseButtons = {buttons.START, buttons.CIRCLE, buttons.CROSS}
local function releasePad()
    for _, button in ipairs(pulseButtons) do pad.clearOverride(button) end
end

GP.pauseListener = PCSX.Events.createEventListener(
    "ExecutionFlow::Pause", function(event)
        if event.exception then
            fail("first-chance CPU exception")
        elseif not GP.failed and not GP.passed then
            fail("emulator paused unexpectedly")
        end
    end)

GP.vsyncListener = PCSX.Events.createEventListener("GPU::Vsync", function()
    GP.vsyncs = GP.vsyncs + 1

    -- Input policy: pulse START/CIRCLE (CROSS cancels on the JP disc)
    -- through the launcher, menu, and title; after the first CreateStage
    -- (the title backdrop), keep confirming at a slow cadence until the
    -- simulation activates humans; then leave the pad alone so the stage
    -- runs undisturbed.
    if GP.activate > 0 or GP.actstate > 0 or GP.actnormal > 0 then
        releasePad()
    elseif GP.createstage > 0 then
        -- Title/menu screens are up (the title backdrop is itself a
        -- CreateStage product); keep confirming until the simulation
        -- activates humans.  CROSS is cancel on the JP disc.
        local phase = GP.vsyncs % 300
        if phase == 1 then pad.setOverride(buttons.START)
        elseif phase == 8 then pad.clearOverride(buttons.START)
        elseif phase == 150 then pad.setOverride(buttons.CIRCLE)
        elseif phase == 157 then pad.clearOverride(buttons.CIRCLE)
        end
    else
        local phase = GP.vsyncs % 90
        if phase == 1 then pad.setOverride(buttons.START)
        elseif phase == 8 then pad.clearOverride(buttons.START)
        elseif phase == 45 then pad.setOverride(buttons.CIRCLE)
        elseif phase == 52 then pad.clearOverride(buttons.CIRCLE)
        end
    end

    if GP.vsyncs % 1800 == 0 then
        print(string.format(
            "TENCHU_GAMEPLAY tick vsyncs=%d main=%d loop=%d create=%d act=%d/%d/%d cb=%d",
            GP.vsyncs, GP.main, GP.loop, GP.createstage, GP.activate,
            GP.actstate, GP.actnormal, GP.cbcheck))
    end

    if not GP.passed and not GP.failed and
       GP.createstage > 0 and
       (GP.activate + GP.actstate + GP.actnormal) >= requiredActs and
       GP.cbcheck >= requiredCallbacks then
        GP.passed = true
        print(string.format(
            "TENCHU_GAMEPLAY PASS create=%d act=%d/%d/%d cb=%d loops=%d vsyncs=%d cycles=%s",
            GP.createstage, GP.activate, GP.actstate, GP.actnormal,
            GP.cbcheck, GP.loop, GP.vsyncs, tostring(PCSX.getCPUCycles())))
        PCSX.quit(0)
    elseif GP.vsyncs > maxVsyncs then
        fail("gameplay conditions unmet before the vsync budget")
    end
end)

print(string.format(
    "TENCHU_GAMEPLAY armed main=0x%08x create=0x%08x activate=0x%08x " ..
    "actstate=0x%08x actnormal=0x%08x cb=0x%08x actMin=%d cbMin=%d maxVsyncs=%d",
    addresses.main, addresses.createstage, addresses.activate,
    addresses.actstate, addresses.actnormal, addresses.cbcheck,
    requiredActs, requiredCallbacks, maxVsyncs))

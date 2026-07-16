#!/usr/bin/env python3
"""Process lifecycle helpers shared by the long-running matching tools.

A matching tool that drives builds or a stochastic search must not leave work
behind when its command frontend dies. Two independent failure modes bit this
project, both here so autorules and permute cannot drift apart:

* An interrupted driver whose children keep running. `subprocess.run` does not
  terminate its child when a Python signal handler raises during `wait()`, so
  autorules used to restore the baseline source while an orphaned Shake build
  carried on mutating the tree.
* A driver orphaned by its frontend. A `timeout`-wrapped or harness-killed shell
  can die while the Python driver survives; PR_SET_PDEATHSIG closes that.

Use `interruption_handlers()` to turn frontend termination into a catchable
`InterruptedError`, `arm_parent_death_signal()` once at startup, and `run_owned`
(or `terminate_process_group`) for every child that must die with the driver.
"""
import ctypes
import os
import signal
import subprocess
import sys
from contextlib import contextmanager

_PR_SET_PDEATHSIG = 1
LIBC = ctypes.CDLL(None, use_errno=True) if sys.platform.startswith("linux") else None


def set_parent_death_signal(sig):
    """Ask Linux to signal this process when its current parent exits."""
    if LIBC is None:
        return
    if LIBC.prctl(_PR_SET_PDEATHSIG, sig, 0, 0, 0) != 0:
        err = ctypes.get_errno()
        raise OSError(err, os.strerror(err))


def arm_parent_death_signal(tool="tool"):
    """Prevent a command runner's death from orphaning the driver.

    PR_SET_PDEATHSIG has a small setup race. Checking getppid again closes it:
    if the original parent disappeared between the two calls, abort before the
    driver can start any work.
    """
    if LIBC is None:
        return
    parent = os.getppid()
    set_parent_death_signal(signal.SIGTERM)
    if os.getppid() != parent:
        raise InterruptedError(f"{tool} parent exited during startup")


@contextmanager
def interruption_handlers(tool="tool"):
    """Convert frontend termination into a catchable driver interruption."""
    def interrupted(signum, _frame):
        raise InterruptedError(f"{tool} interrupted by signal {signum}")

    old_handlers = {}
    for sig in (signal.SIGTERM, getattr(signal, "SIGHUP", None)):
        if sig is not None:
            old_handlers[sig] = signal.signal(sig, interrupted)
    try:
        yield
    finally:
        for sig, handler in old_handlers.items():
            signal.signal(sig, handler)


def child_parent_death_setup(expected_parent):
    """Popen preexec hook: do not leave a child alive if the driver is killed."""
    if LIBC is None:
        return
    # SIGKILL is intentional here. The child has not exec'd yet, so running a
    # Python signal handler in the post-fork/pre-exec window would be unsafe.
    set_parent_death_signal(signal.SIGKILL)
    if os.getppid() != expected_parent:
        os._exit(128 + signal.SIGKILL)


def terminate_process_group(proc):
    """Terminate and reap a subprocess plus every descendant in its session."""
    try:
        os.killpg(proc.pid, signal.SIGTERM)
    except ProcessLookupError:
        pass
    try:
        proc.wait(timeout=2)
    except subprocess.TimeoutExpired:
        pass
    # The group can outlive its leader: a shell/Shake process may exit on TERM
    # before one of its compiler children does. Probe by sending KILL even when
    # wait() already reaped the immediate child; ESRCH means the group is gone.
    try:
        os.killpg(proc.pid, signal.SIGKILL)
    except ProcessLookupError:
        pass
    if proc.poll() is None:
        proc.wait()


def owned_popen(args, **kwargs):
    """Popen in its own session, with the child dying if the driver is killed."""
    parent = os.getpid()
    child_setup = ((lambda: child_parent_death_setup(parent))
                   if LIBC is not None else None)
    return subprocess.Popen(args, start_new_session=True,
                            preexec_fn=child_setup, **kwargs)


def run_owned(args, **kwargs):
    """subprocess.run equivalent whose process group cannot escape an abort."""
    proc = owned_popen(args, **kwargs)
    try:
        stdout, stderr = proc.communicate()
    except BaseException:
        terminate_process_group(proc)
        raise
    return subprocess.CompletedProcess(args, proc.returncode, stdout, stderr)

# Syscall Hooking with .so and .ko Modules

This project demonstrates syscall hooking on Linux using both:
- User-space `.so` shared objects (e.g., via `LD_PRELOAD`)
- Kernel-space `.ko` kernel modules (e.g., via `insmod`)

## ⚠️ Disclaimer

⚠️ This project is for educational purposes only. Syscall hooking can be dangerous, may cause system instability, and could violate terms of use or laws in some jurisdictions. Use responsibly.

## 🧩 Features

- Hook specific syscalls from user space via dynamic linker (`LD_PRELOAD`)
- Hook kernel syscalls via loadable kernel modules (`.ko`)
- Logging of syscall arguments and return values
- Example hook for `open`, `openat`

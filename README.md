# mruby-bin-stat

`mrb_open_allocf()` した直後と、続けて `mrb_full_gc()` した直後のメモリ使用量を出力するだけのプログラムです。

```text
memory stat - after mrb_open_allocf()
	count of malloc:        963 times (realloc: 15 times)
	count of free:          155 times
	allocated memory:    177191 bytes
	realloc delta:         5257 bytes (absolute total: 5257 bytes)
	allocated chunks (grouped by log2):
	     1 B   |      0      0     28     83
	    16 B   |    120     78    284    169
	   256 B   |     90      1     82      6
	     4 KiB |      4      1      0      0
	    64 KiB |      1      0      0      0
	     1 MiB |      0      0      0      0
	    16 MiB |      0      0      0      0
	   256 MiB |      0      0      0      0
	runtime symbols:        377 symbols
	symbol capacity:        426 symbols (+1) (sizeof mrb->symtbl: 6832 bytes)
memory stat - after mrb_full_gc()
	count of malloc:        963 times (realloc: 15 times)
	count of free:          170 times
	allocated memory:    176718 bytes
	realloc delta:         5257 bytes (absolute total: 5257 bytes)
	allocated chunks (grouped by log2):
	     1 B   |      0      0     28     83
	    16 B   |    113     78    276    169
	   256 B   |     90      1     82      6
	     4 KiB |      4      1      0      0
	    64 KiB |      1      0      0      0
	     1 MiB |      0      0      0      0
	    16 MiB |      0      0      0      0
	   256 MiB |      0      0      0      0
	runtime symbols:        377 symbols
	symbol capacity:        426 symbols (+1) (sizeof mrb->symtbl: 6832 bytes)
```

## Specification

  * Package name: mruby-bin-stat
  * Author: [dearblue](https://github.com/dearblue)
  * Project page: <https://github.com/dearblue/mruby-bin-stat>
  * Licensing: [Creative Commons Zero License (CC0 / Public Domain)](LICENSE)
  * Dependency external mrbgems: (NONE)
  * Bundled C libraries (git-submodules): (NONE)
